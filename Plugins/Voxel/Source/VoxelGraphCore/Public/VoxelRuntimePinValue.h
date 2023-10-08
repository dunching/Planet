// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelPinValue.h"
#include "VoxelRuntimePinValue.generated.h"

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelRuntimePinValue
{
	GENERATED_BODY()

private:
	FVoxelPinType Type;

	union
	{
		bool bBool;
		uint8 Byte;
		float Float;
		double Double;
		int32 Int32;
		int64 Int64;
		FVoxelNameWrapper Name;
		TSubclassOf<UObject> Class;
		UScriptStruct* SharedStructType;
		uint64 Raw;
	};
	TSharedPtr<const FVoxelSharedStructOpaque> SharedStruct;

public:
	FORCEINLINE FVoxelRuntimePinValue()
	{
		checkStatic(sizeof(FVoxelRuntimePinValue) == 40);
		Raw = 0;
	}
	explicit FVoxelRuntimePinValue(const FVoxelPinType& Type);

	FORCEINLINE FVoxelRuntimePinValue& WithType(const FVoxelPinType& OtherType)
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

public:
	static FVoxelRuntimePinValue MakeStruct(FConstVoxelStructView Struct);

public:
	template<typename T, typename = typename TEnableIf<!std::is_same_v<T, FVoxelBuffer>>::Type>
	static FVoxelRuntimePinValue Make(const T& Value)
	{
		checkStatic(TIsSafeVoxelPinValue<T>::Value);

		FVoxelRuntimePinValue Result;
		Result.Type = FVoxelPinType::Make<T>();

		if constexpr (std::is_same_v<T, bool>)
		{
			Result.bBool = Value;
		}
		else if constexpr (std::is_same_v<T, float>)
		{
			Result.Float = Value;
		}
		else if constexpr (std::is_same_v<T, double>)
		{
			Result.Double = Value;
		}
		else if constexpr (std::is_same_v<T, int32>)
		{
			Result.Int32 = Value;
		}
		else if constexpr (std::is_same_v<T, int64>)
		{
			Result.Int64 = Value;
		}
		else if constexpr (
			std::is_same_v<T, FName> ||
			std::is_same_v<T, FVoxelNameWrapper>)
		{
			Result.Name = Value;
		}
		else if constexpr (std::is_same_v<T, uint8>)
		{
			Result.Byte = Value;
		}
		else if constexpr (TIsEnum<T>::Value)
		{
			Result.Byte = uint8(Value);
		}
		else if constexpr (TIsTSubclassOf<T>::Value)
		{
			Result.Class = Value;
		}
		else
		{
			const FConstVoxelStructView StructView = MakeVoxelStructView(Value);
			Result.SharedStructType = StructView.GetStruct();
			Result.SharedStruct = StructView.MakeSharedCopy();
		}

		return Result;
	}

	template<typename T, typename = typename TEnableIf<!std::is_same_v<T, FVoxelBuffer>>::Type>
	static FVoxelRuntimePinValue Make(const TSharedRef<const T>& Value)
	{
		checkStatic(TIsSafeVoxelPinValue<T>::Value);
		const FConstVoxelStructView StructView = MakeVoxelStructView(*Value);

		FVoxelRuntimePinValue Result;
		Result.Type = FVoxelPinType::MakeStruct(StructView.GetStruct());
		Result.SharedStructType = StructView.GetStruct();
		Result.SharedStruct = ReinterpretCastRef<TSharedRef<const FVoxelSharedStructOpaque>>(Value);
		return Result;
	}

	template<typename T, typename = typename TEnableIf<!std::is_same_v<T, FVoxelBuffer>>::Type>
	static FVoxelRuntimePinValue Make(const TSharedRef<T>& Value)
	{
		return FVoxelRuntimePinValue::Make<T>(StaticCastSharedRef<const T>(Value));
	}

public:
	static FVoxelRuntimePinValue Make(const TSharedRef<const FVoxelBuffer>& Value) = delete;
	static FVoxelRuntimePinValue Make(const TSharedRef<FVoxelBuffer>& Value) = delete;

	static FVoxelRuntimePinValue Make(
		const TSharedRef<const FVoxelBuffer>& Value,
		const FVoxelPinType& BufferType);

	static FVoxelRuntimePinValue Make(
		const TSharedRef<FVoxelBuffer>& Value,
		const FVoxelPinType& BufferType);

public:
	template<typename T>
	FORCEINLINE TSharedRef<const T> GetSharedStruct() const
	{
		checkStatic(IsStructValue<T>);
		checkVoxelSlow(CanBeCastedTo<T>());
		checkVoxelSlow(Type.IsBuffer() || Type.IsStruct());
		checkVoxelSlow(SharedStructType);
		checkVoxelSlow(SharedStruct);

		return ReinterpretCastRef<TSharedRef<const T>>(SharedStruct.ToSharedRef());
	}
	FORCEINLINE FConstVoxelStructView GetStructView() const
	{
		checkVoxelSlow(Type.IsBuffer() || Type.IsStruct());
		checkVoxelSlow(SharedStructType);
		checkVoxelSlow(SharedStruct);
		checkVoxelSlow(
			!SharedStructType->IsChildOf(StaticStructFast<FVoxelVirtualStruct>()) ||
			reinterpret_cast<const FVoxelVirtualStruct&>(*SharedStruct).GetStruct() == SharedStructType);
		return FConstVoxelStructView(SharedStructType, SharedStruct.Get());
	}
	template<typename T = void>
	FORCEINLINE TConstVoxelStructView<T> GetStructView() const
	{
		checkVoxelSlow(CanBeCastedTo<T>());
		return TConstVoxelStructView<T>(GetStructView());
	}

public:
	FORCEINLINE bool IsValid() const
	{
		return Type.IsValid();
	}
	bool IsValidValue_Slow() const;

public:
	FORCEINLINE const FVoxelPinType& GetType() const
	{
		return Type;
	}
	FORCEINLINE bool IsBuffer() const
	{
		return Type.IsBuffer();
	}
	FORCEINLINE bool IsStruct() const
	{
		return Type.IsStruct();
	}
	template<typename T>
	FORCEINLINE bool Is() const
	{
		return Type.Is<T>();
	}
	template<typename T>
	FORCEINLINE bool CanBeCastedTo() const
	{
		return Type.CanBeCastedTo<T>();
	}
	FORCEINLINE bool CanBeCastedTo(const FVoxelPinType& Other) const
	{
		return Type.CanBeCastedTo(Other);
	}

public:
	template<typename T>
	static constexpr bool IsStructValue =
		!std::is_same_v<T, FVoxelNameWrapper> &&
		std::is_same_v<FVoxelObjectUtilities::TPropertyType<T>, FStructProperty>;

	template<typename T, typename = typename TEnableIf<!std::is_same_v<T, FName>>::Type>
	FORCEINLINE const T& Get() const
	{
		checkStatic(TIsSafeVoxelPinValue<T>::Value);
		checkVoxelSlow(Type.CanBeCastedTo<T>());

		if constexpr (IsStructValue<T>)
		{
			checkVoxelSlow(SharedStructType);
			checkVoxelSlow(SharedStruct.IsValid());
			return reinterpret_cast<const T&>(*SharedStruct.Get());
		}
		else if constexpr (std::is_same_v<T, bool>)
		{
			return bBool;
		}
		else if constexpr (std::is_same_v<T, float>)
		{
			return Float;
		}
		else if constexpr (std::is_same_v<T, double>)
		{
			return Double;
		}
		else if constexpr (std::is_same_v<T, int32>)
		{
			return Int32;
		}
		else if constexpr (std::is_same_v<T, int64>)
		{
			return Int64;
		}
		else if constexpr (std::is_same_v<T, FVoxelNameWrapper>)
		{
			return Name;
		}
		else if constexpr (std::is_same_v<T, uint8>)
		{
			return Byte;
		}
		else if constexpr (TIsEnum<T>::Value)
		{
			return ReinterpretCastRef<T>(Byte);
		}
		else if constexpr (TIsTSubclassOf<T>::Value)
		{
			return ReinterpretCastRef<T>(Class);
		}
		else
		{
			checkStatic(std::is_same_v<T, void>);
			check(false);
			static T* Value = new T();
			return *Value;
		}
	}
	template<typename T, typename = void, typename = typename TEnableIf<std::is_same_v<T, FName>>::Type>
	FORCEINLINE const T Get() const
	{
		return Get<FVoxelNameWrapper>();
	}

	FORCEINLINE TConstVoxelArrayView<uint8> GetRawView() const
	{
		checkVoxelSlow(IsValid());
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
		case EVoxelPinInternalType::Struct: return GetStructView().GetRawView();
		}
	}

	friend class UVoxelFunctionLibrary;
};