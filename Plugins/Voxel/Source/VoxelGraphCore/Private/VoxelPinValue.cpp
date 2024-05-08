// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelPinValue.h"
#include "EdGraph/EdGraphPin.h"

FVoxelTerminalPinValue::FVoxelTerminalPinValue(const FVoxelPinType& Type)
	: FVoxelPinValueBase(Type)
{
	ensure(!Type.IsBuffer());
	Fixup(nullptr);
}

FVoxelPinValue FVoxelTerminalPinValue::AsValue() const
{
	return FVoxelPinValue(FVoxelPinValueBase(*this));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelPinValue::FVoxelPinValue(const FVoxelPinType& Type)
	: FVoxelPinValueBase(Type)
{
	// Exposed buffers should be arrays
	ensure(!Type.IsBuffer() || Type.IsBufferArray());
	Fixup(nullptr);
}

FVoxelPinValue FVoxelPinValue::MakeFromPinDefaultValue(const UEdGraphPin& Pin)
{
	const FVoxelPinType Type = FVoxelPinType(Pin.PinType).GetPinDefaultValueType();
	if (!ensure(Type.IsValid()) ||
		!ensure(!Type.IsBuffer()))
	{
		return {};
	}

	FVoxelPinValue Result(Type);
	Result.InitializeFromPinDefaultValue(Pin);
	return Result;
}

FVoxelPinValue FVoxelPinValue::MakeFromK2PinDefaultValue(const UEdGraphPin & Pin)
{
	const FVoxelPinType Type = FVoxelPinType::MakeFromK2(Pin.PinType);
	if (!ensure(Type.IsValid()) ||
		!ensure(!Type.IsBuffer()))
	{
		return {};
	}

	FVoxelPinValue Result(Type);
	Result.InitializeFromPinDefaultValue(Pin);
	return Result;
}

FVoxelPinValue FVoxelPinValue::MakeStruct(FConstVoxelStructView Struct)
{
	return FVoxelPinValue(Super::MakeStruct(Struct));
}

FVoxelPinValue FVoxelPinValue::MakeFromProperty(const FProperty& Property, const void* Memory)
{
	const FVoxelPinType Type(Property);
	if (Type.IsBuffer())
	{
		const FArrayProperty& ArrayProperty = CastFieldChecked<FArrayProperty>(Property);
		FScriptArrayHelper ArrayHelper(&ArrayProperty, Memory);

		FVoxelPinValue Result(Type.WithBufferArray(true));
		for (int32 Index = 0; Index < ArrayHelper.Num(); Index++)
		{
			Result.Array.Add(FVoxelTerminalPinValue::MakeFromProperty(
				*ArrayProperty.Inner,
				ArrayHelper.GetRawPtr(Index)));
		}
		return Result;
	}

	return FVoxelPinValue(Super::MakeFromProperty(Property, Memory));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelPinValue::Fixup(const FVoxelPinType& NewType, UObject* Outer)
{
	if (!IsValid() ||
		!CanBeCastedTo(NewType))
	{
		*this = FVoxelPinValue(NewType);
	}

	Fixup(Outer);
}

bool FVoxelPinValue::ImportFromUnrelated(FVoxelPinValue Other)
{
	VOXEL_FUNCTION_COUNTER();

	if (GetType() == Other.GetType())
	{
		*this = Other;
		return true;
	}

	if (Other.Is<FColor>())
	{
		Other = Make(FLinearColor(
			Other.Get<FColor>().R,
			Other.Get<FColor>().G,
			Other.Get<FColor>().B,
			Other.Get<FColor>().A));
	}
	if (Other.Is<FQuat>())
	{
		Other = Make(Other.Get<FQuat>().Rotator());
	}
	if (Other.Is<FRotator>())
	{
		Other = Make(FVector(
			Other.Get<FRotator>().Pitch,
			Other.Get<FRotator>().Yaw,
			Other.Get<FRotator>().Roll));
	}
	if (Other.Is<int32>())
	{
		Other = Make<float>(Other.Get<int32>());
	}
	if (Other.Is<FIntPoint>())
	{
		Other = Make(FVector2D(
			Other.Get<FIntPoint>().X,
			Other.Get<FIntPoint>().Y));
	}
	if (Other.Is<FIntVector>())
	{
		Other = Make(FVector(
			Other.Get<FIntVector>().X,
			Other.Get<FIntVector>().Y,
			Other.Get<FIntVector>().Z));
	}

#define CHECK(NewType, OldType, ...) \
	if (Is<NewType>() && Other.Is<OldType>()) \
	{ \
		const OldType Value = Other.Get<OldType>(); \
		Get<NewType>() = __VA_ARGS__; \
		return true; \
	}

	CHECK(float, FVector2D, Value.X);
	CHECK(float, FVector, Value.X);
	CHECK(float, FLinearColor, Value.R);

	CHECK(FVector2D, float, FVector2D(Value));
	CHECK(FVector2D, FVector, FVector2D(Value));
	CHECK(FVector2D, FLinearColor, FVector2D(Value));

	CHECK(FVector, float, FVector(Value));
	CHECK(FVector, FVector2D, FVector(Value, 0.f));
	CHECK(FVector, FLinearColor, FVector(Value));

	CHECK(FLinearColor, float, FLinearColor(Value, Value, Value, Value));
	CHECK(FLinearColor, FVector2D, FLinearColor(Value.X, Value.Y, 0.f));
	CHECK(FLinearColor, FVector, FLinearColor(Value));

	CHECK(int32, float, Value);
	CHECK(int32, FVector2D, Value.X);
	CHECK(int32, FVector, Value.X);
	CHECK(int32, FLinearColor, Value.R);

	CHECK(FIntPoint, float, FIntPoint(Value));
	CHECK(FIntPoint, FVector2D, FIntPoint(Value.X, Value.Y));
	CHECK(FIntPoint, FVector, FIntPoint(Value.X, Value.Y));
	CHECK(FIntPoint, FLinearColor, FIntPoint(Value.R, Value.G));

	CHECK(FIntVector, float, FIntVector(Value));
	CHECK(FIntVector, FVector2D, FIntVector(Value.X, Value.Y, 0));
	CHECK(FIntVector, FVector, FIntVector(Value.X, Value.Y, Value.Z));
	CHECK(FIntVector, FLinearColor, FIntVector(Value.R, Value.G, Value.B));

#undef CHECK

	return ImportFromString(Other.ExportToString());
}

void FVoxelPinValue::PostSerialize(const FArchive& Ar)
{
	if (Enum_DEPRECATED != 0)
	{
		Byte = Enum_DEPRECATED;
	}
}

FVoxelTerminalPinValue FVoxelPinValue::AsTerminalValue() const
{
	if (!ensure(!IsArray()))
	{
		return {};
	}

	return FVoxelTerminalPinValue(FVoxelPinValueBase(*this));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelPinValue::InitializeFromPinDefaultValue(const UEdGraphPin& Pin)
{
	if (Pin.DefaultObject)
	{
		ensure(Pin.DefaultValue.IsEmpty());

		if (IsClass())
		{
			GetClass() = Cast<UClass>(Pin.DefaultObject);
		}
		else if (IsObject())
		{
			GetObject() = Pin.DefaultObject;
		}
		else
		{
			ensure(false);
		}
	}
	else if (!Pin.DefaultValue.IsEmpty())
	{
		ensure(!IsObject());
		ensure(ImportFromString(Pin.DefaultValue));
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FString FVoxelPinValue::ExportToString_Array() const
{
	check(IsArray());
	ensure(false);
	return {};
}

void FVoxelPinValue::ExportToProperty_Array(const FProperty& Property, void* Memory) const
{
	check(IsArray());

	if (!ensure(Property.IsA<FArrayProperty>()))
	{
		return;
	}

	const FArrayProperty& ArrayProperty = CastFieldChecked<FArrayProperty>(Property);

	FScriptArrayHelper ArrayHelper(&ArrayProperty, Memory);
	ArrayHelper.Resize(Array.Num());
	for (int32 Index = 0; Index < Array.Num(); Index++)
	{
		Array[Index].ExportToProperty(*ArrayProperty.Inner, ArrayHelper.GetRawPtr(Index));
	}
}

bool FVoxelPinValue::ImportFromString_Array(const FString& Value)
{
	check(IsArray());
	ensure(false);
	return false;
}

uint32 FVoxelPinValue::GetHash_Array() const
{
	check(IsArray());

	if (Array.Num() == 0)
	{
		return 0;
	}

	return
		GetTypeHash(Array.Num()) ^
		GetTypeHash(Array[0]);
}

void FVoxelPinValue::Fixup_Array(UObject* Outer)
{
	for (FVoxelTerminalPinValue& Value : Array)
	{
		if (!Value.IsValid() ||
			!Value.CanBeCastedTo(Type.GetInnerType()))
		{
			Value = FVoxelTerminalPinValue(Type.GetInnerType());
		}
		Value.Fixup(Outer);
	}
}

bool FVoxelPinValue::Equal_Array(const FVoxelPinValueBase& Other) const
{
	check(IsArray());
	const FVoxelPinValue& OtherValue = static_cast<const FVoxelPinValue&>(Other);

	check(OtherValue.IsArray());
	return Array == OtherValue.Array;
}