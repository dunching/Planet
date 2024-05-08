// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "FunctionLibrary/VoxelBasicFunctionLibrary.h"
#include "Point/VoxelChunkedPointSet.h"
#include "VoxelGraphMigration.h"

VOXEL_RUN_ON_STARTUP_GAME(VoxelBasicFunctionLibraryMigrations)
{
	REGISTER_VOXEL_FUNCTION_MIGRATION("Get Brush Transform", UVoxelBasicFunctionLibrary, GetTransform);
	REGISTER_VOXEL_FUNCTION_MIGRATION("Get Spawnable Bounds", UVoxelBasicFunctionLibrary, GetPointChunkBounds);
}

FVoxelSeedBuffer UVoxelBasicFunctionLibrary::HashPosition(
	const FVoxelVectorBuffer& Position,
	const FVoxelSeed& Seed,
	const int32 RoundingDecimals) const
{
	const uint64 SeedHash = FVoxelUtilities::MurmurHash64(Seed);
	const int32 RoundValue = FVoxelUtilities::IntPow(10, FMath::Clamp(RoundingDecimals, 0, 8));

	FVoxelSeedBufferStorage Seeds;
	Seeds.Allocate(Position.Num());

	for (int32 Index = 0; Index < Position.Num(); Index++)
	{
		const FVector3f RoundedPosition = FVoxelUtilities::RoundToFloat(Position[Index] * RoundValue) / RoundValue;
		Seeds[Index] = FVoxelUtilities::MurmurHash64(SeedHash ^ FVoxelUtilities::MurmurHash(RoundedPosition));
	}

	return FVoxelSeedBuffer::Make(Seeds);
}

FVoxelVectorBuffer UVoxelBasicFunctionLibrary::RandUnitVector(const FVoxelSeedBuffer& Seed) const
{
	FVoxelFloatBufferStorage ResultX; ResultX.Allocate(Seed.Num());
	FVoxelFloatBufferStorage ResultY; ResultY.Allocate(Seed.Num());
	FVoxelFloatBufferStorage ResultZ; ResultZ.Allocate(Seed.Num());

	for (int32 Index = 0; Index < Seed.Num(); Index++)
	{
		const FRandomStream RandomStream(Seed[Index]);
		const FVector UnitVector = RandomStream.GetUnitVector();

		ResultX[Index] = UnitVector.X;
		ResultY[Index] = UnitVector.Y;
		ResultZ[Index] = UnitVector.Z;
	}

	return FVoxelVectorBuffer::Make(ResultX, ResultY, ResultZ);
}

int32 UVoxelBasicFunctionLibrary::GetLOD() const
{
	FindVoxelQueryParameter_Function(FVoxelLODQueryParameter, LODQueryParameter);
	return LODQueryParameter->LOD;
}

FVoxelBox UVoxelBasicFunctionLibrary::GetPointChunkBounds() const
{
	FindVoxelQueryParameter_Function(FVoxelPointChunkRefQueryParameter, PointChunkRefQueryParameter);
	return PointChunkRefQueryParameter->ChunkRef.GetBounds();
}

ECollisionEnabled::Type UVoxelBasicFunctionLibrary::GetCollisionEnabled(const FBodyInstance& BodyInstance) const
{
	return BodyInstance.GetCollisionEnabled(false);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool UVoxelBasicFunctionLibrary::IsPreviewScene() const
{
	return GetQuery().GetInfo(EVoxelQueryInfo::Local).IsPreviewScene();
}

bool UVoxelBasicFunctionLibrary::IsGameWorld() const
{
	return GetQuery().GetInfo(EVoxelQueryInfo::Local).IsGameWorld();
}

bool UVoxelBasicFunctionLibrary::IsEditorWorld() const
{
	return !IsGameWorld();
}

bool UVoxelBasicFunctionLibrary::IsHiddenInEditor() const
{
	return GetQuery().GetInfo(EVoxelQueryInfo::Local).IsHiddenInEditor(GetQuery());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool UVoxelBasicFunctionLibrary::IsStandalone() const
{
	return GetQuery().GetInfo(EVoxelQueryInfo::Local).GetNetMode() == NM_Standalone;
}

bool UVoxelBasicFunctionLibrary::IsDedicatedServer() const
{
	return GetQuery().GetInfo(EVoxelQueryInfo::Local).GetNetMode() == NM_DedicatedServer;
}

bool UVoxelBasicFunctionLibrary::IsListenServer() const
{
	return GetQuery().GetInfo(EVoxelQueryInfo::Local).GetNetMode() == NM_ListenServer;
}

bool UVoxelBasicFunctionLibrary::IsClient() const
{
	return GetQuery().GetInfo(EVoxelQueryInfo::Local).GetNetMode() == NM_Client;
}

bool UVoxelBasicFunctionLibrary::IsServer() const
{
	return GetQuery().GetInfo(EVoxelQueryInfo::Local).GetNetMode() < NM_Client;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FTransform UVoxelBasicFunctionLibrary::GetTransform() const
{
	const FMatrix Transform = GetQuery().GetLocalToQuery().Get(GetQuery());
	ensureVoxelSlow(FTransform(Transform).ToMatrixWithScale().Equals(Transform));
	return FTransform(Transform);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelRuntimePinValue UVoxelBasicFunctionLibrary::ToBuffer(const FVoxelRuntimePinValue& Value) const
{
	const TSharedRef<FVoxelBuffer> Buffer = FVoxelBuffer::Make(Value.GetType());
	Buffer->InitializeFromConstant(Value);
	return FVoxelRuntimePinValue::Make(Buffer, Value.GetType().GetBufferType());
}