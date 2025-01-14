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

void AArticleGrass::HasbeenInteracted(ACharacterBase* CharacterPtr)
{
	Super::HasbeenInteracted(CharacterPtr);

	if (CharacterPtr)
	{
//		CharacterPtr->GetHoldItemComponent()->GetSceneProxyContainer()->AddItem(GetPropertyComponent()->GetItemsType());
	}

	Destroy();
}
