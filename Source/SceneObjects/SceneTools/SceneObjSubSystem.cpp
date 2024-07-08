
#include "SceneObjSubSystem.h"

#include <Subsystems/SubsystemBlueprintLibrary.h>
#include <Kismet/GameplayStatics.h>

#include <AssetRefMap_SceneObjects.h>

USceneObjSubSystem* USceneObjSubSystem::GetInstance()
{
	return Cast<USceneObjSubSystem>(USubsystemBlueprintLibrary::GetGameInstanceSubsystem(
		GEngine->GetWorld(), USceneObjSubSystem::StaticClass())
	);
}

APostProcessVolume* USceneObjSubSystem::GetSkillPost() const
{
	APostProcessVolume * ResultPtr = nullptr;

	TArray<AActor*>ResultAry;
	UGameplayStatics::GetAllActorsOfClassWithTag(
		this,
		APostProcessVolume::StaticClass(), 
		UAssetRefMap_SceneObjects::GetInstance()->PostProcessVolume_Skill_Tag,
		ResultAry
	);

	for (auto Iter : ResultAry)
	{
		ResultPtr = Cast<APostProcessVolume>(Iter);
	}

	return ResultPtr;
}

