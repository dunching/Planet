
#include "AllocationSkills.h"

#include "AssetRefMap.h"
#include "Planet.h"
#include "CharacterBase.h"
#include "CharacterAttributesComponent.h"
#include "PlanetControllerInterface.h"
#include "HumanCharacter.h"
#include "SceneUnitExtendInfo.h"
#include "GameplayTagsSubSystem.h"
#include "SceneUnitContainer.h"

bool FAllocationSkills_FASI::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
{
	return true;
}

void FAllocationSkills_FASI::AddItem(const TSharedPtr<FBasicProxy>& ProxySPtr)
{

}

void FAllocationSkills_FASI::UpdateItem(const TSharedPtr<FBasicProxy>& ProxySPtr)
{

}

bool FSocket_FASI::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	if (Ar.IsSaving())
	{
		auto KeyStr = Key.ToString();
		Ar << KeyStr;
		Ar << Socket;

	}
	else if (Ar.IsLoading())
	{
		FString KeyStr;
		Ar << KeyStr;
		Key = FKey(*KeyStr);

		Ar << Socket;
	}

	return true;
}
