// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelSculptEdMode.h"
#include "VoxelSculptToolkit.h"
#include "VoxelParameterContainer.h"
#include "Sculpt/VoxelSculptStorage.h"
#include "Sculpt/VoxelSculptFunctionLibrary.h"
#include "EdModeInteractiveToolsContext.h"
#include "BaseBehaviors/ClickDragBehavior.h"
#include "BaseBehaviors/MouseHoverBehavior.h"

DEFINE_VOXEL_COMMANDS(FVoxelSculptCommands);

void FVoxelSculptCommands::RegisterCommands()
{
	VOXEL_UI_COMMAND(Sculpt, "Sculpt", "Sculpt", EUserInterfaceActionType::ToggleButton, {});
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void UVoxelSculptTool::Setup()
{
	VOXEL_FUNCTION_COUNTER();

	Super::Setup();

	{
		UMouseHoverBehavior* Behavior = NewObject<UMouseHoverBehavior>();
		Behavior->Initialize(this);
		AddInputBehavior(Behavior);
	}
	{
		UClickDragInputBehavior* Behavior = NewObject<UClickDragInputBehavior>();
		Behavior->Initialize(this);
		AddInputBehavior(Behavior);
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.bDeferConstruction = true;
	SpawnParameters.ObjectFlags = RF_Transient;
	PreviewActor = GetWorld()->SpawnActor<AVoxelPreviewActor>(SpawnParameters);
	PreviewActor->SetActorLabel("VoxelSculptActor");
	PreviewActor->bCreateRuntimeOnBeginPlay = false;
	PreviewActor->DefaultRuntimeParameters.Add<FVoxelRuntimeParameter_DisableCollision>();

	FString ProviderString;
	if (GConfig->GetString(
		TEXT("VoxelSculptTool"),
		TEXT("Provider"),
		ProviderString,
		GEditorPerProjectIni))
	{
		FVoxelObjectUtilities::PropertyFromText_InContainer(
			FindFPropertyChecked(UVoxelParameterContainer, Provider),
			ProviderString,
			PreviewActor->ParameterContainer);
	}

	FString ValueOverridesString;
	if (GConfig->GetString(
		TEXT("VoxelSculptTool"),
		TEXT("ValueOverrides"),
		ValueOverridesString,
		GEditorPerProjectIni))
	{
		FVoxelObjectUtilities::PropertyFromText_InContainer(
			FindFPropertyChecked(UVoxelParameterContainer, ValueOverrides),
			ValueOverridesString,
			PreviewActor->ParameterContainer);
	}

	PreviewActor->FinishSpawning(FTransform::Identity);

	AddToolPropertySource(PreviewActor);

	const auto SetTargetActor = [this](AVoxelActor* TargetActor)
	{
		if (!ensure(PreviewActor))
		{
			return;
		}

		PreviewActor->DefaultRuntimeParameters.Remove<FVoxelRuntimeParameter_SculptStorage>();
		PreviewActor->TargetActor = TargetActor;

		if (TargetActor &&
			ensure(TargetActor->SculptStorageComponent))
		{
			// Setup parameter for preview
			const TSharedRef<FVoxelRuntimeParameter_SculptStorage> Parameter = MakeVoxelShared<FVoxelRuntimeParameter_SculptStorage>();
			Parameter->Data = TargetActor->SculptStorageComponent->GetData();
			Parameter->SurfaceToWorldOverride = FVoxelTransformRef::Make(*TargetActor);
			PreviewActor->DefaultRuntimeParameters.Add(Parameter);
			PreviewActor->QueueRecreate();
		}
		else
		{
			PreviewActor->DestroyRuntime();
		}
	};
	const auto OnSelectionChanged = [this, SetTargetActor](UObject*)
	{
		for (FSelectionIterator It(*GEditor->GetSelectedActors()); It; ++It)
		{
			if (AVoxelActor* Actor = Cast<AVoxelActor>(*It))
			{
				SetTargetActor(Actor);
				break;
			}
		}

	};
	USelection::SelectionChangedEvent.AddWeakLambda(this, OnSelectionChanged);
	OnSelectionChanged(nullptr);

	if (!PreviewActor->TargetActor)
	{
		for (AVoxelActor* Actor : TActorRange<AVoxelActor>(GetWorld()))
		{
			SetTargetActor(Actor);
			break;
		}
	}
}

void UVoxelSculptTool::Shutdown(EToolShutdownType ShutdownType)
{
	VOXEL_FUNCTION_COUNTER();

	if (!ensure(PreviewActor))
	{
		return;
	}

	GConfig->SetString(
		TEXT("VoxelSculptTool"),
		TEXT("Provider"),
		*FVoxelObjectUtilities::PropertyToText_InContainer(
			FindFPropertyChecked(UVoxelParameterContainer, Provider),
			PreviewActor->ParameterContainer),
		GEditorPerProjectIni);

	GConfig->SetString(
		TEXT("VoxelSculptTool"),
		TEXT("ValueOverrides"),
		*FVoxelObjectUtilities::PropertyToText_InContainer(
			FindFPropertyChecked(UVoxelParameterContainer, ValueOverrides),
			PreviewActor->ParameterContainer),
		GEditorPerProjectIni);

	PreviewActor->Destroy();

	USelection::SelectionChangedEvent.RemoveAll(this);
}

void UVoxelSculptTool::OnTick(const float DeltaTime)
{
	if (!bIsEditing)
	{
		return;
	}

	if (LastRay)
	{
		UpdatePosition(LastRay.GetValue());
	}

	(void)DoEdit();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FInputRayHit UVoxelSculptTool::BeginHoverSequenceHitTest(const FInputDeviceRay& PressPos)
{
	FInputRayHit Hit;
	Hit.bHit = true;
	return Hit;
}

bool UVoxelSculptTool::OnUpdateHover(const FInputDeviceRay& DevicePos)
{
	UpdatePosition(DevicePos);
	return true;
}

FInputRayHit UVoxelSculptTool::CanBeginClickDragSequence(const FInputDeviceRay& PressPos)
{
	FInputRayHit Result;
	Result.bHit = DoEdit();
	return Result;
}

void UVoxelSculptTool::OnClickPress(const FInputDeviceRay& PressPos)
{
	UpdatePosition(PressPos);
	DoEdit();

	ensure(!bIsEditing);
	bIsEditing = true;
}

void UVoxelSculptTool::OnClickDrag(const FInputDeviceRay& DragPos)
{
	UpdatePosition(DragPos);
}

void UVoxelSculptTool::OnClickRelease(const FInputDeviceRay& ReleasePos)
{
	ensure(bIsEditing);
	bIsEditing = false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void UVoxelSculptTool::UpdatePosition(const FInputDeviceRay& Position)
{
	LastRay = Position;

	FHitResult HitResult;
	if (!GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Position.WorldRay.Origin,
		Position.WorldRay.Origin + Position.WorldRay.Direction * 1e6,
		ECC_EngineTraceChannel6))
	{
		return;
	}

	if (!ensure(PreviewActor) ||
		!HitResult.bBlockingHit)
	{
		return;
	}

	PreviewActor->SetActorLocation(HitResult.Location);
}

bool UVoxelSculptTool::DoEdit() const
{
	if (!ensure(PreviewActor) ||
		!PreviewActor->GetGraph() ||
		!PreviewActor->TargetActor ||
		!PreviewActor->IsRuntimeCreated())
	{
		VOXEL_MESSAGE(Error, "No voxel actor selected");
		return false;
	}

	UVoxelSculptFunctionLibrary::ApplySculpt(
		PreviewActor->TargetActor,
		PreviewActor);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

UVoxelSculptEdMode::UVoxelSculptEdMode()
{
	SettingsClass = UVoxelSculptEdModeSettings::StaticClass();

	Info = FEditorModeInfo(
		"VoxelSculptEdMode",
		INVTEXT("Voxel Sculpt"),
		FSlateIcon(FVoxelEditorStyle::GetStyleSetName(), "VoxelIcon"),
		true);
}

void UVoxelSculptEdMode::Enter()
{
	Super::Enter();

	const FVoxelSculptCommands& Commands = FVoxelSculptCommands::Get();

	RegisterTool(
		Commands.Sculpt,
		GetClassName<UVoxelSculptTool>(),
		NewObject<UVoxelSculptToolBuilder>(this));

	GetInteractiveToolsContext()->StartTool(GetClassName<UVoxelSculptTool>());
}

void UVoxelSculptEdMode::Exit()
{
	Super::Exit();
}

void UVoxelSculptEdMode::CreateToolkit()
{
	Toolkit = MakeVoxelShared<FVoxelSculptToolkit>();
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> UVoxelSculptEdMode::GetModeCommands() const
{
	const FVoxelSculptCommands& Commands = FVoxelSculptCommands::Get();

	TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> Result;
	Result.Add("Tools", {
		Commands.Sculpt
	});
	return Result;
}