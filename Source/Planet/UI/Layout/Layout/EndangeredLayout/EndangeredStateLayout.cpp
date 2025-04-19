#include "EndangeredStateLayout.h"

#include "CharacterAbilitySystemComponent.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

#include "GameplayCommand.h"
#include "HumanCharacter_Player.h"
#include "OpenWorldSystem.h"
#include "PlanetPlayerController.h"

struct FEndangeredStateLayout : public TStructVariable<FEndangeredStateLayout>
{
	FName RespawnBtn = TEXT("RespawnBtn");
};

void UEndangeredStateLayout::NativeConstruct()
{
	Super::NativeConstruct();
	
	auto UIPtr = Cast<UButton>(GetWidgetFromName(FEndangeredStateLayout::Get().RespawnBtn));
	if (UIPtr)
	{
		UIPtr->OnClicked.AddDynamic(this, &ThisClass::OnClicked);
	}
}

void UEndangeredStateLayout::Enable()
{
}

void UEndangeredStateLayout::DisEnable()
{
}

void UEndangeredStateLayout::OnClicked()
{
	auto PCPtr = Cast<APlanetPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (!PCPtr)
	{
		return;
	}
	
	PCPtr->TeleportPlayerToNearest();
}
