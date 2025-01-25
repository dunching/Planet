#include "TargetPoint_Runtime.h"

#include "NiagaraComponent.h"
#include "Async/TaskGraphInterfaces.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/ArrowComponent.h"
#include "Components/BillboardComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/Texture2D.h"

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
