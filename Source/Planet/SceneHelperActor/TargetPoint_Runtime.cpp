#include "TargetPoint_Runtime.h"

#include "CollisionDataStruct.h"
#include "NiagaraComponent.h"
#include "Async/TaskGraphInterfaces.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/ArrowComponent.h"
#include "Components/BillboardComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/Texture2D.h"

const int32 CheckDistance = 500;

ATargetPoint_Runtime::ATargetPoint_Runtime(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));

	RootComponent = SceneComponent;

	WidgetComponentPtr = CreateEditorOnlyDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponentPtr->SetupAttachment(SceneComponent);
	
	NiagaraComponentPtr = CreateEditorOnlyDefaultSubobject<UNiagaraComponent>(TEXT("Sprite"));
	WidgetComponentPtr->SetupAttachment(SceneComponent);
	
	SetHidden(false);
	SetCanBeDamaged(false);
}

void ATargetPoint_Runtime::BeginPlay()
{
	Super::BeginPlay();

	FCollisionQueryParams Param;
	
	FHitResult OutHit;
	if (GetWorld()->LineTraceSingleByChannel(
		OutHit,
		GetActorLocation() + (FVector::UpVector * CheckDistance),
		GetActorLocation() - (FVector::UpVector * CheckDistance),
		SceneActor_Channel,
		Param
		))
	{
		SetActorLocation(OutHit.ImpactPoint);
	}
}
