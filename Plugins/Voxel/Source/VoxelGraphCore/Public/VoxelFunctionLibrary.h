// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelNodeMessages.h"
#include "VoxelRuntimePinValue.h"
#include "VoxelFunctionLibrary.generated.h"

#undef PARAM_PASSED_BY_REF
#define PARAM_PASSED_BY_REF(ParamName, PropertyType, ParamType) \
	ParamType* RESTRICT ParamName##TempPtr; \
	if constexpr (TIsDerivedFrom<ThisClass, UVoxelFunctionLibrary>::Value) \
	{ \
		ParamType& ParamName = Stack.StepCompiledInRef<PropertyType, ParamType>(nullptr); \
		ParamName##TempPtr = &ParamName; \
	} \
	else \
	{ \
		ParamType* ParamName##Temp = static_cast<ParamType*>(FMemory_Alloca(sizeof(ParamType))); \
		new (ParamName##Temp) ParamType(); \
		ParamType& ParamName = Stack.StepCompiledInRef<PropertyType, ParamType>(ParamName##Temp); \
		ParamName##TempPtr = &ParamName; \
	} \
	ParamType& ParamName = *ParamName##TempPtr; \

struct FVoxelFunctionNode;

#define FindVoxelQueryParameter_Function(Type, Name) \
	checkStatic(TIsDerivedFrom<Type, FVoxelQueryParameter>::Value); \
	const Type* Ptr_ ## Name = GetQuery().GetParameters().Find<Type>(); \
	if (!Ptr_ ## Name) { RaiseQueryError(Type::StaticStruct()); return {}; } \
	const TSharedRef<const Type> Name = CastChecked<const Type>(Ptr_ ## Name->AsShared());

#define CheckVoxelBuffersNum_Function(...) \
	if (!FVoxelBufferAccessor(__VA_ARGS__).IsValid()) \
	{ \
		RaiseBufferError(); \
		return {}; \
	}

#define CheckVoxelBuffersNum_Function_void(...) \
	if (!FVoxelBufferAccessor(__VA_ARGS__).IsValid()) \
	{ \
		RaiseBufferError(); \
		return; \
	}

#define ComputeVoxelBuffersNum_Function(...) FVoxelBufferAccessor(__VA_ARGS__).Num(); CheckVoxelBuffersNum_Function(__VA_ARGS__)
#define ComputeVoxelBuffersNum_Function_void(...) FVoxelBufferAccessor(__VA_ARGS__).Num(); CheckVoxelBuffersNum_Function_void(__VA_ARGS__)

template<typename>
struct TVoxelFunctionLibraryCaller;

UCLASS()
class VOXELGRAPHCORE_API UVoxelFunctionLibrary : public UObject
{
	GENERATED_BODY()

public:
	template<typename T>
	TVoxelFunctionLibraryCaller<T> MakeVoxelFunctionCaller() const;

	template<typename T>
	static TVoxelFunctionLibraryCaller<T> MakeVoxelFunctionCaller(
		const FVoxelGraphNodeRef& NodeRef,
		const FVoxelQuery& Query);

	struct FFrameOverride
	{
		TConstVoxelArrayView<FVoxelRuntimePinValue*> Values;
		int32 Index = 0;

		template<typename PropertyType, typename NativeType>
		FORCEINLINE void StepCompiledIn(NativeType* RESTRICT OutData)
		{
			FVoxelRuntimePinValue* Value = Values[Index++];
			checkVoxelSlow(Value);

			if constexpr (
				std::is_same_v<PropertyType, FBoolProperty> &&
				std::is_same_v<NativeType, uint32>)
			{
				*OutData = Value->Get<bool>();
			}
			else
			{
				static_assert(!FVoxelRuntimePinValue::IsStructValue<NativeType>, "All structs should be passed by ref in voxel functions");
				*OutData = Value->Get<NativeType>();
			}
		}
		template<typename, typename NativeType>
		FORCEINLINE NativeType& StepCompiledInRef(void* TemporaryBuffer)
		{
			static_assert(!std::is_same_v<NativeType, FName>, "Cannot pass FName by ref in voxel functions: either pass it by value or use FVoxelNameWrapper");

			checkVoxelSlow(!TemporaryBuffer);

			FVoxelRuntimePinValue* Value = Values[Index++];
			checkVoxelSlow(Value);

			if constexpr (std::is_same_v<NativeType, FVoxelRuntimePinValue>)
			{
				return *Value;
			}
			else
			{
				return ConstCast(Value->Get<NativeType>());
			}
		}
	};
	VOXEL_UFUNCTION_OVERRIDE(FFrameOverride);

	struct FContext
	{
		const FVoxelGraphNodeRef* NodeRef = nullptr;
		const FVoxelQuery* Query = nullptr;
	};
	FORCEINLINE const FContext& GetContext() const
	{
		return *reinterpret_cast<const FContext*>(this);
	}
	FORCEINLINE const FVoxelGraphNodeRef& GetNodeRef() const
	{
		return *GetContext().NodeRef;
	}
	FORCEINLINE const FVoxelQuery& GetQuery() const
	{
		return *GetContext().Query;
	}
	FORCEINLINE const FVoxelGraphNodeRef& GetMessageArg() const
	{
		return GetNodeRef();
	}

public:
	static void RaiseQueryError(const FVoxelGraphNodeRef& Node, const UScriptStruct* QueryType);
	static void RaiseBufferError(const FVoxelGraphNodeRef& Node);

	template<typename T>
	static void RaiseQueryError(const FVoxelGraphNodeRef& Node)
	{
		UVoxelFunctionLibrary::RaiseQueryError(Node, StaticStructFast<T>());
	}

	void RaiseQueryError(const UScriptStruct* QueryType) const
	{
		RaiseQueryError(GetNodeRef(), QueryType);
	}
	void RaiseBufferError() const
	{
		RaiseBufferError(GetNodeRef());
	}

public:
	static FVoxelPinType MakeType(const FProperty& Property);

	struct FCachedFunction
	{
		const UFunction& Function;
		FNativeFuncPtr NativeFunc;
		const FProperty* ReturnProperty;
		FVoxelPinType ReturnPropertyType;
		UScriptStruct* Struct = nullptr;
		UScriptStruct::ICppStructOps* CppStructOps = nullptr;
		int32 StructureSize = 0;
		bool bStructHasZeroConstructor = false;

		explicit FCachedFunction(const UFunction& Function);
	};
	static void Call(
		const FVoxelFunctionNode& Node,
		const FCachedFunction& CachedFunction,
		const FVoxelQuery& Query,
		const TConstVoxelArrayView<FVoxelRuntimePinValue*> Values);
};

template<typename T>
struct TVoxelFunctionLibraryCaller : UVoxelFunctionLibrary::FContext
{
	FORCEINLINE TVoxelFunctionLibraryCaller(
		const FVoxelGraphNodeRef& Node,
		const FVoxelQuery& Query)
	{
		this->NodeRef = &Node;
		this->Query = &Query;
	}
	FORCEINLINE TVoxelFunctionLibraryCaller(const UVoxelFunctionLibrary& Library)
		: TVoxelFunctionLibraryCaller(Library.GetNodeRef(), Library.GetQuery())
	{
	}

	FORCEINLINE const T* operator->() const
	{
		return reinterpret_cast<const T*>(this);
	}
	FORCEINLINE const T& operator*() const
	{
		return *reinterpret_cast<const T*>(this);
	}
};

template<typename T>
TVoxelFunctionLibraryCaller<T> UVoxelFunctionLibrary::MakeVoxelFunctionCaller() const
{
	return TVoxelFunctionLibraryCaller<T>(GetNodeRef(), GetQuery());
}

template<typename T>
TVoxelFunctionLibraryCaller<T> UVoxelFunctionLibrary::MakeVoxelFunctionCaller(
	const FVoxelGraphNodeRef& NodeRef,
	const FVoxelQuery& Query)
{
	return TVoxelFunctionLibraryCaller<T>(NodeRef, Query);
}

template<typename T>
TVoxelFunctionLibraryCaller<T> MakeVoxelFunctionCaller(
	const FVoxelGraphNodeRef& NodeRef,
	const FVoxelQuery& Query)
{
	return TVoxelFunctionLibraryCaller<T>(NodeRef, Query);
}