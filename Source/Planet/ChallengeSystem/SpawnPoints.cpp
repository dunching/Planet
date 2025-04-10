#include "SpawnPoints.h"

#include "CollisionDataStruct.h"
#include "GuideSubSystem.h"
#include "NiagaraComponent.h"
#include "Async/TaskGraphInterfaces.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/ArrowComponent.h"
#include "Components/BillboardComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/Texture2D.h"
#include "Components/BrushComponent.h"

#include "GuideThread.h"
#include "HumanCharacter_Player.h"
#include "TestCommand.h"

ASpawnPoints::ASpawnPoints(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));

	RootComponent = SceneComponent;

	WidgetComponentPtr = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("WidgetComponent"));
	WidgetComponentPtr->SetupAttachment(RootComponent);
}

TArray<FTransform> ASpawnPoints::GetSpawnPts(int32 Num) const
{
	TArray<FTransform> Result;

	auto Comps = GetComponents();

	TArray<UBillboardComponent*> BillboardComponentAry;
	for (auto Iter : Comps)
	{
		auto Ptr = Cast<UBillboardComponent>(Iter);
		if (Ptr)
		{
			BillboardComponentAry.Add(Ptr);
		}
	}

	if (BillboardComponentAry.Num() >= Num)
	{
		for (int32 Index = 0; Index < Num; Index++)
		{
			Result.Add(BillboardComponentAry[Index]->GetComponentTransform());
		}
	}

	return Result;
}
