// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "SVoxelGraphPinBodyInstance.h"

#include "SGraphPinComboBox.h"
#include "VoxelPinType.h"
#include "VoxelPinValue.h"

void SVoxelGraphPinBodyInstance::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj)
{
	SGraphPin::Construct(SGraphPin::FArguments(), InGraphPinObj);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedRef<SWidget> SVoxelGraphPinBodyInstance::GetDefaultValueWidget()
{
	const FVoxelPinType InnerType = FVoxelPinType(GraphPinObj->PinType).GetPinDefaultValueType();
	if (!ensure(InnerType.Is<FBodyInstance>()))
	{
		return SNullWidget::NullWidget;
	}

	const UEnum* Enum = StaticEnum<ECollisionEnabled::Type>();
	if (!ensure(Enum))
	{
		return SNullWidget::NullWidget;
	}

	TArray<TSharedPtr<int32>> Items;

	// NumEnums() - 1 is _MAX
	for (int32 Index = 0; Index < Enum->NumEnums() - 1; Index++)
	{
		if (Enum->HasMetaData(TEXT("Hidden"), Index))
		{
			continue;
		}

		Items.Add(MakeSharedCopy(Index));
	}

	return SNew(SPinComboBox)
		.ComboItemList(Items)
		.VisibleText_Lambda([=]() -> FString
		{
			const FVoxelPinValue PinValue = FVoxelPinValue::MakeFromPinDefaultValue(*GraphPinObj);
			if (!ensure(PinValue.Is<FBodyInstance>()))
			{
				return "(INVALID)";
			}

			const FBodyInstance& BodyInstance = PinValue.Get<FBodyInstance>();

			// NumEnums() - 1 is _MAX
			for (int32 Index = 0; Index < Enum->NumEnums() - 1; Index++)
			{
				if (Enum->HasMetaData(TEXT("Hidden"), Index))
				{
					continue;
				}

				if (BodyInstance.GetCollisionEnabled() == Enum->GetValueByIndex(Index))
				{
					return Enum->GetDisplayNameTextByIndex(Index).ToString();
				}
			}

			return "(INVALID)";
		})
		.OnSelectionChanged_Lambda([=](const TSharedPtr<int32>& NewSelection, ESelectInfo::Type)
		{
			const FVoxelPinValue PinValue = FVoxelPinValue::MakeFromPinDefaultValue(*GraphPinObj);
			if (!ensure(PinValue.Is<FBodyInstance>()))
			{
				return;
			}

			FBodyInstance BodyInstance = PinValue.Get<FBodyInstance>();

			int32 NewValue;
			if (NewSelection.IsValid())
			{
				NewValue = Enum->GetValueByIndex(*NewSelection);
			}
			else
			{
				NewValue = 0;
			}

			if (BodyInstance.GetCollisionEnabled() == NewValue)
			{
				return;
			}

			BodyInstance.SetCollisionEnabled(ECollisionEnabled::Type(NewValue));

			const FVoxelTransaction Transaction(GraphPinObj, "Change body instance collision settings");
			GraphPinObj->GetSchema()->TrySetDefaultValue(*GraphPinObj, FVoxelPinValue::Make(BodyInstance).ExportToString());
		})
		.IsEnabled(this, &SGraphPin::GetDefaultValueIsEditable)
		.Visibility(this, &SGraphPin::GetDefaultValueVisibility)
		.OnGetDisplayName_Lambda([=](const int32 Index)
		{
			return Enum->GetDisplayNameTextByIndex(Index);
		})
		.OnGetTooltip_Lambda([=](const int32 Index)
		{
			return Enum->GetToolTipTextByIndex(Index);
		});
}