#include "GameOptions.h"

#include <Kismet/GameplayStatics.h>
#include "GameplayTagsLibrary.h"

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
			{UGameplayTagsLibrary::ActiveSocket_1, EKeys::Q},
			{UGameplayTagsLibrary::ActiveSocket_2, EKeys::E},
			{UGameplayTagsLibrary::ActiveSocket_3, EKeys::R},
			{UGameplayTagsLibrary::ActiveSocket_4, EKeys::F},
		
			{UGameplayTagsLibrary::WeaponSocket_1, EKeys::LeftMouseButton},
			{UGameplayTagsLibrary::WeaponSocket_2, EKeys::LeftMouseButton},
		
			{UGameplayTagsLibrary::ConsumableSocket1, EKeys::One},
			{UGameplayTagsLibrary::ConsumableSocket2, EKeys::Two},
			{UGameplayTagsLibrary::ConsumableSocket3, EKeys::Three},
			{UGameplayTagsLibrary::ConsumableSocket4, EKeys::Four},
		
			{UGameplayTagsLibrary::PassiveSocket_1, EKeys::Invalid},
			{UGameplayTagsLibrary::PassiveSocket_2, EKeys::Invalid},
			{UGameplayTagsLibrary::PassiveSocket_3, EKeys::Invalid},
			{UGameplayTagsLibrary::PassiveSocket_4, EKeys::Invalid},
			{UGameplayTagsLibrary::PassiveSocket_5, EKeys::Invalid},
		};

	for (const auto& Iter : Ary)
	{
		ActionKeyMap.Add(Iter.SocketTag, Iter.Key);
	}
}
