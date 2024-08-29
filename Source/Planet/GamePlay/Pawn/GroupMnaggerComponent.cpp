
#include "GroupMnaggerComponent.h"

#include "AIController.h"
#include "Kismet/GameplayStatics.h"

#include "GroupsManaggerSubSystem.h"
#include "CharacterBase.h"
#include "HumanCharacter.h"
#include "CharacterTitle.h"
#include "HumanAIController.h"
#include "PlanetControllerInterface.h"
#include "PlanetPlayerState.h"
#include "HoldingItemsComponent.h"
#include "SceneUnitContainer.h"
#include "GameOptions.h"
#include "SceneUnitTable.h"
#include "SceneElement.h"
#include "Planet.h"
#include "LogWriter.h"

#ifdef WITH_EDITOR
static TAutoConsoleVariable<int32> GroupMnaggerComponent_KnowCharaterChanged(
	TEXT("GroupMnaggerComponent.KnowCharaterChanged"),
	1,
	TEXT("")
	TEXT(" default: 0"));
#endif

FName UGroupMnaggerComponent::ComponentName = TEXT("GroupMnaggerComponent");

UGroupMnaggerComponent::UGroupMnaggerComponent(const FObjectInitializer& ObjectInitializer) :
	Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 1.f / 10;
}

FSkillCooldownHelper::FSkillCooldownHelper()
{

}

bool FSkillCooldownHelper::CheckCooldown() const
{
	if (CooldownTime > 0.f)
	{
		return CooldownConsumeTime > CooldownTime;
	}
	else
	{
		return true;
	}
}

void FSkillCooldownHelper::IncreaseCooldownTime(float DeltaTime)
{
	CooldownConsumeTime += DeltaTime;
}

void FSkillCooldownHelper::AddCooldownConsumeTime(float NewTime)
{
	IncreaseCooldownTime(NewTime);
}

void FSkillCooldownHelper::FreshCooldownTime()
{
	CooldownConsumeTime = CooldownTime + UGameOptions::GetInstance()->ResetCooldownTime;
}

void FSkillCooldownHelper::ResetCooldownTime()
{
	CooldownConsumeTime = 0.f;
}

void FSkillCooldownHelper::OffsetCooldownTime()
{
	CooldownConsumeTime = CooldownTime - UGameOptions::GetInstance()->ResetCooldownTime;
}

void FSkillCooldownHelper::SetCooldown(float CooldDown)
{
	CooldownTime = CooldDown;
}

bool FSkillCooldownHelper::GetRemainingCooldown(float& RemainingCooldown, float& RemainingCooldownPercent) const
{
	if (CooldownTime < 0.f)
	{
		const auto Remaining = CooldownTime - CooldownConsumeTime;

		if (Remaining <= 0.f)
		{
			RemainingCooldown = 0.f;

			RemainingCooldownPercent = 1.f;

			return true;
		}
		else
		{
			RemainingCooldown = Remaining;

			RemainingCooldownPercent = RemainingCooldown / UGameOptions::GetInstance()->ResetCooldownTime;

			return false;
		}
	}

	const auto Remaining = CooldownTime - CooldownConsumeTime;

	if (Remaining <= 0.f)
	{
		RemainingCooldown = 0.f;

		RemainingCooldownPercent = 1.f;

		return true;
	}
	else
	{
		RemainingCooldown = Remaining;

		RemainingCooldownPercent = RemainingCooldown / CooldownTime;

		return false;
	}
}

void UGroupMnaggerComponent::TickComponent(
	float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction
)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	auto CaculationCooldownLambda = [DeltaTime](TMap<FGameplayTag, TSharedPtr<FSkillCooldownHelper>>& CooldownMap) {
		auto TempMap = CooldownMap;
		for (auto& Iter : TempMap)
		{
			Iter.Value->IncreaseCooldownTime(DeltaTime);
			if (Iter.Value->CheckCooldown())
			{
				CooldownMap.Remove(Iter.Key);
			}
		}
		};

	CaculationCooldownLambda(UniqueCooldownMap);
	CaculationCooldownLambda(GetGroupHelper()->CommonCooldownMap);
}

void UGroupMnaggerComponent::AddCharacterToGroup(FCharacterUnitType* CharacterUnitPtr)
{
	//
	GetGroupHelper()->AddCharacter(CharacterUnitPtr);
}

void UGroupMnaggerComponent::AddCharacterToTeam(FCharacterUnitType* CharacterUnitPtr)
{
	GetGroupHelper()->AddCharacter(CharacterUnitPtr);
}

void UGroupMnaggerComponent::OnAddToNewGroup(FCharacterUnitType* CharacterUnitPtr)
{
	GroupHelperSPtr = CharacterUnitPtr->ProxyCharacterPtr->GetGroupMnaggerComponent()->GetGroupHelper();

	if (CharacterUnitPtr->ProxyCharacterPtr->IsPlayerControlled())
	{
		TeamHelperSPtr->SwitchTeammateOption(ETeammateOption::kFree);
	}

	GroupHelperChangedDelegateContainer.ExcuteCallback();
}

void UGroupMnaggerComponent::OnAddToNewTeam(FCharacterUnitType* CharacterUnitPtr)
{
	TeamHelperSPtr = CharacterUnitPtr->ProxyCharacterPtr->GetGroupMnaggerComponent()->GetTeamHelper();

	TeamHelperChangedDelegateContainer.ExcuteCallback();
}

const TSharedPtr<FGroupMatesHelper>& UGroupMnaggerComponent::GetGroupHelper()
{
	if (!GroupHelperSPtr)
	{
		CreateGroup();
	}
	return GroupHelperSPtr;
}

const TSharedPtr<FTeamMatesHelper>& UGroupMnaggerComponent::GetTeamHelper()
{
	if (!TeamHelperSPtr)
	{
		CreateTeam();
	}
	return TeamHelperSPtr;
}

TMap<FGameplayTag, TWeakPtr<FSkillCooldownHelper>> UGroupMnaggerComponent::ApplyCooldown(UActiveSkillUnit* ActiveSkillUnitPtr)
{
	TMap<FGameplayTag, TWeakPtr<FSkillCooldownHelper>> Result;

	if (ActiveSkillUnitPtr)
	{
		// 独立CD
		{
			auto SkillCooldownHelperSPtr = ApplyUniqueCooldown(ActiveSkillUnitPtr);
			Result.Add(ActiveSkillUnitPtr->GetUnitType(), SkillCooldownHelperSPtr);
		}

		// 公共CD
		auto SkillCommonCooldownInfoMap = ActiveSkillUnitPtr->GetTableRowUnit_ActiveSkillExtendInfo()->SkillCommonCooldownInfoMap;
		for (const auto Iter : SkillCommonCooldownInfoMap)
		{
			auto SkillCooldownHelperSPtr = ApplyCommonCooldownTime(Iter);
			Result.Add(Iter, SkillCooldownHelperSPtr);
		}
	}

	return Result;
}

TWeakPtr<FSkillCooldownHelper> UGroupMnaggerComponent::ApplyUniqueCooldown(UActiveSkillUnit* ActiveSkillUnitPtr)
{
	TWeakPtr<FSkillCooldownHelper> Result;
	if (ActiveSkillUnitPtr)
	{
		auto SkillCooldownInfoMap = ActiveSkillUnitPtr->GetTableRowUnit_ActiveSkillExtendInfo()->SkillCooldownInfoMap;
		if (UniqueCooldownMap.Contains(ActiveSkillUnitPtr->GetUnitType()))
		{
			UniqueCooldownMap[ActiveSkillUnitPtr->GetUnitType()]->SetCooldown(
				SkillCooldownInfoMap[ActiveSkillUnitPtr->Level]
			);
			UniqueCooldownMap[ActiveSkillUnitPtr->GetUnitType()]->ResetCooldownTime();
		}
		else
		{
			TSharedPtr<FSkillCooldownHelper> SkillCooldownHelperSPtr = MakeShared<FSkillCooldownHelper>();

			if (SkillCooldownInfoMap.Contains(ActiveSkillUnitPtr->Level))
			{
				SkillCooldownHelperSPtr->SetCooldown(
					SkillCooldownInfoMap[ActiveSkillUnitPtr->Level]
				);
				SkillCooldownHelperSPtr->ResetCooldownTime();
			}

			UniqueCooldownMap.Add(ActiveSkillUnitPtr->GetUnitType(), SkillCooldownHelperSPtr);
			Result = SkillCooldownHelperSPtr;
		}
	}
	return Result;
}

TMap<FGameplayTag, TWeakPtr<FSkillCooldownHelper>> UGroupMnaggerComponent::GetCooldown(const UActiveSkillUnit* ActiveSkillUnitPtr)
{
	TMap<FGameplayTag, TWeakPtr<FSkillCooldownHelper>> Result;

	// 独立CD
	if (UniqueCooldownMap.Contains(ActiveSkillUnitPtr->GetUnitType()))
	{
		Result.Add(ActiveSkillUnitPtr->GetUnitType(), UniqueCooldownMap[ActiveSkillUnitPtr->GetUnitType()]);
	}

	// 公共CD
	auto SkillCommonCooldownInfoMap = ActiveSkillUnitPtr->GetTableRowUnit_ActiveSkillExtendInfo()->SkillCommonCooldownInfoMap;
	auto& CommonCooldownMap = GetGroupHelper()->CommonCooldownMap;
	for (const auto Iter : SkillCommonCooldownInfoMap)
	{
		if (CommonCooldownMap.Contains(Iter))
		{
			Result.Add(Iter, CommonCooldownMap[Iter]);
		}
	}
	return Result;
}

TMap<FGameplayTag, TWeakPtr<FSkillCooldownHelper>> UGroupMnaggerComponent::GetCooldown(const UConsumableUnit* ConsumableUnitPtr)
{
	TMap<FGameplayTag, TWeakPtr<FSkillCooldownHelper>> Result;

	// 公共CD
	auto SkillCommonCooldownInfoMap = ConsumableUnitPtr->GetTableRowUnit_Consumable()->CommonCooldownInfoMap;
	auto& CommonCooldownMap = GetGroupHelper()->CommonCooldownMap;
	for (const auto Iter : SkillCommonCooldownInfoMap)
	{
		if (CommonCooldownMap.Contains(Iter))
		{
			Result.Add(Iter, CommonCooldownMap[Iter]);
		}
	}
	return Result;
}

TMap<FGameplayTag, TWeakPtr<FSkillCooldownHelper>> UGroupMnaggerComponent::ApplyCooldown(UConsumableUnit* ConsumableUnitPtr)
{
	TMap<FGameplayTag, TWeakPtr<FSkillCooldownHelper>> Result;
	if (ConsumableUnitPtr)
	{
		// 公共CD
		auto SkillCommonCooldownInfoMap = ConsumableUnitPtr->GetTableRowUnit_Consumable()->CommonCooldownInfoMap;
		for (const auto Iter : SkillCommonCooldownInfoMap)
		{
			auto SkillCooldownHelperSPtr = ApplyCommonCooldownTime(Iter);
			Result.Add(Iter, SkillCooldownHelperSPtr);
		}
	}
	return Result;
}

TWeakPtr<FSkillCooldownHelper> UGroupMnaggerComponent::ApplyCommonCooldownTime(
	const FGameplayTag& CommonCooldownTag
)
{
	TWeakPtr<FSkillCooldownHelper> Result;

	auto& CommonCooldownMap = GetGroupHelper()->CommonCooldownMap;
	if (CommonCooldownMap.Contains(CommonCooldownTag))
	{
		auto CommonCooldownInfoPtr = GetTableRowUnit_CommonCooldownInfo(CommonCooldownTag);
		if (CommonCooldownInfoPtr)
		{
			CommonCooldownMap[CommonCooldownTag]->SetCooldown(CommonCooldownInfoPtr->CoolDownTime);
			CommonCooldownMap[CommonCooldownTag]->ResetCooldownTime();
		}
	}
	else
	{
		TSharedPtr<FSkillCooldownHelper> SkillCooldownHelperSPtr = MakeShared<FSkillCooldownHelper>();

		auto CommonCooldownInfoPtr = GetTableRowUnit_CommonCooldownInfo(CommonCooldownTag);
		if (CommonCooldownInfoPtr)
		{
			SkillCooldownHelperSPtr->SetCooldown(CommonCooldownInfoPtr->CoolDownTime);
			SkillCooldownHelperSPtr->ResetCooldownTime();

			CommonCooldownMap.Add(CommonCooldownTag, SkillCooldownHelperSPtr);
			Result = SkillCooldownHelperSPtr;
		}
	}
	return Result;
}

void UGroupMnaggerComponent::BeginPlay()
{
	Super::BeginPlay();
}

TSharedPtr<FGroupMatesHelper> UGroupMnaggerComponent::CreateGroup()
{
	GroupHelperSPtr = MakeShared<FGroupMatesHelper>();
	GroupHelperSPtr->OwnerCharacterUnitPtr = GetOwner<FOwnerType>()->GetCharacterUnit();
	for (;;)
	{
		GroupHelperSPtr->ID = FMath::RandRange(1, std::numeric_limits<int32>::max());
		break;
	}

	GroupHelperChangedDelegateContainer.ExcuteCallback();

	return GroupHelperSPtr;
}

TSharedPtr<FTeamMatesHelper> UGroupMnaggerComponent::CreateTeam()
{
	TeamHelperSPtr = MakeShared<FTeamMatesHelper>();
	TeamHelperSPtr->OwnerCharacterUnitPtr = GetOwner<FOwnerType>()->GetCharacterUnit();
	for (;;)
	{
		TeamHelperSPtr->ID = FMath::RandRange(1, std::numeric_limits<int32>::max());
		break;
	}

	TeamHelperChangedDelegateContainer.ExcuteCallback();

	return TeamHelperSPtr;
}

void FGroupMatesHelper::AddCharacter(FPawnType* PCPtr)
{
	auto CharacterUnitPtr = PCPtr->GetCharacterUnit();
	AddCharacter(CharacterUnitPtr);
}

void FGroupMatesHelper::AddCharacter(FCharacterUnitType* CharacterUnitPtr)
{
	MembersSet.Add(CharacterUnitPtr);

	CharacterUnitPtr->ProxyCharacterPtr->GetGroupMnaggerComponent()->OnAddToNewGroup(OwnerCharacterUnitPtr);

	MembersChanged.ExcuteCallback(EGroupMateChangeType::kAdd, CharacterUnitPtr);
}

bool FGroupMatesHelper::IsMember(FCharacterUnitType* CharacterUnitPtr) const
{
	for (auto Iter : MembersSet)
	{
		if (Iter == CharacterUnitPtr)
		{
			return true;
		}
	}

	if (CharacterUnitPtr == OwnerCharacterUnitPtr)
	{
		return true;
	}

	return false;
}

void FTeamMatesHelper::SwitchTeammateOption(ETeammateOption InTeammateOption)
{
	TeammateOption = InTeammateOption;

	TeammateOptionChanged.ExcuteCallback(InTeammateOption, OwnerCharacterUnitPtr);
}

ETeammateOption FTeamMatesHelper::GetTeammateOption() const
{
	return TeammateOption;
}

void FTeamMatesHelper::AddKnowCharacter(ACharacterBase* CharacterPtr)
{
	for (auto& Iter : KnowCharatersSet)
	{
		if (Iter.Key == CharacterPtr)
		{
			Iter.Value++;

			return;
		}
	}

	KnowCharatersSet.Add({ CharacterPtr, 1 });
	KnowCharaterChanged(CharacterPtr, true);
#ifdef WITH_EDITOR
	if (GroupMnaggerComponent_KnowCharaterChanged.GetValueOnGameThread())
	{
		PRINTINVOKEWITHSTR(FString(TEXT("")));
	}
#endif
}

void FTeamMatesHelper::RemoveKnowCharacter(ACharacterBase* CharacterPtr)
{
	for (int32 Index = 0; Index < KnowCharatersSet.Num(); Index++)
	{
		if (KnowCharatersSet[Index].Key == CharacterPtr)
		{
			KnowCharatersSet[Index].Value--;

			if (KnowCharatersSet[Index].Value <= 0)
			{
				KnowCharatersSet.RemoveAt(Index);
			}
			KnowCharaterChanged(CharacterPtr, false);
#ifdef WITH_EDITOR
			if (GroupMnaggerComponent_KnowCharaterChanged.GetValueOnGameThread())
			{
				PRINTINVOKEWITHSTR(FString(TEXT("")));
			}
#endif
			return;
		}
	}
}

TWeakObjectPtr<ACharacterBase> FTeamMatesHelper::GetKnowCharacter() const
{
	if (ForceKnowCharater.IsValid())
	{
		return ForceKnowCharater;
	}

	for (auto Iter : KnowCharatersSet)
	{
		return Iter.Key;
	}

	return nullptr;
}
