
#include "AssetRefMap_Tools.h"

UAssetRefMap_Tools* UAssetRefMap_Tools::GetInstance()
{
	auto InterfacePtr = Cast<IAssetRefMap_Tools_Interface>(GEngine->GetWorld()->GetWorldSettings());
	return InterfacePtr->GetInstance();
}
