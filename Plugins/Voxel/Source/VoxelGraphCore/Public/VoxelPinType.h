// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelNameWrapper.h"
#include "VoxelPinType.generated.h"

struct FEdGraphPinType;
struct FVoxelBuffer;
struct FVoxelPinType;
struct FVoxelPinValue;
struct FVoxelParameterPath;
struct FVoxelRuntimePinValue;

template<typename, typename = void>
struct TVoxelBufferType
{
	using Type = void;
};
template<typename>
struct TVoxelBufferInnerType
{
	using Type = void;
};

template<typename>
struct TIsVoxelBuffer
{
	static constexpr bool Value = false;
};
template<typename>
struct TIsVoxelObjectStruct
{
	static constexpr bool Value = false;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

struct FVoxelWildcard;
struct FVoxelWildcardBuffer;

template <>
struct TVoxelBufferType<FVoxelWildcard>
{
	using Type = FVoxelWildcardBuffer;
};

template <>
struct TVoxelBufferInnerType<FVoxelWildcardBuffer>
{
	using Type = FVoxelWildcard;
};

template <>
struct TIsVoxelBuffer<FVoxelWildcardBuffer>
{
	static constexpr bool Value = true;
};

USTRUCT()
struct FVoxelWildcard_DEPRECATED
{
	GENERATED_BODY()
};
USTRUCT()
struct FVoxelWildcardBuffer_DEPRECATED
{
	GENERATED_BODY()
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelBufferInterface : public FVoxelVirtualStruct
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

	virtual int32 Num_Slow() const VOXEL_PURE_VIRTUAL({});
	virtual bool IsValid_Slow() const VOXEL_PURE_VIRTUAL({});
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template<typename>
struct TIsSafeVoxelPinType
{
	static constexpr bool Value = true;
};

template<>
struct TIsSafeVoxelPinType<FVoxelPinType>
{
	static constexpr bool Value = false;
};
template<>
struct TIsSafeVoxelPinType<FVoxelPinValue>
{
	static constexpr bool Value = false;
};
template<>
struct TIsSafeVoxelPinType<FVoxelInstancedStruct>
{
	static constexpr bool Value = false;
};
template<>
struct TIsSafeVoxelPinType<FVoxelBufferInterface>
{
	static constexpr bool Value = false;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

template<typename T>
struct TIsSafeVoxelPinValue
{
	static constexpr bool Value = TIsSafeVoxelPinType<T>::Value;
};

template<>
struct TIsSafeVoxelPinValue<FVoxelWildcard>
{
	static constexpr bool Value = false;
};
template<>
struct TIsSafeVoxelPinValue<FVoxelWildcardBuffer>
{
	static constexpr bool Value = false;
};
template<>
struct TIsSafeVoxelPinValue<FVoxelRuntimePinValue>
{
	static constexpr bool Value = false;
};
template<>
struct TIsSafeVoxelPinValue<FVoxelBufferInterface>
{
	static constexpr bool Value = true;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

UENUM()
enum class EVoxelPinInternalType : uint8
{
	Invalid,
	Wildcard,
	Bool,
	Float,
	Double,
	Int32,
	Int64,
	Name,
	Byte,
	Class,
	Object,
	Struct
};

USTRUCT()
struct FVoxelPinType_DEPRECATED
{
	GENERATED_BODY()

	UPROPERTY()
	FName PropertyClass;

	UPROPERTY()
	TObjectPtr<UObject> PropertyObject_Internal;

	UPROPERTY()
	FName Tag;
};

USTRUCT(BlueprintType)
struct VOXELGRAPHCORE_API FVoxelPinType
{
	GENERATED_BODY()

private:
	UPROPERTY()
	EVoxelPinInternalType InternalType = EVoxelPinInternalType::Invalid;

	UPROPERTY()
	bool bIsBuffer = false;

	UPROPERTY()
	bool bIsBufferArray = false;

	UPROPERTY()
	TObjectPtr<UField> PrivateInternalField;

	FORCEINLINE UField* GetInternalField() const
	{
		return ResolveObjectPtrFast(PrivateInternalField);
	}

public:
	FVoxelPinType() = default;
	FVoxelPinType(const FEdGraphPinType& PinType);
	explicit FVoxelPinType(const FProperty& Property);

public:
	template<typename T>
	FORCEINLINE static FVoxelPinType Make()
	{
		checkStatic(TIsSafeVoxelPinType<T>::Value);
		checkStatic(!std::is_same_v<T, FVoxelBuffer>);

		if constexpr (TIsVoxelBuffer<T>::Value)
		{
			return Make<typename TVoxelBufferInnerType<T>::Type>().GetBufferType();
		}
		else if constexpr (std::is_same_v<T, FVoxelWildcard>)
		{
			return MakeImpl(EVoxelPinInternalType::Wildcard);
		}
		else if constexpr (std::is_same_v<T, bool>)
		{
			return MakeImpl(EVoxelPinInternalType::Bool);
		}
		else if constexpr (std::is_same_v<T, float>)
		{
			return MakeImpl(EVoxelPinInternalType::Float);
		}
		else if constexpr (std::is_same_v<T, double>)
		{
			return MakeImpl(EVoxelPinInternalType::Double);
		}
		else if constexpr (std::is_same_v<T, int32>)
		{
			return MakeImpl(EVoxelPinInternalType::Int32);
		}
		else if constexpr (std::is_same_v<T, int64>)
		{
			return MakeImpl(EVoxelPinInternalType::Int64);
		}
		else if constexpr (
			std::is_same_v<T, FName> ||
			std::is_same_v<T, FVoxelNameWrapper>)
		{
			return MakeImpl(EVoxelPinInternalType::Name);
		}
		else if constexpr (std::is_same_v<T, uint8>)
		{
			return MakeImpl(EVoxelPinInternalType::Byte);
		}
		else if constexpr (TIsEnum<T>::Value)
		{
			return FVoxelPinType::MakeImpl(EVoxelPinInternalType::Byte, StaticEnumFast<T>());
		}
		else if constexpr (TIsTSubclassOf<T>::Value)
		{
			return FVoxelPinType::MakeImpl(EVoxelPinInternalType::Class, StaticClassFast<typename TSubclassOfType<T>::Type>());
		}
		else if constexpr (TIsDerivedFrom<T, UObject>::Value)
		{
			return FVoxelPinType::MakeImpl(EVoxelPinInternalType::Object, StaticClassFast<T>());
		}
		else
		{
			return FVoxelPinType::MakeImpl(EVoxelPinInternalType::Struct, StaticStructFast<T>());
		}
	}

	FORCEINLINE static FVoxelPinType MakeWildcard()
	{
		return Make<FVoxelWildcard>();
	}
	FORCEINLINE static FVoxelPinType MakeWildcardBuffer()
	{
		return MakeWildcard().GetBufferType();
	}
	FORCEINLINE static FVoxelPinType MakeEnum(UEnum* Enum)
	{
		checkVoxelSlow(Enum);
		return MakeImpl(EVoxelPinInternalType::Byte, Enum);
	}
	FORCEINLINE static FVoxelPinType MakeClass(UClass* BaseClass)
	{
		checkVoxelSlow(BaseClass);
		return MakeImpl(EVoxelPinInternalType::Class, BaseClass);
	}
	FORCEINLINE static FVoxelPinType MakeObject(UClass* Class)
	{
		checkVoxelSlow(Class);
		return MakeImpl(EVoxelPinInternalType::Object, Class);
	}
	static FVoxelPinType MakeStruct(UScriptStruct* Struct);
	static FVoxelPinType MakeFromK2(const FEdGraphPinType & PinType);

private:
	FORCEINLINE static FVoxelPinType MakeImpl(const EVoxelPinInternalType Type, UField* Field = nullptr)
	{
		FVoxelPinType PinType;
		PinType.InternalType = Type;
		PinType.PrivateInternalField = Field;
		checkVoxelSlow(PinType.IsValid());
		return PinType;
	}

public:
	FORCEINLINE bool IsBuffer() const
	{
		checkVoxelSlow(!bIsBufferArray || bIsBuffer);
		return bIsBuffer;
	}
	FORCEINLINE bool IsBufferArray() const
	{
		checkVoxelSlow(!bIsBufferArray || bIsBuffer);
		return bIsBufferArray;
	}

	FORCEINLINE FVoxelPinType GetBufferType() const
	{
		FVoxelPinType Result = *this;
		Result.bIsBuffer = true;
		return Result;
	}
	FORCEINLINE FVoxelPinType GetInnerType() const
	{
		FVoxelPinType Result = *this;
		Result.bIsBuffer = false;
		Result.bIsBufferArray = false;
		return Result;
	}

	FORCEINLINE FVoxelPinType WithBufferArray(const bool bNewIsBufferArray) const
	{
		checkVoxelSlow(IsBuffer());
		FVoxelPinType Result = *this;
		Result.bIsBufferArray = bNewIsBufferArray;
		return Result;
	}

public:
	bool IsValid() const;
	FString ToString() const;
	FEdGraphPinType GetEdGraphPinType() const;
	FEdGraphPinType GetEdGraphPinType_K2() const;
	bool HasPinDefaultValue() const;
	bool CanBeCastedTo(const FVoxelPinType& Other) const;
	bool CanBeCastedTo_K2(const FVoxelPinType& Other) const;
	bool CanBeCastedTo_Schema(const FVoxelPinType& Other) const;

	void PostSerialize(const FArchive& Ar);
	bool Serialize(FStructuredArchive::FSlot Slot);

public:
	FVoxelPinType GetExposedType() const;
	FVoxelPinType GetInnerExposedType() const;
	FVoxelPinType GetPinDefaultValueType() const;

	static FVoxelRuntimePinValue MakeRuntimeValue(
		const FVoxelPinType& RuntimeType,
		const FVoxelPinValue& ExposedValue);

	static FVoxelRuntimePinValue MakeRuntimeValueFromInnerValue(
		const FVoxelPinType& RuntimeType,
		const FVoxelPinValue& ExposedInnerValue);

	static FVoxelPinValue MakeExposedValue(
		const FVoxelRuntimePinValue& RuntimeValue,
		bool bIsArray);

	static FVoxelPinValue MakeExposedInnerValue(const FVoxelRuntimePinValue& RuntimeValue);

public:
	template<typename T>
	FORCEINLINE bool Is() const
	{
		checkStatic(TIsSafeVoxelPinType<T>::Value);
		checkStatic(!std::is_same_v<T, FVoxelBuffer>);

		if (bIsBuffer != TIsVoxelBuffer<T>::Value)
		{
			return false;
		}

		if constexpr (TIsVoxelBuffer<T>::Value)
		{
			return GetInnerType().Is<typename TVoxelBufferInnerType<T>::Type>();
		}

		if constexpr (std::is_same_v<T, bool>)
		{
			return InternalType == EVoxelPinInternalType::Bool;
		}
		else if constexpr (std::is_same_v<T, float>)
		{
			return InternalType == EVoxelPinInternalType::Float;
		}
		else if constexpr (std::is_same_v<T, double>)
		{
			return InternalType == EVoxelPinInternalType::Double;
		}
		else if constexpr (std::is_same_v<T, int32>)
		{
			return InternalType == EVoxelPinInternalType::Int32;
		}
		else if constexpr (std::is_same_v<T, int64>)
		{
			return InternalType == EVoxelPinInternalType::Int64;
		}
		else if constexpr (
			std::is_same_v<T, FName> ||
			std::is_same_v<T, FVoxelNameWrapper>)
		{
			return InternalType == EVoxelPinInternalType::Name;
		}
		else if constexpr (std::is_same_v<T, uint8>)
		{
			return InternalType == EVoxelPinInternalType::Byte;
		}
		else if constexpr (TIsEnum<T>::Value)
		{
			checkVoxelSlow(StaticEnumFast<T>()->GetMaxEnumValue() <= MAX_uint8);
			return
				InternalType == EVoxelPinInternalType::Byte &&
				GetInternalField() == StaticEnumFast<T>();
		}
		else if constexpr (TIsTSubclassOf<T>::Value)
		{
			return
				InternalType == EVoxelPinInternalType::Class &&
				GetInternalField() == StaticClassFast<typename TSubclassOfType<T>::Type>();
		}
		else if constexpr (TIsDerivedFrom<T, UObject>::Value)
		{
			return
				InternalType == EVoxelPinInternalType::Object &&
				GetInternalField() == StaticClassFast<T>();
		}
		else
		{
			return
				InternalType == EVoxelPinInternalType::Struct &&
				GetInternalField() == StaticStructFast<T>();
		}
	}

	template<typename T>
	FORCEINLINE bool CanBeCastedTo() const
	{
		if constexpr (std::is_same_v<T, FVoxelBuffer>)
		{
			return IsBuffer();
		}
		else if constexpr (std::is_same_v<T, FVoxelBufferInterface>)
		{
			if (IsBuffer())
			{
				return true;
			}

			if (!IsStruct())
			{
				return false;
			}

			return GetStruct()->IsChildOf(StaticStructFast<FVoxelBufferInterface>());
		}
		else
		{
			return this->CanBeCastedTo(Make<T>());
		}
	}

public:
	FORCEINLINE EVoxelPinInternalType GetInternalType() const
	{
		return InternalType;
	}

	FORCEINLINE bool IsWildcard() const
	{
		return InternalType == EVoxelPinInternalType::Wildcard;
	}
	FORCEINLINE bool IsClass() const
	{
		return
			!IsBuffer() &&
			InternalType == EVoxelPinInternalType::Class;
	}
	FORCEINLINE bool IsObject() const
	{
		return
			!IsBuffer() &&
			InternalType == EVoxelPinInternalType::Object;
	}
	FORCEINLINE bool IsStruct() const
	{
		return
			!IsBuffer() &&
			InternalType == EVoxelPinInternalType::Struct;
	}

	FORCEINLINE UEnum* GetEnum() const
	{
		checkVoxelSlow(!IsBuffer() && InternalType == EVoxelPinInternalType::Byte);
		return CastChecked<UEnum>(GetInternalField(), ECastCheckedType::NullAllowed);
	}
	FORCEINLINE UClass* GetBaseClass() const
	{
		checkVoxelSlow(IsClass());
		return CastChecked<UClass>(GetInternalField(), ECastCheckedType::NullAllowed);
	}
	FORCEINLINE UClass* GetObjectClass() const
	{
		checkVoxelSlow(IsObject());
		return CastChecked<UClass>(GetInternalField(), ECastCheckedType::NullAllowed);
	}
	FORCEINLINE UScriptStruct* GetStruct() const
	{
		checkVoxelSlow(IsStruct());
		return CastChecked<UScriptStruct>(GetInternalField(), ECastCheckedType::NullAllowed);
	}

public:
	FORCEINLINE bool operator==(const FVoxelPinType& Other) const
	{
		return
			InternalType == Other.InternalType &&
			bIsBuffer == Other.bIsBuffer &&
			bIsBufferArray == Other.bIsBufferArray &&
			GetInternalField() == Other.GetInternalField();
	}
	FORCEINLINE bool operator!=(const FVoxelPinType& Other) const
	{
		return
			InternalType != Other.InternalType ||
			bIsBuffer != Other.bIsBuffer ||
			bIsBufferArray != Other.bIsBufferArray ||
			GetInternalField() != Other.GetInternalField();
	}

	FORCEINLINE friend bool operator==(const FVoxelPinType& PinTypeA, const FEdGraphPinType& PinTypeB)
	{
		return PinTypeA == FVoxelPinType(PinTypeB);
	}
	FORCEINLINE friend bool operator!=(const FVoxelPinType& PinTypeA, const FEdGraphPinType& PinTypeB)
	{
		return PinTypeA != FVoxelPinType(PinTypeB);
	}

	FORCEINLINE friend bool operator==(const FEdGraphPinType& PinTypeA, const FVoxelPinType& PinTypeB)
	{
		return FVoxelPinType(PinTypeA) == PinTypeB;
	}
	FORCEINLINE friend bool operator!=(const FEdGraphPinType& PinTypeA, const FVoxelPinType& PinTypeB)
	{
		return FVoxelPinType(PinTypeA) != PinTypeB;
	}

	FORCEINLINE friend uint32 GetTypeHash(const FVoxelPinType& Type)
	{
		return
			FVoxelUtilities::MurmurHash((int32(Type.InternalType) * 256 + Type.bIsBuffer) * 2 + Type.bIsBufferArray) ^
			GetTypeHash(Type.GetInternalField());
	}
};
checkStatic(sizeof(FVoxelPinType) == 16);

template<>
struct TStructOpsTypeTraits<FVoxelPinType> : public TStructOpsTypeTraitsBase2<FVoxelPinType>
{
	enum
	{
		WithPostSerialize = true,
		WithStructuredSerializer = true,
	};
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#if WITH_EDITOR
enum class EVoxelPinTypeSetType
{
	Set,
	All,
	AllUniforms,
	AllBuffers,
	AllBufferArrays,
	AllUniformsAndBufferArrays,
	AllExposed,
	AllMaterials,
	AllEnums,
	AllObjects
};

struct VOXELGRAPHCORE_API FVoxelPinTypeSet
{
public:
	FVoxelPinTypeSet() = default;

	static FVoxelPinTypeSet All()
	{
		FVoxelPinTypeSet Set;
		Set.SetType = EVoxelPinTypeSetType::All;
		return Set;
	}
	static FVoxelPinTypeSet AllUniforms()
	{
		FVoxelPinTypeSet Set;
		Set.SetType = EVoxelPinTypeSetType::AllUniforms;
		return Set;
	}
	static FVoxelPinTypeSet AllBuffers()
	{
		FVoxelPinTypeSet Set;
		Set.SetType = EVoxelPinTypeSetType::AllBuffers;
		return Set;
	}
	static FVoxelPinTypeSet AllBufferArrays()
	{
		FVoxelPinTypeSet Set;
		Set.SetType = EVoxelPinTypeSetType::AllBufferArrays;
		return Set;
	}
	static FVoxelPinTypeSet AllUniformsAndBufferArrays()
	{
		FVoxelPinTypeSet Set;
		Set.SetType = EVoxelPinTypeSetType::AllUniformsAndBufferArrays;
		return Set;
	}
	static FVoxelPinTypeSet AllExposed()
	{
		FVoxelPinTypeSet Set;
		Set.SetType = EVoxelPinTypeSetType::AllExposed;
		return Set;
	}
	static FVoxelPinTypeSet AllMaterials()
	{
		FVoxelPinTypeSet Set;
		Set.SetType = EVoxelPinTypeSetType::AllMaterials;
		return Set;
	}
	static FVoxelPinTypeSet AllEnums()
	{
		FVoxelPinTypeSet Set;
		Set.SetType = EVoxelPinTypeSetType::AllEnums;
		return Set;
	}
	static FVoxelPinTypeSet AllObjects()
	{
		FVoxelPinTypeSet Set;
		Set.SetType = EVoxelPinTypeSetType::AllObjects;
		return Set;
	}

	EVoxelPinTypeSetType GetSetType() const
	{
		return SetType;
	}
	const TVoxelSet<FVoxelPinType>& GetTypes() const;

	void Add(const FVoxelPinType& Type)
	{
		ensure(SetType == EVoxelPinTypeSetType::Set);
		Types.Add(Type);
	}
	void Add(const TConstArrayView<FVoxelPinType> InTypes)
	{
		ensure(SetType == EVoxelPinTypeSetType::Set);
		Types.Append(InTypes);
	}
	void Add(const TSet<FVoxelPinType>& InTypes)
	{
		ensure(SetType == EVoxelPinTypeSetType::Set);
		Types.Append(InTypes);
	}
	template<typename T>
	void Add()
	{
		this->Add(FVoxelPinType::Make<T>());
	}
	bool Contains(const FVoxelPinType& Type) const;

private:
	EVoxelPinTypeSetType SetType = EVoxelPinTypeSetType::Set;
	TVoxelSet<FVoxelPinType> Types;
};
#endif