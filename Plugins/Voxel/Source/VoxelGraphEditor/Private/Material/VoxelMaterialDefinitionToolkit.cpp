// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelMaterialDefinitionToolkit.h"
#include "VoxelMaterialDefinitionParameterSelectionCustomization.h"
#include "SVoxelMaterialDefinitionParameters.h"
#include "Components/StaticMeshComponent.h"

void FVoxelMaterialDefinitionToolkit::Initialize()
{
	Super::Initialize();

	if (UVoxelMaterialDefinition* Definition = Cast<UVoxelMaterialDefinition>(Asset))
	{
		MaterialLayerParameters =
			SNew(SVoxelMaterialDefinitionParameters)
			.Definition(Definition)
			.Toolkit(SharedThis(this));
	}
}

TSharedPtr<FTabManager::FLayout> FVoxelMaterialDefinitionToolkit::GetLayout() const
{
	if (Asset->IsA<UVoxelMaterialDefinition>())
	{
		return FTabManager::NewLayout("FVoxelMaterialDefinitionToolkit_Definition_Layout_v1")
			->AddArea
			(
				FTabManager::NewPrimaryArea()
				->SetOrientation(Orient_Horizontal)
				->Split
				(
					FTabManager::NewSplitter()
					->SetOrientation(Orient_Vertical)
					->SetSizeCoefficient(0.3f)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.4f)
						->AddTab(DetailsTabId, ETabState::OpenedTab)
					)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.6f)
						->AddTab(ParametersTabId, ETabState::OpenedTab)
					)
				)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.7f)
					->AddTab(ViewportTabId, ETabState::OpenedTab)
				)
			);
	}
	else
	{
		return FTabManager::NewLayout("FVoxelMaterialDefinitionToolkit_Instance_Layout_v1")
			->AddArea
			(
				FTabManager::NewPrimaryArea()
				->SetOrientation(Orient_Horizontal)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.4f)
					->AddTab(DetailsTabId, ETabState::OpenedTab)
				)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.7f)
					->AddTab(ViewportTabId, ETabState::OpenedTab)
				)
			);
	}
}

void FVoxelMaterialDefinitionToolkit::RegisterTabs(FRegisterTab RegisterTab)
{
	Super::RegisterTabs(RegisterTab);

	RegisterTab(ParametersTabId, INVTEXT("Parameters"), "ClassIcon.BlueprintCore", MaterialLayerParameters);
}

void FVoxelMaterialDefinitionToolkit::PostEditChange(const FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChange(PropertyChangedEvent);

	if (PropertyChangedEvent.ChangeType != EPropertyChangeType::Interactive)
	{
		UpdatePreview();
	}
}

void FVoxelMaterialDefinitionToolkit::SetupPreview()
{
	VOXEL_FUNCTION_COUNTER();

	Super::SetupPreview();

	UStaticMesh* StaticMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/EditorMeshes/EditorSphere.EditorSphere"));
	ensure(StaticMesh);

	StaticMeshComponent = NewObject<UStaticMeshComponent>();
	StaticMeshComponent->SetStaticMesh(StaticMesh);
	GetPreviewScene().AddComponent(StaticMeshComponent.Get(), FTransform::Identity);
}

void FVoxelMaterialDefinitionToolkit::UpdatePreview()
{
	VOXEL_FUNCTION_COUNTER();

	Super::UpdatePreview();

	if (!ensure(StaticMeshComponent.IsValid()))
	{
		return;
	}

	StaticMeshComponent->SetMaterial(0, Asset->GetPreviewMaterial());
}

FRotator FVoxelMaterialDefinitionToolkit::GetInitialViewRotation() const
{
	return FRotator(-20.0f, 180 + 45.0f, 0.f);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelMaterialDefinitionToolkit::SelectParameter(const FGuid& Guid, const bool bRequestRename, const bool bRefresh)
{
	if (SelectedGuid == Guid &&
		!bRefresh)
	{
		return;
	}
	SelectedGuid = Guid;

	IDetailsView& DetailsView = GetDetailsView();

	const FVoxelParameter* Parameter = CastChecked<UVoxelMaterialDefinition>(Asset)->FindParameterByGuid(Guid);
	if (!Parameter)
	{
		MaterialLayerParameters->SelectMember({}, 0, false, bRefresh);

		DetailsView.SetGenericLayoutDetailsDelegate(nullptr);
		DetailsView.ForceRefresh();
		return;
	}

	MaterialLayerParameters->SelectMember(Parameter->Name, 1, bRequestRename, bRefresh);

	DetailsView.SetGenericLayoutDetailsDelegate(MakeLambdaDelegate([this, Guid]() -> TSharedRef<IDetailCustomization>
	{
		return MakeVoxelShared<FVoxelMaterialDefinitionParameterSelectionCustomization>(Guid);
	}));
	DetailsView.ForceRefresh();
}