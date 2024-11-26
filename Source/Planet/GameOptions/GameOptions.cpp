#include "GameOptions.h"

#include <Kismet/GameplayStatics.h>
#include "GameplayTagsSubSystem.h"

#include "GameInstance/PlanetGameInstance.h"
#include "StateTagExtendInfo.h"
#include "Planet.h"
#include "PlanetWorldSettings.h"

UGameOptions* UGameOptions::GetInstance()
{
	auto WorldSetting = Cast<APlanetWorldSettings>(GetWorldImp()->GetWorldSettings());
	return WorldSetting->GetGameOptions();
}

void UGameOptions::PostCDOContruct()
{
	Super::PostCDOContruct();
}

void UGameOptions::PostInitProperties()
{
	Super::PostInitProperties();

	struct FMyStruct
	{
		FGameplayTag SocketTag;
		FKey Key;
	};
	TArray<FMyStruct> Ary
	{
			{UGameplayTagsSubSystem::ActiveSocket_1, EKeys::Q},
			{UGameplayTagsSubSystem::ActiveSocket_2, EKeys::E},
			{UGameplayTagsSubSystem::ActiveSocket_3, EKeys::R},
			{UGameplayTagsSubSystem::ActiveSocket_4, EKeys::F},
		
			{UGameplayTagsSubSystem::WeaponSocket_1, EKeys::LeftMouseButton},
			{UGameplayTagsSubSystem::WeaponSocket_2, EKeys::LeftMouseButton},
		
			{UGameplayTagsSubSystem::WeaponActiveSocket_1, EKeys::LeftMouseButton},
			{UGameplayTagsSubSystem::WeaponActiveSocket_2, EKeys::LeftMouseButton},
		
			{UGameplayTagsSubSystem::ConsumableSocket1, EKeys::One},
			{UGameplayTagsSubSystem::ConsumableSocket2, EKeys::Two},
			{UGameplayTagsSubSystem::ConsumableSocket3, EKeys::Three},
			{UGameplayTagsSubSystem::ConsumableSocket4, EKeys::Four},
		
			{UGameplayTagsSubSystem::PassiveSocket_1, EKeys::Invalid},
			{UGameplayTagsSubSystem::PassiveSocket_2, EKeys::Invalid},
			{UGameplayTagsSubSystem::PassiveSocket_3, EKeys::Invalid},
			{UGameplayTagsSubSystem::PassiveSocket_4, EKeys::Invalid},
			{UGameplayTagsSubSystem::PassiveSocket_5, EKeys::Invalid},
		};

	for (const auto& Iter : Ary)
	{
		ActionKeyMap.Add(Iter.SocketTag, Iter.Key);
	}
}
