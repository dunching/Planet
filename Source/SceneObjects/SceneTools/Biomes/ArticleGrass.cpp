
#include "ArticleGrass.h"
#include "Components/StaticMeshComponent.h"
#include "CollisionDataStruct.h"

AArticleGrass::AArticleGrass(const FObjectInitializer& ObjectInitializer) :
	Super(
		ObjectInitializer
	)
{
}

void AArticleGrass::Interaction(ACharacter* CharacterPtr)
{
	Super::Interaction(CharacterPtr);

	if (CharacterPtr)
	{
//		CharacterPtr->GetHoldItemComponent()->GetHoldItemProperty()->AddItem(GetPropertyComponent()->GetItemsType());
	}

	Destroy();
}
