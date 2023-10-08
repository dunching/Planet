// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelPinValueBase.h"
#include "VoxelPinValue.generated.h"

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelTerminalPinValue final : public FVoxelPinValueBase
{
	GENERATED_BODY()

public:
	FVoxelTerminalPinValue() = default;
	explicit FVoxelTerminalPinValue(const FVoxelPinType& Type);

private:
	explicit FVoxelTerminalPinValue(const FVoxelPinValueBase& Value)
		: FVoxelPinValueBase((Value))
	{
	}
	explicit FVoxelTerminalPinValue(FVoxelPinValueBase&& Value)
		: FVoxelPinValueBase((MoveTemp(Value)))
	{
	}

public:
	FVoxelPinValue AsValue() const;

	FORCEINLINE TVoxelArrayView<uint8> GetRawView()
	{
		checkVoxelSlow(!Type.IsBuffer());

		switch (Type.GetInternalType())
		{
		default: VOXEL_ASSUME(false);
		case EVoxelPinInternalType::Bool: return MakeByteVoxelArrayView(bBool);
		case EVoxelPinInternalType::Float: return MakeByteVoxelArrayView(Float);
		case EVoxelPinInternalType::Double: return MakeByteVoxelArrayView(Double);
		case EVoxelPinInternalType::Int32: return MakeByteVoxelArrayView(Int32);
		case EVoxelPinInternalType::Int64: return MakeByteVoxelArrayView(Int64);
		case EVoxelPinInternalType::Name: return MakeByteVoxelArrayView(Name);
		case EVoxelPinInternalType::Byte: return MakeByteVoxelArrayView(Byte);
		case EVoxelPinInternalType::Class: return MakeByteVoxelArrayView(Class);
		case EVoxelPinInternalType::Object: return MakeByteVoxelArrayView(Object);
		case EVoxelPinInternalType::Struct: return Struct.GetStructView();
		}
	}
	FORCEINLINE TConstVoxelArrayView<uint8> GetRawView() const
	{
		return ConstCast(this)->GetRawView();
	}

public:
	template<typename T>
	static FVoxelTerminalPinValue Make(const T& Value = FVoxelUtilities::MakeSafe<T>())
	{
		checkStatic(TIsSafeVoxelPinValue<T>::Value);
		checkStatic(!TIsVoxelBuffer<T>::Value);

		FVoxelTerminalPinValue Result(FVoxelPinType::Make<T>());
		Result.Get<T>() = Value;
		return Result;
	}
	// For objects
	template<typename T>
	static FVoxelTerminalPinValue Make(T* Value)
	{
		checkStatic(TIsDerivedFrom<T, UObject>::Value);

		FVoxelTerminalPinValue Result(FVoxelPinType::Make<T>());
		Result.Get<T>() = Value;
		return Result;
	}

	static FVoxelTerminalPinValue MakeStruct(FConstVoxelStructView Struct)
	{
		return FVoxelTerminalPinValue(Super::MakeStruct(Struct));
	}
	static FVoxelTerminalPinValue MakeFromProperty(const FProperty& Property, const void* Memory)
	{
		return FVoxelTerminalPinValue(Super::MakeFromProperty(Property, Memory));
	}

public:
	bool operator==(const FVoxelTerminalPinValue& Other) const
	{
		return Super::operator==(Other);
	}
	bool operator!=(const FVoxelTerminalPinValue& Other) const
	{
		return Super::operator!=(Other);
	}

	friend uint32 GetTypeHash(const FVoxelTerminalPinValue& Value)
	{
		return Value.GetHash();
	}

	friend struct FVoxelPinValue;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct VOXELGRAPHCORE_API FVoxelPinValue final : public FVoxelPinValueBase
{
	GENERATED_BODY()

public:
	FVoxelPinValue() = default;
	explicit FVoxelPinValue(const FVoxelPinType& Type);

	template<typename T>
	static FVoxelPinValue Make(const T& Value = FVoxelUtilities::MakeSafe<T>())
	{
		checkStatic(TIsSafeVoxelPinValue<T>::Value);
		checkStatic(!TIsVoxelBuffer<T>::Value);

		FVoxelPinValue Result(FVoxelPinType::Make<T>());
		Result.Get<T>() = Value;
		return Result;
	}
	// For objects
	template<typename T>
	static FVoxelPinValue Make(T* Value)
	{
		checkStatic(TIsDerivedFrom<T, UObject>::Value);

		FVoxelPinValue Result(FVoxelPinType::Make<T>());
		Result.Get<T>() = Value;
		return Result;
	}

	static FVoxelPinValue MakeFromPinDefaultValue(const UEdGraphPin& Pin);
	static FVoxelPinValue MakeFromK2PinDefaultValue(const UEdGraphPin& Pin);

	static FVoxelPinValue MakeStruct(FConstVoxelStructView Struct);
	static FVoxelPinValue MakeFromProperty(const FProperty& Property, const void* Memory);

	using FVoxelPinValueBase::Fixup;
	void Fixup(const FVoxelPinType& NewType, UObject* Outer);
	bool ImportFromUnrelated(FVoxelPinValue Other);
	void PostSerialize(const FArchive& Ar);
	FVoxelTerminalPinValue AsTerminalValue() const;

public:
	FORCEINLINE bool IsArray() const
	{
		ensureVoxelSlow(!Type.IsBuffer() || Type.IsBufferArray());
		return Type.IsBuffer();
	}
	FORCEINLINE FVoxelPinValue& WithType(const FVoxelPinType& OtherType)
	{
		checkVoxelSlow(Type.IsBuffer() == OtherType.IsBuffer());

		if (Type.GetInnerType().Is<uint8>())
		{
			checkVoxelSlow(OtherType.GetInnerType().Is<uint8>());
		}
		else if (Type.GetInnerType().IsClass())
		{
			checkVoxelSlow(OtherType.GetInnerType().IsClass());
		}
		else
		{
			checkVoxelSlow(Type == OtherType);
		}

		Type = OtherType;
		return *this;
	}

	FORCEINLINE const TArray<FVoxelTerminalPinValue>& GetArray() const
	{
		checkVoxelSlow(IsArray());
		return Array;
	}
	FORCEINLINE void AddValue(const FVoxelTerminalPinValue& InnerValue)
	{
		checkVoxelSlow(IsArray());
		checkVoxelSlow(Type.GetInnerType() == InnerValue.GetType());
		Array.Add(InnerValue);
	}

private:
	UPROPERTY(EditAnywhere, Category = "Config")
	TArray<FVoxelTerminalPinValue> Array;

	UPROPERTY()
	uint8 Enum_DEPRECATED = 0;

	explicit FVoxelPinValue(const FVoxelPinValueBase& Value)
		: FVoxelPinValueBase((Value))
	{
	}
	explicit FVoxelPinValue(FVoxelPinValueBase&& Value)
		: FVoxelPinValueBase((MoveTemp(Value)))
	{
	}

	void InitializeFromPinDefaultValue(const UEdGraphPin& Pin);

	virtual bool HasArray() const override {return true;}
	virtual FString ExportToString_Array() const override;
	virtual void ExportToProperty_Array(const FProperty& Property, void* Memory) const override;
	virtual bool ImportFromString_Array(const FString& Value) override;
	virtual uint32 GetHash_Array() const override;
	virtual void Fixup_Array(UObject* Outer) override;
	virtual bool Equal_Array(const FVoxelPinValueBase& Other) const override;

public:
	bool operator==(const FVoxelPinValue& Other) const
	{
		return Super::operator==(Other);
	}
	bool operator!=(const FVoxelPinValue& Other) const
	{
		return Super::operator!=(Other);
	}

	friend uint32 GetTypeHash(const FVoxelPinValue& Value)
	{
		return Value.GetHash();
	}

	friend FVoxelTerminalPinValue;
	friend class FVoxelParameterDetails;
	friend class FVoxelPinValueCustomization;
	friend class FVoxelPinValueCustomizationHelper;
};

template<>
struct TStructOpsTypeTraits<FVoxelPinValue> : public TStructOpsTypeTraitsBase2<FVoxelPinValue>
{
	enum
	{
		WithPostSerialize = true,
	};
};