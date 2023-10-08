// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelHeightmapFunctionLibrary.h"
#include "VoxelPositionQueryParameter.h"
#include "VoxelHeightmapFunctionLibraryImpl.ispc.generated.h"

FVoxelFloatBuffer UVoxelHeightmapFunctionLibrary::SampleHeightmap(
	const FVoxelHeightmapRef& Heightmap,
	const FVoxelVector2DBuffer& Position) const
{
	if (!Heightmap.Data)
	{
		VOXEL_MESSAGE(Error, "{0}: Heightmap is null", this);
		return 0.f;
	}
	const FVoxelHeightmapConfig& Config = Heightmap.Config;

	const float ScaleZ = Config.ScaleZ * Config.InternalScaleZ / MAX_uint16;
	const float OffsetZ = Config.ScaleZ * Config.InternalOffsetZ;

	FVoxelFloatBufferStorage Result;
	Result.Allocate(Position.Num());

	ForeachVoxelBufferChunk(Position.Num(), [&](const FVoxelBufferIterator& Iterator)
	{
		ispc::VoxelHeightmapFunctionLibrary_SampleHeightmap(
			Position.X.GetData(Iterator),
			Position.X.IsConstant(),
			Position.Y.GetData(Iterator),
			Position.Y.IsConstant(),
			Config.ScaleXY,
			ScaleZ,
			OffsetZ,
			Heightmap.Data->GetSizeX(),
			Heightmap.Data->GetSizeY(),
			Heightmap.Data->GetHeights().GetData(),
			Iterator.Num(),
			Result.GetData(Iterator));
	});

	return FVoxelFloatBuffer::Make(Result);
}

FVoxelSurface UVoxelHeightmapFunctionLibrary::MakeCubemapPlanetSurface(
	const FVoxelHeightmapRef& PosX,
	const FVoxelHeightmapRef& NegX,
	const FVoxelHeightmapRef& PosY,
	const FVoxelHeightmapRef& NegY,
	const FVoxelHeightmapRef& PosZ,
	const FVoxelHeightmapRef& NegZ,
	const FVector& PlanetCenter,
	const float PlanetRadius,
	const float MaxHeight) const
{
	FVoxelSurface Surface = FVoxelSurface::MakeWithLocalBounds(
		GetNodeRef(),
		GetQuery(),
		FVoxelBox().Extend(PlanetRadius + MaxHeight).ShiftBy(PlanetCenter));

	Surface.SetLocalDistance(GetQuery(), GetNodeRef(), [=, NodeRef = GetNodeRef()](const FVoxelQuery& Query)
	{
		return MakeVoxelFunctionCaller<UVoxelHeightmapFunctionLibrary>(NodeRef, Query)->MakeCubemapPlanetSurface_Distance(
			PosX,
			NegX,
			PosY,
			NegY,
			PosZ,
			NegZ,
			PlanetCenter,
			PlanetRadius,
			MaxHeight);
	});

	return Surface;
}

FVoxelFloatBuffer UVoxelHeightmapFunctionLibrary::MakeCubemapPlanetSurface_Distance(
	const FVoxelHeightmapRef& PosX,
	const FVoxelHeightmapRef& NegX,
	const FVoxelHeightmapRef& PosY,
	const FVoxelHeightmapRef& NegY,
	const FVoxelHeightmapRef& PosZ,
	const FVoxelHeightmapRef& NegZ,
	const FVector& PlanetCenter,
	const float PlanetRadius,
	const float MaxHeight) const
{
	VOXEL_FUNCTION_COUNTER();

	if (!PosX.Data)
	{
		VOXEL_MESSAGE(Error, "{0}: PosX is null", this);
		return {};
	}

	const FIntPoint Size = PosX.Data->GetSize();

#define CHECK(Name) \
	if (!Name.Data) \
	{ \
		VOXEL_MESSAGE(Error, "{0}: " #Name " is null", this); \
		return {}; \
	} \
	if (Name.Data->GetSize() != Size) \
	{ \
		VOXEL_MESSAGE(Error, "{0}: {1}.Size is different from {2}.Size: {3} != {4}", \
			this, \
			Name.Asset, \
			PosX.Asset, \
			Name.Data->GetSize().ToString(), \
			Size.ToString()); \
		return {}; \
	}

	CHECK(PosY);
	CHECK(PosZ);
	CHECK(NegX);
	CHECK(NegY);
	CHECK(NegZ);

#undef CHECK

	FindVoxelQueryParameter_Function(FVoxelPositionQueryParameter, PositionQueryParameter);

	const FVoxelVectorBuffer Positions = PositionQueryParameter->GetPositions();

	FVoxelFloatBufferStorage Distance;
	Distance.Allocate(Positions.Num());

	const TVoxelStaticArray<const uint16*, 6> Heightmaps
	{
		PosX.Data->GetHeights().GetData(),
		NegX.Data->GetHeights().GetData(),
		PosY.Data->GetHeights().GetData(),
		NegY.Data->GetHeights().GetData(),
		PosZ.Data->GetHeights().GetData(),
		NegZ.Data->GetHeights().GetData(),
	};

	ForeachVoxelBufferChunk(Positions.Num(), [&](const FVoxelBufferIterator& Iterator)
	{
		ispc::VoxelHeightmapFunctionLibrary_GetDistanceToCubemapPlanet(
			Positions.X.GetData(Iterator),
			Positions.X.IsConstant(),
			Positions.Y.GetData(Iterator),
			Positions.Y.IsConstant(),
			Positions.Z.GetData(Iterator),
			Positions.Z.IsConstant(),
			GetISPCValue(PlanetCenter),
			PlanetRadius,
			MaxHeight,
			Size.X,
			Size.Y,
			Heightmaps.GetData(),
			Iterator.Num(),
			Distance.GetData(Iterator));
	});

	return FVoxelFloatBuffer::Make(Distance);
}

FVoxelBox UVoxelHeightmapFunctionLibrary::GetHeightmapBounds(const FVoxelHeightmapRef& Heightmap) const
{
	if (!Heightmap.Data)
	{
		VOXEL_MESSAGE(Error, "{0}: Heightmap is null", this);
		return FVoxelBox::Infinite;
	}
	const FVoxelHeightmapConfig& Config = Heightmap.Config;

	const float ScaleZ = Config.ScaleZ * Config.InternalScaleZ / MAX_uint16;
	const float OffsetZ = Config.ScaleZ * Config.InternalOffsetZ;

	const FVector2D Size = FVector2D(Heightmap.Data->GetSizeX(), Heightmap.Data->GetSizeY()) / 2.f * Config.ScaleXY;
	const float MinHeight = Heightmap.Data->GetMinHeight() * ScaleZ + OffsetZ;
	const float MaxHeight = Heightmap.Data->GetMaxHeight() * ScaleZ + OffsetZ;

	FVoxelBox Bounds;
	Bounds.Min.X = -Size.X;
	Bounds.Min.Y = -Size.Y;
	Bounds.Min.Z = MinHeight;
	Bounds.Max.X = Size.X;
	Bounds.Max.Y = Size.Y;
	Bounds.Max.Z = MaxHeight;
	return Bounds;
}