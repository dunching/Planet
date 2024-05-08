// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelBufferBuilder.h"

FVoxelBufferBuilder::FVoxelBufferBuilder(const FVoxelPinType& InnerType)
	: InnerType(InnerType)
	, Buffer(FVoxelBuffer::Make(InnerType))
{
	for (FVoxelTerminalBuffer& TerminalBuffer : Buffer->GetTerminalBuffers())
	{
		if (FVoxelSimpleTerminalBuffer* SimpleTerminalBuffer = Cast<FVoxelSimpleTerminalBuffer>(TerminalBuffer))
		{
			SimpleTerminalBuffer->SetStorage(SimpleTerminalBuffer->MakeNewStorage());
		}
		else
		{
			FVoxelComplexTerminalBuffer& ComplexTerminalBuffer = CastChecked<FVoxelComplexTerminalBuffer>(TerminalBuffer);
			ComplexTerminalBuffer.SetStorage(ComplexTerminalBuffer.MakeNewStorage());
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelBufferBuilder::Allocate(const int32 Num)
{
	check(PrivateNum == 0);
	PrivateNum = Num;

	for (FVoxelTerminalBuffer& TerminalBuffer : Buffer->GetTerminalBuffers())
	{
		if (const FVoxelSimpleTerminalBuffer* SimpleTerminalBuffer = Cast<FVoxelSimpleTerminalBuffer>(TerminalBuffer))
		{
			SimpleTerminalBuffer->GetMutableStorage().Allocate(Num);
		}
		else
		{
			CastChecked<FVoxelComplexTerminalBuffer>(TerminalBuffer).GetMutableStorage().Allocate(Num);
		}
	}

	CheckNum();
}

int32 FVoxelBufferBuilder::Add(const FVoxelRuntimePinValue& Value)
{
	return BulkAdd(Value, 1);
}

int32 FVoxelBufferBuilder::AddZeroed(const int32 NumToAdd)
{
	VOXEL_FUNCTION_COUNTER();

	const int32 OldNum = PrivateNum;

	PrivateNum += NumToAdd;

	for (FVoxelTerminalBuffer& TerminalBuffer : Buffer->GetTerminalBuffers())
	{
		if (const FVoxelSimpleTerminalBuffer* SimpleTerminalBuffer = Cast<FVoxelSimpleTerminalBuffer>(TerminalBuffer))
		{
			SimpleTerminalBuffer->GetMutableStorage().AddZeroed(NumToAdd);
		}
		else
		{
			CastChecked<FVoxelComplexTerminalBuffer>(TerminalBuffer).GetMutableStorage().AddZeroed(NumToAdd);
		}
	}

	CheckNum();

	return OldNum;
}

int32 FVoxelBufferBuilder::BulkAdd(const FVoxelRuntimePinValue& Value, const int32 NumToAdd)
{
	VOXEL_FUNCTION_COUNTER();
	check(Buffer->GetInnerType() == Value.GetType());

	const int32 OldNum = PrivateNum;

	PrivateNum += NumToAdd;

	const TSharedRef<FVoxelBuffer> Default = FVoxelBuffer::Make(InnerType);
	Default->InitializeFromConstant(Value);

	check(Default->NumTerminalBuffers() == Buffer->NumTerminalBuffers());
	for (int32 Index = 0; Index < Buffer->NumTerminalBuffers(); Index++)
	{
		FVoxelTerminalBuffer& TerminalBuffer = Buffer->GetTerminalBuffer(Index);
		const FVoxelTerminalBuffer& ConstantTerminalBuffer = Default->GetTerminalBuffer(Index);

		if (FVoxelSimpleTerminalBuffer* SimpleTerminalBuffer = Cast<FVoxelSimpleTerminalBuffer>(TerminalBuffer))
		{
			VOXEL_SWITCH_TERMINAL_TYPE_SIZE(SimpleTerminalBuffer->GetTypeSize())
			{
				using Type = VOXEL_GET_TYPE(TypeInstance);

				SimpleTerminalBuffer->GetMutableStorage<Type>().BulkAdd(
					CastChecked<FVoxelSimpleTerminalBuffer>(ConstantTerminalBuffer).GetStorage<Type>().GetConstant(),
					NumToAdd);
			};
		}
		else
		{
			CastChecked<FVoxelComplexTerminalBuffer>(TerminalBuffer).GetMutableStorage().BulkAdd(
				CastChecked<FVoxelComplexTerminalBuffer>(ConstantTerminalBuffer).GetStorage().GetConstant(),
				NumToAdd);
		}
	}

	CheckNum();

	return OldNum;
}

void FVoxelBufferBuilder::Append(const FVoxelBuffer& BufferToAppend, const int32 Num)
{
	VOXEL_FUNCTION_COUNTER();

	if (!ensure(BufferToAppend.GetStruct() == Buffer->GetStruct()) ||
		!ensure(BufferToAppend.IsConstant() || BufferToAppend.Num() == Num))
	{
		return;
	}

	PrivateNum += Num;

	check(BufferToAppend.NumTerminalBuffers() == Buffer->NumTerminalBuffers());
	for (int32 Index = 0; Index < Buffer->NumTerminalBuffers(); Index++)
	{
		FVoxelTerminalBuffer& TerminalBuffer = Buffer->GetTerminalBuffer(Index);
		const FVoxelTerminalBuffer& TerminalBufferToAppend = BufferToAppend.GetTerminalBuffer(Index);

		if (const FVoxelSimpleTerminalBuffer* SimpleTerminalBuffer = Cast<FVoxelSimpleTerminalBuffer>(TerminalBuffer))
		{
			SimpleTerminalBuffer->GetMutableStorage().Append(
				CastChecked<FVoxelSimpleTerminalBuffer>(TerminalBufferToAppend).GetStorage(),
				Num);
		}
		else
		{
			CastChecked<FVoxelComplexTerminalBuffer>(TerminalBuffer).GetMutableStorage().Append(
				CastChecked<FVoxelComplexTerminalBuffer>(TerminalBufferToAppend).GetStorage(),
				Num);
		}
	}

	CheckNum();
}

bool FVoxelBufferBuilder::CopyFrom(const FVoxelBuffer& BufferToCopy, const int32 BufferIndex, const int32 DestIndex)
{
	VOXEL_FUNCTION_COUNTER();

	if (!ensure(BufferToCopy.GetStruct() == Buffer->GetStruct()))
	{
		return false;
	}

	bool bChanged = false;
	check(BufferToCopy.NumTerminalBuffers() == Buffer->NumTerminalBuffers());
	for (int32 Index = 0; Index < Buffer->NumTerminalBuffers(); Index++)
	{
		FVoxelTerminalBuffer& TerminalBuffer = Buffer->GetTerminalBuffer(Index);
		const FVoxelTerminalBuffer& OtherTerminalBuffer = BufferToCopy.GetTerminalBuffer(Index);

		if (const FVoxelSimpleTerminalBuffer* SimpleTerminalBuffer = Cast<FVoxelSimpleTerminalBuffer>(TerminalBuffer))
		{
			VOXEL_SWITCH_TERMINAL_TYPE_SIZE(SimpleTerminalBuffer->GetTypeSize())
			{
				const int32 LocalBufferIndex = BufferIndex;
				using Type = VOXEL_GET_TYPE(TypeInstance);

				Type& OldValueRef = SimpleTerminalBuffer->GetMutableStorage<Type>()[DestIndex];
				const Type NewValue = CastChecked<FVoxelSimpleTerminalBuffer>(OtherTerminalBuffer).GetStorage<Type>()[LocalBufferIndex];

				if (OldValueRef == NewValue)
				{
					return;
				}

				bChanged = true;
				OldValueRef = NewValue;
			};
		}
		else
		{
			FVoxelStructView OldValueRef = CastChecked<FVoxelComplexTerminalBuffer>(TerminalBuffer).GetMutableStorage()[Index];
			const FConstVoxelStructView NewValue = CastChecked<FVoxelComplexTerminalBuffer>(OtherTerminalBuffer).GetStorage()[Index];

			if (OldValueRef.Identical(NewValue))
			{
				continue;
			}

			bChanged = true;
			NewValue.CopyTo(OldValueRef);
		}
	}
	return bChanged;
}

TSharedRef<FVoxelBuffer> FVoxelBufferBuilder::MakeBuffer()
{
	check(!bMakeCalled);
	bMakeCalled = true;

	CheckNum();

	// Make a copy so buffer nums are initialized properly
	const TSharedRef<FVoxelBuffer> Result = FVoxelBuffer::Make(InnerType);

	check(Result->NumTerminalBuffers() == Buffer->NumTerminalBuffers());
	for (int32 Index = 0; Index < Buffer->NumTerminalBuffers(); Index++)
	{
		const FVoxelTerminalBuffer& TerminalBuffer = Buffer->GetTerminalBuffer(Index);
		FVoxelTerminalBuffer& ResultTerminalBuffer = Result->GetTerminalBuffer(Index);

		if (const FVoxelSimpleTerminalBuffer* SimpleTerminalBuffer = Cast<FVoxelSimpleTerminalBuffer>(TerminalBuffer))
		{
			CastChecked<FVoxelSimpleTerminalBuffer>(ResultTerminalBuffer).SetStorage(SimpleTerminalBuffer->GetSharedStorage());
		}
		else
		{
			const FVoxelComplexTerminalBuffer& ComplexTerminalBuffer = CastChecked<FVoxelComplexTerminalBuffer>(TerminalBuffer);
			CastChecked<FVoxelComplexTerminalBuffer>(ResultTerminalBuffer).SetStorage(ComplexTerminalBuffer.GetSharedStorage());
		}

		ensure(ResultTerminalBuffer.Num() == PrivateNum);
	}

	ensure(Result->Num() == PrivateNum);
	return Result;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelBufferBuilder::CheckNum() const
{
	for (const FVoxelTerminalBuffer& TerminalBuffer : Buffer->GetTerminalBuffers())
	{
		if (const FVoxelSimpleTerminalBuffer* SimpleTerminalBuffer = Cast<FVoxelSimpleTerminalBuffer>(TerminalBuffer))
		{
			ensure(SimpleTerminalBuffer->GetStorage().Num() == PrivateNum);
		}
		else
		{
			ensure(CastChecked<FVoxelComplexTerminalBuffer>(TerminalBuffer).GetStorage().Num() == PrivateNum);
		}
	}
}