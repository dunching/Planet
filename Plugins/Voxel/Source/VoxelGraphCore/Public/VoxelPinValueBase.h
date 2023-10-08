// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelPinType.h"
#include "VoxelPinValueBase.generated.h"

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelPinValueBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category = "Config")
	FVoxelPinType Type;

	UPROPERTY(EditAnywhere, Category = "Config")
	bool bBool = false;

	UPROPERTY(EditAnywhere, Category = "Config")
	float Float = 0.f;

	UPROPERTY(EditAnywhere, Category = "Config")
	double Double = 0.;

	UPROPERTY(EditAnywhere, Category = "Config")
	int32 Int32 = 0;

	UPROPERTY(EditAnywhere, Category = "Config")
	int64 Int64 = 0;

	UPROPERTY(EditAnywhere, Category = "Config")
	FName Name;

	UPROPERTY(EditAnywhere, Category = "Config")
	uint8 Byte = 0;

	UPROPERTY(EditAnywhere, Category = "Config")
	TSubclassOf<UObject> Class;

	UPROPERTY(EditAnywhere, Category = "Config")
	TObjectPtr<UObject> Object;

	UPROPERTY(EditAnywhere, Category = "Config")
	FVoxelInstancedStruct Struct;

	virtual bool HasArray() const { return false; }
	virtual FString ExportToString_Array() const VOXEL_PURE_VIRTUAL({});
	virtual void ExportToProperty_Array(const FProperty& Property, void* Memory) const VOXEL_PURE_VIRTUAL();
	virtual bool ImportFromString_Array(const FString& Value) VOXEL_PURE_VIRTUAL({});
	virtual uint32 GetHash_Array() const VOXEL_PURE_VIRTUAL({});
	virtual void Fixup_Array(UObject* Outer) VOXEL_PURE_VIRTUAL();
	virtual bool Equal_Array(const FVoxelPinValueBase& Other) const VOXEL_PURE_VIRTUAL({});

	static FVoxelPinValueBase MakeStruct(FConstVoxelStructView Struct);
	static FVoxelPinValueBase MakeFromProperty(const FProperty& Property, const void* Memory);

public:
	FVoxelPinValueBase() = default;
	explicit FVoxelPinValueBase(const FVoxelPinType& Type);
	virtual ~FVoxelPinValueBase() = default;

	void ApplyToPinDefaultValue(UEdGraphPin& Pin) const;

	FString ExportToString() const;
	void ExportToProperty(const FProperty& Property, void* Memory) const;
	bool ImportFromString(const FString& Value);

	uint32 GetHash() const;
	void Fixup(UObject* Outer);
	bool IsValid() const;

public:
	FORCEINLINE const FVoxelPinType& GetType() const
	{
		return Type;
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
	FORCEINLINE auto& Get()
	{
		checkVoxelSlow(Type.CanBeCastedTo<T>());
		checkStatic(TIsSafeVoxelPinValue<T>::Value);
		checkStatic(!TIsVoxelBuffer<T>::Value);

		if constexpr (std::is_same_v<T, bool>)
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
		else if constexpr (std::is_same_v<T, FName>)
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
			using Type = typename TSubclassOfType<T>::Type;
			return ReinterpretCastRef<TSubclassOf<Type>>(Class);
		}
		else if constexpr (TIsDerivedFrom<T, UObject>::Value)
		{
			ensure(!Object || Object->IsA<T>());
			return ReinterpretCastRef<TObjectPtr<T>>(Object);
		}
		else
		{
			return Struct.Get<T>();
		}
	}

	FORCEINLINE bool IsClass() const
	{
		return Type.IsClass();
	}
	FORCEINLINE bool IsObject() const
	{
		return Type.IsObject();
	}
	FORCEINLINE bool IsStruct() const
	{
		return Type.IsStruct();
	}

	FORCEINLINE TSubclassOf<UObject>& GetClass()
	{
		checkVoxelSlow(IsClass());
		checkVoxelSlow(!Class || Class->IsChildOf(Type.GetBaseClass()));
		checkVoxelSlow(IsInGameThread() || IsInAsyncLoadingThread());
		return Class;
	}
	FORCEINLINE TObjectPtr<UObject>& GetObject()
	{
		checkVoxelSlow(IsObject());
		checkVoxelSlow(!Object || Object->IsA(Type.GetObjectClass()));
		checkVoxelSlow(IsInGameThread() || IsInAsyncLoadingThread());
		return Object;
	}
	FORCEINLINE FVoxelInstancedStruct& GetStruct()
	{
		checkVoxelSlow(IsStruct());
		checkVoxelSlow(Struct.GetScriptStruct()->IsChildOf(Type.GetStruct()));
		return Struct;
	}

public:
	template<typename T>
	FORCEINLINE const auto& Get() const
	{
		return ConstCast(this)->Get<T>();
	}
	FORCEINLINE const TSubclassOf<UObject>& GetClass() const
	{
		return ConstCast(this)->GetClass();
	}
	FORCEINLINE const TObjectPtr<UObject>& GetObject() const
	{
		return ConstCast(this)->GetObject();
	}
	FORCEINLINE const FVoxelInstancedStruct& GetStruct() const
	{
		return ConstCast(this)->GetStruct();
	}

public:
	bool operator==(const FVoxelPinValueBase& Other) const;
	bool operator!=(const FVoxelPinValueBase& Other) const
	{
		return !(*this == Other);
	}
};