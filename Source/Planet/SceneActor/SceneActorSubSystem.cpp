
#include "SceneActorSubSystem.h"

#include <Subsystems/SubsystemBlueprintLibrary.h>
#include <Kismet/GameplayStatics.h>

#include "Planet.h"
#include "AssetRefMap.h"

USceneActorSubSystem* USceneActorSubSystem::GetInstance()
{
	return Cast<USceneActorSubSystem>(USubsystemBlueprintLibrary::GetGameInstanceSubsystem(
		GetWorldImp(), USceneActorSubSystem::StaticClass())
	);
}

APostProcessVolume* USceneActorSubSystem::GetSkillPost() const
{
	APostProcessVolume * ResultPtr = nullptr;

	TArray<AActor*>ResultAry;
	UGameplayStatics::GetAllActorsOfClassWithTag(
		this,
		APostProcessVolume::StaticClass(), 
		UAssetRefMap::GetInstance()->PostProcessVolume_Skill_Tag,
		ResultAry
	);

	for (auto Iter : ResultAry)
	{
		ResultPtr = Cast<APostProcessVolume>(Iter);
	}

	return ResultPtr;
}

