// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelMinimal.h"
#include "UObject/MetaData.h"
#include "Misc/UObjectToken.h"
#include "Serialization/BulkDataReader.h"
#include "Serialization/BulkDataWriter.h"

#if WITH_EDITOR
#include "AssetToolsModule.h"
#include "Editor/EditorEngine.h"
#include "Editor/UnrealEdEngine.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "AssetRegistry/AssetRegistryModule.h"
#endif

#if WITH_EDITOR
FString FVoxelObjectUtilities::GetClassDisplayName_EditorOnly(const UClass* Class)
{
	if (!Class)
	{
		return "NULL";
	}

	FString ClassName = Class->GetDisplayNameText().ToString();

	const TArray<FString> Acronyms = { "RGB", "LOD" };
	for (const FString& Acronym : Acronyms)
	{
		ClassName.ReplaceInline(*Acronym, *(Acronym + " "), ESearchCase::CaseSensitive);
	}

	return ClassName;
}

FString FVoxelObjectUtilities::GetPropertyTooltip(const UFunction& Function, const FProperty& Property)
{
	ensure(Property.Owner == &Function);

	return GetPropertyTooltip(
		Function.GetToolTipText().ToString(),
		Property.GetName(),
		&Property == Function.GetReturnProperty());
}

FString FVoxelObjectUtilities::GetPropertyTooltip(const FString& FunctionTooltip, const FString& PropertyName, bool bIsReturnPin)
{
	VOXEL_FUNCTION_COUNTER();

	// Same as UK2Node_CallFunction::GeneratePinTooltipFromFunction

	const FString Tag = bIsReturnPin ? "@return" : "@param";

	int32 Position = -1;
	while (Position < FunctionTooltip.Len())
	{
		Position = FunctionTooltip.Find(Tag, ESearchCase::IgnoreCase, ESearchDir::FromStart, Position);
		if (Position == -1) // If the tag wasn't found
		{
			break;
		}

		// Advance past the tag
		Position += Tag.Len();

		// Advance past whitespace
		while (Position < FunctionTooltip.Len() && FChar::IsWhitespace(FunctionTooltip[Position]))
		{
			Position++;
		}

		// If this is a parameter pin
		if (!bIsReturnPin)
		{
			FString TagParamName;

			// Copy the parameter name
			while (Position < FunctionTooltip.Len() && !FChar::IsWhitespace(FunctionTooltip[Position]))
			{
				TagParamName.AppendChar(FunctionTooltip[Position++]);
			}

			// If this @param tag doesn't match the param we're looking for
			if (TagParamName != PropertyName)
			{
				continue;
			}
		}

		// Advance past whitespace
		while (Position < FunctionTooltip.Len() && FChar::IsWhitespace(FunctionTooltip[Position]))
		{
			Position++;
		}

		FString PropertyTooltip;
		while (Position < FunctionTooltip.Len() && FunctionTooltip[Position] != TEXT('@'))
		{
			// advance past newline
			while (Position < FunctionTooltip.Len() && FChar::IsLinebreak(FunctionTooltip[Position]))
			{
				Position++;

				// advance past whitespace at the start of a new line
				while (Position < FunctionTooltip.Len() && FChar::IsWhitespace(FunctionTooltip[Position]))
				{
					Position++;
				}

				// replace the newline with a single space
				if (Position < FunctionTooltip.Len() && !FChar::IsLinebreak(FunctionTooltip[Position]))
				{
					PropertyTooltip.AppendChar(TEXT(' '));
				}
			}

			if (Position < FunctionTooltip.Len() && FunctionTooltip[Position] != TEXT('@'))
			{
				PropertyTooltip.AppendChar(FunctionTooltip[Position++]);
			}
		}

		// Trim any trailing whitespace from the descriptive text
		PropertyTooltip.TrimEndInline();

		// If we came up with a valid description for the param/return-val
		if (PropertyTooltip.IsEmpty())
		{
			continue;
		}

		return PropertyTooltip;
	}

	return FunctionTooltip;
}

TMap<FName, FString> FVoxelObjectUtilities::GetMetadata(const UObject* Object)
{
	if (!ensure(Object))
	{
		return {};
	}

	return Object->GetOutermost()->GetMetaData()->ObjectMetaDataMap.FindRef(Object);
}

void FVoxelObjectUtilities::CreateNewAsset_Deferred(UClass* Class, const FString& BaseName, const FString& Suffix, TFunction<void(UObject*)> SetupObject)
{
	// Create an appropriate and unique name
	FString AssetName;
	FString PackageName;

	const FAssetToolsModule& AssetToolsModule = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>("AssetTools");
	AssetToolsModule.Get().CreateUniqueAssetName(BaseName, Suffix, PackageName, AssetName);

	IVoxelFactory* Factory = IVoxelAutoFactoryInterface::GetInterface().MakeFactory(Class);
	if (!ensure(Factory))
	{
		return;
	}

	Factory->OnSetupObject.AddLambda(SetupObject);

	IContentBrowserSingleton& ContentBrowser = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser").Get();
	ContentBrowser.FocusPrimaryContentBrowser(false);
	ContentBrowser.CreateNewAsset(AssetName, FPackageName::GetLongPackagePath(PackageName), Class, Factory->GetUFactory());
}

UObject* FVoxelObjectUtilities::CreateNewAsset_Direct(UClass* Class, const FString& BaseName, const FString& Suffix)
{
	FString NewPackageName;
	FString NewAssetName;

	const FAssetToolsModule& AssetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools");
	AssetToolsModule.Get().CreateUniqueAssetName(BaseName, Suffix, NewPackageName, NewAssetName);

	UPackage* Package = CreatePackage(*NewPackageName);
	if (!ensure(Package))
	{
		return nullptr;
	}

	UObject* Object = NewObject<UObject>(Package, Class, *NewAssetName, RF_Public | RF_Standalone);
	if (!ensure(Object))
	{
		return nullptr;
	}

	FAssetRegistryModule::AssetCreated(Object);
	Object->MarkPackageDirty();
	return Object;
}
#endif

TSharedRef<FUObjectToken> FVoxelObjectUtilities::MakeSafeObjectToken(UObject* Object)
{
	if (!Object)
	{
		return FUObjectToken::Create(Object);
	}

	if (AActor* Actor = Cast<AActor>(Object))
	{
		FText Text;
#if WITH_EDITOR
		Text = FText::FromString(Actor->GetActorLabel());
#endif
		const TSharedRef<FUObjectToken> Token = FUObjectToken::Create(Actor, Text);

#if WITH_EDITOR
		Token->OnMessageTokenActivated(MakeLambdaDelegate([WeakActor = MakeWeakObjectPtr(Actor)](const TSharedRef<IMessageToken>&)
		{
			AActor* ActorPtr = WeakActor.Get();
			if (!ActorPtr)
			{
				return;
			}

			GEditor->SelectNone(false, true);
			GEditor->SelectActor(ActorPtr, true, false, true);
			GEditor->NoteSelectionChange();
			GEditor->MoveViewportCamerasToActor(*ActorPtr, false);
		}));
#endif

		return Token;
	}

	if (UActorComponent* Component = Cast<UActorComponent>(Object))
	{
		FText Text;
#if WITH_EDITOR
		if (const AActor* Owner = Component->GetOwner())
		{
			Text = FText::FromString(Owner->GetActorLabel() + "." + Component->GetName());
		}
#endif
		const TSharedRef<FUObjectToken> Token = FUObjectToken::Create(Component, Text);

#if WITH_EDITOR
		Token->OnMessageTokenActivated(MakeLambdaDelegate([WeakComponent = MakeWeakObjectPtr(Component)](const TSharedRef<IMessageToken>&)
		{
			UActorComponent* ComponentPtr = WeakComponent.Get();
			if (!ComponentPtr)
			{
				return;
			}

			GEditor->SelectNone(false, true);
			GEditor->SelectComponent(ComponentPtr, true, false, true);
			GEditor->NoteSelectionChange();

			if (const USceneComponent* SceneComponent = Cast<USceneComponent>(ComponentPtr))
			{
				GEditor->MoveViewportCamerasToComponent(SceneComponent, false);
			}
		}));
#endif

		return Token;
	}

	const auto GetObjectDisplayName = [](const UObject* InObject)
	{
		if (FUObjectToken::DefaultOnGetObjectDisplayName().IsBound())
		{
			return FUObjectToken::DefaultOnGetObjectDisplayName().Execute(InObject, false).ToString();
		}
		else
		{
			return InObject->GetName();
		}
	};

	FString Text = GetObjectDisplayName(Object);

	if (!Object->HasAllFlags(RF_Transient))
	{
		// Find the first outer which is an actual package
		while (
			Object &&
			Object->GetOuter() &&
			!Object->GetOuter()->IsA<UPackage>())
		{
			Object = Object->GetOuter();
			Text = GetObjectDisplayName(Object) + "." + Text;
		}
	}

	const TSharedRef<FUObjectToken> Token = FUObjectToken::Create(Object, FText::FromString(Text));
	ConstCast(Token->GetOriginalObjectPathName()) = Object->GetPathName();

#if WITH_EDITOR
	Token->OnMessageTokenActivated(MakeLambdaDelegate([WeakObject = MakeWeakObjectPtr(Object)](const TSharedRef<IMessageToken>& InToken)
	{
		UObject* ObjectPtr = WeakObject.Get();
		if (!ObjectPtr)
		{
			return;
		}

		// Check if we can open an asset editor (OpenEditorForAsset return value is unreliable)
		const FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
		const TSharedPtr<IAssetTypeActions> AssetTypeActions = AssetToolsModule.Get().GetAssetTypeActionsForClass(ObjectPtr->GetClass()).Pin();

		if (!AssetTypeActions)
		{
			(void)FUObjectToken::DefaultOnMessageTokenActivated().ExecuteIfBound(InToken);
			return;
		}

		GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(ObjectPtr);
	}));
#endif

	return Token;
}

void FVoxelObjectUtilities::InvokeFunctionWithNoParameters(UObject* Object, UFunction* Function)
{
	VOXEL_FUNCTION_COUNTER();

	if (!ensure(Object) ||
		!ensure(Function) ||
		!ensure(!Function->Children))
	{
		return;
	}

	TGuardValue<bool> Guard(GAllowActorScriptExecutionInEditor, true);

	void* Params = nullptr;
	if (Function->ParmsSize > 0)
	{
		// Return value
		Params = FMemory::Malloc(Function->ParmsSize);
	}
	Object->ProcessEvent(Function, Params);
	FMemory::Free(Params);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool FVoxelObjectUtilities::PropertyFromText_Direct(const FProperty& Property, const FString& Text, void* Data, UObject* Owner)
{
	return Property.ImportText_Direct(*Text, Data, Owner, PPF_None) != nullptr;
}

bool FVoxelObjectUtilities::PropertyFromText_InContainer(const FProperty& Property, const FString& Text, void* ContainerData, UObject* Owner)
{
	return Property.ImportText_InContainer(*Text, ContainerData, Owner, PPF_None) != nullptr;
}

bool FVoxelObjectUtilities::PropertyFromText_InContainer(const FProperty& Property, const FString& Text, UObject* Owner)
{
	check(Owner);
	check(Owner->GetClass()->IsChildOf(CastChecked<UClass>(Property.Owner.ToUObject())));

	return PropertyFromText_InContainer(Property, Text, static_cast<void*>(Owner), Owner);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FString FVoxelObjectUtilities::PropertyToText_Direct(const FProperty& Property, const void* Data, const UObject* Owner)
{
	VOXEL_FUNCTION_COUNTER();

	FString Value;
	ensure(Property.ExportText_Direct(Value, Data, Data, ConstCast(Owner), PPF_None));
	return Value;
}

FString FVoxelObjectUtilities::PropertyToText_InContainer(const FProperty& Property, const void* ContainerData, const UObject* Owner)
{
	VOXEL_FUNCTION_COUNTER();

	FString Value;
	ensure(Property.ExportText_InContainer(0, Value, ContainerData, ContainerData, ConstCast(Owner), PPF_None));
	return Value;
}

FString FVoxelObjectUtilities::PropertyToText_InContainer(const FProperty& Property, const UObject* Owner)
{
	check(Owner);
	check(Owner->GetClass()->IsChildOf(CastChecked<UClass>(Property.Owner.ToUObject())));

	return PropertyToText_InContainer(Property, static_cast<const void*>(Owner), Owner);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool FVoxelObjectUtilities::ShouldSerializeBulkData(FArchive& Ar)
{
	return
		Ar.IsPersistent() &&
		!Ar.IsObjectReferenceCollector() &&
		!Ar.ShouldSkipBulkData() &&
		ensure(!Ar.IsTransacting());
}

void FVoxelObjectUtilities::SerializeBulkData(
	UObject* Object,
	FByteBulkData& BulkData,
	FArchive& Ar,
	TFunctionRef<void()> SaveBulkData,
	TFunctionRef<void()> LoadBulkData)
{
	VOXEL_FUNCTION_COUNTER();

	if (!ShouldSerializeBulkData(Ar))
	{
		return;
	}

	if (Ar.IsSaving())
	{
		// Clear the bulk data before writing it
		BulkData.RemoveBulkData();

		{
			VOXEL_SCOPE_COUNTER("SaveBulkData");
			SaveBulkData();
		}
	}

	BulkData.Serialize(Ar, Object);

	// NOTE: we can't call RemoveBulkData after saving as serialization is queued until the end of the save process

	if (Ar.IsLoading())
	{
		{
			VOXEL_SCOPE_COUNTER("LoadBulkData");
			LoadBulkData();
		}

		// Clear bulk data after loading to save memory
		BulkData.RemoveBulkData();
	}
}

void FVoxelObjectUtilities::SerializeBulkData(
	UObject* Object,
	FByteBulkData& BulkData,
	FArchive& Ar,
	TFunctionRef<void(FArchive&)> Serialize)
{
	SerializeBulkData(
		Object,
		BulkData,
		Ar,
		[&]
		{
			FBulkDataWriter Writer(BulkData, true);
			Serialize(Writer);
		},
		[&]
		{
			FBulkDataReader Reader(BulkData, true);
			Serialize(Reader);
			ensure(!Reader.IsError() && Reader.AtEnd());
		});
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

uint32 FVoxelObjectUtilities::HashProperty(const FProperty& Property, const void* DataPtr)
{
	switch (Property.GetCastFlags())
	{
#define CASE(Type) case Type::StaticClassCastFlags(): return FVoxelUtilities::MurmurHash(CastFieldChecked<Type>(Property).GetPropertyValue(DataPtr));
	CASE(FBoolProperty);
	CASE(FByteProperty);
	CASE(FIntProperty);
	CASE(FFloatProperty);
	CASE(FDoubleProperty);
	CASE(FUInt16Property);
	CASE(FUInt32Property);
	CASE(FUInt64Property);
	CASE(FInt8Property);
	CASE(FInt16Property);
	CASE(FInt64Property);
	CASE(FClassProperty);
	CASE(FNameProperty);
	CASE(FObjectProperty);
	CASE(FObjectPtrProperty);
	CASE(FWeakObjectProperty);
#undef CASE
	default: break;
	}

	if (Property.IsA<FArrayProperty>())
	{
		const FArrayProperty& ArrayProperty = CastFieldChecked<FArrayProperty>(Property);
		FScriptArrayHelper ArrayHelper(&ArrayProperty, DataPtr);

		uint32 Hash = FVoxelUtilities::MurmurHash(ArrayHelper.Num());
		for (int32 Index = 0; Index < ArrayHelper.Num(); Index++)
		{
			Hash ^= FVoxelUtilities::MurmurHash(HashProperty(*ArrayProperty.Inner, ArrayHelper.GetRawPtr(Index)), Index);
		}
		return Hash;
	}

	if (Property.IsA<FSetProperty>())
	{
		const FSetProperty& SetProperty = CastFieldChecked<FSetProperty>(Property);
		FScriptSetHelper SetHelper(&SetProperty, DataPtr);

		uint32 Hash = FVoxelUtilities::MurmurHash(SetHelper.Num());
		for (int32 Index = 0; Index < SetHelper.GetMaxIndex(); Index++)
		{
			if (!SetHelper.IsValidIndex(Index))
			{
				continue;
			}

			Hash ^= FVoxelUtilities::MurmurHash(HashProperty(*SetProperty.ElementProp, SetHelper.GetElementPtr(Index)));
		}
		return Hash;
	}

	if (Property.IsA<FMapProperty>())
	{
		const FMapProperty& MapProperty = CastFieldChecked<FMapProperty>(Property);
		FScriptMapHelper MapHelper(&MapProperty, DataPtr);

		uint32 Hash = FVoxelUtilities::MurmurHash(MapHelper.Num());
		for (int32 Index = 0; Index < MapHelper.GetMaxIndex(); Index++)
		{
			if (!MapHelper.IsValidIndex(Index))
			{
				continue;
			}

			Hash ^= FVoxelUtilities::MurmurHashMulti(
				HashProperty(*MapProperty.KeyProp, MapHelper.GetKeyPtr(Index)),
				HashProperty(*MapProperty.ValueProp, MapHelper.GetValuePtr(Index)));
		}
		return Hash;
	}

	if (!ensure(Property.IsA<FStructProperty>()))
	{
		return 0;
	}

	const UScriptStruct* Struct = CastFieldChecked<FStructProperty>(Property).Struct;

#define CASE(Type) \
	if (Struct == StaticStructFast<Type>()) \
	{ \
		return FVoxelUtilities::MurmurHash(*static_cast<const Type*>(DataPtr)); \
	}

	CASE(FVector2D);
	CASE(FVector);
	CASE(FVector4);
	CASE(FIntPoint);
	CASE(FIntVector);
	CASE(FRotator);
	CASE(FQuat);
	CASE(FTransform);
	CASE(FColor);
	CASE(FLinearColor);
	CASE(FMatrix);

#undef CASE

	UScriptStruct::ICppStructOps* CppStructOps = Struct->GetCppStructOps();
	check(CppStructOps);

	if (!ensure(CppStructOps->HasGetTypeHash()))
	{
		return 0;
	}

	return CppStructOps->GetStructTypeHash(DataPtr);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#if !UE_BUILD_SHIPPING
TMap<const UScriptStruct*, UScriptStruct::ICppStructOps*> GVoxelCachedStructOps;

VOXEL_RUN_ON_STARTUP_GAME(RegisterVoxelCachedStructOps)
{
	GVoxelCachedStructOps.Reserve(8192);

	ForEachObjectOfClass<UScriptStruct>([&](const UScriptStruct* Struct)
	{
		GVoxelCachedStructOps.Add(Struct, Struct->GetCppStructOps());
	});
}
#endif

void FVoxelObjectUtilities::DestroyStruct_Safe(const UScriptStruct* Struct, void* StructMemory)
{
	check(Struct);
	check(StructMemory);

	if (UObjectInitialized())
	{
		Struct->DestroyStruct(StructMemory);
	}
	else
	{
#if !UE_BUILD_SHIPPING
		// Always cleanly destroy structs so that leak detection works properly
		UScriptStruct::ICppStructOps* CppStructOps = GVoxelCachedStructOps[Struct];
		check(CppStructOps);
		if (CppStructOps->HasDestructor())
		{
			CppStructOps->Destruct(StructMemory);
		}
#endif
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelObjectUtilities::AddStructReferencedObjects(FReferenceCollector& Collector, const UScriptStruct* Struct, void* StructMemory)
{
	VOXEL_FUNCTION_COUNTER();

	if (!ensure(Struct))
	{
		return;
	}

	if (Struct->StructFlags & STRUCT_AddStructReferencedObjects)
	{
		Struct->GetCppStructOps()->AddStructReferencedObjects()(StructMemory, Collector);
	}

	for (TPropertyValueIterator<const FObjectProperty> It(Struct, StructMemory); It; ++It)
	{
		UObject** ObjectPtr = static_cast<UObject**>(ConstCast(It.Value()));
		Collector.AddReferencedObject(*ObjectPtr);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TUniquePtr<FBoolProperty> FVoxelObjectUtilities::MakeBoolProperty()
{
	TUniquePtr<FBoolProperty> Property = MakeUnique<FBoolProperty>(FFieldVariant(), FName(), EObjectFlags());
	Property->ElementSize = sizeof(bool);
	return Property;
}

TUniquePtr<FFloatProperty> FVoxelObjectUtilities::MakeFloatProperty()
{
	TUniquePtr<FFloatProperty> Property = MakeUnique<FFloatProperty>(FFieldVariant(), FName(), EObjectFlags());
	Property->ElementSize = sizeof(float);
	return Property;
}

TUniquePtr<FIntProperty> FVoxelObjectUtilities::MakeIntProperty()
{
	TUniquePtr<FIntProperty> Property = MakeUnique<FIntProperty>(FFieldVariant(), FName(), EObjectFlags());
	Property->ElementSize = sizeof(int32);
	return Property;
}

TUniquePtr<FNameProperty> FVoxelObjectUtilities::MakeNameProperty()
{
	TUniquePtr<FNameProperty> Property = MakeUnique<FNameProperty>(FFieldVariant(), FName(), EObjectFlags());
	Property->ElementSize = sizeof(FName);
	return Property;
}

TUniquePtr<FEnumProperty> FVoxelObjectUtilities::MakeEnumProperty(const UEnum* Enum)
{
	TUniquePtr<FEnumProperty> Property = MakeUnique<FEnumProperty>(FFieldVariant(), FName(), EObjectFlags());
	Property->SetEnum(ConstCast(Enum));
	Property->ElementSize = sizeof(uint8);
	return Property;
}

TUniquePtr<FStructProperty> FVoxelObjectUtilities::MakeStructProperty(const UScriptStruct* Struct)
{
	check(Struct);

	TUniquePtr<FStructProperty> Property = MakeUnique<FStructProperty>(FFieldVariant(), FName(), EObjectFlags());
	Property->Struct = ConstCast(Struct);
	Property->ElementSize = Struct->GetStructureSize();
	return Property;
}

TUniquePtr<FObjectProperty> FVoxelObjectUtilities::MakeObjectProperty(const UClass* Class)
{
	check(Class);

	TUniquePtr<FObjectProperty> Property = MakeUnique<FObjectProperty>(FFieldVariant(), FName(), EObjectFlags());
	Property->PropertyClass = ConstCast(Class);
	Property->ElementSize = sizeof(UObject*);
	return Property;
}

TUniquePtr<FArrayProperty> FVoxelObjectUtilities::MakeArrayProperty(FProperty* InnerProperty)
{
	check(InnerProperty);

	TUniquePtr<FArrayProperty> Property = MakeUnique<FArrayProperty>(FFieldVariant(), FName(), EObjectFlags());
	Property->Inner = InnerProperty;
	return Property;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TArray<UScriptStruct*> GetDerivedStructs(const UScriptStruct* StructType, bool bIncludeBase)
{
	VOXEL_FUNCTION_COUNTER();

	TArray<UScriptStruct*> Result;
	ForEachObjectOfClass<UScriptStruct>([&](UScriptStruct* Struct)
	{
		if (!Struct->IsChildOf(StructType))
		{
			return;
		}
		if (!bIncludeBase &&
			StructType == Struct)
		{
			return;
		}

		Result.Add(Struct);
	});
	Result.Shrink();
	return Result;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool IsFunctionInput(const FProperty& Property)
{
	ensure(Property.Owner.IsA(UFunction::StaticClass()));

	return
		!Property.HasAnyPropertyFlags(CPF_ReturnParm) &&
		(!Property.HasAnyPropertyFlags(CPF_OutParm) || Property.HasAnyPropertyFlags(CPF_ReferenceParm));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class FRestoreClassInfo
{
public:
	static const TMap<FName, TObjectPtr<UFunction>>& GetFunctionMap(const UClass* Class)
	{
		return ReinterpretCastRef<TMap<FName, TObjectPtr<UFunction>>>(Class->FuncMap);
	}
};

TArray<UFunction*> GetClassFunctions(const UClass* Class, const bool bIncludeSuper)
{
	TArray<TObjectPtr<UFunction>> Functions;
	FRestoreClassInfo::GetFunctionMap(Class).GenerateValueArray(Functions);

	if (bIncludeSuper)
	{
		const UClass* SuperClass = Class->GetSuperClass();
		while (SuperClass)
		{
			for (auto& It : FRestoreClassInfo::GetFunctionMap(SuperClass))
			{
				Functions.Add(It.Value);
			}
			SuperClass = Class->GetSuperClass();
		}
	}

	return Functions;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#if WITH_EDITOR
FString GetStringMetaDataHierarchical(const UStruct* Struct, const FName Name)
{
	FString Result;
	Struct->GetStringMetaDataHierarchical(Name, &Result);
	return Result;
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelNullCheckReferenceCollector::AddStableReference(UObject** Object)
{
	const bool bValid = *Object != nullptr;
	ReferenceCollector.AddStableReference(Object);
	ensure(bValid == (*Object != nullptr));
}

void FVoxelNullCheckReferenceCollector::AddStableReferenceArray(TArray<UObject*>* Objects)
{
	for (UObject*& Object : *Objects)
	{
		AddStableReference(&Object);
	}
}

void FVoxelNullCheckReferenceCollector::AddStableReferenceSet(TSet<UObject*>* Objects)
{
	for (UObject*& Object : *Objects)
	{
		AddStableReference(&Object);
	}
}

bool FVoxelNullCheckReferenceCollector::NeedsPropertyReferencer() const
{
	return ReferenceCollector.NeedsPropertyReferencer();
}

bool FVoxelNullCheckReferenceCollector::IsIgnoringArchetypeRef() const
{
	return ReferenceCollector.IsIgnoringArchetypeRef();
}

bool FVoxelNullCheckReferenceCollector::IsIgnoringTransient() const
{
	return ReferenceCollector.IsIgnoringTransient();
}

void FVoxelNullCheckReferenceCollector::AllowEliminatingReferences(const bool bAllow)
{
	ReferenceCollector.AllowEliminatingReferences(bAllow);
}

void FVoxelNullCheckReferenceCollector::SetSerializedProperty(FProperty* InProperty)
{
	ReferenceCollector.SetSerializedProperty(InProperty);
}

FProperty* FVoxelNullCheckReferenceCollector::GetSerializedProperty() const
{
	return ReferenceCollector.GetSerializedProperty();
}

bool FVoxelNullCheckReferenceCollector::MarkWeakObjectReferenceForClearing(UObject** WeakReference)
{
	return ReferenceCollector.MarkWeakObjectReferenceForClearing(WeakReference);
}

void FVoxelNullCheckReferenceCollector::SetIsProcessingNativeReferences(const bool bIsNative)
{
	ReferenceCollector.SetIsProcessingNativeReferences(bIsNative);
}

bool FVoxelNullCheckReferenceCollector::IsProcessingNativeReferences() const
{
	return ReferenceCollector.IsProcessingNativeReferences();
}

bool FVoxelNullCheckReferenceCollector::NeedsInitialReferences() const
{
	return ReferenceCollector.NeedsInitialReferences();
}

void FVoxelNullCheckReferenceCollector::HandleObjectReference(UObject*& InObject, const UObject* InReferencingObject, const FProperty* InReferencingProperty)
{
	const bool bValid = InObject != nullptr;
	ReferenceCollector.AddReferencedObject(InObject, InReferencingObject, InReferencingProperty);
	ensure(bValid == (InObject != nullptr));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

struct FVoxelSharedStructOpaque {};

TSharedRef<FVoxelSharedStructOpaque> MakeSharedStruct(const UScriptStruct* Struct, const void* StructToCopyFrom)
{
	check(Struct);
	checkVoxelSlow(UObjectInitialized());

	void* Memory = FVoxelMemory::Malloc(FMath::Max(1, Struct->GetStructureSize()));
	Struct->InitializeStruct(Memory);

	if (StructToCopyFrom)
	{
		Struct->CopyScriptStruct(Memory, StructToCopyFrom);
	}

	return MakeShareableStruct(Struct, Memory);
}

TSharedRef<FVoxelSharedStructOpaque> MakeShareableStruct(const UScriptStruct* Struct, void* StructMemory)
{
	if (Struct->IsChildOf(StaticStructFast<FVoxelVirtualStruct>()))
	{
		const TSharedRef<FVoxelVirtualStruct> SharedRef = MakeVoxelShareable(static_cast<FVoxelVirtualStruct*>(StructMemory));
		SharedRef->Internal_UpdateWeakReferenceInternal(SharedRef);
		return ReinterpretCastRef<TSharedRef<FVoxelSharedStructOpaque>>(SharedRef);
	}

	return TSharedPtr<FVoxelSharedStructOpaque>(static_cast<FVoxelSharedStructOpaque*>(StructMemory), [Struct](void* InMemory)
	{
		FVoxelObjectUtilities::DestroyStruct_Safe(Struct, InMemory);
		FVoxelMemory::Free(InMemory);
	}).ToSharedRef();
}