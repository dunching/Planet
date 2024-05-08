// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelFunctionNode.h"
#include "VoxelFunctionLibrary.h"
#include "VoxelBuffer.h"
#include "VoxelQueryCache.h"
#include "VoxelSourceParser.h"
#include "VoxelGraphMigration.h"

TSharedRef<FVoxelFunctionNode> FVoxelFunctionNode::Make(UFunction* InFunction)
{
	ensure(InFunction);

	const TSharedRef<FVoxelFunctionNode> Node = MakeVoxelShared<FVoxelFunctionNode>();
	Node->Function = InFunction;

	// Add empty metadata - we can't cache it since the node is created at runtime
	for (const FProperty& Property : GetFunctionProperties(InFunction))
	{
		Node->MetadataMapCache.Add(Property.GetFName());

#if WITH_EDITOR
		if (const TMap<FName, FString>* MetadataMap = Property.GetMetaDataMap())
		{
			TMap<FName, FString> Map = *MetadataMap;
			Map.Remove(STATIC_FNAME("NativeConst"));
			Map.Remove(STATIC_FNAME("DisplayName"));
			Map.Remove(STATIC_FNAME("AdvancedDisplay"));
			ensure(Map.Num() == 0);
		}
#endif
	}

	Node->FixupPins();

	return Node;
}

#if WITH_EDITOR
void FVoxelFunctionNode::SetFunction_EditorOnly(UFunction* NewFunction)
{
	ensure(NewFunction);
	ensure(!Function);
	Function = NewFunction;

	FixupPins();
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#if WITH_EDITOR
UStruct& FVoxelFunctionNode::GetMetadataContainer() const
{
	if (!ensureVoxelSlow(Function))
	{
		return Super::GetMetadataContainer();
	}

	return *Function;
}

FString FVoxelFunctionNode::GetCategory() const
{
	if (!ensure(Function))
	{
		return {};
	}

	FString Category;
	if (!Function->GetStringMetaDataHierarchical(STATIC_FNAME("Category"), &Category) ||
		Category.IsEmpty())
	{
		ensureMsgf(
			Function->GetOuterUClass()->GetStringMetaDataHierarchical(STATIC_FNAME("Category"), &Category) &&
			!Category.IsEmpty(),
			TEXT("%s is missing a Category"),
			*Function->GetName());
	}
	return Category;
}

FString FVoxelFunctionNode::GetDisplayName() const
{
	if (!Function)
	{
		return CachedName.ToString();
	}

	return FVoxelNode::GetDisplayName();
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelFunctionNode::PreCompile()
{
	if (!ensure(Function))
	{
		return;
	}

	CachedFunction = MakeVoxelShared<UVoxelFunctionLibrary::FCachedFunction>(*Function);

	ensure(CachedPins.Num() == 0);
	for (const FProperty& Property : GetFunctionProperties(Function))
	{
		const FVoxelNodeRuntime::FPinData& PinData = GetNodeRuntime().GetPinData(Property.GetFName());

		CachedPins.Add(FCachedPin
		{
			Property.GetFName(),
			IsFunctionInput(Property),
			UVoxelFunctionLibrary::MakeType(Property),
			PinData.Type,
			PinData.PinId
		});
	}
}

FVoxelComputeValue FVoxelFunctionNode::CompileCompute(const FName ReturnPinName) const
{
	if (!ensure(Function))
	{
		return {};
	}

	const FVoxelNodeRuntime::FPinData& ReturnPinData = GetNodeRuntime().GetPinData(ReturnPinName);

	return [this, ReturnPinId = ReturnPinData.PinId](const FVoxelQuery& InQuery) -> FVoxelFutureValue
	{
		const FVoxelQuery Query = InQuery.EnterScope(*this);

		checkVoxelSlow(FVoxelTaskReferencer::Get().IsReferenced(this));
		{
			FVoxelQueryCache::FEntry& Entry = Query.GetQueryCache().FindOrAddEntry(ReturnPinId);
			VOXEL_SCOPE_LOCK(Entry.CriticalSection);
			if (Entry.Value.IsValid())
			{
				return Entry.Value;
			}
		}

		using FAllocatorType = TVoxelInlineAllocator<8>;

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

		MakeVoxelTask(STATIC_FNAME("FVoxelFunctionNode"))
		.Dependencies(InputValues)
		.Execute([=]
		{
			TVoxelArray<FVoxelRuntimePinValue*, FAllocatorType> Values;
			TVoxelArray<FVoxelRuntimePinValue, FAllocatorType> InputBufferStorage;
			TVoxelArray<FVoxelRuntimePinValue, FAllocatorType> OutputStorage;

			Values.Reserve(CachedPins.Num());
			InputBufferStorage.Reserve(CachedPins.Num());
			OutputStorage.Reserve(CachedPins.Num());

			int32 InputIndex = 0;
			for (const FCachedPin& CachedPin : CachedPins)
			{
				FVoxelRuntimePinValue* Value;
				if (CachedPin.bIsInput)
				{
					const FVoxelRuntimePinValue& InputValue = InputValues[InputIndex++].GetValue_CheckCompleted();
					ensure(InputValue.GetType().CanBeCastedTo(CachedPin.PinType));

					if (CachedPin.PropertyType.IsWildcard())
					{
						Value = ConstCast(&InputValue);
					}
					else if (CachedPin.PinType.CanBeCastedTo(CachedPin.PropertyType))
					{
						Value = ConstCast(&InputValue);
					}
					else
					{
						ensure(CachedPin.PinType.GetBufferType().CanBeCastedTo(CachedPin.PropertyType));

						const TSharedRef<FVoxelBuffer> Buffer = FVoxelBuffer::Make(CachedPin.PropertyType.GetInnerType());
						Buffer->InitializeFromConstant(InputValue);
						Value = &InputBufferStorage.Add_GetRef(FVoxelRuntimePinValue::Make(Buffer, CachedPin.PropertyType));
					}
				}
				else
				{
					if (CachedPin.PropertyType.IsWildcard())
					{
						Value = &OutputStorage.Emplace_GetRef();
					}
					else
					{
						Value = &OutputStorage.Add_GetRef(FVoxelRuntimePinValue(CachedPin.PropertyType));
					}
				}

				Values.Add(Value);
			}

			UVoxelFunctionLibrary::Call(
				*this,
				*CachedFunction,
				Query,
				Values);

			int32 OutputIndex = 0;
			for (int32 Index = 0; Index < CachedPins.Num(); Index++)
			{
				const FCachedPin& CachedPin = CachedPins[Index];
				if (CachedPin.bIsInput)
				{
					continue;
				}

				FVoxelRuntimePinValue& Value = *Values[Index];

				if (!Value.GetType().CanBeCastedTo(CachedPin.PinType))
				{
					if (!ensure(Value.IsBuffer()) ||
						!ensure(Value.GetType().GetInnerType() == CachedPin.PinType))
					{
						Value = FVoxelRuntimePinValue(CachedPin.PinType);
					}
					else
					{
						const FVoxelBuffer& Buffer = Value.Get<FVoxelBuffer>();
						if (Buffer.Num() == 0 ||
							!ensure(Buffer.Num() == 1))
						{
							Value = FVoxelRuntimePinValue(CachedPin.PinType);
						}
						else
						{
							Value = Buffer.GetGenericConstant();
						}
					}
				}

				if (!ensureMsgf(Value.IsValidValue_Slow(), TEXT("Invalid value produced by %s"), *Function->GetName()))
				{
					Value = FVoxelRuntimePinValue(CachedPin.PinType);
				}

				const TSharedRef<FVoxelFutureValueStateImpl>& State = OutputStates[OutputIndex++];
				State->SetValue(Value);
			}
		});

		FVoxelQueryCache::FEntry& Entry = Query.GetQueryCache().FindOrAddEntry(ReturnPinId);
		VOXEL_SCOPE_LOCK(Entry.CriticalSection);
		ensure(Entry.Value.IsValid());
		return Entry.Value;
	};
}

void FVoxelFunctionNode::PreSerialize()
{
	Super::PreSerialize();

#if WITH_EDITOR
	MetadataMapCache.Reset();

	if (!Function)
	{
		return;
	}

	CachedName = FName(Function->GetDisplayNameText().ToString());

	for (const FProperty& Property : GetFunctionProperties(Function))
	{
		FVoxelFunctionNodeMetadataMap& Cache = MetadataMapCache.Add(Property.GetFName());
		if (const TMap<FName, FString>* MetadataMap = Property.GetMetaDataMap())
		{
			Cache.MetadataMap = *MetadataMap;
			Cache.MetadataMap.Remove(STATIC_FNAME("NativeConst"));
			Cache.MetadataMap.Remove(STATIC_FNAME("DisplayName"));
			Cache.MetadataMap.Remove(STATIC_FNAME("AdvancedDisplay"));
		}
	}
#endif
}

void FVoxelFunctionNode::PostSerialize()
{
	if (!Function)
	{
		Function = GVoxelGraphMigration->FindNewFunction(CachedName);
	}

	FixupPins();

	Super::PostSerialize();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelFunctionNode::FixupPins()
{
	VOXEL_FUNCTION_COUNTER();

	if (!ensureVoxelSlow(Function))
	{
		return;
	}

	for (const auto& It : MakeCopy(GetPinsMap()))
	{
		RemovePin(It.Key);
	}

	for (const FProperty& Property : GetFunctionProperties(Function))
	{
		FVoxelPinType Type = UVoxelFunctionLibrary::MakeType(Property);

		FVoxelPinMetadata Metadata;

		const TMap<FName, FString>* MetadataMap = nullptr;
		if (WITH_EDITOR)
		{
#if WITH_EDITOR
			MetadataMap = Property.GetMetaDataMap();
#endif
		}
		else
		{
			const FVoxelFunctionNodeMetadataMap* CachedMetadataMap = MetadataMapCache.Find(Property.GetFName());
			if (ensure(CachedMetadataMap))
			{
				MetadataMap = &CachedMetadataMap->MetadataMap;
			}
		}

		bool bNotTemplate = false;
		if (MetadataMap)
		{
			for (auto& It : *MetadataMap)
			{
				const FName Name = It.Key;

#if WITH_EDITOR
				if (Name == STATIC_FNAME("NativeConst"))
				{
					continue;
				}
				if (Name == STATIC_FNAME("DisplayName"))
				{
					ensure(Metadata.DisplayName.IsEmpty());
					Metadata.DisplayName = It.Value;
					continue;
				}
				if (Name == STATIC_FNAME("AdvancedDisplay"))
				{
					ensure(Metadata.Category.IsEmpty());
					Metadata.Category = "Advanced";
					continue;
				}
#endif

				if (Name == STATIC_FNAME("VirtualPin"))
				{
					ensure(!Metadata.bVirtualPin);
					Metadata.bVirtualPin = true;
				}
				else if (Name == STATIC_FNAME("DisplayLast"))
				{
					ensure(!Metadata.bDisplayLast);
					Metadata.bDisplayLast = true;
				}
				else if (Name == STATIC_FNAME("NoDefault"))
				{
					ensure(!Metadata.bNoDefault);
					Metadata.bNoDefault = true;
				}
				else if (Name == STATIC_FNAME("ShowInDetail"))
				{
					ensure(!Metadata.bShowInDetail);
					Metadata.bShowInDetail = true;
				}
				else if (Name == STATIC_FNAME("NotTemplate"))
				{
					ensure(!bNotTemplate);
					bNotTemplate = true;
					ensure(Type.IsBuffer());
				}
				else
				{
					ensure(false);
				}
			}
		}

#if WITH_EDITOR
		if (Function->GetMetaData("AdvancedDisplay").Contains(Property.GetName()))
		{
			ensure(Metadata.Category.IsEmpty());
			Metadata.Category = "Advanced";
		}

		Metadata.Tooltip = FVoxelObjectUtilities::GetPropertyTooltip(*Function, Property);

		if (IsFunctionInput(Property) &&
			!Type.IsWildcard())
		{
			Metadata.DefaultValue = GVoxelSourceParser->GetPropertyDefault(Function, Property.GetFName());

			if (!Metadata.DefaultValue.IsEmpty())
			{
				// Sanitize the default value
				// Required for namespaced enums
				FVoxelPinValue Value(Type.GetPinDefaultValueType());

				if (Value.Is<FName>())
				{
					ensure(Metadata.DefaultValue.RemoveFromStart("\""));
					ensure(Metadata.DefaultValue.RemoveFromEnd("\""));
				}

				ensure(Value.ImportFromString(Metadata.DefaultValue));
				Metadata.DefaultValue = Value.ExportToString();
			}
		}
#endif

		EVoxelPinFlags Flags = EVoxelPinFlags::None;
		if (Type.IsBuffer() && !bNotTemplate)
		{
			Flags |= EVoxelPinFlags::TemplatePin;
		}

		CreatePin(
			Type,
			IsFunctionInput(Property),
			Property.GetFName(),
			Metadata,
			Flags);
	}
}