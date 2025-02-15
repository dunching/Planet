#include "Teleport.h"

#include "NiagaraComponent.h"
#include "Async/TaskGraphInterfaces.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/ArrowComponent.h"
#include "Components/BillboardComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/Texture2D.h"

ATeleport::ATeleport(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	USceneComponent* SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComp"));

	RootComponent = SceneComponent;

	StaticComponentPtr = CreateEditorOnlyDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticComponentPtr->SetupAttachment(RootComponent);

	LandPtComponentPtr = CreateEditorOnlyDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	LandPtComponentPtr->SetupAttachment(RootComponent);

#if WITH_EDITOR
	WidgetComponentPtr = CreateEditorOnlyDefaultSubobject<UBillboardComponent>(TEXT("WidgetComponent"));
	WidgetComponentPtr->SetupAttachment(LandPtComponentPtr);
#endif
	
}

FTransform ATeleport::GetLandTransform() const
{
	return LandPtComponentPtr->GetComponentTransform();
}
