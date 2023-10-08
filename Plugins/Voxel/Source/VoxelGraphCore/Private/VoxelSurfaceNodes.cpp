// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelSurfaceNodes.h"
#include "VoxelBufferUtilities.h"
#include "VoxelPositionQueryParameter.h"
#include "VoxelSurfaceNodesImpl.ispc.generated.h"

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_GetSurfaceDistance, Distance)
{
	const TValue<FVoxelSurface> Surface = Get(SurfacePin, Query);

	return VOXEL_ON_COMPLETE(Surface)
	{
		return Surface->GetDistance(Query);
	};
}

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_GetSurfaceMaterial, Material)
{
	const TValue<FVoxelSurface> Surface = Get(SurfacePin, Query);

	return VOXEL_ON_COMPLETE(Surface)
	{
		return Surface->GetMaterial(Query);
	};
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_GetSurfaceAttribute, Value)
{
	const TValue<FVoxelSurface> Surface = Get(SurfacePin, Query);
	const TValue<FName> Name = Get(NamePin, Query);

	return VOXEL_ON_COMPLETE(Surface, Name)
	{
		const FVoxelSurface::FAttribute* Attribute = Surface->NameToAttribute.Find(Name);
		if (!Attribute)
		{
			TVoxelArray<FName> Attributes;
			Surface->NameToAttribute.GenerateKeyArray(Attributes);

			VOXEL_MESSAGE(Error, "{0}: No attribute named {1}. Valid attributes: {2}",
				this,
				Name,
				Attributes);

			return {};
		}
		if (Attribute->InnerType != ReturnPinType.GetInnerType())
		{
			VOXEL_MESSAGE(Error, "{0}: Attribute {1} has type {2}, not {3}",
				this,
				Name,
				Attribute->InnerType.ToString(),
				ReturnPinType.GetInnerType().ToString());

			return {};
		}

		return (*Attribute->Compute)(Query);
	};
}

#if WITH_EDITOR
FVoxelPinTypeSet FVoxelNode_GetSurfaceAttribute::GetPromotionTypes(const FVoxelPin& Pin) const
{
	return FVoxelPinTypeSet::AllBuffers();
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_MakeHeightSurface, Surface)
{
	FVoxelSurface Surface = FVoxelSurface::MakeInfinite(GetNodeRef());

	Surface.SetLocalDistance(Query, GetNodeRef(), [NodeRef = GetNodeRef(), GetHeight = GetCompute(HeightPin, Query.GetSharedContext())](const FVoxelQuery& Query)
	{
		const TValue<FVoxelFloatBuffer> FutureHeight = INLINE_LAMBDA -> TValue<FVoxelFloatBuffer>
		{
			const FVoxelPositionQueryParameter* PositionQueryParameter = Query.GetParameters().Find<FVoxelPositionQueryParameter>();
			if (!ensure(PositionQueryParameter))
			{
				return FVoxelFloatBuffer();
			}

			if (!PositionQueryParameter->IsGrid())
			{
				return (*GetHeight)(Query);
			}

			const int32 SizeZ = PositionQueryParameter->GetGrid().Size.Z;
			if (SizeZ <= 1)
			{
				return (*GetHeight)(Query);
			}

			FIntVector Size = PositionQueryParameter->GetGrid().Size;
			Size.Z = 1;

			const TSharedRef<FVoxelQueryParameters> Parameters = Query.CloneParameters();
			Parameters->Add<FVoxelPositionQueryParameter>().InitializeGrid(
				PositionQueryParameter->GetGrid().Start,
				PositionQueryParameter->GetGrid().Step,
				Size);
			const TValue<FVoxelFloatBuffer> LocalHeights = (*GetHeight)(Query.MakeNewQuery(Parameters));

			return
				MakeVoxelTask(STATIC_FNAME("UnpackHeights"))
				.Dependency(LocalHeights)
				.Execute<FVoxelFloatBuffer>([=]
				{
					FVoxelFloatBufferStorage Result;
					for (int32 Z = 0; Z < SizeZ; Z++)
					{
						Result.Append(
							LocalHeights.Get_CheckCompleted().GetStorage(),
							Size.X * Size.Y);
					}
					return FVoxelFloatBuffer::Make(Result);
				});
		};

		return
			MakeVoxelTask(STATIC_FNAME("MakeHeightSurface"))
			.Dependency(FutureHeight)
			.Execute<FVoxelFloatBuffer>([=]() -> TValue<FVoxelFloatBuffer>
			{
				const FVoxelFloatBuffer Height = FutureHeight.Get_CheckCompleted();

				const FVoxelPositionQueryParameter* PositionQueryParameter = Query.GetParameters().Find<FVoxelPositionQueryParameter>();
				if (!PositionQueryParameter)
				{
					RaiseQueryError<FVoxelPositionQueryParameter>(NodeRef);
					return {};
				}

				const FVoxelFloatBuffer Z = PositionQueryParameter->GetPositions().Z;
				const FVoxelBufferAccessor BufferAccessor(Height, Z);
				if (!BufferAccessor.IsValid())
				{
					RaiseBufferError(NodeRef);
					return {};
				}

				FVoxelFloatBufferStorage Distance;
				Distance.Allocate(BufferAccessor.Num());

				ForeachVoxelBufferChunk(BufferAccessor.Num(), [&](const FVoxelBufferIterator& Iterator)
				{
					ispc::VoxelNode_MakeHeightSurface(
						Height.GetData(Iterator),
						Height.IsConstant(),
						Z.GetData(Iterator),
						Z.IsConstant(),
						Iterator.Num(),
						Distance.GetData(Iterator));
				});

				return FVoxelFloatBuffer::Make(Distance);
			});
	});

	return Surface;
}

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_MakeVolumetricSurface, Surface)
{
	const TValue<FVoxelBounds> Bounds = Get(BoundsPin, Query);

	return VOXEL_ON_COMPLETE(Bounds)
	{
		FVoxelSurface Surface = FVoxelSurface::Make(
			GetNodeRef(),
			Bounds->IsValid() ? *Bounds : FVoxelBounds::Infinite());

		Surface.ComputeDistance = GetCompute(DistancePin, Query.GetSharedContext());
		return Surface;
	};
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_SetSurfaceMaterial, NewSurface)
{
	const TValue<FVoxelSurface> Surface = Get(SurfacePin, Query);

	return VOXEL_ON_COMPLETE(Surface)
	{
		FVoxelSurface NewSurface = *Surface;
		NewSurface.ComputeMaterial = GetCompute(MaterialPin, Query.GetSharedContext());
		return NewSurface;
	};
}

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_MakeSurfaceMaterial, Result)
{
	const TValue<FVoxelMaterialDefinitionBuffer> Material = Get(MaterialPin, Query);

	return VOXEL_ON_COMPLETE(Material)
	{
		FVoxelSurfaceMaterial Result;
		Result.Layers.Add(FVoxelSurfaceMaterial::FLayer
		{
			Material,
			255
		});
		return Result;
	};
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_SetSurfaceAttribute, NewSurface)
{
	const TValue<FVoxelSurface> Surface = Get(SurfacePin, Query);
	const TValue<FName> Name = Get(NamePin, Query);

	return VOXEL_ON_COMPLETE(Surface, Name)
	{
		const FVoxelPinType InnerType = GetNodeRuntime().GetPinData(ValuePin).Type.GetInnerType();

		FVoxelSurface NewSurface = *Surface;
		FVoxelSurface::FAttribute& Attribute = NewSurface.NameToAttribute.FindOrAdd(Name);

		if (!Attribute.InnerType.IsValid())
		{
			Attribute.InnerType = InnerType;
		}
		else if (Attribute.InnerType != InnerType)
		{
			VOXEL_MESSAGE(Error, "{0}: Surface already has an attribute named {1} with type {2}",
				this,
				Name,
				Attribute.InnerType.ToString());

			return Surface;
		}

		Attribute.Compute = ReinterpretCastSharedPtr<const TVoxelComputeValue<FVoxelBuffer>>(GetCompute(ValuePin, Query.GetSharedContext()));
		return NewSurface;
	};
}

#if WITH_EDITOR
FVoxelPinTypeSet FVoxelNode_SetSurfaceAttribute::GetPromotionTypes(const FVoxelPin& Pin) const
{
	return FVoxelPinTypeSet::AllBuffers();
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_GrowSurface, NewSurface)
{
	const TValue<FVoxelSurface> Surface = Get(SurfacePin, Query);
	const TValue<float> InMaxAmount = Get(MaxAmountPin, Query);

	return VOXEL_ON_COMPLETE(Surface, InMaxAmount)
	{
		if (!Surface->bIsValid)
		{
			return {};
		}

		const float MaxAmount = FMath::Max(InMaxAmount, 0.f);
		const float Scale = Query.GetLocalToQuery().Get(Query).GetScaleVector().GetAbsMax();

		FVoxelSurface Result = FVoxelSurface::Make(
			GetNodeRef(),
			Surface->Bounds.Extend(MaxAmount, Query, Query.GetLocalToWorld()));

		Result.SetDistance(Query, GetNodeRef(), [Surface, MaxAmount, Scale, NodeRef = GetNodeRef(), ComputeAmount = GetCompute(AmountPin, Query.GetSharedContext())](const FVoxelQuery& Query)
		{
			const TValue<FVoxelFloatBuffer> FutureDistance = Surface->GetDistance(Query);
			const TValue<FVoxelFloatBuffer> FutureAmount = (*ComputeAmount)(Query);

			return
				MakeVoxelTask(STATIC_FNAME("Grow"))
				.Dependencies(FutureDistance, FutureAmount)
				.Execute<FVoxelFloatBuffer>([=]() -> TValue<FVoxelFloatBuffer>
				{
					const FVoxelFloatBuffer Distance = FutureDistance.Get_CheckCompleted();
					const FVoxelFloatBuffer Amount = FutureAmount.Get_CheckCompleted();

					const FVoxelBufferAccessor BufferAccessor(Distance, Amount);
					if (!BufferAccessor.IsValid())
					{
						RaiseBufferError(NodeRef);
						return {};
					}

					FVoxelFloatBufferStorage NewDistance;
					NewDistance.Allocate(BufferAccessor.Num());

					ForeachVoxelBufferChunk(BufferAccessor.Num(), [&](const FVoxelBufferIterator& Iterator)
					{
						ispc::VoxelNode_GrowSurface(
							Distance.GetData(Iterator),
							Distance.IsConstant(),
							Amount.GetData(Iterator),
							Amount.IsConstant(),
							MaxAmount,
							Scale,
							NewDistance.GetData(Iterator),
							Iterator.Num());
					});

					return FVoxelFloatBuffer::Make(NewDistance);
				});
		});

		Result.CopyMaterialAttributes(*Surface);
		return Result;
	};
}

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_GrowSurfaceUniform, NewSurface)
{
	return VOXEL_CALL_NODE(FVoxelNode_GrowSurface, NewSurfacePin, Query)
	{
		VOXEL_CALL_NODE_BIND(SurfacePin)
		{
			return Get(SurfacePin, Query);
		};
		VOXEL_CALL_NODE_BIND(AmountPin)
		{
			const TValue<float> Amount = Get(AmountPin, Query);
			return VOXEL_ON_COMPLETE(Amount)
			{
				return Amount;
			};
		};
		VOXEL_CALL_NODE_BIND(MaxAmountPin)
		{
			const TValue<float> Amount = Get(AmountPin, Query);
			return VOXEL_ON_COMPLETE(Amount)
			{
				return Amount;
			};
		};
	};
}