// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelSurface.h"
#include "VoxelNodeHelpers.h"
#include "VoxelBufferUtilities.h"
#include "VoxelPositionQueryParameter.h"
#include "FunctionLibrary/VoxelSurfaceFunctionLibrary.h"

int32 FVoxelSurfaceMaterial::Num() const
{
	int32 Num = 1;
	for (const FLayer& Layer : Layers)
	{
		ensure(FVoxelBufferAccessor::MergeNum(Num, Layer.Material));
		ensure(FVoxelBufferAccessor::MergeNum(Num, Layer.Strength));
	}
	return Num;
}

int32 FVoxelSurfaceMaterial::Num_Slow() const
{
	return Num();
}

bool FVoxelSurfaceMaterial::IsValid_Slow() const
{
	int32 Num = 1;
	for (const FLayer& Layer : Layers)
	{
		if (!FVoxelBufferAccessor::MergeNum(Num, Layer.Material) ||
			!FVoxelBufferAccessor::MergeNum(Num, Layer.Strength))
		{
			return false;
		}
	}
	return true;
}

void FVoxelSurfaceMaterial::GetLayers(
	const FVoxelBufferIterator& Iterator,
	TVoxelArray<ispc::FVoxelSurfaceLayer>& OutLayers) const
{
	OutLayers.Reset(Layers.Num());

	for (const FLayer& Layer : Layers)
	{
		OutLayers.Add(
		{
			Layer.Material.IsConstant(),
			ReinterpretCastPtr<uint16>(Layer.Material.GetData(Iterator)),
			Layer.Strength.IsConstant(),
			Layer.Strength.GetData(Iterator)
		});
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelSurface FVoxelSurface::Make(
	const FVoxelGraphNodeRef& Node,
	const FVoxelBounds& Bounds)
{
	FVoxelSurface Result;
	Result.bIsValid = true;
	Result.Node = Node;
	Result.Bounds = Bounds;
	return Result;
}

FVoxelSurface FVoxelSurface::MakeInfinite(const FVoxelGraphNodeRef& Node)
{
	return Make(Node, FVoxelBounds::Infinite());
}

FVoxelSurface FVoxelSurface::MakeWithLocalBounds(
	const FVoxelGraphNodeRef& Node,
	const FVoxelQuery& Query,
	const FVoxelBox& Bounds)
{
	return Make(Node, FVoxelBounds(Bounds, Query.GetLocalToWorld()));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelSurface::CopyMaterialAttributes(const FVoxelSurface& Other)
{
	ComputeMaterial = Other.ComputeMaterial;
	NameToAttribute = Other.NameToAttribute;
}

void FVoxelSurface::LerpMaterialAttributes(
	const FVoxelQuery& InQuery,
	const FVoxelGraphNodeRef& NodeRef,
	const FVoxelSurface& A,
	const FVoxelSurface& B,
	const TSharedRef<const TVoxelComputeValue<FVoxelFloatBuffer>>& ComputeAlpha)
{
	VOXEL_FUNCTION_COUNTER();

	if (!A.ComputeMaterial)
	{
		ComputeMaterial = B.ComputeMaterial;
	}
	else if (!B.ComputeMaterial)
	{
		ComputeMaterial = A.ComputeMaterial;
	}
	else
	{
		SetMaterial(InQuery, NodeRef, [NodeRef, A, B, ComputeAlpha](const FVoxelQuery& Query)
		{
			const TVoxelFutureValue<FVoxelSurfaceMaterial> MaterialA = A.GetMaterial(Query);
			const TVoxelFutureValue<FVoxelSurfaceMaterial> MaterialB = B.GetMaterial(Query);
			const TVoxelFutureValue<FVoxelFloatBuffer> Alpha = (*ComputeAlpha)(Query);

			return
				MakeVoxelTask(STATIC_FNAME("SmoothMax Material"))
				.Dependencies(MaterialA, MaterialB, Alpha)
				.Execute<FVoxelSurfaceMaterial>([=]
				{
					return MakeVoxelFunctionCaller<UVoxelSurfaceFunctionLibrary>(NodeRef, Query)->BlendSurfaceMaterials(
						MaterialA.Get_CheckCompleted(),
						MaterialB.Get_CheckCompleted(),
						Alpha.Get_CheckCompleted());
				});
		});
	}

	TVoxelSet<FName> AttributeNames;
	{
		AttributeNames.Reserve(A.NameToAttribute.Num() + B.NameToAttribute.Num());

		for (const auto& It : A.NameToAttribute)
		{
			AttributeNames.Add(It.Key);
		}
		for (const auto& It : B.NameToAttribute)
		{
			AttributeNames.Add(It.Key);
		}
	}
	NameToAttribute.Reserve(AttributeNames.Num());

	for (const FName Name : AttributeNames)
	{
		const FAttribute* AttributeA = A.NameToAttribute.Find(Name);
		const FAttribute* AttributeB = B.NameToAttribute.Find(Name);

		if (!AttributeA)
		{
			NameToAttribute.Add(Name, *AttributeB);
			continue;
		}
		if (!AttributeB)
		{
			NameToAttribute.Add(Name, *AttributeA);
			continue;
		}

		if (AttributeA->InnerType != AttributeB->InnerType)
		{
			VOXEL_MESSAGE(Error, "{0}: Type mismatch for attribute {1}: {2} vs {3}",
				NodeRef,
				Name,
				AttributeA->InnerType.ToString(),
				AttributeB->InnerType.ToString());
			continue;
		}
		const FVoxelPinType InnerType = AttributeA->InnerType;
		checkVoxelSlow(!InnerType.IsBuffer());

		TWeakPtr<FVoxelDetailTexturePool> DetailTexturePool;
		if (!AttributeA->DetailTexturePool.IsValid())
		{
			DetailTexturePool = AttributeB->DetailTexturePool;
		}
		else if (!AttributeB->DetailTexturePool.IsValid())
		{
			DetailTexturePool = AttributeA->DetailTexturePool;
		}
		else if (AttributeA->DetailTexturePool == AttributeB->DetailTexturePool)
		{
			DetailTexturePool = AttributeA->DetailTexturePool;
		}
		else
		{
			VOXEL_MESSAGE(Error, "{0}: Different detail textures for attribute {1}", NodeRef, Name);
		}

		SetAttribute(
			Name,
			InnerType,
			DetailTexturePool,
			InQuery,
			NodeRef,
			[ComputeAlpha, ComputeA = AttributeA->Compute, ComputeB = AttributeB->Compute, InnerType](const FVoxelQuery& Query) -> TVoxelFutureValue<FVoxelBuffer>
		{
			const TVoxelFutureValue<FVoxelBuffer> ValueA = (*ComputeA)(Query);
			const TVoxelFutureValue<FVoxelBuffer> ValueB = (*ComputeB)(Query);
			const TVoxelFutureValue<FVoxelFloatBuffer> Alpha = (*ComputeAlpha)(Query);

			return
				MakeVoxelTask(STATIC_FNAME("Lerp Attribute"))
				.Dependencies(ValueA, ValueB, Alpha)
				.Execute<FVoxelBuffer>(InnerType.GetBufferType(), [=]
				{
					if (InnerType.Is<float>())
					{
						return FVoxelRuntimePinValue::Make(FVoxelBufferUtilities::Lerp(
							ValueA.Get_CheckCompleted<FVoxelFloatBuffer>(),
							ValueB.Get_CheckCompleted<FVoxelFloatBuffer>(),
							Alpha.Get_CheckCompleted()));
					}
					if (InnerType.Is<FVector2D>())
					{
						return FVoxelRuntimePinValue::Make(FVoxelBufferUtilities::Lerp(
							ValueA.Get_CheckCompleted<FVoxelVector2DBuffer>(),
							ValueB.Get_CheckCompleted<FVoxelVector2DBuffer>(),
							Alpha.Get_CheckCompleted()));
					}
					if (InnerType.Is<FVector>())
					{
						return FVoxelRuntimePinValue::Make(FVoxelBufferUtilities::Lerp(
							ValueA.Get_CheckCompleted<FVoxelVectorBuffer>(),
							ValueB.Get_CheckCompleted<FVoxelVectorBuffer>(),
							Alpha.Get_CheckCompleted()));
					}
					if (InnerType.Is<FLinearColor>())
					{
						return FVoxelRuntimePinValue::Make(FVoxelBufferUtilities::Lerp(
							ValueA.Get_CheckCompleted<FVoxelLinearColorBuffer>(),
							ValueB.Get_CheckCompleted<FVoxelLinearColorBuffer>(),
							Alpha.Get_CheckCompleted()));
					}

					const FVoxelBoolBuffer Condition = FVoxelBufferUtilities::Less(0.5f, Alpha.Get_CheckCompleted());

					return FVoxelRuntimePinValue::Make(FVoxelBufferUtilities::Select(
						InnerType,
						Condition,
						TVoxelArray<const FVoxelBuffer*>{ &ValueA.Get_CheckCompleted(), &ValueB.Get_CheckCompleted() }),
						InnerType.GetBufferType());
				});
		});
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TVoxelFutureValue<FVoxelFloatBuffer> FVoxelSurface::GetDistance(const FVoxelQuery& Query) const
{
	VOXEL_FUNCTION_COUNTER();

	if (!ComputeDistance ||
		!ensure(bIsValid))
	{
		return 1.e6f;
	}

	// Task to flatten callstack
	return
		MakeVoxelTask()
		.Execute<FVoxelFloatBuffer>([ComputeDistance = ComputeDistance, Query]() -> TVoxelFutureValue<FVoxelFloatBuffer>
		{
			const TVoxelFutureValue<FVoxelFloatBuffer> Distance = (*ComputeDistance)(Query);
			if (!Distance.IsValid())
			{
				return 1.e6f;
			}
			return Distance;
		});
}

TVoxelFutureValue<FVoxelSurfaceMaterial> FVoxelSurface::GetMaterial(const FVoxelQuery& Query) const
{
	VOXEL_FUNCTION_COUNTER();

	if (!ComputeMaterial ||
		!ensure(bIsValid))
	{
		return FVoxelSurfaceMaterial();
	}

	// Task to flatten callstack
	return
		MakeVoxelTask()
		.Execute<FVoxelSurfaceMaterial>([ComputeMaterial = ComputeMaterial, Query]() -> TVoxelFutureValue<FVoxelSurfaceMaterial>
		{
			const TVoxelFutureValue<FVoxelSurfaceMaterial> Material = (*ComputeMaterial)(Query);
			if (!Material.IsValid())
			{
				return FVoxelSurfaceMaterial();
			}
			return Material;
		});
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelSurface::SetDistance(
	const FVoxelQuery& Query,
	const FVoxelGraphNodeRef& NodeRef,
	TVoxelComputeValue<FVoxelFloatBuffer>&& Lambda)
{
	ComputeDistance = MakeVoxelShared<TVoxelComputeValue<FVoxelFloatBuffer>>([NodeRef, Lambda = MoveTemp(Lambda), Context = Query.GetSharedContext()](const FVoxelQuery& InQuery)
	{
		const FVoxelQuery NewQuery = InQuery.MakeNewQuery(Context).EnterScope(NodeRef);
		const FVoxelQueryScope Scope(NewQuery);
		return Lambda(NewQuery);
	});
}

void FVoxelSurface::SetMaterial(
	const FVoxelQuery& Query,
	const FVoxelGraphNodeRef& NodeRef,
	TVoxelComputeValue<FVoxelSurfaceMaterial>&& Lambda)
{
	ComputeMaterial = MakeVoxelShared<TVoxelComputeValue<FVoxelSurfaceMaterial>>([NodeRef, Lambda = MoveTemp(Lambda), Context = Query.GetSharedContext()](const FVoxelQuery& InQuery)
	{
		const FVoxelQuery NewQuery = InQuery.MakeNewQuery(Context).EnterScope(NodeRef);
		const FVoxelQueryScope Scope(NewQuery);
		return Lambda(NewQuery);
	});
}

void FVoxelSurface::SetAttribute(
	const FName Name,
	const FVoxelPinType& InnerType,
	const TWeakPtr<FVoxelDetailTexturePool>& DetailTexturePool,
	const FVoxelQuery& Query,
	const FVoxelGraphNodeRef& NodeRef,
	TVoxelComputeValue<FVoxelBuffer>&& Lambda)
{
	checkVoxelSlow(!InnerType.IsBuffer());

	FAttribute& Attribute = NameToAttribute.Add(Name);
	Attribute.InnerType = InnerType;
	Attribute.DetailTexturePool = DetailTexturePool;
	Attribute.Compute = MakeVoxelShared<TVoxelComputeValue<FVoxelBuffer>>([NodeRef, Lambda = MoveTemp(Lambda), Context = Query.GetSharedContext()](const FVoxelQuery& InQuery)
	{
		const FVoxelQuery NewQuery = InQuery.MakeNewQuery(Context).EnterScope(NodeRef);
		const FVoxelQueryScope Scope(NewQuery);
		return Lambda(NewQuery);
	});
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelSurface::SetLocalDistance(
	const FVoxelQuery& InQuery,
	const FVoxelGraphNodeRef& NodeRef,
	TVoxelComputeValue<FVoxelFloatBuffer>&& Lambda)
{
	SetDistance(InQuery, NodeRef, [NodeRef, Lambda = MoveTemp(Lambda)](const FVoxelQuery& Query) -> TVoxelFutureValue<FVoxelFloatBuffer>
	{
		if (!Query.GetParameters().Find<FVoxelPositionQueryParameter>())
		{
			FVoxelNodeHelpers::RaiseQueryError<FVoxelPositionQueryParameter>(NodeRef);
			return {};
		}

		const FVoxelTransformRef QueryToLocal = Query.GetQueryToLocal();
		const FVoxelQuery NewQuery = FVoxelPositionQueryParameter::TransformQuery(Query, QueryToLocal.Get(Query));
		const TVoxelFutureValue<FVoxelFloatBuffer> Distance = Lambda(NewQuery);

		return
			MakeVoxelTask(STATIC_FNAME("TransformDistance"))
			.Dependency(Distance)
			.Execute<FVoxelFloatBuffer>([=]
			{
				return FVoxelBufferUtilities::TransformDistance(Distance.Get_CheckCompleted(), QueryToLocal.Inverse().Get(Query));
			});
	});
}

void FVoxelSurface::SetLocalMaterial(
	const FVoxelQuery& InQuery,
	const FVoxelGraphNodeRef& NodeRef,
	TVoxelComputeValue<FVoxelSurfaceMaterial>&& Lambda)
{
	SetMaterial(InQuery, NodeRef, [NodeRef, Lambda = MoveTemp(Lambda)](const FVoxelQuery& Query) -> TVoxelFutureValue<FVoxelSurfaceMaterial>
	{
		if (!Query.GetParameters().Find<FVoxelPositionQueryParameter>())
		{
			FVoxelNodeHelpers::RaiseQueryError<FVoxelPositionQueryParameter>(NodeRef);
			return {};
		}

		const FVoxelTransformRef QueryToLocal = Query.GetQueryToLocal();
		const FVoxelQuery NewQuery = FVoxelPositionQueryParameter::TransformQuery(Query, QueryToLocal.Get(Query));

		return Lambda(NewQuery);
	});
}

void FVoxelSurface::SetLocalAttribute(
	const FName Name,
	const FVoxelPinType& InnerType,
	const TWeakPtr<FVoxelDetailTexturePool>& DetailTexturePool,
	const FVoxelQuery& InQuery,
	const FVoxelGraphNodeRef& NodeRef,
	TVoxelComputeValue<FVoxelBuffer>&& Lambda)
{
	SetAttribute(
		Name,
		InnerType,
		DetailTexturePool,
		InQuery,
		NodeRef,
		[NodeRef, Lambda = MoveTemp(Lambda)](const FVoxelQuery& Query) -> TVoxelFutureValue<FVoxelBuffer>
	{
		if (!Query.GetParameters().Find<FVoxelPositionQueryParameter>())
		{
			FVoxelNodeHelpers::RaiseQueryError<FVoxelPositionQueryParameter>(NodeRef);
			return {};
		}

		const FVoxelTransformRef QueryToLocal = Query.GetQueryToLocal();
		const FVoxelQuery NewQuery = FVoxelPositionQueryParameter::TransformQuery(Query, QueryToLocal.Get(Query));

		return Lambda(NewQuery);
	});
}