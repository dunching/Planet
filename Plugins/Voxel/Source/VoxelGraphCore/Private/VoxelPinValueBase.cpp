// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelPinValueBase.h"
#include "VoxelPinValueInterface.h"
#include "Buffer/VoxelDoubleBuffers.h"
#include "EdGraph/EdGraphPin.h"
#include "Misc/DefaultValueHelper.h"

FVoxelPinValueBase::FVoxelPinValueBase(const FVoxelPinType& Type)
	: Type(Type)
{
	ensure(!Type.IsWildcard());

	if (Type.IsStruct())
	{
		Struct = FVoxelInstancedStruct(Type.GetStruct());
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelPinValueBase::ApplyToPinDefaultValue(UEdGraphPin& Pin) const
{
	ensure(!Type.IsBuffer());
	ensure(Type.HasPinDefaultValue());

	if (Type.IsClass())
	{
		Pin.DefaultValue.Reset();
		Pin.DefaultObject = GetClass();
	}
	else if (Type.IsObject())
	{
		Pin.DefaultValue.Reset();
		Pin.DefaultObject = GetObject();
	}
	else
	{
		Pin.DefaultValue = ExportToString();
		Pin.DefaultObject = nullptr;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FString FVoxelPinValueBase::ExportToString() const
{
	VOXEL_FUNCTION_COUNTER();

	if (!ensure(IsValid()))
	{
		return {};
	}

	if (Type.IsBuffer())
	{
		return ExportToString_Array();
	}

	switch (Type.GetInternalType())
	{
	default:
	{
		ensure(false);
		return {};
	}
	case EVoxelPinInternalType::Bool:
	{
		return LexToString(Get<bool>());
	}
	case EVoxelPinInternalType::Float:
	{
		return LexToString(Get<float>());
	}
	case EVoxelPinInternalType::Double:
	{
		return LexToString(Get<double>());
	}
	case EVoxelPinInternalType::Int32:
	{
		return LexToString(Get<int32>());
	}
	case EVoxelPinInternalType::Int64:
	{
		return LexToString(Get<int64>());
	}
	case EVoxelPinInternalType::Name:
	{
		return Get<FName>().ToString();
	}
	case EVoxelPinInternalType::Byte:
	{
		if (const UEnum* Enum = Type.GetEnum())
		{
			return Enum->GetNameStringByValue(Byte);
		}
		return LexToString(Get<uint8>());
	}
	case EVoxelPinInternalType::Class:
	{
		return FSoftObjectPath(GetClass()).ToString();
	}
	case EVoxelPinInternalType::Object:
	{
		return FSoftObjectPath(GetObject()).ToString();
	}
	case EVoxelPinInternalType::Struct:
	{
		if (Type.Is<FVector>())
		{
			const FVector Vector = Get<FVector>();
			return FString::Printf(TEXT("%f,%f,%f"), Vector.X, Vector.Y, Vector.Z);
		}
		else if (Type.Is<FRotator>())
		{
			const FRotator Rotator = Get<FRotator>();
			return FString::Printf(TEXT("P=%f,Y=%f,R=%f"), Rotator.Pitch, Rotator.Yaw, Rotator.Roll);
		}
		else if (Type.Is<FQuat>())
		{
			const FRotator Rotator = Get<FQuat>().Rotator();
			return FString::Printf(TEXT("P=%f,Y=%f,R=%f"), Rotator.Pitch, Rotator.Yaw, Rotator.Roll);
		}
		else if (Type.Is<FColor>())
		{
			const FColor& Color = Get<FColor>();
			return FString::Printf(TEXT("%d,%d,%d,%d"), Color.R, Color.G, Color.B, Color.A);
		}
		else if (Type.Is<FVoxelDoubleVector>())
		{
			const FVoxelDoubleVector& Vector = Get<FVoxelDoubleVector>();
			return FString::Printf(TEXT("%f,%f,%f"), Vector.X, Vector.Y, Vector.Z);
		}
		else
		{
			return FVoxelObjectUtilities::PropertyToText_Direct(
				*FVoxelObjectUtilities::MakeStructProperty(Type.GetStruct()),
				Struct.GetStructMemory(),
				nullptr);
		}
	}
	}
}

void FVoxelPinValueBase::ExportToProperty(const FProperty& Property, void* Memory) const
{
	VOXEL_FUNCTION_COUNTER();

	if (!ensure(IsValid()) ||
		// CanBeCastedTo_K2: ExportToProperty does implicit float -> double conversion
		!ensure(Type.CanBeCastedTo_K2(FVoxelPinType(Property))))
	{
		return;
	}

	if (Type.IsBuffer())
	{
		ExportToProperty_Array(Property, Memory);
		return;
	}

	switch (Type.GetInternalType())
	{
	default:
	{
		ensure(false);
	}
	break;
	case EVoxelPinInternalType::Bool:
	{
		if (!ensure(Property.IsA<FBoolProperty>()))
		{
			return;
		}

		CastFieldChecked<FBoolProperty>(Property).SetPropertyValue(Memory, Get<bool>());
	}
	break;
	case EVoxelPinInternalType::Float:
	{
		if (Property.IsA<FFloatProperty>())
		{
			CastFieldChecked<FFloatProperty>(Property).SetPropertyValue(Memory, Get<float>());
			return;
		}

		if (Property.IsA<FDoubleProperty>())
		{
			CastFieldChecked<FDoubleProperty>(Property).SetPropertyValue(Memory, Get<float>());
			return;
		}

		ensure(false);
		return;
	}
	break;
	case EVoxelPinInternalType::Double:
	{
		if (!ensure(Property.IsA<FDoubleProperty>()))
		{
			return;
		}

		CastFieldChecked<FDoubleProperty>(Property).SetPropertyValue(Memory, Get<double>());
	}
	break;
	case EVoxelPinInternalType::Int32:
	{
		if (!ensure(Property.IsA<FIntProperty>()))
		{
			return;
		}

		CastFieldChecked<FIntProperty>(Property).SetPropertyValue(Memory, Get<int32>());
	}
	break;
	case EVoxelPinInternalType::Int64:
	{
		if (!ensure(Property.IsA<FInt64Property>()))
		{
			return;
		}

		CastFieldChecked<FInt64Property>(Property).SetPropertyValue(Memory, Get<int64>());
	}
	break;
	case EVoxelPinInternalType::Name:
	{
		if (!ensure(Property.IsA<FNameProperty>()))
		{
			return;
		}

		CastFieldChecked<FNameProperty>(Property).SetPropertyValue(Memory, Get<FName>());
	}
	break;
	case EVoxelPinInternalType::Byte:
	{
		if (const FEnumProperty* EnumProperty = CastField<FEnumProperty>(Property))
		{
			if (!ensure(Type.GetEnum() == EnumProperty->GetEnum()) ||
				!ensure(EnumProperty->GetUnderlyingProperty()->IsA<FByteProperty>()))
			{
				return;
			}

			CastFieldChecked<FByteProperty>(EnumProperty->GetUnderlyingProperty())->SetPropertyValue(Memory, Get<uint8>());
		}
		else if (const FByteProperty* ByteProperty = CastField<FByteProperty>(Property))
		{
			if (!ensure(Type.GetEnum() == ByteProperty->Enum))
			{
				return;
			}

			ByteProperty->SetPropertyValue(Memory, Get<uint8>());
		}
		else
		{
			ensure(false);
		}
	}
	break;
	case EVoxelPinInternalType::Class:
	{
		if (Property.IsA<FClassProperty>())
		{
			const FClassProperty& ClassProperty = CastFieldChecked<FClassProperty>(Property);
			if (!ensure(Type.GetBaseClass()->IsChildOf(ClassProperty.MetaClass)))
			{
				return;
			}

			ensure(IsInGameThread() || IsInAsyncLoadingThread());
			ClassProperty.SetObjectPropertyValue(Memory, GetClass());
		}
		else if (Property.IsA<FSoftClassProperty>())
		{
			const FSoftClassProperty& ClassProperty = CastFieldChecked<FSoftClassProperty>(Property);
			if (!ensure(Type.GetBaseClass()->IsChildOf(ClassProperty.MetaClass)))
			{
				return;
			}

			ensure(IsInGameThread() || IsInAsyncLoadingThread());
			ClassProperty.SetObjectPropertyValue(Memory, GetClass());
		}
		else
		{
			ensure(false);
		}
	}
	break;
	case EVoxelPinInternalType::Object:
	{
		if (Property.IsA<FObjectProperty>())
		{
			const FObjectProperty& ObjectProperty = CastFieldChecked<FObjectProperty>(Property);
			if (!ensure(ObjectProperty.PropertyClass == Type.GetObjectClass()))
			{
				return;
			}

			ensure(IsInGameThread() || IsInAsyncLoadingThread());
			ObjectProperty.SetObjectPropertyValue(Memory, GetObject());
		}
		else if (Property.IsA<FSoftObjectProperty>())
		{
			const FSoftObjectProperty& ObjectProperty = CastFieldChecked<FSoftObjectProperty>(Property);
			if (!ensure(ObjectProperty.PropertyClass == Type.GetObjectClass()))
			{
				return;
			}

			ensure(IsInGameThread() || IsInAsyncLoadingThread());
			ObjectProperty.SetObjectPropertyValue(Memory, GetObject());
		}
		else
		{
			ensure(false);
		}
	}
	break;
	case EVoxelPinInternalType::Struct:
	{
		if (!ensure(Property.IsA<FStructProperty>()))
		{
			return;
		}

		const FStructProperty& StructProperty = CastFieldChecked<FStructProperty>(Property);
		if (!ensure(Type.GetStruct() == StructProperty.Struct) ||
			!ensure(Type.GetStruct() == Struct.GetScriptStruct()))
		{
			return;
		}

		GetStruct().CopyTo(Memory);
	}
	}
}

bool FVoxelPinValueBase::ImportFromString(const FString& Value)
{
	VOXEL_FUNCTION_COUNTER();

	if (!ensure(IsValid()))
	{
		return false;
	}

	if (Type.IsBuffer())
	{
		return ImportFromString_Array(Value);
	}

	switch (Type.GetInternalType())
	{
	default:
	{
		ensure(false);
		return false;
	}
	case EVoxelPinInternalType::Bool:
	{
		bBool = FCString::ToBool(*Value);
		return true;
	}
	case EVoxelPinInternalType::Float:
	{
		Float = FCString::Atof(*Value);
		return true;
	}
	case EVoxelPinInternalType::Double:
	{
		Double = FCString::Atod(*Value);
		return true;
	}
	case EVoxelPinInternalType::Int32:
	{
		Int32 = FCString::Atoi(*Value);
		return true;
	}
	case EVoxelPinInternalType::Int64:
	{
		Int64 = FCString::Atoi64(*Value);
		return true;
	}
	case EVoxelPinInternalType::Name:
	{
		Name = *Value;
		return true;
	}
	case EVoxelPinInternalType::Byte:
	{
		if (const UEnum* Enum = Type.GetEnum())
		{
			int64 EnumValue = Enum->GetValueByNameString(Value);
			if (EnumValue == -1)
			{
				EnumValue = FCString::Atoi(*Value);
			}
			if (EnumValue < 0 ||
				EnumValue > 255)
			{
				return false;
			}

			Byte = uint8(EnumValue);
			return true;
		}
		else
		{
			const int32 ByteValue = FCString::Atoi(*Value);
			if (ByteValue < 0 ||
				ByteValue > 255)
			{
				return false;
			}

			Byte = uint8(ByteValue);
			return true;
		}
	}
	case EVoxelPinInternalType::Class:
	{
		check(IsInGameThread());
		UClass* LoadedClass = Cast<UClass>(FSoftObjectPtr(Value).LoadSynchronous());

		if (LoadedClass &&
			!LoadedClass->IsChildOf(Type.GetBaseClass()))
		{
			return false;
		}

		Class = LoadedClass;
		return true;
	}
	case EVoxelPinInternalType::Object:
	{
		check(IsInGameThread());
		UObject* LoadedObject = FSoftObjectPtr(Value).LoadSynchronous();

		if (LoadedObject &&
			!LoadedObject->IsA(Type.GetObjectClass()))
		{
			return false;
		}

		Object = LoadedObject;
		return true;
	}
	case EVoxelPinInternalType::Struct:
	{
		Struct = FVoxelInstancedStruct(Type.GetStruct());

		if (Value.IsEmpty())
		{
			return true;
		}

#define CHECK(InType, ...) \
		if (Type.Is<InType>() && \
			Value == TEXT(#__VA_ARGS__)) \
		{ \
			Get<InType>() = __VA_ARGS__; \
			return true; \
		}

		if (Type.Is<FVector>())
		{
			CHECK(FVector, FVector::ZeroVector);
			CHECK(FVector, FVector::OneVector);

			if (Value.StartsWith(TEXT("FVector(")) &&
				Value.EndsWith(TEXT(")")))
			{
				FString Inner = Value;
				ensure(Inner.RemoveFromStart(TEXT("FVector(")));
				ensure(Inner.RemoveFromEnd(TEXT(")")));
				if (!FVoxelUtilities::IsFloat(Inner))
				{
					return false;
				}

				Get<FVector>() = FVector(FVoxelUtilities::Atof(Inner));
				return true;
			}

			return FDefaultValueHelper::ParseVector(Value, Get<FVector>());
		}
		else if (Type.Is<FVector2D>())
		{
			CHECK(FVector2D, FVector2D::ZeroVector);
			CHECK(FVector2D, FVector2D::One());

			return FDefaultValueHelper::ParseVector2D(Value, Get<FVector2D>());
		}
		else if (Type.Is<FRotator>())
		{
			CHECK(FRotator, FRotator::ZeroRotator);

			return FDefaultValueHelper::ParseRotator(Value, Get<FRotator>());
		}
		else if (Type.Is<FQuat>())
		{
			CHECK(FQuat, FQuat::Identity);

			FRotator Rotator;
			if (!FDefaultValueHelper::ParseRotator(Value, Rotator))
			{
				return false;
			}

			Get<FQuat>() = Rotator.Quaternion();
			return true;
		}
		else if (Type.Is<FColor>())
		{
			CHECK(FColor, FColor::Black);
			CHECK(FColor, FColor::White);

			return FDefaultValueHelper::ParseColor(Value, Get<FColor>());
		}
		else if (Type.Is<FVoxelDoubleVector>())
		{
			return FDefaultValueHelper::ParseVector(Value, ReinterpretCastRef<FVector>(Get<FVoxelDoubleVector>()));
		}
		else if (Type.Is<FVoxelDoubleVector2D>())
		{
			return FDefaultValueHelper::ParseVector2D(Value, ReinterpretCastRef<FVector2D>(Get<FVoxelDoubleVector2D>()));
		}
		else
		{
			CHECK(FVector4, FVector4::Zero());
			CHECK(FVector4, FVector4::One());

			return FVoxelObjectUtilities::PropertyFromText_Direct(
				*FVoxelObjectUtilities::MakeStructProperty(Struct.GetScriptStruct()),
				Value,
				Struct.GetStructMemory(),
				nullptr);
		}

#undef CHECK
	}
	}
}

uint32 FVoxelPinValueBase::GetHash() const
{
	if (!IsValid())
	{
		return 0;
	}

	if (Type.IsBuffer())
	{
		return GetHash_Array();
	}

	switch (Type.GetInternalType())
	{
	default:
	{
		ensure(false);
		return 0;
	}
	case EVoxelPinInternalType::Bool:
	{
		return GetTypeHash(Get<bool>());
	}
	case EVoxelPinInternalType::Float:
	{
		return GetTypeHash(Get<float>());
	}
	case EVoxelPinInternalType::Double:
	{
		return GetTypeHash(Get<double>());
	}
	case EVoxelPinInternalType::Int32:
	{
		return GetTypeHash(Get<int32>());
	}
	case EVoxelPinInternalType::Int64:
	{
		return GetTypeHash(Get<int64>());
	}
	case EVoxelPinInternalType::Name:
	{
		return GetTypeHash(Get<FName>());
	}
	case EVoxelPinInternalType::Byte:
	{
		return GetTypeHash(Get<uint8>());
	}
	case EVoxelPinInternalType::Class:
	{
		return GetTypeHash(GetClass());
	}
	case EVoxelPinInternalType::Object:
	{
		return GetTypeHash(GetObject());
	}
	case EVoxelPinInternalType::Struct:
	{
		if (!Struct.IsValid())
		{
			return 0;
		}

		return Struct.GetScriptStruct()->GetStructTypeHash(Struct.GetStructMemory());
	}
	}
}

void FVoxelPinValueBase::Fixup(UObject* Outer)
{
	if (Is<FBodyInstance>())
	{
		Get<FBodyInstance>().LoadProfileData(false);
	}

	if (CanBeCastedTo<FVoxelPinValueInterface>())
	{
		Get<FVoxelPinValueInterface>().Fixup(Outer);
	}

	if (Is<uint8>())
	{
		if (const UEnum* Enum = Type.GetEnum())
		{
			if (!Enum->IsValidEnumValue(Byte))
			{
				Byte = Enum->GetValueByIndex(0);
			}
			else if (
				Enum->NumEnums() > 1 &&
				Byte == Enum->GetMaxEnumValue())
			{
				Byte = Enum->GetValueByIndex(0);
			}
		}
	}

	if (Type.IsBuffer())
	{
		Fixup_Array(Outer);
	}
}

bool FVoxelPinValueBase::IsValid() const
{
	if (!Type.IsValid())
	{
		return false;
	}

	if (!HasArray() &&
		!ensure(!Type.IsBuffer()))
	{
		return false;
	}

	if (Type.IsStruct())
	{
		return ensure(Type.GetStruct() == Struct.GetScriptStruct());
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelPinValueBase FVoxelPinValueBase::MakeStruct(const FConstVoxelStructView Struct)
{
	check(Struct.IsValid());

	FVoxelPinValueBase Result;
	Result.Type = FVoxelPinType::MakeStruct(Struct.GetStruct());
	Result.Struct = Struct.MakeInstancedStruct();
	return Result;
}

FVoxelPinValueBase FVoxelPinValueBase::MakeFromProperty(const FProperty& Property, const void* Memory)
{
	FVoxelPinValueBase Result = FVoxelPinValueBase(FVoxelPinType(Property));
	if (!ensure(!Result.Type.IsBuffer()))
	{
		return {};
	}

	if (Property.IsA<FBoolProperty>())
	{
		Result.Get<bool>() = CastFieldChecked<FBoolProperty>(Property).GetPropertyValue(Memory);
	}
	else if (Property.IsA<FFloatProperty>())
	{
		Result.Get<float>() = CastFieldChecked<FFloatProperty>(Property).GetPropertyValue(Memory);
	}
	else if (Property.IsA<FDoubleProperty>())
	{
		Result.Get<double>() = CastFieldChecked<FDoubleProperty>(Property).GetPropertyValue(Memory);
	}
	else if (Property.IsA<FIntProperty>())
	{
		Result.Get<int32>() = CastFieldChecked<FIntProperty>(Property).GetPropertyValue(Memory);
	}
	else if (Property.IsA<FInt64Property>())
	{
		Result.Get<int64>() = CastFieldChecked<FInt64Property>(Property).GetPropertyValue(Memory);
	}
	else if (Property.IsA<FNameProperty>())
	{
		Result.Get<FName>() = CastFieldChecked<FNameProperty>(Property).GetPropertyValue(Memory);
	}
	else if (Property.IsA<FByteProperty>())
	{
		Result.Get<uint8>() = CastFieldChecked<FByteProperty>(Property).GetPropertyValue(Memory);
	}
	else if (Property.IsA<FEnumProperty>())
	{
		FNumericProperty* UnderlyingProperty = CastFieldChecked<FEnumProperty>(Property).GetUnderlyingProperty();
		if (!ensure(UnderlyingProperty->IsA<FByteProperty>()))
		{
			return {};
		}
		Result.Get<uint8>() = CastFieldChecked<FByteProperty>(UnderlyingProperty)->GetPropertyValue(Memory);
	}
	else if (Property.IsA<FClassProperty>())
	{
		const FClassProperty& ClassProperty = CastFieldChecked<FClassProperty>(Property);
		ensure(ClassProperty.MetaClass == Result.Type.GetBaseClass());

		Result.GetClass() = Cast<UClass>(ClassProperty.GetObjectPropertyValue(Memory));
	}
	else if (Property.IsA<FSoftClassProperty>())
	{
		const FSoftClassProperty& ClassProperty = CastFieldChecked<FSoftClassProperty>(Property);
		ensure(ClassProperty.MetaClass == Result.Type.GetBaseClass());

		Result.GetClass() = Cast<UClass>(ClassProperty.GetObjectPropertyValue(Memory));
	}
	else if (Property.IsA<FObjectProperty>())
	{
		const FObjectProperty& ObjectProperty = CastFieldChecked<FObjectProperty>(Property);
		ensure(ObjectProperty.PropertyClass == Result.Type.GetObjectClass());

		Result.GetObject() = ObjectProperty.GetObjectPropertyValue(Memory);
	}
	else if (Property.IsA<FSoftObjectProperty>())
	{
		const FSoftObjectProperty& ObjectProperty = CastFieldChecked<FSoftObjectProperty>(Property);
		ensure(ObjectProperty.PropertyClass == Result.Type.GetObjectClass());

		Result.GetObject() = ObjectProperty.GetObjectPropertyValue(Memory);
	}
	else if (Property.IsA<FStructProperty>())
	{
		const UScriptStruct* Struct = CastFieldChecked<FStructProperty>(Property).Struct;
		ensure(Result.GetStruct().GetScriptStruct() == Struct);
		Result.GetStruct().CopyFrom(Memory);
	}
	else
	{
		ensure(false);
	}

	return Result;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool FVoxelPinValueBase::operator==(const FVoxelPinValueBase& Other) const
{
	if (Type != Other.Type)
	{
		return false;
	}

	if (!Type.IsValid())
	{
		return true;
	}
	if (!ensure(IsValid()) ||
		!ensure(Other.IsValid()))
	{
		return false;
	}

	if (Type.IsBuffer())
	{
		return Equal_Array(Other);
	}

	switch (Type.GetInternalType())
	{
	default:
	{
		ensure(false);
		return false;
	}
	case EVoxelPinInternalType::Bool:
	{
		return Get<bool>() == Other.Get<bool>();
	}
	case EVoxelPinInternalType::Float:
	{
		return Get<float>() == Other.Get<float>();
	}
	case EVoxelPinInternalType::Double:
	{
		return Get<double>() == Other.Get<double>();
	}
	case EVoxelPinInternalType::Int32:
	{
		return Get<int32>() == Other.Get<int32>();
	}
	case EVoxelPinInternalType::Int64:
	{
		return Get<int64>() == Other.Get<int64>();
	}
	case EVoxelPinInternalType::Name:
	{
		return Get<FName>() == Other.Get<FName>();
	}
	case EVoxelPinInternalType::Byte:
	{
		return Get<uint8>() == Other.Get<uint8>();
	}
	case EVoxelPinInternalType::Class:
	{
		ensure(IsInGameThread() || IsInAsyncLoadingThread());
		return GetClass() == Other.GetClass();
	}
	case EVoxelPinInternalType::Object:
	{
		ensure(IsInGameThread() || IsInAsyncLoadingThread());
		return GetObject() == Other.GetObject();
	}
	case EVoxelPinInternalType::Struct:
	{
		return GetStruct() == Other.GetStruct();
	}
	}
}