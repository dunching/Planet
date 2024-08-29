
#include "CS_PeriodicStateModify_Ice.h"

#include <Engine/AssetManager.h>
#include <Engine/StreamableManager.h>
#include "AbilitySystemGlobals.h"
#include <GameFramework/CharacterMovementComponent.h>

#include "KismetGravityLibrary.h"

#include "AbilityTask_TimerHelper.h"
#include "CharacterBase.h"
#include "InteractiveSkillComponent.h"
#include "CharacterAttributesComponent.h"
#include "GenerateType.h"
#include "GAEvent_Send.h"
#include "EffectsList.h"
#include "UIManagerSubSystem.h"
#include "EffectItem.h"
#include "InteractiveBaseGAComponent.h"
#include "GameplayTagsSubSystem.h"
#include "AbilityTask_MyApplyRootMotionConstantForce.h"
#include "AbilityTask_FlyAway.h"
#include "AbilityTask_ApplyRootMotionBySPline.h"
#include "SPlineActor.h"
#include "AbilityTask_Tornado.h"
#include "Skill_Active_Tornado.h"

FGameplayAbilityTargetData_StateModify_Ice::FGameplayAbilityTargetData_StateModify_Ice(
		int32 InCount,
		float InImmuneTime
):
	Super(UGameplayTagsSubSystem::GetInstance()->State_Debuff_Ice, 5.f),
	ImmuneTime(InImmuneTime),
	Count(InCount)
{
}

FGameplayAbilityTargetData_StateModify_Ice* FGameplayAbilityTargetData_StateModify_Ice::Clone() const
{
	auto ResultPtr =
		new FGameplayAbilityTargetData_StateModify_Ice;

	*ResultPtr = *this;

	return ResultPtr;
}

UCS_PeriodicStateModify_Ice::UCS_PeriodicStateModify_Ice():
	Super()
{

}

void UCS_PeriodicStateModify_Ice::PreActivate(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	FOnGameplayAbilityEnded::FDelegate* OnGameplayAbilityEndedDelegate,
	const FGameplayEventData* TriggerEventData /*= nullptr */
)
{
	Super::PreActivate(Handle, ActorInfo, ActivationInfo, OnGameplayAbilityEndedDelegate, TriggerEventData);

}

void UCS_PeriodicStateModify_Ice::ModifyMaterials()
{
	auto MaterialsNum=CharacterPtr->GetMesh()->GetMaterials().Num();
	auto Mesh=CharacterPtr->GetMesh();
	static float MinValue=-50.f;
	static float MaxValue=230.f;
	const auto MatValue=MinValue+float(CurrentCount)/float(MaxCount)*MaxValue;
	for (int i=0;i<MaterialsNum;i++)
	{
		auto InstDy = Cast<UMaterialInstanceDynamic>(Mesh->GetMaterial(i));
		if (!InstDy)
		{
			auto Inst = Cast<UMaterialInstance>(Mesh->GetMaterial(i));
			InstDy = UMaterialInstanceDynamic::Create(Inst, CharacterPtr);
			Mesh->SetMaterial(i,InstDy);
		}
		//static float MaxCount=3;
		InstDy->SetScalarParameterValue(TEXT("Frozen"),MatValue);
	}
}

void UCS_PeriodicStateModify_Ice::AddTags(FGameplayAbilityTargetData_StateModify_Ice*CurParamPtr)
{
	ActivationOwnedTags.AddTag(UGameplayTagsSubSystem::GetInstance()->MovementStateAble_CantPlayerInputMove);
	ActivationOwnedTags.AddTag(UGameplayTagsSubSystem::GetInstance()->MovementStateAble_CantPathFollowMove);
	ActivationOwnedTags.AddTag(UGameplayTagsSubSystem::GetInstance()->MovementStateAble_CantJump);
	ActivationOwnedTags.AddTag(UGameplayTagsSubSystem::GetInstance()->MovementStateAble_CantRotation);

	UAbilitySystemComponent* Comp = GetActorInfo().AbilitySystemComponent.Get();

	Comp->AddLooseGameplayTags(ActivationOwnedTags);

	if (UAbilitySystemGlobals::Get().ShouldReplicateActivationOwnedTags())
	{
		Comp->AddReplicatedLooseGameplayTags(ActivationOwnedTags);
	}
	
	CharacterPtr->GetInteractiveBaseGAComponent()->ClearData2Self(
		GetAllData(), GameplayAbilityTargetDataSPtr->Tag
	);
	TaskPtr->SetDuration(CurParamPtr->ImmuneTime);
	TaskPtr->UpdateDuration();

	bIsImmune = true;
	
	StateDisplayInfoSPtr->Duration = GameplayAbilityTargetDataSPtr->Duration;
	StateDisplayInfoSPtr->DataChanged();
}

void UCS_PeriodicStateModify_Ice::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UCS_PeriodicStateModify_Ice::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	CurrentCount = 0;
	
	ModifyMaterials();
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCS_PeriodicStateModify_Ice::UpdateDuration()
{
	if (bIsImmune)
	{
		return;
	}

	Super::UpdateDuration();
}

void UCS_PeriodicStateModify_Ice::PerformAction()
{
	Super::PerformAction();
	
	const auto OldCount = CurrentCount;
	
	auto CurParamPtr= dynamic_cast<FGameplayAbilityTargetData_StateModify_Ice*>(GameplayAbilityTargetDataSPtr.Get()) ;
	if (CurParamPtr)
	{
		CurrentCount += CurParamPtr->Count;
		StateDisplayInfoSPtr->Num = CurrentCount;
	}

	if (CurrentCount > MaxCount)
	{
		AddTags(CurParamPtr);
	}
	else
	{
		const auto ActuallyCount = CurrentCount - OldCount;
		//ºı…ŸÀŸ∂»
		TMap<ECharacterPropertyType, FBaseProperty>ModifyPropertyMap;
		ModifyPropertyMap.Add(ECharacterPropertyType::GAPerformSpeed, ActuallyCount * -10);
		ModifyPropertyMap.Add(ECharacterPropertyType::MoveSpeed, ActuallyCount * -10);
	
		CharacterPtr->GetInteractiveBaseGAComponent()->SendEvent2Self(
			ModifyPropertyMap, GameplayAbilityTargetDataSPtr->Tag
		);
	
		ModifyMaterials();
	}
}
