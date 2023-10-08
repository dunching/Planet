// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelNodeHelpers.h"
#include "VoxelNodeMessages.h"
#include "VoxelNodeDefinition.h"
#include "VoxelNode.generated.h"

class UVoxelGraph;
class IVoxelSubsystem;
class FVoxelNodeDefinition;

enum class EVoxelPinFlags : uint32
{
	None         = 0,
	TemplatePin  = 1 << 0,
	ArrayPin     = 1 << 1,
};
ENUM_CLASS_FLAGS(EVoxelPinFlags);

struct FVoxelPinMetadataFlags
{
	bool bArrayPin = false;
	bool bVirtualPin = false;
	bool bConstantPin = false;
	bool bOptionalPin = false;
	bool bDisplayLast = false;
	bool bNoDefault = false;
	bool bShowInDetail = false;
};

struct FVoxelPinMetadata : FVoxelPinMetadataFlags
{
#if WITH_EDITOR
	FString DisplayName;
	FString Category;
	TAttribute<FString> Tooltip;
	FString DefaultValue;
	int32 Line = 0;
	UScriptStruct* Struct = nullptr;
#endif

	UClass* BaseClass = nullptr;
};

struct VOXELGRAPHCORE_API FVoxelPin
{
public:
	const FName Name;
	const bool bIsInput;
	const float SortOrder;
	const FName ArrayOwner;
	const EVoxelPinFlags Flags;
	const FVoxelPinType BaseType;
	const FVoxelPinMetadata Metadata;

	bool IsPromotable() const
	{
		return BaseType.IsWildcard();
	}

	void SetType(const FVoxelPinType& NewType)
	{
		ensure(IsPromotable());
		ChildType = NewType;
	}
	const FVoxelPinType& GetType() const
	{
		ensure(BaseType == ChildType || IsPromotable());
		return ChildType;
	}

private:
	FVoxelPinType ChildType;

	FVoxelPin(
		const FName Name,
		const bool bIsInput,
		const float SortOrder,
		const FName ArrayOwner,
		const EVoxelPinFlags Flags,
		const FVoxelPinType& BaseType,
		const FVoxelPinType& ChildType,
		const FVoxelPinMetadata& Metadata)
		: Name(Name)
		, bIsInput(bIsInput)
		, SortOrder(SortOrder)
		, ArrayOwner(ArrayOwner)
		, Flags(Flags)
		, BaseType(BaseType)
		, Metadata(Metadata)
		, ChildType(ChildType)
	{
		ensure(BaseType.IsValid());
		ensure(ChildType.IsValid());
	}

	friend struct FVoxelNode;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

struct FVoxelPinRef
{
public:
	using Type = void;

	FVoxelPinRef() = default;
	explicit FVoxelPinRef(const FName Name)
		: Name(Name)
	{
	}
	operator FName() const
	{
		return Name;
	}

private:
	FName Name;
};

struct FVoxelPinArrayRef
{
public:
	using Type = void;

	FVoxelPinArrayRef() = default;
	explicit FVoxelPinArrayRef(const FName Name)
		: Name(Name)
	{
	}
	operator FName() const
	{
		return Name;
	}

private:
	FName Name;
};

template<typename T>
struct TVoxelPinRef : FVoxelPinRef
{
	using Type = T;

	TVoxelPinRef() = default;
	explicit TVoxelPinRef(const FName Name)
		: FVoxelPinRef(Name)
	{
	}
};

template<>
struct TVoxelPinRef<FVoxelWildcard> : FVoxelPinRef
{
	using Type = void;

	TVoxelPinRef() = default;
	explicit TVoxelPinRef(const FName Name)
		: FVoxelPinRef(Name)
	{
	}
};

template<>
struct TVoxelPinRef<FVoxelWildcardBuffer> : FVoxelPinRef
{
	using Type = void;

	TVoxelPinRef() = default;
	explicit TVoxelPinRef(const FName Name)
		: FVoxelPinRef(Name)
	{
	}
};

template<typename T>
struct TVoxelPinArrayRef : FVoxelPinArrayRef
{
	using Type = T;

	TVoxelPinArrayRef() = default;
	explicit TVoxelPinArrayRef(const FName Name)
		: FVoxelPinArrayRef(Name)
	{
	}
};

template<>
struct TVoxelPinArrayRef<FVoxelWildcard> : FVoxelPinArrayRef
{
	using Type = void;

	TVoxelPinArrayRef() = default;
	explicit TVoxelPinArrayRef(const FName Name)
		: FVoxelPinArrayRef(Name)
	{
	}
};

template<>
struct TVoxelPinArrayRef<FVoxelWildcardBuffer> : FVoxelPinArrayRef
{
	using Type = void;

	TVoxelPinArrayRef() = default;
	explicit TVoxelPinArrayRef(const FName Name)
		: FVoxelPinArrayRef(Name)
	{
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DECLARE_UNIQUE_VOXEL_ID(FVoxelPinRuntimeId);
DECLARE_UNIQUE_VOXEL_ID(FVoxelGraphExecutorGlobalId);

class VOXELGRAPHCORE_API FVoxelNodeRuntime
	: public TSharedFromThis<FVoxelNodeRuntime>
	, public FVoxelNodeAliases
{
public:
	FVoxelNodeRuntime() = default;
	UE_NONCOPYABLE(FVoxelNodeRuntime);

	VOXEL_COUNT_INSTANCES();

	FVoxelFutureValue Get(
		const FVoxelPinRef& Pin,
		const FVoxelQuery& Query) const;

public:
	template<typename T>
	TValue<T> Get(const FVoxelPinRef& Pin, const FVoxelQuery& Query) const
	{
		return TValue<T>(Get(Pin, Query));
	}

	template<typename T, typename = typename TEnableIf<!std::is_same_v<T, FVoxelWildcard> && !std::is_same_v<T, FVoxelWildcardBuffer>>::Type>
	TValue<T> Get(const TVoxelPinRef<T>& Pin, const FVoxelQuery& Query) const
	{
		return this->Get<T>(static_cast<const FVoxelPinRef&>(Pin), Query);
	}

	template<typename T>
	TVoxelArray<TValue<T>> Get(const FVoxelPinArrayRef& ArrayPin, const FVoxelQuery& Query) const
	{
		const TVoxelArray<FName>& PinArray = PinArrays[ArrayPin];

		TVoxelArray<TValue<T>> Array;
		Array.Reserve(PinArray.Num());
		for (const FName Pin : PinArray)
		{
			Array.Add(Get<T>(FVoxelPinRef(Pin), Query));
		}
		return Array;
	}

	template<typename T>
	TVoxelArray<TValue<T>> Get(const TVoxelPinArrayRef<T>& ArrayPin, const FVoxelQuery& Query) const
	{
		return Get<T>(FVoxelPinArrayRef(ArrayPin), Query);
	}

public:
	const FVoxelGraphNodeRef& GetNodeRef() const
	{
		return NodeRef;
	}
	bool IsCallNode() const
	{
		return bIsCallNode;
	}

	TSharedRef<const FVoxelComputeValue> GetCompute(
		const FVoxelPinRef& Pin,
		const TSharedRef<FVoxelQueryContext>& Context) const;

	FVoxelDynamicValueFactory MakeDynamicValueFactory(const FVoxelPinRef& Pin) const;

	template<typename T, typename = typename TEnableIf<!std::is_same_v<T, FVoxelWildcard> && !std::is_same_v<T, FVoxelWildcardBuffer>>::Type>
	TSharedRef<const TVoxelComputeValue<T>> GetCompute(
		const TVoxelPinRef<T>& Pin,
		const TSharedRef<FVoxelQueryContext>& Context) const
	{
		return ReinterpretCastSharedPtr<const TVoxelComputeValue<T>>(this->GetCompute(FVoxelPinRef(Pin), Context));
	}
	template<typename T>
	TVoxelDynamicValueFactory<T> MakeDynamicValueFactory(const TVoxelPinRef<T>& Pin) const
	{
		return TVoxelDynamicValueFactory<T>(this->MakeDynamicValueFactory(FVoxelPinRef(Pin)));
	}

public:
	struct FPinData : TSharedFromThis<FPinData>
	{
		const FVoxelPinType Type;
		const bool bIsInput;
		const FName StatName;
		const FVoxelPinRuntimeId PinId;
		const FVoxelPinMetadataFlags Metadata;

		TSharedPtr<const FVoxelComputeValue> Compute;

		FPinData(
			const FVoxelPinType& Type,
			const bool bIsInput,
			const FName StatName,
			const FVoxelGraphPinRef& PinRef,
			const FVoxelPinMetadataFlags& Metadata);
	};
	const FPinData& GetPinData(const FName PinName) const
	{
		return *PinDatas[PinName];
	}
	const TVoxelMap<FName, TSharedPtr<FPinData>>& GetPinDatas() const
	{
		return PinDatas;
	}

private:
	const FVoxelNode* Node = nullptr;
	FVoxelGraphNodeRef NodeRef;
	TOptional<bool> AreTemplatePinsBuffers;

	bool bIsCallNode = false;

	struct FErrorMessage
	{
		FVoxelMessageBuilderId FirstBuilderId;
		FVoxelGraphExecutorGlobalId ExecutorGlobalId;
	};
	mutable TVoxelMap<FString, FErrorMessage> ErrorMessages;

	TVoxelMap<FName, TVoxelArray<FName>> PinArrays;
	TVoxelMap<FName, TSharedPtr<FPinData>> PinDatas;

	friend FVoxelNode;
	friend class FVoxelNodeCaller;
	friend TVoxelMessageArgProcessor<FVoxelNodeRuntime>;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template<typename NodeType>
struct TVoxelNodeRuntimeForward : public NodeType
{
	FORCEINLINE const FVoxelNodeRuntime& GetNodeRuntime() const
	{
		return static_cast<const NodeType*>(this)->GetNodeRuntime();
	}

	template<typename T, typename = typename TEnableIf<TIsDerivedFrom<T, FVoxelPinRef>::Value>::Type>
	FORCEINLINE auto Get(const T& Pin, const FVoxelQuery& Query) const -> decltype(auto)
	{
		return GetNodeRuntime().Get(Pin, Query);
	}
	template<typename T, typename = typename TEnableIf<!TIsDerivedFrom<T, FVoxelPinRef>::Value>::Type>
	FORCEINLINE auto Get(const FVoxelPinRef& Pin, const FVoxelQuery& Query) const -> decltype(auto)
	{
		return GetNodeRuntime().template Get<T>(Pin, Query);
	}

	template<typename T, typename = typename TEnableIf<TIsDerivedFrom<T, FVoxelPinArrayRef>::Value>::Type, typename = void>
	FORCEINLINE auto Get(const T& Pin, const FVoxelQuery& Query) const -> decltype(auto)
	{
		return GetNodeRuntime().Get(Pin, Query);
	}
	template<typename T, typename = typename TEnableIf<!TIsDerivedFrom<T, FVoxelPinArrayRef>::Value>::Type>
	FORCEINLINE auto Get(const FVoxelPinArrayRef& Pin, const FVoxelQuery& Query) const -> decltype(auto)
	{
		return GetNodeRuntime().template Get<T>(Pin, Query);
	}

	template<typename T, typename = typename TEnableIf<TIsDerivedFrom<T, FVoxelPinRef>::Value>::Type>
	FORCEINLINE auto GetCompute(const T& Pin, const TSharedRef<FVoxelQueryContext>& Context) const -> decltype(auto)
	{
		return GetNodeRuntime().GetCompute(Pin, Context);
	}
	template<typename T, typename = typename TEnableIf<TIsDerivedFrom<T, FVoxelPinRef>::Value>::Type>
	FORCEINLINE auto MakeDynamicValueFactory(const T& Pin) const -> decltype(auto)
	{
		return GetNodeRuntime().MakeDynamicValueFactory(Pin);
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelNodeExposedPinValue
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Voxel")
	FName Name;

	UPROPERTY(EditAnywhere, Category = "Voxel")
	FVoxelPinValue Value;

	bool operator==(const FName OtherName) const
	{
		return Name == OtherName;
	}

	// Required to compare nodes
	friend uint32 GetTypeHash(const FVoxelNodeExposedPinValue& InValue)
	{
		return FVoxelUtilities::MurmurHash(InValue.Name);
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT()
struct FVoxelNodeSerializedArrayData
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FName> PinNames;
};

USTRUCT()
struct FVoxelNodeSerializedData
{
	GENERATED_BODY()

	UPROPERTY()
	bool bIsValid = false;

	UPROPERTY()
	TMap<FName, FVoxelPinType> PinTypes;

	UPROPERTY()
	TMap<FName, FVoxelNodeSerializedArrayData> ArrayDatas;

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	TSet<FName> ExposedPins;

	UPROPERTY()
	TArray<FVoxelNodeExposedPinValue> ExposedPinsValues;
#endif

	// Required to compare nodes
	friend uint32 GetTypeHash(const FVoxelNodeSerializedData& Data)
	{
		return FVoxelUtilities::MurmurHashMulti(
			Data.PinTypes.Num(),
			Data.ArrayDatas.Num()
#if WITH_EDITOR
			, Data.ExposedPins.Num()
			, Data.ExposedPinsValues.Num()
#endif
		);
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

using FVoxelNodeComputePtr = FVoxelFutureValue (*) (const FVoxelNode& Node, const FVoxelQuery& Query);

VOXELGRAPHCORE_API void RegisterVoxelNodeComputePtr(
	const UScriptStruct* Node,
	FName PinName,
	FVoxelNodeComputePtr Ptr);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DECLARE_VOXEL_MEMORY_STAT(VOXELGRAPHCORE_API, STAT_VoxelNodes, "Nodes");

USTRUCT(meta = (Abstract))
struct VOXELGRAPHCORE_API FVoxelNode
	: public FVoxelVirtualStruct
	, public FVoxelNodeAliases
	, public FVoxelNodeHelpers
	, public IVoxelNodeInterface
{
	GENERATED_BODY()
	DECLARE_VIRTUAL_STRUCT_PARENT(FVoxelNode, GENERATED_VOXEL_NODE_BODY)

public:
	FVoxelNode() = default;
	FVoxelNode(const FVoxelNode& Other) = delete;
	FVoxelNode& operator=(const FVoxelNode& Other);

	VOXEL_COUNT_INSTANCES();
	VOXEL_ALLOCATED_SIZE_TRACKER(STAT_VoxelNodes);

	virtual int64 GetAllocatedSize() const;

	//~ Begin IVoxelNodeInterface Interface
	FORCEINLINE virtual const FVoxelGraphNodeRef& GetNodeRef() const final override
	{
		return GetNodeRuntime().GetNodeRef();
	}
	//~ End IVoxelNodeInterface Interface

public:
#if WITH_EDITOR
	virtual UStruct& GetMetadataContainer() const;

	virtual FString GetCategory() const;
	virtual FString GetDisplayName() const;
	virtual FString GetTooltip() const;
#endif

	virtual bool ShowPromotablePinsAsWildcards() const
	{
		return true;
	}
	virtual bool IsPureNode() const
	{
		return false;
	}

	virtual void ReturnToPool();

	virtual void PreCompile() {}
	virtual FVoxelComputeValue CompileCompute(FName PinName) const;

	virtual uint32 GetNodeHash() const;
	virtual bool IsNodeIdentical(const FVoxelNode& Other) const;

public:
	virtual void PreSerialize() override;
	virtual void PostSerialize() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) {}
#endif

private:
	using FVersion = DECLARE_VOXEL_VERSION
	(
		FirstVersion,
		AddIsValid
	);

	UPROPERTY()
	int32 Version = FVersion::FirstVersion;

public:
#if WITH_EDITOR
	using FDefinition = FVoxelNodeDefinition;
	virtual TSharedRef<FVoxelNodeDefinition> GetNodeDefinition();
#endif

#if WITH_EDITOR
	// Pin will always be a promotable pin
	virtual FVoxelPinTypeSet GetPromotionTypes(const FVoxelPin& Pin) const;
	virtual FString GetPinPromotionWarning(const FVoxelPin& Pin, const FVoxelPinType& NewType) const { return {}; }
	virtual void PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType);
#endif
	virtual void PromotePin_Runtime(FVoxelPin& Pin, const FVoxelPinType& NewType);

	FORCEINLINE bool AreTemplatePinsBuffers() const
	{
		const TOptional<bool> Value =
			NodeRuntime
			? NodeRuntime->AreTemplatePinsBuffers
			: AreTemplatePinsBuffersImpl();

		ensure(Value.IsSet());
		return Value.Get(false);
	}
	TOptional<bool> AreTemplatePinsBuffersImpl() const;

#if WITH_EDITOR
	bool IsPinHidden(const FVoxelPin& Pin) const;
	FString GetPinDefaultValue(const FVoxelPin& Pin) const;
	void UpdatePropertyBoundDefaultValue(const FVoxelPin& Pin, const FVoxelPinValue& NewValue);
#endif

public:
	template<typename T>
	struct TPinIterator
	{
		TVoxelMap<FName, TSharedPtr<FVoxelPin>>::TRangedForConstIterator Iterator;

		TPinIterator(TVoxelMap<FName, TSharedPtr<FVoxelPin>>::TRangedForConstIterator&& Iterator)
			: Iterator(Iterator)
		{
		}

		TPinIterator& operator++()
		{
			++Iterator;
			return *this;
		}
		explicit operator bool() const
		{
			return bool(Iterator);
		}
		T& operator*() const
		{
			return *Iterator.Value();
		}

		friend bool operator!=(const TPinIterator& Lhs, const TPinIterator& Rhs)
		{
			return Lhs.Iterator != Rhs.Iterator;
		}
	};
	template<typename T>
	struct TPinView
	{
		const TVoxelMap<FName, TSharedPtr<FVoxelPin>>& Pins;

		TPinView() = default;
		TPinView(const TVoxelMap<FName, TSharedPtr<FVoxelPin>>& Pins)
			: Pins(Pins)
		{
		}

		TPinIterator<T> begin() const { return TPinIterator<T>{ Pins.begin() }; }
		TPinIterator<T> end() const { return TPinIterator<T>{ Pins.end() }; }
	};

	TPinView<FVoxelPin> GetPins()
	{
		FlushDeferredPins();
		return TPinView<FVoxelPin>(InternalPins);
	}
	TPinView<const FVoxelPin> GetPins() const
	{
		FlushDeferredPins();
		return TPinView<const FVoxelPin>(InternalPins);
	}

	const TVoxelMap<FName, TSharedPtr<FVoxelPin>>& GetPinsMap()
	{
		FlushDeferredPins();
		return InternalPins;
	}
	const TVoxelMap<FName, TSharedPtr<const FVoxelPin>>& GetPinsMap() const
	{
		FlushDeferredPins();
		return ReinterpretCastRef<TVoxelMap<FName, TSharedPtr<const FVoxelPin>>>(InternalPins);
	}

	TSharedPtr<FVoxelPin> FindPin(const FName Name)
	{
		return GetPinsMap().FindRef(Name);
	}
	TSharedPtr<const FVoxelPin> FindPin(const FName Name) const
	{
		return GetPinsMap().FindRef(Name);
	}

	FVoxelPin& GetPin(const FVoxelPinRef& Pin)
	{
		return *GetPinsMap().FindChecked(Pin);
	}
	const FVoxelPin& GetPin(const FVoxelPinRef& Pin) const
	{
		return *GetPinsMap().FindChecked(Pin);
	}

	FVoxelPin& GetUniqueInputPin();
	FVoxelPin& GetUniqueOutputPin();

	const FVoxelPin& GetUniqueInputPin() const
	{
		return ConstCast(this)->GetUniqueInputPin();
	}
	const FVoxelPin& GetUniqueOutputPin() const
	{
		return ConstCast(this)->GetUniqueOutputPin();
	}

public:
	FName AddPinToArray(FName ArrayName, FName PinName = {});
	FName InsertPinToArrayPosition(FName ArrayName, int32 Position);
	FName AddPinToArray(FVoxelPinRef ArrayName, FName PinName = {}) = delete;

	const TVoxelArray<FVoxelPinRef>& GetArrayPins(const FVoxelPinArrayRef& ArrayRef) const
	{
		FlushDeferredPins();
		return ReinterpretCastVoxelArray<FVoxelPinRef>(InternalPinArrays[ArrayRef]->Pins);
	}
	template<typename T>
	const TArray<TVoxelPinRef<T>>& GetArrayPins(const TVoxelPinArrayRef<T>& ArrayRef) const
	{
		return ReinterpretCastArray<TVoxelPinRef<T>>(GetArrayPins(static_cast<const FVoxelPinArrayRef&>(ArrayRef)));
	}

private:
	void FixupArrayNames(FName ArrayName);

protected:
	FName CreatePin(
		const FVoxelPinType& Type,
		bool bIsInput,
		FName Name,
		const FVoxelPinMetadata& Metadata = {},
		EVoxelPinFlags Flags = EVoxelPinFlags::None,
		int32 MinArrayNum = 0);

	void RemovePin(FName Name);

protected:
	FVoxelPinRef CreateInputPin(
		const FVoxelPinType& Type,
		const FName Name,
		const FVoxelPinMetadata& Metadata = {},
		const EVoxelPinFlags Flags = EVoxelPinFlags::None)
	{
		return FVoxelPinRef(CreatePin(
			Type,
			true,
			Name,
			Metadata,
			Flags));
	}
	FVoxelPinRef CreateOutputPin(
		const FVoxelPinType& Type,
		const FName Name,
		const FVoxelPinMetadata& Metadata = {},
		const EVoxelPinFlags Flags = EVoxelPinFlags::None)
	{
		return FVoxelPinRef(CreatePin(
			Type,
			false,
			Name,
			Metadata,
			Flags));
	}

protected:
	template<typename Type>
	TVoxelPinRef<Type> CreateInputPin(
		const FName Name,
		const FVoxelPinMetadata& Metadata,
		const EVoxelPinFlags Flags = EVoxelPinFlags::None)
	{
		return TVoxelPinRef<Type>(this->CreateInputPin(
			FVoxelPinType::Make<Type>(),
			Name,
			Metadata,
			Flags));
	}
	template<typename Type>
	TVoxelPinRef<Type> CreateOutputPin(
		const FName Name,
		const FVoxelPinMetadata& Metadata,
		const EVoxelPinFlags Flags = EVoxelPinFlags::None)
	{
		return TVoxelPinRef<Type>(this->CreateOutputPin(
			FVoxelPinType::Make<Type>(),
			Name,
			Metadata,
			Flags));
	}

protected:
	FVoxelPinArrayRef CreateInputPinArray(
		const FVoxelPinType& Type,
		const FName Name,
		const FVoxelPinMetadata& Metadata,
		const int32 MinNum,
		const EVoxelPinFlags Flags = EVoxelPinFlags::None)
	{
		return FVoxelPinArrayRef(CreatePin(
			Type,
			true,
			Name,
			Metadata,
			Flags | EVoxelPinFlags::ArrayPin,
			MinNum));
	}
	template<typename Type>
	TVoxelPinArrayRef<Type> CreateInputPinArray(
		const FName Name,
		const FVoxelPinMetadata& Metadata,
		const int32 MinNum,
		const EVoxelPinFlags Flags = EVoxelPinFlags::None)
	{
		return TVoxelPinArrayRef<Type>(this->CreateInputPinArray(FVoxelPinType::Make<Type>(), Name, Metadata, MinNum, Flags));
	}

protected:
	struct FDeferredPin
	{
		FName ArrayOwner;
		int32 MinArrayNum = 0;

		FName Name;
		bool bIsInput = false;
		float SortOrder = 0.f;
		EVoxelPinFlags Flags = {};
		FVoxelPinType BaseType;
		FVoxelPinType ChildType;
		FVoxelPinMetadata Metadata;

		bool IsArrayElement() const
		{
			return !ArrayOwner.IsNone();
		}

		bool IsArrayDeclaration() const
		{
			return EnumHasAllFlags(Flags, EVoxelPinFlags::ArrayPin);
		}
	};
	struct FPinArray
	{
		const FDeferredPin PinTemplate;
		TVoxelArray<FName> Pins;

		explicit FPinArray(const FDeferredPin& PinTemplate)
			: PinTemplate(PinTemplate)
		{
		}
	};

private:
	bool bEditorDataRemoved = false;

	int32 SortOrderCounter = 1;

	bool bIsDeferringPins = true;
	TVoxelArray<FDeferredPin> DeferredPins;

	TVoxelMap<FName, FDeferredPin> InternalPinBackups;
	TVoxelArray<FName> InternalPinsOrder;
	int32 DisplayLastPins = 0;
	TVoxelMap<FName, TSharedPtr<FVoxelPin>> InternalPins;
	TVoxelMap<FName, TSharedPtr<FPinArray>> InternalPinArrays;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category = "Voxel", Transient)
	TArray<FVoxelNodeExposedPinValue> ExposedPinValues;

	TVoxelSet<FName> ExposedPins;
	FSimpleMulticastDelegate OnExposedPinsUpdated;
#endif

	FORCEINLINE void FlushDeferredPins() const
	{
		ensure(!bEditorDataRemoved);

		if (bIsDeferringPins)
		{
			ConstCast(this)->FlushDeferredPinsImpl();
		}
	}
	void FlushDeferredPinsImpl();
	void RegisterPin(FDeferredPin Pin, bool bApplyMinNum = true);
	void SortPins();
	void SortArrayPins(FName PinArrayName);

private:
	UPROPERTY()
	FVoxelNodeSerializedData SerializedDataProperty;

	FVoxelNodeSerializedData GetSerializedData() const;
	void LoadSerializedData(const FVoxelNodeSerializedData& SerializedData);

public:
	void InitializeNodeRuntime(
		const FVoxelGraphNodeRef& NodeRef,
		bool bIsCallNode);
	void RemoveEditorData();
	void EnableSharedNode(const TSharedRef<FVoxelNode>& SharedThis);

	FORCEINLINE bool HasNodeRuntime() const
	{
		return NodeRuntime.IsValid();
	}
	FORCEINLINE const FVoxelNodeRuntime& GetNodeRuntime() const
	{
		return *NodeRuntime;
	}
	template<typename T = FVoxelNode>
	FORCEINLINE TSharedRef<T> SharedNode() const
	{
		return CastChecked<T>(WeakThis.Pin().ToSharedRef());
	}

private:
	TSharedPtr<FVoxelNodeRuntime> NodeRuntime;
	TWeakPtr<FVoxelNode> WeakThis;

protected:
	using FReturnToPoolFunc = void (FVoxelNode::*)();

	void AddReturnToPoolFunc(FReturnToPoolFunc ReturnToPool)
	{
		ReturnToPoolFuncs.Add(ReturnToPool);
	}

private:
	TArray<FReturnToPoolFunc> ReturnToPoolFuncs;

	friend struct TVoxelMessageArgProcessor<FVoxelNode>;
	friend class FVoxelNodeDefinition;
	friend class FVoxelGraphNodeCustomization;
	friend class FVoxelGraphNodePinArrayCustomization;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#define GENERATED_VOXEL_NODE_DEFINITION_BODY(NodeType) \
	NodeType& Node; \
	explicit FDefinition(NodeType& Node) \
		: Super::FDefinition(Node) \
		, Node(Node) \
	{}

#if WITH_EDITOR
class VOXELGRAPHCORE_API FVoxelNodeDefinition : public IVoxelNodeDefinition
{
public:
	FVoxelNode& Node;

	explicit FVoxelNodeDefinition(FVoxelNode& Node)
		: Node(Node)
	{
	}

	virtual void Initialize(UEdGraphNode& EdGraphNode) {}

	virtual TSharedPtr<const FNode> GetInputs() const override;
	virtual TSharedPtr<const FNode> GetOutputs() const override;
	TSharedPtr<const FNode> GetPins(const bool bInput) const;

	virtual FString GetAddPinLabel() const override;
	virtual FString GetAddPinTooltip() const override;
	virtual FString GetRemovePinTooltip() const override;

	virtual bool CanAddToCategory(FName Category) const override;
	virtual void AddToCategory(FName Category) override;

	virtual bool CanRemoveFromCategory(FName Category) const override;
	virtual void RemoveFromCategory(FName Category) override;

	virtual bool CanRemoveSelectedPin(FName PinName) const override;
	virtual void RemoveSelectedPin(FName PinName) override;

	virtual void InsertPinBefore(FName PinName) override;
	virtual void DuplicatePin(FName PinName) override;
};
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#if WITH_EDITOR
#define IMPL_GENERATED_VOXEL_NODE_EDITOR_BODY() \
	virtual TSharedRef<FVoxelNodeDefinition> GetNodeDefinition() override { return MakeVoxelShared<FDefinition>(*this); }
#else
#define IMPL_GENERATED_VOXEL_NODE_EDITOR_BODY()
#endif

#define GENERATED_VOXEL_NODE_BODY() \
	GENERATED_VIRTUAL_STRUCT_BODY_IMPL(FVoxelNode) \
	IMPL_GENERATED_VOXEL_NODE_EDITOR_BODY()

namespace FVoxelPinMetadataBuilder
{
	struct ArrayPin {};
	// If the sub-graph changes, the value will update but this node won't be re-created
	struct VirtualPin {};
	// For exec nodes only. If set, the pin value will be computed before Create is called
	// Implies VirtualPin
	struct ConstantPin {};
	struct OptionalPin {};
	struct DisplayLast {};
	// Will error out if pin is unlinked
	struct NoDefault {};
	struct ShowInDetail {};
	struct AdvancedDisplay {};

	template<typename>
	struct BaseClass {};

	namespace Internal
	{
		struct None {};

		template<typename T>
		struct TStringParam
		{
#if WITH_EDITOR
			FString Value;
#endif

			template<typename CharType>
			explicit TStringParam(const CharType* Value)
#if WITH_EDITOR
				: Value(Value)
#endif
			{
			}
			explicit TStringParam(const FString& Value)
#if WITH_EDITOR
				: Value(Value)
#endif
			{
			}

			T operator()() const { return ReinterpretCastRef<const T&>(*this); }
		};
	}

	struct DisplayName : Internal::TStringParam<DisplayName> { using TStringParam::TStringParam; };
	struct Tooltip : Internal::TStringParam<Tooltip> { using TStringParam::TStringParam; };
	struct Category : Internal::TStringParam<Category> { using TStringParam::TStringParam; };

	template<typename Type>
	struct TBuilder
	{
		static void MakeImpl(FVoxelPinMetadata&, Internal::None) {}

		static void MakeImpl(FVoxelPinMetadata& Metadata, DisplayName Value)
		{
#if WITH_EDITOR
			ensure(Metadata.DisplayName.IsEmpty());
			Metadata.DisplayName = Value.Value;
#endif
		}
		static void MakeImpl(FVoxelPinMetadata& Metadata, Category Value)
		{
#if WITH_EDITOR
			ensure(Metadata.Category.IsEmpty());
			Metadata.Category = Value.Value;
#endif
		}
		static void MakeImpl(FVoxelPinMetadata& Metadata, Tooltip Value)
		{
#if WITH_EDITOR
			ensure(Metadata.Category.IsEmpty());
			Metadata.Tooltip = Value.Value;
#endif
		}

		static void MakeImpl(FVoxelPinMetadata& Metadata, ArrayPin)
		{
			ensure(!Metadata.bArrayPin);
			Metadata.bArrayPin = true;
		}
		static void MakeImpl(FVoxelPinMetadata& Metadata, VirtualPin)
		{
			ensure(!Metadata.bVirtualPin);
			Metadata.bVirtualPin = true;
		}
		static void MakeImpl(FVoxelPinMetadata& Metadata, ConstantPin)
		{
			ensure(!Metadata.bVirtualPin);
			Metadata.bVirtualPin = true;
			ensure(!Metadata.bConstantPin);
			Metadata.bConstantPin = true;
		}
		static void MakeImpl(FVoxelPinMetadata& Metadata, OptionalPin)
		{
			ensure(!Metadata.bOptionalPin);
			Metadata.bOptionalPin = true;
		}
		static void MakeImpl(FVoxelPinMetadata& Metadata, DisplayLast)
		{
			ensure(!Metadata.bDisplayLast);
			Metadata.bDisplayLast = true;
		}
		static void MakeImpl(FVoxelPinMetadata& Metadata, NoDefault)
		{
			ensure(!Metadata.bNoDefault);
			Metadata.bNoDefault = true;
		}
		static void MakeImpl(FVoxelPinMetadata& Metadata, ShowInDetail)
		{
			ensure(!Metadata.bShowInDetail);
			Metadata.bShowInDetail = true;
		}
		static void MakeImpl(FVoxelPinMetadata& Metadata, AdvancedDisplay)
		{
#if WITH_EDITOR
			ensure(Metadata.Category.IsEmpty());
			Metadata.Category = "Advanced";
#endif
		}

		template<typename Class>
		static void MakeImpl(FVoxelPinMetadata& Metadata, BaseClass<Class>)
		{
			ensure(!Metadata.BaseClass);
			Metadata.BaseClass = Class::StaticClass();
		}

		template<typename T>
		static constexpr bool IsValid = std::is_same_v<decltype(TBuilder::MakeImpl(DeclVal<FVoxelPinMetadata&>(), DeclVal<T>())), void>;

		template<typename T, typename... ArgTypes, typename = typename TEnableIf<(... && IsValid<ArgTypes>)>::Type>
		static FVoxelPinMetadata Make(
			const int32 Line,
			UScriptStruct* Struct,
			const T& DefaultValue,
			ArgTypes... Args)
		{
			FVoxelPinMetadata Metadata;
#if WITH_EDITOR
			ensure(Metadata.DefaultValue.IsEmpty());
			Metadata.DefaultValue = FVoxelNodeDefaultValueHelper::Get(static_cast<Type*>(nullptr), DefaultValue);
			Metadata.Line = Line;
			Metadata.Struct = Struct;
#endif
			VOXEL_FOLD_EXPRESSION(TBuilder::MakeImpl(Metadata, Args));
			return Metadata;
		}
	};
}

#define INTERNAL_VOXEL_PIN_METADATA_FOREACH(X) FVoxelPinMetadataBuilder::X()

#define VOXEL_PIN_METADATA_IMPL(Type, Line, Struct, Default, ...) \
	FVoxelPinMetadataBuilder::TBuilder<Type>::Make(Line, Struct, Default, VOXEL_FOREACH_COMMA(INTERNAL_VOXEL_PIN_METADATA_FOREACH, __VA_ARGS__))

#define VOXEL_PIN_METADATA(Type, Default, ...) VOXEL_PIN_METADATA_IMPL(Type, 0, nullptr, Default, Internal::None, ##__VA_ARGS__)

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#define INTERNAL_DECLARE_VOXEL_PIN(Name) INTELLISENSE_ONLY(void VOXEL_APPEND_LINE(__DummyFunction)(FVoxelPinRef Name);)

#define VOXEL_INPUT_PIN(InType, Name, Default, ...) \
	INTERNAL_DECLARE_VOXEL_PIN(Name); \
	TVoxelPinRef<InType> Name ## Pin = CreateInputPin<InType>(STATIC_FNAME(#Name), VOXEL_PIN_METADATA_IMPL(InType, __LINE__, StaticStruct(), Default, Internal::None, ##__VA_ARGS__));

#define VOXEL_INPUT_PIN_ARRAY(InType, Name, Default, MinNum, ...) \
	INTERNAL_DECLARE_VOXEL_PIN(Name); \
	TVoxelPinArrayRef<InType> Name ## Pins = CreateInputPinArray<InType>(STATIC_FNAME(#Name), VOXEL_PIN_METADATA_IMPL(InType, __LINE__, StaticStruct(), Default, Internal::None, ##__VA_ARGS__), MinNum);

#define VOXEL_OUTPUT_PIN(InType, Name, ...) \
	INTERNAL_DECLARE_VOXEL_PIN(Name); \
	TVoxelPinRef<InType> Name ## Pin = CreateOutputPin<InType>(STATIC_FNAME(#Name), VOXEL_PIN_METADATA_IMPL(InType, __LINE__, StaticStruct(), nullptr, Internal::None, ##__VA_ARGS__));

///////////////////////////////////////////////////////////////////////////////
/////////////////// Template pins can be scalar or array //////////////////////
///////////////////////////////////////////////////////////////////////////////

#define VOXEL_TEMPLATE_INPUT_PIN(InType, Name, Default, ...) \
	INTERNAL_DECLARE_VOXEL_PIN(Name); \
	FVoxelPinRef Name ## Pin = ( \
		[] { checkStatic(!TIsVoxelBuffer<InType>::Value); }, \
		CreateInputPin(FVoxelPinType::Make<InType>().GetBufferType(), STATIC_FNAME(#Name), VOXEL_PIN_METADATA_IMPL(InType, __LINE__, StaticStruct(), Default, Internal::None, ##__VA_ARGS__), EVoxelPinFlags::TemplatePin));

#define VOXEL_TEMPLATE_INPUT_PIN_ARRAY(InType, Name, Default, MinNum, ...) \
	INTERNAL_DECLARE_VOXEL_PIN(Name); \
	FVoxelPinArrayRef Name ## Pins = ( \
		[] { checkStatic(!TIsVoxelBuffer<InType>::Value); }, \
		CreateInputPinArray(FVoxelPinType::Make<InType>().GetBufferType(), STATIC_FNAME(#Name), VOXEL_PIN_METADATA_IMPL(InType, __LINE__, StaticStruct(), Default, Internal::None, ##__VA_ARGS__), MinNum, EVoxelPinFlags::TemplatePin));

#define VOXEL_TEMPLATE_OUTPUT_PIN(InType, Name, ...) \
	INTERNAL_DECLARE_VOXEL_PIN(Name); \
	FVoxelPinRef Name ## Pin = ( \
		[] { checkStatic(!TIsVoxelBuffer<InType>::Value); }, \
		CreateOutputPin(FVoxelPinType::Make<InType>().GetBufferType(), STATIC_FNAME(#Name), VOXEL_PIN_METADATA_IMPL(InType, __LINE__, StaticStruct(), nullptr, Internal::None, ##__VA_ARGS__), EVoxelPinFlags::TemplatePin));

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#define VOXEL_PIN_NAME(NodeType, PinName) \
	([]() -> const auto& \
	 { \
		static const auto StaticName = VOXEL_ALLOW_MALLOC_INLINE(NodeType().PinName); \
		return StaticName; \
	 }())

#define VOXEL_CALL_PARAM(Type, Name) \
	Type Name = {}; \
	void Internal_ReturnToPool_ ## Name() \
	{ \
		Name = Type(); \
	} \
	VOXEL_ON_CONSTRUCT() \
	{ \
		using ThisType = VOXEL_THIS_TYPE; \
		AddReturnToPoolFunc(static_cast<FReturnToPoolFunc>(&ThisType::Internal_ReturnToPool_ ## Name)); \
	};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class VOXELGRAPHCORE_API FVoxelNodeCaller
{
public:
	class VOXELGRAPHCORE_API FBindings
	{
	public:
		FVoxelComputeValue& Bind(FName Name) const;

		template<typename T>
		FORCEINLINE TVoxelComputeValue<T>& Bind(const TVoxelPinRef<T> Name) const
		{
			return ReinterpretCastRef<TVoxelComputeValue<T>>(Bind(FName(Name)));
		}

	private:
		FVoxelNodeRuntime& NodeRuntime;

		explicit FBindings(FVoxelNodeRuntime& NodeRuntime)
			: NodeRuntime(NodeRuntime)
		{
		}

		friend FVoxelNodeCaller;
	};

	struct VOXELGRAPHCORE_API FNodePool
	{
		FVoxelFastCriticalSection CriticalSection;
		TVoxelArray<TSharedRef<FVoxelNode>> Nodes;

		FNodePool();
	};

	template<typename NodeType, typename OutputType>
	struct TLambdaCaller
	{
		const FVoxelQuery& Query;
		const FVoxelPinRef OutputPin;

		template<typename LambdaType>
		TVoxelFutureValue<OutputType> operator+(LambdaType Lambda)
		{
			static FNodePool Pool;
			static const FName StatName = FVoxelUtilities::AppendName(TEXT("Call "), NodeType::StaticStruct()->GetFName());

			return TVoxelFutureValue<OutputType>(FVoxelNodeCaller::CallNode(
				Pool,
				NodeType::StaticStruct(),
				StatName,
				Query,
				OutputPin,
				[&](FBindings& Bindings, FVoxelNode& Node)
				{
					Lambda(Bindings, CastChecked<NodeType>(Node));
				}));
		}
	};

private:
	static FVoxelFutureValue CallNode(
		FNodePool& Pool,
		const UScriptStruct* Struct,
		FName StatName,
		const FVoxelQuery& Query,
		const FVoxelPinRef OutputPin,
		const TFunctionRef<void(FBindings&, FVoxelNode& Node)> Bind);
};

#define VOXEL_CALL_NODE(NodeType, OutputPin, Query) \
	FVoxelNodeCaller::TLambdaCaller<NodeType, VOXEL_GET_TYPE(NodeType().OutputPin)::Type>{ Query, VOXEL_PIN_NAME(NodeType, OutputPin) } + \
		[&](FVoxelNodeCaller::FBindings& Bindings, NodeType& CalleeNode)

#define VOXEL_CALL_NODE_BIND(Name, ...) \
	Bindings.Bind(CalleeNode.Name) = [this, \
		ReturnPinType = CalleeNode.GetNodeRuntime().GetPinData(CalleeNode.Name).Type, \
		ReturnPinStatName = CalleeNode.GetNodeRuntime().GetPinData(CalleeNode.Name).StatName, \
		ReturnInnerType = TVoxelTypeInstance<VOXEL_GET_TYPE(CalleeNode.Name)::Type>(), \
		##__VA_ARGS__](const FVoxelQuery& Query) \
	-> \
	TChooseClass< \
		TIsDerivedFrom<VOXEL_GET_TYPE(CalleeNode.Name), FVoxelPinRef>::Value, \
		::TVoxelFutureValue<VOXEL_GET_TYPE(CalleeNode.Name)::Type>, \
		TArray<::TVoxelFutureValue<VOXEL_GET_TYPE(CalleeNode.Name)::Type>> \
	>::Result