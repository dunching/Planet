#include "Skill_Active_IceGun.h"

#include "AbilitySystemBlueprintLibrary.h"

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
#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Particles/ParticleSystemComponent.h"


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
			ASkill_IceGun_Projectile::StaticClass(),
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
		// debuff
		auto CSPtr = OtherCharacterPtr->GetInteractiveBaseGAComponent()->GetCharacterState(SkillUnitPtr->GetUnitType());
		if ((CSPtr && CSPtr->GetStateDisplayInfo().Pin()->Num < 3) || !CSPtr)
		{
			static USoundBase* HitSound=LoadObject<USoundBase>(this,TEXT("/Script/Engine.SoundWave'/Game/UltraDynamicSky/Sound/Rain/RainHit_1.RainHit_1'"));
			static UParticleSystem* HitParticle=LoadObject<UParticleSystem>(this,
				TEXT("/Script/Engine.ParticleSystem'/Game/InfinityBladeEffects/Effects/FX_Monsters/FX_Monster_Elemental/ICE/P_CIN_Eye_Flare_01.P_CIN_Eye_Flare_01'"));
			if (HitSound)
				UGameplayStatics::PlaySoundAtLocation(OtherActor,HitSound,OtherActor->GetActorLocation());	
			if (HitParticle)
				UGameplayStatics::SpawnEmitterAtLocation(OtherActor,HitParticle,OtherActor->GetActorLocation());

			TMap<ECharacterPropertyType, FBaseProperty>ModifyPropertyMap;
			ModifyPropertyMap.Add(ECharacterPropertyType::GAPerformSpeed, -100);
			ModifyPropertyMap.Add(ECharacterPropertyType::MoveSpeed, -100);
			auto GameplayAbilityTargetDataPtr = new FGameplayAbilityTargetData_PropertyModify(
				SkillUnitPtr->GetUnitType(),
				SkillUnitPtr->GetIcon(),
				5,
				-1.f,
				-1.f,
				ModifyPropertyMap
			);

			GameplayAbilityTargetDataPtr->TriggerCharacterPtr = CharacterPtr;
			GameplayAbilityTargetDataPtr->TargetCharacterPtr = OtherCharacterPtr;

			auto ICPtr = CharacterPtr->GetInteractiveBaseGAComponent();
			ICPtr->SendEventImp(GameplayAbilityTargetDataPtr);
		}
		else
		{
			// 1.清空缓速
			auto GameplayAbilityTargetDataPtr = new FGameplayAbilityTargetData_PropertyModify(
				SkillUnitPtr->GetUnitType(),
				true
			);

			GameplayAbilityTargetDataPtr->TriggerCharacterPtr = CharacterPtr;
			GameplayAbilityTargetDataPtr->TargetCharacterPtr = OtherCharacterPtr;

			auto ICPtr = CharacterPtr->GetInteractiveBaseGAComponent();
			ICPtr->SendEventImp(GameplayAbilityTargetDataPtr);

			// 2.加眩晕

			// 3.设置冷却
			
			
			// // 冰冻
			// auto GameplayAbilityTargetDataPtr = new FGameplayAbilityTargetData_StateModify(
			// 	UGameplayTagsSubSystem::GetInstance()->Stun,
			// 	2
			// );
			//
			// GameplayAbilityTargetDataPtr->TriggerCharacterPtr = CharacterPtr;
			// GameplayAbilityTargetDataPtr->TargetCharacterPtr = OtherCharacterPtr;

			// auto ICPtr = CharacterPtr->GetInteractiveBaseGAComponent();
			// ICPtr->SendEventImp(GameplayAbilityTargetDataPtr);
			static UParticleSystem* BoomParticle=LoadObject<UParticleSystem>(this,TEXT("/Script/Engine.ParticleSystem'/Game/InfinityBladeEffects/Effects/FX_Monsters/FX_Monster_Elemental/ICE/P_IceElementalSplit_Small.P_IceElementalSplit_Small'"));
			static USoundBase* BoomSound=LoadObject<USoundBase>(this,TEXT("/Script/Engine.SoundWave'/Game/StarterContent/Audio/Explosion01.Explosion01'"));
			if (BoomParticle)
				UGameplayStatics::SpawnEmitterAtLocation(OtherActor,BoomParticle,OtherActor->GetActorLocation());
			if (BoomSound)
				UGameplayStatics::PlaySoundAtLocation(OtherActor,BoomSound,OtherActor->GetActorLocation());	
			OtherCharacterPtr->GetInteractiveBaseGAComponent()->ClearData2Self({},FGameplayTag::RequestGameplayTag(FName(TEXT("Unit.Skill.Active.IceGun"))));
		}
		auto MaterialsNum=OtherCharacterPtr->GetMesh()->GetMaterials().Num();
		auto Mesh=OtherCharacterPtr->GetMesh();
		float Count=1.0f;//这里要算百分比，不能用整数除
		if (CSPtr)
		{
			Count=CSPtr->GetStateDisplayInfo().Pin()->Num;
		}
		for (int i=0;i<MaterialsNum;i++)
		{
			auto InstDy = Cast<UMaterialInstanceDynamic>(Mesh->GetMaterial(i));
			if (!InstDy)
			{
				auto Inst = Cast<UMaterialInstance>(Mesh->GetMaterial(i));
				InstDy = UMaterialInstanceDynamic::Create(Inst, OtherCharacterPtr);
				Mesh->SetMaterial(i,InstDy);
			}
			static float MinValue=-50.f;
			static float MaxValue=230.f;
			static float MaxCount=3;
			InstDy->SetScalarParameterValue(TEXT("Frozen"),MinValue+Count/MaxCount*MaxValue);
		}
		GEngine->AddOnScreenDebugMessage(123,2.0f,FColor::Red,FString::FromInt(int(Count)));
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



