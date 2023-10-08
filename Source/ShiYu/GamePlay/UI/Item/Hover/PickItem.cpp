#include "PickItem.h"
#include "Components/WidgetComponent.h"

APickItem::APickItem() :
	Super()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WidgetCompptr = CreateDefaultSubobject<UWidgetComponent>(TEXT("PawnSensingComp"));
	RootComponent = WidgetCompptr;
}

void APickItem::SetUIClass(UClass* ClassPtr)
{
	WidgetCompptr->SetWidgetClass(ClassPtr);
}

void APickItem::SetVisible(bool Visibility)
{
	WidgetCompptr->SetVisibility(Visibility);  
}
