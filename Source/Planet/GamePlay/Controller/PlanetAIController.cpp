
#include "PlanetAIController.h"

#include "AIHumanInfo.h"
#include "CharacterBase.h"
#include "AssetRefMap.h"
#include "Planet.h"
#include "GroupMnaggerComponent.h"
#include "SceneElement.h"
#include "HumanCharacter.h"

void APlanetAIController::SetCampType(ECharacterCampType CharacterCampType)
{
	if (AIHumanInfoPtr)
	{
		AIHumanInfoPtr->SetCampType(CharacterCampType);
	}
}

UGroupMnaggerComponent* APlanetAIController::GetGroupMnaggerComponent()
{
	if (!GroupMnaggerComponentPtr)
	{
		GroupMnaggerComponentPtr = Cast<UGroupMnaggerComponent>(AddComponentByClass(UGroupMnaggerComponent::StaticClass(), true, FTransform::Identity, false));
	}
	return  GroupMnaggerComponentPtr;
}

UGourpmateUnit* APlanetAIController::GetGourpMateUnit()
{
	return Cast<AHumanCharacter>(GetPawn())->GetGourpMateUnit();
}

void APlanetAIController::BeginPlay()
{
	Super::BeginPlay();
}

void APlanetAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (AIHumanInfoPtr)
	{
		AIHumanInfoPtr->RemoveFromParent();
		AIHumanInfoPtr = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}

void APlanetAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	auto CharacterPtr = Cast<ACharacterBase>(InPawn);
	if (CharacterPtr)
	{
		auto AssetRefMapPtr = UAssetRefMap::GetInstance();
		AIHumanInfoPtr = CreateWidget<UAIHumanInfo>(GetWorldImp(), AssetRefMapPtr->AIHumanInfoClass);
		if (AIHumanInfoPtr)
		{
			AIHumanInfoPtr->CharacterPtr = CharacterPtr;
			AIHumanInfoPtr->AddToViewport();
		}
	}

	SetCampType(ECharacterCampType::kEnemy);
}
