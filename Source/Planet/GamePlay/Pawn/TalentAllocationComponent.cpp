
#include "TalentAllocationComponent.h"

#include "CharacterBase.h"
#include "SceneElement.h"
#include "HoldingItemsComponent.h"
#include "CharacterAttributesComponent.h"
#include "CharacterAttibutes.h"
#include "UnitProxyProcessComponent.h"
#include "AssetRefMap.h"
#include "GameplayTagsSubSystem.h"
#include "SceneUnitContainer.h"
#include "PlanetControllerInterface.h"
#include "BaseFeatureComponent.h"

FName UTalentAllocationComponent::ComponentName = TEXT("TalentAllocationComponent");

FTalentHelper UTalentAllocationComponent::AddCheck(FTalentHelper& TalentHelper)
{
	FTalentHelper Result;
	Result.PointType = EPointType::kNone;

	auto Iter = GetCheck(TalentHelper);
	if (Iter)
	{
		if (GetUsedTalentPointNum() < GetTotalTalentPointNum())
		{
			if (Iter->Level < Iter->TotalLevel)
			{
				auto OldValue = *Iter;
				Iter->Level++;
				Result = *Iter;

				CalculorUsedTalentPointNum();
			}
		}
		Result = *Iter;
	}

	return Result;
}

FTalentHelper UTalentAllocationComponent::SubCheck(FTalentHelper& TalentHelper)
{
	FTalentHelper Result;
	Result.PointType = EPointType::kNone;

	auto Iter = GetCheck(TalentHelper);
	if (Iter)
	{
		if (Iter->Level > 0)
		{
			auto OldValue = *Iter;
			Iter->Level--;
			Result = *Iter;

			CalculorUsedTalentPointNum();
		}
		Result = *Iter;
	}

	return Result;
}

void UTalentAllocationComponent::Clear(FTalentHelper& TalentHelper)
{
	FTalentHelper * Iter = GetCheck(TalentHelper);
	if (Iter)
	{
		Iter->Level = 0;
	}
}

FTalentHelper* UTalentAllocationComponent::GetCheck(FTalentHelper& TalentHelper)
{
	return TalentMap.Find(TalentHelper.IconSocket);
}

int32 UTalentAllocationComponent::GetTotalTalentPointNum() const
{
	return TotalTalentPointNum;
}

int32 UTalentAllocationComponent::GetUsedTalentPointNum() const
{
	return UsedTalentPointNum;
}

void UTalentAllocationComponent::BeginPlay()
{
	Super::BeginPlay();

	InitialTalentData();

	CalculorUsedTalentPointNum();
}

void UTalentAllocationComponent::InitialTalentData()
{
	struct FSkillHelper
	{
		FName Text;
		EPointSkillType PointSkillType;
	};
	TArray<FSkillHelper>SkillAry
	{
		{TEXT("Talent.NuQi"), EPointSkillType::kNuQi },
		{TEXT("Talent.YinYang"), EPointSkillType::kYinYang },
		{TEXT("Talent.DuXing"), EPointSkillType::kDuXing },
		{TEXT("Talent.FaLi"), EPointSkillType::kFaLi },
		{TEXT("Talent.GongMing"), EPointSkillType::kGongMing },
	};
	for (auto Iter : SkillAry)
	{
		FGameplayTag GameplayTag = FGameplayTag::RequestGameplayTag(Iter.Text);

		FTalentHelper TalentHelper;

		TalentHelper.IconSocket = GameplayTag;
		TalentHelper.PointType = EPointType::kSkill;
		TalentHelper.Type = Iter.PointSkillType;
		TalentHelper.Level = 0;
		TalentHelper.TotalLevel = 3;

		TalentMap.Add(GameplayTag, TalentHelper);
	}

	struct FPropertyHelper
	{
		FName Text;
		EPointPropertyType PointPropertyType;
	};
	TArray<FPropertyHelper>PropertyAry
	{
		{TEXT("Talent.Level1.LiDao"), EPointPropertyType::kLiDao },
		{TEXT("Talent.Level1.GenGu"), EPointPropertyType::kGenGu },
		{TEXT("Talent.Level1.ShenFa"), EPointPropertyType::kShenFa },
		{TEXT("Talent.Level1.DongCha"), EPointPropertyType::kDongCha },
		{TEXT("Talent.Level1.TianZi"), EPointPropertyType::kTianZi },

		{TEXT("Talent.Level2.LiDao"), EPointPropertyType::kLiDao },
		{TEXT("Talent.Level2.GenGu"), EPointPropertyType::kGenGu },
		{TEXT("Talent.Level2.ShenFa"), EPointPropertyType::kShenFa },
		{TEXT("Talent.Level2.DongCha"), EPointPropertyType::kDongCha },
		{TEXT("Talent.Level2.TianZi"), EPointPropertyType::kTianZi },
	};
	for (auto Iter : PropertyAry)
	{
		FGameplayTag GameplayTag = FGameplayTag::RequestGameplayTag(Iter.Text);

		FTalentHelper TalentHelper;

		TalentHelper.IconSocket = GameplayTag;
		TalentHelper.PointType = EPointType::kProperty;
		TalentHelper.Type = Iter.PointPropertyType;
		TalentHelper.Level = 0;
		TalentHelper.TotalLevel = 3;

		TalentMap.Add(GameplayTag, TalentHelper);
	}
}

void UTalentAllocationComponent::SyncToHolding()
{
	for (const auto& Iter : TalentMap)
	{
		switch (Iter.Value.PointType)
		{
		case EPointType::kSkill:
		{
			if (std::get<EPointSkillType>(Iter.Value.Type) == PreviousSkillType)
			{
				continue;
			}
			else
			{
				auto CharacterPtr = GetOwner<FOwnerType>();
				if (CharacterPtr)
				{
// 					TMap<FGameplayTag, TSharedPtr<FSkillSocketInfo>> SkillsMap;
// 					auto HoldItemComponent = CharacterPtr->GetHoldingItemsComponent()->GetSceneUnitContainer();
// 					switch (std::get<EPointSkillType>(Iter.Value.Type))
// 					{
// 					case EPointSkillType::kNuQi:
// 					{
// 						auto SkillUnitPtr = HoldItemComponent->FindUnit_Skill(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Talent_NuQi);
// 						if (!SkillUnitPtr)
// 						{
// 							SkillUnitPtr = HoldItemComponent->AddUnit_Skill(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Talent_NuQi);
// 						}
// 						if (SkillUnitPtr)
// 						{
// 							SkillUnitPtr->Level = Iter.Value.Level;
// 						}
// 
// 						TSharedPtr<FSkillSocketInfo >SkillsSocketInfo = MakeShared<FSkillSocketInfo>();
// 
// 						SkillsSocketInfo->SkillSocket = UGameplayTagsSubSystem::GetInstance()->TalentSocket;
// 						SkillsSocketInfo->SkillUnitPtr = SkillUnitPtr;
// 						SkillsSocketInfo->Key = EKeys::Invalid;
// 
// 						SkillsMap.Add(SkillsSocketInfo->SkillSocket, SkillsSocketInfo);
// 					}
// 					break;
// 					case EPointSkillType::kYinYang:
// 					{
// 						auto SkillUnitPtr = HoldItemComponent->FindUnit_Skill(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Talent_YinYang);
// 						if (!SkillUnitPtr)
// 						{
// 							SkillUnitPtr = HoldItemComponent->AddUnit_Skill(UGameplayTagsSubSystem::GetInstance()->Unit_Skill_Talent_YinYang);
// 						}
// 						if (SkillUnitPtr)
// 						{
// 							SkillUnitPtr->Level = Iter.Value.Level;
// 						}
// 
// 						TSharedPtr<FSkillSocketInfo >SkillsSocketInfo = MakeShared<FSkillSocketInfo>();
// 
// 						SkillsSocketInfo->SkillSocket = UGameplayTagsSubSystem::GetInstance()->TalentSocket;
// 						SkillsSocketInfo->SkillUnitPtr = SkillUnitPtr;
// 						SkillsSocketInfo->Key = EKeys::Invalid;
// 
// 						SkillsMap.Add(SkillsSocketInfo->SkillSocket, SkillsSocketInfo);
// 					}
// 					break;
// 					}
// 					auto EICPtr = CharacterPtr->GetInteractiveSkillComponent();
// 					EICPtr->RegisterMultiGAs(SkillsMap);
				}
			}
		}
		break;
		case EPointType::kProperty:
		{
			auto CharacterPtr = GetOwner<FOwnerType>();
			if (CharacterPtr)
			{
				const auto DataSource = UGameplayTagsSubSystem::GetInstance()->DataSource_Regular;
				TMap<ECharacterPropertyType, FBaseProperty> ModifyPropertyMap;
				
				switch (std::get<EPointPropertyType>(Iter.Value.Type))
				{
				case EPointPropertyType::kLiDao:
				{
					ModifyPropertyMap.Add(ECharacterPropertyType::LiDao, Iter.Value.Level);
				}
				break;
				case EPointPropertyType::kGenGu:
				{
				}
				break;
				case EPointPropertyType::kShenFa:
				{
				}
				break;
				case EPointPropertyType::kDongCha:
				{
				}
				break;
				case EPointPropertyType::kTianZi:
				{
				}
				break;
				}
				
				CharacterPtr->GetBaseFeatureComponent()->SendEvent2Self(ModifyPropertyMap, UGameplayTagsSubSystem::GetInstance()->DataSource_Regular);
			}
		}
		break;
		}
	}
}

void UTalentAllocationComponent::CalculorUsedTalentPointNum()
{
	const auto OldValue = UsedTalentPointNum;

	UsedTalentPointNum = 0;
	for (const auto& Iter : TalentMap)
	{
		UsedTalentPointNum += Iter.Value.Level;
	}

	CallbackContainerHelper.ValueChanged(OldValue, UsedTalentPointNum);
}

