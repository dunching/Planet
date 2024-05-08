// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelGraph.h"
#include "VoxelActor.h"
#include "VoxelRuntimeGraph.h"
#include "VoxelParameterView.h"
#if WITH_EDITOR
#include "ObjectTools.h"
#endif

DEFINE_VOXEL_FACTORY(UVoxelGraph);

UVoxelGraph::UVoxelGraph()
{
	RuntimeGraph = CreateDefaultSubobject<UVoxelRuntimeGraph>("RuntimeGraph");

	for (const EVoxelGraphParameterType Type : TEnumRange<EVoxelGraphParameterType>())
	{
		ParametersCategories.Add(Type, {});
	}
}

FString UVoxelGraph::GetGraphName() const
{
	if (bEnableNameOverride)
	{
		return NameOverride;
	}

	return Super::GetGraphName();
}

void UVoxelGraph::SetGraphName(const FString& NewName)
{
	bEnableNameOverride = true;
	NameOverride = NewName;

	if (NameOverride.IsEmpty())
	{
		NameOverride = "Graph";
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void UVoxelGraph::ForceRecompile()
{
	RuntimeGraph->ForceRecompile();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void UVoxelGraph::PostLoad()
{
	VOXEL_FUNCTION_COUNTER();

	Super::PostLoad();

#if WITH_EDITOR
	if (!MainEdGraph)
	{
		MainEdGraph = Graphs_DEPRECATED.FindRef("Main");
	}
#endif

	if (Graph_DEPRECATED)
	{
		FObjectDuplicationParameters DuplicationParameters(Graph_DEPRECATED, GetOuter());
		DuplicationParameters.DuplicationSeed.Add(Graph_DEPRECATED, this);
		StaticDuplicateObjectEx(DuplicationParameters);
	}

	if (NameOverride.IsEmpty())
	{
		NameOverride = "Graph";
	}

	FixupParameters();
	FixupInlineMacroCategories();
}

void UVoxelGraph::PostCDOContruct()
{
	Super::PostCDOContruct();

	// Force load the deprecated object even if Export.bIsInheritedInstance is true
	NewObject<UVoxelGraph>(this, "Graph");
}

#if WITH_EDITOR
void UVoxelGraph::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	VOXEL_FUNCTION_COUNTER();

	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.ChangeType == EPropertyChangeType::Interactive)
	{
		return;
	}

#if WITH_EDITOR
	if (!bEnableThumbnail)
	{
		ThumbnailTools::CacheEmptyThumbnail(GetFullName(), GetOutermost());
	}
#endif

	FixupParameters();
	FixupInlineMacroCategories();

	if (PropertyChangedEvent.GetMemberPropertyName() == GET_OWN_MEMBER_NAME(Category))
	{
		if (UVoxelGraph* ParentGraph = GetTypedOuter<UVoxelGraph>())
		{
			ParentGraph->FixupInlineMacroCategories();
			ParentGraph->OnParametersChanged.Broadcast(EParameterChangeType::Unknown);
		}
	}

	if (PropertyChangedEvent.Property &&
		PropertyChangedEvent.Property->Owner == FVoxelPinValue::StaticStruct() &&
		PropertyChangedEvent.GetMemberPropertyName() == GET_OWN_MEMBER_NAME(Parameters))
	{
		OnParametersChanged.Broadcast(EParameterChangeType::DefaultValue);
	}
	else
	{
		OnParametersChanged.Broadcast(EParameterChangeType::Unknown);
	}
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class FVoxelGraphParameterView : public IVoxelParameterView
{
public:
	TWeakObjectPtr<UVoxelGraph> Graph;
	FGuid Guid;

	using IVoxelParameterView::IVoxelParameterView;

	virtual const FVoxelParameter* GetParameter() const override
	{
		if (!ensure(Graph.IsValid()))
		{
			return nullptr;
		}

		const FVoxelGraphParameter* Parameter = Graph->FindParameterByGuid(Guid);
		if (!ensureVoxelSlow(Parameter))
		{
			return nullptr;
		}

		return Parameter;
	}
};

class FVoxelGraphParameterRootView : public IVoxelParameterRootView
{
public:
	TWeakObjectPtr<UVoxelGraph> Graph;
	FVoxelParameterPath BasePath;
	TVoxelMap<FGuid, TSharedPtr<FVoxelGraphParameterView>> GuidToParameterView;

	using IVoxelParameterRootView::IVoxelParameterRootView;

	virtual TVoxelArray<IVoxelParameterView*> GetChildren() override
	{
		VOXEL_FUNCTION_COUNTER();

		if (!ensure(Graph.IsValid()))
		{
			return {};
		}

		TVoxelArray<IVoxelParameterView*> Children;
		for (const FVoxelGraphParameter& Parameter : Graph->Parameters)
		{
			if (Parameter.ParameterType != EVoxelGraphParameterType::Parameter)
			{
				continue;
			}

			TSharedPtr<FVoxelGraphParameterView>& ParameterView = GuidToParameterView.FindOrAdd(Parameter.Guid);
			if (!ParameterView)
			{
				ParameterView = MakeVoxelShared<FVoxelGraphParameterView>(*this, BasePath.MakeChild(Parameter.Guid));
				ParameterView->Graph = Graph;
				ParameterView->Guid = Parameter.Guid;
			}
			Children.Add(ParameterView.Get());
		}
		return Children;
	}
	virtual IVoxelParameterView* FindByGuid(const FGuid& Guid) override
	{
		if (const TSharedPtr<FVoxelGraphParameterView> ParameterView = GuidToParameterView.FindRef(Guid))
		{
			return ParameterView.Get();
		}

		(void)GetChildren();
		return GuidToParameterView.FindRef(Guid).Get();
	}
	virtual const FVoxelParameterCategories* GetCategories() const override
	{
		if (!ensure(Graph.IsValid()))
		{
			return nullptr;
		}
		return &ConstCast(*Graph).ParametersCategories.FindOrAdd(EVoxelGraphParameterType::Parameter);
	}
};

void UVoxelGraph::AddOnChanged(const FSimpleDelegate& Delegate)
{
	OnParametersChanged.Add(MakeWeakDelegateDelegate(Delegate, [Delegate](const EParameterChangeType)
	{
		ensure(Delegate.ExecuteIfBound());
	}));
}

TSharedPtr<IVoxelParameterRootView> UVoxelGraph::GetParameterViewImpl(const FVoxelParameterPath& BasePath)
{
	const TSharedRef<FVoxelGraphParameterRootView> ParameterRootView = MakeVoxelShared<FVoxelGraphParameterRootView>(this);
	ParameterRootView->Graph = this;
	ParameterRootView->BasePath = BasePath;
	return ParameterRootView;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void UVoxelGraph::FixupParameters()
{
	VOXEL_FUNCTION_COUNTER();

	for (FVoxelGraphParameter& Parameter : Parameters)
	{
		if (Parameter.ParameterType == EVoxelGraphParameterType::Parameter &&
			!ensure(!Parameter.Type.IsBuffer() || Parameter.Type.IsBufferArray()))
		{
			Parameter.Type = Parameter.Type.GetInnerType();
		}
	}

	FVoxelParameter::FixupParameterArray(this, Parameters);

#if WITH_EDITOR
	for (FVoxelGraphParameter& Parameter : Parameters)
	{
		if (Parameter.ParameterType != EVoxelGraphParameterType::Parameter)
		{
			continue;
		}

		if (ParameterGraphs.FindRef(Parameter.Guid))
		{
			continue;
		}

		UVoxelGraph* NewParameterGraph = NewObject<UVoxelGraph>(this, NAME_None, RF_Transactional);
		NewParameterGraph->bIsParameterGraph = true;

		ParameterGraphs.Add(Parameter.Guid, NewParameterGraph);
	}

	for (auto It = ParameterGraphs.CreateIterator(); It; ++It)
	{
		if (Parameters.FindByKey(It.Key()))
		{
			continue;
		}

		It.RemoveCurrent();
	}
#endif

	FixupCategories();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#if WITH_EDITOR
TArray<UVoxelGraph*> UVoxelGraph::GetAllGraphs()
{
	TArray<UVoxelGraph*> Graphs;
	Graphs.Add(this);

	for (UVoxelGraph* InlineMacro : InlineMacros)
	{
		if (!ensure(InlineMacro))
		{
			continue;
		}

		Graphs.Add(InlineMacro);
	}

	for (const auto& It : ParameterGraphs)
	{
		if (!ensure(It.Value))
		{
			continue;
		}

		Graphs.Add(It.Value);
	}

	return Graphs;
}

UVoxelGraph* UVoxelGraph::FindGraph(const UEdGraph* EdGraph)
{
	for (UVoxelGraph* Graph : GetAllGraphs())
	{
		if (EdGraph == Graph->MainEdGraph)
		{
			return Graph;
		}
	}
	return nullptr;
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void UVoxelGraph::FixupCategories()
{
	VOXEL_FUNCTION_COUNTER();

	TMap<EVoxelGraphParameterType, TArray<FString>> CategoriesListByType;
	for (const FVoxelGraphParameter& Parameter : Parameters)
	{
		CategoriesListByType.FindOrAdd(Parameter.ParameterType).Add(Parameter.Category);
	}

	for (const EVoxelGraphParameterType Type : TEnumRange<EVoxelGraphParameterType>())
	{
		ParametersCategories.FindOrAdd(Type).Fixup(CategoriesListByType.FindOrAdd(Type));
	}
}

void UVoxelGraph::FixupInlineMacroCategories()
{
	VOXEL_FUNCTION_COUNTER();

	TArray<FString> CategoriesList;
	for (const UVoxelGraph* InlineMacro : InlineMacros)
	{
		if (!ensure(InlineMacro))
		{
			continue;
		}

		CategoriesList.Add(InlineMacro->Category);
	}

	InlineMacroCategories.Fixup(CategoriesList);
}