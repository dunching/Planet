#include "TalentUnit.h"


bool FTalentHelper::operator==(const FTalentHelper& RightValue) const
{
	if (PointType == RightValue.PointType)
	{
		switch (PointType)
		{
		case EPointType::kSkill:
		{
			return std::get<EPointSkillType>(Type) == std::get<EPointSkillType>(RightValue.Type);
		}
		break;
		case EPointType::kProperty:
		{
			return std::get<EPointPropertyType>(Type) == std::get<EPointPropertyType>(RightValue.Type);
		}
		break;
		}
	}

	return false;
}

