#include "EndangeredStateLayout.h"

#include "AssetRefMap.h"
#include "CharacterAbilitySystemComponent.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

#include "GameplayCommand.h"
#include "GuideSubSystem.h"
#include "HumanCharacter_Player.h"
#include "GuideThreadChallenge.h"
#include "LayoutCommon.h"
#include "PlanetPlayerController.h"
#include "PlayerGameplayTasks.h"

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

ELayoutCommon UEndangeredStateLayout::GetLayoutType() const
{
	return ELayoutCommon::kEndangeredLayout;
}

void UEndangeredStateLayout::OnClicked()
{
	UGuideSubSystem::GetInstance()->StopParallelGuideThread(
		UAssetRefMap::GetInstance()->GuideThreadChallengeActorClass
		);
	
	auto PCPtr = Cast<APlanetPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (!PCPtr)
	{
		return;
	}
	
	PCPtr->GetGameplayTasksComponent()->TeleportPlayerToNearest();
}
