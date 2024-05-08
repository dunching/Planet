// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelNodeStats.h"
#include "VoxelDynamicValue.h"

#if WITH_EDITOR
class VOXELGRAPHCORE_API FVoxelNodeDefaultValueHelper
{
public:
	template<typename PinType, typename ValueType>
	static FString Get(PinType*, ValueType Value)
	{
		if constexpr (std::is_same_v<ValueType, decltype(nullptr)>)
		{
			return {};
		}
		else if constexpr (
			std::is_same_v<PinType, void> &&
			std::is_same_v<ValueType, FString>)
		{
			// Generic default
			return Value;
		}
		else if constexpr (
			std::is_same_v<PinType, FVoxelWildcard> ||
			std::is_same_v<PinType, FVoxelWildcardBuffer>)
		{
			// Wildcards can take any default value
			return FVoxelPinValue::Make(Value).ExportToString();
		}
		else if constexpr (TIsVoxelBuffer<PinType>::Value)
		{
			// Buffer can be initialized by their uniform
			checkStatic(std::is_same_v<typename TVoxelBufferInnerType<PinType>::Type, ValueType>);
			return FVoxelPinValue::Make(Value).ExportToString();
		}
		else if constexpr (TIsVoxelObjectStruct<PinType>::Value)
		{
			// Object need to be initialized by object paths
			return FVoxelNodeDefaultValueHelper::MakeObject(FVoxelPinType::Make<PinType>(), Value);
		}
		else if constexpr (
			std::is_same_v<PinType, FBodyInstance> &&
			std::is_same_v<ValueType, ECollisionEnabled::Type>)
		{
			FBodyInstance BodyInstance;
			BodyInstance.SetCollisionEnabled(Value);
			return FVoxelPinValue::Make(BodyInstance).ExportToString();
		}
		else
		{
			// Try to convert
			return FVoxelPinValue::Make(PinType(Value)).ExportToString();
		}
	}

private:
	static FString MakeObject(const FVoxelPinType& RuntimeType, const FString& Path);
};
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

struct FVoxelNodeAliases
{
	template<typename T>
	using TValue = ::TVoxelFutureValue<T>;

	template<typename T>
	using TVoxelFutureValue [[deprecated]] = TValue<T>;
};

struct VOXELGRAPHCORE_API IVoxelNodeInterface
{
	IVoxelNodeInterface() = default;
	virtual ~IVoxelNodeInterface() = default;

	virtual const FVoxelGraphNodeRef& GetNodeRef() const = 0;
};

struct VOXELGRAPHCORE_API FVoxelNodeHelpers
{
	static void RaiseQueryError(const FVoxelGraphNodeRef& Node, const UScriptStruct* QueryType);
	static void RaiseBufferError(const FVoxelGraphNodeRef& Node);

	template<typename T>
	static void RaiseQueryError(const FVoxelGraphNodeRef& Node)
	{
		FVoxelNodeHelpers::RaiseQueryError(Node, StaticStructFast<T>());
	}

	static void RaiseQueryError(const IVoxelNodeInterface& Node, const UScriptStruct* QueryType)
	{
		RaiseQueryError(Node.GetNodeRef(), QueryType);
	}
	static void RaiseBufferError(const IVoxelNodeInterface& Node)
	{
		RaiseBufferError(Node.GetNodeRef());
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#define DEFINE_VOXEL_NODE_COMPUTE(NodeName, PinName) \
	INTELLISENSE_ONLY(void VOXEL_APPEND_LINE(NodeName)(FVoxelPinRef PinName) { (void)((NodeName*)nullptr)->PinName ## Pin; (void)PinName; }); \
	struct NodeName ## _ ## PinName : public TVoxelNodeRuntimeForward<NodeName> \
	{ \
		using ReturnInnerType = decltype(DeclVal<NodeName>().PinName ## Pin)::Type; \
		using ReturnType = TVoxelFutureValueType<ReturnInnerType>; \
		\
		FORCEINLINE static FVoxelFutureValue _Compute(const FVoxelNode& Node, const FVoxelQuery& Query) \
		{ \
			const FVoxelQueryScope Scope(Query); \
			const NodeName& TypedNode = CastChecked<NodeName>(Node); \
			const FVoxelNodeRuntime::FPinData& PinData = Node.GetNodeRuntime().GetPinData(TypedNode.PinName ## Pin); \
			return static_cast<const NodeName ## _ ## PinName&>(TypedNode)._ComputeImpl( \
				PinData.Type, \
				PinData.StatName, \
				Query); \
		} \
		ReturnType _ComputeImpl( \
			const FVoxelPinType& ReturnPinType, \
			FName ReturnPinStatName, \
			const FVoxelQuery& Query) const; \
	}; \
	VOXEL_RUN_ON_STARTUP_GAME(Register ## NodeName ## _ ## PinName) \
	{ \
		RegisterVoxelNodeComputePtr( \
			NodeName::StaticStruct(), \
			#PinName, \
			&NodeName ## _ ## PinName::_Compute); \
	} \
	NodeName ## _ ## PinName::ReturnType \
	NodeName ## _ ## PinName::_ComputeImpl( \
		const FVoxelPinType& ReturnPinType, \
		const FName ReturnPinStatName, \
		const FVoxelQuery& Query) const

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#define FindVoxelQueryParameter(Type, Name) \
	checkStatic(TIsDerivedFrom<Type, FVoxelQueryParameter>::Value); \
	const Type* Ptr_ ## Name = Query.GetParameters().Find<Type>(); \
	if (!Ptr_ ## Name) { RaiseQueryError(*this, Type::StaticStruct()); return {}; } \
	const TSharedRef<const Type> Name = CastChecked<const Type>(Ptr_ ## Name->AsShared());

#define CheckVoxelBuffersNum(...) \
	if (!FVoxelBufferAccessor(__VA_ARGS__).IsValid()) \
	{ \
		RaiseBufferError(*this); \
		return {}; \
	}

#define ComputeVoxelBuffersNum(...) FVoxelBufferAccessor(__VA_ARGS__).Num(); CheckVoxelBuffersNum(__VA_ARGS__)

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template<typename T, typename = void>
struct TVoxelNodeLambdaArg
{
	static const T& Get(const T& Arg)
	{
		return Arg;
	}
};

template<>
struct TVoxelNodeLambdaArg<FVoxelFutureValue>
{
	static const FVoxelRuntimePinValue& Get(const FVoxelFutureValue& Arg)
	{
		return Arg.GetValue_CheckCompleted();
	}
};

template<typename T>
struct TVoxelNodeLambdaArg<TVoxelFutureValue<T>, typename TEnableIf<VoxelPassByValue<T>>::Type>
{
	static auto Get(const TVoxelFutureValue<T>& Arg) -> decltype(auto)
	{
		return Arg.Get_CheckCompleted();
	}
};
template<typename T>
struct TVoxelNodeLambdaArg<TVoxelFutureValue<T>, typename TEnableIf<!VoxelPassByValue<T>>::Type>
{
	static TSharedRef<const T> Get(const TVoxelFutureValue<T>& Arg)
	{
		return Arg.GetShared_CheckCompleted();
	}
};

template<typename T>
struct TVoxelNodeLambdaArg<TVoxelArray<TVoxelFutureValue<T>>, typename TEnableIf<VoxelPassByValue<T>>::Type>
{
	static TVoxelArray<T> Get(const TVoxelArray<TVoxelFutureValue<T>>& Arg)
	{
		TVoxelArray<T> Result;
		Result.Reserve(Arg.Num());
		for (const TVoxelFutureValue<T>& Element : Arg)
		{
			Result.Add(Element.Get_CheckCompleted());
		}
		return Result;
	}
};
template<typename T>
struct TVoxelNodeLambdaArg<TVoxelArray<TVoxelFutureValue<T>>, typename TEnableIf<!VoxelPassByValue<T>>::Type>
{
	static TVoxelArray<TSharedRef<const T>> Get(const TVoxelArray<TVoxelFutureValue<T>>& Arg)
	{
		TVoxelArray<TSharedRef<const T>> Result;
		Result.Reserve(Arg.Num());
		for (const TVoxelFutureValue<T>& Element : Arg)
		{
			Result.Add(Element.GetShared_CheckCompleted());
		}
		return Result;
	}
};

template<typename T>
struct TVoxelNodeLambdaArgType
{
	using Type = VOXEL_GET_TYPE(TVoxelNodeLambdaArg<T>::Get(DeclVal<T>()));
};

template<typename ReturnType, EVoxelTaskThread Thread, typename... ArgTypes>
class TVoxelNodeOnComplete
{
public:
	const FVoxelGraphNodeRef NodeRef;
	const FVoxelQuery& Query;
	const FVoxelPinType Type;
	const FName StatName;
	TTuple<ArgTypes...> Args;
#if VOXEL_DEBUG
	TVoxelUniqueFunction<void()> CheckIsReferenced_Debug;
#endif

	using FDependencies = TVoxelArray<FVoxelFutureValue, TVoxelInlineAllocator<16>>;
	FDependencies Dependencies;

	template<typename ThisType>
	TVoxelNodeOnComplete(
		const ThisType* This,
		const FVoxelQuery& Query,
		const FVoxelPinType& Type,
		const FName StatName,
		const ArgTypes&... Args)
		: NodeRef(This->GetNodeRef())
		, Query(Query)
		, Type(Type)
		, StatName(StatName)
		, Args(Args...)
	{
#if VOXEL_DEBUG
		CheckIsReferenced_Debug = [This]
		{
			checkVoxelSlow(FVoxelTaskReferencer::Get().IsReferenced(This));
		};
#endif

		if constexpr (!std::is_same_v<ReturnType, void>)
		{
			ensure(Type.Is<ReturnType>());
		}
		VOXEL_FOLD_EXPRESSION(TVoxelNodeOnComplete::AddDependencies(Dependencies, Args));
	}

public:
	static void AddDependencies(FDependencies& InDependencies, const FVoxelFutureValue& Value)
	{
		InDependencies.Add(Value);
	}
	template<typename OtherType, typename = typename TEnableIf<TIsDerivedFrom<OtherType, FVoxelFutureValue>::Value>::Type>
	static void AddDependencies(FDependencies& InDependencies, const TVoxelArray<OtherType>& Values)
	{
		for (const FVoxelFutureValue& Value : Values)
		{
			InDependencies.Add(Value);
		}
	}

	template<typename OtherType>
	static void AddDependencies(FDependencies&, const TSharedRef<OtherType>&)
	{
	}
	template<typename OtherType>
	static void AddDependencies(FDependencies&, const TSharedPtr<OtherType>&)
	{
	}
	template<typename OtherType>
	static void AddDependencies(FDependencies&, const TVoxelArray<TSharedRef<OtherType>>&)
	{
	}
	template<typename OtherType>
	static void AddDependencies(FDependencies&, const TVoxelArray<TSharedPtr<OtherType>>&)
	{
	}
	template<typename OtherType, typename = typename TEnableIf<TIsTriviallyDestructible<OtherType>::Value>::Type>
	static void AddDependencies(FDependencies&, OtherType)
	{
	}
	static void AddDependencies(FDependencies&, const FVoxelQuery&)
	{
	}
	static void AddDependencies(FDependencies&, const FVoxelBuffer&)
	{
	}
	static void AddDependencies(FDependencies&, const FVoxelTransformRef&)
	{
	}
	template<typename Body>
	static void AddDependencies(FDependencies&, const TFunction<Body>&)
	{
	}
	template<typename OtherType>
	static void AddDependencies(FDependencies&, OtherType*) = delete;

public:
	template<typename LambdaType>
	FORCEINLINE auto operator+(LambdaType&& Lambda)
	{
		return this->Execute(MoveTemp(Lambda), TMakeIntegerSequence<uint32, sizeof...(ArgTypes)>());
	}

private:
	template<typename LambdaType, uint32... ArgIndices>
	FORCEINLINE TVoxelFutureValueType<ReturnType> Execute(LambdaType&& Lambda, TIntegerSequence<uint32, ArgIndices...>)
	{
		return TVoxelFutureValueType<ReturnType>(
			MakeVoxelTask(StatName)
			.Thread(Thread)
			.Dependencies(Dependencies)
			.Execute(Type, [
#if VOXEL_DEBUG
				CheckIsReferenced_Debug = MoveTemp(CheckIsReferenced_Debug),
#endif
				Query = Query.EnterScope(NodeRef),
				Args = MoveTemp(Args),
				Lambda = MoveTemp(Lambda)]
			{
#if VOXEL_DEBUG
				CheckIsReferenced_Debug();
#endif

				const FVoxelQueryScope Scope(Query);

				if constexpr (Thread == EVoxelTaskThread::RenderThread)
				{
					return Lambda(Query, TVoxelNodeLambdaArg<ArgTypes>::Get(Args.template Get<ArgIndices>())..., FVoxelRDGBuilderScope::Get());
				}
				else
				{
					return Lambda(Query, TVoxelNodeLambdaArg<ArgTypes>::Get(Args.template Get<ArgIndices>())...);
				}
			}));
	}
};

#define VOXEL_SETUP_ON_COMPLETE(Pin) \
	using ReturnInnerType = decltype(Pin)::Type; \
	const FVoxelNodeRuntime::FPinData& VOXEL_APPEND_LINE(PinData) = GetNodeRuntime().GetPinData(Pin); \
	const FVoxelPinType ReturnPinType = VOXEL_APPEND_LINE(PinData).Type; \
	const FName ReturnPinStatName = VOXEL_APPEND_LINE(PinData).StatName;

#define VOXEL_SETUP_ON_COMPLETE_MANUAL(InReturnType, StatName) \
	using ReturnInnerType = InReturnType; \
	FVoxelPinType ReturnPinType = FVoxelPinType::Make<InReturnType>(); \
	FName ReturnPinStatName = STATIC_FNAME(StatName);

#define VOXEL_ON_COMPLETE_IMPL_CHECK(Data) \
	{ \
		TVoxelNodeOnComplete<void, EVoxelTaskThread::AsyncThread>::FDependencies Dependencies; \
		TVoxelNodeOnComplete<void, EVoxelTaskThread::AsyncThread>::AddDependencies(Dependencies, Data); \
	}

#define VOXEL_ON_COMPLETE_IMPL_DECLTYPE(Data) , std::remove_const_t<VOXEL_GET_TYPE(Data)>
#define VOXEL_ON_COMPLETE_IMPL_LAMBDA_ARGS(Data) , const TVoxelNodeLambdaArgType<std::remove_const_t<VOXEL_GET_TYPE(Data)>>::Type& Data

#define VOXEL_ON_COMPLETE_IMPL_AsyncThread
#define VOXEL_ON_COMPLETE_IMPL_GameThread
#define VOXEL_ON_COMPLETE_IMPL_RenderThread , FRDGBuilder& GraphBuilder

#define VOXEL_ON_COMPLETE_IMPL(Thread, ...) \
	(INTELLISENSE_ONLY([__VA_ARGS__] { VOXEL_FOREACH(VOXEL_ON_COMPLETE_IMPL_CHECK, DeclVal<FVoxelFutureValue>(), ##__VA_ARGS__); },) \
	TVoxelNodeOnComplete<decltype(ReturnInnerType()), EVoxelTaskThread::Thread VOXEL_FOREACH(VOXEL_ON_COMPLETE_IMPL_DECLTYPE, ##__VA_ARGS__)>(this, Query, ReturnPinType, ReturnPinStatName, ##__VA_ARGS__)) + \
	[this, ReturnPinType = ReturnPinType, ReturnPinStatName = ReturnPinStatName](const FVoxelQuery& Query VOXEL_FOREACH(VOXEL_ON_COMPLETE_IMPL_LAMBDA_ARGS, ##__VA_ARGS__) VOXEL_ON_COMPLETE_IMPL_ ## Thread) \
		-> TVoxelFutureValueType<decltype(ReturnInnerType())>

#define VOXEL_ON_COMPLETE(...) VOXEL_ON_COMPLETE_IMPL(AsyncThread, ##__VA_ARGS__)
#define VOXEL_ON_COMPLETE_GAME_THREAD(...) VOXEL_ON_COMPLETE_IMPL(GameThread, ##__VA_ARGS__)
#define VOXEL_ON_COMPLETE_RENDER_THREAD(...) VOXEL_ON_COMPLETE_IMPL(RenderThread, ##__VA_ARGS__)