#include "TeamConfigure.h"

FTeamConfigure::FTeamConfigure()
{
}

bool FTeamConfigure::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	if (Ar.IsSaving())
	{
		auto TeamNum = TeamConfigureAry.Num();
		Ar << TeamNum;
		for (auto& Iter : TeamConfigureAry)
		{
			auto TeammateNum =Iter.Num();
			Ar << TeammateNum;
			for (auto& SecondIter : Iter)
			{
				Ar << SecondIter;
			}
		}
	}
	else if (Ar.IsLoading())
	{
		int32 TeamNum = 0;
		Ar << TeamNum;
		for (int32 Index = 0;Index < TeamNum; Index++)
		{
			int32 TeammateNum = 0;
			Ar << TeammateNum;

			auto &TeammateAry = TeamConfigureAry.AddZeroed_GetRef();
			for (int32 SecondIndex = 0;SecondIndex < TeammateNum; SecondIndex++)
			{
				auto &TeammateRef = TeammateAry.AddZeroed_GetRef();
				Ar << TeammateRef;
			}
		}
	}

	return true;
}

bool FTeamConfigure::operator==(const FTeamConfigure& RightValue) const
{
	return TeamConfigureAry == RightValue.TeamConfigureAry;
}

void FTeamConfigure::UpdateTeammateConfig(const FTeammate&Teammate)
{
	if (Teammate.CharacterProxyID.IsValid())
	{
		if (TeamConfigureAry.IsValidIndex(Teammate.TeamIndex))
		{
			if (TeamConfigureAry[Teammate.TeamIndex].IsValidIndex(Teammate.IndexInTheTeam))
			{
				TeamConfigureAry[Teammate.TeamIndex][Teammate.IndexInTheTeam] = Teammate.CharacterProxyID;
			}
			else
			{
				TeamConfigureAry[Teammate.TeamIndex].SetNumZeroed(Teammate.IndexInTheTeam + 1);
				TeamConfigureAry[Teammate.TeamIndex][Teammate.IndexInTheTeam] = Teammate.CharacterProxyID;
			}
		}
		else
		{
			TeamConfigureAry.SetNumZeroed(Teammate.TeamIndex + 1);
			TeamConfigureAry[Teammate.TeamIndex].SetNumZeroed(Teammate.IndexInTheTeam + 1);
			TeamConfigureAry[Teammate.TeamIndex][Teammate.IndexInTheTeam] = Teammate.CharacterProxyID;
		}
	}
	else
	{
		if (TeamConfigureAry.IsValidIndex(Teammate.TeamIndex))
		{
			if (TeamConfigureAry[Teammate.TeamIndex].IsValidIndex(Teammate.IndexInTheTeam))
			{
				TeamConfigureAry[Teammate.TeamIndex][Teammate.IndexInTheTeam] = Teammate.CharacterProxyID;
			}
		}
	}

	Check();
}

TArray<TArray<FGuid>> FTeamConfigure::GetCharactersAry() const
{
	return TeamConfigureAry;
}

void FTeamConfigure::Check()
{
}
