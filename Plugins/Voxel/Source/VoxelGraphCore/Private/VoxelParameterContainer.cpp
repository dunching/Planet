// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelParameterContainer.h"
#include "VoxelParameterView.h"
#include "VoxelInlineGraph.h"

void UVoxelParameterContainer::SetProvider(IVoxelParameterProvider* NewProvider)
{
	VOXEL_FUNCTION_COUNTER();

	Provider = Cast<UObject>(NewProvider);
	Fixup();
}

void UVoxelParameterContainer::Fixup()
{
	VOXEL_FUNCTION_COUNTER();

	DelegatePtr.Reset();
	FixupProvider();

	if (LastProvider != Provider)
	{
		LastProvider = Provider;
		OnProviderChanged.Broadcast();
	}

	for (auto& It : ValueOverrides)
	{
		ensure(!It.Value.Value.Is<FVoxelInlineGraph>());

		// Don't pass an outer, values stored as override should never need one
		It.Value.Value.Fixup(nullptr);
	}

	if (!Provider)
	{
		return;
	}

	IVoxelParameterProvider* TypedProvider = Cast<IVoxelParameterProvider>(Provider);
	if (!ensure(TypedProvider))
	{
		return;
	}

	DelegatePtr = MakeSharedVoid();
	TypedProvider->AddOnChanged(MakeWeakPtrDelegate(DelegatePtr, [this]
	{
		Fixup();
		OnChanged.Broadcast();
	}));

	const TSharedPtr<IVoxelParameterRootView> ParameterRootView = TypedProvider->GetParameterView();
	if (!ensure(ParameterRootView))
	{
		return;
	}
	ParameterRootView->GetContext().AddContainer(FVoxelParameterContainerRef::MakeRoot(this));

	for (auto It = ValueOverrides.CreateIterator(); It; ++It)
	{
		const FVoxelParameterPath Path = It.Key();
		FVoxelParameterValueOverride& ParameterOverrideValue = It.Value();

		if (!ensureVoxelSlow(ParameterOverrideValue.Value.IsValid()))
		{
			It.RemoveCurrent();
			continue;
		}

		if (bAlwaysEnabled)
		{
			ParameterOverrideValue.bEnable = true;
		}

		ParameterRootView->GetContext().AddValueOverrideToIgnore(FVoxelParameterContainerRef::MakeRoot(this), Path);
		ON_SCOPE_EXIT
		{
			ParameterRootView->GetContext().RemoveValueOverrideToIgnore(FVoxelParameterContainerRef::MakeRoot(this), Path);
		};

		// We don't want to store inline graphs overrides, but override of their children
		// using the right parameter path instead
		ensure(!ParameterOverrideValue.Value.Is<FVoxelInlineGraph>());
		ParameterOverrideValue.Value.Fixup(this);

		const IVoxelParameterView* ParameterView = ParameterRootView->FindChild(Path);
		if (!ParameterView)
		{
			// Orphan
			continue;
		}

		ParameterOverrideValue.CachedName = ParameterView->GetName();
		ParameterOverrideValue.CachedCategory = FName(ParameterView->GetCategory());

		if (!ParameterOverrideValue.Value.GetType().CanBeCastedTo(ParameterView->GetType().GetExposedType()))
		{
			// Type changed: move orphan to a new GUID
			ValueOverrides.Add(
				Path.GetParent().MakeChild(FGuid::NewGuid()),
				ParameterOverrideValue);
			It.RemoveCurrent();
			continue;
		}

		const FVoxelPinValue Value = ParameterView->GetValue();
		if (!ensure(Value.IsValid()))
		{
			continue;
		}

		if (!bAlwaysEnabled &&
			ParameterOverrideValue.bEnable)
		{
			// Explicitly enabled: never reset to default
			continue;
		}

		if (ParameterOverrideValue.Value == Value)
		{
			// Back to default value, no need to store an override
			It.RemoveCurrent();
		}
	}
}

void UVoxelParameterContainer::FixupProvider()
{
	static TSet<IVoxelParameterProvider*> Visited;
	ON_SCOPE_EXIT
	{
		Visited.Reset();
	};

	IVoxelParameterProvider* It = this;
	while (It)
	{
		if (Visited.Contains(It))
		{
			VOXEL_MESSAGE(Error, "Hierarchy loop detected: {0}", Visited.Array());
			Provider = nullptr;
			return;
		}
		Visited.Add(It);

		const UVoxelParameterContainer* ParameterContainer = Cast<UVoxelParameterContainer>(It);
		if (!ParameterContainer)
		{
			ParameterContainer = Cast<UVoxelParameterContainer>(It->GetSourceProvider());
		}
		if (!ParameterContainer)
		{
			break;
		}

		It = Cast<IVoxelParameterProvider>(ParameterContainer->Provider);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void UVoxelParameterContainer::PostLoad()
{
	VOXEL_FUNCTION_COUNTER();

	Super::PostLoad();

	LastProvider = Provider;
	Fixup();
}

#if WITH_EDITOR
void UVoxelParameterContainer::PostEditUndo()
{
	VOXEL_FUNCTION_COUNTER();

	Super::PostEditUndo();

	Fixup();
	OnChanged.Broadcast();
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class FVoxelParameterContainerRootView : public IVoxelParameterRootView
{
public:
	TWeakObjectPtr<UVoxelParameterContainer> ParameterContainer;
	FVoxelParameterPath BasePath;

	using IVoxelParameterRootView::IVoxelParameterRootView;

	virtual const FVoxelParameterCategories* GetCategories() const override
	{
		if (!ensure(ParameterContainer.IsValid()))
		{
			return nullptr;
		}

		const IVoxelParameterRootView* Source = GetContext().MakeRootView(
			BasePath,
			Cast<IVoxelParameterProvider>(ParameterContainer->Provider));
		if (!Source)
		{
			return {};
		}
		return Source->GetCategories();
	}
	virtual TVoxelArray<IVoxelParameterView*> GetChildren() override
	{
		if (!ensure(ParameterContainer.IsValid()))
		{
			return {};
		}

		IVoxelParameterRootView* Source = GetContext().MakeRootView(
			BasePath,
			Cast<IVoxelParameterProvider>(ParameterContainer->Provider));
		if (!Source)
		{
			return {};
		}
		return Source->GetChildren();
	}
};

void UVoxelParameterContainer::AddOnChanged(const FSimpleDelegate& Delegate)
{
	OnChanged.Add(Delegate);
}

TSharedPtr<IVoxelParameterRootView> UVoxelParameterContainer::GetParameterViewImpl(const FVoxelParameterPath& BasePath)
{
	const TSharedRef<FVoxelParameterContainerRootView> ParameterRootView = MakeVoxelShared<FVoxelParameterContainerRootView>(this);
	ParameterRootView->GetContext().AddContainer(FVoxelParameterContainerRef::Make(BasePath, this));
	ParameterRootView->ParameterContainer = this;
	ParameterRootView->BasePath = BasePath;
	return ParameterRootView;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool UVoxelParameterContainer::Set(
	const FName Name,
	FVoxelPinValue Value,
	FString* OutError)
{
	VOXEL_FUNCTION_COUNTER();

	const TSharedPtr<IVoxelParameterRootView> ParameterRootView = GetParameterView();
	if (!ensure(ParameterRootView))
	{
		return false;
	}

	const IVoxelParameterView* ParameterView = ParameterRootView->FindByName(Name);
	if (!ParameterView)
	{
		if (OutError)
		{
			*OutError =
				"Failed to find parameter " + Name.ToString() + ". Valid parameters: " +
				FString::Join(ParameterRootView->GetValidParameters(), TEXT(","));
		}
		return false;
	}

	const FVoxelPinType ExposedType = ParameterView->GetType().GetExposedType();
	if (ExposedType.Is<float>() &&
		Value.Is<double>())
	{
		// Implicitly convert from double to float for convenience with blueprints
		Value = FVoxelPinValue::Make<float>(Value.Get<double>());
	}

	if (!Value.CanBeCastedTo(ExposedType))
	{
		if (OutError)
		{
			*OutError =
				"Invalid parameter type for " + Name.ToString() + ". Parameter has type " + ExposedType.ToString() +
				", but value of type " + Value.GetType().ToString() + " was passed";
		}
		return false;
	}

	// Always enabled
	FVoxelParameterValueOverride ValueOverride;
	ValueOverride.bEnable = true;
	ValueOverride.Value = Value;
	ValueOverrides.Add(ParameterView->Path, ValueOverride);

	OnChanged.Broadcast();
	return true;
}