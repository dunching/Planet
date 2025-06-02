
#include "OnEffectedTawrgetCallback.h"

#include "ConversationComponent.h"
#include "TeamMatesHelperComponentBase.h"
#include "CharacterBase.h"

bool FOnEffectedTawrgetCallback::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Ar << InstigatorCharacterPtr;
	Ar << TargetCharacterPtr;
	Ar << ElementalType;
	Ar << bIsDeath;
	Ar << bIsEvade;
	Ar << bIsCritical;
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
