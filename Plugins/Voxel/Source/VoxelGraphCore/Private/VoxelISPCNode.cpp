// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelISPCNode.h"
#include "VoxelISPCNodeImpl.h"
#include "VoxelBuffer.h"
#include "VoxelQueryCache.h"

void FVoxelISPCNode::PreCompile()
{
	ensure(!CachedPtr);
	CachedPtr = GVoxelNodeISPCPtrs.FindRef(GetStruct()->GetFName());

	for (const FVoxelPin& Pin : GetPins())
	{
		CachedPins.Add(FCachedPin
		{
			Pin.Name,
			Pin.bIsInput,
			Pin.GetType(),
			GetNodeRuntime().GetPinData(Pin.Name).PinId
		});
	}
}

FVoxelComputeValue FVoxelISPCNode::CompileCompute(const FName ReturnPinName) const
{
	if (!ensure(CachedPtr))
	{
		return {};
	}

	const FVoxelNodeRuntime::FPinData& ReturnPinData = GetNodeRuntime().GetPinData(ReturnPinName);

	return [this, ReturnPinId = ReturnPinData.PinId](const FVoxelQuery& Query) -> FVoxelFutureValue
	{
		checkVoxelSlow(FVoxelTaskReferencer::Get().IsReferenced(this));
		{
			FVoxelQueryCache::FEntry& Entry = Query.GetQueryCache().FindOrAddEntry(ReturnPinId);
			VOXEL_SCOPE_LOCK(Entry.CriticalSection);
			if (Entry.Value.IsValid())
			{
				return Entry.Value;
			}
		}

		using FAllocatorType = TVoxelInlineAllocator<16>;

		TVoxelArray<FVoxelFutureValue, FAllocatorType> InputValues;
		TVoxelArray<TSharedRef<FVoxelFutureValueStateImpl>, FAllocatorType> OutputStates;
		for (const FCachedPin& CachedPin : CachedPins)
		{
			if (CachedPin.bIsInput)
			{
				InputValues.Add(GetNodeRuntime().Get(FVoxelPinRef(CachedPin.Name), Query));
			}
			else
			{
				FVoxelQueryCache::FEntry& Entry = Query.GetQueryCache().FindOrAddEntry(CachedPin.PinId);
				VOXEL_SCOPE_LOCK(Entry.CriticalSection);

				const TSharedRef<FVoxelFutureValueStateImpl> State = MakeVoxelShared<FVoxelFutureValueStateImpl>(CachedPin.PinType);

				ensure(!Entry.Value.IsValid());
				Entry.Value = FVoxelFutureValue(State);

				OutputStates.Add(State);
			}
		}

		MakeVoxelTask(STATIC_FNAME("FVoxelISPCNode"))
		.Dependencies(InputValues)
		.Execute([=]
		{
			int32 Num = 1;
			for (const FVoxelFutureValue& InputValue : InputValues)
			{
				const FVoxelRuntimePinValue& Value = InputValue.GetValue_CheckCompleted();
				if (!Value.GetType().IsBuffer())
				{
					// Uniform node
					continue;
				}

				const int32 NewNum = Value.Get<FVoxelBuffer>().Num();
				if (Num == 1)
				{
					Num = NewNum;
					continue;
				}
				if (NewNum == 1 ||
					NewNum == Num)
				{
					continue;
				}

				RaiseBufferError(*this);
				for (const TSharedRef<FVoxelFutureValueStateImpl>& State : OutputStates)
				{
					State->SetValue(FVoxelRuntimePinValue(State->Type));
				}
				return;
			}

			if (Num == 0)
			{
				for (const TSharedRef<FVoxelFutureValueStateImpl>& State : OutputStates)
				{
					FVoxelRuntimePinValue Value(State->Type);
					if (ensureVoxelSlow(Value.IsBuffer()))
					{
						ConstCast(Value.Get<FVoxelBuffer>()).SetAsEmpty();
					}
					State->SetValue(Value);
				}
				return;
			}

			TVoxelArray<TSharedRef<FVoxelBuffer>, FAllocatorType> InputBuffers;
			TVoxelArray<TSharedRef<FVoxelBuffer>, FAllocatorType> OutputBuffers;
			InputBuffers.Reserve(CachedPins.Num());
			OutputBuffers.Reserve(CachedPins.Num());

			TVoxelArray<const FVoxelBuffer*, FAllocatorType> Buffers;
			Buffers.Reserve(CachedPins.Num());

			int32 InputIndex = 0;
			int32 NumTerminalBuffers = 0;
			for (const FCachedPin& CachedPin : CachedPins)
			{
				const FVoxelBuffer* Buffer;
				if (CachedPin.bIsInput)
				{
					const FVoxelRuntimePinValue& Value = InputValues[InputIndex++].GetValue_CheckCompleted();
					checkVoxelSlow(Value.GetType().CanBeCastedTo(CachedPin.PinType));

					if (CachedPin.PinType.IsBuffer())
					{
						Buffer = &Value.Get<FVoxelBuffer>();
					}
					else
					{
						const TSharedRef<FVoxelBuffer> InputBuffer = FVoxelBuffer::Make(CachedPin.PinType);
						InputBuffer->InitializeFromConstant(Value);
						InputBuffers.Add(InputBuffer);
						Buffer = &InputBuffer.Get();
					}
				}
				else
				{
					const TSharedRef<FVoxelBuffer> OutputBuffer = FVoxelBuffer::Make(CachedPin.PinType.GetInnerType());
					for (FVoxelTerminalBuffer& TerminalBuffer : OutputBuffer->GetTerminalBuffers())
					{
						FVoxelSimpleTerminalBuffer& SimpleBuffer = CastChecked<FVoxelSimpleTerminalBuffer>(TerminalBuffer);
						const TSharedRef<FVoxelBufferStorage> Storage = SimpleBuffer.MakeNewStorage();
						Storage->Allocate(Num);
						SimpleBuffer.SetStorage(Storage);
					}
					OutputBuffers.Add(OutputBuffer);
					Buffer = &OutputBuffer.Get();
				}
				Buffers.Add(Buffer);
				NumTerminalBuffers += Buffer->NumTerminalBuffers();
			}

			{
				VOXEL_SCOPE_COUNTER_FORMAT("%s Num=%d", *GetStruct()->GetName(), Num);
				checkVoxelSlow(CachedPtr);
				FVoxelNodeStatScope StatScope(*this, Num);

				ForeachVoxelBufferChunk(Num, [&](const FVoxelBufferIterator& Iterator)
				{
					TVoxelArray<ispc::FVoxelBuffer, TVoxelInlineAllocator<16>> ISPCBuffers;
					ISPCBuffers.Reserve(NumTerminalBuffers);
					for (int32 Index = 0; Index < CachedPins.Num(); Index++)
					{
						const FVoxelBuffer* Buffer = Buffers[Index];

						for (const FVoxelTerminalBuffer& TerminalBuffer : Buffer->GetTerminalBuffers())
						{
							const FVoxelSimpleTerminalBuffer& SimpleTerminalBuffer = CastChecked<FVoxelSimpleTerminalBuffer>(TerminalBuffer);
							check(SimpleTerminalBuffer.IsConstant() || SimpleTerminalBuffer.Num() == Num);

							ispc::FVoxelBuffer& ISPCBuffer = ISPCBuffers.Emplace_GetRef();
							ISPCBuffer.Data = ConstCast(SimpleTerminalBuffer.GetStorage().GetByteData(Iterator));
							ISPCBuffer.bIsConstant = SimpleTerminalBuffer.Num() == 1;
						}
					}

					(*CachedPtr)(ISPCBuffers.GetData(), Iterator.Num());
				});
			}

			check(OutputStates.Num() == OutputBuffers.Num());
			for (int32 Index = 0; Index < OutputStates.Num(); Index++)
			{
				const TSharedRef<FVoxelFutureValueStateImpl>& State = OutputStates[Index];
				const TSharedRef<FVoxelBuffer>& Buffer = OutputBuffers[Index];
				const FVoxelPinType& Type = State->Type;

				Buffer->CheckSlow();

				if (Type.IsBuffer())
				{
					State->SetValue(FVoxelRuntimePinValue::Make(Buffer, Type));
				}
				else
				{
					checkVoxelSlow(Buffer->Num() == 1);
					State->SetValue(Buffer->GetGenericConstant().WithType(Type));
				}
			}
		});

		FVoxelQueryCache::FEntry& Entry = Query.GetQueryCache().FindOrAddEntry(ReturnPinId);
		VOXEL_SCOPE_LOCK(Entry.CriticalSection);
		ensure(Entry.Value.IsValid());
		return Entry.Value;
	};
}