// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelPinType.h"
#include "VoxelNode.h"
#include "VoxelSurface.h"
#include "VoxelExposedSeed.h"
#include "VoxelBufferBuilder.h"
#include "VoxelObjectPinType.h"
#include "VoxelFunctionLibrary.h"
#include "VoxelPinValueInterface.h"
#include "Buffer/VoxelBaseBuffers.h"
#include "VoxelChannelAsset_DEPRECATED.h"
#include "Engine/Texture2D.h"
#include "EdGraph/EdGraphPin.h"

#if WITH_EDITOR && VOXEL_DEBUG
VOXEL_RUN_ON_STARTUP_EDITOR(TestVoxelPinTypes)
{
	{
		FVoxelPinValue Value(FVoxelPinType::Make<TSubclassOf<UObject>>());
		Value.Get<TSubclassOf<UObject>>() = AActor::StaticClass();
		ensure(Value.Get<TSubclassOf<AActor>>() == AActor::StaticClass());
		ensure(Value.Get<TSubclassOf<AVolume>>() == nullptr);
	}

	for (const FVoxelPinType& Type : FVoxelPinTypeSet::All().GetTypes())
	{
		const FVoxelPinValue ExposedValue(Type.GetExposedType());
		const FVoxelRuntimePinValue RuntimeValue = FVoxelPinType::MakeRuntimeValue(Type, ExposedValue);
		ensure(FVoxelPinType::MakeExposedValue(RuntimeValue, Type.IsBufferArray()) == ExposedValue);

		const FEdGraphPinType GraphType = Type.GetEdGraphPinType();
		ensure(FVoxelPinType(GraphType) == Type);

		// Array flag is not preserved when going through K2
		// K2 only support doubles
		if (!Type.IsBufferArray() &&
			!Type.GetInnerType().Is<float>())
		{
			const FEdGraphPinType K2Type = Type.GetEdGraphPinType_K2();
			ensure(FVoxelPinType::MakeFromK2(K2Type) == Type);
		}
	}

	TArray<FDateTime> Times;
	Times.Emplace_GetRef() = FDateTime::FromJulianDay(1);
	Times.Emplace_GetRef() = FDateTime::FromJulianDay(2);
	Times.Emplace_GetRef() = FDateTime::FromJulianDay(3);

	TUniquePtr<FStructProperty> StructProperty = FVoxelObjectUtilities::MakeStructProperty<FDateTime>();
	const TUniquePtr<FArrayProperty> ArrayProperty = FVoxelObjectUtilities::MakeArrayProperty(StructProperty.Release());

	const FVoxelPinValue Value = FVoxelPinValue::MakeFromProperty(*ArrayProperty, &Times);
	const FVoxelRuntimePinValue RuntimeValue = FVoxelPinType::MakeRuntimeValue(
		FVoxelPinType::Make<FDateTime>().GetBufferType().WithBufferArray(true),
		Value);
	const FVoxelPinValue ExposedValue = FVoxelPinType::MakeExposedValue(RuntimeValue, true);
	ensure(Value == ExposedValue);

	TArray<FDateTime> NewTimes;
	ExposedValue.ExportToProperty(*ArrayProperty, &NewTimes);

	ensure(Times == NewTimes);
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelPinType::FVoxelPinType(const FEdGraphPinType& PinType)
{
	if (PinType == FEdGraphPinType())
	{
		// Empty type
		return;
	}

	const int64 Value = StaticEnumFast<EVoxelPinInternalType>()->GetValueByNameString(PinType.PinCategory.ToString());
	if (Value != -1)
	{
		ensure(
			!PinType.PinSubCategoryObject.IsValid() ||
			PinType.PinSubCategoryObject->IsA<UField>());

		InternalType = EVoxelPinInternalType(Value);
		bIsBuffer = PinType.IsArray();
		bIsBufferArray = PinType.PinSubCategory == STATIC_FNAME("Array") && ensure(bIsBuffer);
		PrivateInternalField = Cast<UField>(PinType.PinSubCategoryObject.Get());
	}
	else
	{
		// Legacy load

		if (PinType.PinCategory == GetClassFName<FBoolProperty>())
		{
			InternalType = EVoxelPinInternalType::Bool;
		}
		else if (PinType.PinCategory == GetClassFName<FFloatProperty>())
		{
			InternalType = EVoxelPinInternalType::Float;
		}
		else if (PinType.PinCategory == GetClassFName<FDoubleProperty>())
		{
			InternalType = EVoxelPinInternalType::Double;
		}
		else if (PinType.PinCategory == GetClassFName<FIntProperty>())
		{
			InternalType = EVoxelPinInternalType::Int32;
		}
		else if (PinType.PinCategory == GetClassFName<FInt64Property>())
		{
			InternalType = EVoxelPinInternalType::Int64;
		}
		else if (PinType.PinCategory == GetClassFName<FNameProperty>())
		{
			InternalType = EVoxelPinInternalType::Name;
		}
		else if (
			PinType.PinCategory == GetClassFName<FByteProperty>() ||
			PinType.PinCategory == GetClassFName<FEnumProperty>())
		{
			InternalType = EVoxelPinInternalType::Byte;
			PrivateInternalField = Cast<UField>(PinType.PinSubCategoryObject.Get());
		}
		else if (
			PinType.PinCategory == GetClassFName<FClassProperty>() ||
			PinType.PinCategory == GetClassFName<FSoftClassProperty>())
		{
			InternalType = EVoxelPinInternalType::Class;
			PrivateInternalField = Cast<UField>(PinType.PinSubCategoryObject.Get());
		}
		else if (
			PinType.PinCategory == GetClassFName<FObjectProperty>() ||
			PinType.PinCategory == GetClassFName<FSoftObjectProperty>())
		{
			InternalType = EVoxelPinInternalType::Object;
			PrivateInternalField = Cast<UField>(PinType.PinSubCategoryObject.Get());
		}
		else if (
			PinType.PinCategory == GetClassFName<FStructProperty>() || (
			PinType.PinCategory.IsNone() &&
			PinType.PinSubCategoryObject.IsValid() &&
			PinType.PinSubCategoryObject->IsA<UScriptStruct>()))
		{
			if (PinType.PinSubCategoryObject == FVoxelWildcard_DEPRECATED::StaticStruct())
			{
				InternalType = EVoxelPinInternalType::Wildcard;
			}
			else if (PinType.PinSubCategoryObject == FVoxelWildcardBuffer_DEPRECATED::StaticStruct())
			{
				InternalType = EVoxelPinInternalType::Wildcard;
				bIsBuffer = true;
			}
			else
			{
				UScriptStruct* Struct = Cast<UScriptStruct>(PinType.PinSubCategoryObject.Get());
				if (ensure(Struct))
				{
					if (Struct->IsChildOf(FVoxelBuffer::StaticStruct()))
					{
						*this = TVoxelInstancedStruct<FVoxelBuffer>(Struct)->GetInnerType().GetBufferType();
					}
					else
					{
						*this = MakeStruct(Struct);
					}
				}
			}
		}
		else
		{
			ensure(false);
		}

		if (PinType.PinSubCategory == "Seed")
		{
			if (IsBuffer())
			{
				*this = Make<FVoxelSeedBuffer>();
			}
			else
			{
				*this = Make<FVoxelSeed>();
			}
		}
		else if (!PinType.PinSubCategory.IsNone())
		{
			static TMap<FName, UEnum*> EnumToName;
			if (EnumToName.Num() == 0)
			{
				ForEachObjectOfClass<UEnum>([&](UEnum* Enum)
				{
					EnumToName.Add(Enum->GetFName(), Enum);
				});
			}

			UEnum* Enum = EnumToName.FindRef(PinType.PinSubCategory);
			if (ensure(Enum))
			{
				*this = MakeEnum(Enum);
			}
		}
	}

	if (GetInnerType().Is<FVoxelDistance_DEPRECATED>())
	{
		if (IsBuffer())
		{
			*this = Make<FVoxelSurface>();
		}
		else
		{
			*this = Make<float>();
		}
	}

	ensureVoxelSlow(IsValid());
}

FVoxelPinType::FVoxelPinType(const FProperty& Property)
{
	if (Property.IsA<FArrayProperty>())
	{
		*this = FVoxelPinType(*CastFieldChecked<FArrayProperty>(Property).Inner);
		bIsBuffer = true;
		// If it's from a property, it's likely from BP and thus an array
		bIsBufferArray = true;
	}
	else if (Property.IsA<FBoolProperty>())
	{
		InternalType = EVoxelPinInternalType::Bool;
		ensure(CastFieldChecked<FBoolProperty>(Property).IsNativeBool());
	}
	else if (Property.IsA<FFloatProperty>())
	{
		InternalType = EVoxelPinInternalType::Float;
	}
	else if (Property.IsA<FDoubleProperty>())
	{
		InternalType = EVoxelPinInternalType::Double;
	}
	else if (Property.IsA<FIntProperty>())
	{
		InternalType = EVoxelPinInternalType::Int32;
	}
	else if (Property.IsA<FInt64Property>())
	{
		InternalType = EVoxelPinInternalType::Int64;
	}
	else if (Property.IsA<FNameProperty>())
	{
		InternalType = EVoxelPinInternalType::Name;
	}
	else if (Property.IsA<FByteProperty>())
	{
		InternalType = EVoxelPinInternalType::Byte;
		PrivateInternalField = CastFieldChecked<FByteProperty>(Property).Enum;
	}
	else if (Property.IsA<FEnumProperty>())
	{
		InternalType = EVoxelPinInternalType::Byte;
		PrivateInternalField = CastFieldChecked<FEnumProperty>(Property).GetEnum();
	}
	else if (Property.IsA<FClassProperty>())
	{
		InternalType = EVoxelPinInternalType::Class;
		PrivateInternalField = CastFieldChecked<FClassProperty>(Property).MetaClass;
	}
	else if (Property.IsA<FSoftClassProperty>())
	{
		InternalType = EVoxelPinInternalType::Class;
		PrivateInternalField = CastFieldChecked<FSoftClassProperty>(Property).MetaClass;
	}
	else if (Property.IsA<FObjectProperty>())
	{
		InternalType = EVoxelPinInternalType::Object;
		PrivateInternalField = CastFieldChecked<FObjectProperty>(Property).PropertyClass;
	}
	else if (Property.IsA<FSoftObjectProperty>())
	{
		InternalType = EVoxelPinInternalType::Object;
		PrivateInternalField = CastFieldChecked<FSoftObjectProperty>(Property).PropertyClass;
	}
	else if (Property.IsA<FStructProperty>())
	{
		*this = MakeStruct(CastFieldChecked<FStructProperty>(Property).Struct);
	}
	else
	{
		ensure(false);
	}

	ensure(IsValid());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelPinType FVoxelPinType::MakeStruct(UScriptStruct* Struct)
{
	checkVoxelSlow(Struct);
	checkVoxelSlow(Struct != StaticStructFast<FVoxelPinType>());
	checkVoxelSlow(Struct != StaticStructFast<FVoxelPinValue>());
	checkVoxelSlow(Struct != StaticStructFast<FVoxelNameWrapper>());
	checkVoxelSlow(Struct != StaticStructFast<FVoxelRuntimePinValue>());
	checkVoxelSlow(!Struct->IsChildOf(StaticStructFast<FVoxelBuffer>()));

	return MakeImpl(EVoxelPinInternalType::Struct, Struct);
}

FVoxelPinType FVoxelPinType::MakeFromK2(const FEdGraphPinType& PinType)
{
	// From UEdGraphSchema_K2

	FVoxelPinType Type = INLINE_LAMBDA -> FVoxelPinType
	{
		if (PinType.PinCategory == STATIC_FNAME("wildcard"))
		{
			return MakeWildcard();
		}
		else if (PinType.PinCategory == STATIC_FNAME("bool"))
		{
			return Make<bool>();
		}
		else if (PinType.PinCategory == STATIC_FNAME("real"))
		{
			if (PinType.PinSubCategory == STATIC_FNAME("float"))
			{
				return Make<float>();
			}
			else
			{
				ensure(PinType.PinSubCategory == STATIC_FNAME("double"));
				return Make<double>();
			}
		}
		else if (PinType.PinCategory == STATIC_FNAME("int"))
		{
			return Make<int32>();
		}
		else if (PinType.PinCategory == STATIC_FNAME("int64"))
		{
			return Make<int64>();
		}
		else if (PinType.PinCategory == STATIC_FNAME("name"))
		{
			return Make<FName>();
		}
		else if (PinType.PinCategory == STATIC_FNAME("byte"))
		{
			if (UEnum* EnumType = Cast<UEnum>(PinType.PinSubCategoryObject.Get()))
			{
				return MakeEnum(EnumType);
			}
			else
			{
				return Make<uint8>();
			}
		}
		else if (PinType.PinCategory == STATIC_FNAME("class"))
		{
			if (UClass* ClassType = Cast<UClass>(PinType.PinSubCategoryObject.Get()))
			{
				return MakeClass(ClassType);
			}
			else
			{
				return {};
			}
		}
		else if (
			PinType.PinCategory == STATIC_FNAME("object") ||
			PinType.PinCategory == STATIC_FNAME("interface"))
		{
			if (UClass* ObjectType = Cast<UClass>(PinType.PinSubCategoryObject.Get()))
			{
				return MakeObject(ObjectType);
			}
			else
			{
				return {};
			}
		}
		else if (PinType.PinCategory == STATIC_FNAME("struct"))
		{
			if (UScriptStruct* StructType = Cast<UScriptStruct>(PinType.PinSubCategoryObject.Get()))
			{
				return MakeStruct(StructType);
			}
			else
			{
				return {};
			}
		}
		else
		{
			return {};
		}
	};

	Type.bIsBuffer = PinType.IsArray();
	return Type;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool FVoxelPinType::IsValid() const
{
	switch (InternalType)
	{
	case EVoxelPinInternalType::Invalid:
	{
		ensure(!GetInternalField());
		return false;
	}
	case EVoxelPinInternalType::Wildcard:
	{
		ensure(!GetInternalField());
		return true;
	}
	case EVoxelPinInternalType::Bool:
	case EVoxelPinInternalType::Float:
	case EVoxelPinInternalType::Double:
	case EVoxelPinInternalType::Int32:
	case EVoxelPinInternalType::Int64:
	case EVoxelPinInternalType::Name:
	{
		return ensure(!GetInternalField());
	}
	case EVoxelPinInternalType::Byte:
	{
		if (!GetInternalField())
		{
			return true;
		}

		return ensureVoxelSlow(Cast<UEnum>(GetInternalField()));
	}
	case EVoxelPinInternalType::Class:
	{
		return ensureVoxelSlow(Cast<UClass>(GetInternalField()));
	}
	case EVoxelPinInternalType::Object:
	{
		return ensureVoxelSlow(Cast<UClass>(GetInternalField()));
	}
	case EVoxelPinInternalType::Struct:
	{
		const UScriptStruct* Struct = Cast<UScriptStruct>(GetInternalField());
		return
			ensureVoxelSlow(Struct) &&
			ensureVoxelSlow(!Struct->IsChildOf(StaticStructFast<FVoxelBuffer>()));
	}
	default:
	{
		ensure(false);
		return false;
	}
	}
}

FString FVoxelPinType::ToString() const
{
	if (!ensureVoxelSlow(IsValid()))
	{
		return "INVALID";
	}

	if (IsBuffer())
	{
		FString Name = GetInnerType().ToString();
		if (IsBufferArray())
		{
			Name += " Array";
		}
		else
		{
			Name += " Buffer";
		}
		return Name;
	}

	FString Name = INLINE_LAMBDA -> FString
	{
		switch (InternalType)
		{
		default: ensure(false);
		case EVoxelPinInternalType::Wildcard: return "Wildcard";
		case EVoxelPinInternalType::Bool: return "Boolean";
		case EVoxelPinInternalType::Float: return "Float";
		case EVoxelPinInternalType::Double: return "Double";
		case EVoxelPinInternalType::Int32: return "Integer";
		case EVoxelPinInternalType::Int64: return "Integer 64";
		case EVoxelPinInternalType::Name: return "Name";
		case EVoxelPinInternalType::Byte:
		{
			if (const UEnum* Enum = GetEnum())
			{
	#if WITH_EDITOR
				return Enum->GetDisplayNameText().ToString();
	#else
				return Enum->GetName();
	#endif
			}
			else
			{
				return "Byte";
			}
		}
		case EVoxelPinInternalType::Class:
		{
	#if WITH_EDITOR
			return GetBaseClass()->GetDisplayNameText().ToString() + " Class";
	#else
			return GetBaseClass()->GetName() + " Class";
	#endif
		}
		case EVoxelPinInternalType::Object:
		{
	#if WITH_EDITOR
			return GetObjectClass()->GetDisplayNameText().ToString();
	#else
			return GetObjectClass()->GetName();
	#endif
		}
		case EVoxelPinInternalType::Struct:
		{
	#if WITH_EDITOR
			return GetStruct()->GetDisplayNameText().ToString();
	#else
			return GetStruct()->GetName();
	#endif
		}
		}
	};

	Name.RemoveFromStart("Voxel ");
	Name.RemoveFromStart("EVoxel ");
	return Name;
}

FEdGraphPinType FVoxelPinType::GetEdGraphPinType() const
{
	FEdGraphPinType PinType;
	PinType.PinCategory = UEnum::GetValueAsName(InternalType);
	PinType.ContainerType = IsBuffer() ? EPinContainerType::Array : EPinContainerType::None;
	PinType.PinSubCategory = IsBufferArray() ? STATIC_FNAME("Array") : FName();
	PinType.PinSubCategoryObject = GetInternalField();
	return PinType;
}

FEdGraphPinType FVoxelPinType::GetEdGraphPinType_K2() const
{
	if (!IsValid())
	{
		return {};
	}

	// From UEdGraphSchema_K2

	FEdGraphPinType PinType;
	PinType.ContainerType = IsBuffer() ? EPinContainerType::Array : EPinContainerType::None;

	switch (InternalType)
	{
	case EVoxelPinInternalType::Wildcard:
	{
		PinType.PinCategory = STATIC_FNAME("wildcard");
		return PinType;
	}
	case EVoxelPinInternalType::Bool:
	{
		PinType.PinCategory = STATIC_FNAME("bool");
		return PinType;
	}
	// Always use double with blueprints
	case EVoxelPinInternalType::Float:
	case EVoxelPinInternalType::Double:
	{
		PinType.PinCategory = STATIC_FNAME("real");
		PinType.PinSubCategory = STATIC_FNAME("double");
		return PinType;
	}
	case EVoxelPinInternalType::Int32:
	{
		PinType.PinCategory = STATIC_FNAME("int");
		return PinType;
	}
	case EVoxelPinInternalType::Int64:
	{
		PinType.PinCategory = STATIC_FNAME("int64");
		return PinType;
	}
	case EVoxelPinInternalType::Name:
	{
		PinType.PinCategory = STATIC_FNAME("name");
		return PinType;
	}
	case EVoxelPinInternalType::Byte:
	{
		PinType.PinCategory = STATIC_FNAME("byte");
		PinType.PinSubCategoryObject = GetInnerType().GetEnum();
		return PinType;
	}
	case EVoxelPinInternalType::Class:
	{
		PinType.PinCategory = STATIC_FNAME("class");
		PinType.PinSubCategoryObject = GetInnerType().GetBaseClass();
		return PinType;
	}
	case EVoxelPinInternalType::Object:
	{
		PinType.PinCategory = STATIC_FNAME("object");
		PinType.PinSubCategoryObject = GetInnerType().GetObjectClass();
		return PinType;
	}
	case EVoxelPinInternalType::Struct:
	{
		PinType.PinCategory = STATIC_FNAME("struct");
		PinType.PinSubCategoryObject = GetInnerType().GetStruct();
		return PinType;
	}
	default:
	{
		ensure(false);
		PinType.PinCategory = STATIC_FNAME("wildcard");
		return PinType;
	}
	}
}

bool FVoxelPinType::HasPinDefaultValue() const
{
	// No default for wildcards or arrays
	return
		!IsWildcard() &&
		!IsBufferArray();
}

bool FVoxelPinType::CanBeCastedTo(const FVoxelPinType& Other) const
{
	ensureVoxelSlowNoSideEffects(IsValid());
	ensureVoxelSlowNoSideEffects(Other.IsValid());

	if (*this == Other)
	{
		return true;
	}

	if (InternalType != Other.InternalType ||
		bIsBuffer != Other.bIsBuffer)
	{
		return false;
	}

	switch (InternalType)
	{
	default:
	{
		return
			ensureVoxelSlow(!GetInternalField()) &&
			ensureVoxelSlow(!Other.GetInternalField());
	}
	case EVoxelPinInternalType::Byte:
	{
		// Enums can be casted to byte and the other way around
		return true;
	}
	case EVoxelPinInternalType::Class:
	{
		// Classes can always be casted, check is done in TSubclassOf::Get
		return true;
	}
	case EVoxelPinInternalType::Object:
	{
		const UClass* Class = Cast<UClass>(GetInternalField());
		const UClass* OtherClass = Cast<UClass>(Other.GetInternalField());

		if (!ensureVoxelSlow(Class) ||
			!ensureVoxelSlow(OtherClass))
		{
			return false;
		}

		return Class->IsChildOf(OtherClass);
	}
	case EVoxelPinInternalType::Struct:
	{
		const UScriptStruct* Struct = Cast<UScriptStruct>(GetInternalField());
		const UScriptStruct* OtherStruct = Cast<UScriptStruct>(Other.GetInternalField());

		if (!ensureVoxelSlow(Struct) ||
			!ensureVoxelSlow(OtherStruct))
		{
			return false;
		}

		return Struct->IsChildOf(OtherStruct);
	}
	}
}

bool FVoxelPinType::CanBeCastedTo_K2(const FVoxelPinType& Other) const
{
	if (IsBuffer() == Other.IsBuffer() &&
		InternalType == EVoxelPinInternalType::Float &&
		Other.InternalType == EVoxelPinInternalType::Double)
	{
		return true;
	}

	return CanBeCastedTo(Other);
}

bool FVoxelPinType::CanBeCastedTo_Schema(const FVoxelPinType& Other) const
{
	if (bIsBufferArray != Other.bIsBufferArray)
	{
		return false;
	}

	if (!CanBeCastedTo(Other) &&
		// Inner to buffer
		!CanBeCastedTo(Other.GetInnerType()))
	{
		return false;
	}

	// Do strict inheritance checks for the schema (ie, the user UI)
	// At runtime we allow implicit conversions between classes as TSubclassOf::Get does the inheritance checks
	if (InternalType == EVoxelPinInternalType::Class)
	{
		check(Other.InternalType == EVoxelPinInternalType::Class);

		const UClass* Class = CastChecked<UClass>(GetInternalField());
		const UClass* OtherClass = CastChecked<UClass>(Other.GetInternalField());
		return Class->IsChildOf(OtherClass);
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

using FVoxelPinTypeCustomVersion = DECLARE_VOXEL_VERSION
(
	FirstVersion,
	StoreIsBuffer
);

constexpr FVoxelGuid GVoxelPinTypeCustomVersionGUID = MAKE_VOXEL_GUID("2A19FDEF616A4DC493F3266E3B35047B");
FCustomVersionRegistration GRegisterVoxelPinTypeCustomVersionGUID(GVoxelPinTypeCustomVersionGUID, FVoxelPinTypeCustomVersion::LatestVersion, TEXT("VoxelPinTypeVer"));

void FVoxelPinType::PostSerialize(const FArchive& Ar)
{
	if (IsValid())
	{
		*this = FVoxelPinType(GetEdGraphPinType());
	}
}

bool FVoxelPinType::Serialize(const FStructuredArchive::FSlot Slot)
{
	FArchive& UnderlyingArchive = Slot.GetUnderlyingArchive();
	UnderlyingArchive.UsingCustomVersion(GVoxelPinTypeCustomVersionGUID);

	if (UnderlyingArchive.CustomVer(GVoxelPinTypeCustomVersionGUID) >= FVoxelPinTypeCustomVersion::StoreIsBuffer)
	{
		return false;
	}

	FVoxelPinType_DEPRECATED OldType;
	const FVoxelPinType_DEPRECATED Defaults;
	FVoxelPinType_DEPRECATED::StaticStruct()->SerializeItem(Slot, &OldType, &Defaults);

	FEdGraphPinType EdGraphPinType;
	EdGraphPinType.PinCategory = OldType.PropertyClass;
	EdGraphPinType.PinSubCategory = OldType.Tag;
	EdGraphPinType.PinSubCategoryObject = OldType.PropertyObject_Internal;

	*this = FVoxelPinType(EdGraphPinType);

	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelPinType FVoxelPinType::GetExposedType() const
{
	if (IsBuffer())
	{
		if (IsBufferArray())
		{
			return GetInnerExposedType().GetBufferType().WithBufferArray(true);
		}
		else
		{
			return GetInnerExposedType();
		}
	}

	if (Is<FVoxelSeed>())
	{
		return Make<FVoxelExposedSeed>();
	}

	if (IsStruct())
	{
		if (const FVoxelObjectPinType* ObjectPinType = FVoxelObjectPinType::StructToPinType().FindRef(GetStruct()))
		{
			return MakeObject(ObjectPinType->GetClass());
		}
	}

	return *this;
}

FVoxelPinType FVoxelPinType::GetInnerExposedType() const
{
	return GetInnerType().GetExposedType();
}

FVoxelPinType FVoxelPinType::GetPinDefaultValueType() const
{
	ensure(HasPinDefaultValue());
	ensure(GetInnerExposedType() == GetExposedType());
	return GetExposedType();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class FVoxelGraphSeedStatics : public FVoxelSingleton
{
public:
	FVoxelSeed Hash(const FVoxelExposedSeed& Seed)
	{
		const FVoxelSeed Value = FCrc::StrCrc32(*Seed.Seed);
		{
			VOXEL_SCOPE_LOCK(CriticalSection);
			if (const FString* ExistingSeed = LookupMap.Find(Value))
			{
				ensure(Seed.Seed == *ExistingSeed);
			}
			else
			{
				LookupMap.Add_CheckNew(Value, Seed.Seed);
				ensure(LookupMap.Num() < 1000);
			}
		}
		return Value;
	}

	FVoxelExposedSeed Lookup(const FVoxelSeed Value)
	{
		VOXEL_SCOPE_LOCK(CriticalSection);
		const FString* Seed = LookupMap.Find(Value);
		if (!ensure(Seed))
		{
			return {};
		}
		return { *Seed };
	}

private:
	FVoxelFastCriticalSection CriticalSection;
	TVoxelAddOnlyMap<FVoxelSeed, FString> LookupMap;
};
FVoxelGraphSeedStatics* GVoxelGraphSeedStatics = MakeVoxelSingleton(FVoxelGraphSeedStatics);

FVoxelRuntimePinValue FVoxelPinType::MakeRuntimeValue(
	const FVoxelPinType& RuntimeType,
	const FVoxelPinValue& ExposedValue)
{
	ensure(IsInGameThread() || IsInAsyncLoadingThread());

	if (!ensure(!RuntimeType.IsWildcard()) ||
		!ensure(ExposedValue.GetType().CanBeCastedTo(RuntimeType.GetExposedType())))
	{
		return {};
	}

	if (RuntimeType.IsBuffer())
	{
		if (ExposedValue.IsArray())
		{
			FVoxelBufferBuilder BufferBuilder(RuntimeType.GetInnerType());
			for (const FVoxelTerminalPinValue& ExposedInnerValue : ExposedValue.GetArray())
			{
				const FVoxelRuntimePinValue InnerValue = MakeRuntimeValue(RuntimeType.GetInnerType(), ExposedInnerValue.AsValue());
				BufferBuilder.Add(InnerValue);
			}
			return FVoxelRuntimePinValue::Make(BufferBuilder.MakeBuffer(), RuntimeType);
		}
		else
		{
			const FVoxelRuntimePinValue InnerValue = MakeRuntimeValue(RuntimeType.GetInnerType(), ExposedValue);

			const TSharedRef<FVoxelBuffer> Buffer = FVoxelBuffer::Make(RuntimeType.GetInnerType());
			Buffer->InitializeFromConstant(InnerValue);
			return FVoxelRuntimePinValue::Make(Buffer, RuntimeType);
		}
	}

	if (ExposedValue.Is<FVoxelExposedSeed>())
	{
		const FVoxelSeed Seed = GVoxelGraphSeedStatics->Hash(ExposedValue.Get<FVoxelExposedSeed>());
		return FVoxelRuntimePinValue::Make(Seed);
	}

	if (ExposedValue.IsObject())
	{
		if (const FVoxelObjectPinType* ObjectPinType = FVoxelObjectPinType::StructToPinType().FindRef(RuntimeType.GetStruct()))
		{
			UObject* Object = ExposedValue.GetObject();
			const FVoxelInstancedStruct Struct = ObjectPinType->GetStruct(Object);
			ensure(ObjectPinType->GetObject(Struct) == Object);

			return FVoxelRuntimePinValue::MakeStruct(Struct);
		}
	}

	if (!ensure(ExposedValue.GetType().CanBeCastedTo(RuntimeType)))
	{
		return {};
	}

	if (ExposedValue.CanBeCastedTo<FVoxelPinValueInterface>())
	{
		const TSharedRef<FVoxelPinValueInterface> RuntimeValue = ExposedValue.Get<FVoxelPinValueInterface>().MakeSharedCopy();
		RuntimeValue->ComputeRuntimeData();
		return FVoxelRuntimePinValue::Make(RuntimeValue);
	}

	FVoxelRuntimePinValue Result = INLINE_LAMBDA -> FVoxelRuntimePinValue
	{
		switch (RuntimeType.GetInternalType())
		{
		default:
		{
			ensure(false);
			return {};
		}
		case EVoxelPinInternalType::Bool: return FVoxelRuntimePinValue::Make(ExposedValue.Get<bool>());
		case EVoxelPinInternalType::Float: return FVoxelRuntimePinValue::Make(ExposedValue.Get<float>());
		case EVoxelPinInternalType::Double: return FVoxelRuntimePinValue::Make(ExposedValue.Get<double>());
		case EVoxelPinInternalType::Int32: return FVoxelRuntimePinValue::Make(ExposedValue.Get<int32>());
		case EVoxelPinInternalType::Int64: return FVoxelRuntimePinValue::Make(ExposedValue.Get<int64>());
		case EVoxelPinInternalType::Name: return FVoxelRuntimePinValue::Make(ExposedValue.Get<FName>());
		case EVoxelPinInternalType::Byte: return FVoxelRuntimePinValue::Make(ExposedValue.Get<uint8>());
		case EVoxelPinInternalType::Class: return FVoxelRuntimePinValue::Make(ExposedValue.Get<TSubclassOf<UObject>>());
		case EVoxelPinInternalType::Struct: return FVoxelRuntimePinValue::MakeStruct(ExposedValue.GetStruct());
		}
	};
	return Result.WithType(RuntimeType);
}

FVoxelRuntimePinValue FVoxelPinType::MakeRuntimeValueFromInnerValue(
	const FVoxelPinType& RuntimeType,
	const FVoxelPinValue& ExposedInnerValue)
{
	ensure(!ExposedInnerValue.IsArray());

	if (RuntimeType.IsBufferArray())
	{
		FVoxelPinValue ExposedValue(ExposedInnerValue.GetType().GetBufferType().WithBufferArray(true));
		ExposedValue.AddValue(ExposedInnerValue.AsTerminalValue());
		return MakeRuntimeValue(RuntimeType, ExposedValue);
	}

	return MakeRuntimeValue(RuntimeType, ExposedInnerValue);
}

FVoxelPinValue FVoxelPinType::MakeExposedValue(
	const FVoxelRuntimePinValue& RuntimeValue,
	bool bIsArray)
{
	ensure(IsInGameThread());

	if (!RuntimeValue.IsValid())
	{
		return {};
	}

	ensure(!bIsArray || RuntimeValue.IsBuffer());

	if (RuntimeValue.IsBuffer())
	{
		const FVoxelBuffer& Buffer = RuntimeValue.Get<FVoxelBuffer>();

		if (bIsArray)
		{
			FVoxelPinValue ExposedValue(RuntimeValue.GetType().WithBufferArray(true).GetExposedType());
			for (int32 Index = 0; Index < Buffer.Num(); Index++)
			{
				const FVoxelRuntimePinValue InnerValue = Buffer.GetGeneric(Index).WithType(RuntimeValue.GetType().GetInnerType());
				const FVoxelPinValue ExposedInnerValue = MakeExposedValue(InnerValue, false);
				ExposedValue.AddValue(ExposedInnerValue.AsTerminalValue());
			}
			return ExposedValue;
		}
		else
		{
			if (!ensure(Buffer.IsConstant()))
			{
				return {};
			}

			return MakeExposedValue(Buffer.GetGenericConstant().WithType(RuntimeValue.GetType().GetInnerType()), false);
		}
	}

	if (RuntimeValue.Is<FVoxelSeed>())
	{
		const FVoxelExposedSeed Seed = GVoxelGraphSeedStatics->Lookup(RuntimeValue.Get<FVoxelSeed>());
		return FVoxelPinValue::Make(Seed);
	}

	if (RuntimeValue.GetType().IsStruct())
	{
		if (const FVoxelObjectPinType* ObjectPinType = FVoxelObjectPinType::StructToPinType().FindRef(RuntimeValue.GetType().GetStruct()))
		{
			UObject* Object = ObjectPinType->GetObject(RuntimeValue.GetStructView());

			FVoxelPinValue ExposedValue(MakeObject(ObjectPinType->GetClass()));
			ExposedValue.GetObject() = Object;
			return ExposedValue;
		}
	}

	if (RuntimeValue.CanBeCastedTo<FVoxelPinValueInterface>())
	{
		const TSharedRef<FVoxelPinValueInterface> ExposedValue = RuntimeValue.Get<FVoxelPinValueInterface>().MakeSharedCopy();
		ExposedValue->ComputeExposedData();
		return FVoxelPinValue::MakeStruct(FVoxelRuntimePinValue::Make(ExposedValue).GetStructView());
	}

	FVoxelPinValue Result = INLINE_LAMBDA -> FVoxelPinValue
	{
		switch (RuntimeValue.GetType().GetInternalType())
		{
		default:
		{
			ensure(false);
			return {};
		}
		case EVoxelPinInternalType::Bool: return FVoxelPinValue::Make(RuntimeValue.Get<bool>());
		case EVoxelPinInternalType::Float: return FVoxelPinValue::Make(RuntimeValue.Get<float>());
		case EVoxelPinInternalType::Double: return FVoxelPinValue::Make(RuntimeValue.Get<double>());
		case EVoxelPinInternalType::Int32: return FVoxelPinValue::Make(RuntimeValue.Get<int32>());
		case EVoxelPinInternalType::Int64: return FVoxelPinValue::Make(RuntimeValue.Get<int64>());
		case EVoxelPinInternalType::Name: return FVoxelPinValue::Make(RuntimeValue.Get<FName>());
		case EVoxelPinInternalType::Byte: return FVoxelPinValue::Make(RuntimeValue.Get<uint8>());
		case EVoxelPinInternalType::Class: return FVoxelPinValue::Make(RuntimeValue.Get<TSubclassOf<UObject>>());
		case EVoxelPinInternalType::Struct: return FVoxelPinValue::MakeStruct(RuntimeValue.GetStructView());
		}
	};
	return Result.WithType(RuntimeValue.GetType());
}

FVoxelPinValue FVoxelPinType::MakeExposedInnerValue(const FVoxelRuntimePinValue& RuntimeValue)
{
	const FVoxelPinValue ExposedValue = MakeExposedValue(RuntimeValue, false);
	if (!ExposedValue.IsArray())
	{
		return ExposedValue;
	}

	if (!ensure(ExposedValue.GetArray().Num() == 1))
	{
		return {};
	}
	return ExposedValue.GetArray()[0].AsValue();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#if WITH_EDITOR
struct FVoxelPinTypeSetRegistry
{
	TVoxelSet<FVoxelPinType> AllTypes;
	TVoxelSet<FVoxelPinType> AllUniformTypes;
	TVoxelSet<FVoxelPinType> AllBufferTypes;
	TVoxelSet<FVoxelPinType> AllBufferArrayTypes;
	TVoxelSet<FVoxelPinType> AllUniformsAndBufferArrays;
	TVoxelSet<FVoxelPinType> AllExposedTypes;
	TVoxelSet<FVoxelPinType> AllMaterials;
	TVoxelSet<FVoxelPinType> AllEnums;
	TVoxelSet<FVoxelPinType> AllObjects;

	FVoxelPinTypeSetRegistry()
	{
		VOXEL_FUNCTION_COUNTER();

		TVoxelAddOnlyMap<FVoxelPinType, int32> Map;
		Map.Reserve(1024);

		ForEachObjectOfClass<UScriptStruct>([&](UScriptStruct* Struct)
		{
			if (!Struct->HasMetaDataHierarchical(STATIC_FNAME("VoxelPinType")))
			{
				return;
			}

			Map.FindOrAdd(FVoxelPinType::MakeStruct(Struct));
		});

		for (const auto& It : FVoxelObjectPinType::StructToPinType())
		{
			Map.FindOrAdd(FVoxelPinType::MakeStruct(ConstCast(It.Key)));
		}

		for (UScriptStruct* Struct : GetDerivedStructs<FVoxelNode>())
		{
			if (Struct->HasMetaData(STATIC_FNAME("Abstract")) ||
				Struct->HasMetaData(STATIC_FNAME("Internal")))
			{
				continue;
			}

			const TVoxelInstancedStruct<FVoxelNode> Node(Struct);

			for (const FVoxelPin& Pin : Node->GetPins())
			{
				if (!Pin.IsPromotable())
				{
					Map.FindOrAdd(Pin.GetType().GetInnerType())++;
					continue;
				}

				const FVoxelPinTypeSet PromotionTypes = Node->GetPromotionTypes(Pin);
				if (PromotionTypes.GetSetType() != EVoxelPinTypeSetType::Set)
				{
					continue;
				}

				for (const FVoxelPinType& Type : PromotionTypes.GetTypes())
				{
					Map.FindOrAdd(Type.GetInnerType())++;
				}
			}
		}
		for (const TSubclassOf<UVoxelFunctionLibrary>& Class : GetDerivedClasses<UVoxelFunctionLibrary>())
		{
			for (const UFunction* Function : GetClassFunctions(Class))
			{
				if (Function->HasMetaData(STATIC_FNAME("Internal")))
				{
					continue;
				}

				for (const FProperty& Property : GetFunctionProperties(Function))
				{
					const FVoxelPinType Type = UVoxelFunctionLibrary::MakeType(Property);
					Map.FindOrAdd(Type.GetInnerType())++;
				}
			}
		}

		AllTypes.Reserve(3 * Map.Num());
		for (const auto& It : Map)
		{
			ensure(!It.Key.IsBuffer());
			if (It.Key.IsWildcard())
			{
				continue;
			}

			AllTypes.Add(It.Key);
			AllTypes.Add(It.Key.GetBufferType());
			AllTypes.Add(It.Key.GetBufferType().WithBufferArray(true));
		}

		AllTypes.Sort([&](const FVoxelPinType& A, const FVoxelPinType& B)
		{
			const int32 CountA = Map[A.GetInnerType()];
			const int32 CountB = Map[B.GetInnerType()];
			if (CountA != CountB)
			{
				return CountA > CountB;
			}

			return A.ToString() < B.ToString();
		});

		AllUniformTypes.Reserve(AllTypes.Num());
		AllBufferTypes.Reserve(AllTypes.Num());
		AllBufferArrayTypes.Reserve(AllTypes.Num());
		AllUniformsAndBufferArrays.Reserve(AllTypes.Num());
		AllExposedTypes.Reserve(AllTypes.Num());

		for (const FVoxelPinType& Type : AllTypes)
		{
			AllUniformTypes.Add(Type.GetInnerType());
			AllBufferTypes.Add(Type.GetBufferType().WithBufferArray(false));
			AllBufferArrayTypes.Add(Type.GetBufferType().WithBufferArray(true));
			AllUniformsAndBufferArrays.Add(Type.GetInnerType());
			AllUniformsAndBufferArrays.Add(Type.GetBufferType().WithBufferArray(true));
			AllExposedTypes.Add(Type.GetExposedType());

			if (Type.GetInnerExposedType().IsObject())
			{
				AllObjects.Add(Type.GetInnerType());
				AllObjects.Add(Type.GetBufferType().WithBufferArray(false));
			}
		}

		AllMaterials = AllExposedTypes;
		AllMaterials.Add(FVoxelPinType::Make<UTexture2D>());

		ForEachObjectOfClass<UEnum>([&](UEnum* Enum)
		{
			AllEnums.Add(FVoxelPinType::MakeEnum(Enum));
		});
	}
};

const TVoxelSet<FVoxelPinType>& FVoxelPinTypeSet::GetTypes() const
{
	if (SetType == EVoxelPinTypeSetType::Set)
	{
		return Types;
	}

	static FVoxelPinTypeSetRegistry* Registry = nullptr;
	if (!Registry)
	{
		Registry = new FVoxelPinTypeSetRegistry();

		GOnVoxelModuleUnloaded_DoCleanup.AddLambda([]
		{
			delete Registry;
			Registry = nullptr;
		});
	}

	switch (SetType)
	{
	default: ensure(false);
	case EVoxelPinTypeSetType::All: return Registry->AllTypes;
	case EVoxelPinTypeSetType::AllUniforms: return Registry->AllUniformTypes;
	case EVoxelPinTypeSetType::AllBuffers: return Registry->AllBufferTypes;
	case EVoxelPinTypeSetType::AllBufferArrays: return Registry->AllBufferArrayTypes;
	case EVoxelPinTypeSetType::AllUniformsAndBufferArrays: return Registry->AllUniformsAndBufferArrays;
	case EVoxelPinTypeSetType::AllExposed: return Registry->AllExposedTypes;
	case EVoxelPinTypeSetType::AllMaterials: return Registry->AllMaterials;
	case EVoxelPinTypeSetType::AllEnums: return Registry->AllEnums;
	case EVoxelPinTypeSetType::AllObjects: return Registry->AllObjects;
	}
}

bool FVoxelPinTypeSet::Contains(const FVoxelPinType& Type) const
{
	const bool bResult = GetTypes().Contains(Type);
	// If this is raised you might need to add 'VoxelPinType' to your struct metadata
	ensure(
		bResult ||
		SetType != EVoxelPinTypeSetType::All ||
		!Type.IsValid() ||
		Type.IsWildcard());
	return bResult;
}
#endif