
#include "ArticleTree.h"

#include <Kismet/GameplayStatics.h>

#include "Components/StaticMeshComponent.h"
#include "CollisionDataStruct.h"
#include "ArticleSharedData.h"

AArticleTree::AArticleTree(const FObjectInitializer& ObjectInitializer) :
	Super(
		ObjectInitializer
	)
{
}

void AArticleTree::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
}
