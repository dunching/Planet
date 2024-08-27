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
#include "Components/SphereComponent.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


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
		}
		else
		{
			IceGunPtr->Activate();
			IceGunPtr->SetActorLocation(Location);
		}
		ExcuteTasks();
		
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
		IceGunPtr->Reset();
		TaskPtr->EndTask();
	}

}

ASkill_IceGun_Projectile::ASkill_IceGun_Projectile(const FObjectInitializer& ObjectInitializer):
Super(ObjectInitializer)
{

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	CapsuleComponentPtr = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	auto MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	
	UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
	// 设置立方体网格资源给 MeshComp
	if (CubeMesh != nullptr)
	{
		MeshComp->SetStaticMesh(CubeMesh);
		MeshComp->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
		MeshComp->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
		MeshComp->SetRelativeScale3D(FVector(0.2f)); // 可以根据需要调整大小
	}
	
	CapsuleComponentPtr->InitCapsuleSize(20.0f, 20.0f);
	CapsuleComponentPtr->SetCollisionProfileName(UCollisionProfile::Pawn_ProfileName);
	CapsuleComponentPtr->CanCharacterStepUpOn = ECB_No;
	CapsuleComponentPtr->SetShouldUpdatePhysicsVolume(true);
	CapsuleComponentPtr->SetCanEverAffectNavigation(false);
	CapsuleComponentPtr->bDynamicObstacle = true;
	CapsuleComponentPtr->SetupAttachment(RootComponent);
	MeshComp->SetupAttachment(RootComponent);
	
	this->InitialLifeSpan=0.f;
	this->ProjectileMovementCompPtr->InitialSpeed=100.f;
	this->ProjectileMovementCompPtr->MaxSpeed=100.f;
	this->ProjectileMovementCompPtr->ProjectileGravityScale=0.f;
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
			TMap<ECharacterPropertyType, FBaseProperty>ModifyPropertyMap;

			ModifyPropertyMap.Add(ECharacterPropertyType::GAPerformSpeed, -100);
			ModifyPropertyMap.Add(ECharacterPropertyType::MoveSpeed, -100);

			auto GameplayAbilityTargetDataPtr = new FGameplayAbilityTargetData_PropertyModify(
				SkillUnitPtr->GetUnitType(),
				SkillUnitPtr->GetIcon(),
				5,
				-1.f,
				1,
				ModifyPropertyMap
			);

			GameplayAbilityTargetDataPtr->TriggerCharacterPtr = CharacterPtr;
			GameplayAbilityTargetDataPtr->TargetCharacterPtr = OtherCharacterPtr;

			auto ICPtr = CharacterPtr->GetInteractiveBaseGAComponent();
			ICPtr->SendEventImp(GameplayAbilityTargetDataPtr);
		}
		else
		{
			// 冰冻
			auto GameplayAbilityTargetDataPtr = new FGameplayAbilityTargetData_StateModify(
				UGameplayTagsSubSystem::GetInstance()->Stun,
				2
			);

			GameplayAbilityTargetDataPtr->TriggerCharacterPtr = CharacterPtr;
			GameplayAbilityTargetDataPtr->TargetCharacterPtr = OtherCharacterPtr;

			auto ICPtr = CharacterPtr->GetInteractiveBaseGAComponent();
			ICPtr->SendEventImp(GameplayAbilityTargetDataPtr);
		}
		auto Mat=LoadObject<UMaterialInstance>(nullptr,TEXT("/Script/Engine.MaterialInstanceConstant'/Game/Megascans/Surfaces/Skated_Frozen_Lake_td0nfjlr/MI_Frozen.MI_Frozen'"));
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
			InstDy->SetScalarParameterValue(TEXT("Frozen"),-50.f+Count/3.f*230);
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
		return true;
	}
	else
		return false;
}



