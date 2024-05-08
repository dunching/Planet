// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelNode.h"
#include "VoxelExecNode.h"
#include "VoxelQueryCache.h"
#include "VoxelTemplateNode.h"
#include "VoxelSourceParser.h"
#include "VoxelGraph.h"
#include "VoxelGraphExecutor.h"
#include "VoxelCompiledGraph.h"

DEFINE_UNIQUE_VOXEL_ID(FVoxelPinRuntimeId);
DEFINE_VOXEL_INSTANCE_COUNTER(FVoxelNodeRuntime);

TMap<FName, FVoxelNodeComputePtr> GVoxelNodeStaticComputes;

void RegisterVoxelNodeComputePtr(
	const UScriptStruct* Node,
	const FName PinName,
	const FVoxelNodeComputePtr Ptr)
{
	VOXEL_FUNCTION_COUNTER();

	static const TArray<UScriptStruct*> NodeStructs = GetDerivedStructs<FVoxelNode>();

	for (const UScriptStruct* ChildNode : NodeStructs)
	{
		if (!ChildNode->IsChildOf(Node))
		{
			continue;
		}

		const FName Name(ChildNode->GetStructCPPName() + "." + PinName.ToString());

		ensure(!GVoxelNodeStaticComputes.Contains(Name));
		GVoxelNodeStaticComputes.Add(Name, Ptr);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#if WITH_EDITOR
VOXEL_RUN_ON_STARTUP_GAME(FixupVoxelNodes)
{
	VOXEL_FUNCTION_COUNTER();

	for (UScriptStruct* Struct : GetDerivedStructs<FVoxelNode>())
	{
		if (Struct->HasMetaData(STATIC_FNAME("Abstract")) ||
			Struct->HasMetaData(STATIC_FNAME("Internal")))
		{
			continue;
		}

		if (Struct->GetPathName().StartsWith(TEXT("/Script/Voxel")))
		{
			const UStruct* Parent = Struct;
			while (Parent->GetSuperStruct() != FVoxelNode::StaticStruct())
			{
				Parent = Parent->GetSuperStruct();
			}

			if (Parent == FVoxelTemplateNode::StaticStruct())
			{
				ensure(Struct->GetStructCPPName().StartsWith("FVoxelTemplateNode_"));
			}
			else if (Parent == FVoxelExecNode::StaticStruct())
			{
				ensure(Struct->HasMetaData("DisplayName") || Struct->HasMetaData("Autocast"));
				ensure(Struct->GetStructCPPName().StartsWith("FVoxel"));
				ensure(Struct->GetStructCPPName().EndsWith("ExecNode"));
			}
			else
			{
				ensure(Struct->GetStructCPPName().StartsWith("FVoxelNode_"));
			}
		}

		TArray<FString> Array;
		Struct->GetName().ParseIntoArray(Array, TEXT("_"));

		const FString DefaultName = FName::NameToDisplayString(Struct->GetName(), false);
		const FString FixedName = FName::NameToDisplayString(Array.Last(), false);

		if (Struct->GetDisplayNameText().ToString() == DefaultName)
		{
			Struct->SetMetaData("DisplayName", *FixedName);
		}

		FString Tooltip = Struct->GetToolTipText().ToString();
		if (Tooltip.RemoveFromStart("Voxel Node "))
		{
			Struct->SetMetaData("Tooltip", *Tooltip);
		}
	}
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelFutureValue FVoxelNodeRuntime::Get(const FVoxelPinRef& Pin, const FVoxelQuery& Query) const
{
	const FPinData& PinData = GetPinData(Pin);
	checkVoxelSlow(PinData.bIsInput);
	ensureVoxelSlow(!PinData.Metadata.bConstantPin);

	FVoxelFutureValue Value = (*PinData.Compute)(Query);
	checkVoxelSlow(Value.IsValid());
	checkVoxelSlow(
		PinData.Type.CanBeCastedTo(Value.GetParentType()) ||
		Value.GetParentType().CanBeCastedTo(PinData.Type));
	return Value;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedRef<const FVoxelComputeValue> FVoxelNodeRuntime::GetCompute(
	const FVoxelPinRef& Pin,
	const TSharedRef<FVoxelQueryContext>& Context) const
{
	const TSharedPtr<FPinData> PinData = PinDatas.FindRef(Pin);
	checkVoxelSlow(PinData);
	ensureVoxelSlow(!PinData->Metadata.bConstantPin);

	return MakeVoxelShared<FVoxelComputeValue>([Context, Compute = PinData->Compute.ToSharedRef()](const FVoxelQuery& InQuery)
	{
		const FVoxelQuery NewQuery = InQuery.MakeNewQuery(Context);
		const FVoxelQueryScope Scope(NewQuery);
		return (*Compute)(NewQuery);
	});
}

FVoxelDynamicValueFactory FVoxelNodeRuntime::MakeDynamicValueFactory(const FVoxelPinRef& Pin) const
{
	const TSharedPtr<FPinData> PinData = PinDatas.FindRef(Pin);
	checkVoxelSlow(PinData);
	ensureVoxelSlow(!PinData->Metadata.bConstantPin);
	return FVoxelDynamicValueFactory(PinData->Compute.ToSharedRef(), PinData->Type, PinData->StatName);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelPinRuntimeId GetVoxelPinRuntimeId(const FVoxelGraphPinRef& PinRef)
{
	static FVoxelFastCriticalSection CriticalSection;
	VOXEL_SCOPE_LOCK(CriticalSection);

	static TMap<FVoxelGraphPinRef, FVoxelPinRuntimeId> PinRefToId;

	FVoxelPinRuntimeId& Id = PinRefToId.FindOrAdd(PinRef);
	if (!Id.IsValid())
	{
		Id = FVoxelPinRuntimeId::New();
	}
	return Id;
}

FVoxelNodeRuntime::FPinData::FPinData(
	const FVoxelPinType& Type,
	const bool bIsInput,
	const FName StatName,
	const FVoxelGraphPinRef& PinRef,
	const FVoxelPinMetadataFlags& Metadata)
	: Type(Type)
	, bIsInput(bIsInput)
	, StatName(StatName)
	, PinId(GetVoxelPinRuntimeId(PinRef))
	, Metadata(Metadata)
{
	ensure(!Type.IsWildcard());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DEFINE_VOXEL_INSTANCE_COUNTER(FVoxelNode);
DEFINE_VOXEL_MEMORY_STAT(STAT_VoxelNodes);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelNode& FVoxelNode::operator=(const FVoxelNode& Other)
{
	VOXEL_SCOPE_COUNTER_FORMAT("FVoxelNode::operator= %s", *GetStruct()->GetName());

	check(GetStruct() == Other.GetStruct());
	ensure(!NodeRuntime);
	ensure(!Other.NodeRuntime);

#if WITH_EDITOR
	ExposedPins = Other.ExposedPins;
	ExposedPinValues = Other.ExposedPinValues;
#endif

	FlushDeferredPins();
	Other.FlushDeferredPins();

	InternalPinBackups.Reset();
	InternalPins.Reset();
	InternalPinArrays.Reset();
	InternalPinsOrder.Reset();

	SortOrderCounter = Other.SortOrderCounter;

	TArray<FDeferredPin> Pins;
	Other.InternalPinBackups.GenerateValueArray(Pins);

	// Register arrays first
	Pins.Sort([](const FDeferredPin& A, const FDeferredPin& B)
	{
		return A.ArrayOwner.IsNone() > B.ArrayOwner.IsNone();
	});

	for (const FDeferredPin& Pin : Pins)
	{
		RegisterPin(Pin, false);
	}

	InternalPinsOrder.Append(Other.InternalPinsOrder);

	LoadSerializedData(Other.GetSerializedData());
	UpdateStats();

	return *this;
}

int64 FVoxelNode::GetAllocatedSize() const
{
	int64 AllocatedSize = GetStruct()->GetStructureSize();

	AllocatedSize += DeferredPins.GetAllocatedSize();
	AllocatedSize += InternalPinBackups.GetAllocatedSize();
	AllocatedSize += InternalPinsOrder.GetAllocatedSize();
	AllocatedSize += InternalPins.GetAllocatedSize();
	AllocatedSize += InternalPins.Num() * sizeof(FVoxelPin);
	AllocatedSize += InternalPinArrays.GetAllocatedSize();
#if WITH_EDITOR
	AllocatedSize += ExposedPinValues.GetAllocatedSize();
	AllocatedSize += ExposedPins.GetAllocatedSize();
#endif
	AllocatedSize += ReturnToPoolFuncs.GetAllocatedSize();

	for (auto& It : InternalPinArrays)
	{
		AllocatedSize += It.Value->Pins.GetAllocatedSize();
	}

	return AllocatedSize;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#if WITH_EDITOR
UStruct& FVoxelNode::GetMetadataContainer() const
{
	return *GetStruct();
}

FString FVoxelNode::GetCategory() const
{
	FString Category;
	ensureMsgf(
		GetMetadataContainer().GetStringMetaDataHierarchical(STATIC_FNAME("Category"), &Category) &&
		!Category.IsEmpty(),
		TEXT("%s is missing a Category"),
		*GetStruct()->GetStructCPPName());
	return Category;
}

FString FVoxelNode::GetDisplayName() const
{
	if (GetMetadataContainer().HasMetaData(STATIC_FNAME("Autocast")) &&
		ensure(InternalPinsOrder.Num() > 0) &&
		ensure(FindPin(InternalPinsOrder[0])))
	{
		ensure(!GetMetadataContainer().HasMetaData(STATIC_FNAME("DisplayName")));

		const FString FromType = FindPin(InternalPinsOrder[0])->GetType().GetInnerType().ToString();
		const FString ToType = GetUniqueOutputPin().GetType().GetInnerType().ToString();
		{
			FString ExpectedName = FromType + "To" + ToType;
			ExpectedName.ReplaceInline(TEXT(" "), TEXT(""));
			ensure(GetMetadataContainer().GetName() == ExpectedName);
		}
		return "To " + ToType + " (" + FromType + ")";
	}

	return GetMetadataContainer().GetDisplayNameText().ToString();
}

FString FVoxelNode::GetTooltip() const
{
	if (GetMetadataContainer().HasMetaData(STATIC_FNAME("Autocast")) &&
		ensure(InternalPinsOrder.Num() > 0) &&
		ensure(FindPin(InternalPinsOrder[0])))
	{
		ensure(!GetMetadataContainer().HasMetaData(STATIC_FNAME("Tooltip")));
		ensure(!GetMetadataContainer().HasMetaData(STATIC_FNAME("ShortTooltip")));

		const FString FromType = FindPin(InternalPinsOrder[0])->GetType().GetInnerType().ToString();
		const FString ToType = GetUniqueOutputPin().GetType().GetInnerType().ToString();

		return "Cast from " + FromType + " to " + ToType;
	}

	return GetMetadataContainer().GetToolTipText().ToString();
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelNode::ReturnToPool()
{
	for (const auto& It : NodeRuntime->PinDatas)
	{
		if (It.Value->bIsInput)
		{
			It.Value->Compute = nullptr;
		}
	}

	for (const FReturnToPoolFunc& ReturnToPoolFunc : ReturnToPoolFuncs)
	{
		(this->*ReturnToPoolFunc)();
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelComputeValue FVoxelNode::CompileCompute(const FName PinName) const
{
	const FName Name = FName(GetStruct()->GetStructCPPName() + "." + PinName.ToString());
	if (!GVoxelNodeStaticComputes.Contains(Name))
	{
		return nullptr;
	}

	const FVoxelNodeComputePtr Ptr = GVoxelNodeStaticComputes.FindChecked(Name);
	const FVoxelNodeRuntime::FPinData& PinData = GetNodeRuntime().GetPinData(PinName);

	if (!ensure(!PinData.Type.IsWildcard()))
	{
		return {};
	}

	return [this, Ptr, Type = PinData.Type, PinId = PinData.PinId](const FVoxelQuery& Query) -> FVoxelFutureValue
	{
		checkVoxelSlow(FVoxelTaskReferencer::Get().IsReferenced(this));
		ON_SCOPE_EXIT
		{
			checkVoxelSlow(FVoxelTaskReferencer::Get().IsReferenced(this));
		};

		// No caching for call nodes
		if (GetNodeRuntime().IsCallNode())
		{
			return
				MakeVoxelTask()
				.Execute(Type, [this, Ptr, Query]
				{
					return (*Ptr)(*this, Query.EnterScope(*this));
				});
		}

		FVoxelQueryCache::FEntry& Entry = Query.GetQueryCache().FindOrAddEntry(PinId);

		VOXEL_SCOPE_LOCK(Entry.CriticalSection);

		if (!Entry.Value.IsValid())
		{
			// Always wrap in a task to sanitize the values,
			// otherwise errors propagate to other nodes and are impossible to track down
			Entry.Value =
				MakeVoxelTask()
				.Execute(Type, [this, Ptr, Query]
				{
					return (*Ptr)(*this, Query.EnterScope(*this));
				});
		}
		return Entry.Value;
	};
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

uint32 FVoxelNode::GetNodeHash() const
{
	ensure(!NodeRuntime.IsValid());

	// Ensure properties are up to date
	ConstCast(this)->PreSerialize();

	uint64 Hash = FVoxelUtilities::MurmurHash(GetStruct());
	int32 Index = 0;
	for (const FProperty& Property : GetStructProperties(StaticStruct()))
	{
		const uint32 PropertyHash = FVoxelObjectUtilities::HashProperty(Property, Property.ContainerPtrToValuePtr<void>(this));
		Hash ^= FVoxelUtilities::MurmurHash(PropertyHash, Index++);
	}
	return Hash;
}

bool FVoxelNode::IsNodeIdentical(const FVoxelNode& Other) const
{
	if (GetStruct() != Other.GetStruct())
	{
		return false;
	}

	// Ensure properties are up to date
	// (needed for pin arrays)
	ConstCast(this)->PreSerialize();
	ConstCast(Other).PreSerialize();

	return GetStruct()->CompareScriptStruct(this, &Other, PPF_None);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelNode::PreSerialize()
{
	Super::PreSerialize();

	Version = FVersion::LatestVersion;

	SerializedDataProperty = GetSerializedData();
}

void FVoxelNode::PostSerialize()
{
	Super::PostSerialize();

	if (Version < FVersion::AddIsValid)
	{
		ensure(!SerializedDataProperty.bIsValid);
		SerializedDataProperty.bIsValid = true;
	}
	ensure(SerializedDataProperty.bIsValid);

	LoadSerializedData(SerializedDataProperty);
	SerializedDataProperty = {};

	UpdateStats();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#if WITH_EDITOR
TSharedRef<FVoxelNodeDefinition> FVoxelNode::GetNodeDefinition()
{
	return MakeVoxelShared<FVoxelNodeDefinition>(*this);
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#if WITH_EDITOR
FVoxelPinTypeSet FVoxelNode::GetPromotionTypes(const FVoxelPin& Pin) const
{
	if (!ensure(Pin.IsPromotable()))
	{
		return {};
	}

	if (EnumHasAllFlags(Pin.Flags, EVoxelPinFlags::TemplatePin))
	{
		// GetPromotionTypes should be overridden by child nodes
		ensure(!Pin.GetType().IsWildcard());

		FVoxelPinTypeSet Types;
		Types.Add(Pin.GetType().GetInnerType());
		Types.Add(Pin.GetType().GetBufferType());
		return Types;
	}

	ensure(Pin.BaseType.IsWildcard());
	return FVoxelPinTypeSet::All();
}

void FVoxelNode::PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType)
{
	if (!ensure(Pin.IsPromotable()) ||
		!ensure(GetPromotionTypes(Pin).Contains(NewType)))
	{
		return;
	}

	if (EnumHasAllFlags(Pin.Flags, EVoxelPinFlags::TemplatePin))
	{
		ensure(NewType.GetInnerType() == Pin.GetType().GetInnerType());

		for (FVoxelPin& OtherPin : GetPins())
		{
			if (!EnumHasAllFlags(OtherPin.Flags, EVoxelPinFlags::TemplatePin))
			{
				continue;
			}

			if (NewType.IsBuffer())
			{
				OtherPin.SetType(OtherPin.GetType().GetBufferType());
			}
			else
			{
				OtherPin.SetType(OtherPin.GetType().GetInnerType());
			}
		}
		return;
	}

	ensure(Pin.BaseType.IsWildcard());
	Pin.SetType(NewType);
}
#endif

void FVoxelNode::PromotePin_Runtime(FVoxelPin& Pin, const FVoxelPinType& NewType)
{
	// For convenience
	if (Pin.GetType() == NewType)
	{
		return;
	}

#if WITH_EDITOR
	ensure(GetPromotionTypes(Pin).Contains(NewType));
#endif

	if (!ensure(Pin.IsPromotable()))
	{
		return;
	}

	// If this fails, PromotePin_Runtime needs to be overriden
	ensure(Pin.GetType().IsWildcard() || EnumHasAllFlags(Pin.Flags, EVoxelPinFlags::TemplatePin));

	if (Pin.GetType().IsWildcard())
	{
		Pin.SetType(NewType);
	}

	if (!EnumHasAllFlags(Pin.Flags, EVoxelPinFlags::TemplatePin))
	{
		return;
	}

	ensure(NewType.GetInnerType() == Pin.GetType().GetInnerType());

	for (FVoxelPin& OtherPin : GetPins())
	{
		if (!EnumHasAllFlags(OtherPin.Flags, EVoxelPinFlags::TemplatePin))
		{
			continue;
		}

		if (NewType.IsBuffer())
		{
			OtherPin.SetType(OtherPin.GetType().GetBufferType());
		}
		else
		{
			OtherPin.SetType(OtherPin.GetType().GetInnerType());
		}
	}
}

TOptional<bool> FVoxelNode::AreTemplatePinsBuffersImpl() const
{
	VOXEL_FUNCTION_COUNTER();

	TOptional<bool> Result;
	for (const FVoxelPin& Pin : GetPins())
	{
		if (!EnumHasAllFlags(Pin.Flags, EVoxelPinFlags::TemplatePin))
		{
			continue;
		}

		if (!Result)
		{
			Result = Pin.GetType().IsBuffer();
		}
		else
		{
			ensure(*Result == Pin.GetType().IsBuffer());
		}
	}
	return Result;
}

#if WITH_EDITOR
bool FVoxelNode::IsPinHidden(const FVoxelPin& Pin) const
{
	if (!Pin.Metadata.bShowInDetail)
	{
		return false;
	}

	return !ExposedPins.Contains(Pin.Name);
}

FString FVoxelNode::GetPinDefaultValue(const FVoxelPin& Pin) const
{
	if (!Pin.Metadata.bShowInDetail)
	{
		return Pin.Metadata.DefaultValue;
	}

	const FVoxelNodeExposedPinValue* ExposedPinValue = ExposedPinValues.FindByKey(Pin.Name);
	if (!ensure(ExposedPinValue))
	{
		return Pin.Metadata.DefaultValue;
	}
	return ExposedPinValue->Value.ExportToString();
}

void FVoxelNode::UpdatePropertyBoundDefaultValue(const FVoxelPin& Pin, const FVoxelPinValue& NewValue)
{
	if (!Pin.Metadata.bShowInDetail)
	{
		return;
	}

	if (FVoxelNodeExposedPinValue* PinValue = ExposedPinValues.FindByKey(Pin.Name))
	{
		if (PinValue->Value != NewValue)
		{
			PinValue->Value = NewValue;
			OnExposedPinsUpdated.Broadcast();
		}
	}
	else
	{
		ExposedPinValues.Add({ Pin.Name, NewValue });
		OnExposedPinsUpdated.Broadcast();
	}
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelPin& FVoxelNode::GetUniqueInputPin()
{
	FVoxelPin* InputPin = nullptr;
	for (FVoxelPin& Pin : GetPins())
	{
		if (Pin.bIsInput)
		{
			check(!InputPin);
			InputPin = &Pin;
		}
	}
	check(InputPin);

	return *InputPin;
}

FVoxelPin& FVoxelNode::GetUniqueOutputPin()
{
	FVoxelPin* OutputPin = nullptr;
	for (FVoxelPin& Pin : GetPins())
	{
		if (!Pin.bIsInput)
		{
			check(!OutputPin);
			OutputPin = &Pin;
		}
	}
	check(OutputPin);

	return *OutputPin;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FName FVoxelNode::AddPinToArray(const FName ArrayName, FName PinName)
{
	FlushDeferredPins();

	const TSharedPtr<FPinArray> PinArray = InternalPinArrays.FindRef(ArrayName);
	if (!ensure(PinArray))
	{
		return {};
	}

	if (PinName.IsNone())
	{
		PinName = ArrayName + "_0";

		while (InternalPins.Contains(PinName) || InternalPinArrays.Contains(PinName))
		{
			PinName.SetNumber(PinName.GetNumber() + 1);
		}
	}

	if (!ensure(!InternalPins.Contains(PinName)) ||
		!ensure(!InternalPinArrays.Contains(PinName)))
	{
		return {};
	}

	FDeferredPin Pin = PinArray->PinTemplate;
	Pin.Name = PinName;
	RegisterPin(Pin);

	FixupArrayNames(ArrayName);

	return PinName;
}

FName FVoxelNode::InsertPinToArrayPosition(FName ArrayName, int32 Position)
{
	FlushDeferredPins();

	const TSharedPtr<FPinArray> PinArray = InternalPinArrays.FindRef(ArrayName);
	if (!ensure(PinArray))
	{
		return {};
	}

	FName PinName = ArrayName + "_0";
	while (InternalPins.Contains(PinName) || InternalPinArrays.Contains(PinName))
	{
		PinName.SetNumber(PinName.GetNumber() + 1);
	}

	if (!ensure(!InternalPins.Contains(PinName)) ||
		!ensure(!InternalPinArrays.Contains(PinName)))
	{
		return {};
	}

	FDeferredPin Pin = PinArray->PinTemplate;
	Pin.Name = PinName;
	RegisterPin(Pin);
	SortPins();

	for (int32 Index = Position; Index < PinArray->Pins.Num() - 1; Index++)
	{
		PinArray->Pins.Swap(Index, PinArray->Pins.Num() - 1);
	}

	FixupArrayNames(ArrayName);

	return PinName;
}

void FVoxelNode::FixupArrayNames(FName ArrayName)
{
	FlushDeferredPins();

	const TSharedPtr<FPinArray> PinArray = InternalPinArrays.FindRef(ArrayName);
	if (!ensure(PinArray))
	{
		return;
	}

	for (int32 Index = 0; Index < PinArray->Pins.Num(); Index++)
	{
		const FName ArrayPinName = PinArray->Pins[Index];
		const TSharedPtr<FVoxelPin> ArrayPin = InternalPins.FindRef(ArrayPinName);
		if (!ensure(ArrayPin))
		{
			continue;
		}

		ConstCast(ArrayPin->SortOrder) = PinArray->PinTemplate.SortOrder + Index / 100.f;

#if WITH_EDITOR
		ConstCast(ArrayPin->Metadata.DisplayName) = FName::NameToDisplayString(ArrayName.ToString(), false) + " " + FString::FromInt(Index);
#endif
	}

	SortPins();
}

FName FVoxelNode::CreatePin(
	const FVoxelPinType& Type,
	const bool bIsInput,
	const FName Name,
	const FVoxelPinMetadata& Metadata,
	const EVoxelPinFlags Flags,
	const int32 MinArrayNum)
{
	if (bIsInput)
	{
#if WITH_EDITOR
		ensure(
			Metadata.DefaultValue.IsEmpty() ||
			Type.IsWildcard() ||
			FVoxelPinValue(Type.GetPinDefaultValueType()).ImportFromString(Metadata.DefaultValue));
#endif

		// Use Internal_GetStruct as we might be in a struct constructor
		if (Internal_GetStruct()->IsChildOf(StaticStructFast<FVoxelExecNode>()))
		{
			ensure(Metadata.bVirtualPin);
		}
	}
	else
	{
#if WITH_EDITOR
		ensure(Metadata.DefaultValue.IsEmpty());
#endif

		ensure(!Metadata.bArrayPin);
		ensure(!Metadata.bVirtualPin);
		ensure(!Metadata.bConstantPin);
		ensure(!Metadata.bDisplayLast);
		ensure(!Metadata.bNoDefault);
		ensure(!Metadata.bShowInDetail);
	}

	FVoxelPinType BaseType = Type;
	if (Metadata.BaseClass)
	{
		ensure(BaseType.GetInnerType().Is<TSubclassOf<UObject>>());
		BaseType = FVoxelPinType::MakeClass(Metadata.BaseClass);

		if (Type.IsBuffer())
		{
			BaseType = BaseType.GetBufferType();
		}
	}
	if (Metadata.bArrayPin)
	{
		BaseType = BaseType.WithBufferArray(true);
	}

	const FVoxelPinType ChildType = BaseType;

	if (EnumHasAnyFlags(Flags, EVoxelPinFlags::TemplatePin))
	{
		BaseType = FVoxelPinType::MakeWildcard();
	}

	RegisterPin(FDeferredPin
	{
		{},
		MinArrayNum,
		Name,
		bIsInput,
		0,
		Flags,
		BaseType,
		ChildType,
		Metadata
	});

	if (Metadata.bDisplayLast)
	{
		InternalPinsOrder.Add(Name);
		DisplayLastPins++;
	}
	else
	{
		InternalPinsOrder.Add(Name);
		if (DisplayLastPins > 0)
		{
			for (int32 Index = InternalPinsOrder.Num() - DisplayLastPins - 1; Index < InternalPinsOrder.Num() - 1; Index++)
			{
				InternalPinsOrder.Swap(Index, InternalPinsOrder.Num() - 1);
			}
		}
	}

	return Name;
}

void FVoxelNode::RemovePin(const FName Name)
{
	FlushDeferredPins();

	const TSharedPtr<FVoxelPin> Pin = FindPin(Name);
	if (ensure(Pin) &&
		!Pin->ArrayOwner.IsNone() &&
		ensure(InternalPinArrays.Contains(Pin->ArrayOwner)))
	{
		ensure(InternalPinArrays[Pin->ArrayOwner]->Pins.Remove(Name));
	}

	ensure(InternalPinBackups.Remove(Name));
	ensure(InternalPins.Remove(Name));
	InternalPinsOrder.Remove(Name);

#if WITH_EDITOR
	if (Pin->Metadata.bShowInDetail)
	{
		OnExposedPinsUpdated.Broadcast();
	}
#endif
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelNode::FlushDeferredPinsImpl()
{
	ensure(bIsDeferringPins);
	bIsDeferringPins = false;

	for (const FDeferredPin& Pin : DeferredPins)
	{
		RegisterPin(Pin);
	}
	DeferredPins.Empty();
}

void FVoxelNode::RegisterPin(FDeferredPin Pin, const bool bApplyMinNum)
{
	VOXEL_FUNCTION_COUNTER();

	ensure(!Pin.Name.IsNone());
	ensure(Pin.BaseType.IsValid());

	if (Pin.ArrayOwner.IsNone() &&
		Pin.SortOrder == 0)
	{
		Pin.SortOrder = SortOrderCounter++;
	}

	if (Pin.Metadata.bDisplayLast)
	{
		Pin.SortOrder += 10000.f;
	}

	if (bIsDeferringPins)
	{
		DeferredPins.Add(Pin);
		return;
	}

	ensure(!InternalPinBackups.Contains(Pin.Name));
	InternalPinBackups.Add(Pin.Name, Pin);

	const FString PinName = Pin.Name.ToString();

#if WITH_EDITOR
	if (!Pin.Metadata.Tooltip.IsSet())
	{
		Pin.Metadata.Tooltip = MakeAttributeLambda([Struct = GetStruct(), Name = Pin.ArrayOwner.IsNone() ? Pin.Name : Pin.ArrayOwner]
		{
			return GVoxelSourceParser->GetPinTooltip(Struct, Name);
		});
	}

	if (Pin.Metadata.DisplayName.IsEmpty())
	{
		Pin.Metadata.DisplayName = FName::NameToDisplayString(PinName, PinName.StartsWith("b", ESearchCase::CaseSensitive));
		Pin.Metadata.DisplayName.RemoveFromStart("Out ");
	}

	if (Pin.Metadata.bShowInDetail)
	{
		OnExposedPinsUpdated.Broadcast();

		if (!ExposedPinValues.FindByKey(Pin.Name))
		{
			FVoxelNodeExposedPinValue ExposedPinValue;
			ExposedPinValue.Name = Pin.Name;
			ExposedPinValue.Value = FVoxelPinValue(Pin.ChildType.GetPinDefaultValueType());

			if (!Pin.Metadata.DefaultValue.IsEmpty())
			{
				ensure(ExposedPinValue.Value.ImportFromString(Pin.Metadata.DefaultValue));
			}

			ExposedPinValues.Add(ExposedPinValue);
		}
	}
#endif

	if (EnumHasAnyFlags(Pin.Flags, EVoxelPinFlags::ArrayPin))
	{
		ensure(Pin.ArrayOwner.IsNone());

		FDeferredPin PinTemplate = Pin;
		PinTemplate.ArrayOwner = Pin.Name;
		EnumRemoveFlags(PinTemplate.Flags, EVoxelPinFlags::ArrayPin);

		ensure(!InternalPinArrays.Contains(Pin.Name));
		InternalPinArrays.Add(Pin.Name, MakeVoxelShared<FPinArray>(PinTemplate));

		if (bApplyMinNum)
		{
			for (int32 Index = 0; Index < Pin.MinArrayNum; Index++)
			{
				AddPinToArray(Pin.Name);
			}
		}
	}
	else
	{
		if (!Pin.ArrayOwner.IsNone() &&
			ensure(InternalPinArrays.Contains(Pin.ArrayOwner)))
		{
			FPinArray& PinArray = *InternalPinArrays[Pin.ArrayOwner];
#if WITH_EDITOR
			Pin.Metadata.Category = Pin.ArrayOwner.ToString();
#endif
			PinArray.Pins.Add(Pin.Name);
		}

		ensure(!InternalPins.Contains(Pin.Name));
		InternalPins.Add(Pin.Name, MakeSharedCopy(FVoxelPin(
			Pin.Name,
			Pin.bIsInput,
			Pin.SortOrder,
			Pin.ArrayOwner,
			Pin.Flags,
			Pin.BaseType,
			Pin.ChildType,
			Pin.Metadata)));
	}

	SortPins();
}

void FVoxelNode::SortPins()
{
	InternalPins.ValueSort([](const TSharedPtr<FVoxelPin>& A, const TSharedPtr<FVoxelPin>& B)
	{
		if (A->bIsInput != B->bIsInput)
		{
			return A->bIsInput > B->bIsInput;
		}

		return A->SortOrder < B->SortOrder;
	});
}

void FVoxelNode::SortArrayPins(FName PinArrayName)
{
	VOXEL_FUNCTION_COUNTER();

	const TSharedPtr<FPinArray> PinArray = InternalPinArrays.FindRef(PinArrayName);
	if (!PinArray)
	{
		return;
	}

	TArray<int32> SortOrders;
	TArray<TSharedPtr<FVoxelPin>> AffectedPins;
	for (const FName PinName : PinArray->Pins)
	{
		if (TSharedPtr<FVoxelPin> Pin = InternalPins.FindRef(PinName))
		{
			SortOrders.Add(Pin->SortOrder);
		}
	}

	SortOrders.Sort();
	for (int32 Index = 0; Index < AffectedPins.Num(); Index++)
	{
		ConstCast(AffectedPins[Index]->SortOrder) = SortOrders[Index];
	}

	SortPins();
}

FVoxelNodeSerializedData FVoxelNode::GetSerializedData() const
{
	FlushDeferredPins();

	FVoxelNodeSerializedData SerializedData;
	SerializedData.bIsValid = true;

	for (const auto& It : InternalPins)
	{
		const FVoxelPin& Pin = *It.Value;
		if (!Pin.IsPromotable())
		{
			continue;
		}

		SerializedData.PinTypes.Add(Pin.Name, Pin.GetType());
	}

	for (const auto& It : InternalPinArrays)
	{
		FVoxelNodeSerializedArrayData ArrayData;
		ArrayData.PinNames = It.Value->Pins;
		SerializedData.ArrayDatas.Add(It.Key, ArrayData);
	}

#if WITH_EDITOR
	SerializedData.ExposedPins = ExposedPins;
	SerializedData.ExposedPinsValues = ExposedPinValues;
#endif

	return SerializedData;
}

void FVoxelNode::LoadSerializedData(const FVoxelNodeSerializedData& SerializedData)
{
	VOXEL_FUNCTION_COUNTER();
	ensure(SerializedData.bIsValid);

#if WITH_EDITOR
	ExposedPins = SerializedData.ExposedPins;
	ExposedPinValues = SerializedData.ExposedPinsValues;
#endif

	FlushDeferredPins();

	for (const auto& It : InternalPinArrays)
	{
		FPinArray& PinArray = *It.Value;

		const TVoxelArray<FName> Pins = PinArray.Pins;
		for (const FName Name : Pins)
		{
			RemovePin(Name);
		}
		ensure(PinArray.Pins.Num() == 0);
	}

	for (const auto& It : SerializedData.ArrayDatas)
	{
		const TSharedPtr<FPinArray> PinArray = InternalPinArrays.FindRef(It.Key);
		if (!ensure(PinArray))
		{
			continue;
		}

		for (const FName Name : It.Value.PinNames)
		{
			AddPinToArray(It.Key, Name);
		}
	}

	// Ensure MinNum is applied
	for (const auto& It : InternalPinArrays)
	{
		FPinArray& PinArray = *It.Value;

		for (int32 Index = PinArray.Pins.Num(); Index < PinArray.PinTemplate.MinArrayNum; Index++)
		{
			AddPinToArray(It.Key);
		}
	}

	for (const auto& It : SerializedData.PinTypes)
	{
		const TSharedPtr<FVoxelPin> Pin = InternalPins.FindRef(It.Key);
		if (!Pin ||
			!Pin->IsPromotable())
		{
			continue;
		}

#if WITH_EDITOR
		if (!It.Value.IsWildcard() &&
			!ensureVoxelSlow(GetPromotionTypes(*Pin).Contains(It.Value)))
		{
			continue;
		}
#endif

		Pin->SetType(It.Value);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelNode::InitializeNodeRuntime(
	const FVoxelGraphNodeRef& NodeRef,
	const bool bIsCallNode)
{
	VOXEL_FUNCTION_COUNTER();
	ensure(NodeRef.Graph.IsValid() || bIsCallNode);
	ensure(!NodeRef.NodeId.IsNone() || bIsCallNode);

	FlushDeferredPins();

	FString DebugName = GetStruct()->GetName();
	DebugName.RemoveFromStart("VoxelNode_");
	DebugName.RemoveFromStart("VoxelMathNode_");
	DebugName.RemoveFromStart("VoxelTemplateNode_");

	check(!NodeRuntime);
	NodeRuntime = MakeVoxelShared<FVoxelNodeRuntime>();
	NodeRuntime->Node = this;
	NodeRuntime->NodeRef = NodeRef;
	NodeRuntime->AreTemplatePinsBuffers = AreTemplatePinsBuffersImpl();
	NodeRuntime->bIsCallNode = bIsCallNode;

	for (const auto& It : InternalPinArrays)
	{
		NodeRuntime->PinArrays.Add(It.Key, It.Value->Pins);
	}

	for (const FVoxelPin& Pin : GetPins())
	{
		const TSharedRef<FVoxelNodeRuntime::FPinData> PinData = MakeVoxelShared<FVoxelNodeRuntime::FPinData>(
			Pin.GetType(),
			Pin.bIsInput,
			FName(DebugName + "." + Pin.Name.ToString()),
			FVoxelGraphPinRef{ NodeRef, Pin.Name },
			Pin.Metadata);

		NodeRuntime->PinDatas.Add(Pin.Name, PinData);
	}

	PreCompile();

	for (const FVoxelPin& Pin : GetPins())
	{
		FVoxelNodeRuntime::FPinData& PinData = *NodeRuntime->PinDatas[Pin.Name];

		if (Pin.bIsInput)
		{
			if (Pin.Metadata.bVirtualPin)
			{
				const FVoxelGraphPinRef PinRef
				{
					NodeRef,
					Pin.Name
				};
				PinData.Compute = GVoxelGraphExecutorManager->MakeCompute_GameThread(Pin.GetType(), PinRef);
			}
			else
			{
				// Assigned by compiler utilities
			}
		}
		else
		{
			FVoxelComputeValue Compute = CompileCompute(Pin.Name);
			if (!ensure(Compute))
			{
				VOXEL_MESSAGE(Error, "INTERNAL ERROR: {0}.{1} has no Compute", this, Pin.Name);
				return;
			}

			PinData.Compute = MakeSharedCopy(MoveTemp(Compute));
		}
	}
}

void FVoxelNode::RemoveEditorData()
{
	VOXEL_FUNCTION_COUNTER();

	ensure(!bEditorDataRemoved);
	bEditorDataRemoved = true;

	DeferredPins.Empty();
	InternalPinBackups.Empty();
	InternalPinsOrder.Empty();
	InternalPins.Empty();
	InternalPinArrays.Empty();

#if WITH_EDITOR
	ExposedPinValues.Empty();
	ExposedPins.Empty();
#endif

	SerializedDataProperty = {};

	UpdateStats();
}

void FVoxelNode::EnableSharedNode(const TSharedRef<FVoxelNode>& SharedThis)
{
	check(this == &SharedThis.Get());
	check(!WeakThis.IsValid());
	WeakThis = SharedThis;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#if WITH_EDITOR
TSharedPtr<const IVoxelNodeDefinition::FNode> FVoxelNodeDefinition::GetInputs() const
{
	return GetPins(true);
}

TSharedPtr<const IVoxelNodeDefinition::FNode> FVoxelNodeDefinition::GetOutputs() const
{
	return GetPins(false);
}

TSharedPtr<const IVoxelNodeDefinition::FNode> FVoxelNodeDefinition::GetPins(const bool bInput) const
{
#if WITH_EDITOR
	TArray<TSharedRef<FNode>> Leaves;

	for (const FName PinName : Node.InternalPinsOrder)
	{
		const FVoxelNode::FDeferredPin* Pin = Node.InternalPinBackups.Find(PinName);
		if (!ensure(Pin))
		{
			continue;
		}

		if (Pin->bIsInput != bInput)
		{
			continue;
		}

		if (Pin->IsArrayElement())
		{
			continue;
		}

		if (Pin->IsArrayDeclaration())
		{
			TArray<FName> PinArrayPath = FNode::MakePath(Pin->Metadata.Category, PinName);
			TSharedRef<FNode> ArrayCategoryNode = FNode::MakeArrayCategory(PinName, PinArrayPath);
			Leaves.Add(ArrayCategoryNode);

			if (const TSharedPtr<FVoxelNode::FPinArray>& PinArray = Node.InternalPinArrays.FindRef(PinName))
			{
				for (const FName ArrayElement : PinArray->Pins)
				{
					ArrayCategoryNode->Children.Add(FNode::MakePin(ArrayElement, PinArrayPath));
				}
			}
			continue;
		}

		Leaves.Add(FNode::MakePin(PinName, FNode::MakePath(Pin->Metadata.Category)));
	}

	const TSharedRef<FNode> Root = FNode::MakeCategory({}, {});
	TMap<FName, TSharedPtr<FNode>> MappedCategories;

	const auto FindOrAddCategory = [&MappedCategories](const TSharedPtr<FNode>& Parent, const FString& PathElement, const FName FullPath)
	{
		if (const TSharedPtr<FNode>& CategoryNode = MappedCategories.FindRef(FullPath))
		{
			return CategoryNode.ToSharedRef();
		}

		TSharedRef<FNode> Category = FNode::MakeCategory(FName(PathElement), FNode::MakePath(FullPath.ToString()));
		Parent->Children.Add(Category);
		MappedCategories.Add(FullPath, Category);

		return Category;
	};

	for (const TSharedRef<FNode>& Leaf : Leaves)
	{
		int32 NumPath = Leaf->Path.Num();
		if (Leaf->NodeState != ENodeState::Pin)
		{
			NumPath--;
		}

		if (NumPath == 0)
		{
			Root->Children.Add(Leaf);
			continue;
		}

		FString CurrentPath = Leaf->Path[0].ToString();
		TSharedRef<FNode> ParentCategoryNode = FindOrAddCategory(Root, CurrentPath, FName(CurrentPath));

		for (int32 Index = 1; Index < NumPath; Index++)
		{
			CurrentPath += "|" + Leaf->Path[Index].ToString();
			ParentCategoryNode = FindOrAddCategory(ParentCategoryNode, Leaf->Path[Index].ToString(), FName(CurrentPath));
		}

		ParentCategoryNode->Children.Add(Leaf);
	}

	return Root;
#else
	ensure(false);
	return nullptr;
#endif
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FString FVoxelNodeDefinition::GetAddPinLabel() const
{
	return "Add pin";
}

FString FVoxelNodeDefinition::GetAddPinTooltip() const
{
	return "Add pin";
}

FString FVoxelNodeDefinition::GetRemovePinTooltip() const
{
	return "Remove pin";
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool FVoxelNodeDefinition::CanAddToCategory(FName Category) const
{
	return Node.InternalPinArrays.Contains(Category);
}

void FVoxelNodeDefinition::AddToCategory(FName Category)
{
	Node.AddPinToArray(Category);
}

bool FVoxelNodeDefinition::CanRemoveFromCategory(FName Category) const
{
	const TSharedPtr<FVoxelNode::FPinArray> PinArray = Node.InternalPinArrays.FindRef(Category);
	return PinArray && PinArray->Pins.Num() > PinArray->PinTemplate.MinArrayNum;
}

void FVoxelNodeDefinition::RemoveFromCategory(FName Category)
{
	const TSharedPtr<FVoxelNode::FPinArray> PinArray = Node.InternalPinArrays.FindRef(Category);
	if (!ensure(PinArray) ||
		!ensure(PinArray->Pins.Num() > 0) ||
		!ensure(PinArray->Pins.Num() > PinArray->PinTemplate.MinArrayNum))
	{
		return;
	}

	const FName PinName = PinArray->Pins.Last();
	Node.RemovePin(PinName);

	Node.ExposedPins.Remove(PinName);
	const int32 ExposedPinIndex = Node.ExposedPinValues.IndexOfByKey(PinName);
	if (ExposedPinIndex != -1)
	{
		Node.ExposedPinValues.RemoveAt(ExposedPinIndex);
	}

	Node.FixupArrayNames(Category);
}

bool FVoxelNodeDefinition::CanRemoveSelectedPin(FName PinName) const
{
	if (const TSharedPtr<FVoxelPin> Pin = Node.FindPin(PinName))
	{
		return CanRemoveFromCategory(Pin->ArrayOwner);
	}

	return false;
}

void FVoxelNodeDefinition::RemoveSelectedPin(FName PinName)
{
	if (!ensure(CanRemoveSelectedPin(PinName)))
	{
		return;
	}

	const TSharedPtr<FVoxelPin> Pin = Node.FindPin(PinName);

	if (Pin->Metadata.bShowInDetail)
	{
		Node.ExposedPins.Remove(Pin->Name);
		const int32 ExposedPinIndex = Node.ExposedPinValues.IndexOfByKey(Pin->Name);
		if (ExposedPinIndex != -1)
		{
			Node.ExposedPinValues.RemoveAt(ExposedPinIndex);
		}
	}

	Node.RemovePin(Pin->Name);

	Node.FixupArrayNames(Pin->ArrayOwner);
}

void FVoxelNodeDefinition::InsertPinBefore(FName PinName)
{
	const TSharedPtr<FVoxelPin> Pin = Node.FindPin(PinName);
	if (!Pin)
	{
		return;
	}

	const TSharedPtr<FVoxelNode::FPinArray> PinArray = Node.InternalPinArrays.FindRef(Pin->ArrayOwner);
	if (!PinArray)
	{
		return;
	}

	const int32 PinPosition = PinArray->Pins.IndexOfByPredicate([&Pin] (const FName& Name)
	{
		return Pin->Name == Name;
	});

	if (!ensure(PinPosition != -1))
	{
		return;
	}

	const FName NewPinName = Node.InsertPinToArrayPosition(Pin->ArrayOwner, PinPosition);
	Node.SortArrayPins(Pin->ArrayOwner);
	if (Pin->Metadata.bShowInDetail)
	{
		if (Node.ExposedPins.Contains(Pin->Name))
		{
			Node.ExposedPins.Add(NewPinName);
		}
	}
}

void FVoxelNodeDefinition::DuplicatePin(FName PinName)
{
	const TSharedPtr<FVoxelPin> Pin = Node.FindPin(PinName);
	if (!Pin)
	{
		return;
	}

	const TSharedPtr<FVoxelNode::FPinArray> PinArray = Node.InternalPinArrays.FindRef(Pin->ArrayOwner);
	if (!PinArray)
	{
		return;
	}

	const int32 PinPosition = PinArray->Pins.IndexOfByPredicate([&Pin] (const FName& Name)
	{
		return Pin->Name == Name;
	});

	if (!ensure(PinPosition != -1))
	{
		return;
	}

	const FName NewPinName = Node.InsertPinToArrayPosition(Pin->ArrayOwner, PinPosition + 1);
	Node.SortArrayPins(Pin->ArrayOwner);
	if (Pin->Metadata.bShowInDetail)
	{
		FVoxelPinValue NewValue;
		if (const FVoxelNodeExposedPinValue* PinValue = Node.ExposedPinValues.FindByKey(Pin->Name))
		{
			NewValue = PinValue->Value;
		}

		Node.ExposedPinValues.Add({ NewPinName, NewValue });

		if (Node.ExposedPins.Contains(Pin->Name))
		{
			Node.ExposedPins.Add(NewPinName);
		}
	}
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelComputeValue& FVoxelNodeCaller::FBindings::Bind(const FName Name) const
{
	FVoxelNodeRuntime::FPinData& PinData = *NodeRuntime.PinDatas[FName(Name)];
	ensure(!PinData.Compute);

	TVoxelUniquePtr<FVoxelComputeValue> NewCompute = MakeVoxelUnique<FVoxelComputeValue>();
	FVoxelComputeValue* NewComputePtr = NewCompute.Get();

	PinData.Compute = MakeVoxelShared<FVoxelComputeValue>(
		[NewCompute = MoveTemp(NewCompute), Type = PinData.Type](const FVoxelQuery& Query) -> FVoxelFutureValue
		{
			const FVoxelFutureValue Value = (*NewCompute)(Query);
			if (!Value.IsValid())
			{
				return FVoxelRuntimePinValue(Type);
			}
			return Value;
		});

	return *NewComputePtr;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelFastCriticalSection GVoxelNodePoolsCriticalSection;
TArray<FVoxelNodeCaller::FNodePool*> GVoxelNodePools_RequiresLock;

void PurgeVoxelNodePools()
{
	VOXEL_FUNCTION_COUNTER();
	VOXEL_SCOPE_LOCK(GVoxelNodePoolsCriticalSection);

	for (FVoxelNodeCaller::FNodePool* Pool : GVoxelNodePools_RequiresLock)
	{
		VOXEL_SCOPE_LOCK(Pool->CriticalSection);
		Pool->Nodes.Empty();
	}
}

VOXEL_CONSOLE_COMMAND(
	PurgeNodePools,
	"voxel.PurgeNodePools",
	"")
{
	PurgeVoxelNodePools();
}

VOXEL_RUN_ON_STARTUP_GAME(RegisterPurgeVoxelNodePools)
{
	GOnVoxelModuleUnloaded_DoCleanup.AddLambda([]
	{
		PurgeVoxelNodePools();
	});
}

FVoxelNodeCaller::FNodePool::FNodePool()
{
	VOXEL_SCOPE_LOCK(GVoxelNodePoolsCriticalSection);
	GVoxelNodePools_RequiresLock.Add(this);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelFutureValue FVoxelNodeCaller::CallNode(
	FNodePool& Pool,
	const UScriptStruct* Struct,
	const FName StatName,
	const FVoxelQuery& Query,
	const FVoxelPinRef OutputPin,
	const TFunctionRef<void(FBindings&, FVoxelNode& Node)> Bind)
{
	VOXEL_SCOPE_COUNTER_FNAME(StatName);

	TSharedPtr<FVoxelNode> Node;
	{
		VOXEL_SCOPE_LOCK(Pool.CriticalSection);

		if (Pool.Nodes.Num() > 0)
		{
			Node = Pool.Nodes.Pop(false);
		}
	}

	if (!Node)
	{
		VOXEL_SCOPE_COUNTER("Allocate new node");
		VOXEL_ALLOW_MALLOC_SCOPE();
		VOXEL_ALLOW_REALLOC_SCOPE();

		Node = MakeSharedStruct<FVoxelNode>(Struct);

		for (FVoxelPin& Pin : Node->GetPins())
		{
			// Calling nodes taking pin arrays is not supported
			ensure(Pin.ArrayOwner.IsNone());
			// Virtual pins are not supported
			ensure(!Pin.Metadata.bVirtualPin);
		}

		// Promote all template pins to buffers
		for (FVoxelPin& Pin : Node->GetPins())
		{
			if (EnumHasAllFlags(Pin.Flags, EVoxelPinFlags::TemplatePin))
			{
				Pin.SetType(Pin.GetType().GetBufferType());
			}
		}

		Node->InitializeNodeRuntime({}, true);
		Node->RemoveEditorData();
	}

	FVoxelNodeRuntime& NodeRuntime = ConstCast(Node->GetNodeRuntime());
	NodeRuntime.NodeRef = Query.GetTopNode();

	FBindings Bindings(NodeRuntime);
	Bind(Bindings, *Node);

	for (auto& It : NodeRuntime.PinDatas)
	{
		FVoxelNodeRuntime::FPinData& PinData = *It.Value;
		if (!PinData.bIsInput)
		{
			continue;
		}

		if (ensure(PinData.Compute))
		{
			continue;
		}

		PinData.Compute = MakeVoxelShared<FVoxelComputeValue>([Type = PinData.Type](const FVoxelQuery&)
		{
			return FVoxelFutureValue(FVoxelRuntimePinValue(Type));
		});
	}

	const FVoxelNodeRuntime::FPinData& PinData = NodeRuntime.GetPinData(OutputPin);
	check(!PinData.bIsInput);

	const FVoxelFutureValue Result = (*PinData.Compute)(Query.EnterScope(*Node));

	// Use a destructor so that the node is returned to pool even if the task is cancelled
	struct FReturnToPool
	{
		FNodePool& Pool;
		TSharedRef<FVoxelNode> Node;

		~FReturnToPool()
		{
			Node->ReturnToPool();

			VOXEL_SCOPE_LOCK(Pool.CriticalSection);
			VOXEL_ALLOW_REALLOC_SCOPE();
			Pool.Nodes.Add(Node);
		}
	};
	const TSharedRef<FReturnToPool> ReturnToPool = MakeVoxelShareable(new (GVoxelMemory) FReturnToPool
	{
		Pool,
		Node.ToSharedRef()
	});

	return MakeVoxelTask(STATIC_FNAME("Return node to pool"))
		.Dependency(Result)
		.Execute(PinData.Type, [Result, ReturnToPool]
		{
			(void)ReturnToPool;
			return Result;
		});
}