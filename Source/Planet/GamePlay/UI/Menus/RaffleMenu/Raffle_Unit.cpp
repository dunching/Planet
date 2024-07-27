
#include "Raffle_Unit.h"

#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include <Blueprint/WidgetTree.h>
#include <Components/Button.h>
#include <Components/Border.h>
#include <Components/Image.h>
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

#include "CharacterBase.h"
#include "TalentAllocationComponent.h"
#include "TalentIcon.h"
#include "UICommon.h"

struct FRaffle_Unit : public TGetSocketName<FRaffle_Unit>
{
	FName Texture = TEXT("Texture");

	FName Text = TEXT("Text");
};

void URaffle_Unit::NativeConstruct()
{
	Super::NativeConstruct();
}

void URaffle_Unit::NativeDestruct()
{
	Super::NativeDestruct();
}

void URaffle_Unit::InvokeReset(UUserWidget* BaseWidgetPtr)
{
}

void URaffle_Unit::ResetToolUIByData(UBasicUnit* BasicUnitPtr)
{
}

void URaffle_Unit::EnableIcon(bool bIsEnable)
{
}
