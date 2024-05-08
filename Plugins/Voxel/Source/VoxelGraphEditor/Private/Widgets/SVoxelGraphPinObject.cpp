// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "SVoxelGraphPinObject.h"
#include "VoxelPinType.h"
#include "SLevelOfDetailBranchNode.h"
#include "SVoxelGraphObjectSelector.h"

void SVoxelGraphPinObject::Construct(const FArguments& InArgs, UEdGraphPin* InGraphPinObj)
{
	VOXEL_FUNCTION_COUNTER();

	SGraphPinObject::Construct(SGraphPinObject::FArguments(), InGraphPinObj);
	GetLabelAndValue()->SetWrapSize(300.f);

	const auto GetFullWidgetSize = [this]
	{
		static FVector2D DefaultSize(20.f, 57.f);

		const TSharedPtr<SHorizontalBox> Box = FullPinHorizontalRowWidget.Pin();
		if (!Box)
		{
			return DefaultSize;
		}

		const FVector2D Size = Box->GetDesiredSize();
		if (Size == FVector2D::ZeroVector)
		{
			return DefaultSize;
		}

		return Size;
	};

	SBorder::Construct(SBorder::FArguments()
		.BorderImage(this, &SVoxelGraphPinObject::GetPinBorder)
		.BorderBackgroundColor(this, &SVoxelGraphPinObject::GetHighlightColor)
		.OnMouseButtonDown(this, &SVoxelGraphPinObject::OnPinNameMouseDown)
		[
			SNew(SBorder)
			.BorderImage(CachedImg_Pin_DiffOutline)
			.BorderBackgroundColor(this, &SVoxelGraphPinObject::GetPinDiffColor)
			[
				SNew(SLevelOfDetailBranchNode)
				.UseLowDetailSlot(this, &SVoxelGraphPinObject::UseLowDetailPinNames)
				.LowDetail()
				[
					SNew(SBox)
					.WidthOverride_Lambda([=]
					{
						return GetFullWidgetSize().X;
					})
					.HeightOverride_Lambda([=]
					{
						return GetFullWidgetSize().Y;
					})
					.HAlign(GetDirection() == EGPD_Input ? HAlign_Left : HAlign_Right)
					.VAlign(VAlign_Center)
					[
						PinImage.ToSharedRef()
					]
				]
				.HighDetail()
				[
					FullPinHorizontalRowWidget.Pin().ToSharedRef()
				]
			]
		]
	);
}

TSharedRef<SWidget>	SVoxelGraphPinObject::GetDefaultValueWidget()
{
	VOXEL_FUNCTION_COUNTER();

	if (!ensure(GraphPinObj))
	{
		return SNullWidget::NullWidget;
	}

	if (!ensureVoxelSlow(FVoxelPinType(GraphPinObj->PinType).IsValid()))
	{
		// Will happen if we remove a FVoxelObjectPinType
		return
			SNew(SBox)
			.MaxDesiredWidth(200.f)
			[
				SNew(SVoxelGraphObjectSelector)
				.Visibility(this, &SGraphPin::GetDefaultValueVisibility)
				.IsEnabled(false)
				.AllowedClass(UObject::StaticClass())
				.ThumbnailPool(FVoxelEditorUtilities::GetThumbnailPool())
				.ObjectPath_Lambda([this]
				{
					return GetAssetData(true).GetSoftObjectPath().ToString();
				})
				.OnObjectChanged(this, &SVoxelGraphPinObject::OnAssetSelectedFromPicker)
			];
	}

	const FVoxelPinType Type = FVoxelPinType(GraphPinObj->PinType).GetPinDefaultValueType();
	if (!ensure(Type.IsObject()))
	{
		return SNullWidget::NullWidget;
	}

	UClass* AllowedClass = Type.GetObjectClass();
	if (!ensure(AllowedClass))
	{
		return SNullWidget::NullWidget;
	}

	return
		SNew(SBox)
		.MaxDesiredWidth(200.f)
		[
			SNew(SVoxelGraphObjectSelector)
			.Visibility(this, &SGraphPin::GetDefaultValueVisibility)
			.IsEnabled(this, &SGraphPin::IsEditingEnabled)
			.AllowedClass(AllowedClass)
			.ThumbnailPool(FVoxelEditorUtilities::GetThumbnailPool())
			.ObjectPath_Lambda([this]
			{
				return GetAssetData(true).GetSoftObjectPath().ToString();
			})
			.OnObjectChanged(this, &SVoxelGraphPinObject::OnAssetSelectedFromPicker)
		];
}

void SVoxelGraphPinObject::OnAssetSelectedFromPicker(const FAssetData& AssetData)
{
	VOXEL_FUNCTION_COUNTER();

	if (!ensure(!GraphPinObj->IsPendingKill()) ||
		AssetData == GetAssetData(true))
	{
		return;
	}

	const FVoxelTransaction Transaction(GraphPinObj, "Change Object Pin Value");

	GraphPinObj->GetSchema()->TrySetDefaultObject(*GraphPinObj, AssetData.GetAsset());
}