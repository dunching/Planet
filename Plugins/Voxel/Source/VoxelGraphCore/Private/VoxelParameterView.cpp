// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelParameterView.h"
#include "VoxelInlineGraph.h"
#include "VoxelParameterContainer.h"

IVoxelParameterView* IVoxelParameterViewBase::FindByGuid(const FGuid& Guid)
{
	IVoxelParameterView* FoundParameterView = nullptr;
	for (IVoxelParameterView* ParameterView : GetChildren())
	{
		if (ParameterView->GetGuid() != Guid)
		{
			continue;
		}

		ensure(!FoundParameterView);
		FoundParameterView = ParameterView;
	}
	return FoundParameterView;
}

IVoxelParameterView* IVoxelParameterViewBase::FindByName(const FName Name)
{
	IVoxelParameterView* FoundParameterView = nullptr;
	for (IVoxelParameterView* ParameterView : GetChildren())
	{
		if (ParameterView->GetName() != Name)
		{
			continue;
		}

		ensure(!FoundParameterView);
		FoundParameterView = ParameterView;
	}
	return FoundParameterView;
}

IVoxelParameterView* IVoxelParameterViewBase::FindChild(const FVoxelParameterPath& Path)
{
	if (!ensure(Path.Guids.Num() > 0))
	{
		return nullptr;
	}

	IVoxelParameterViewBase* ParameterView = this;
	for (const FGuid& Guid : Path.Guids)
	{
		ParameterView = ParameterView->FindByGuid(Guid);

		if (!ParameterView)
		{
			return nullptr;
		}
	}

	IVoxelParameterView* Result = static_cast<IVoxelParameterView*>(ParameterView);
	ensure(Result->Path == Path);
	return Result;
}

TArray<FString> IVoxelParameterViewBase::GetValidParameters()
{
	TArray<FString> ValidParameters;
	for (const IVoxelParameterView* ParameterView : GetChildren())
	{
		ValidParameters.Add(ParameterView->GetName().ToString() + " (" + ParameterView->GetType().GetExposedType().ToString() + ")");
	}
	return ValidParameters;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TVoxelArray<TVoxelArray<IVoxelParameterView*>> IVoxelParameterViewBase::GetCommonChildren(const TConstVoxelArrayView<IVoxelParameterViewBase*> ParameterViews)
{
	VOXEL_FUNCTION_COUNTER();

	bool bPathsSet = false;
	TSet<FGuid> Guids;
	for (IVoxelParameterViewBase* ParameterView : ParameterViews)
	{
		const TVoxelArray<IVoxelParameterView*> Children = ParameterView->GetChildren();

		TSet<FGuid> NewGuids;
		NewGuids.Reserve(Children.Num());

		for (const IVoxelParameterView* Child : Children)
		{
			ensure(!NewGuids.Contains(Child->Path.Leaf()));
			NewGuids.Add(Child->Path.Leaf());
		}

		if (bPathsSet)
		{
			Guids = Guids.Intersect(NewGuids);
		}
		else
		{
			bPathsSet = true;
			Guids = MoveTemp(NewGuids);
		}
	}

	const TArray<FGuid> GuidArray = Guids.Array();

	TVoxelArray<TVoxelArray<IVoxelParameterView*>> Result;
	Result.SetNum(GuidArray.Num());

	for (IVoxelParameterViewBase* ParameterView : ParameterViews)
	{
		for (int32 Index = 0; Index < GuidArray.Num(); Index++)
		{
			const FGuid Guid = GuidArray[Index];

			IVoxelParameterView* ChildParameterView = ParameterView->FindByGuid(Guid);
			if (!ensure(ChildParameterView))
			{
				return {};
			}

			Result[Index].Add(ChildParameterView);
		}
	}

	return Result;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool IVoxelParameterView::IsInlineGraph() const
{
	const FVoxelParameter* Parameter = GetParameter();
	if (!ensure(Parameter))
	{
		return false;
	}

	return Parameter->Type.Is<FVoxelInlineGraph>();
}

FGuid IVoxelParameterView::GetGuid() const
{
	const FVoxelParameter* Parameter = GetParameter();
	if (!ensureVoxelSlow(Parameter))
	{
		return {};
	}

	ensure(Parameter->Guid == Path.Leaf());
	return Parameter->Guid;
}

FName IVoxelParameterView::GetName() const
{
	const FVoxelParameter* Parameter = GetParameter();
	if (!ensureVoxelSlow(Parameter))
	{
		return {};
	}

	return Parameter->Name;
}

FVoxelPinType IVoxelParameterView::GetType() const
{
	if (IsInlineGraph())
	{
		return FVoxelPinType::Make<UVoxelGraphInterface>();
	}

	const FVoxelParameter* Parameter = GetParameter();
	if (!ensure(Parameter))
	{
		return {};
	}

	return Parameter->Type;
}

FString IVoxelParameterView::GetCategory() const
{
	const FVoxelParameter* Parameter = GetParameter();
	if (!ensure(Parameter))
	{
		return {};
	}
	return Parameter->Category;
}

FString IVoxelParameterView::GetDescription() const
{
	const FVoxelParameter* Parameter = GetParameter();
	if (!ensure(Parameter))
	{
		return {};
	}
	return Parameter->Description;
}

FVoxelPinValue IVoxelParameterView::GetValue() const
{
	VOXEL_FUNCTION_COUNTER();

	const FVoxelParameter* Parameter = GetParameter();
	if (!ensureVoxelSlow(Parameter))
	{
		return {};
	}

	if (IsInlineGraph())
	{
		if (const FVoxelParameterValueOverride* ValueOverride = RootView.GetContext().FindValueOverride(Path))
		{
			ensure(ValueOverride->Value.Is<UVoxelGraphInterface>());
			return ValueOverride->Value;
		}

		if (!ensure(Parameter->DefaultValue.Is<FVoxelInlineGraph>()))
		{
			return {};
		}

		UVoxelParameterContainer* ParameterContainer = Parameter->DefaultValue.Get<FVoxelInlineGraph>().ParameterContainer;
		if (!ensure(ParameterContainer))
		{
			return {};
		}

		RootView.GetContext().VisitedProviders.Add(ParameterContainer);

		return FVoxelPinValue::Make(ParameterContainer->GetTypedProvider<UVoxelGraphInterface>());
	}

	if (const FVoxelParameterValueOverride* ValueOverride = RootView.GetContext().FindValueOverride(Path))
	{
		return ValueOverride->Value;
	}

	return Parameter->DefaultValue;
}

#if WITH_EDITOR
TMap<FName, FString> IVoxelParameterView::GetMetaData() const
{
	const FVoxelParameter* Parameter = GetParameter();
	if (!ensure(Parameter))
	{
		return {};
	}
	return Parameter->MetaData;
}
#endif

FVoxelParameter IVoxelParameterView::GetAsParameter() const
{
	FVoxelParameter Result;
	Result.Guid = GetGuid();
	Result.Name = GetName();
	Result.Type = GetType();
	Result.Category = GetCategory();
	Result.Description = GetDescription();
	Result.DefaultValue = GetValue();
#if WITH_EDITOR
	Result.MetaData = GetMetaData();
#endif
	return Result;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TVoxelArray<IVoxelParameterView*> IVoxelParameterView::GetChildren()
{
	IVoxelParameterRootView* InlineRootView = GetInlineRootView();
	if (!InlineRootView)
	{
		return {};
	}
	return InlineRootView->GetChildren();
}

IVoxelParameterView* IVoxelParameterView::FindByGuid(const FGuid& Guid)
{
	IVoxelParameterRootView* InlineRootView = GetInlineRootView();
	if (!InlineRootView)
	{
		return nullptr;
	}
	return InlineRootView->FindByGuid(Guid);
}

IVoxelParameterView* IVoxelParameterView::FindByName(const FName Name)
{
	IVoxelParameterRootView* InlineRootView = GetInlineRootView();
	if (!InlineRootView)
	{
		return nullptr;
	}
	return InlineRootView->FindByName(Name);
}

const FVoxelParameterCategories* IVoxelParameterView::GetCategories() const
{
	const IVoxelParameterRootView* InlineRootView = GetInlineRootView();
	if (!InlineRootView)
	{
		return nullptr;
	}
	return InlineRootView->GetCategories();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

IVoxelParameterRootView* IVoxelParameterView::GetInlineRootView() const
{
	if (!IsInlineGraph())
	{
		return nullptr;
	}

	const FVoxelPinValue Value = GetValue();
	if (!ensure(Value.IsValid()) ||
		!ensure(Value.Is<UVoxelGraphInterface>()))
	{
		return nullptr;
	}

	UVoxelGraphInterface* GraphInterface = Value.Get<UVoxelGraphInterface>();
	if (!GraphInterface)
	{
		return nullptr;
	}

	IVoxelParameterRootView* View = RootView.GetContext().MakeRootView(Path, GraphInterface);
	if (!ensure(View))
	{
		return nullptr;
	}

	return View;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelParameterContainerRef FVoxelParameterContainerRef::MakeRoot(UVoxelParameterContainer* Container)
{
	ensure(Container);
	FVoxelParameterContainerRef Result;
	Result.Container = Container;
	return Result;
}

FVoxelParameterContainerRef FVoxelParameterContainerRef::Make(const FVoxelParameterPath& BasePath, UVoxelParameterContainer* Container)
{
	ensure(Container);
	FVoxelParameterContainerRef Result;
	Result.BasePath = BasePath;
	Result.Container = Container;
	return Result;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

IVoxelParameterRootView* FVoxelParameterViewContext::MakeRootView(
	const FVoxelParameterPath& Path,
	IVoxelParameterProvider* Provider)
{
	if (!Provider)
	{
		return nullptr;
	}

	VisitedProviders.Add(Provider);

	TSharedPtr<IVoxelParameterRootView>& RootView = PathAndProviderToRootView.FindOrAdd({ Path, Provider });
	if (RootView)
	{
		return RootView.Get();
	}

	RootView = Provider->GetParameterViewImpl(Path);
	if (!ensure(RootView))
	{
		return nullptr;
	}

	Containers.Append(RootView->SharedContext->Containers);
	ensure(RootView->SharedContext->ContainerToValueOverridesToIgnore.Num() == 0);
	ensure(RootView->SharedContext->PathAndProviderToRootView.Num() == 0);

	RootView->SharedContext = nullptr;
	RootView->Context = this;
	return RootView.Get();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelParameterViewContext::AddContainer(const FVoxelParameterContainerRef& Container)
{
	Containers.Add(Container);
}

void FVoxelParameterViewContext::AddContainerToForceEnable(const FVoxelParameterContainerRef& Container)
{
	ensure(Containers.Contains(Container));
	ContainersToForceEnable.Add(Container);
}

void FVoxelParameterViewContext::AddValueOverrideToIgnore(const FVoxelParameterContainerRef& Container, const FVoxelParameterPath& Path)
{
	ensure(!ContainerToValueOverridesToIgnore.Contains(Container));
	ContainerToValueOverridesToIgnore.FindOrAdd(Container).Add(Path);
}

void FVoxelParameterViewContext::RemoveValueOverrideToIgnore(const FVoxelParameterContainerRef& Container, const FVoxelParameterPath& Path)
{
	ensure(ContainerToValueOverridesToIgnore.Contains(Container));
	ensure(ContainerToValueOverridesToIgnore.FindOrAdd(Container).Remove(Path));
	ensure(ContainerToValueOverridesToIgnore.FindOrAdd(Container).Num() == 0);
	ensure(ContainerToValueOverridesToIgnore.Remove(Container));
}

const FVoxelParameterValueOverride* FVoxelParameterViewContext::FindValueOverride(const FVoxelParameterPath& Path) const
{
	VOXEL_FUNCTION_COUNTER();

	for (const FVoxelParameterContainerRef& Container : Containers)
	{
		if (!Path.StartsWith(Container.BasePath))
		{
			continue;
		}
		const FVoxelParameterPath RelativePath = Path.MakeRelative(Container.BasePath);

		const UVoxelParameterContainer* ContainerObject = Container.Container.Get();
		if (!ensure(ContainerObject))
		{
			continue;
		}

		if (const TVoxelSet<FVoxelParameterPath>* ValueOverridesToIgnore = ContainerToValueOverridesToIgnore.Find(Container))
		{
			if (ValueOverridesToIgnore->Contains(RelativePath))
			{
				continue;
			}
		}

		const FVoxelParameterValueOverride* ValueOverride = ContainerObject->ValueOverrides.Find(RelativePath);
		if (!ValueOverride)
		{
			continue;
		}

		if (!ContainersToForceEnable.Contains(Container) &&
			!ValueOverride->bEnable)
		{
			continue;
		}

		return ValueOverride;
	}
	return nullptr;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

IVoxelParameterRootView::IVoxelParameterRootView(const TWeakInterfacePtr<IVoxelParameterProvider>& Provider)
{
	ensure(Provider.IsValid());

	SharedContext = MakeVoxelShared<FVoxelParameterViewContext>();
	Context = SharedContext.Get();
}