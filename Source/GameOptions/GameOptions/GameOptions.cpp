#include "GameOptions.h"

#include <Kismet/GameplayStatics.h>

#include "GameplayTagsLibrary.h"

UGameOptions::UGameOptions(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	{
		struct FMyStruct
		{
			FGameplayTag SocketTag;
			FKey Key;
		};
		TArray<FMyStruct> Ary
		{
					{UGameplayTagsLibrary::ActiveSocket_1, ActiveSkill_1_Key},
					{UGameplayTagsLibrary::ActiveSocket_2, ActiveSkill_2_Key},
					{UGameplayTagsLibrary::ActiveSocket_3, ActiveSkill_3_Key},
					{UGameplayTagsLibrary::ActiveSocket_4, ActiveSkill_4_Key},
		
					{UGameplayTagsLibrary::WeaponSocket_1, EKeys::LeftMouseButton},
					{UGameplayTagsLibrary::WeaponSocket_2, EKeys::LeftMouseButton},
		
					{UGameplayTagsLibrary::ConsumableSocket_1, Consumable_1_Key},
					{UGameplayTagsLibrary::ConsumableSocket_2, Consumable_2_Key},
					{UGameplayTagsLibrary::ConsumableSocket_3, Consumable_3_Key},
					{UGameplayTagsLibrary::ConsumableSocket_4, Consumable_4_Key},
		
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
}

UGameOptions* UGameOptions::GetInstance()
{
	// auto WorldSetting = Cast<APlanetWorldSettings>(GetWorldImp()->GetWorldSettings());
	// return WorldSetting->GetGameOptions();
	
	return GEngine ? CastChecked<UGameOptions>(GEngine->GetGameUserSettings()) : nullptr;
}

int32 UGameOptions::MaxTeammateNum = 5;

void UGameOptions::PostCDOContruct()
{
	Super::PostCDOContruct();
}

void UGameOptions::PostInitProperties()
{
	Super::PostInitProperties();
}

TMap<FGameplayTag, FKey> UGameOptions::GetActionKeyMap() const
{
	return ActionKeyMap;
}
