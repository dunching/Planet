
#include "CoinList.h"

#include "Kismet/GameplayStatics.h"
#include "Components/TextBlock.h"
#include <Blueprint/WidgetTree.h>
#include <Components/HorizontalBox.h>

#include "CharacterBase.h"
#include "TalentAllocationComponent.h"
#include "TalentIcon.h"
#include "CoinInfo.h"

struct FCoinList : public TStructVariable<FCoinList>
{
	FName HorizontalBox = TEXT("HorizontalBox");
	
	FName Regular = TEXT("Regular");
};

void UCoinList::NativeConstruct()
{
	Super::NativeConstruct();
}

void UCoinList::NativeDestruct()
{
	Super::NativeDestruct(); 
}

void UCoinList::Enable()
{
	ILayoutItemInterfacetion::Enable();

	auto UIPtr = Cast<UCoinInfo>(GetWidgetFromName(FCoinList::Get().Regular));
	if (UIPtr )
	{
		UIPtr->Enable();
	}
}
