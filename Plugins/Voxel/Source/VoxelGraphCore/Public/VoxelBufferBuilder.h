// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelBuffer.h"

class VOXELGRAPHCORE_API FVoxelBufferBuilder
{
public:
	const FVoxelPinType InnerType;

	explicit FVoxelBufferBuilder(const FVoxelPinType& InnerType);
	UE_NONCOPYABLE(FVoxelBufferBuilder);

	FORCEINLINE int32 Num() const
	{
#if VOXEL_DEBUG
		CheckNum();
#endif
		return PrivateNum;
	}

	template<typename LambdaType>
	FORCEINLINE void ForeachTerminalBuffer(const FVoxelBuffer& OtherBuffer, LambdaType&& Lambda)
	{
		checkVoxelSlow(Buffer->GetStruct() == OtherBuffer.GetStruct());
		checkVoxelSlow(Buffer->NumTerminalBuffers() == OtherBuffer.NumTerminalBuffers());

		for (int32 Index = 0; Index < Buffer->NumTerminalBuffers(); Index++)
		{
			Lambda(Buffer->GetTerminalBuffer(Index), OtherBuffer.GetTerminalBuffer(Index));
		}
	}
	template<typename LambdaType>
	FORCEINLINE void ForeachTerminalBuffer(const FVoxelBufferBuilder& OtherBuffer, LambdaType&& Lambda)
	{
		return this->ForeachTerminalBuffer(*OtherBuffer.Buffer, MoveTemp(Lambda));
	}

	void Allocate(int32 Num);
	int32 Add(const FVoxelRuntimePinValue& Value);
	int32 AddZeroed(int32 NumToAdd);
	int32 BulkAdd(const FVoxelRuntimePinValue& Value, int32 NumToAdd);
	void Append(const FVoxelBuffer& BufferToAppend, int32 Num);
	bool CopyFrom(const FVoxelBuffer& BufferToCopy, int32 BufferIndex, int32 DestIndex);
	TSharedRef<FVoxelBuffer> MakeBuffer();

	template<typename T>
	int32 BulkAdd(const T& Value, const int32 NumToAdd)
	{
		return BulkAdd(FVoxelRuntimePinValue::Make(Value), NumToAdd);
	}

private:
	const TSharedRef<FVoxelBuffer> Buffer;
	int32 PrivateNum = 0;
	bool bMakeCalled = false;

	void CheckNum() const;
};