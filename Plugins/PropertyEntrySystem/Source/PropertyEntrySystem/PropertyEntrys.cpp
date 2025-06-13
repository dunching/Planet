

#include "PropertyEntrys.h"

bool FGeneratedPropertyEntryInfo::NetSerialize(
	FArchive& Ar,
	class UPackageMap* Map,
	bool& bOutSuccess
	)
{
	Ar << PropertyTag;
	Ar << Value;
	Ar << Percent;
	Ar << Level;
	Ar << bIsValue;
	
	return bOutSuccess;
}
