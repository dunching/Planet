#include "ArticleGrass.h"
#include "Components/StaticMeshComponent.h"
#include "CollisionDataStruct.h"
#include "CharacterBase.h"
#include "HoldingItemsComponent.h"

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
//		CharacterPtr->GetHoldItemComponent()->GetSceneProxyContainer()->AddItem(GetPropertyComponent()->GetItemsType());
	}

	Destroy();
}
