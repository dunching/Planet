// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "SVoxelNewAssetInstanceFilterBox.generated.h"

UENUM()
enum class EVoxelGraphScriptSource : uint8
{
	Voxel,
	VoxelExamples,
	Game,
	Plugins,
	Developer,
	Unknown
};

class SVoxelNewAssetInstanceSourceFilterCheckBox : public SCheckBox
{
public:
	DECLARE_DELEGATE_TwoParams(FOnSourceStateChanged, EVoxelGraphScriptSource, bool);
	DECLARE_DELEGATE_TwoParams(FOnShiftClicked, EVoxelGraphScriptSource, bool);

	VOXEL_SLATE_ARGS()
	{
		SLATE_EVENT(FOnSourceStateChanged, OnSourceStateChanged)
		SLATE_EVENT(FOnShiftClicked, OnShiftClicked)
		SLATE_ATTRIBUTE(ECheckBoxState, IsChecked)
	};

	void Construct(const FArguments& Args, EVoxelGraphScriptSource Source);

	//~ Begin SCheckBox Interface
	virtual FReply OnMouseButtonUp(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	//~ End SCheckBox Interface

private:
	FSlateColor GetTextColor() const;
	FSlateColor GetScriptSourceColor() const;
	FMargin GetFilterNamePadding() const;

private:
	EVoxelGraphScriptSource Source = EVoxelGraphScriptSource::Voxel;

	FOnSourceStateChanged OnSourceStateChanged;
	FOnShiftClicked OnShiftClicked;
};

class SVoxelNewAssetInstanceSourceFilterBox : public SCompoundWidget
{
public:
	typedef TMap<EVoxelGraphScriptSource, bool> FSourceMap;
	DECLARE_DELEGATE_OneParam(FOnFiltersChanged, const FSourceMap&);

	VOXEL_SLATE_ARGS()
	{
		SLATE_EVENT(FOnFiltersChanged, OnFiltersChanged)
	};

	void Construct(const FArguments& Args);

public:
	static bool IsFilterActive(const EVoxelGraphScriptSource Source)
	{
		if (Source == EVoxelGraphScriptSource::Voxel)
		{
			return true;
		}

		if (SourceState.Contains(Source))
		{
			return SourceState[Source];
		}

		return true;
	}

private:
	void BroadcastFiltersChanged() const
	{
		OnFiltersChanged.Execute(SourceState);
	}

	static EVoxelGraphScriptSource GetScriptSource(const int32 Index)
	{
		static const UEnum* ScriptSourceEnum = StaticEnum<EVoxelGraphScriptSource>();
		return EVoxelGraphScriptSource(ScriptSourceEnum->GetValueByIndex(Index));
	}

	static bool IsFilterActive(const int32 Index)
	{
		return IsFilterActive(GetScriptSource(Index));
	}

private:
	FOnFiltersChanged OnFiltersChanged;
	TMap<EVoxelGraphScriptSource, TSharedRef<SVoxelNewAssetInstanceSourceFilterCheckBox>> SourceButtons;

	static TMap<EVoxelGraphScriptSource, bool> SourceState;
};

class SVoxelNewAssetInstanceFilterBox : public SCompoundWidget
{
public:
	VOXEL_SLATE_ARGS()
	{
		SLATE_EVENT(SVoxelNewAssetInstanceSourceFilterBox::FOnFiltersChanged, OnSourceFiltersChanged)
		SLATE_ARGUMENT(UClass*, Class)
	};

	void Construct(const FArguments& InArgs);

	bool IsSourceFilterActive(const FAssetData& Item) const;

private:
	static EVoxelGraphScriptSource GetScriptSource(const FAssetData& ScriptAssetData);

private:
	TSharedPtr<SVoxelNewAssetInstanceSourceFilterBox> SourceFilterBox;
};