
#include "TalentAllocationComponent.h"

#include "CharacterBase.h"
#include "SceneElement.h"
#include "HoldingItemsComponent.h"
#include "CharacterAttributesComponent.h"
#include "CharacterAttibutes.h"

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
				SyncToHolding(Result, OldValue);
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
			SyncToHolding(Result, OldValue);
		}
		Result = *Iter;
	}

	return Result;
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
		{TEXT("Talent.WuXing"), EPointSkillType::kWuXing },
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
		TalentHelper.Type = EPointSkillType::kNuQi;
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

void UTalentAllocationComponent::SyncToHolding(const FTalentHelper& NewTalentHelper, const FTalentHelper& OldTalentHelper)
{
	switch (NewTalentHelper.PointType)
	{
	case EPointType::kSkill:
	{
		switch (std::get<EPointSkillType>(NewTalentHelper.Type))
		{
		case EPointSkillType::kNuQi:
		{
			auto CharacterPtr = GetOwner<ACharacterBase>();
			if (CharacterPtr)
			{
				auto& HoldItemComponent = CharacterPtr->GetHoldingItemsComponent()->GetHoldItemProperty();
				auto SkillUnitPtr = HoldItemComponent.FindUnit(ESkillUnitType::kHumanSkill_TalentSkill_NuQi);
				if (!SkillUnitPtr)
				{
					SkillUnitPtr = HoldItemComponent.AddUnit(ESkillUnitType::kHumanSkill_TalentSkill_NuQi);
				}
				if (SkillUnitPtr)
				{
					SkillUnitPtr->Level = NewTalentHelper.Level;
				}
			}
		}
		break;
		}
	}
	break;
	case EPointType::kProperty:
	{
		auto CharacterPtr = GetOwner<ACharacterBase>();
		if (CharacterPtr)
		{
			auto& CharacterAttributes = CharacterPtr->GetCharacterAttributesComponent()->GetCharacterAttributes();
			switch (std::get<EPointPropertyType>(NewTalentHelper.Type))
			{
			case EPointPropertyType::kLiDao:
			{
				CharacterAttributes.LiDao.AddCurrentValue((NewTalentHelper.Level - OldTalentHelper.Level) * BasePropertyOffset);
			}
			break;
			case EPointPropertyType::kGenGu:
			{
				CharacterAttributes.GenGu.AddCurrentValue((NewTalentHelper.Level - OldTalentHelper.Level) * BasePropertyOffset);
			}
			break;
			case EPointPropertyType::kShenFa:
			{
				CharacterAttributes.ShenFa.AddCurrentValue((NewTalentHelper.Level - OldTalentHelper.Level) * BasePropertyOffset);
			}
			break;
			case EPointPropertyType::kDongCha:
			{
				CharacterAttributes.DongCha.AddCurrentValue((NewTalentHelper.Level - OldTalentHelper.Level) * BasePropertyOffset);
			}
			break;
			case EPointPropertyType::kTianZi:
			{
				CharacterAttributes.TianZi.AddCurrentValue((NewTalentHelper.Level - OldTalentHelper.Level) * BasePropertyOffset);
			}
			break;
			}
		}
	}
	break;
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
