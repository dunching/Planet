
#include "Skill_Active_GroupTherapy.h"

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
#include "EquipmentElementComponent.h"
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

namespace Skill_Active_ContinuousGroupTherapy
{
	const FName TriggerTherapy = TEXT("TriggerTherapy");

	const FName AttachEnd = TEXT("AttachEnd");
}

USkill_Active_GroupTherapy::USkill_Active_GroupTherapy() :
	Super()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	bRetriggerInstancedAbility = true;
}

void USkill_Active_GroupTherapy::PreActivate(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
	const FGameplayEventData* TriggerEventData /*= nullptr */
)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);
}

void USkill_Active_GroupTherapy::ActivateAbility(
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

void USkill_Active_GroupTherapy::PerformAction()
{
	StartTasksLink();
}

void USkill_Active_GroupTherapy::StartTasksLink()
{
	PlayMontage();
}

void USkill_Active_GroupTherapy::OnNotifyBeginReceived(FName NotifyName)
{
	if (NotifyName == Skill_Active_ContinuousGroupTherapy::TriggerTherapy)
	{
		EmitEffect();
	}
}

void USkill_Active_GroupTherapy::EmitEffect()
{
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(PawnECC);

	FCollisionShape  CollisionShape = FCollisionShape::MakeSphere(Radius);

	FCollisionQueryParams CapsuleParams;

	TSet<ACharacterBase*>TeammatesSet;
	TeammatesSet.Add(CharacterPtr);
	auto GroupMnaggerComponent = Cast<AHumanCharacter>(CharacterPtr)->GetGroupMnaggerComponent();
	if (GroupMnaggerComponent)
	{
		auto TeamsHelperSPtr = GroupMnaggerComponent->GetTeamHelper();
		if (TeamsHelperSPtr)
		{
			for (auto Iter : TeamsHelperSPtr->MembersMap)
			{
				TeammatesSet.Add(Cast<ACharacterBase>(Cast<AController>(Iter.Value)->GetPawn()));
			}
		}
	}

	// Test:����״���ң�����ֱ�ӻ�ȡ����Ա��������뼴��
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
		FGameplayAbilityTargetData_GAEvent* GAEventData = new FGameplayAbilityTargetData_GAEvent(CharacterPtr);

		FGameplayEventData Payload;
		Payload.TargetData.Add(GAEventData);

		GAEventData->TargetActorAry.Empty();
		GAEventData->TriggerCharacterPtr = CharacterPtr;
		GAEventData->Data.TreatmentVolume = TreatmentVolume;

		for (auto Iter : OutOverlaps)
		{
			auto TargetCharacterPtr = Cast<ACharacterBase>(Iter.GetActor());
			if (TargetCharacterPtr)
			{
				auto CharacterIter = TeammatesSet.Find(TargetCharacterPtr);
				if (CharacterIter)
				{
					GAEventData->TargetActorAry.Add(TargetCharacterPtr);
				}
			}
		}

		SendEvent(Payload);
	}
}

void USkill_Active_GroupTherapy::PlayMontage()
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