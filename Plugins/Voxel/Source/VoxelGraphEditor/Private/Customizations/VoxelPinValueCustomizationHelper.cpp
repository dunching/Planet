// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "Customizations/VoxelPinValueCustomizationHelper.h"
#include "VoxelPinType.h"
#include "VoxelPinValue.h"
#include "VoxelParameter.h"

TSharedPtr<FVoxelStructCustomizationWrapper> FVoxelPinValueCustomizationHelper::CreatePinValueCustomization(
	const TSharedRef<IPropertyHandle>& PropertyHandle,
	const FVoxelDetailInterface& DetailInterface,
	const TMap<FName, FString>& MetaData,
	const TFunctionRef<void(FDetailWidgetRow&, const TSharedRef<SWidget>&)> SetupRow,
	const FAddPropertyParams& Params,
	const TAttribute<bool>& IsEnabled)
{
	const auto ApplyMetaData = [&](const TSharedRef<IPropertyHandle>& ChildHandle)
	{
		if (const FProperty* MetaDataProperty = PropertyHandle->GetMetaDataProperty())
		{
			if (const TMap<FName, FString>* MetaDataMap = MetaDataProperty->GetMetaDataMap())
			{
				for (const auto& It : *MetaDataMap)
				{
					ChildHandle->SetInstanceMetaData(It.Key, It.Value);
				}
			}
		}

		if (const TMap<FName, FString>* MetaDataMap = PropertyHandle->GetInstanceMetaDataMap())
		{
			for (const auto& It : *MetaDataMap)
			{
				ChildHandle->SetInstanceMetaData(It.Key, It.Value);
			}
		}

		for (const auto& It : MetaData)
		{
			ChildHandle->SetInstanceMetaData(It.Key, It.Value);
		}
	};

	const TSharedRef<IPropertyHandle> TypeHandle = PropertyHandle->GetChildHandleStatic(FVoxelPinValue, Type);
	const FVoxelPinType Type = FVoxelEditorUtilities::GetStructPropertyValue<FVoxelPinType>(TypeHandle);
	if (!Type.IsValid())
	{
		return nullptr;
	}

	if (Type.IsBuffer())
	{
		const TSharedRef<IPropertyHandle> ArrayHandle = PropertyHandle->GetChildHandleStatic(FVoxelPinValue, Array);

		IDetailPropertyRow& Row = DetailInterface.AddProperty(ArrayHandle);

		// Disable child rows
		Row.EditCondition(IsEnabled, {});

		TSharedPtr<SWidget> NameWidget;
		TSharedPtr<SWidget> ValueWidget;
		Row.GetDefaultWidgets(NameWidget, ValueWidget, true);
		Row.ShowPropertyButtons(false);
		Row.ShouldAutoExpand();

		SetupRow(Row.CustomWidget(true), ValueWidget.ToSharedRef());

		return nullptr;
	}

	if (Type.IsStruct())
	{
		const TSharedRef<IPropertyHandle> StructHandle = PropertyHandle->GetChildHandleStatic(FVoxelPinValue, Struct);
		const TSharedPtr<FVoxelStructCustomizationWrapper> Wrapper = FVoxelStructCustomizationWrapper::Make(StructHandle);
		if (!ensure(Wrapper))
		{
			return nullptr;
		}

		IDetailPropertyRow* Row = Wrapper->AddExternalStructure(DetailInterface, Params);
		if (!ensure(Row))
		{
			return nullptr;
		}

		// Disable child rows
		Row->EditCondition(IsEnabled, {});

		TSharedPtr<SWidget> NameWidget;
		TSharedPtr<SWidget> ValueWidget;
		Row->GetDefaultWidgets(NameWidget, ValueWidget, true);

		SetupRow(Row->CustomWidget(true), AddArrayItemOptions(PropertyHandle, ValueWidget).ToSharedRef());

		return Wrapper;
	}

	const TSharedPtr<SWidget> ValueWidget = INLINE_LAMBDA -> TSharedPtr<SWidget>
	{
		switch (Type.GetInternalType())
		{
		default:
		{
			ensure(false);
			return nullptr;
		}
		case EVoxelPinInternalType::Bool:
		{
			return PropertyHandle->GetChildHandleStatic(FVoxelPinValue, bBool)->CreatePropertyValueWidget();
		}
		case EVoxelPinInternalType::Float:
		{
			const TSharedRef<IPropertyHandle> ValueHandle = PropertyHandle->GetChildHandleStatic(FVoxelPinValue, Float);
			ApplyMetaData(ValueHandle);
			return ValueHandle->CreatePropertyValueWidget();
		}
		case EVoxelPinInternalType::Double:
		{
			const TSharedRef<IPropertyHandle> ValueHandle = PropertyHandle->GetChildHandleStatic(FVoxelPinValue, Double);
			ApplyMetaData(ValueHandle);
			return ValueHandle->CreatePropertyValueWidget();
		}
		case EVoxelPinInternalType::Int32:
		{
			const TSharedRef<IPropertyHandle> ValueHandle = PropertyHandle->GetChildHandleStatic(FVoxelPinValue, Int32);
			ApplyMetaData(ValueHandle);
			return ValueHandle->CreatePropertyValueWidget();
		}
		case EVoxelPinInternalType::Int64:
		{
			const TSharedRef<IPropertyHandle> ValueHandle = PropertyHandle->GetChildHandleStatic(FVoxelPinValue, Int64);
			ApplyMetaData(ValueHandle);
			return ValueHandle->CreatePropertyValueWidget();
		}
		case EVoxelPinInternalType::Name:
		{
			return PropertyHandle->GetChildHandleStatic(FVoxelPinValue, Name)->CreatePropertyValueWidget();
		}
		case EVoxelPinInternalType::Byte:
		{
			const TSharedRef<IPropertyHandle> Handle = PropertyHandle->GetChildHandleStatic(FVoxelPinValue, Byte);

			const UEnum* Enum = Type.GetEnum();
			if (!Enum)
			{
				return Handle->CreatePropertyValueWidget();
			}

			uint8 Byte = 0;
			switch (Handle->GetValue(Byte))
			{
			default:
			{
				ensure(false);
				return nullptr;
			}
			case FPropertyAccess::MultipleValues:
			{
				return SNew(SVoxelDetailText).Text(INVTEXT("Multiple Values"));
			}
			case FPropertyAccess::Fail:
			{
				ensure(false);
				return nullptr;
			}
			case FPropertyAccess::Success: break;
			}

			return
				SNew(SBox)
				.MinDesiredWidth(125.f)
				[
					SNew(SVoxelDetailComboBox<uint8>)
					.RefreshDelegate(FVoxelEditorUtilities::MakeRefreshDelegate(PropertyHandle, DetailInterface))
					.Options_Lambda([=]
					{
						TArray<uint8> Enumerators;
						for (int32 Index = 0; Index < Enum->NumEnums() - 1; Index++)
						{
							if (Enum->HasMetaData(TEXT("Hidden"), Index) ||
								Enum->HasMetaData(TEXT("Spacer"), Index))
							{
								continue;
							}

							Enumerators.Add(Enum->GetValueByIndex(Index));
						}

						return Enumerators;
					})
					.CurrentOption(Enum->GetIndexByValue(Byte))
					.OptionText(MakeLambdaDelegate([=](const uint8 Value)
					{
						FString EnumDisplayName = Enum->GetDisplayNameTextByValue(Value).ToString();
						if (EnumDisplayName.Len() == 0)
						{
							return Enum->GetNameStringByValue(Value);
						}

						return EnumDisplayName;
					}))
					.OnSelection_Lambda([Handle](const uint8 NewValue)
					{
						ensure(Handle->SetValue(uint8(NewValue)) == FPropertyAccess::Success);
					})
				];
		}
		case EVoxelPinInternalType::Class:
		{
			const TSharedRef<IPropertyHandle> Handle = PropertyHandle->GetChildHandleStatic(FVoxelPinValue, Class);

			Handle->GetProperty()->SetMetaData("AllowedClasses", Type.GetBaseClass()->GetPathName());
			const TSharedRef<SWidget> CustomValueWidget = Handle->CreatePropertyValueWidget();
			Handle->GetProperty()->RemoveMetaData("AllowedClasses");

			return CustomValueWidget;
		}
		case EVoxelPinInternalType::Object:
		{
			const TSharedRef<IPropertyHandle> Handle = PropertyHandle->GetChildHandleStatic(FVoxelPinValue, Object);

			Handle->GetProperty()->SetMetaData("AllowedClasses", Type.GetObjectClass()->GetPathName());
			const TSharedRef<SWidget> CustomValueWidget = Handle->CreatePropertyValueWidget();
			Handle->GetProperty()->RemoveMetaData("AllowedClasses");

			return CustomValueWidget;
		}
		}
	};

	if (!ValueWidget)
	{
		return nullptr;
	}

	FUIAction CopyAction;
	FUIAction PasteAction;
	PropertyHandle->CreateDefaultPropertyCopyPasteActions(CopyAction, PasteAction);

	FDetailWidgetRow& Row = DetailInterface.AddCustomRow(PropertyHandle->GetPropertyDisplayName())
	.CopyAction(CopyAction)
	.PasteAction(PasteAction);

	SetupRow(
		Row,
		AddArrayItemOptions(PropertyHandle, ValueWidget).ToSharedRef());

	return nullptr;
}

void FVoxelPinValueCustomizationHelper::CreatePinValueRangeSetter(FDetailWidgetRow& Row,
		const TSharedRef<IPropertyHandle>& PropertyHandle,
		const FText& Name,
		const FText& ToolTip,
		FName Min,
		FName Max,
		const TFunction<TMap<FName, FString>(const TSharedPtr<IPropertyHandle>&)>& GetMetaData,
		const TFunction<void(const TSharedPtr<IPropertyHandle>&, const TMap<FName, FString>&)>& SetMetaData)
{
	const auto RangeVisibility = [WeakHandle = MakeWeakPtr(PropertyHandle)]() -> EVisibility
	{
		const TSharedPtr<IPropertyHandle> Handle = WeakHandle.Pin();
		if (!Handle)
		{
			return EVisibility::Collapsed;
		}

		const TSharedRef<IPropertyHandle> TypeHandle = Handle->GetChildHandleStatic(FVoxelParameter, Type);
		const FVoxelPinType& PinType = FVoxelEditorUtilities::GetStructPropertyValue<FVoxelPinType>(TypeHandle);
		if (PinType.Is<int32>() ||
			PinType.Is<float>() ||
			PinType.Is<double>())
		{
			return EVisibility::Visible;
		}

		return EVisibility::Collapsed;
	};

	FText MinValue;
	FText MaxValue;
	{
		TMap<FName, FString> MetaData = GetMetaData(PropertyHandle);
		if (const FString* Value = MetaData.Find(Min))
		{
			MinValue = FText::FromString(*Value);
		}
		if (const FString* Value = MetaData.Find(Max))
		{
			MaxValue = FText::FromString(*Value);
		}
	}

	Row
	.Visibility(MakeAttributeLambda(RangeVisibility))
	.NameContent()
	[
		SNew(SVoxelDetailText)
		.Text(Name)
		.ToolTipText(ToolTip)
	]
	.ValueContent()
	[
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.FillWidth(1.f)
		[
			SNew(SEditableTextBox)
			.Text(MinValue)
			.OnTextCommitted_Lambda([Min, GetMetaData, SetMetaData, WeakHandle = MakeWeakPtr(PropertyHandle)](const FText& NewValue, ETextCommit::Type ActionType)
			{
				const TSharedPtr<IPropertyHandle> Handle = WeakHandle.Pin();
				if (!ensure(Handle))
				{
					return;
				}

				if (ActionType != ETextCommit::OnEnter &&
					ActionType != ETextCommit::OnUserMovedFocus)
				{
					return;
				}

				TMap<FName, FString> MetaData = GetMetaData(Handle);
				MetaData.Add(Min, NewValue.ToString());
				SetMetaData(Handle, MetaData);
			})
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		[
			SNew(SVoxelDetailText)
			.Text(INVTEXT(" .. "))
		]
		+ SHorizontalBox::Slot()
		.FillWidth(1)
		[
			SNew(SEditableTextBox)
			.Text(MaxValue)
			.OnTextCommitted_Lambda([Max, GetMetaData, SetMetaData, WeakHandle = MakeWeakPtr(PropertyHandle)](const FText& NewValue, ETextCommit::Type ActionType)
			{
				const TSharedPtr<IPropertyHandle> Handle = WeakHandle.Pin();
				if (!ensure(Handle))
				{
					return;
				}

				if (ActionType != ETextCommit::OnEnter &&
					ActionType != ETextCommit::OnUserMovedFocus)
				{
					return;
				}

				TMap<FName, FString> MetaData = GetMetaData(Handle);
				MetaData.Add(Max, NewValue.ToString());
				SetMetaData(Handle, MetaData);
			})
		]
	];
}

float FVoxelPinValueCustomizationHelper::GetValueWidgetWidthByType(const TSharedPtr<IPropertyHandle>& PropertyHandle, const FVoxelPinType& Type)
{
	const FVoxelPinType ExposedType = Type.GetExposedType();
	const bool bIsArrayItem = PropertyHandle && PropertyHandle->GetParentHandle() && CastField<FArrayProperty>(PropertyHandle->GetParentHandle()->GetProperty());
	const float ExtendByArray = bIsArrayItem ? 32.f : 0.f;

	if (!ExposedType.IsValid())
	{
		return FDetailWidgetRow::DefaultValueMaxWidth + ExtendByArray;
	}

	if (ExposedType.Is<FVector2D>() ||
		ExposedType.Is<FIntPoint>() ||
		ExposedType.Is<FVoxelFloatRange>() ||
		ExposedType.Is<FVoxelInt32Range>())
	{
		return FDetailWidgetRow::DefaultValueMaxWidth * 2.f + ExtendByArray;
	}

	if (ExposedType.Is<FVector>() ||
		ExposedType.Is<FIntVector>() ||
		ExposedType.Is<FQuat>())
	{
		return FDetailWidgetRow::DefaultValueMaxWidth * 3.f + ExtendByArray;
	}

	if (ExposedType.Is<FVector4>() ||
		ExposedType.Is<FIntVector4>())
	{
		return FDetailWidgetRow::DefaultValueMaxWidth * 4.f + ExtendByArray;
	}

	if (ExposedType.IsObject())
	{
		return FDetailWidgetRow::DefaultValueMaxWidth * 2.f + ExtendByArray;
	}

	return FDetailWidgetRow::DefaultValueMaxWidth + ExtendByArray;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedPtr<SWidget> FVoxelPinValueCustomizationHelper::AddArrayItemOptions(const TSharedRef<IPropertyHandle>& PropertyHandle, const TSharedPtr<SWidget>& ValueWidget)
{
	if (!PropertyHandle->GetParentHandle() ||
		!CastField<FArrayProperty>(PropertyHandle->GetParentHandle()->GetProperty()))
	{
		return ValueWidget;
	}

	const FExecuteAction InsertAction = MakeWeakPtrDelegate(PropertyHandle, [PropertyHandle]
	{
		const TSharedPtr<IPropertyHandleArray> ArrayHandle = PropertyHandle->GetParentHandle()->AsArray();
		check(ArrayHandle.IsValid());

		const int32 Index = PropertyHandle->GetIndexInArray();
		ArrayHandle->Insert(Index);
	});
	const FExecuteAction DeleteAction = MakeWeakPtrDelegate(PropertyHandle, [PropertyHandle]
	{
		const TSharedPtr<IPropertyHandleArray> ArrayHandle = PropertyHandle->GetParentHandle()->AsArray();
		check(ArrayHandle.IsValid());

		const int32 Index = PropertyHandle->GetIndexInArray();
		ArrayHandle->DeleteItem(Index);
	});
	const FExecuteAction DuplicateAction = MakeWeakPtrDelegate(PropertyHandle, [PropertyHandle]
	{
		const TSharedPtr<IPropertyHandleArray> ArrayHandle = PropertyHandle->GetParentHandle()->AsArray();
		check(ArrayHandle.IsValid());

		const int32 Index = PropertyHandle->GetIndexInArray();
		ArrayHandle->DuplicateItem(Index);
	});

	return
		SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
		.FillWidth(1.f)
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Center)
		[
			ValueWidget.ToSharedRef()
		]
		+ SHorizontalBox::Slot()
		.AutoWidth()
		.HAlign(HAlign_Left)
		.VAlign(VAlign_Center)
		.Padding(4.0f, 1.0f, 0.0f, 1.0f)
		[
			PropertyCustomizationHelpers::MakeInsertDeleteDuplicateButton(InsertAction, DeleteAction, DuplicateAction)
		];
}