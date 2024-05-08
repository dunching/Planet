// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "RichCurveEditorModel.h"
#include "Tree/ICurveEditorTreeItem.h"
#include "VoxelCurveCustomization.generated.h"

class SVoxelCurveThumbnail;

UCLASS()
class UVoxelCurveObject_Temp : public UObject
{
	GENERATED_BODY()
};

class FVoxelCurveEditorModel : public FRichCurveEditorModel
{
public:
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnPostCurveChange, const FRichCurve&);

public:
	explicit FVoxelCurveEditorModel(const TSharedPtr<FRichCurve>& InRichCurve)
		: FRichCurveEditorModel(NewObject<UVoxelCurveObject_Temp>(GetTransientPackage()))
		, Curve(InRichCurve)
	{
	}

	//~ Begin FRichCurveEditorModel Interface
	virtual auto AddKeys(TArrayView<const FKeyPosition> InKeyPositions, TArrayView<const FKeyAttributes> InAttributes, TArrayView<TOptional<FKeyHandle>>* OutKeyHandles) -> void override;
	virtual void RemoveKeys(TArrayView<const FKeyHandle> InKeys) override;
	virtual void SetKeyPositions(TArrayView<const FKeyHandle> InKeys, TArrayView<const FKeyPosition> InKeyPositions, EPropertyChangeType::Type ChangeType) override;
	virtual void SetKeyAttributes(TArrayView<const FKeyHandle> InKeys, TArrayView<const FKeyAttributes> InAttributes, EPropertyChangeType::Type ChangeType = EPropertyChangeType::Unspecified) override;
	virtual void SetCurveAttributes(const FCurveAttributes& InCurveAttributes) override;

	virtual bool IsReadOnly() const override { return false; }

	virtual bool IsValid() const override { return true; }
	virtual FRichCurve& GetRichCurve() override { return *Curve; }
	virtual const FRichCurve& GetReadOnlyRichCurve() const override { return *Curve; }
	//~ End FRichCurveEditorModel Interface

private:
	TSharedPtr<FRichCurve> Curve;

public:
	FSimpleMulticastDelegate CurvePreEditChangeDelegate;
	FOnPostCurveChange CurvePostEditChangeDelegate;
};

struct FVoxelCurveDetailsTreeItem
	: public ICurveEditorTreeItem
	, public TSharedFromThis<FVoxelCurveDetailsTreeItem>
{
	TSharedPtr<FRichCurve> Curve;

	explicit FVoxelCurveDetailsTreeItem(const TSharedPtr<FRichCurve>& Curve);

	//~ Begin ICurveEditorTreeItem Interface
	virtual TSharedPtr<SWidget> GenerateCurveEditorTreeWidget(const FName& InColumnName, TWeakPtr<FCurveEditor> InCurveEditor, FCurveEditorTreeItemID InTreeItemID, const TSharedRef<ITableRow>& TableRow) override;
	virtual void CreateCurveModels(TArray<TUniquePtr<FCurveModel>>& OutCurveModels) override;
	//~ End ICurveEditorTreeItem Interface

private:
	void PreEditChange() const { PreEditChangeDelegate.Broadcast(); }
	void PostEditChange(const FRichCurve& InCurve) const { PostEditChangeDelegate.Broadcast(InCurve); }

public:
	FSimpleMulticastDelegate PreEditChangeDelegate;
	FVoxelCurveEditorModel::FOnPostCurveChange PostEditChangeDelegate;
};

class FVoxelCurveCustomization
	: public IPropertyTypeCustomization
	, public FVoxelTicker
{
public:
	//~ Begin IPropertyTypeCustomization Interface
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> PropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> PropertyHandle, IDetailChildrenBuilder& ChildBuilder, IPropertyTypeCustomizationUtils& CustomizationUtils) override;
	virtual void Tick() override;
	//~ End IPropertyTypeCustomization Interface

private:
	TSharedRef<SWidget> MakeToolbar();
	void PostEditChange(const FRichCurve& InCurve) const;
	void CopySelectedCurve(const FAssetData& AssetData) const;
	void SaveCurveToAsset() const;

private:
	TSharedPtr<IPropertyHandle> CurveHandle;

	TWeakPtr<FCurveEditor> WeakCurveEditor;
	TWeakPtr<FVoxelCurveDetailsTreeItem> WeakTreeItem;
	TSharedPtr<FRichCurve> CachedCurve;
	TSharedPtr<SVoxelCurveThumbnail> CurveThumbnailWidget;
};