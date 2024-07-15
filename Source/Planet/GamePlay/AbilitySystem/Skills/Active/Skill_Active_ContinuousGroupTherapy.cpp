
#include "Skill_Active_ContinuousGroupTherapy.h"

#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "NiagaraComponent.h"
#include "Components/PrimitiveComponent.h"
#include "UObject/Class.h"
#include "Components/SkeletalMeshComponent.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_Repeat.h"
#include "GameFramework/Controller.h"
#include <Engine/OverlapResult.h>

#include "GAEvent_Helper.h"
#include "CharacterBase.h"
#include "InteractiveSkillComponent.h"
#include "ToolFuture_Base.h"
#include "AbilityTask_PlayMontage.h"
#include "ToolFuture_PickAxe.h"
#include "Planet.h"
#include "CollisionDataStruct.h"
#include "CharacterAttributesComponent.h"
#include "AbilityTask_TimerHelper.h"
#include "Weapon_PickAxe.h"
#include "HumanControllerInterface.h"
#include "GroupMnaggerComponent.h"
#include "HumanCharacter.h"
#include "InteractiveBaseGAComponent.h"

namespace Skill_GroupTherapy
{
	const FName TriggerTherapy = TEXT("TriggerTherapy");

	const FName AttachEnd = TEXT("AttachEnd");
}

USkill_Active_ContinuousGroupTherapy::USkill_Active_ContinuousGroupTherapy() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	bRetriggerInstancedAbility = true;
}

void USkill_Active_ContinuousGroupTherapy::PreActivate(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
	const FGameplayEventData* TriggerEventData /*= nullptr */
)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
}

void USkill_Active_ContinuousGroupTherapy::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	CommitAbility(Handle, ActorInfo, ActivationInfo);

	PerformAction();
}

void USkill_Active_ContinuousGroupTherapy::PerformAction()
{
	StartTasksLink();
}

void USkill_Active_ContinuousGroupTherapy::StartTasksLink()
{
	PlayMontage();

	auto TaskPtr = UAbilityTask_TimerHelper::DelayTask(this);
	TaskPtr->SetDuration(Duration, PerformActionInterval);
	TaskPtr->IntervalDelegate.BindUObject(this, &ThisClass::OnTimerHelperTick);
	TaskPtr->OnFinished.BindLambda([this](auto) {
		K2_CancelAbility();
		});
	TaskPtr->ReadyForActivation();
}

void USkill_Active_ContinuousGroupTherapy::OnTimerHelperTick(UAbilityTask_TimerHelper* TaskPtr, float CurrentInterval, float Interval)
{
	if (CurrentInterval >= Interval)
	{
		EmitEffect();
	}
}

void USkill_Active_ContinuousGroupTherapy::OnNotifyBeginReceived(FName NotifyName)
{
	if (NotifyName == Skill_GroupTherapy::TriggerTherapy)
	{
	}
}

void USkill_Active_ContinuousGroupTherapy::EmitEffect()
{
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(Pawn_Object);

	FCollisionShape  CollisionShape = FCollisionShape::MakeSphere(Radius);

	FCollisionQueryParams CapsuleParams;

	TSet<ACharacterBase*>TeammatesSet;
	auto GroupMnaggerComponent = Cast<AHumanCharacter>(CharacterPtr)->GetGroupMnaggerComponent();
	if (GroupMnaggerComponent)
	{
		auto TeamsHelperSPtr = GroupMnaggerComponent->GetTeamHelper();
		if (TeamsHelperSPtr)
		{
			TeammatesSet.Add(TeamsHelperSPtr->OwnerPtr);
			for (auto Iter : TeamsHelperSPtr->MembersMap)
			{
				TeammatesSet.Add(Iter.Value);
			}
		}
	}

	// Test:按形状查找，否则直接获取“队员”计算距离即可
	TArray<struct FOverlapResult> OutOverlaps;
	if (GetWorldImp()->OverlapMultiByObjectType(
		OutOverlaps,
		CharacterPtr->GetActorLocation(),
		FQuat::Identity,
		ObjectQueryParams,
		CollisionShape,
		CapsuleParams
	))
	{
		FGameplayAbilityTargetData_GASendEvent* GAEventDataPtr = new FGameplayAbilityTargetData_GASendEvent(CharacterPtr);

		GAEventDataPtr->TriggerCharacterPtr = CharacterPtr;

		for (auto Iter : OutOverlaps)
		{
			auto TargetCharacterPtr = Cast<ACharacterBase>(Iter.GetActor());
			if (TargetCharacterPtr)
			{
				auto CharacterIter = TeammatesSet.Find(TargetCharacterPtr);
				if (CharacterIter)
				{
					FGAEventData GAEventData(TargetCharacterPtr, CharacterPtr);

					GAEventData.HP = TreatmentVolume;

					GAEventDataPtr->DataAry.Add(GAEventData);
				}
			}
		}

		auto ICPtr = CharacterPtr->GetInteractiveBaseGAComponent();
		ICPtr->SendEvent(GAEventDataPtr);
	}
}

void USkill_Active_ContinuousGroupTherapy::PlayMontage()
{
	const auto GAPerformSpeed = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes().GAPerformSpeed.GetCurrentValue();
	const float Rate = static_cast<float>(GAPerformSpeed) / 100;

	{
		auto AbilityTask_PlayMontage_HumanPtr = UAbilityTask_ASCPlayMontage::CreatePlayMontageAndWaitProxy(
			this,
			TEXT(""),
			HumanMontage,
			CharacterPtr->GetMesh()->GetAnimInstance(),
			Rate
		);

		AbilityTask_PlayMontage_HumanPtr->Ability = this;
		AbilityTask_PlayMontage_HumanPtr->SetAbilitySystemComponent(CharacterPtr->GetAbilitySystemComponent());
		AbilityTask_PlayMontage_HumanPtr->OnCompleted.BindUObject(this, &ThisClass::K2_CancelAbility);
		AbilityTask_PlayMontage_HumanPtr->OnInterrupted.BindUObject(this, &ThisClass::K2_CancelAbility);

		AbilityTask_PlayMontage_HumanPtr->OnNotifyBegin.BindUObject(this, &ThisClass::OnNotifyBeginReceived);

		AbilityTask_PlayMontage_HumanPtr->ReadyForActivation();
	}
}
