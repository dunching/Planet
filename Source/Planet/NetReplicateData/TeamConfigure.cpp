#include "TeamConfigure.h"

bool FTeamConfigure::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	if (Ar.IsSaving())
	{
		auto Num= CharactersAry.Num();
		Ar << Num;
		for (auto& Iter : CharactersAry)
		{
			Ar << Iter;
		}
	}
	else if (Ar.IsLoading())
	{
		int32 Num= 0;
		Ar << Num;
		CharactersAry.SetNum(Num);
		for (auto& Iter : CharactersAry)
		{
			Ar << Iter;
		}
	}

	return true;
}

bool FTeamConfigure::operator==(const FTeamConfigure& RightValue) const
{
	return CharactersAry ==  RightValue.CharactersAry;
}
