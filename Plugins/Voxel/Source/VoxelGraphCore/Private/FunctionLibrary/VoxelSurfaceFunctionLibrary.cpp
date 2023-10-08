// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "FunctionLibrary/VoxelSurfaceFunctionLibrary.h"
#include "FunctionLibrary/VoxelMathFunctionLibrary.h"
#include "VoxelDetailTexture.h"
#include "VoxelPositionQueryParameter.h"
#include "VoxelSurfaceFunctionLibraryImpl.ispc.generated.h"

FVoxelBounds UVoxelSurfaceFunctionLibrary::GetSurfaceBounds(
	const FVoxelSurface& Surface,
	const float Smoothness) const
{
	return Surface.Bounds.Extend(Smoothness, GetQuery(), GetQuery().GetLocalToWorld());
}

FVoxelBounds UVoxelSurfaceFunctionLibrary::MakeBoundsFromLocalBox(const FVoxelBox& Box) const
{
	return FVoxelBounds(Box, GetQuery().GetLocalToWorld());
}

FVoxelBox UVoxelSurfaceFunctionLibrary::GetBoundsBox(
	const FVoxelBounds& Bounds,
	const EVoxelTransformSpace TransformSpace) const
{
	FVoxelTransformRef LocalToWorld;
	switch (TransformSpace)
	{
	default: ensure(false);
	case EVoxelTransformSpace::Local:
	{
		LocalToWorld = GetQuery().GetLocalToWorld();
	}
	break;
	case EVoxelTransformSpace::World:
	{
		LocalToWorld = FVoxelTransformRef::Identity();
	}
	break;
	case EVoxelTransformSpace::Query:
	{
		LocalToWorld = GetQuery().GetQueryToWorld();
	}
	break;
	}

	return Bounds.GetBox(GetQuery(), LocalToWorld);
}

FVoxelSurfaceMaterial UVoxelSurfaceFunctionLibrary::BlendSurfaceMaterials(
	const FVoxelSurfaceMaterial& A,
	const FVoxelSurfaceMaterial& B,
	const FVoxelFloatBuffer& Alpha) const
{
	const int32 Num = ComputeVoxelBuffersNum_Function(A, B, Alpha);
	const int32 NumLayers = A.Layers.Num() + B.Layers.Num();

	TVoxelArray<FVoxelMaterialDefinitionBufferStorage> Materials;
	TVoxelArray<FVoxelByteBufferStorage> Strengths;
	for (int32 Index = 0; Index < NumLayers; Index++)
	{
		Materials.Emplace_GetRef().Allocate(Num);
		Strengths.Emplace_GetRef().Allocate(Num);
	}

	ForeachVoxelBufferChunk(Num, [&](const FVoxelBufferIterator& Iterator)
	{
		TVoxelArray<ispc::FVoxelSurfaceLayer> LayersA;
		TVoxelArray<ispc::FVoxelSurfaceLayer> LayersB;
		A.GetLayers(Iterator, LayersA);
		B.GetLayers(Iterator, LayersB);

		TVoxelArray<ispc::FVoxelSurfaceWriteLayer> Layers;
		for (int32 Index = 0; Index < NumLayers; Index++)
		{
			Layers.Add(
			{
				ReinterpretCastPtr<uint16>(Materials[Index].GetData(Iterator)),
				Strengths[Index].GetData(Iterator)
			});
		}

		ispc::VoxelSurfaceFunctionLibrary_MergeSurfaces(
			Alpha.GetData(Iterator),
			Alpha.IsConstant(),
			LayersA.GetData(),
			LayersB.GetData(),
			LayersA.Num(),
			LayersB.Num(),
			Iterator.Num(),
			Layers.GetData()
		);
	});

	FVoxelSurfaceMaterial Result;
	for (int32 Index = 0; Index < NumLayers; Index++)
	{
		FVoxelMaterialDefinitionBufferStorage& Material = Materials[Index];
		FVoxelByteBufferStorage& Strength = Strengths[Index];

		Material.TryReduceIntoConstant();

		if (Strength.TryReduceIntoConstant())
		{
			if (Strength.GetConstant() == 0)
			{
				continue;
			}
		}

		Result.Layers.Add(
		{
			FVoxelMaterialDefinitionBuffer::Make(Material),
			FVoxelByteBuffer::Make(Strength)
		});
	}
	return Result;

}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelSurface UVoxelSurfaceFunctionLibrary::BindFloatAttributeDetailTexture(
	const FVoxelSurface& Surface,
	const FName Name,
	const FVoxelFloatDetailTextureRef& DetailTexture,
	const bool bLogError) const
{
	FVoxelSurface Result = Surface;

	FVoxelSurface::FAttribute* Attribute = Result.NameToAttribute.Find(Name);
	if (!Attribute)
	{
		if (bLogError)
		{
			TVoxelArray<FName> Attributes;
			Surface.NameToAttribute.GenerateKeyArray(Attributes);

			VOXEL_MESSAGE(Error, "{0}: No attribute named {1}. Valid attributes: {2}",
				this,
				Name,
				Attributes);
		}

		return Surface;
	}
	if (!Attribute->InnerType.Is<float>())
	{
		VOXEL_MESSAGE(Error, "{0}: Attribute {1} has type {2}, not float",
			this,
			Name,
			Attribute->InnerType.ToString());

		return Surface;
	}

	Attribute->DetailTexturePool = DetailTexture.WeakPool;
	return Result;
}

FVoxelSurface UVoxelSurfaceFunctionLibrary::BindColorAttributeDetailTexture(
	const FVoxelSurface& Surface,
	const FName Name,
	const FVoxelColorDetailTextureRef& DetailTexture,
	const bool bLogError) const
{
	FVoxelSurface Result = Surface;

	FVoxelSurface::FAttribute* Attribute = Result.NameToAttribute.Find(Name);
	if (!Attribute)
	{
		if (bLogError)
		{
			TVoxelArray<FName> Attributes;
			Surface.NameToAttribute.GenerateKeyArray(Attributes);

			VOXEL_MESSAGE(Error, "{0}: No attribute named {1}. Valid attributes: {2}",
				this,
				Name,
				Attributes);
		}

		return Surface;
	}
	if (!Attribute->InnerType.Is<FLinearColor>())
	{
		VOXEL_MESSAGE(Error, "{0}: Attribute {1} has type {2}, not color",
			this,
			Name,
			Attribute->InnerType.ToString());

		return Surface;
	}

	Attribute->DetailTexturePool = DetailTexture.WeakPool;
	return Result;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelSurface UVoxelSurfaceFunctionLibrary::Invert(const FVoxelSurface& Surface) const
{
	if (!Surface.bIsValid)
	{
		return {};
	}

	FVoxelSurface Result = FVoxelSurface::MakeInfinite(GetNodeRef());

	Result.SetDistance(GetQuery(), GetNodeRef(), [Surface](const FVoxelQuery& Query) -> TVoxelFutureValue<FVoxelFloatBuffer>
	{
		const TVoxelFutureValue<FVoxelFloatBuffer> FutureDistance = Surface.GetDistance(Query);

		return
			MakeVoxelTask(STATIC_FNAME("Invert"))
			.Dependency(FutureDistance)
			.Execute<FVoxelFloatBuffer>([=]
			{
				const FVoxelFloatBuffer Distance = FutureDistance.Get_CheckCompleted();

				FVoxelFloatBufferStorage NewDistance;
				NewDistance.Allocate(Distance.Num());

				ForeachVoxelBufferChunk(Distance.Num(), [&](const FVoxelBufferIterator& Iterator)
				{
					ispc::VoxelSurfaceFunctionLibrary_Invert(
						Distance.GetData(Iterator),
						NewDistance.GetData(Iterator),
						Iterator.Num());
				});

				return FVoxelFloatBuffer::Make(NewDistance);
			});
	});

	Result.CopyMaterialAttributes(Surface);
	return Result;
}

FVoxelSurface UVoxelSurfaceFunctionLibrary::SmoothUnion(
	const FVoxelSurface& A,
	const FVoxelSurface& B,
	const float Smoothness) const
{
	if (!A.bIsValid)
	{
		return B;
	}
	if (!B.bIsValid)
	{
		return A;
	}

	const float QuerySmoothness = Smoothness * GetQuery().GetLocalToQuery().Get(GetQuery()).GetScaleVector().GetAbsMax();

	FVoxelSurface Result = FVoxelSurface::MakeInfinite(GetNodeRef());

	Result.SetDistance(GetQuery(), GetNodeRef(), [A, B, QuerySmoothness, NodeRef = GetNodeRef()](const FVoxelQuery& Query)
	{
		const TVoxelFutureValue<FVoxelFloatBuffer> DistanceA = A.GetDistance(Query);
		const TVoxelFutureValue<FVoxelFloatBuffer> DistanceB = B.GetDistance(Query);

		return
			MakeVoxelTask(STATIC_FNAME("SmoothMin"))
			.Dependencies(DistanceA, DistanceB)
			.Execute<FVoxelFloatBuffer>([=]
			{
				FVoxelFloatBuffer Distance;
				FVoxelFloatBuffer Alpha;
				MakeVoxelFunctionCaller<UVoxelMathFunctionLibrary>(NodeRef, Query)->SmoothMin(
					Distance,
					Alpha,
					DistanceA.Get_CheckCompleted(),
					DistanceB.Get_CheckCompleted(),
					QuerySmoothness);
				return Distance;
			});
	});

	Result.LerpMaterialAttributes(
		GetQuery(),
		GetNodeRef(),
		A,
		B,
		MakeVoxelShared<TVoxelComputeValue<FVoxelFloatBuffer>>([NodeRef = GetNodeRef(), A, B, QuerySmoothness](const FVoxelQuery& Query)
		{
			const TVoxelFutureValue<FVoxelFloatBuffer> DistanceA = A.GetDistance(Query);
			const TVoxelFutureValue<FVoxelFloatBuffer> DistanceB = B.GetDistance(Query);

			return
				MakeVoxelTask(STATIC_FNAME("SmoothMin"))
				.Dependencies(DistanceA, DistanceB)
				.Execute<FVoxelFloatBuffer>([=]
				{
					FVoxelFloatBuffer Distance;
					FVoxelFloatBuffer Alpha;
					MakeVoxelFunctionCaller<UVoxelMathFunctionLibrary>(NodeRef, Query)->SmoothMin(
						Distance,
						Alpha,
						DistanceA.Get_CheckCompleted(),
						DistanceB.Get_CheckCompleted(),
						QuerySmoothness);

					return Alpha;
				});
		}));

	return Result;
}

FVoxelSurface UVoxelSurfaceFunctionLibrary::SmoothIntersection(
	const FVoxelSurface& A,
	const FVoxelSurface& B,
	const float Smoothness) const
{
	if (!A.bIsValid ||
		!B.bIsValid)
	{
		return {};
	}

	const float QuerySmoothness = Smoothness * GetQuery().GetLocalToQuery().Get(GetQuery()).GetScaleVector().GetAbsMax();

	FVoxelSurface Result = FVoxelSurface::MakeInfinite(GetNodeRef());

	Result.SetDistance(GetQuery(), GetNodeRef(), [A, B, QuerySmoothness, NodeRef = GetNodeRef()](const FVoxelQuery& Query)
	{
		const TVoxelFutureValue<FVoxelFloatBuffer> DistanceA = A.GetDistance(Query);
		const TVoxelFutureValue<FVoxelFloatBuffer> DistanceB = B.GetDistance(Query);

		return
			MakeVoxelTask(STATIC_FNAME("SmoothMax"))
			.Dependencies(DistanceA, DistanceB)
			.Execute<FVoxelFloatBuffer>([=]
			{
				FVoxelFloatBuffer Distance;
				FVoxelFloatBuffer Alpha;
				MakeVoxelFunctionCaller<UVoxelMathFunctionLibrary>(NodeRef, Query)->SmoothMax(
					Distance,
					Alpha,
					DistanceA.Get_CheckCompleted(),
					DistanceB.Get_CheckCompleted(),
					QuerySmoothness);
				return Distance;
			});
	});

	Result.LerpMaterialAttributes(
		GetQuery(),
		GetNodeRef(),
		A,
		B,
		MakeVoxelShared<TVoxelComputeValue<FVoxelFloatBuffer>>([NodeRef = GetNodeRef(), A, B, QuerySmoothness](const FVoxelQuery& Query)
		{
			const TVoxelFutureValue<FVoxelFloatBuffer> DistanceA = A.GetDistance(Query);
			const TVoxelFutureValue<FVoxelFloatBuffer> DistanceB = B.GetDistance(Query);

			return
				MakeVoxelTask(STATIC_FNAME("SmoothMax"))
				.Dependencies(DistanceA, DistanceB)
				.Execute<FVoxelFloatBuffer>([=]
				{
					FVoxelFloatBuffer Distance;
					FVoxelFloatBuffer Alpha;
					MakeVoxelFunctionCaller<UVoxelMathFunctionLibrary>(NodeRef, Query)->SmoothMax(
						Distance,
						Alpha,
						DistanceA.Get_CheckCompleted(),
						DistanceB.Get_CheckCompleted(),
						QuerySmoothness);

					return Alpha;
				});
		}));

	return Result;
}

FVoxelSurface UVoxelSurfaceFunctionLibrary::SmoothSubtraction(
	const FVoxelSurface& Surface,
	const FVoxelSurface& SurfaceToSubtract,
	const float Smoothness) const
{
	if (!SurfaceToSubtract.bIsValid)
	{
		return Surface;
	}

	return SmoothIntersection(Surface, Invert(SurfaceToSubtract), Smoothness);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelSurface UVoxelSurfaceFunctionLibrary::MakeSphereSurface(
	const FVector& Center,
	const float InRadius) const
{
	const float Radius = FMath::Max(InRadius, 0.f);

	FVoxelSurface Surface = FVoxelSurface::MakeWithLocalBounds(
		GetNodeRef(),
		GetQuery(),
		FVoxelBox(Center - Radius, Center + Radius));

	Surface.SetLocalDistance(GetQuery(), GetNodeRef(), [=, NodeRef = GetNodeRef()](const FVoxelQuery& Query)
	{
		return MakeVoxelFunctionCaller<UVoxelSurfaceFunctionLibrary>(NodeRef, Query)->MakeSphereSurface_Distance(Center, Radius);
	});

	return Surface;
}

FVoxelSurface UVoxelSurfaceFunctionLibrary::MakeBoxSurface(
	const FVector& Center,
	const FVector& InExtent,
	const float InSmoothness) const
{
	const FVector Extent = FVector::Max(InExtent, FVector::ZeroVector);
	const float Smoothness = FMath::Max(InSmoothness, 0.f);

	FVoxelSurface Surface = FVoxelSurface::MakeWithLocalBounds(
		GetNodeRef(),
		GetQuery(),
		FVoxelBox(Center - Extent, Center + Extent).Extend(Smoothness));

	Surface.SetLocalDistance(GetQuery(), GetNodeRef(), [=, NodeRef = GetNodeRef()](const FVoxelQuery& Query)
	{
		return MakeVoxelFunctionCaller<UVoxelSurfaceFunctionLibrary>(NodeRef, Query)->MakeBoxSurface_Distance(Center, Extent, Smoothness);
	});

	return Surface;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelFloatBuffer UVoxelSurfaceFunctionLibrary::MakeSphereSurface_Distance(
	const FVector& Center,
	const float Radius) const
{
	VOXEL_FUNCTION_COUNTER();
	FindVoxelQueryParameter_Function(FVoxelPositionQueryParameter, PositionQueryParameter);

	const FVoxelVectorBuffer Positions = PositionQueryParameter->GetPositions();

	FVoxelFloatBufferStorage Distance;
	Distance.Allocate(Positions.Num());

	ForeachVoxelBufferChunk(Positions.Num(), [&](const FVoxelBufferIterator& Iterator)
	{
		ispc::VoxelSurfaceFunctionLibrary_MakeSphereSurface(
			Positions.X.GetData(Iterator),
			Positions.X.IsConstant(),
			Positions.Y.GetData(Iterator),
			Positions.Y.IsConstant(),
			Positions.Z.GetData(Iterator),
			Positions.Z.IsConstant(),
			GetISPCValue(FVector3f(Center)),
			Radius,
			Distance.GetData(Iterator),
			Iterator.Num()
		);
	});

	return FVoxelFloatBuffer::Make(Distance);
}

FVoxelFloatBuffer UVoxelSurfaceFunctionLibrary::MakeBoxSurface_Distance(
	const FVector& Center,
	const FVector& Extent,
	const float Smoothness) const
{
	VOXEL_FUNCTION_COUNTER();
	FindVoxelQueryParameter_Function(FVoxelPositionQueryParameter, PositionQueryParameter);

	const FVoxelVectorBuffer Positions = PositionQueryParameter->GetPositions();

	FVoxelFloatBufferStorage Distance;
	Distance.Allocate(Positions.Num());

	ForeachVoxelBufferChunk(Positions.Num(), [&](const FVoxelBufferIterator& Iterator)
	{
		ispc::VoxelSurfaceFunctionLibrary_MakeBoxSurface(
			Positions.X.GetData(Iterator),
			Positions.X.IsConstant(),
			Positions.Y.GetData(Iterator),
			Positions.Y.IsConstant(),
			Positions.Z.GetData(Iterator),
			Positions.Z.IsConstant(),
			GetISPCValue(FVector3f(Center)),
			GetISPCValue(FVector3f(Extent)),
			Smoothness,
			Distance.GetData(Iterator),
			Iterator.Num()
		);
	});

	return FVoxelFloatBuffer::Make(Distance);
}