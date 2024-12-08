
#include "ItemProxy_Consumable.h"

#include "HumanCharacter_AI.h"
#include "CharacterAttributesComponent.h"
#include "SceneProxyExtendInfo.h"
#include "AllocationSkills.h"
#include "Skill_Base.h"
#include "Weapon_Base.h"
#include "Skill_Consumable_Generic.h"
#include "BaseFeatureComponent.h"
#include "CDcaculatorComponent.h"

FConsumableProxy::FConsumableProxy()
{

}

bool FConsumableProxy::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Super::NetSerialize(Ar, Map, bOutSuccess);

	Ar << Num;

	return true;
}

void FConsumableProxy::UpdateByRemote(const TSharedPtr<FConsumableProxy>& RemoteSPtr)
{
	Super::UpdateByRemote(RemoteSPtr);

	const auto OldValue = Num;

	Num = RemoteSPtr->Num;

	CallbackContainerHelper.ValueChanged(OldValue, Num);
}

bool FConsumableProxy::Active()
{
#if UE_EDITOR || UE_SERVER
	auto ProxyCharacterPtr = GetOwnerCharacter();
	if (ProxyCharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		auto GameplayAbilityTargetPtr =
			new FGameplayAbilityTargetData_Consumable;

		// Test
		GameplayAbilityTargetPtr->ProxyID = GetID();

		const auto InputID = FMath::Rand32();
		FGameplayAbilitySpec AbilitySpec(
			USkill_Consumable_Generic::StaticClass(),
			1,
			InputID
		);

		auto GameplayEventData = MakeShared<FGameplayEventData>();
		GameplayEventData->TargetData.Add(GameplayAbilityTargetPtr);

		auto AllocationCharacter = GetAllocationCharacterProxy().Pin()->ProxyCharacterPtr;
		AllocationCharacter->GetAbilitySystemComponent()->ReplicateEventData(
			InputID,
			*GameplayEventData
		);

		auto ASCPtr = ProxyCharacterPtr->GetAbilitySystemComponent();
		ASCPtr->GiveAbilityAndActivateOnce(
			AbilitySpec
		);
	}
#endif

	return true;
}

void FConsumableProxy::AddCurrentValue(int32 val)
{
	const auto Old = Num;
	Num += val;

	CallbackContainerHelper.ValueChanged(Old, Num);
}

int32 FConsumableProxy::GetCurrentValue() const
{
	return Num;
}

FTableRowProxy_Consumable* FConsumableProxy::GetTableRowProxy_Consumable() const
{
	auto SceneProxyExtendInfoMapPtr = USceneProxyExtendInfoMap::GetInstance();
	auto DataTable = SceneProxyExtendInfoMapPtr->DataTable_Proxy_Consumable.LoadSynchronous();

	auto SceneProxyExtendInfoPtr = DataTable->FindRow<FTableRowProxy_Consumable>(*ProxyType.ToString(), TEXT("GetProxy"));
	return SceneProxyExtendInfoPtr;
}

bool FConsumableProxy::GetRemainingCooldown(float& RemainingCooldown, float& RemainingCooldownPercent) const
{
	auto CDSPtr = GetAllocationCharacter()->GetCDCaculatorComponent()->GetCooldown(
		this
	);

	if (CDSPtr)
	{
		return CDSPtr->GetRemainingCooldown(RemainingCooldown, RemainingCooldownPercent);
	}

	return true;
}

bool FConsumableProxy::CheckCooldown() const
{
	auto CDSPtr = GetAllocationCharacter()->GetCDCaculatorComponent()->GetCooldown(
		this
	);

	if (CDSPtr)
	{
		return CDSPtr->CheckCooldown();
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
	GetAllocationCharacter()->GetCDCaculatorComponent()->ApplyCooldown(
		this
	);
}

void FConsumableProxy::OffsetCooldownTime()
{
}
