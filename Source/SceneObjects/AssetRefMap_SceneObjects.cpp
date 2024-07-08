
#include "AssetRefMap_SceneObjects.h"

UAssetRefMap_SceneObjects* UAssetRefMap_SceneObjects::GetInstance()
{
	auto InterfacePtr = Cast<IAssetRefMap_SceneObjects_Interface>(GEngine->GetWorld()->GetWorldSettings());
	return InterfacePtr->GetInstance();
}
