// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Point/VoxelPointStorageNode.h"
#include "Point/VoxelPointStorage.h"
#include "Point/VoxelPointStorageData.h"
#include "Point/VoxelChunkedPointSet.h"
#include "VoxelDependency.h"
#include "VoxelBufferBuilder.h"

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_ApplyPointStorage, Out)
{
	const TValue<FVoxelPointSet> Points = Get(InPin, Query);

	return VOXEL_ON_COMPLETE(Points)
	{
		FindVoxelQueryParameter(FVoxelPointChunkRefQueryParameter, ChunkRefQueryParameter);
		FindVoxelPointSetAttribute(*Points, FVoxelPointAttributes::Id, FVoxelPointIdBuffer, PointIds);

		const TSharedPtr<const FVoxelRuntimeParameter_PointStorage> RuntimeParameter = Query.GetInfo(EVoxelQueryInfo::Local).FindParameter<FVoxelRuntimeParameter_PointStorage>();
		if (!RuntimeParameter ||
			!ensure(RuntimeParameter->Data))
		{
			return Points;
		}

		FVoxelPointStorageData& PointStorageData = *RuntimeParameter->Data;
		Query.GetDependencyTracker().AddDependency(PointStorageData.Dependency);

		const TSharedRef<const FVoxelPointStorageChunkData> ChunkData = PointStorageData.FindOrAddChunkData(ChunkRefQueryParameter->ChunkRef);

		const FVoxelPointStorageChunkData& Data = *ChunkData;
		Query.GetDependencyTracker().AddDependency(Data.Dependency);

		VOXEL_SCOPE_LOCK(Data.CriticalSection);

		if (Data.NameToAttributeOverride.Num() == 0)
		{
			return Points;
		}

		FVoxelNodeStatScope StatScope(*this, Points->Num());

		TVoxelSet<FName> AttributeNames;
		for (const auto& It : Points->GetAttributes())
		{
			AttributeNames.Add(It.Key);
		}
		for (const auto& It : Data.NameToAttributeOverride)
		{
			AttributeNames.Add(It.Key);
		}
		AttributeNames.Remove(FVoxelPointAttributes::Id);

		const TSharedRef<FVoxelPointSet> NewPoints = MakeVoxelShared<FVoxelPointSet>();
		NewPoints->SetNum(Points->Num());

		const TSharedRef<FVoxelPointIdBufferStorage> NewPointIds = MakeVoxelShared<FVoxelPointIdBufferStorage>();
		NewPointIds->Append(PointIds.GetStorage(), Points->Num());
		NewPoints->Add(FVoxelPointAttributes::Id, FVoxelPointIdBuffer::Make(NewPointIds));

		for (const FName AttributeName : AttributeNames)
		{
			const TSharedPtr<const FVoxelBuffer> ExistingBuffer = Points->Find(AttributeName);
			const TSharedPtr<FVoxelPointStorageChunkData::FAttribute> AttributeOverride = Data.NameToAttributeOverride.FindRef(AttributeName);
			check(ExistingBuffer || AttributeOverride);

			TSharedPtr<FVoxelBufferBuilder> BufferBuilder;
			if (ExistingBuffer)
			{
				BufferBuilder = MakeVoxelShared<FVoxelBufferBuilder>(ExistingBuffer->GetInnerType());
				BufferBuilder->Append(*ExistingBuffer, Points->Num());
			}
			else
			{
				BufferBuilder = MakeVoxelShared<FVoxelBufferBuilder>(AttributeOverride->Buffer->InnerType);
				FVoxelPointAttributes::AddDefaulted(*BufferBuilder, AttributeName, Points->Num());
			}
			check(BufferBuilder->Num() == Points->Num());

			if (AttributeOverride)
			{
				VOXEL_SCOPE_COUNTER_FORMAT("Copy %s", *AttributeName.ToString());

				const TVoxelMap<FVoxelPointId, int32>& PointIdToIndex = AttributeOverride->PointIdToIndex;
				BufferBuilder->ForeachTerminalBuffer(*AttributeOverride->Buffer, [&](FVoxelTerminalBuffer& NewTerminalBuffer, const FVoxelTerminalBuffer& OverrideTerminalBuffer)
				{
					VOXEL_SCOPE_COUNTER("Copy");

					if (const FVoxelSimpleTerminalBuffer* NewSimpleTerminalBuffer = Cast<FVoxelSimpleTerminalBuffer>(NewTerminalBuffer))
					{
						const FVoxelSimpleTerminalBuffer& OverrideSimpleTerminalBuffer = CastChecked<FVoxelSimpleTerminalBuffer>(OverrideTerminalBuffer);

						VOXEL_SWITCH_TERMINAL_TYPE_SIZE(NewSimpleTerminalBuffer->GetTypeSize())
						{
							using Type = VOXEL_GET_TYPE(TypeInstance);

							for (int32 Index = 0; Index < NewPointIds->Num(); Index++)
							{
								const int32* OverrideIndexPtr = PointIdToIndex.Find((*NewPointIds)[Index]);
								if (!OverrideIndexPtr)
								{
									continue;
								}

								NewSimpleTerminalBuffer->GetMutableStorage<Type>()[Index] = OverrideSimpleTerminalBuffer.GetStorage<Type>()[*OverrideIndexPtr];
							}
						};
					}
					else
					{
						for (int32 Index = 0; Index < NewPointIds->Num(); Index++)
						{
							const int32* OverrideIndexPtr = PointIdToIndex.Find((*NewPointIds)[Index]);
							if (!OverrideIndexPtr)
							{
								continue;
							}

							const FConstVoxelStructView Override = CastChecked<FVoxelComplexTerminalBuffer>(OverrideTerminalBuffer).GetStorage()[*OverrideIndexPtr];
							const FVoxelStructView Value = CastChecked<FVoxelComplexTerminalBuffer>(NewTerminalBuffer).GetMutableStorage()[Index];

							Override.CopyTo(Value);
						}
					}
				});
			}

			NewPoints->Add(AttributeName, BufferBuilder->MakeBuffer());
		}
		return NewPoints;
	};
}