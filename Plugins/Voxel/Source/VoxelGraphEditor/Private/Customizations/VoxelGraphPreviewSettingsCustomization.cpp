// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelGraphPreviewSettingsCustomization.h"
#include "VoxelGraphNodeBase.h"
#include "Preview/VoxelPreviewHandler.h"

void FVoxelGraphPreviewSettingsCustomization::CustomizeDetails(IDetailLayoutBuilder& DetailLayout)
{
	if (DetailLayout.GetSelectedObjects().Num() != 1)
	{
		return;
	}

	const TWeakObjectPtr<UVoxelGraphNodeBase> Node = Cast<UVoxelGraphNodeBase>(DetailLayout.GetSelectedObjects()[0].Get());
	if (!ensure(Node.IsValid()))
	{
		return;
	}

	DetailLayout.HideCategory("Voxel");

	const TSharedRef<IPropertyHandle> PreviewedPinHandle = DetailLayout.GetProperty(GET_MEMBER_NAME_STATIC(UVoxelGraphNodeBase, PreviewedPin), UVoxelGraphNodeBase::StaticClass());
	PreviewedPinHandle->MarkHiddenByCustomization();
	PreviewedPinHandle->SetOnPropertyValueChanged(FVoxelEditorUtilities::MakeRefreshDelegate(PreviewedPinHandle, DetailLayout));

	const TSharedRef<IPropertyHandle> PreviewSettingsHandle = DetailLayout.GetProperty(GET_MEMBER_NAME_STATIC(UVoxelGraphNodeBase, PreviewSettings), UVoxelGraphNodeBase::StaticClass());
	PreviewSettingsHandle->MarkHiddenByCustomization();

	uint32 NumChildren = 0;
	if (!ensure(PreviewSettingsHandle->GetNumChildren(NumChildren) == FPropertyAccess::Success))
	{
		return;
	}

	if (NumChildren == 0)
	{
		return;
	}

	IDetailCategoryBuilder& Category = DetailLayout.EditCategory(STATIC_FNAME("Preview"));

	Category.AddCustomRow(INVTEXT("Previewed Pin"))
	.NameContent()
	[
		PreviewedPinHandle->CreatePropertyNameWidget()
	]
	.ValueContent()
	[
		SNew(SBox)
		.MinDesiredWidth(125.f)
		[
			SNew(SVoxelDetailComboBox<FName>)
			.RefreshDelegate(PreviewedPinHandle, DetailLayout)
			.Options_Lambda([=]() -> TArray<FName>
			{
				if (!ensure(Node.IsValid()))
				{
					return {};
				}

				TArray<FName> Names;
				for (const UEdGraphPin* Pin : Node->GetOutputPins())
				{
					if (Pin->ParentPin)
					{
						continue;
					}
					Names.Add(Pin->PinName);
				}
				return Names;
			})
			.CurrentOption(Node->PreviewedPin)
			.OptionText(MakeLambdaDelegate([](const FName Option)
			{
				return Option.ToString();
			}))
			.OnSelection_Lambda([PreviewedPinHandle](const FName NewValue)
			{
				PreviewedPinHandle->SetValue(NewValue);
			})
		]
	];

	FVoxelPinType PinType;
	TSharedPtr<IPropertyHandle> PreviewHandlerHandle;
	for (uint32 Index = 0; Index < NumChildren; Index++)
	{
		const TSharedPtr<IPropertyHandle> ChildHandle = PreviewSettingsHandle->GetChildHandle(Index);
		if (!ensure(ChildHandle))
		{
			continue;
		}

		const TSharedPtr<IPropertyHandle> PinNameHandle = ChildHandle->GetChildHandle(GET_MEMBER_NAME_STATIC(FVoxelPinPreviewSettings, PinName));
		const TSharedPtr<IPropertyHandle> PinTypeHandle = ChildHandle->GetChildHandle(GET_MEMBER_NAME_STATIC(FVoxelPinPreviewSettings, PinType));
		if (!ensure(PinNameHandle) ||
			!ensure(PinTypeHandle))
		{
			continue;
		}

		FName PinName;
		if (!ensure(PinNameHandle->GetValue(PinName) == FPropertyAccess::Success) ||
			PinName != Node->PreviewedPin)
		{
			continue;
		}

		PinType = FVoxelEditorUtilities::GetStructPropertyValue<FEdGraphPinType>(PinTypeHandle);

		PreviewHandlerHandle = ChildHandle->GetChildHandle(GET_MEMBER_NAME_STATIC(FVoxelPinPreviewSettings, PreviewHandler));
		ensure(PreviewHandlerHandle);
		break;
	}

	if (!ensureVoxelSlow(PreviewHandlerHandle))
	{
		return;
	}

	Category.AddCustomRow(INVTEXT("Preview Type"))
	.NameContent()
	[
		PreviewedPinHandle->CreatePropertyNameWidget(INVTEXT("Preview Type"))
	]
	.ValueContent()
	[
		SNew(SBox)
		.MinDesiredWidth(125.f)
		[
			SNew(SVoxelDetailComboBox<UScriptStruct*>)
			.RefreshDelegate(PreviewedPinHandle, DetailLayout)
			.Options_Lambda([=]() -> TArray<UScriptStruct*>
			{
				if (!ensure(Node.IsValid()))
				{
					return {};
				}

				TArray<UScriptStruct*> Structs;
				for (const FVoxelPreviewHandler* Handler : FVoxelPreviewHandler::GetHandlers())
				{
					if (!Handler->SupportsType(PinType))
					{
						continue;
					}
					Structs.Add(Handler->GetStruct());
				}
				return Structs;
			})
			.CurrentOption(FVoxelEditorUtilities::GetStructPropertyValue<FVoxelInstancedStruct>(PreviewHandlerHandle).GetScriptStruct())
			.OptionText(MakeLambdaDelegate([](UScriptStruct* Option) -> FString
			{
				if (!Option)
				{
					return {};
				}
				return Option->GetDisplayNameText().ToString();
			}))
			.OnSelection_Lambda([PreviewHandlerHandle, RefreshDelegate = FVoxelEditorUtilities::MakeRefreshDelegate(PreviewHandlerHandle, DetailLayout)](UScriptStruct* NewValue)
			{
				FVoxelInstancedStruct* PreviewHandler = nullptr;
				if (!ensure(FVoxelEditorUtilities::GetPropertyValue(PreviewHandlerHandle, PreviewHandler)))
				{
					return;
				}

				PreviewHandlerHandle->NotifyPreChange();
				*PreviewHandler = FVoxelInstancedStruct(NewValue);
				PreviewHandlerHandle->NotifyPostChange(EPropertyChangeType::ValueSet);
				PreviewHandlerHandle->NotifyFinishedChangingProperties();

				(void)RefreshDelegate.ExecuteIfBound();
			})
		]
	];

	Wrapper = FVoxelStructCustomizationWrapper::Make(PreviewHandlerHandle.ToSharedRef());
	if (!Wrapper)
	{
		return;
	}

	for (const TSharedPtr<IPropertyHandle>& Handle : Wrapper->AddChildStructure())
	{
		Category.AddProperty(Handle);
	}
}
