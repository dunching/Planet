
#include "OnEffectedTawrgetCallback.h"

#include "ConversationComponent.h"
#include "TeamMatesHelperComponent.h"
#include "CharacterBase.h"

bool FOnEffectedTawrgetCallback::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Ar << InstigatorCharacterPtr;
	Ar << TargetCharacterPtr;
	Ar << bIsDeath;
	if (!AllAssetTags.NetSerialize(Ar, Map, bOutSuccess))
	{
		return false;
	}
	Ar << SetByCallerTagMagnitudes;
	Ar << Damage;
	Ar << TherapeuticalDose;

	bOutSuccess = true;
	return true;
}
