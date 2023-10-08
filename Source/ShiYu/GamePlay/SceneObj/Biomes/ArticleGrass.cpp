#include "ArticleGrass.h"
#include "Components/StaticMeshComponent.h"
#include "SceneObj/CollisionDataStruct.h"
#include "CharacterBase.h"
#include "Pawn/HoldItemComponent.h"

AArticleGrass::AArticleGrass(const FObjectInitializer& ObjectInitializer) :
	Super(
		ObjectInitializer
	)
{
}

void AArticleGrass::Interaction(ACharacterBase* CharacterPtr)
{
	Super::Interaction(CharacterPtr);

	if (CharacterPtr)
	{
//		CharacterPtr->GetHoldItemComponent()->GetHoldItemProperty()->AddItem(GetPropertyComponent()->GetItemsType());
	}

	Destroy();
}
