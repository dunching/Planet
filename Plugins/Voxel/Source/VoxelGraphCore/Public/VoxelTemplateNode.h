// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelNode.h"
#include "VoxelTemplateNode.generated.h"

// We need to make template node refs deterministic
struct FVoxelTemplateNodeContext
{
	const FVoxelGraphNodeRef& NodeRef;
	int32 Counter = 1;

	explicit FVoxelTemplateNodeContext(const FVoxelGraphNodeRef& NodeRef)
		: NodeRef(NodeRef)
	{
		ensure(NodeRef.TemplateInstance == 0);
	}
};
extern VOXELGRAPHCORE_API FVoxelTemplateNodeContext* GVoxelTemplateNodeContext;

struct VOXELGRAPHCORE_API FVoxelTemplateNodeUtilities
{
public:
	using FPin = Voxel::Graph::FPin;
	using FNode = Voxel::Graph::FNode;
	using FGraph = Voxel::Graph::FGraph;
	using ENodeType = Voxel::Graph::ENodeType;

	static FVoxelGraphNodeRef GetNodeRef()
	{
		check(GVoxelTemplateNodeContext);
		FVoxelGraphNodeRef Result = GVoxelTemplateNodeContext->NodeRef;
		Result.TemplateInstance = GVoxelTemplateNodeContext->Counter++;
		return Result;
	}

	// Keeps the same dimension
	// ie will convert FIntVector to FVector, FVector2D to FIntPoint etc
	template<typename T>
	static void SetPinScalarType(FVoxelPin& Pin)
	{
		checkStatic(
			std::is_same_v<T, float> ||
			std::is_same_v<T, double> ||
			std::is_same_v<T, int32>);

		Pin.SetType(FVoxelTemplateNodeUtilities::GetVectorType(FVoxelPinType::Make<T>(), GetDimension(Pin.GetType())));
	}
	static void SetPinDimension(FVoxelPin& Pin, int32 Dimension);

public:
	static TConstVoxelArrayView<FVoxelPinType> GetByteTypes();
	static TConstVoxelArrayView<FVoxelPinType> GetFloatTypes();
	static TConstVoxelArrayView<FVoxelPinType> GetDoubleTypes();
	static TConstVoxelArrayView<FVoxelPinType> GetIntTypes();
	static TConstVoxelArrayView<FVoxelPinType> GetObjectTypes();

public:
	static bool IsBool(const FVoxelPinType& PinType)
	{
		return PinType.GetInnerType().Is<bool>();
	}
	static bool IsByte(const FVoxelPinType& PinType)
	{
		return PinType.GetInnerType().Is<uint8>();
	}
	static bool IsFloat(const FVoxelPinType& PinType)
	{
		return GetFloatTypes().Contains(PinType);
	}
	static bool IsDouble(const FVoxelPinType& PinType)
	{
		return GetDoubleTypes().Contains(PinType);
	}
	static bool IsInt(const FVoxelPinType& PinType)
	{
		return GetIntTypes().Contains(PinType);
	}
	static bool IsObject(const FVoxelPinType& PinType)
	{
		return GetObjectTypes().Contains(PinType);
	}

	static bool IsPinBool(const FPin* Pin);
	static bool IsPinByte(const FPin* Pin);
	static bool IsPinFloat(const FPin* Pin);
	static bool IsPinDouble(const FPin* Pin);
	static bool IsPinInt(const FPin* Pin);
	static bool IsPinObject(const FPin* Pin);

public:
	static int32 GetDimension(const FVoxelPinType& PinType);
	static FVoxelPinType GetVectorType(const FVoxelPinType& PinType, int32 Dimension);

	static TSharedPtr<FVoxelNode> GetBreakNode(const FVoxelPinType& PinType);
	static TSharedPtr<FVoxelNode> GetMakeNode(const FVoxelPinType& PinType);

public:
	static int32 GetMaxDimension(const TArray<FPin*>& Pins);
	static FPin* GetLinkedPin(FPin* Pin);

	static FPin* ConvertToByte(FPin* Pin);
	static FPin* ConvertToFloat(FPin* Pin);
	static FPin* ConvertToDouble(FPin* Pin);

	static FPin* ScalarToVector(FPin* Pin, int32 HighestDimension);
	static FPin* ZeroExpandVector(FPin* Pin, int32 HighestDimension);

	static TArray<FPin*> BreakVector(FPin* Pin);
	static TArray<FPin*> BreakNode(FPin* Pin, const TSharedPtr<FVoxelNode>& BreakVoxelNode, int32 NumExpectedOutputPins);

	static FPin* MakeVector(TArray<FPin*> Pins);

	static bool IsPinOfName(const FPin* Pin, TSet<FName> Names);
	static FPin* MakeConstant(const FNode& Node, const FVoxelPinValue& Value);

private:
	static TSharedPtr<FVoxelNode> GetConvertToFloatNode(const FPin* Pin);
	static TSharedPtr<FVoxelNode> GetConvertToDoubleNode(const FPin* Pin);
	static TSharedPtr<FVoxelNode> GetMakeNode(const FPin* Pin, const int32 Dimension);

	static FNode& CreateNode(const FPin* Pin, const TSharedRef<FVoxelNode>& VoxelNode);

public:
	static bool Any(const TArray<FPin*>& Pins, const TFunctionRef<bool(FPin*)> Lambda);
	static bool All(const TArray<FPin*>& Pins, const TFunctionRef<bool(FPin*)> Lambda);
	static TArray<FPin*> Filter(const TArray<FPin*>& Pins, const TFunctionRef<bool(const FPin*)> Lambda);

	static FPin* Reduce(TArray<FPin*> Pins, TFunctionRef<FPin*(FPin*, FPin*)> Lambda);

	template<typename T, typename = typename TEnableIf<TIsDerivedFrom<T, FVoxelNode>::Value>::Type>
	static FPin* Call_Single(
		const TArray<FPin*>& Pins,
		TOptional<FVoxelPinType> OutputPinType = {})
	{
		return Call_Single(T::StaticStruct(), Pins, OutputPinType);
	}
	template<typename... TArg, typename = typename TEnableIf<(... && std::is_same_v<TArg, FPin*>)>::Type>
	static FPin* Call_Single(UScriptStruct* NodeStruct, TArg... Args)
	{
		return Call_Single(NodeStruct, { Args... });
	}
	template<typename T, typename... TArg, typename = typename TEnableIf<(TIsDerivedFrom<T, FVoxelNode>::Value && ... && std::is_same_v<TArg, FPin*>)>::Type>
	static FPin* Call_Single(TArg... Args)
	{
		return Call_Single(T::StaticStruct(), { Args... });
	}
	static FPin* Call_Single(
		const UScriptStruct* NodeStruct,
		const TArray<FPin*>& Pins,
		TOptional<FVoxelPinType> OutputPinType = {});

	template<typename T, typename = typename TEnableIf<TIsDerivedFrom<T, FVoxelNode>::Value>::Type>
	static TArray<FPin*> Call_Multi(const TArray<TArray<FPin*>>& Pins)
	{
		return Call_Multi(T::StaticStruct(), Pins);
	}
	template<typename... TArg, typename = typename TEnableIf<(... && std::is_same_v<TArg, TArray<FPin*>>)>::Type>
	static TArray<FPin*> Call_Multi(UScriptStruct* NodeStruct, TArg... Args)
	{
		return Call_Multi(NodeStruct, { Args... });
	}
	template<typename T, typename... TArg, typename = typename TEnableIf<(TIsDerivedFrom<T, FVoxelNode>::Value && ... && std::is_same_v<TArg, FPin*>)>::Type>
	static TArray<FPin*> Call_Multi(TArg... Args)
	{
		return Call_Multi(T::StaticStruct(), { Args... });
	}
	static TArray<FPin*> Call_Multi(
		const UScriptStruct* NodeStruct,
		const TArray<TArray<FPin*>>& Pins,
		TOptional<FVoxelPinType> OutputPinType = {});

	template<typename LambdaType, typename... TArgs, typename = typename TEnableIf<TIsInvocable<LambdaType, FPin*, TArgs...>::Value>::Type>
	static TArray<FPin*> Apply(TConstArrayView<FPin*> InPins, LambdaType&& Lambda, TArgs... Args)
	{
		TArray<FPin*> Pins(InPins);
		for (FPin*& Pin : Pins)
		{
			Pin = Lambda(Pin, Args...);
		}
		return Pins;
	}

	template<typename LambdaType, typename... TArgs, typename = typename TEnableIf<TIsInvocable<LambdaType, FPin*, TArgs...>::Value>::Type>
	static TArray<TArray<FPin*>> ApplyVector(TConstArrayView<FPin*> InPins, LambdaType&& Lambda, TArgs... Args)
	{
		TArray<TArray<FPin*>> Result;
		TArray<FPin*> Pins(InPins);
		for (FPin*& Pin : Pins)
		{
			Result.Add(Lambda(Pin, Args...));
		}
		return Result;
	}
};

USTRUCT(Category = "Template", meta = (Abstract))
struct VOXELGRAPHCORE_API FVoxelTemplateNode
	: public FVoxelNode
	, public FVoxelTemplateNodeUtilities
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

public:
	FVoxelTemplateNode() = default;

	virtual bool IsPureNode() const override
	{
		return true;
	}

	virtual void ExpandNode(FGraph& Graph, FNode& Node) const;
	virtual void ExpandPins(FNode& Node, TArray<FPin*> Pins, const TArray<FPin*>& AllPins, TArray<FPin*>& OutPins) const
	{
		OutPins = { ExpandPins(Node, Pins, AllPins) };
	}
	virtual FPin* ExpandPins(FNode& Node, TArray<FPin*> Pins, const TArray<FPin*>& AllPins) const VOXEL_PURE_VIRTUAL({});

protected:
	using FVoxelTemplateNodeUtilities::GetMaxDimension;

	TVoxelArray<FVoxelPinRef> GetAllPins() const;
	int32 GetMaxDimension(const TVoxelArray<FVoxelPinRef>& Pins) const;

	void FixupWildcards(const FVoxelPinType& NewType);
	void FixupBuffers(
		const FVoxelPinType& NewType,
		const TVoxelArray<FVoxelPinRef>& Pins);

	// Setting output will set all inputs to have less or same precision: eg, setting output to int will set all inputs to int
	// Setting input will set output to have at least the same precision: eg, setting input to double will set output to double
	// Will also force output to have a dimension same or equal as max input
	void EnforceNoPrecisionLoss(
		const FVoxelPin& InPin,
		const FVoxelPinType& NewType,
		const TVoxelArray<FVoxelPinRef>& Pins);

	// All pins will have to have the same dimension or be scalars
	// Allowed: float int32 FVector FIntVector
	// Not allowed: float FVector2D FVector
	void EnforceSameDimensions(
		const FVoxelPin& InPin,
		const FVoxelPinType& NewType,
		const TVoxelArray<FVoxelPinRef>& Pins);
};