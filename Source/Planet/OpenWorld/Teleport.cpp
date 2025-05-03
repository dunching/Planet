#include "Teleport.h"

#include "HumanCharacter.h"
#include "NiagaraComponent.h"
#include "Async/TaskGraphInterfaces.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/ArrowComponent.h"
#include "Components/BillboardComponent.h"
#include "Components/SphereComponent.h"
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
	
	GEAreaComponentPtr = CreateEditorOnlyDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	GEAreaComponentPtr->SetupAttachment(RootComponent);

	GEAreaComponentPtr->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnComponentBeginOverlap);
}

void ATeleport::BeginPlay()
{
	Super::BeginPlay();
}

FTransform ATeleport::GetLandTransform() const
{
	return LandPtComponentPtr->GetComponentTransform();
}

void ATeleport::OnComponentBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (OtherActor && OtherActor->IsA(AHumanCharacter::StaticClass()))
	{
		auto CharacterPtr = Cast<AHumanCharacter>(OtherActor);
		if (CharacterPtr)
		{
			CharacterPtr->GetCharacterAbilitySystemComponent();
		}
	}
}
