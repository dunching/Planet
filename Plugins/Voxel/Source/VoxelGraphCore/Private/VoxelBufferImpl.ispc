// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelMinimal.isph"

export void VoxelBufferStorage_FixupSignBit(
	uniform float Distances[],
	const uniform int32 Num)
{
	FOREACH(Index, 0, Num)
	{
		if (intbits(Distances[Index]) == 0x80000000)
		{
			Distances[Index] = 0;
		}
	}
}