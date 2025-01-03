
#include "ItemProxy_Skills.h"

#include "AbilitySystemComponent.h"

#include "AssetRefMap.h"
#include "Planet.h"
#include "CharacterBase.h"
#include "HumanCharacter_AI.h"
#include "CharacterAttributesComponent.h"
#include "PlanetControllerInterface.h"
#include "HumanCharacter.h"
#include "SceneProxyExtendInfo.h"
#include "GameplayTagsLibrary.h"
#include "CharacterAttibutes.h"
#include "AllocationSkills.h"
#include "ItemProxy_Container.h"
#include "TeamMatesHelperComponent.h"
#include "PropertyEntrys.h"
#include "CharactersInfo.h"
#include "Skill_Base.h"
#include "Weapon_Base.h"
#include "Skill_Active_Base.h"
#include "Skill_Active_Control.h"
#include "Skill_WeaponActive_Base.h"
#include "Skill_WeaponActive_PickAxe.h"
#include "Skill_WeaponActive_HandProtection.h"
#include "Skill_WeaponActive_RangeTest.h"
#include "Weapon_HandProtection.h"
#include "Weapon_PickAxe.h"
#include "Weapon_RangeTest.h"
#include "HoldingItemsComponent.h"
#include "CDCaculatorComponent.h"
#include "Skill_Consumable_Generic.h"
#include "CharacterAbilitySystemComponent.h"
#include "Weapon_Bow.h"
#include "Skill_WeaponActive_Bow.h"
#include "Skill_WeaponActive_FoldingFan.h"
#include "ItemProxy_Character.h"
FSkillProxy::FSkillProxy() :
	Super()
{

}

bool FSkillProxy::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Super::NetSerialize(Ar, Map, bOutSuccess);

	Ar << Level;
	Ar << GameplayAbilitySpecHandle;

	return true;
}

void FSkillProxy::SetAllocationCharacterProxy(const TSharedPtr<FCharacterProxy>& InAllocationCharacterProxyPtr, const FGameplayTag& InSocketTag)
{
	if (!InAllocationCharacterProxyPtr)
	{
		UnAllocation();
	}

	Super::SetAllocationCharacterProxy(InAllocationCharacterProxyPtr, InSocketTag);
}

void FSkillProxy::UpdateByRemote(const TSharedPtr<FSkillProxy>& RemoteSPtr)
{
	Super::UpdateByRemote(RemoteSPtr);

	Level = RemoteSPtr->Level;
	GameplayAbilitySpecHandle = RemoteSPtr->GameplayAbilitySpecHandle;
}

TSubclassOf<USkill_Base> FSkillProxy::GetSkillClass() const
{
	return nullptr;
}

void FSkillProxy::Allocation()
{
	RegisterSkill();
}

void FSkillProxy::UnAllocation()
{
	UnRegisterSkill();
}

void FSkillProxy::RegisterSkill()
{
#if UE_EDITOR || UE_SERVER
	auto ProxyCharacterPtr = GetOwnerCharacter();
	if (ProxyCharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		auto GameplayAbilityTargetDataPtr = new FGameplayAbilityTargetData_SkillBase_RegisterParam;

		GameplayAbilityTargetDataPtr->ProxyID = GetID();

		const auto InputID = FMath::RandHelper(std::numeric_limits<int32>::max());
		FGameplayAbilitySpec GameplayAbilitySpec(
			GetSkillClass(),
			Level,
			InputID
		);

		FGameplayEventData GameplayEventData;
		GameplayEventData.TargetData.Add(GameplayAbilityTargetDataPtr);

		auto AllocationCharacter = GetAllocationCharacterProxy().Pin()->ProxyCharacterPtr;

		AllocationCharacter->GetCharacterAbilitySystemComponent()->ReplicateEventData(
			InputID,
			GameplayEventData
		);
		GameplayAbilitySpecHandle = AllocationCharacter->GetCharacterAbilitySystemComponent()->GiveAbility(GameplayAbilitySpec);
	}
#endif
}

void FSkillProxy::UnRegisterSkill()
{
#if UE_EDITOR || UE_SERVER
	auto ProxyCharacterPtr = GetOwnerCharacter();
	if (ProxyCharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		if (GetAllocationCharacterProxy().IsValid())
		{
			auto AllocationCharacter = GetAllocationCharacterProxy().Pin()->ProxyCharacterPtr;

			if (AllocationCharacter.IsValid())
			{
				auto ASCPtr = AllocationCharacter->GetCharacterAbilitySystemComponent();

				ASCPtr->CancelAbilityHandle(GameplayAbilitySpecHandle);
				ASCPtr->ClearAbility(GameplayAbilitySpecHandle);
			}
		}
	}
#endif

	GameplayAbilitySpecHandle = FGameplayAbilitySpecHandle();
}

TArray<USkill_Base*> FSkillProxy::GetGAInstAry()const
{
	TArray<USkill_Base*>ResultAry;
	auto ProxyCharacterPtr = GetOwnerCharacter();
	auto ASCPtr = ProxyCharacterPtr->GetCharacterAbilitySystemComponent();
	auto GameplayAbilitySpecPtr = ASCPtr->FindAbilitySpecFromHandle(GameplayAbilitySpecHandle);
	if (GameplayAbilitySpecPtr)
	{
		ResultAry.Add(Cast<USkill_Base>(GameplayAbilitySpecPtr->GetPrimaryInstance()));
	}

	return ResultAry;
}

USkill_Base* FSkillProxy::GetGAInst()const
{
	auto ProxyCharacterPtr = GetOwnerCharacter();
	auto ASCPtr = ProxyCharacterPtr->GetCharacterAbilitySystemComponent();
	auto GameplayAbilitySpecPtr = ASCPtr->FindAbilitySpecFromHandle(GameplayAbilitySpecHandle);
	if (GameplayAbilitySpecPtr)
	{
		return Cast<USkill_Base>(GameplayAbilitySpecPtr->GetPrimaryInstance());
	}

	return nullptr;
}

FGameplayAbilitySpecHandle FSkillProxy::GetGAHandle() const
{
	return GameplayAbilitySpecHandle;
}

FTalentSkillProxy::FTalentSkillProxy()
{
}

FActiveSkillProxy::FActiveSkillProxy()
{
}

bool FActiveSkillProxy::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Super::NetSerialize(Ar, Map, bOutSuccess);

	// if (Ar.IsSaving())
	// {
	// 	auto Handle = GetTypeHash(CD_GE_Handle);
	// 	Ar << Handle;
	// }
	// else if (Ar.IsLoading())
	// {
	// 	int32 Handle= 0;
	// 	Ar << Handle;
	// 	CD_GE_Handle = FActiveGameplayEffectHandle(Handle);
	// }

	return true;
}

bool FActiveSkillProxy::CanActive() const
{
	auto InGAInsPtr = Cast<USkill_Active_Base>(GetGAInst());
	if (!InGAInsPtr)
	{
		return false;
	}

	// 本地判断是否能释放（有些条件仅本地存在，比如是否锁定了目标）
	if (
		InGAInsPtr->CanActivateAbility(InGAInsPtr->GetCurrentAbilitySpecHandle(),
			InGAInsPtr->GetCurrentActorInfo())
		)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool FActiveSkillProxy::Active()
{
	// #if UE_EDITOR || UE_SERVER
	auto ProxyCharacterPtr = GetOwnerCharacter();
	// 	if (ProxyCharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		auto InGAInsPtr = Cast<USkill_Active_Base>(GetGAInst());
		if (!InGAInsPtr)
		{
			return false;
		}

		auto ASCPtr = GetOwnerCharacter()->GetCharacterAbilitySystemComponent();

		// 需要特殊参数的
		if (
			GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Active_Control)
			)
		{
			if (InGAInsPtr->IsActive())
			{
				InGAInsPtr->SetContinuePerform(true);
				return true;
			}
			else
			{
				auto GameplayAbilityTargetPtr =
					new FGameplayAbilityTargetData_Control;

				// Test
				GameplayAbilityTargetPtr->TargetCharacterPtr = GetAllocationCharacter();

				FGameplayEventData Payload;
				Payload.TargetData.Add(GameplayAbilityTargetPtr);

				return ASCPtr->TriggerAbilityFromGameplayEvent(
					InGAInsPtr->GetCurrentAbilitySpecHandle(),
					ASCPtr->AbilityActorInfo.Get(),
					GetProxyType(),
					&Payload,
					*ASCPtr
				);
			}
		}
		else
		{
			if (InGAInsPtr->IsActive())
			{
				InGAInsPtr->SetContinuePerform(true);
				return true;
			}
			else
			{
				auto GameplayAbilityTargetPtr =
					new FGameplayAbilityTargetData_ActiveSkill_ActiveParam;

				GameplayAbilityTargetPtr->bIsAutoContinue =
					ProxyCharacterPtr->GetRemoteRole() == ROLE_AutonomousProxy ? false : true;

				FGameplayEventData Payload;
				Payload.TargetData.Add(GameplayAbilityTargetPtr);

				return ASCPtr->TriggerAbilityFromGameplayEvent(
					InGAInsPtr->GetCurrentAbilitySpecHandle(),
					ASCPtr->AbilityActorInfo.Get(),
					GetProxyType(),
					&Payload,
					*ASCPtr
				);
			}
		}
	}
	//#endif

	//	return true;
}

void FActiveSkillProxy::Cancel()
{
}

bool FActiveSkillProxy::GetRemainingCooldown(float& RemainingCooldown, float& RemainingCooldownPercent) const
{
	const auto GameplayEffectHandleAry = GetOwnerCharacter()->GetCharacterAbilitySystemComponent()->GetActiveEffects(
	FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(FGameplayTagContainer(GetProxyType()))
	);

	if (!GameplayEffectHandleAry.IsEmpty())
	{
		auto GameplayEffectPtr = GetOwnerCharacter()->GetCharacterAbilitySystemComponent()->GetActiveGameplayEffect(
		GameplayEffectHandleAry[0]
		);

		if (GameplayEffectPtr)
		{
			RemainingCooldown = GameplayEffectPtr->GetTimeRemaining(GetWorldImp()->GetTimeSeconds());
			RemainingCooldownPercent = RemainingCooldown / GameplayEffectPtr->GetDuration(); 

			return false;
		}
	}
	return true;
}

bool FActiveSkillProxy::CheckCooldown() const
{
	const auto GameplayEffectHandleAry = GetOwnerCharacter()->GetCharacterAbilitySystemComponent()->GetActiveEffects(
	FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(FGameplayTagContainer(GetProxyType()))
	);

	return GameplayEffectHandleAry.IsEmpty();
}

void FActiveSkillProxy::AddCooldownConsumeTime(float NewTime)
{
}

void FActiveSkillProxy::FreshUniqueCooldownTime()
{
}

void FActiveSkillProxy::ApplyCooldown()
{
}

void FActiveSkillProxy::OffsetCooldownTime()
{
}

// void FActiveSkillProxy::SetCDGEChandle(FActiveGameplayEffectHandle InCD_GE_Handle)
// {
// 	CD_GE_Handle = InCD_GE_Handle;
//
// 	Update2Client();
// }

FTableRowProxy_ActiveSkillExtendInfo* FActiveSkillProxy::GetTableRowProxy_ActiveSkillExtendInfo() const
{
	auto SceneProxyExtendInfoMapPtr = USceneProxyExtendInfoMap::GetInstance();
	auto DataTable = SceneProxyExtendInfoMapPtr->DataTable_Proxy_ActiveSkillExtendInfo.LoadSynchronous();

	auto SceneProxyExtendInfoPtr = DataTable->FindRow<FTableRowProxy_ActiveSkillExtendInfo>(*ProxyType.ToString(), TEXT("GetProxy"));
	return SceneProxyExtendInfoPtr;
}

TSubclassOf<USkill_Base> FActiveSkillProxy::GetSkillClass() const
{
	return GetTableRowProxy_ActiveSkillExtendInfo()->SkillClass;
}

FPassiveSkillProxy::FPassiveSkillProxy()
{
}

void FPassiveSkillProxy::InitialProxy(const FGameplayTag& InProxyType)
{
	Super::InitialProxy(InProxyType);
}

void FPassiveSkillProxy::Allocation()
{
	Super::Allocation();

#if UE_EDITOR || UE_SERVER
	auto ProxyCharacterPtr = GetOwnerCharacter();
	if (ProxyCharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		auto AllocationCharacter = GetAllocationCharacter();
		// 词条
		{
			auto MainPropertyEntryPtr = GetMainPropertyEntry();
			if (MainPropertyEntryPtr)
			{
				TMap<ECharacterPropertyType, FBaseProperty> ModifyPropertyMap;

				for (const auto& Iter : MainPropertyEntryPtr->Map)
				{
					ModifyPropertyMap.Add(Iter);
				}

				AllocationCharacter->GetCharacterAbilitySystemComponent()->SendEvent2Self(
					ModifyPropertyMap, GetProxyType()
				);
			}
		}
	}
#endif
}

void FPassiveSkillProxy::UnAllocation()
{
#if UE_EDITOR || UE_SERVER
	auto ProxyCharacterPtr = GetOwnerCharacter();
	if (ProxyCharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		auto AllocationCharacter = GetAllocationCharacter();
		AllocationCharacter->GetCharacterAbilitySystemComponent()->ClearData2Self(
			GetAllData(), GetProxyType()
		);
	}
#endif

	Super::UnAllocation();
}

FTableRowProxy_PassiveSkillExtendInfo* FPassiveSkillProxy::GetTableRowProxy_PassiveSkillExtendInfo() const
{
	auto SceneProxyExtendInfoMapPtr = USceneProxyExtendInfoMap::GetInstance();
	auto DataTable = SceneProxyExtendInfoMapPtr->DataTable_Proxy_PassiveSkillExtendInfo.LoadSynchronous();

	auto SceneProxyExtendInfoPtr = DataTable->FindRow<FTableRowProxy_PassiveSkillExtendInfo>(*ProxyType.ToString(), TEXT("GetProxy"));
	return SceneProxyExtendInfoPtr;
}

FTableRowProxy_PropertyEntrys* FPassiveSkillProxy::GetMainPropertyEntry() const
{
	auto SceneProxyExtendInfoMapPtr = USceneProxyExtendInfoMap::GetInstance();
	auto DataTable = SceneProxyExtendInfoMapPtr->DataTable_PropertyEntrys.LoadSynchronous();

	// 
	TArray<FTableRowProxy_PropertyEntrys*>ResultAry;
	DataTable->GetAllRows(TEXT("GetProxy"), ResultAry);
	if (!ResultAry.IsEmpty())
	{
		return ResultAry[FMath::RandRange(0, ResultAry.Num() - 1)];
	}

	auto SceneProxyExtendInfoPtr = DataTable->FindRow<FTableRowProxy_PropertyEntrys>(*ProxyType.ToString(), TEXT("GetProxy"));
	return SceneProxyExtendInfoPtr;
}

TSubclassOf<USkill_Base> FPassiveSkillProxy::GetSkillClass() const
{
	return GetTableRowProxy_PassiveSkillExtendInfo()->SkillClass;
}

void FWeaponSkillProxy::SetAllocationCharacterProxy(const TSharedPtr < FCharacterProxy>& InAllocationCharacterProxyPtr, const FGameplayTag& InSocketTag)
{
	Super::SetAllocationCharacterProxy(InAllocationCharacterProxyPtr, InSocketTag);
}

bool FWeaponSkillProxy::Active()
{
#if UE_EDITOR || UE_SERVER
	auto ProxyCharacterPtr = GetOwnerCharacter();
	if (ProxyCharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		auto InGaInsPtr = Cast<USkill_WeaponActive_Base>(GetGAInst());
		if (!InGaInsPtr)
		{
			return false;
		}

		if (InGaInsPtr->IsActive())
		{
			InGaInsPtr->SetContinuePerform(true);
			return true;
		}

		FGameplayEventData Payload;
		if (
			GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Weapon) 
			)
		{
			auto GameplayAbilityTargetDashPtr = new FGameplayAbilityTargetData_WeaponActive_ActiveParam;
			GameplayAbilityTargetDashPtr->WeaponPtr = ActivedWeaponPtr;
			GameplayAbilityTargetDashPtr->bIsAutoContinue = true;
			Payload.TargetData.Add(GameplayAbilityTargetDashPtr);
		}

		auto ASCPtr = GetOwnerCharacter()->GetCharacterAbilitySystemComponent();

		return ASCPtr->TriggerAbilityFromGameplayEvent(
			InGaInsPtr->GetCurrentAbilitySpecHandle(),
			ASCPtr->AbilityActorInfo.Get(),
			GetProxyType(),
			&Payload,
			*ASCPtr
		);
	}
#endif

	return true;
}

void FWeaponSkillProxy::Cancel()
{
	auto InGAInsPtr = Cast<USkill_WeaponActive_Base>(GetGAInst());
	if (!InGAInsPtr)
	{
		return;
	}

	InGAInsPtr->SetContinuePerform(false);
}

void FWeaponSkillProxy::End()
{
	auto InGAInsPtr = Cast<USkill_WeaponActive_Base>(GetGAInst());
	if (!InGAInsPtr)
	{
		return;
	}

	InGAInsPtr->K2_CancelAbility();
}

FWeaponSkillProxy::FWeaponSkillProxy()
{
}

bool FWeaponSkillProxy::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Super::NetSerialize(Ar, Map, bOutSuccess);

	Ar << ActivedWeaponPtr;

	return true;
}

FTableRowProxy_WeaponSkillExtendInfo* FWeaponSkillProxy::GetTableRowProxy_WeaponSkillExtendInfo() const
{
	auto SceneProxyExtendInfoMapPtr = USceneProxyExtendInfoMap::GetInstance();
	auto DataTable = SceneProxyExtendInfoMapPtr->DataTable_Proxy_WeaponSkillExtendInfo.LoadSynchronous();

	auto SceneProxyExtendInfoPtr = DataTable->FindRow<FTableRowProxy_WeaponSkillExtendInfo>(*ProxyType.ToString(), TEXT("GetProxy"));
	return SceneProxyExtendInfoPtr;
}

TSubclassOf<USkill_Base> FWeaponSkillProxy::GetSkillClass() const
{
	return GetTableRowProxy_WeaponSkillExtendInfo()->SkillClass;
}

void FWeaponSkillProxy::RegisterSkill()
{
#if UE_EDITOR || UE_SERVER
	auto ProxyCharacterPtr = GetOwnerCharacter();
	if (ProxyCharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		FGameplayAbilityTargetData_SkillBase_RegisterParam* GameplayAbilityTargetDataPtr = nullptr;
		// 需要特殊参数的
		if (
			GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Weapon_Bow)
			)
		{
			GameplayAbilityTargetDataPtr = new FGameplayAbilityTargetData_Bow_RegisterParam;
		}
		else if (
			GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Weapon_FoldingFan)
			)
		{
			GameplayAbilityTargetDataPtr = new FGameplayAbilityTargetData_FoldingFan_RegisterParam;
		}
		else
		{
			GameplayAbilityTargetDataPtr = new FGameplayAbilityTargetData_SkillBase_RegisterParam;
		}
		GameplayAbilityTargetDataPtr->ProxyID = GetID();

		const auto InputID = FMath::RandHelper(std::numeric_limits<int32>::max());
		FGameplayAbilitySpec GameplayAbilitySpec(
			GetSkillClass(),
			Level,
			InputID
		);

		auto GameplayEventData = MakeShared<FGameplayEventData>();
		GameplayEventData->TargetData.Add(GameplayAbilityTargetDataPtr);

		auto AllocationCharacter = GetAllocationCharacterProxy().Pin()->ProxyCharacterPtr;

		AllocationCharacter->GetCharacterAbilitySystemComponent()->ReplicateEventData(
			InputID,
			*GameplayEventData
		);
		GameplayAbilitySpecHandle = AllocationCharacter->GetCharacterAbilitySystemComponent()->GiveAbility(GameplayAbilitySpec);
	}
#endif
}
