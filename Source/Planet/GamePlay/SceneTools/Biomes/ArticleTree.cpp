
#include "ArticleTree.h"

#include <Kismet/GameplayStatics.h>

#include "Components/StaticMeshComponent.h"
#include "CollisionDataStruct.h"
#include "GameInstance/PlanetGameInstance.h"
#include "ArticleSharedData.h"
#include <AssetRefMap.h>

AArticleTree::AArticleTree(const FObjectInitializer& ObjectInitializer) :
	Super(
		ObjectInitializer
	)
{
}

void AArticleTree::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	static const auto StaticMeshAry = UAssetRefMap::GetInstance()->TreeStaticMeshRefAry;
	if ((StaticMeshAry.Num() > 0) && StaticMeshCompPtr)
	{
		StaticMeshCompPtr->SetStaticMesh(StaticMeshAry[FMath::RandHelper(StaticMeshAry.Num())].LoadSynchronous());
	}
}
