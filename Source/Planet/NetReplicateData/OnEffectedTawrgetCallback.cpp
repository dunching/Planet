
#include "OnEffectedTawrgetCallback.h"

#include "ConversationComponent.h"
#include "TeamMatesHelperComponent.h"
#include "CharacterBase.h"

bool FOnEffectedTawrgetCallback::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Ar << InstigatorCharacterPtr;
	Ar << TargetCharacterPtr;
	Ar << bIsDeath;

	return true;
}
