#include "TargetPoint_Runtime.h"

#include "NiagaraComponent.h"
#include "Async/TaskGraphInterfaces.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/ArrowComponent.h"
#include "Components/BillboardComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/Texture2D.h"
#include "Kismet/GameplayStatics.h"
#include "Slate/SGameLayerManager.h"

#include "Slate/SWorldWidgetScreenLayer.h"

#include "CollisionDataStruct.h"
#include "PlanetGameViewportClient.h"
#include "MarkPoints.h"

const int32 CheckDistance = 500;

ATargetPoint_Runtime::ATargetPoint_Runtime(
	const FObjectInitializer& ObjectInitializer
) :
  Super(ObjectInitializer)
{
	USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));

	RootComponent = SceneComponent;

	NiagaraComponentPtr = CreateEditorOnlyDefaultSubobject<UNiagaraComponent>(TEXT("Sprite"));
	NiagaraComponentPtr->SetupAttachment(SceneComponent);

	SetHidden(false);
	SetCanBeDamaged(false);
}

void ATargetPoint_Runtime::BeginPlay()
{
	Super::BeginPlay();

	// 调整位置 紧贴地面
	// TODO. 重新生成导航
	
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(LandScape_Object);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	FHitResult OutHit;
	if (GetWorld()->LineTraceSingleByObjectType(
		OutHit,
		GetActorLocation() + (FVector::UpVector * CheckDistance),
		GetActorLocation() - (FVector::UpVector * CheckDistance),
		ObjectQueryParams,
		Params
	))
	{
		SetActorLocation(OutHit.ImpactPoint);
	}

	auto ScreenLayer = UKismetGameLayerManagerLibrary::GetGameLayer<FHoverWidgetScreenLayer>(
		GetWorld(),
		TargetPointSharedLayerName
	);
	if (ScreenLayer)
	{
		MarkPointsPtr = CreateWidget<UMarkPoints>(GetWorld(), MarkPointsClass);
		if (MarkPointsPtr)
		{
			MarkPointsPtr->TargetPoint_RuntimePtr = this;

			ScreenLayer->AddHoverWidget(MarkPointsPtr);
		}
	}
}

void ATargetPoint_Runtime::EndPlay(
	const EEndPlayReason::Type EndPlayReason
)
{
	if (MarkPointsPtr)
	{
		auto ScreenLayer = UKismetGameLayerManagerLibrary::GetGameLayer<FHoverWidgetScreenLayer>(
			GetWorld(),
			TargetPointSharedLayerName
		);
		if (ScreenLayer)
		{
			ScreenLayer->RemoveHoverWidget(MarkPointsPtr);
		}

		MarkPointsPtr  =nullptr;
	}
	Super::EndPlay(EndPlayReason);
}
