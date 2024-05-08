// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelInlineGraph.h"
#include "VoxelGraph.h"
#include "VoxelDependency.h"
#include "VoxelGraphExecutor.h"
#include "VoxelParameterValues.h"
#include "VoxelFunctionCallNode.h"
#include "VoxelParameterContainer.h"

DEFINE_VOXEL_INSTANCE_COUNTER(FVoxelInlineGraphData);

TSharedRef<FVoxelInlineGraphData> FVoxelInlineGraphData::Create(
	const TWeakInterfacePtr<IVoxelParameterProvider>& Provider,
	const TOptional<FVoxelParameterPath>& ParameterPath)
{
	VOXEL_FUNCTION_COUNTER();

	const TSharedRef<FVoxelInlineGraphData> Result = MakeVoxelShareable(new (GVoxelMemory) FVoxelInlineGraphData(
		Provider,
		FVoxelDependency::Create(STATIC_FNAME("InlineGraph"), {}),
		ParameterPath,
		ParameterPath.IsSet()
		? nullptr
		: TSharedPtr<FVoxelParameterValues>(FVoxelParameterValues::Create(Provider))));
	Result->Update();
	return Result;
}

TSharedPtr<const FVoxelComputeValue> FVoxelInlineGraphData::FindOutput(
	const FVoxelQuery& Query,
	const FName OutputName) const
{
	Query.GetDependencyTracker().AddDependency(Dependency);

	VOXEL_SCOPE_LOCK(CriticalSection);
	return OutputNameToCompute_RequiresLock.FindRef(OutputName);
}

TVoxelArray<FName> FVoxelInlineGraphData::GetValidOutputNames(const FVoxelQuery& Query) const
{
	Query.GetDependencyTracker().AddDependency(Dependency);

	VOXEL_SCOPE_LOCK(CriticalSection);

	TVoxelArray<FName> Names;
	OutputNameToCompute_RequiresLock.GenerateKeyArray(Names);
	return Names;
}

TWeakObjectPtr<UVoxelGraphInterface> FVoxelInlineGraphData::GetWeakGraphInterface(const FVoxelQuery& Query) const
{
	Query.GetDependencyTracker().AddDependency(Dependency);

	VOXEL_SCOPE_LOCK(CriticalSection);
	return WeakGraphInterface_RequiresLock;
}

void FVoxelInlineGraphData::Update()
{
	VOXEL_FUNCTION_COUNTER();
	FVoxelDependencyInvalidationScope InvalidationScope;
	VOXEL_SCOPE_LOCK(CriticalSection);
	ensure(ParameterPath.IsSet() != ParameterValues.IsValid());

	OutputNameToCompute_RequiresLock.Reset();
	DummyPtr_RequiresLock = MakeVoxelShared<int32>();

	if (IVoxelParameterProvider* Provider = WeakProvider.Get())
	{
		if (UVoxelParameterContainer* ParameterContainer = Cast<UVoxelParameterContainer>(Provider))
		{
			ParameterContainer->OnProviderChanged.Add(MakeWeakPtrDelegate(DummyPtr_RequiresLock, [this]
			{
				Update();
			}));

			if (ParameterContainer->Provider != WeakGraphInterface_RequiresLock)
			{
				WeakGraphInterface_RequiresLock = ParameterContainer->GetTypedProvider<UVoxelGraphInterface>();
				Dependency->Invalidate();
			}
		}
		else if (UVoxelGraphInterface* GraphInterface = Cast<UVoxelGraphInterface>(Provider))
		{
			WeakGraphInterface_RequiresLock = GraphInterface;
		}
		else
		{
			ensure(false);
		}
	}

	const UVoxelGraphInterface* GraphInterface = WeakGraphInterface_RequiresLock.Get();
	if (!GraphInterface)
	{
		return;
	}

	const UVoxelGraph* Graph = GraphInterface->GetGraph();
	if (!Graph)
	{
		return;
	}

	Graph->OnParametersChanged.Add(MakeWeakPtrDelegate(DummyPtr_RequiresLock, [this](const UVoxelGraph::EParameterChangeType ChangeType)
	{
		if (ChangeType != UVoxelGraph::EParameterChangeType::DefaultValue)
		{
			FVoxelDependencyInvalidationScope LocalInvalidationScope;
			Update();
			Dependency->Invalidate();
		}
	}));

	for (const FVoxelGraphParameter& Parameter : Graph->Parameters)
	{
		if (Parameter.ParameterType != EVoxelGraphParameterType::Output)
		{
			continue;
		}

		const FVoxelGraphNodeRef NodeRef
		{
			Graph,
			// See FVoxelRuntimeNode::GetNodeId
			FName("Output." + Parameter.Name.ToString())
		};
		const FVoxelGraphPinRef PinRef
		{
			NodeRef,
			VOXEL_PIN_NAME(FVoxelNode_FunctionCallOutput, ValuePin)
		};

		const TSharedRef<const FVoxelComputeValue> Compute = GVoxelGraphExecutorManager->MakeCompute_GameThread(Parameter.Type, PinRef);
		OutputNameToCompute_RequiresLock.Add(Parameter.Name, Compute);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelInlineGraph::Fixup(UObject* Outer)
{
	VOXEL_FUNCTION_COUNTER();

	if (!Outer)
	{
		return;
	}

	if (!ParameterContainer)
	{
		ParameterContainer = NewObject<UVoxelParameterContainer>(Outer);
	}
	if (ParameterContainer->GetOuter() != Outer)
	{
		ParameterContainer = DuplicateObject<UVoxelParameterContainer>(ParameterContainer, Outer);
	}

	ParameterContainer->SetFlags(RF_Transactional);
	ParameterContainer->Fixup();
}

void FVoxelInlineGraph::ComputeRuntimeData()
{
	ensure(!Data);

	if (!ParameterContainer)
	{
		return;
	}

	Data = FVoxelInlineGraphData::Create(ParameterContainer, {});
	ParameterContainer = nullptr;
}

void FVoxelInlineGraph::ComputeExposedData()
{
	ensure(!ParameterContainer);

	if (!Data)
	{
		return;
	}

	ParameterContainer = Cast<UVoxelParameterContainer>(Data->WeakProvider.Get());
	ensure(ParameterContainer);

	Data = nullptr;
}