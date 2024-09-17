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
#include "UnitProxyProcessComponent.h"
#include "Tool_PickAxe.h"
#include "AbilityTask_PlayMontage.h"
#include "AbilityTask_TimerHelper.h"
#include "CS_PeriodicPropertyModify.h"
#include "CS_PeriodicStateModify.h"
#include "GameplayTagsSubSystem.h"
#include "BaseFeatureGAComponent.h"
#include "ToolBuilderUtil.h"
#include "StateProcessorComponent.h"
#include "CS_PeriodicStateModify_Ice.h"
#include "CharacterStateInfo.h"


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
		const float InPlayRate = HumanMontage->CalculateSequenceLength() / Duration;
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

void USkill_Active_IceGun::OnHitCallback(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor->IsA(ACharacterBase::StaticClass())&&OtherActor!=CharacterPtr)
	{
		OnOverlap(OtherActor);
		ResetIceGun(nullptr);
	}

}

void USkill_Active_IceGun::OnNotifyBeginReceived(FName NotifyName)
{
	if (NotifyName == Skill_IceGun_Notify::IceGunFire)
	{
		auto Location=CharacterPtr->GetMesh()->GetSocketLocation(TEXT("weapon_r"));
		const auto Dir = UKismetMathLibrary::MakeRotFromZX(-CharacterPtr->GetGravityDirection(), CharacterPtr->GetControlRotation().Vector());
		StartPt = Location + (Dir.Vector() * Offset);
		EndPt = Location + (Dir.Vector() * (Offset + Distance));
		if (!IceGunPtr)
		{
			IceGunPtr = GetWorld()->SpawnActor<ASkill_IceGun_Projectile>(
			IceGunPtrClass,
			Location,
			CharacterPtr->GetActorRotation());
			IceGunPtr->CapsuleComponentPtr->OnComponentHit.AddDynamic(this, &ThisClass::OnHitCallback);
		}
		else
		{
			IceGunPtr->SetActorLocation(Location);
			IceGunPtr->SetActorRotation(CharacterPtr->GetActorRotation());
			IceGunPtr->Activate();
		}

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
			ExcuteTasks();
		}
	}
}

void USkill_Active_IceGun::OnProjectileBounce(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (OtherActor && OtherActor->IsA(ACharacterBase::StaticClass()))
	{
		auto OtherCharacterPtr = Cast<ACharacterBase>(OtherActor);
		if (CharacterPtr->IsGroupmate(OtherCharacterPtr))
		{
			return;
		}

		//MakeDamage(OtherCharacterPtr);
	}

	OverlappedComponent->GetOwner()->Destroy();
}

void USkill_Active_IceGun::OnTimerHelperTick(UAbilityTask_TimerHelper* TaskPtr, float CurrentInterval, float Interval)
{

	if (CurrentInterval <= Interval)
	{
		const auto Percent = CurrentInterval / Interval;
		const auto NewOffset = EndPt - StartPt;
		const auto NewPt = StartPt + (Percent * NewOffset);
		IceGunPtr->SetActorLocation(NewPt);
		{
			ECollisionChannel TestChannel = ECC_MAX;
			FComponentQueryParams DefaultComponentQueryParams;
			DefaultComponentQueryParams.AddIgnoredActor(IceGunPtr);

			FCollisionObjectQueryParams DefaultObjectQueryParam;
			DefaultObjectQueryParam.AddObjectTypesToQuery(ECC_Pawn);

			TArray<FOverlapResult> OutOverlap;
			IceGunPtr->CapsuleComponentPtr->ComponentOverlapMulti(
				OutOverlap,
				GetWorld(),
				NewPt,
				CharacterPtr->GetActorRotation().Quaternion(),
				TestChannel,
				DefaultComponentQueryParams,
				DefaultObjectQueryParam
			);
			for (const auto& Iter : OutOverlap)
			{
				OnOverlap(Iter.GetActor());
			}
		}
	}
	else
	{
		ResetIceGun(TaskPtr);
		IceGunPtr->Reset();
		TaskPtr->EndTask();
	}

}

void USkill_Active_IceGun::ExcuteTasks()
{
	auto TaskPtr = UAbilityTask_TimerHelper::DelayTask(this);
	TaskPtr->SetDuration(Duration);
	TaskPtr->DurationDelegate.BindUObject(this, &ThisClass::OnTimerHelperTick);
	TaskPtr->OnFinished.BindUObject(this, &ThisClass::ResetIceGun);
	TaskPtr->ReadyForActivation();
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
		
		// debuff
		auto CSPtr = OtherCharacterPtr->GetStateProcessorComponent()->GetCharacterState(GameplayAbilityTargetDataPtr->Tag);
		if ((CSPtr && CSPtr->Num < 3) || !CSPtr)
		{
			if (HitSound.IsValid())
				UGameplayStatics::PlaySoundAtLocation(OtherActor,HitSound.LoadSynchronous(),OtherActor->GetActorLocation());	
			if (HitParticle.IsValid())
				UGameplayStatics::SpawnEmitterAtLocation(OtherActor,HitParticle.LoadSynchronous(),OtherActor->GetActorLocation());
		}
		else
		{
			// // 冰冻
			static UParticleSystem* BoomParticle=LoadObject<UParticleSystem>(this,TEXT("/Script/Engine.ParticleSystem'/Game/InfinityBladeEffects/Effects/FX_Monsters/FX_Monster_Elemental/ICE/P_IceElementalSplit_Small.P_IceElementalSplit_Small'"));
			static USoundBase* BoomSound=LoadObject<USoundBase>(this,TEXT("/Script/Engine.SoundWave'/Game/StarterContent/Audio/Explosion01.Explosion01'"));
			if (BoomParticle)
				UGameplayStatics::SpawnEmitterAtLocation(OtherActor,BoomParticle,OtherActor->GetActorLocation());
			if (BoomSound)
				UGameplayStatics::PlaySoundAtLocation(OtherActor,BoomSound,OtherActor->GetActorLocation());	
		}
		IceGunPtr->Reset();
	}
}

bool USkill_Active_IceGun::ResetIceGun(UAbilityTask_TimerHelper* TaskPtr)
{
	if (IceGunPtr)
	{
		IceGunPtr->Reset();
		K2_CancelAbility();
		if (TaskPtr)
			TaskPtr->EndTask();
		return true;
	}
	else
		return false;
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
	CapsuleComponentPtr->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	CapsuleComponentPtr->CanCharacterStepUpOn = ECB_No;
	CapsuleComponentPtr->SetShouldUpdatePhysicsVolume(true);
	CapsuleComponentPtr->SetCanEverAffectNavigation(false);
	CapsuleComponentPtr->bDynamicObstacle = true;
	this->InitialLifeSpan=0.f;
	this->ProjectileMovementComp->InitialSpeed=200.f;
	this->ProjectileMovementComp->MaxSpeed=200.f;
	this->ProjectileMovementComp->ProjectileGravityScale=0.f;
	CapsuleComponentPtr->SetupAttachment(RootComponent);
	ParticleSystemComp->SetupAttachment(RootComponent);
}

void ASkill_IceGun_Projectile::Reset()
{
	// 禁用碰撞
	CapsuleComponentPtr->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComponentPtr->SetCollisionResponseToAllChannels(ECR_Ignore);
	// 隐藏 Actor
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false); // 禁用Actor的Tick
	ProjectileMovementComp->SetActive(false);
	// ParticleSystemComp->SetVisibility(false);
	// GetWorld()->GetTimerManager().ClearTimer(TimerHandle);
}

void ASkill_IceGun_Projectile::Activate()
{
	CapsuleComponentPtr->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CapsuleComponentPtr->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block); 
	SetActorHiddenInGame(false);
	SetActorTickEnabled(true);
	ProjectileMovementComp->SetActive(true);
	//下一帧再把特效组件显示出来，免得有拖尾残留
	//TWeakObjectPtr<ASkill_IceGun_Projectile> WeakThis=this;

	// // 当设置定时器时，将句柄保存起来
	// GetWorld()->GetTimerManager().SetTimer(
	// 	TimerHandle,
	// 	FTimerDelegate::CreateLambda([this, WeakThis]()
	// 	{
	// 		if (WeakThis.IsValid())
	// 		{
	// 			ParticleSystemComp->SetVisibility(true);
	// 		}
	// 	}),
	// 	1.0f,
	// 	false
	// );
}



