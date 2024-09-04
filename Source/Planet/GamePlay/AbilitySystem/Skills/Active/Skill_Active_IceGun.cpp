#include "Skill_Active_IceGun.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/PrimitiveComponent.h"
#include "UObject/Class.h"
#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemComponent.h"

#include "CharacterBase.h"
#include "InteractiveSkillComponent.h"
#include "Tool_PickAxe.h"
#include "AbilityTask_PlayMontage.h"
#include "AbilityTask_TimerHelper.h"
#include "CS_PeriodicPropertyModify.h"
#include "CS_PeriodicStateModify.h"
#include "GameplayTagsSubSystem.h"
#include "InteractiveBaseGAComponent.h"
#include "ToolBuilderUtil.h"
#include "CS_PeriodicStateModify_Ice.h"


namespace Skill_IceGun_Notify
{
	const FName IceGunFire = TEXT("IceGunFire");
}

void USkill_Active_IceGun::PreActivate(const FGameplayAbilitySpecHandle Handle,
                                       const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                       FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate, const FGameplayEventData* TriggerEventData)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
	if (CharacterPtr)
	{
		const auto Dir = UKismetMathLibrary::MakeRotFromZX(-CharacterPtr->GetGravityDirection(), CharacterPtr->GetControlRotation().Vector());

		if (CharacterPtr->GetCharacterMovement()->CurrentFloor.IsWalkableFloor())
		{
			const auto Location = CharacterPtr->GetCharacterMovement()->CurrentFloor.HitResult.ImpactPoint;
		}
	}
}

void USkill_Active_IceGun::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	PlayMontage();
}

bool USkill_Active_IceGun::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void USkill_Active_IceGun::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USkill_Active_IceGun::PerformAction(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::PerformAction(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void USkill_Active_IceGun::PlayMontage()
{
	{
		const float InPlayRate = HumanMontage->CalculateSequenceLength() / 1.0f;
		auto TaskPtr = UAbilityTask_ASCPlayMontage::CreatePlayMontageAndWaitProxy(
			this,
			TEXT(""),
			HumanMontage,
			CharacterPtr->GetMesh()->GetAnimInstance(),
			InPlayRate
		);

		TaskPtr->Ability = this;
		TaskPtr->SetAbilitySystemComponent(CharacterPtr->GetAbilitySystemComponent());
		// TaskPtr->OnCompleted.BindUObject(this, &ThisClass::K2_CancelAbility);
		// TaskPtr->OnInterrupted.BindUObject(this, &ThisClass::K2_CancelAbility);
		TaskPtr->OnNotifyBegin.BindUObject(this, &ThisClass::OnNotifyBeginReceived);

		TaskPtr->ReadyForActivation();
	}
}

void USkill_Active_IceGun::OnOverlapCallback(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(ACharacterBase::StaticClass())&&OtherActor!=CharacterPtr)
	{
		OnOverlap(OtherActor);
		OverlappedComponent->GetOwner()->Destroy();
	}
}


void USkill_Active_IceGun::OnNotifyBeginReceived(FName NotifyName)
{
	if (NotifyName == Skill_IceGun_Notify::IceGunFire)
	{
		auto Location=CharacterPtr->GetMesh()->GetSocketLocation(TEXT("weapon_r"));
		for (int i=0;i<IceGunNum;i++)
		{
			FVector FireOffect=this->CharacterPtr->GetActorRightVector();
			FireOffect.X*=(i-IceGunNum/2)*100.f;
			FireOffect.Y*=(i-IceGunNum/2)*100.f;
			FireOffect.Z=0.f;
			auto IceGunPtr = GetWorld()->SpawnActor<ASkill_IceGun_Projectile>(
				IceGunPtrClass,
				Location+FireOffect,
				CharacterPtr->GetActorRotation());
		
			IceGunPtr->CapsuleComponentPtr->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnOverlapCallback);
			IceGunPtrAry.Add(IceGunPtr);
			const auto & Target=GetNearnestTarget(CharacterPtr,1000);
			if (Target)
			{
				IceGunPtr->ProjectileMovementComp->bIsHomingProjectile=true;
				IceGunPtr->ProjectileMovementComp->bRotationFollowsVelocity=true;
				IceGunPtr->ProjectileMovementComp->HomingAccelerationMagnitude=2000;
				IceGunPtr->ProjectileMovementComp->HomingTargetComponent=Target;
			}
			else
			{
				//否则就直线发射
				IceGunPtr->InitialLifeSpan=Duration;
			}
		}
		K2_CancelAbility();//动画播放完毕，可以立马再次释放
	}
}

void USkill_Active_IceGun::OnOverlap(AActor* OtherActor)
{
	if (OtherActor && OtherActor->IsA(ACharacterBase::StaticClass()))
	{
		auto OtherCharacterPtr = Cast<ACharacterBase>(OtherActor);
		if (CharacterPtr->IsGroupmate(OtherCharacterPtr))
		{
			return;
		}

		auto ICPtr = CharacterPtr->GetInteractiveBaseGAComponent();
		auto GameplayAbilityTargetDataPtr = new FGameplayAbilityTargetData_StateModify_Ice();
		GameplayAbilityTargetDataPtr->TargetCharacterPtr=OtherCharacterPtr;
		GameplayAbilityTargetDataPtr->TriggerCharacterPtr=CharacterPtr;
		ICPtr->SendEventImp(GameplayAbilityTargetDataPtr);
		

		auto CSPtr = OtherCharacterPtr->GetInteractiveBaseGAComponent()->GetCharacterState(GameplayAbilityTargetDataPtr->Tag);
		if ((CSPtr && CSPtr->GetStateDisplayInfo().Pin()->Num < 3) || !CSPtr)
		{
			if (HitSound.IsValid())
				UGameplayStatics::PlaySoundAtLocation(OtherActor,HitSound.LoadSynchronous(),OtherActor->GetActorLocation());	
			//if (HitParticle->IsValidLowLevel())  //草了，这个资源无法判断是否有效，北老师修一下
				UGameplayStatics::SpawnEmitterAtLocation(OtherActor,HitParticle.LoadSynchronous(),OtherActor->GetActorLocation());
		}
		else
		{
			// 冰冻
			//if (IceParticle->IsValidLowLevel())  //草了，这个资源无法判断是否有效，北老师修一下
				UGameplayStatics::SpawnEmitterAtLocation(OtherActor,IceParticle.LoadSynchronous(),OtherActor->GetActorLocation());
			if (IceSound.IsValid())
				UGameplayStatics::PlaySoundAtLocation(OtherActor,IceSound.LoadSynchronous(),OtherActor->GetActorLocation());	
		}
	}
}


USceneComponent* USkill_Active_IceGun::GetNearnestTarget(ACharacterBase* SelfCharacter,float SearchRadius)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(SelfCharacter,ACharacterBase::StaticClass(),FoundActors);

	if (FoundActors.IsEmpty())
		return nullptr; // 没有找到任何角色

	ACharacter* NearestCharacter = nullptr;
	float NearestDistanceSquared = MAX_FLT;

	for (AActor* Actor : FoundActors)
	{
		ACharacterBase* Character = Cast<ACharacterBase>(Actor);
		if (Character != nullptr&&Character!=SelfCharacter)
		{
			float DistanceSquared = (Character->GetActorLocation() - SelfCharacter->GetActorLocation()).Length();
			if (DistanceSquared < NearestDistanceSquared&&DistanceSquared<=SearchRadius)
			{
				NearestDistanceSquared = DistanceSquared;
				NearestCharacter = Character;
			}
		}
	}
	if (NearestCharacter)
		return NearestCharacter->GetRootComponent();
	return  nullptr;
}


ASkill_IceGun_Projectile::ASkill_IceGun_Projectile(const FObjectInitializer& ObjectInitializer):
Super(ObjectInitializer)
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	RootComponent->SetWorldScale3D(FVector(1.0f));
	CapsuleComponentPtr = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	ProjectileMovementComp=CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ParticleSystemComp=CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystemComp"));
	
	UParticleSystem* ParticleSystem=LoadObject<UParticleSystem>(nullptr,TEXT("/Script/Engine.ParticleSystem'/Game/InfinityBladeEffects/Effects/FX_Monsters/FX_Monster_Elemental/ICE/P_LazerIceAttack.P_LazerIceAttack'"));
	ParticleSystemComp->SetTemplate(ParticleSystem);
	//ParticleSystemComp->SetRelativeRotation(FRotator(0.f,60.0f,0.f));

	CapsuleComponentPtr->InitCapsuleSize(34.0f, 34.0f);
	//CapsuleComponentPtr->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	this->SetActorEnableCollision(ECollisionEnabled::QueryOnly);
	CapsuleComponentPtr->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CapsuleComponentPtr->CanCharacterStepUpOn = ECB_No;
	CapsuleComponentPtr->SetShouldUpdatePhysicsVolume(true);
	CapsuleComponentPtr->SetCanEverAffectNavigation(false);
	CapsuleComponentPtr->bDynamicObstacle = true;
	this->InitialLifeSpan=10.f;
	this->ProjectileMovementComp->InitialSpeed=300.f;
	this->ProjectileMovementComp->MaxSpeed=300.f;
	this->ProjectileMovementComp->ProjectileGravityScale=0.f;
	CapsuleComponentPtr->SetupAttachment(RootComponent);
	ParticleSystemComp->SetupAttachment(RootComponent);
}



