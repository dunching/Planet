
#include "CS_RootMotion_Traction.h"

#include <Engine/AssetManager.h>
#include <Engine/StreamableManager.h>
#include "AbilitySystemGlobals.h"
#include <GameFramework/CharacterMovementComponent.h>
#include <Components/SphereComponent.h>
#include "Engine/OverlapResult.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

#include "KismetGravityLibrary.h"

#include "AbilityTask_TimerHelper.h"
#include "CharacterBase.h"
#include "ProxyProcessComponent.h"
#include "CharacterAttributesComponent.h"
#include "GenerateType.h"
#include "GAEvent_Send.h"
#include "EffectsList.h"
#include "UIManagerSubSystem.h"
#include "EffectItem.h"
#include "BaseFeatureComponent.h"
#include "GameplayTagsSubSystem.h"
#include "AbilityTask_MyApplyRootMotionConstantForce.h"
#include "AbilityTask_FlyAway.h"
#include "AbilityTask_ApplyRootMotionBySPline.h"
#include "SPlineActor.h"
#include "AbilityTask_Tornado.h"
#include "AbilityTask_MyApplyRootMotionRadialForce.h"
#include "StateProcessorComponent.h"

ATractionPoint::ATractionPoint(const FObjectInitializer& ObjectInitializer /*= FObjectInitializer::Get()*/)
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
// 
//  	SphereComponentPtr = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
//  	SphereComponentPtr->SetSphereRadius(10.f);
//  	SphereComponentPtr->SetCollisionEnabled(ECollisionEnabled::NoCollision);
//  	SphereComponentPtr->CanCharacterStepUpOn = ECB_No;
//  	SphereComponentPtr->SetShouldUpdatePhysicsVolume(false);
//  	SphereComponentPtr->SetCanEverAffectNavigation(false);
//  	SphereComponentPtr->bDynamicObstacle = true;
//  	SphereComponentPtr->SetupAttachment(RootComponent);

	SetRemoteRoleForBackwardsCompat(ROLE_SimulatedProxy);
	bReplicates = true;
	NetPriority = 3.0f;
	NetUpdateFrequency = 100.f;
	SetReplicatingMovement(true);

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.f;

	TractionPoint_Guid = FGuid::NewGuid();
}

void ATractionPoint::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ThisClass, Radius, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(ThisClass, Strength, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(ThisClass, bIsPush, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(ThisClass, bIsAdditive, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(ThisClass, bNoZForce, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(ThisClass, StrengthDistanceFalloff, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(ThisClass, StrengthOverTime, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(ThisClass, bUseFixedWorldDirection, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(ThisClass, FixedWorldDirection, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(ThisClass, VelocityOnFinishMode, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(ThisClass, SetVelocityOnFinish, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(ThisClass, ClampVelocityOnFinished, COND_InitialOnly);

	DOREPLIFETIME_CONDITION(ThisClass, TractionPoint_Guid, COND_InitialOnly);
}

void ATractionPoint::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

#if UE_EDITOR || UE_SERVER
	if (GetNetMode() == NM_DedicatedServer)
	{
		auto CharacterPtr = GetOwner<ACharacterBase>();

		FCollisionObjectQueryParams ObjectQueryParams;
		ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(GetOwner());

		TArray<FOverlapResult> Result;
		GetWorld()->OverlapMultiByObjectType(
			Result,
			GetActorLocation(),
			FQuat::Identity,
			ObjectQueryParams,
			FCollisionShape::MakeSphere(Radius),
			Params
		);

		TSet<ACharacterBase*>TargetSet;
		for (const auto& Iter : Result)
		{
			auto TargetCharacterPtr = Cast<ACharacterBase>(Iter.GetActor());
			if (TargetCharacterPtr && !CharacterPtr->IsGroupmate(TargetCharacterPtr))
			{
				TargetSet.Add(TargetCharacterPtr);
			}
		}

		auto ICPtr = CharacterPtr->GetBaseFeatureComponent();

		// 控制效果
		for (const auto& Iter : TargetSet)
		{
			if (FVector::Distance(Iter->GetActorLocation(), GetActorLocation()) < Radius)
			{
				auto GameplayAbilityTargetData_StateModifyPtr = new FGameplayAbilityTargetData_RootMotion_Traction;

				GameplayAbilityTargetData_StateModifyPtr->TriggerCharacterPtr = CharacterPtr;
				GameplayAbilityTargetData_StateModifyPtr->TargetCharacterPtr = Iter;

				GameplayAbilityTargetData_StateModifyPtr->TractionPointPtr = this;

				ICPtr->SendEventImp(GameplayAbilityTargetData_StateModifyPtr);
			}
		}
	}
#endif
}

FGameplayAbilityTargetData_RootMotion_Traction::FGameplayAbilityTargetData_RootMotion_Traction() :
	Super(UGameplayTagsSubSystem::GetInstance()->State_RootMotion_Traction)
{

}

UScriptStruct* FGameplayAbilityTargetData_RootMotion_Traction::GetScriptStruct() const
{
	return FGameplayAbilityTargetData_RootMotion_Traction::StaticStruct();
}

bool FGameplayAbilityTargetData_RootMotion_Traction::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Super::NetSerialize(Ar, Map, bOutSuccess);

	if (Ar.IsSaving())
	{
		TractionPoint_Guid = TractionPointPtr->TractionPoint_Guid;
		Ar << TractionPoint_Guid;
	}
	else if (Ar.IsLoading())
	{
		Ar << TractionPoint_Guid;
	}

	return true;
}

FGameplayAbilityTargetData_RootMotion_Traction* FGameplayAbilityTargetData_RootMotion_Traction::Clone() const
{
	auto ResultPtr =
		new FGameplayAbilityTargetData_RootMotion_Traction;

	*ResultPtr = *this;

	return ResultPtr;
}

void UCS_RootMotion_Traction::OnAvatarSet(
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilitySpec& Spec
)
{
	Super::OnAvatarSet(ActorInfo, Spec);

	if (Spec.GameplayEventData)
	{
		GameplayAbilityTargetDataSPtr =
			MakeSPtr_GameplayAbilityTargetData<FRootMotionParam>(Spec.GameplayEventData->TargetData.Get(0));
	}
}

void UCS_RootMotion_Traction::PreActivate(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
	const FGameplayEventData* TriggerEventData /*= nullptr */
)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
}

void UCS_RootMotion_Traction::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	PerformAction();
}

void UCS_RootMotion_Traction::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	//
	CharacterPtr->GetStateProcessorComponent()->RemoveStateDisplay(CharacterStateInfoSPtr);

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCS_RootMotion_Traction::PerformAction()
{
	if (CharacterPtr)
	{
		ExcuteTasks();
	}
}

void UCS_RootMotion_Traction::UpdateRootMotionImp(const TSharedPtr<FGameplayAbilityTargetData_RootMotion>& DataSPtr)
{
	Super::UpdateRootMotionImp(DataSPtr);

#if UE_EDITOR || UE_SERVER
	if (CharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		if (CharacterStateInfoSPtr)
		{
			CharacterStateInfoSPtr->Tag = GameplayAbilityTargetDataSPtr->Tag;
			CharacterStateInfoSPtr->DefaultIcon = GameplayAbilityTargetDataSPtr->DefaultIcon;
			CharacterStateInfoSPtr->RefreshTime();
			CharacterStateInfoSPtr->DataChanged();
		}
	}
#endif

	if (GameplayAbilityTargetDataSPtr && GameplayAbilityTargetDataSPtr->TractionPointPtr.IsValid())
	{
		OnTaskComplete();
	}
	else if (RootMotionTaskPtr)
	{
		RootMotionTaskPtr->UpdateLocation(GameplayAbilityTargetDataSPtr->TractionPointPtr);
	}
}

void UCS_RootMotion_Traction::ExcuteTasks()
{
#if UE_EDITOR || UE_SERVER
	if (CharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		// 
		CharacterStateInfoSPtr = MakeShared<FCharacterStateInfo>();
		CharacterStateInfoSPtr->Tag = GameplayAbilityTargetDataSPtr->Tag;
		CharacterStateInfoSPtr->DefaultIcon = GameplayAbilityTargetDataSPtr->DefaultIcon;
		CharacterStateInfoSPtr->DataChanged();
		CharacterPtr->GetStateProcessorComponent()->AddStateDisplay(CharacterStateInfoSPtr);

	}
#endif
	
#if UE_EDITOR || UE_SERVER
	if (CharacterPtr->GetLocalRole() == ENetRole::ROLE_AutonomousProxy)
	{
		TArray<AActor*> OutActors;
		UGameplayStatics::GetAllActorsOfClass(CharacterPtr, ATractionPoint::StaticClass(), OutActors);

		for (auto Iter : OutActors)
		{
			auto TractionPointPtr = Cast<ATractionPoint>(Iter);
			if (
				TractionPointPtr && 
				(TractionPointPtr->TractionPoint_Guid == GameplayAbilityTargetDataSPtr->TractionPoint_Guid)
				)
			{
				GameplayAbilityTargetDataSPtr->TractionPointPtr = TractionPointPtr;
				break;
			}
		}
	}
#endif

	// 
	RootMotionTaskPtr = UAbilityTask_MyApplyRootMotionRadialForce::MyApplyRootMotionRadialForce(
		this,
		TEXT(""),
		GameplayAbilityTargetDataSPtr->TractionPointPtr
	);

	RootMotionTaskPtr->OnFinish.BindUObject(this, &ThisClass::OnTaskComplete);
	RootMotionTaskPtr->ReadyForActivation();
}

void UCS_RootMotion_Traction::OnTaskComplete()
{
	K2_CancelAbility();
}
