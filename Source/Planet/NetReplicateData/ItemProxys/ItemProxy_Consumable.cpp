#include "ItemProxy_Consumable.h"

#include "HumanCharacter_AI.h"
#include "CharacterAttributesComponent.h"
#include "SceneProxyExtendInfo.h"
#include "AllocationSkills.h"
#include "Skill_Base.h"
#include "Weapon_Base.h"
#include "Skill_Consumable_Generic.h"
#include "CharacterAbilitySystemComponent.h"
#include "ConversationComponent.h"
#include "GameplayTagsLibrary.h"
#include "GroupManagger.h"
#include "InventoryComponent.h"
#include "Planet_Tools.h"

FConsumableProxy::FConsumableProxy()
{
}

bool FConsumableProxy::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Super::NetSerialize(Ar, Map, bOutSuccess);
	NetSerialize_Allocationble(Ar, Map, bOutSuccess);
	NetSerialize_Unique(Ar, Map, bOutSuccess);

	return true;
}

void FConsumableProxy::UpdateByRemote(const TSharedPtr<FConsumableProxy>& RemoteSPtr)
{
	Super::UpdateByRemote(RemoteSPtr);
	UpdateByRemote_Allocationble(RemoteSPtr);
	UpdateByRemote_Unique(RemoteSPtr);
}

bool FConsumableProxy::Active()
{
#if UE_EDITOR || UE_SERVER
	if (InventoryComponentPtr->GetNetMode() == NM_DedicatedServer)
	{
		auto GameplayAbilityTargetPtr =
			new FGameplayAbilityTargetData_Consumable;

		// Test
		GameplayAbilityTargetPtr->ProxyID = GetID();

		const auto InputID = FMath::Rand32();
		FGameplayAbilitySpec AbilitySpec(
			GetTableRowProxy_Consumable()->Skill_Consumable_Class,
			1,
			InputID
		);

		auto GameplayEventData = MakeShared<FGameplayEventData>();
		GameplayEventData->TargetData.Add(GameplayAbilityTargetPtr);

		auto AllocationCharacter = GetAllocationCharacterProxy().Pin()->GetCharacterActor();
		AllocationCharacter->GetCharacterAbilitySystemComponent()->ReplicateEventData(
			InputID,
			*GameplayEventData
		);

		auto ASCPtr = GetAllocationCharacter()->GetCharacterAbilitySystemComponent();
		ASCPtr->GiveAbilityAndActivateOnce(
			AbilitySpec
		);
	}
#endif

	return true;
}

FTableRowProxy_Consumable* FConsumableProxy::GetTableRowProxy_Consumable() const
{
	auto SceneProxyExtendInfoMapPtr = USceneProxyExtendInfoMap::GetInstance();
	auto DataTable = SceneProxyExtendInfoMapPtr->DataTable_Proxy_Consumable.LoadSynchronous();

	auto SceneProxyExtendInfoPtr = DataTable->FindRow<FTableRowProxy_Consumable>(
		*GetProxyType().ToString(), TEXT("GetProxy"));
	return SceneProxyExtendInfoPtr;
}

bool FConsumableProxy::GetRemainingCooldown(float& RemainingCooldown, float& RemainingCooldownPercent) const
{
	RemainingCooldown = -1.f;

	// 独立冷却
	{
		auto AbilitySystemComponentPtr = GetAllocationCharacter()->GetCharacterAbilitySystemComponent();
		auto GameplayTagContainer = FGameplayTagContainer::EmptyContainer;
		GameplayTagContainer.AddTag(UGameplayTagsLibrary::GEData_CD);
		GameplayTagContainer.AddTag(GetProxyType());
		const auto GameplayEffectHandleAry = AbilitySystemComponentPtr->GetActiveEffects(
			FGameplayEffectQuery::MakeQuery_MatchAllOwningTags(GameplayTagContainer)
		);

		if (!GameplayEffectHandleAry.IsEmpty())
		{
			auto GameplayEffectPtr = AbilitySystemComponentPtr->GetActiveGameplayEffect(
				GameplayEffectHandleAry[0]
			);

			if (GameplayEffectPtr)
			{
				RemainingCooldown = GameplayEffectPtr->GetTimeRemaining(GetWorldImp()->GetTimeSeconds());
				RemainingCooldownPercent = RemainingCooldown / GameplayEffectPtr->GetDuration();
			}
		}
	}

	// 公共冷却
	{
		auto AbilitySystemComponentPtr = InventoryComponentPtr->GetOwner<AGroupManagger>()->GetAbilitySystemComponent();
		auto SkillCommonCooldownInfoMap = GetTableRowProxy_Consumable()->CommonCooldownInfoMap;
		for (const auto Iter : SkillCommonCooldownInfoMap)
		{
			auto CommonCooldownInfoPtr = GetTableRowProxy_CommonCooldownInfo(Iter);
			if (CommonCooldownInfoPtr)
			{
				auto GameplayTagContainer = FGameplayTagContainer::EmptyContainer;
				GameplayTagContainer.AddTag(UGameplayTagsLibrary::GEData_CD);
				GameplayTagContainer.AddTag(Iter);
		
				const auto GameplayEffectHandleAry = AbilitySystemComponentPtr->GetActiveEffects(
					FGameplayEffectQuery::MakeQuery_MatchAllOwningTags(GameplayTagContainer)
				);

				for (const auto GEIter : GameplayEffectHandleAry)
				{
					auto GameplayEffectPtr =
						AbilitySystemComponentPtr->GetActiveGameplayEffect(
							GEIter
						);

					if (GameplayEffectPtr)
					{
						const auto TempRemainingCooldown = GameplayEffectPtr->GetTimeRemaining(
							GetWorldImp()->GetTimeSeconds());
						if (TempRemainingCooldown > RemainingCooldown)
						{
							RemainingCooldown = TempRemainingCooldown;
							RemainingCooldownPercent = RemainingCooldown / GameplayEffectPtr->GetDuration();
						}
					}
				}
			}
		}
	}

	if (RemainingCooldown > 0.f)
	{
		return false;
	}

	return true;
}

bool FConsumableProxy::CheckNotInCooldown() const
{
	// 独立冷却
	{
		auto AbilitySystemComponentPtr = GetAllocationCharacter()->GetCharacterAbilitySystemComponent();
		auto GameplayTagContainer = FGameplayTagContainer::EmptyContainer;
		GameplayTagContainer.AddTag(UGameplayTagsLibrary::GEData_CD);
		GameplayTagContainer.AddTag(GetProxyType());
		const auto GameplayEffectHandleAry = AbilitySystemComponentPtr->GetActiveEffects(
			FGameplayEffectQuery::MakeQuery_MatchAllOwningTags(GameplayTagContainer)
		);

		if (!GameplayEffectHandleAry.IsEmpty())
		{
			return false;
		}
	}
	// 公共冷却
	{
		auto AbilitySystemComponentPtr = InventoryComponentPtr->GetOwner<AGroupManagger>()->GetAbilitySystemComponent();
		auto SkillCommonCooldownInfoMap = GetTableRowProxy_Consumable()->CommonCooldownInfoMap;
		for (const auto Iter : SkillCommonCooldownInfoMap)
		{
			auto CommonCooldownInfoPtr = GetTableRowProxy_CommonCooldownInfo(Iter);
			if (CommonCooldownInfoPtr)
			{
				auto GameplayTagContainer = FGameplayTagContainer::EmptyContainer;
				GameplayTagContainer.AddTag(UGameplayTagsLibrary::GEData_CD);
				GameplayTagContainer.AddTag(Iter);
		
				const auto GameplayEffectHandleAry =
					AbilitySystemComponentPtr->GetActiveEffects(
						FGameplayEffectQuery::MakeQuery_MatchAllOwningTags(GameplayTagContainer)
					);

				if (!GameplayEffectHandleAry.IsEmpty())
				{
					return false;
				}
			}
		}
	}
	return true;
}

void FConsumableProxy::AddCooldownConsumeTime(float NewTime)
{
}

void FConsumableProxy::FreshUniqueCooldownTime()
{
}

void FConsumableProxy::ApplyCooldown()
{
	// GetAllocationCharacter()->GetConversationComponent()->ApplyCooldown(
	// 	this
	// );
}

void FConsumableProxy::OffsetCooldownTime()
{
}
