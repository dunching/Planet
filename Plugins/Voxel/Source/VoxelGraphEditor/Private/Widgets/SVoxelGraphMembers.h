// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "VoxelGraph.h"
#include "Widgets/SVoxelMembers.h"

class SVoxelGraphMembers : public SVoxelMembers
{
public:
	enum class ESection
	{
		None,
		Graph,
		InlineMacros,
		MacroLibraries,
		Parameters,
		MacroInputs,
		MacroOutputs,
		LocalVariables,
	};

	FORCEINLINE static int32 GetSectionId(const ESection Type)
	{
		switch (Type)
		{
		default: check(false);
		case ESection::None: return 0;
		case ESection::Graph: return 1;
		case ESection::InlineMacros: return 2;
		case ESection::MacroLibraries: return 3;
		case ESection::Parameters: return 4;
		case ESection::MacroInputs: return 5;
		case ESection::MacroOutputs: return 6;
		case ESection::LocalVariables: return 7;
		}
	}
	FORCEINLINE static ESection GetSection(const int32 SectionId)
	{
		switch (SectionId)
		{
		case 1: return ESection::Graph;
		case 2: return ESection::InlineMacros;
		case 3: return ESection::MacroLibraries;
		case 4: return ESection::Parameters;
		case 5: return ESection::MacroInputs;
		case 6: return ESection::MacroOutputs;
		case 7: return ESection::LocalVariables;
		default: ensure(SectionId == 0);
			return ESection::None;
		}
	}
	FORCEINLINE static ESection GetSection(const EVoxelGraphParameterType Type)
	{
		switch (Type)
		{
		default: ensure(false);
		case EVoxelGraphParameterType::Parameter: return ESection::Parameters;
		case EVoxelGraphParameterType::Input: return ESection::MacroInputs;
		case EVoxelGraphParameterType::Output: return ESection::MacroOutputs;
		case EVoxelGraphParameterType::LocalVariable: return ESection::LocalVariables;
		}
	}
	FORCEINLINE static int32 GetSectionId(const EVoxelGraphParameterType Type)
	{
		return GetSectionId(GetSection(Type));
	}
	FORCEINLINE static EVoxelGraphParameterType GetParameterType(const ESection Type)
	{
		switch (Type)
		{
		default: ensure(false);
		case ESection::Parameters: return EVoxelGraphParameterType::Parameter;
		case ESection::MacroInputs: return EVoxelGraphParameterType::Input;
		case ESection::MacroOutputs: return EVoxelGraphParameterType::Output;
		case ESection::LocalVariables: return EVoxelGraphParameterType::LocalVariable;
		}
	}

public:
	VOXEL_SLATE_ARGS()
	{
		SLATE_ARGUMENT(UVoxelGraph*, Graph);
		SLATE_ARGUMENT(TSharedPtr<FVoxelToolkit>, Toolkit);
	};

	void Construct(const FArguments& Args);
	void UpdateActiveGraph(const TWeakObjectPtr<UVoxelGraph>& NewActiveGraph);

protected:
	//~ Begin SVoxelMembers Interface
	virtual void CollectStaticSections(TArray<int32>& StaticSectionIds) override;
	virtual FText OnGetSectionTitle(int32 SectionId) override;
	virtual TSharedRef<SWidget> OnGetMenuSectionWidget(TSharedRef<SWidget> RowWidget, int32 SectionId) override;
	virtual void CollectSortedActions(FVoxelMembersActionsSortHelper& OutActionsList) override;
	virtual void SelectBaseObject() override;
	virtual void GetContextMenuAddOptions(FMenuBuilder& MenuBuilder) override;
	virtual void OnPasteItem(const FString& ImportText, int32 SectionId) override;
	virtual bool CanPasteItem(const FString& ImportText, int32 SectionId) override;
	virtual void OnAddNewMember(int32 SectionId) override;
	virtual const TArray<FString>& GetCopyPrefixes() const override;

public:
	virtual TArray<FString>& GetEditableCategories(int32 SectionId) override;
	//~ End SVoxelMembers Interface

private:
	void OnParametersChanged(UVoxelGraph::EParameterChangeType ChangeType);

private:
	FDelegateHandle OnMembersChangedHandle;
};