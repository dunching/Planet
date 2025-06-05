#include "TaskPromt.h"

#include <Kismet/GameplayStatics.h>
#include <Blueprint/WidgetLayoutLibrary.h>
#include <Components/SizeBox.h>

#include "CharacterAbilitySystemComponent.h"
#include "CharacterBase.h"
#include "LogWriter.h"

FVector UTaskPromt::GetHoverPosition()
{
	return TargetCharacterPtr->GetActorLocation() + Offsert;
}
