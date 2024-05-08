// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Sculpt/VoxelSculptStorageData.h"
#include "VoxelDependency.h"
#include "Serialization/LargeMemoryWriter.h"
#include "Serialization/LargeMemoryReader.h"
#include "Compression/OodleDataCompressionUtil.h"

FVoxelSculptStorageData::FVoxelSculptStorageData(const FName Name)
	: Name(Name)
	, Dependency(FVoxelDependency::Create(STATIC_FNAME("SculptStorage"), Name))
{
}

bool FVoxelSculptStorageData::HasChunks(const FVoxelBox& Bounds) const
{
	VOXEL_FUNCTION_COUNTER();
	checkVoxelSlow(CriticalSection.IsLocked_Read_Debug());

	bool bHasChunks = false;
	Octree->TraverseBounds(FVoxelIntBox::FromFloatBox_WithPadding(Bounds / ChunkSize), [&](const FOctree::FNodeRef& NodeRef)
	{
		if (NodeRef.GetHeight() == 0)
		{
			bHasChunks = true;
		}
		return !bHasChunks;
	});
	return bHasChunks;
}

void FVoxelSculptStorageData::SetDistances(
	const FVoxelIntBox& Bounds,
	const TConstVoxelArrayView<float> Distances)
{
	VOXEL_FUNCTION_COUNTER();
	ensure(Bounds.IsMultipleOf(ChunkSize));

	if (!ensure(Bounds.Count_SmallBox() == Distances.Num()))
	{
		return;
	}

	ON_SCOPE_EXIT
	{
		FVoxelDependency::FInvalidationParameters Parameters;
		Parameters.Bounds = Bounds.ToVoxelBox();
		Dependency->Invalidate(Parameters);
	};

	FVoxelScopeLock_Write Lock(CriticalSection);

	Bounds.IterateChunks(ChunkSize, [&](const FVoxelIntBox& ChunkBounds)
	{
		const FIntVector Key = FVoxelUtilities::DivideFloor(ChunkBounds.Min, ChunkSize);
		TSharedPtr<FChunk>& Chunk = Chunks.FindOrAdd(Key);
		if (!Chunk)
		{
			Chunk = MakeVoxelShared<FChunk>(NoInit);
			// We're not going to query the source data, Distances needs to have everything we need
			ensure(ChunkBounds == ChunkBounds.MakeMultipleOfBigger(ChunkSize));

			Octree->TraverseBounds(FVoxelIntBox(Key), [&](const FOctree::FNodeRef& NodeRef)
			{
				if (NodeRef.GetHeight() > 0)
				{
					Octree->CreateAllChildren(NodeRef);
				}
			});
		}

		const FIntVector Size = Bounds.Size();
		const FIntVector Offset = ChunkBounds.Min - Bounds.Min;
		FChunk& ChunkData = *Chunk;

		for (int32 Z = 0; Z < ChunkSize; Z++)
		{
			for (int32 Y = 0; Y < ChunkSize; Y++)
			{
				for (int32 X = 0; X < ChunkSize; X++)
				{
					const int32 ReadIndex = FVoxelUtilities::Get3DIndex<int32>(Size, Offset.X + X, Offset.Y + Y, Offset.Z + Z);
					const int32 WriteIndex = FVoxelUtilities::Get3DIndex<int32>(ChunkSize, X, Y, Z);
					ChunkData[WriteIndex] = ToDensity(Distances[ReadIndex]);
				}
			}
		}
	});
}

void FVoxelSculptStorageData::ClearData()
{
	VOXEL_FUNCTION_COUNTER();

	{
		FVoxelScopeLock_Write Lock(CriticalSection);
		Octree = MakeVoxelShared<FOctree>();
		Chunks.Empty();
	}

	Dependency->Invalidate();
}

void FVoxelSculptStorageData::Serialize(FArchive& Ar)
{
	VOXEL_FUNCTION_COUNTER();

	using FVersion = DECLARE_VOXEL_VERSION
	(
		FirstVersion
	);

	int32 Version = FVersion::LatestVersion;
	Ar << Version;
	check(Version == FVersion::FirstVersion);

	if (Ar.IsSaving())
	{
		FLargeMemoryWriter Writer;
		{
			FVoxelScopeLock_Read Lock(CriticalSection);

			int32 DensitySize = sizeof(FDensity);
			Writer << DensitySize;

			TVoxelArray<FIntVector> Keys;
			Keys.Reserve(Chunks.Num());
			for (const auto& It : Chunks)
			{
				Keys.Add(It.Key);
			}
			Writer << Keys;

			for (const FIntVector& Key : Keys)
			{
				Writer << *Chunks[Key];
			}
		}

		TArray64<uint8> CompressedData;
		{
			VOXEL_SCOPE_COUNTER("Compress");
			ensure(FOodleCompressedArray::CompressData64(
				CompressedData,
				Writer.GetData(),
				Writer.TotalSize(),
				FOodleDataCompression::ECompressor::Kraken,
				FOodleDataCompression::ECompressionLevel::Normal));
		}

		CompressedData.BulkSerialize(Ar);
	}
	else
	{
		check(Ar.IsLoading());

		// Invalidate outside of the lock
		FVoxelDependencyInvalidationScope InvalidationScope;

		Octree = MakeVoxelShared<FOctree>();
		Chunks.Empty();

		TArray64<uint8> CompressedData;
		CompressedData.BulkSerialize(Ar);

		TArray64<uint8> Data;
		{
			VOXEL_SCOPE_COUNTER("Decompress");
			if (!ensure(FOodleCompressedArray::DecompressToTArray64(Data, CompressedData)))
			{
				return;
			}
		}

		FLargeMemoryReader Reader(Data.GetData(), Data.Num());

		FVoxelScopeLock_Write Lock(CriticalSection);

		int32 DensitySize = 0;
		Reader << DensitySize;

		if (!ensure(DensitySize == sizeof(FDensity)))
		{
			return;
		}

		TArray<FIntVector> Keys;
		Reader << Keys;

		for (const FIntVector& Key : Keys)
		{
			const TSharedRef<FChunk> Chunk = MakeVoxelShared<FChunk>(ForceInit);
			Reader << *Chunk;
			Chunks.Add_CheckNew(Key, Chunk);

			Octree->TraverseBounds(FVoxelIntBox(Key), [&](const FOctree::FNodeRef& NodeRef)
			{
				if (NodeRef.GetHeight() > 0)
				{
					Octree->CreateAllChildren(NodeRef);
				}
			});
		}

		Dependency->Invalidate();
	}
}