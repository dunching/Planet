// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelSelectNode.h"
#include "VoxelBufferUtilities.h"

FVoxelNode_Select::FVoxelNode_Select()
{
	GetPin(IndexPin).SetType(FVoxelPinType::Make<FVoxelBoolBuffer>());
	FixupValuePins();
}

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_Select, Result)
{
	const FVoxelPinType IndexType = GetNodeRuntime().GetPinData(IndexPin).Type;

	if (IndexType.Is<bool>() ||
		IndexType.Is<uint8>() ||
		IndexType.Is<int32>())
	{
		const FVoxelFutureValue IndexValue = Get(IndexPin, Query);

		return VOXEL_ON_COMPLETE(IndexType, IndexValue)
		{
			int32 Index;
			if (IndexType.Is<bool>())
			{
				Index = IndexValue.Get<bool>() ? 1 : 0;
			}
			else if (IndexType.Is<uint8>())
			{
				Index = IndexValue.Get<uint8>();
			}
			else
			{
				Index = IndexValue.Get<int32>();
			}

			if (!ValuePins.IsValidIndex(Index))
			{
				return {};
			}

			return Get(ValuePins[Index], Query);
		};
	}
	else if (
		IndexType.Is<FVoxelBoolBuffer>() ||
		IndexType.Is<FVoxelByteBuffer>() ||
		IndexType.Is<FVoxelInt32Buffer>())
	{
		const TValue<FVoxelBuffer> Indices = Get<FVoxelBuffer>(IndexPin, Query);

		return VOXEL_ON_COMPLETE(IndexType, Indices)
		{
			if (Indices->Num() == 0)
			{
				return {};
			}

			if (Indices->IsConstant())
			{
				if (Indices->IsA<FVoxelBoolBuffer>())
				{
					checkVoxelSlow(ValuePins.Num() == 2);
					return Get(ValuePins[Indices->AsChecked<FVoxelBoolBuffer>().GetConstant() ? 1 : 0], Query);
				}
				else if (Indices->IsA<FVoxelByteBuffer>())
				{
					const int32 Index = Indices->AsChecked<FVoxelByteBuffer>().GetConstant();
					if (!ValuePins.IsValidIndex(Index))
					{
						return {};
					}
					return Get(ValuePins[Index], Query);
				}
				else
				{
					const int32 Index = Indices->AsChecked<FVoxelInt32Buffer>().GetConstant();
					if (!ValuePins.IsValidIndex(Index))
					{
						return {};
					}
					return Get(ValuePins[Index], Query);
				}
			}

			TVoxelArray<TValue<FVoxelBuffer>> Buffers;
			Buffers.Reserve(ValuePins.Num());
			for (const FVoxelPinRef& Pin : ValuePins)
			{
				Buffers.Add(Get<FVoxelBuffer>(Pin, Query));
			}

			return VOXEL_ON_COMPLETE(Buffers, Indices)
			{
				TVoxelArray<const FVoxelBuffer*> BufferPtrs;
				BufferPtrs.Reserve(Buffers.Num());

				int32 Num = Indices->Num();
				for (const TSharedRef<const FVoxelBuffer>& Buffer : Buffers)
				{
					BufferPtrs.Add(&Buffer.Get());

					if (!FVoxelBufferAccessor::MergeNum(Num, *Buffer))
					{
						RaiseBufferError(GetNodeRef());
						return {};
					}
				}

				const FVoxelPinType BufferType = GetNodeRuntime().GetPinData(ResultPin).Type;

				return FVoxelRuntimePinValue::Make(
					FVoxelBufferUtilities::Select(
						BufferType.GetInnerType(),
						*Indices,
						BufferPtrs),
					BufferType);
			};
		};
	}
	else
	{
		ensure(false);
		return {};
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#if WITH_EDITOR
FVoxelPinTypeSet FVoxelNode_Select::GetPromotionTypes(const FVoxelPin& Pin) const
{
	if (Pin.Name == IndexPin)
	{
		FVoxelPinTypeSet OutTypes;

		OutTypes.Add<bool>();
		OutTypes.Add<FVoxelBoolBuffer>();

		OutTypes.Add<int32>();
		OutTypes.Add<FVoxelInt32Buffer>();

		return OutTypes;
	}
	else
	{
		return FVoxelPinTypeSet::All();
	}
}

void FVoxelNode_Select::PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType)
{
	if (Pin.Name == IndexPin)
	{
		GetPin(IndexPin).SetType(NewType);

		const bool bIndexIsBuffer = GetPin(IndexPin).GetType().IsBuffer();
		const bool bResultIsBuffer = GetPin(ResultPin).GetType().IsBuffer();

		if (bIndexIsBuffer != bResultIsBuffer)
		{
			const FVoxelPinType ResultType = GetPin(ResultPin).GetType();
			if (bIndexIsBuffer)
			{
				GetPin(ResultPin).SetType(ResultType.GetBufferType());
			}
			else
			{
				GetPin(ResultPin).SetType(ResultType.GetInnerType());
			}
		}
	}
	else
	{
		GetPin(ResultPin).SetType(NewType);

		for (FVoxelPinRef& ValuePin : ValuePins)
		{
			GetPin(ValuePin).SetType(NewType);
		}

		const bool bIndexIsBuffer = GetPin(IndexPin).GetType().IsBuffer();
		const bool bResultIsBuffer = GetPin(ResultPin).GetType().IsBuffer();

		if (bIndexIsBuffer != bResultIsBuffer)
		{
			const FVoxelPinType IndexType = GetPin(IndexPin).GetType();
			GetPin(IndexPin).SetType(bResultIsBuffer ? IndexType.GetBufferType() : IndexType.GetInnerType());
		}
	}

	FixupValuePins();
}
#endif

void FVoxelNode_Select::PreSerialize()
{
	Super::PreSerialize();

	SerializedIndexType = GetPin(IndexPin).GetType();
}

void FVoxelNode_Select::PostSerialize()
{
	GetPin(IndexPin).SetType(SerializedIndexType);
	FixupValuePins();

	Super::PostSerialize();
}

void FVoxelNode_Select::FixupValuePins()
{
	for (const FVoxelPinRef& Pin : ValuePins)
	{
		RemovePin(Pin);
	}
	ValuePins.Reset();

	const FVoxelPinType IndexType = GetPin(IndexPin).GetType();

	if (IndexType.IsWildcard())
	{
		return;
	}

	if (IndexType.Is<bool>() ||
		IndexType.Is<FVoxelBoolBuffer>())
	{
		ValuePins.Add(CreateInputPin(FVoxelPinType::MakeWildcard(), "False", {}));
		ValuePins.Add(CreateInputPin(FVoxelPinType::MakeWildcard(), "True", {}));
	}
	else if (
		IndexType.Is<int32>() ||
		IndexType.Is<FVoxelInt32Buffer>())
	{
		for (int32 Index = 0; Index < NumIntegerOptions; Index++)
		{
			ValuePins.Add(CreateInputPin(
				FVoxelPinType::MakeWildcard(),
				FName("Option", Index + 1),
				VOXEL_PIN_METADATA(
					void,
					nullptr,
					DisplayName("Option " + FString::FromInt(Index)))));
		}
	}
	else
	{
		ensure(false);
	}

	for (FVoxelPinRef& ValuePin : ValuePins)
	{
		GetPin(ValuePin).SetType(GetPin(ResultPin).GetType());
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#if WITH_EDITOR
FString FVoxelNode_Select::FDefinition::GetAddPinLabel() const
{
	return "Add Option";
}

FString FVoxelNode_Select::FDefinition::GetAddPinTooltip() const
{
	return "Adds a new option to the node";
}

FString FVoxelNode_Select::FDefinition::GetRemovePinTooltip() const
{
	return "Removes last option from the node";
}

bool FVoxelNode_Select::FDefinition::CanAddInputPin() const
{
	return Node.GetPin(Node.IndexPin).GetType().GetInnerType().Is<int32>();
}

void FVoxelNode_Select::FDefinition::AddInputPin()
{
	Node.NumIntegerOptions++;
	Node.FixupValuePins();
}

bool FVoxelNode_Select::FDefinition::CanRemoveInputPin() const
{
	return
		Node.GetPin(Node.IndexPin).GetType().GetInnerType().Is<int32>() &&
		Node.NumIntegerOptions > 2;
}

void FVoxelNode_Select::FDefinition::RemoveInputPin()
{
	Node.NumIntegerOptions--;
	Node.FixupValuePins();
}
#endif