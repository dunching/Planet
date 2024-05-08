// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Customizations/VoxelParameterContainerDetails.h"
#include "Customizations/VoxelParameterDetails.h"
#include "VoxelParameterContainer.h"

DEFINE_VOXEL_INSTANCE_COUNTER(FVoxelParameterContainerDetails);

TSharedPtr<FVoxelParameterContainerDetails> FVoxelParameterContainerDetails::Create(
	const FVoxelDetailCategoryInterface& DetailCategoryInterface,
	const TSharedRef<IPropertyHandle>& ParameterContainerHandle,
	const TSharedPtr<SBox>& ProviderDropdownContainer)
{
	const TSharedRef<FVoxelParameterContainerDetails> Result = MakeVoxelShareable(new (GVoxelMemory) FVoxelParameterContainerDetails(ParameterContainerHandle));
	Result->Initialize();

	if (!ensure(Result->SharedParameterRootViews.Num() > 0))
	{
		return nullptr;
	}

	Result->Initialize(DetailCategoryInterface, ProviderDropdownContainer);
	return Result;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelParameterContainerDetails::Tick()
{
	VOXEL_FUNCTION_COUNTER();

	for (auto It = PathToParameterDetails.CreateIterator(); It; ++It)
	{
		const TSharedPtr<FVoxelParameterDetails> ParameterDetails = It.Value();
		if (!ParameterDetails->IsValid())
		{
			// Parent ParameterDetails was collapsed
			It.RemoveCurrent();
			continue;
		}

		ParameterDetails->Tick();

		if (!ParameterDetails->IsValid())
		{
			// Remove now to avoid ticking StructWrapper with an invalid handle
			It.RemoveCurrent();
		}
	}

	if (!bRefreshQueued)
	{
		return;
	}
	bRefreshQueued = false;

	// If categories changed, force full refresh
	const FVoxelParameterCategories* NewCategories = ParameterRootViews[0]->GetCategories();
	if (LastCategories.IsSet() != (NewCategories != nullptr))
	{
		ForceRefresh();
		return;
	}

	if (LastCategories &&
		NewCategories->Categories != *LastCategories)
	{
		ForceRefresh();
		return;
	}

	// If the number of root parameter changed, trigger a full refresh
	if (ParameterRootViewsCommonChildren != IVoxelParameterViewBase::GetCommonChildren(ParameterRootViews))
	{
		ForceRefresh();
		return;
	}

	// Check for type changes
	for (const auto& It : PathToParameterDetails)
	{
		const TSharedPtr<FVoxelParameterDetails> ParameterDetails = It.Value;
		if (ParameterDetails->IsOrphan())
		{
			continue;
		}

		const IVoxelParameterView* ParameterView = ParameterDetails->ParameterViews[0];
		if (!ensure(ParameterView))
		{
			return;
		}

		if (ParameterDetails->bIsInlineGraph != ParameterView->IsInlineGraph() ||
			ParameterDetails->RowExposedType != ParameterView->GetType().GetExposedType())
		{
			// Don't bother doing granular updates on type change
			ForceRefresh();
			return;
		}
	}

	// Check for layout changes
	TSet<FVoxelParameterPath> PathsToRebuild;
	for (const auto& It : PathToParameterDetails)
	{
		if (It.Value->ShouldRebuildChildren())
		{
			PathsToRebuild.Add(It.Value->Path);
		}
	}

	// Process children first
	PathsToRebuild.Sort([](const FVoxelParameterPath& A, const FVoxelParameterPath& B)
	{
		return A.Num() > B.Num();
	});

	// Remove children whose parents are going to be rebuilt
	for (auto It = PathsToRebuild.CreateIterator(); It; ++It)
	{
		if (It->Num() == 1)
		{
			continue;
		}
		ensure(It->Num() > 1);

		for (FVoxelParameterPath Path = It->GetParent(); Path.Num() > 1; Path = Path.GetParent())
		{
			if (PathsToRebuild.Contains(Path))
			{
				It.RemoveCurrent();
				break;
			}
		}
	}

	// Rebuild
	for (const FVoxelParameterPath& Path : PathsToRebuild)
	{
		const TSharedPtr<FVoxelParameterDetails> ParameterDetails = PathToParameterDetails.FindRef(Path);
		if (!ensure(ParameterDetails))
		{
			continue;
		}

		ParameterDetails->RebuildChildren();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TVoxelArray<UVoxelParameterContainer*> FVoxelParameterContainerDetails::GetParameterContainers() const
{
	TVoxelArray<UVoxelParameterContainer*> Result;
	for (const TWeakObjectPtr<UVoxelParameterContainer> ParameterContainer : WeakParameterContainers)
	{
		if (ensure(ParameterContainer.IsValid()))
		{
			Result.Add(ParameterContainer.Get());
		}
	}
	return Result;
}

void FVoxelParameterContainerDetails::GenerateView(
	const TVoxelArray<IVoxelParameterView*>& ParameterViews,
	const FVoxelDetailInterface& DetailInterface)
{
	VOXEL_FUNCTION_COUNTER();

	if (!ensure(ParameterViews.Num() > 0))
	{
		return;
	}

	const FVoxelParameterPath Path = ParameterViews[0]->Path;
	for (const IVoxelParameterView* ParameterView : ParameterViews)
	{
		ensure(Path == ParameterView->Path);
	}

	const TSharedRef<FVoxelParameterDetails> ParameterDetails = MakeVoxelShared<FVoxelParameterDetails>(
		*this,
		Path,
		ParameterViews);
	PathToParameterDetails.Add(Path, ParameterDetails);

	ParameterDetails->MakeRow(DetailInterface);
}

void FVoxelParameterContainerDetails::AddOrphans(
	const FVoxelParameterPath& BasePath,
	const TConstVoxelArrayView<IVoxelParameterViewBase*> ParameterViews,
	const FVoxelDetailInterface& DetailInterface,
	const FName Category)
{
	VOXEL_FUNCTION_COUNTER();

	bool bCommonPathSet = false;
	TVoxelSet<FVoxelParameterPath> CommonPaths;

	for (UVoxelParameterContainer* ParameterContainer : GetParameterContainers())
	{
		TVoxelSet<FVoxelParameterPath> Paths;
		for (auto& It : ParameterContainer->ValueOverrides)
		{
			if (It.Key.GetParent() != BasePath ||
				It.Value.CachedCategory != Category)
			{
				continue;
			}

			Paths.Add(It.Key);
		}

		if (bCommonPathSet)
		{
			CommonPaths = CommonPaths.Intersect(Paths);
		}
		else
		{
			bCommonPathSet = true;
			CommonPaths = MoveTemp(Paths);
		}
	}

	for (const FVoxelParameterPath& Path : CommonPaths)
	{
		ensure(Path.GetParent() == BasePath);

		bool bIsAnyValid = false;
		for (IVoxelParameterViewBase* ParameterView : ParameterViews)
		{
			if (ParameterView->FindByGuid(Path.Leaf()))
			{
				bIsAnyValid = true;
				break;
			}
		}

		if (bIsAnyValid)
		{
			// Not orphan for at least one of the views, skip
			continue;
		}

		FName Name;
		FVoxelPinValue Value;
		bool bHasSingleValue = true;
		{
			TVoxelArray<FVoxelParameterValueOverride> ValueOverrides;
			for (UVoxelParameterContainer* ParameterContainer : GetParameterContainers())
			{
				ValueOverrides.Add(ParameterContainer->ValueOverrides.FindChecked(Path));
			}

			if (!ensure(ValueOverrides.Num() > 0))
			{
				continue;
			}

			Name = ValueOverrides[0].CachedName;
			Value = ValueOverrides[0].Value;
			for (const FVoxelParameterValueOverride& ValueOverride : ValueOverrides)
			{
				if (Name != ValueOverride.CachedName)
				{
					Name = "Multiple Values";
				}
				if (Value != ValueOverride.Value)
				{
					bHasSingleValue = false;
				}
			}
		}

		const TSharedRef<FVoxelParameterDetails> ParameterDetails = MakeVoxelShared<FVoxelParameterDetails>(
			*this,
			Path,
			TVoxelArray<IVoxelParameterView*>());

		ParameterDetails->OrphanName = Name;
		ParameterDetails->OrphanExposedType = Value.GetType();
		ParameterDetails->InitializeOrphan(Value, bHasSingleValue);
		PathToParameterDetails.Add(Path, ParameterDetails);

		ParameterDetails->MakeRow(DetailInterface);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelParameterContainerDetails::Initialize()
{
	VOXEL_FUNCTION_COUNTER();

	ensure(ParameterContainerHandle->IsValidHandle());
	ParameterContainerHandle->MarkHiddenByCustomization();

	FVoxelEditorUtilities::ForeachData<TObjectPtr<UVoxelParameterContainer>>(
		ParameterContainerHandle,
		[&](const TObjectPtr<UVoxelParameterContainer>& ParameterContainer)
		{
			if (!ensure(ParameterContainer))
			{
				return;
			}

			ParameterContainer->Fixup();

			const TSharedPtr<IVoxelParameterRootView> ParameterRootView = ParameterContainer->GetParameterView();
			if (!ensure(ParameterRootView))
			{
				return;
			}

			// We want GetValue to show the override value even if it's disabled
			ParameterRootView->GetContext().AddContainerToForceEnable(FVoxelParameterContainerRef::MakeRoot(ParameterContainer));

			ParameterContainer->OnChanged.Add(MakeWeakPtrDelegate(this, [this]
			{
				bRefreshQueued = true;
			}));

			SharedParameterRootViews.Add(ParameterRootView.ToSharedRef());
			ParameterRootViews.Add(ParameterRootView.Get());
			WeakParameterContainers.Add(ParameterContainer);
			bAlwaysEnabled = ParameterContainer->bAlwaysEnabled;
		});

	ParameterRootViewsCommonChildren = IVoxelParameterViewBase::GetCommonChildren(ParameterRootViews);

	for (const UVoxelParameterContainer* ParameterContainer : GetParameterContainers())
	{
		ensure(ParameterContainer->bAlwaysEnabled == bAlwaysEnabled);
	}
}

void FVoxelParameterContainerDetails::Initialize(
	const FVoxelDetailCategoryInterface& DetailCategoryInterface,
	const TSharedPtr<SBox>& ProviderDropdownContainer)
{
	VOXEL_FUNCTION_COUNTER();

	if (DetailCategoryInterface.IsDetailLayout())
	{
		for (FProperty& Property : GetClassProperties(UActorComponent::StaticClass()))
		{
			if (const TSharedPtr<IPropertyHandle> PropertyHandle = DetailCategoryInterface.GetDetailLayout().GetProperty(Property.GetFName(), UActorComponent::StaticClass()))
			{
				PropertyHandle->MarkHiddenByCustomization();
			}
		}
	}

	RefreshDelegate = FVoxelEditorUtilities::MakeRefreshDelegate(ParameterContainerHandle, DetailCategoryInterface);

	if (const FVoxelParameterCategories* ParameterCategories = ParameterRootViews[0]->GetCategories())
	{
		LastCategories = ParameterCategories->Categories;
	}

	if (!ParameterContainerHandle->GetInstanceMetaData("HideProvider"))
	{
		if (DetailCategoryInterface.IsDetailLayout())
		{
			// Force graph at the bottom
			IDetailCategoryBuilder& Category = DetailCategoryInterface.GetDetailLayout().EditCategory(
				"Config",
				{},
				ECategoryPriority::Uncommon);
			AddProviderProperty(Category, ProviderDropdownContainer);
		}
		else if (ProviderDropdownContainer)
		{
			// Don't make a new category property just for a fake row
			AddProviderProperty(DetailCategoryInterface.GetDetailInterface(), ProviderDropdownContainer);
		}
		else
		{
			DetailCategoryInterface.EditCategory("Config", "Config", MakeWeakPtrLambda(this,
				[this](const FVoxelDetailInterface& DetailInterface)
				{
					AddProviderProperty(DetailInterface, nullptr);
				}));
		}
	}

	TMap<FName, TVoxelArray<TVoxelArray<IVoxelParameterView*>>> CategoryToAllChildParameterViews;
	for (const TVoxelArray<IVoxelParameterView*>& ChildParameterViews : ParameterRootViewsCommonChildren)
	{
		const FString Category = ChildParameterViews[0]->GetCategory();
		for (const IVoxelParameterView* ChildParameterView : ChildParameterViews)
		{
			ensure(ChildParameterView->GetCategory() == Category);
		}

		CategoryToAllChildParameterViews.FindOrAdd(FName(Category)).Add(ChildParameterViews);
	}

	for (UVoxelParameterContainer* ParameterContainer : GetParameterContainers())
	{
		for (const auto& It : ParameterContainer->ValueOverrides)
		{
			CategoryToAllChildParameterViews.FindOrAdd(It.Value.CachedCategory);
		}
	}

	if (const FVoxelParameterCategories* Categories = ParameterRootViews[0]->GetCategories())
	{
		const TArray<FName> CategoryArray(Categories->Categories);
		CategoryToAllChildParameterViews.KeySort([&](const FName CategoryA, const FName CategoryB)
		{
			return CategoryArray.IndexOfByKey(CategoryA) < CategoryArray.IndexOfByKey(CategoryB);
		});
	}

	const FString CategoryDelim = "|";
	for (const auto& It : CategoryToAllChildParameterViews)
	{
		TArray<FString> Categories;
		It.Key.ToString().ParseIntoArray(Categories, *CategoryDelim, true);

		DetailCategoryInterface.EditCategory(
			*Categories[0],
			*Categories[0],
			MakeWeakPtrLambda(this,
				[=](const FVoxelDetailInterface& DetailInterface)
				{
					AddParameters(DetailInterface, It.Key, Categories, It.Value, 1);
				}));
	}
}

void FVoxelParameterContainerDetails::AddParameters(
	const FVoxelDetailInterface& DetailInterface,
	const FName FullCategoryPath,
	const TArray<FString>& Categories,
	const TVoxelArray<TVoxelArray<IVoxelParameterView*>>& AllChildParameterViews,
	const int32 CategoryIndex)
{
	if (CategoryIndex < Categories.Num())
	{
		const FVoxelDetailCategoryInterface DetailCategoryInterface(DetailInterface);
		DetailCategoryInterface.EditCategory(
			*Categories[CategoryIndex],
			*Categories[CategoryIndex],
			MakeWeakPtrLambda(this,
				[=](const FVoxelDetailInterface& ChildDetailInterface)
				{
					AddParameters(ChildDetailInterface, FullCategoryPath, Categories, AllChildParameterViews, CategoryIndex + 1);
				}));
		return;
	}

	for (const TVoxelArray<IVoxelParameterView*>& ChildParameterViews : AllChildParameterViews)
	{
		GenerateView(ChildParameterViews, DetailInterface);
	}

	AddOrphans(
		{},
		ParameterRootViews,
		DetailInterface,
		FullCategoryPath);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelParameterContainerDetails::AddProviderProperty(
	const FVoxelDetailInterface& DetailInterface,
	const TSharedPtr<SBox>& ProviderDropdownContainer)
{
	VOXEL_FUNCTION_COUNTER();

	const FString ProviderClassPath = ParameterContainerHandle->GetMetaData("ProviderClass");
	const UClass* ProviderClass = FindObject<UClass>(nullptr, *ProviderClassPath);
	if (!ensure(ProviderClass))
	{
		return;
	}

	IDetailPropertyRow* Row = DetailInterface.AddExternalObjectProperty(
		TArray<UObject*>(GetParameterContainers()),
		GET_MEMBER_NAME_STATIC(UVoxelParameterContainer, Provider));
	if (!ensure(Row))
	{
		return;
	}

	Row->Visibility(EVisibility::Collapsed);

	const TSharedPtr<IPropertyHandle> PropertyHandle = Row->GetPropertyHandle();
	if (!ensure(PropertyHandle))
	{
		return;
	}

	FindFPropertyChecked(UVoxelParameterContainer, Provider).SetMetaData("AllowedClasses", *ProviderClass->GetPathName());
	const TSharedRef<SWidget> ValueWidget = PropertyHandle->CreatePropertyValueWidget();
	FindFPropertyChecked(UVoxelParameterContainer, Provider).RemoveMetaData("AllowedClasses");

	PropertyHandle->SetOnPropertyValuePreChange(MakeWeakPtrDelegate(this, [this]
	{
		VOXEL_SCOPE_COUNTER("OnPropertyValuePreChange");

		for (UVoxelParameterContainer* ParameterContainer : GetParameterContainers())
		{
			ParameterContainer->PreEditChange(nullptr);
		}
	}));
	PropertyHandle->SetOnPropertyValueChanged(MakeWeakPtrDelegate(this, [this]
	{
		VOXEL_SCOPE_COUNTER("OnPropertyValueChanged");

		for (UVoxelParameterContainer* ParameterContainer : GetParameterContainers())
		{
			ParameterContainer->Fixup();
			ParameterContainer->PostEditChange();
			ParameterContainer->OnChanged.Broadcast();
		}
	}));

	if (ProviderDropdownContainer)
	{
		ProviderDropdownContainer->SetContent(ValueWidget);
		return;
	}

	FString Name = ProviderClass->GetDisplayNameText().ToString();
	Name.RemoveFromStart("Voxel ");
	Name.RemoveFromEnd(" Interface");

	DetailInterface.AddCustomRow(FText::FromString(Name))
	.NameContent()
	[
		SNew(SVoxelDetailText)
		.Text(FText::FromString(Name))
	]
	.ValueContent()
	[
		ValueWidget
	];
}