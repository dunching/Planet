// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelObjectPinType.generated.h"

template<typename>
struct TIsVoxelObjectStruct;

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelObjectPinType
{
	GENERATED_BODY()

	FVoxelObjectPinType() = default;
	virtual ~FVoxelObjectPinType() = default;

	virtual UClass* GetClass() const VOXEL_PURE_VIRTUAL({});
	virtual UScriptStruct* GetStruct() const VOXEL_PURE_VIRTUAL({});

	virtual TWeakObjectPtr<UObject> GetWeakObject(FConstVoxelStructView Struct) const VOXEL_PURE_VIRTUAL({});
	virtual FVoxelInstancedStruct GetStruct(UObject* Object) const VOXEL_PURE_VIRTUAL({});

	UObject* GetObject(FConstVoxelStructView Struct) const;

	static const TVoxelMap<const UScriptStruct*, const FVoxelObjectPinType*>& StructToPinType();
};

#define DECLARE_VOXEL_OBJECT_PIN_TYPE(Type) \
	template<> \
	struct TIsVoxelObjectStruct<Type> \
	{ \
		static constexpr bool Value = true; \
	};

#define DEFINE_VOXEL_OBJECT_PIN_TYPE(StructType, ObjectType) \
	void Dummy1() { checkStatic(std::is_same_v<VOXEL_THIS_TYPE, StructType ## PinType>); } \
	void Dummy2() { static_assert(TIsVoxelObjectStruct<StructType>::Value, "Object pin type not declared: use DECLARE_VOXEL_OBJECT_PIN_TYPE(YourType);"); } \
	virtual UClass* GetClass() const override { return ObjectType::StaticClass(); } \
	virtual UScriptStruct* GetStruct() const override { return StructType::StaticStruct(); } \
	virtual TWeakObjectPtr<UObject> GetWeakObject(const FConstVoxelStructView Struct) const override \
	{ \
		if (!ensure(Struct.IsValid()) || \
			!ensure(Struct.IsA<StructType>())) \
		{ \
			return nullptr; \
		} \
		\
		TWeakObjectPtr<ObjectType> Object; \
		Convert(true, Object, ConstCast(Struct.Get<StructType>())); \
		return Object; \
	} \
	virtual FVoxelInstancedStruct GetStruct(UObject* Object) const override \
	{ \
		check(IsInGameThread()); \
		FVoxelInstancedStruct Struct = FVoxelInstancedStruct::Make<StructType>(); \
		if (!Object) \
		{ \
			return Struct; \
		} \
		TWeakObjectPtr<ObjectType> TypedObject = Cast<ObjectType>(Object); \
		if (!ensure(TypedObject.IsValid())) \
		{ \
			return Struct; \
		} \
		Convert(false, TypedObject, Struct.Get<StructType>()); \
		ensure(TypedObject == Object); \
		return Struct; \
	} \
	void Convert(const bool bSetObject, TWeakObjectPtr<ObjectType>& Object, StructType& Struct) const