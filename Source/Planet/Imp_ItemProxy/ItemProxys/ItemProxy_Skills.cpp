#include "ItemProxy_Skills.h"

#include "AbilitySystemComponent.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/RichTextBlock.h"
#include "Kismet/KismetStringLibrary.h"

#include "CharacterBase.h"
#include "DataTableCollection.h"
#include "GameplayTagsLibrary.h"
#include "CharacterAttibutes.h"
#include "AllocationSkills.h"
#include "AssetRefMap.h"
#include "PropertyEntrys.h"
#include "Skill_Base.h"
#include "Skill_Active_Base.h"
#include "Skill_Active_Control.h"
#include "Skill_WeaponActive_Base.h"
#include "Skill_WeaponActive_PickAxe.h"
#include "Weapon_HandProtection.h"
#include "InventoryComponent.h"
#include "CharacterAbilitySystemComponent.h"
#include "PropertyEntryDescription.h"
#include "Skill_WeaponActive_Bow.h"
#include "Skill_WeaponActive_FoldingFan.h"
#include "ItemProxy_Character.h"
#include "MainHUDLayout.h"
#include "ModifyItemProxyStrategy.h"
#include "PropertyEntrySussystem.h"
#include "Tools.h"
#include "UIManagerSubSystem.h"

FSkillProxy::FSkillProxy() :
                           Super()
{
}

bool FSkillProxy::NetSerialize(
	FArchive& Ar,
	class UPackageMap* Map,
	bool& bOutSuccess
	)
{
	Super::NetSerialize(Ar, Map, bOutSuccess);
	NetSerialize_Allocationble(Ar, Map, bOutSuccess);

	Ar << Level;
	Ar << GameplayAbilitySpecHandle;

	return true;
}

void FSkillProxy::InitialProxy(
	const FGameplayTag& InProxyType
	)
{
	Super::InitialProxy(InProxyType);

	ProxyPtr = this;
}

TSet<EItemProxyInteractionType> FSkillProxy::GetInteractionsType() const
{
	return {EItemProxyInteractionType::kUpgrade};
}

void FSkillProxy::SetAllocationCharacterProxy(
	const TSharedPtr<FCharacterProxy>& InAllocationCharacterProxyPtr,
	const FGameplayTag& InSocketTag
	)
{
	IProxy_Allocationble::SetAllocationCharacterProxy(InAllocationCharacterProxyPtr, InSocketTag);
}

void FSkillProxy::UpdateByRemote(
	const TSharedPtr<FSkillProxy>& RemoteSPtr
	)
{
	Super::UpdateByRemote(RemoteSPtr);

	ProxyPtr = this;
	UpdateByRemote_Allocationble(RemoteSPtr);

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
	if (GetInventoryComponent()->GetNetMode() == NM_DedicatedServer)
	{
		// 尝试注册这个GA，如果AI未生成，则在生成时（SpwanCharacter）注册
		if (!GetAllocationCharacterID().IsValid())
		{
			return;
		}

		auto AllocationCharacter = GetAllocationCharacterProxy()->GetCharacterActor();
		// 确认是否生成了CharacterActor
		if (!AllocationCharacter.IsValid())
		{
			return;
		}

		auto GameplayAbilityTargetDataPtr = new FGameplayAbilityTargetData_RegisterParam_SkillBase;

		GameplayAbilityTargetDataPtr->ProxyID = GetID();

		const auto InputID = FMath::RandHelper(std::numeric_limits<int32>::max());
		FGameplayAbilitySpec GameplayAbilitySpec(
		                                         GetSkillClass(),
		                                         Level,
		                                         InputID
		                                        );

		FGameplayEventData GameplayEventData;
		GameplayEventData.TargetData.Add(GameplayAbilityTargetDataPtr);

		AllocationCharacter->GetCharacterAbilitySystemComponent()->ReplicateEventData(
			 InputID,
			 GameplayEventData
			);
		GameplayAbilitySpecHandle = AllocationCharacter->GetCharacterAbilitySystemComponent()->GiveAbility(
			 GameplayAbilitySpec
			);
	}
#endif
}

void FSkillProxy::UnRegisterSkill()
{
#if UE_EDITOR || UE_SERVER
	if (GetInventoryComponent()->GetNetMode() == NM_DedicatedServer)
	{
		if (GetAllocationCharacterProxy().IsValid())
		{
			auto AllocationCharacter = GetAllocationCharacterProxy()->GetCharacterActor();

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

TArray<USkill_Base*> FSkillProxy::GetGAInstAry() const
{
	TArray<USkill_Base*> ResultAry;
	auto ProxyCharacterPtr = GetAllocationCharacter();
	auto ASCPtr = ProxyCharacterPtr->GetCharacterAbilitySystemComponent();
	auto GameplayAbilitySpecPtr = ASCPtr->FindAbilitySpecFromHandle(GameplayAbilitySpecHandle);
	if (GameplayAbilitySpecPtr)
	{
		ResultAry.Add(Cast<USkill_Base>(GameplayAbilitySpecPtr->GetPrimaryInstance()));
	}

	return ResultAry;
}

USkill_Base* FSkillProxy::GetGAInst() const
{
	auto ProxyCharacterPtr = GetAllocationCharacter();
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

bool FActiveSkillProxy::NetSerialize(
	FArchive& Ar,
	class UPackageMap* Map,
	bool& bOutSuccess
	)
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
		InGAInsPtr->CanActivateAbility(
		                               InGAInsPtr->GetCurrentAbilitySpecHandle(),
		                               InGAInsPtr->GetCurrentActorInfo()
		                              )
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
	// 	if (ProxyCharacterPtr->GetNetMode() == NM_DedicatedServer)
	{
		auto InGAInsPtr = Cast<USkill_Active_Base>(GetGAInst());
		if (!InGAInsPtr)
		{
			return false;
		}

		auto ASCPtr = GetAllocationCharacter()->GetCharacterAbilitySystemComponent();

		// 需要特殊参数的
		if (
			GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Active_Control)
		)
		{
			if (InGAInsPtr->IsActive())
			{
				ASCPtr->SetContinuePerform_Server(
				                                  InGAInsPtr->GetCurrentAbilitySpecHandle(),
				                                  InGAInsPtr->GetCurrentActivationInfo(),
				                                  true
				                                 );
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
				ASCPtr->SetContinuePerform_Server(
				                                  InGAInsPtr->GetCurrentAbilitySpecHandle(),
				                                  InGAInsPtr->GetCurrentActivationInfo(),
				                                  true
				                                 );
				return true;
			}
			else
			{
				auto GameplayAbilityTargetPtr =
					new FGameplayAbilityTargetData_ActiveSkill_ActiveParam;

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
	auto InGAInsPtr = Cast<USkill_Active_Base>(GetGAInst());
	if (!InGAInsPtr)
	{
		return;
	}

	auto ASCPtr = GetAllocationCharacter()->GetCharacterAbilitySystemComponent();

	ASCPtr->SetContinuePerform_Server(
	                                  InGAInsPtr->GetCurrentAbilitySpecHandle(),
	                                  InGAInsPtr->GetCurrentActivationInfo(),
	                                  false
	                                 );
}

bool FActiveSkillProxy::GetRemainingCooldown(
	float& RemainingCooldown,
	float& RemainingCooldownPercent
	) const
{
	auto InTags = FGameplayTagContainer::EmptyContainer;

	InTags.AddTag(GetProxyType());
	InTags.AddTag(UGameplayTagsLibrary::GEData_CD);

	const auto GameplayEffectHandleAry = GetAllocationCharacter()->GetCharacterAbilitySystemComponent()->
	                                                               GetActiveEffectsWithAllTags(
		                                                                InTags
		                                                               );

	if (!GameplayEffectHandleAry.IsEmpty())
	{
		auto GameplayEffectPtr = GetAllocationCharacter()->GetCharacterAbilitySystemComponent()->
		                                                   GetActiveGameplayEffect(
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

bool FActiveSkillProxy::CheckNotInCooldown() const
{
	auto InTags = FGameplayTagContainer::EmptyContainer;

	InTags.AddTag(GetProxyType());
	InTags.AddTag(UGameplayTagsLibrary::GEData_CD);

	const auto GameplayEffectHandleAry = GetAllocationCharacter()->GetCharacterAbilitySystemComponent()->
	                                                               GetActiveEffectsWithAllTags(
		                                                                InTags
		                                                               );

	return GameplayEffectHandleAry.IsEmpty();
}

void FActiveSkillProxy::AddCooldownConsumeTime(
	float CDOffset
	)
{
	auto InTags = FGameplayTagContainer::EmptyContainer;

	InTags.AddTag(GetProxyType());
	InTags.AddTag(UGameplayTagsLibrary::GEData_CD);

	const auto GameplayEffectHandleAry = GetAllocationCharacter()->GetCharacterAbilitySystemComponent()->
	                                                               GetActiveEffectsWithAllTags(
		                                                                InTags
		                                                               );

	if (!GameplayEffectHandleAry.IsEmpty())
	{
		GetAllocationCharacter()->GetCharacterAbilitySystemComponent()->
		                          ModifyActiveEffectStartTime(
		                                                      GameplayEffectHandleAry[0],
		                                                      CDOffset
		                                                     );
	}
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

UItemProxy_Description_ActiveSkill* FActiveSkillProxy::GetTableRowProxy_ActiveSkillExtendInfo() const
{
	auto TableRowPtr = GetTableRowProxy();
	auto ItemProxy_Description_SkillPtr = Cast<UItemProxy_Description_ActiveSkill>(
		 TableRowPtr->ItemProxy_Description.LoadSynchronous()
		);
	return ItemProxy_Description_SkillPtr;
}

TSubclassOf<USkill_Base> FActiveSkillProxy::GetSkillClass() const
{
	return GetTableRowProxy_ActiveSkillExtendInfo()->SkillClass;
}

int32 FActiveSkillProxy::GetCount() const
{
	return -1;
}

UItemProxy_Description_PassiveSkill::UItemProxy_Description_PassiveSkill(
	const FObjectInitializer& ObjectInitializer
	):
	 Super(ObjectInitializer)
{
	GrowthAttributeMap =
	{
		{100, {1}},
		{200,},
		{300,},
		{400,},
		{500,},
		{600, {1, 2}},
		{700,},
		{800,},
		{900,},
		{1000,},
		{1100, {1, 2, 3}},
		{1200,},
		{1300,},
		{1400,},
		{1500,},
		{1600, {1,}},
		{1700,},
		{1800,},
		{1900,},
		{2000,},
		{2100, {1, 2,}},
		{2200,},
		{2300,},
		{2400,},
		{0, {1, 2, 3}},
	};
}

FPassiveSkillProxy::FPassiveSkillProxy()
{
}

void FPassiveSkillProxy::InitialProxy(
	const FGameplayTag& InProxyType
	)
{
	Super::InitialProxy(InProxyType);

	GenerationPropertyEntry();
}

void FPassiveSkillProxy::UpdateByRemote(
	const TSharedPtr<FPassiveSkillProxy>& RemoteSPtr
	)
{
	Super::UpdateByRemote(RemoteSPtr);
	UpdateByRemote_Allocationble(RemoteSPtr);

	GeneratedPropertyEntryAry = RemoteSPtr->GeneratedPropertyEntryAry;
	
	if (Level != RemoteSPtr->Level)
	{
		LevelChangedDelegate.ValueChanged(Level, RemoteSPtr->Level);
	}
	Level = RemoteSPtr->Level;

	if (Experience != RemoteSPtr->Experience)
	{
		ExperienceChangedDelegate.ValueChanged(Experience, RemoteSPtr->Experience);
	}
	Experience = RemoteSPtr->Experience;
}

bool FPassiveSkillProxy::NetSerialize(
	FArchive& Ar,
	class UPackageMap* Map,
	bool& bOutSuccess
	)
{
	Super::NetSerialize(Ar, Map, bOutSuccess);
	NetSerialize_Allocationble(Ar, Map, bOutSuccess);

	if (Ar.IsSaving())
	{
		int32 size = GeneratedPropertyEntryAry.size();
		Ar << size;

		for (auto Iter : GeneratedPropertyEntryAry)
		{
			int32 Value = Iter.first;
			Ar << Value;

			Iter.second.NetSerialize(Ar, Map, bOutSuccess);
		}
	}
	else if (Ar.IsLoading())
	{
		GeneratedPropertyEntryAry.clear();

		int32 size = 0;
		Ar << size;

		for (int32 Index = 0; Index < size; Index++)
		{
			int32 Value = 0;
			Ar << Value;

			FGeneratedPropertyEntryInfo GeneratedPropertyEntryInfo;
			GeneratedPropertyEntryInfo.NetSerialize(Ar, Map, bOutSuccess);

			GeneratedPropertyEntryAry.emplace(Value, GeneratedPropertyEntryInfo);
		}
	}

	Ar << Level;
	Ar << Experience;

	return true;
}

void FPassiveSkillProxy::ProcessProxyInteraction(
	EItemProxyInteractionType ItemProxyInteractionType
	)
{
	switch (ItemProxyInteractionType)
	{
	case EItemProxyInteractionType::kDiscard:
		break;
	case EItemProxyInteractionType::kBreakDown:
		break;
	case EItemProxyInteractionType::kUpgrade:
		{
			auto UIPtr = CreateWidget<UUpgradeBoder>(
			                                         GEngine->GetFirstLocalPlayerController(GetWorldImp()),
			                                         UAssetRefMap::GetInstance()->UpgradeBoderClass
			                                        );

			UIPtr->BindData(GetInventoryComponentBase()->FindProxy<FModifyItemProxyStrategy_PassveSkill>(GetID()));

			UUIManagerSubSystem::GetInstance()->GetMainHUDLayout()->DisplayWidgetInOtherCanvas(
				 UIPtr
				);
		}
		break;
	}
}

void FPassiveSkillProxy::Allocation()
{
	Super::Allocation();

#if UE_EDITOR || UE_SERVER
	if (GetInventoryComponent()->GetNetMode() == NM_DedicatedServer)
	{
		auto CharacterSPtr = GetAllocationCharacter();
		if (!CharacterSPtr)
		{
			return;
		}

		UPlanetAbilitySystemComponent* GASPtr = nullptr;
		GASPtr = CharacterSPtr->GetAbilitySystemComponent();
		if (!GASPtr)
		{
			return;
		}

		for (const auto& Iter : GeneratedPropertyEntryAry)
		{
			auto SpecHandle = GASPtr->MakeOutgoingSpec(
			                                           UAssetRefMap::GetInstance()->OnceGEClass,
			                                           1,
			                                           GASPtr->MakeEffectContext()
			                                          );

			SpecHandle.Data.Get()->AddDynamicAssetTag(
			                                          UGameplayTagsLibrary::GEData_ModifyType_Temporary_Data_Addtive
			                                         );

			SpecHandle.Data.Get()->AddDynamicAssetTag(Iter.second.PropertyTag);
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(
			                                               GetProxyType(),
			                                               Iter.second.Value
			                                              );

			GASPtr->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
#endif
}

void FPassiveSkillProxy::UnAllocation()
{
#if UE_EDITOR || UE_SERVER
	if (GetInventoryComponent()->GetNetMode() == NM_DedicatedServer)
	{
		auto CharacterSPtr = GetAllocationCharacter();
		if (!CharacterSPtr)
		{
			return;
		}

		UPlanetAbilitySystemComponent* GASPtr = nullptr;
		GASPtr = CharacterSPtr->GetAbilitySystemComponent();
		if (!GASPtr)
		{
			return;
		}

		for (const auto& Iter : GeneratedPropertyEntryAry)
		{
			auto SpecHandle = GASPtr->MakeOutgoingSpec(
			                                           UAssetRefMap::GetInstance()->OnceGEClass,
			                                           1,
			                                           GASPtr->MakeEffectContext()
			                                          );

			SpecHandle.Data.Get()->AddDynamicAssetTag(
			                                          UGameplayTagsLibrary::GEData_ModifyType_Temporary_Data_Override
			                                         );

			SpecHandle.Data.Get()->AddDynamicAssetTag(Iter.second.PropertyTag);
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(
			                                               GetProxyType(),
			                                               0
			                                              );

			GASPtr->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
#endif

	Super::UnAllocation();
}

void FPassiveSkillProxy::GenerationPropertyEntry()
{
	// 已根据等级生成的词条索引
	uint8 NeedGenerationLevel = 0;
	for (const auto& Iter : GeneratedPropertyEntryAry)
	{
		NeedGenerationLevel = Iter.first;
	}

	auto TableRowProxy_PassiveSkillExtendInfoPtr = GetTableRowProxy_PassiveSkillExtendInfo();
	if (!TableRowProxy_PassiveSkillExtendInfoPtr)
	{
		return;
	}

	if (Level > TableRowProxy_PassiveSkillExtendInfoPtr->GrowthAttributeMap.Num())
	{
		return;
	}

	for (; NeedGenerationLevel <= Level; NeedGenerationLevel++)
	{
		if (!TableRowProxy_PassiveSkillExtendInfoPtr->GrowthAttributeMap.IsValidIndex(NeedGenerationLevel))
		{
			continue;
		}

		// 
		const auto GrowthAttribute = TableRowProxy_PassiveSkillExtendInfoPtr->GrowthAttributeMap[NeedGenerationLevel];
		const auto Ary = UPropertyEntrySussystem::GetInstance()->GenerationPropertyEntry(
			 GrowthAttribute.
			 GeneratiblePropertyEntryAry
			);

		for (const auto& Iter : Ary)
		{
			GeneratedPropertyEntryAry.emplace(NeedGenerationLevel, Iter);
		}
	}
}

void UItemDecription_Skill_PassiveSkill::SetUIStyle()
{
	if (ProxySPtr)
	{
		{
			if (Title)
			{
				Title->SetText(FText::FromString(ProxySPtr->GetProxyName()));
			}
		}
		auto SkillProxySPtr = DynamicCastSharedPtr<FSkillProxyType>(ProxySPtr);
		if (!SkillProxySPtr)
		{
			return;
		}
	}
	else if (ProxyType.IsValid())
	{
		{
			if (Title)
			{
				Title->SetText(FText::FromString(ItemProxy_Description->ProxyName));
			}
		}
	}
	
	
	auto ItemProxy_DescriptionPtr = ItemProxy_Description.LoadSynchronous();
	if (ItemProxy_DescriptionPtr && !ItemProxy_DescriptionPtr->DecriptionText.IsEmpty())
	{
		FString Text = ItemProxy_DescriptionPtr->DecriptionText[0];
		for (const auto& Iter : ItemProxy_DescriptionPtr->Values)
		{
			if (Iter.Value.PerLevelValue.IsEmpty())
			{
				continue;
			}

			Text = Text.Replace(*Iter.Key, *UKismetStringLibrary::Conv_IntToString(Iter.Value.PerLevelValue[0]));
		}

		if (DescriptionText)
		{
			DescriptionText->SetText(FText::FromString(Text));
		}
	}
}

UItemProxy_Description_PassiveSkill* FPassiveSkillProxy::GetTableRowProxy_PassiveSkillExtendInfo() const
{
	auto TableRowPtr = GetTableRowProxy();
	auto ItemProxy_Description_SkillPtr = Cast<UItemProxy_Description_PassiveSkill>(
		 TableRowPtr->ItemProxy_Description.LoadSynchronous()
		);
	return ItemProxy_Description_SkillPtr;
}

TSubclassOf<USkill_Base> FPassiveSkillProxy::GetSkillClass() const
{
	return GetTableRowProxy_PassiveSkillExtendInfo()->SkillClass;
}

int32 FPassiveSkillProxy::GetCount() const
{
	return -1;
}

bool FPassiveSkillProxy::GetRemainingCooldown(
	float& RemainingCooldown,
	float& RemainingCooldownPercent
	) const
{
	auto InTags = FGameplayTagContainer::EmptyContainer;

	InTags.AddTag(GetProxyType());
	InTags.AddTag(UGameplayTagsLibrary::GEData_CD);

	const auto GameplayEffectHandleAry = GetAllocationCharacter()->GetCharacterAbilitySystemComponent()->
	                                                               GetActiveEffectsWithAllTags(
		                                                                InTags
		                                                               );

	if (!GameplayEffectHandleAry.IsEmpty())
	{
		auto GameplayEffectPtr = GetAllocationCharacter()->GetCharacterAbilitySystemComponent()->
		                                                   GetActiveGameplayEffect(
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

bool FPassiveSkillProxy::CheckNotInCooldown() const
{
	auto InTags = FGameplayTagContainer::EmptyContainer;

	InTags.AddTag(GetProxyType());
	InTags.AddTag(UGameplayTagsLibrary::GEData_CD);

	const auto GameplayEffectHandleAry = GetAllocationCharacter()->GetCharacterAbilitySystemComponent()->
	                                                               GetActiveEffectsWithAllTags(
		                                                                InTags
		                                                               );

	return GameplayEffectHandleAry.IsEmpty();
}

void FPassiveSkillProxy::AddCooldownConsumeTime(
	float CDOffset
	)
{
	auto InTags = FGameplayTagContainer::EmptyContainer;

	InTags.AddTag(GetProxyType());
	InTags.AddTag(UGameplayTagsLibrary::GEData_CD);

	const auto GameplayEffectHandleAry = GetAllocationCharacter()->GetCharacterAbilitySystemComponent()->
	                                                               GetActiveEffectsWithAllTags(
		                                                                InTags
		                                                               );

	if (!GameplayEffectHandleAry.IsEmpty())
	{
		GetAllocationCharacter()->GetCharacterAbilitySystemComponent()->
		                          ModifyActiveEffectStartTime(
		                                                      GameplayEffectHandleAry[0],
		                                                      CDOffset
		                                                     );
	}
}

void FPassiveSkillProxy::FreshUniqueCooldownTime()
{
}

void FPassiveSkillProxy::ApplyCooldown()
{
}

void FPassiveSkillProxy::OffsetCooldownTime()
{
}

void FPassiveSkillProxy::AddExperience(
	uint32 Value
	)
{
	const auto CharacterGrowthAttributeAry = GetTableRowProxy_PassiveSkillExtendInfo()->GrowthAttributeMap;

	if (CharacterGrowthAttributeAry.Num() <= Level)
	{
		//满级了
		return;
	}

	const auto OldExperience = Experience;

	Experience += Value;

	int32 LevelExperience = 0;

	int32 NewLevelExperience = 0;

	ON_SCOPE_EXIT
	{
#if UE_EDITOR || UE_SERVER
		if (GetInventoryComponent()->GetNetMode() == NM_DedicatedServer)
		{
			GetInventoryComponent()->UpdateProxy(GetID());
		}
#endif

		ExperienceChangedDelegate.ValueChanged(OldExperience, Experience);

		if (LevelExperience > 0 && NewLevelExperience > 0)
		{
			LevelExperienceChangedDelegate.ValueChanged(LevelExperience, NewLevelExperience);
		}
	};

	for (; Level > 0 && Level < CharacterGrowthAttributeAry.Num();)
	{
		const auto& CurrentLevelAttribute = CharacterGrowthAttributeAry[Level - 1];

		// 当前等级升级所需经验
		LevelExperience = CurrentLevelAttribute.LevelExperience;

		if (Experience < LevelExperience)
		{
			return;
		}

		// 调整数据
		const auto OldLevel = Level;
		Level++;

		LevelChangedDelegate.ValueChanged(OldLevel, Level);

		if (Level < CharacterGrowthAttributeAry.Num())
		{
			Experience -= LevelExperience;

			LevelExperience = CurrentLevelAttribute.LevelExperience;

			NewLevelExperience = CharacterGrowthAttributeAry[Level].LevelExperience;
		}
		else
		{
			Experience = CurrentLevelAttribute.LevelExperience;

			LevelExperience = CurrentLevelAttribute.LevelExperience;

			NewLevelExperience = CurrentLevelAttribute.LevelExperience;
		}
	}

	GenerationPropertyEntry();
}

uint8 FPassiveSkillProxy::GetLevel() const
{
	return Level;
}

uint8 FPassiveSkillProxy::GetMaxLevel() const
{
	const auto CharacterGrowthAttributeAry = GetTableRowProxy_PassiveSkillExtendInfo()->GrowthAttributeMap;

	return CharacterGrowthAttributeAry.Num();
}

int32 FPassiveSkillProxy::GetCurrentExperience() const
{
	return Experience;
}

int32 FPassiveSkillProxy::GetCurrentLevelExperience() const
{
	return GetLevelExperience(Level);
}

int32 FPassiveSkillProxy::GetLevelExperience(
	int32 InLevel
	) const
{
	const auto CharacterGrowthAttributeAry = GetTableRowProxy_PassiveSkillExtendInfo()->GrowthAttributeMap;

	if (CharacterGrowthAttributeAry.Num() < 0)
	{
		return -1;
	}

	const auto TargetLevel = InLevel - 1;

	if (CharacterGrowthAttributeAry.IsValidIndex(TargetLevel))
	{
		return CharacterGrowthAttributeAry[TargetLevel].LevelExperience;
	}
	else
	{
		checkNoEntry();
	}

	return 0;
}

bool FWeaponSkillProxy::Active()
{
#if UE_EDITOR || UE_SERVER
	if (GetInventoryComponent()->GetNetMode() == NM_DedicatedServer)
	{
		auto InGAInsPtr = Cast<USkill_WeaponActive_Base>(GetGAInst());
		if (!InGAInsPtr)
		{
			return false;
		}

		if (InGAInsPtr->IsActive())
		{
			auto ASCPtr = GetAllocationCharacter()->GetCharacterAbilitySystemComponent();

			ASCPtr->SetContinuePerform_Server(
			                                  InGAInsPtr->GetCurrentAbilitySpecHandle(),
			                                  InGAInsPtr->GetCurrentActivationInfo(),
			                                  true
			                                 );
			return true;
		}

		FGameplayEventData Payload;
		if (
			GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Weapon)
		)
		{
			auto GameplayAbilityTargetDashPtr = new FGameplayAbilityTargetData_ActiveParam_WeaponActive;
			GameplayAbilityTargetDashPtr->WeaponPtr = ActivedWeaponPtr;
			GameplayAbilityTargetDashPtr->bIsAutoContinue = true;
			Payload.TargetData.Add(GameplayAbilityTargetDashPtr);
		}

		auto ASCPtr = GetAllocationCharacter()->GetCharacterAbilitySystemComponent();

		return ASCPtr->TriggerAbilityFromGameplayEvent(
		                                               InGAInsPtr->GetCurrentAbilitySpecHandle(),
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

	auto ASCPtr = GetAllocationCharacter()->GetCharacterAbilitySystemComponent();

	ASCPtr->SetContinuePerform_Server(
	                                  InGAInsPtr->GetCurrentAbilitySpecHandle(),
	                                  InGAInsPtr->GetCurrentActivationInfo(),
	                                  false
	                                 );
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

bool FWeaponSkillProxy::NetSerialize(
	FArchive& Ar,
	class UPackageMap* Map,
	bool& bOutSuccess
	)
{
	Super::NetSerialize(Ar, Map, bOutSuccess);

	Ar << ActivedWeaponPtr;

	return true;
}

UItemProxy_Description_WeaponSkill* FWeaponSkillProxy::GetTableRowProxy_WeaponSkillExtendInfo() const
{
	auto TableRowPtr = GetTableRowProxy();
	auto ItemProxy_Description_SkillPtr = Cast<UItemProxy_Description_WeaponSkill>(
		 TableRowPtr->ItemProxy_Description.LoadSynchronous()
		);
	return ItemProxy_Description_SkillPtr;
}

TSubclassOf<USkill_Base> FWeaponSkillProxy::GetSkillClass() const
{
	auto TableRowPtr = GetTableRowProxy();
	auto ItemProxy_Description_SkillPtr = Cast<UItemProxy_Description_WeaponSkill>(
		 TableRowPtr->ItemProxy_Description.LoadSynchronous()
		);
	return GetTableRowProxy_WeaponSkillExtendInfo()->SkillClass;
}

void FWeaponSkillProxy::RegisterSkill()
{
#if UE_EDITOR || UE_SERVER
	if (GetInventoryComponent()->GetNetMode() == NM_DedicatedServer)
	{
		// 尝试注册这个GA，如果AI未生成，则在生成时（SpwanCharacter）注册
		if (!GetAllocationCharacterID().IsValid())
		{
			return;
		}

		auto AllocationCharacter = GetAllocationCharacterProxy()->GetCharacterActor();
		// 确认是否生成了CharacterActor
		if (!AllocationCharacter.IsValid())
		{
			return;
		}

		FGameplayAbilityTargetData_RegisterParam_SkillBase* GameplayAbilityTargetDataPtr = nullptr;
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
		else if (
			GetProxyType().MatchesTag(UGameplayTagsLibrary::Proxy_Skill_Weapon_Axe)
		)
		{
			GameplayAbilityTargetDataPtr = new FGameplayAbilityTargetData_Axe_RegisterParam;
		}
		else
		{
			GameplayAbilityTargetDataPtr = new FGameplayAbilityTargetData_RegisterParam_SkillBase;
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

		AllocationCharacter->GetCharacterAbilitySystemComponent()->ReplicateEventData(
			 InputID,
			 *GameplayEventData
			);

		GameplayAbilitySpecHandle = AllocationCharacter->GetCharacterAbilitySystemComponent()->GiveAbility(
			 GameplayAbilitySpec
			);
	}
#endif
}
