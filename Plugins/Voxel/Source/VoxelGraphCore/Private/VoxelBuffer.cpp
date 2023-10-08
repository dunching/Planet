// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelBuffer.h"
#include "Buffer/VoxelNameBuffer.h"
#include "Buffer/VoxelBaseBuffers.h"
#include "Buffer/VoxelClassBuffer.h"

struct FVoxelBufferStatics
{
	bool bInitialized = false;
	TVoxelAddOnlyMap<FVoxelPinType, UScriptStruct*> InnerToBuffer;
	TVoxelAddOnlyMap<UScriptStruct*, FVoxelPinType> BufferToInner;
};
FVoxelBufferStatics GVoxelBufferStatics;

VOXEL_RUN_ON_STARTUP(FVoxelBufferStatics_Initialize, Game, 999)
{
	VOXEL_FUNCTION_COUNTER();
	check(IsInGameThread());

	check(!GVoxelBufferStatics.bInitialized);
	GVoxelBufferStatics.bInitialized = true;

	for (UScriptStruct* Struct : GetDerivedStructs<FVoxelBuffer>())
	{
		if (Struct == StaticStructFast<FVoxelTerminalBuffer>() ||
			Struct == StaticStructFast<FVoxelSimpleTerminalBuffer>() ||
			Struct == StaticStructFast<FVoxelComplexTerminalBuffer>())
		{
			continue;
		}

		TVoxelInstancedStruct<FVoxelBuffer> Buffer(Struct);

		const FVoxelPinType InnerType = Buffer->GetInnerType();

		GVoxelBufferStatics.InnerToBuffer.Add_CheckNew(InnerType, Struct);
		GVoxelBufferStatics.BufferToInner.Add_CheckNew(Struct, InnerType);
	}

	GOnVoxelModuleUnloaded_DoCleanup.AddLambda([]
	{
		GVoxelBufferStatics = {};
	});
}

TSharedRef<FVoxelBuffer> FVoxelBuffer::Make(const FVoxelPinType& InnerType)
{
	checkVoxelSlow(!InnerType.IsBuffer());
	checkVoxelSlow(GVoxelBufferStatics.bInitialized);

	if (InnerType.Is<uint8>())
	{
		return MakeVoxelShared<FVoxelByteBuffer>();
	}
	if (InnerType.IsClass())
	{
		return MakeVoxelShared<FVoxelClassBuffer>();
	}
	if (InnerType.GetInternalType() == EVoxelPinInternalType::Name)
	{
		return MakeVoxelShared<FVoxelNameBuffer>();
	}

	if (const UScriptStruct* BufferStruct = GVoxelBufferStatics.InnerToBuffer.FindRef(InnerType))
	{
		return MakeSharedStruct<FVoxelBuffer>(BufferStruct);
	}

	const TSharedRef<FVoxelComplexTerminalBuffer> Result = MakeVoxelShared<FVoxelComplexTerminalBuffer>();
	Result->Initialize(InnerType);
	return Result;
}

FVoxelPinType FVoxelBuffer::FindInnerType_NotComplex(UScriptStruct* Struct)
{
	checkVoxelSlow(GVoxelBufferStatics.bInitialized);
	return GVoxelBufferStatics.BufferToInner[Struct];
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelBuffer::SetAsEmpty()
{
	for (FVoxelTerminalBuffer& TerminalBuffer : GetTerminalBuffers())
	{
		TerminalBuffer.SetAsEmpty();
	}
}

void FVoxelBuffer::Shrink()
{
	for (FVoxelTerminalBuffer& TerminalBuffer : GetTerminalBuffers())
	{
		TerminalBuffer.Shrink();
	}
}

void FVoxelBuffer::CheckSlowImpl() const
{
	for (const FVoxelTerminalBuffer& TerminalBuffer : GetTerminalBuffers())
	{
		TerminalBuffer.CheckSlow();
	}
}

int64 FVoxelBuffer::GetAllocatedSize() const
{
	int64 AllocatedSize = 0;
	for (const FVoxelTerminalBuffer& Buffer : GetTerminalBuffers())
	{
		AllocatedSize += Buffer.GetAllocatedSize();
	}
	return AllocatedSize;
}

int32 FVoxelBuffer::Num_Slow() const
{
	ensure(NumTerminalBuffers() > 1);

	int32 NewNum = 1;
	for (const FVoxelTerminalBuffer& Buffer : GetTerminalBuffers())
	{
		ensure(FVoxelBufferAccessor::MergeNum(NewNum, Buffer));
	}
	return NewNum;
}

bool FVoxelBuffer::IsValid_Slow() const
{
	if (NumTerminalBuffers() == 1)
	{
		return true;
	}

	int32 ActualNum = 1;
	bool bValid = true;
	for (const FVoxelTerminalBuffer& Buffer : GetTerminalBuffers())
	{
		const int32 BufferNum = Buffer.Num();

		if (ActualNum == 1)
		{
			ActualNum = BufferNum;
		}
		else if (BufferNum != ActualNum && BufferNum != 1)
		{
			bValid = false;
		}
	}

	if (!ensure(ActualNum == Num()))
	{
		return false;
	}

	return bValid;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelRuntimePinValue FVoxelBuffer::GetGenericConstant() const
{
	check(IsConstant());
	return GetGeneric(0);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelBuffer::ComputeBuffers(
	const FVoxelBuffer* Template,
	TArray<int64>& BufferOffsets)
{
	for (const FProperty& Property : GetStructProperties(Template->GetStruct()))
	{
		const UScriptStruct* Struct = CastFieldChecked<FStructProperty>(Property).Struct;
		check(Struct->IsChildOf(FVoxelBuffer::StaticStruct()));

		if (Struct->IsChildOf(FVoxelTerminalBuffer::StaticStruct()))
		{
			const uint8* Pointer = Property.ContainerPtrToValuePtr<uint8>(Template);
			BufferOffsets.Add(Pointer - reinterpret_cast<const uint8*>(Template));
		}
		else
		{
			check(Struct->GetSuperStruct() == StaticStructFast<FVoxelBuffer>());

			const FVoxelBuffer& Buffer = *Property.ContainerPtrToValuePtr<FVoxelBuffer>(Template);
			for (const int64 BufferOffset : Buffer.PrivateBufferOffsets)
			{
				BufferOffsets.Add(reinterpret_cast<const uint8*>(&Buffer) + BufferOffset - reinterpret_cast<const uint8*>(Template));
			}
		}
	}
	BufferOffsets.Shrink();
	check(BufferOffsets.Num() > 0);
}

void FVoxelBuffer::UpdateNum()
{
	ensure(NumTerminalBuffers() > 1);

	int32 NewNum = 1;
	for (const FVoxelTerminalBuffer& Buffer : GetTerminalBuffers())
	{
		ensure(FVoxelBufferAccessor::MergeNum(NewNum, Buffer));
	}

	// PrivateNum == NewNum in race condition
	ensure(PrivateNum == -1 || PrivateNum == NewNum);
	ensure(NewNum != -1);
	PrivateNum = NewNum;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelSimpleTerminalBuffer::SetAsEmpty()
{
	SetStorage(MakeVoxelShared<FVoxelBufferStorage>(GetTypeSize()));
}

void FVoxelSimpleTerminalBuffer::Shrink()
{
	if (IsDefault())
	{
		return;
	}

	GetMutableStorage().Shrink();
}

void FVoxelSimpleTerminalBuffer::CheckSlowImpl() const
{
	PrivateStorage->CheckSlow(GetInnerType());
}

int64 FVoxelSimpleTerminalBuffer::GetAllocatedSize() const
{
	if (IsDefault())
	{
		// Shared storage
		return 0;
	}

	return GetStorage().GetAllocatedSize() + sizeof(FVoxelBufferStorage);
}

int32 FVoxelSimpleTerminalBuffer::Num_Slow() const
{
	return GetStorage().Num();
}

TSharedRef<FVoxelBufferStorage> FVoxelSimpleTerminalBuffer::MakeNewStorage() const
{
	return MakeVoxelShared<FVoxelBufferStorage>(GetTypeSize());
}

void FVoxelSimpleTerminalBuffer::SetStorage(const TSharedRef<const FVoxelBufferStorage>& Storage)
{
	check(Storage->GetTypeSize() == GetTypeSize());
	checkVoxelSlow(IsDefault());

	PrivateStorage = Storage;
	PrivateNum = Storage->Num();

	CheckSlow();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelComplexTerminalBuffer::FVoxelComplexTerminalBuffer()
{
	static const TArray<int64> BufferOffsets = { 0 };
	PrivateBufferOffsets = BufferOffsets;
}

void FVoxelComplexTerminalBuffer::SetAsEmpty()
{
	SetStorage(MakeVoxelShared<FVoxelComplexBufferStorage>(GetInnerStruct()));
}

void FVoxelComplexTerminalBuffer::Shrink()
{
}

void FVoxelComplexTerminalBuffer::CheckSlowImpl() const
{
}

int64 FVoxelComplexTerminalBuffer::GetAllocatedSize() const
{
	if (IsDefault())
	{
		// Shared storage
		return 0;
	}

	return GetStorage().GetAllocatedSize() + sizeof(FVoxelComplexBufferStorage);
}

int32 FVoxelComplexTerminalBuffer::Num_Slow() const
{
	return GetStorage().Num();
}

void FVoxelComplexTerminalBuffer::InitializeFromConstant(const FVoxelRuntimePinValue& Constant)
{
	const TSharedRef<FVoxelComplexBufferStorage> Storage = MakeVoxelShared<FVoxelComplexBufferStorage>(GetInnerStruct());
	Storage->SetConstant(Constant.GetStructView());
	SetStorage(Storage);
}

FVoxelRuntimePinValue FVoxelComplexTerminalBuffer::GetGeneric(const int32 Index) const
{
	return FVoxelRuntimePinValue::MakeStruct(GetStorage()[Index]);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedRef<FVoxelComplexBufferStorage> FVoxelComplexTerminalBuffer::MakeNewStorage() const
{
	return MakeVoxelShared<FVoxelComplexBufferStorage>(GetInnerStruct());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

struct FVoxelComplexTerminalBufferStatics
{
	FVoxelFastCriticalSection CriticalSection;
	TVoxelAddOnlyMap<FVoxelPinType, TSharedPtr<FVoxelComplexBufferStorage>> InnerTypeToStorage;
};
FVoxelComplexTerminalBufferStatics GVoxelComplexTerminalBufferStatics;

VOXEL_RUN_ON_STARTUP_GAME(RegisterVoxelComplexTerminalBufferStaticsCleanup)
{
	GOnVoxelModuleUnloaded_DoCleanup.AddLambda([]
	{
		VOXEL_SCOPE_LOCK(GVoxelComplexTerminalBufferStatics.CriticalSection);

		for (auto& It : GVoxelComplexTerminalBufferStatics.InnerTypeToStorage)
		{
			check(It.Value.GetSharedReferenceCount() == 0);
			FVoxelMemory::Delete(It.Value.Get());
		}
		GVoxelComplexTerminalBufferStatics.InnerTypeToStorage.Empty();
	});
}

void FVoxelComplexTerminalBuffer::Initialize(const FVoxelPinType& InnerType)
{
	checkVoxelSlow(!PrivateStorage.ToSharedPtr().IsValid());

	const TSharedRef<FVoxelComplexBufferStorage> NewStorage = INLINE_LAMBDA
	{
		VOXEL_SCOPE_LOCK(GVoxelComplexTerminalBufferStatics.CriticalSection);

		TSharedPtr<FVoxelComplexBufferStorage>& Storage = GVoxelComplexTerminalBufferStatics.InnerTypeToStorage.FindOrAdd(InnerType);
		if (!Storage)
		{
			Storage = MakeVoxelShared<FVoxelComplexBufferStorage>(InnerType.GetStruct());
			Storage->AddZeroed(1);

			// Clear the reference count to avoid a lot of contention on a few atomics
			ClearSharedPtrReferencer(Storage);
		}
		return Storage.ToSharedRef();
	};

	PrivateStorage = NewStorage;
	PrivateNum = NewStorage->Num();
	PrivateInnerType = InnerType;

	checkVoxelSlow(IsDefault());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelComplexTerminalBuffer::SetStorage(const TSharedRef<const FVoxelComplexBufferStorage>& Storage)
{
	check(Storage->GetInnerStruct() == GetInnerStruct());
	checkVoxelSlow(IsDefault());

	PrivateStorage = Storage;
	PrivateNum = Storage->Num();

	CheckSlow();
}