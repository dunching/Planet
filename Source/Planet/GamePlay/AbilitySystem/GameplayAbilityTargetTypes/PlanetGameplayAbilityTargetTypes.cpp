
#include "PlanetGameplayAbilityTargetTypes.h"

inline bool FGameplayAbilityTargetData_MyActorArray::NetSerialize(
	FArchive& Ar,
	class UPackageMap* Map,
	bool& bOutSuccess
	)
{
	HitResult.NetSerialize(Ar, Map, bOutSuccess);

	return true;
}
