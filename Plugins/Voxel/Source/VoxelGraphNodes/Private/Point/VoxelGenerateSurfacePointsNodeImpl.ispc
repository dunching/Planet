// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelMinimal.isph"

FORCEINLINE void ComputeIndicesHelper(
	const varying int32 ChildIndex,
	const uniform uint32 CellX,
	const uniform uint32 CellY,
	const uniform uint32 CellZ,
	const uniform int32 CellIndex,
	uniform int32 uniform& NumElements,
	uniform int32 ValueIndices[],
	const uniform int32 HashSize,
	uniform int32 HashTable[],
	uniform uint32 Elements_Values[],
	uniform int32 Elements_NextIndices[],
	const uniform int32 Offset)
{
	const varying uint32 ChildCellX = CellX + ((ChildIndex & 0x1) >> 0);
	const varying uint32 ChildCellY = CellY + ((ChildIndex & 0x2) >> 1);
	const varying uint32 ChildCellZ = CellZ + ((ChildIndex & 0x4) >> 2);
	check(ChildCellX < 1024);
	check(ChildCellY < 1024);
	check(ChildCellZ < 1024);

	const varying uint32 ChildCell =
		(ChildCellX << 0) |
		(ChildCellY << 10) |
		(ChildCellZ << 20);

	// Ensure HashIndex never collides between the 8 children
	const varying uint32 Hash =
		((ChildCellX & 0x1) << 0) |
		((ChildCellY & 0x1) << 1) |
		((ChildCellZ & 0x1) << 2) |
		(MurmurHash32(ChildCell) << 3);

	const varying uint32 HashIndex = Hash & (HashSize - 1);

#if VOXEL_DEBUG
	UNROLL
	for (uniform int32 Index = 1; Index < TARGET_WIDTH; Index++)
	{
		check(extract(HashIndex, 0) != extract(HashIndex, Index));
	}
#endif

	IGNORE_PERF_WARNING
	varying int32 ElementIndex = HashTable[HashIndex];

	ElementIndex = select(
		or
		(
			ElementIndex == -1,
			// Safe to access [-1]
			IGNORE_PERF_WARNING
			Elements_Values[ElementIndex] == ChildCell
		),
		ElementIndex,
		IGNORE_PERF_WARNING
		Elements_NextIndices[ElementIndex]);

	if (any(and(
		ElementIndex != -1,
		IGNORE_PERF_WARNING
		Elements_Values[ElementIndex] != ChildCell)))
	{
		UNROLL
		for (uniform int32 Index = 0; Index < TARGET_WIDTH; Index++)
		{
			uniform int32 LocalElementIndex = extract(ElementIndex, Index);
			while (
				LocalElementIndex != -1 &&
				Elements_Values[LocalElementIndex] != ChildCell)
			{
				LocalElementIndex = Elements_NextIndices[LocalElementIndex];
			}
			ElementIndex = insert(ElementIndex, Index, LocalElementIndex);
		}
	}

	const varying int32 IsNew = ElementIndex == -1;

	UNROLL
	for (uniform int32 Index = 0; Index < TARGET_WIDTH; Index++)
	{
		if (!extract(IsNew, Index))
		{
			continue;
		}

		const uniform int32 NewElementIndex = NumElements++;
		ElementIndex = insert(ElementIndex, Index, NewElementIndex);

		const uniform uint32 LocalChildCell = extract(ChildCell, Index);
		const uniform uint32 LocalHashIndex = extract(HashIndex, Index);

		Elements_Values[NewElementIndex] = LocalChildCell;
		Elements_NextIndices[NewElementIndex] = HashTable[LocalHashIndex];
		HashTable[LocalHashIndex] = NewElementIndex;
	}

	ValueIndices[8 * CellIndex + ChildIndex] = Offset + ElementIndex;
}

export uniform int32 GenerateSurfacePointsBuilder_ComputeFinalDistances_ComputeIndices(
	const uniform uint32 Cells[],
	uniform int32 ValueIndices[],
	const uniform int32 HashSize,
	uniform int32 HashTable[],
	uniform uint32 Elements_Values[],
	uniform int32 Elements_NextIndices[],
	const uniform int32 Offset,
	const uniform int32 Num)
{
	check(IsPowerOfTwo(HashSize));

	uniform int32 NumElements = 0;
	for (uniform int32 CellIndex = 0; CellIndex < Num; CellIndex++)
	{
		const uniform uint32 Cell = Cells[CellIndex];
		const uniform uint32 CellX = (Cell >> 0) & 1023;
		const uniform uint32 CellY = (Cell >> 10) & 1023;
		const uniform uint32 CellZ = (Cell >> 20) & 1023;

#if TARGET_WIDTH == 8
		const varying int32 ChildIndex = MakeRegister(0, 1, 2, 3, 4, 5, 6, 7);

		ComputeIndicesHelper(
			ChildIndex,
			CellX,
			CellY,
			CellZ,
			CellIndex,
			NumElements,
			ValueIndices,
			HashSize,
			HashTable,
			Elements_Values,
			Elements_NextIndices,
			Offset);
#elif TARGET_WIDTH == 4
		const varying int32 ChildIndexA = MakeRegister(0, 1, 2, 3);
		const varying int32 ChildIndexB = MakeRegister(4, 5, 6, 7);

		ComputeIndicesHelper(
			ChildIndexA,
			CellX,
			CellY,
			CellZ,
			CellIndex,
			NumElements,
			ValueIndices,
			HashSize,
			HashTable,
			Elements_Values,
			Elements_NextIndices,
			Offset);

		ComputeIndicesHelper(
			ChildIndexB,
			CellX,
			CellY,
			CellZ,
			CellIndex,
			NumElements,
			ValueIndices,
			HashSize,
			HashTable,
			Elements_Values,
			Elements_NextIndices,
			Offset);
#else
#error "Unsupported TARGET_WIDTH"
		unsupportedTargetWidth();
#endif
	}
	return NumElements;
}

export void GenerateSurfacePointsBuilder_ComputeFinalDistances_WriteIndices(
	uniform uint32 Elements_Values[],
	uniform float QueryX[],
	uniform float QueryY[],
	uniform float QueryZ[],
	const uniform float OffsetX,
	const uniform float OffsetY,
	const uniform float OffsetZ,
	const uniform float BaseCellSize,
	const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		const varying uint32 Cell = Elements_Values[Index];

		const varying int32 X = (Cell >> 0) & 1023;
		const varying int32 Y = (Cell >> 10) & 1023;
		const varying int32 Z = (Cell >> 20) & 1023;

		QueryX[Index] = OffsetX + X * BaseCellSize;
		QueryY[Index] = OffsetY + Y * BaseCellSize;
		QueryZ[Index] = OffsetZ + Z * BaseCellSize;
	}
}