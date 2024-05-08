// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelGraphSearchManager.h"
#include "VoxelGraph.h"

FName FVoxelGraphSearchManager::GlobalSearchTabId = "VoxelGraphSearch";

FVoxelGraphSearchManager::FVoxelGraphSearchManager()
{
	Initialize();
}

FVoxelGraphSearchManager& FVoxelGraphSearchManager::Get()
{
	static FVoxelGraphSearchManager Manager;
	return Manager;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelGraphSearchManager::Initialize()
{
	VOXEL_FUNCTION_COUNTER();

	const TSharedRef<FGlobalTabmanager>& GlobalTabManager = FGlobalTabmanager::Get();

	if (!GlobalTabManager->HasTabSpawner(GlobalSearchTabId))
	{
		GlobalTabManager->RegisterNomadTabSpawner(GlobalSearchTabId, FOnSpawnTab::CreateRaw(this, &FVoxelGraphSearchManager::SpawnGlobalSearchWidgetTab))
			.SetDisplayName(INVTEXT("Find in Voxel Graphs"))
			.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "BlueprintEditor.FindInBlueprints.MenuIcon"));
	}

	AssetClasses.Add(UVoxelGraph::StaticClass(), nullptr);

	ForEachObjectOfClass<UClass>([&](const UClass* Class)
	{
		if (Class->HasAnyClassFlags(CLASS_Deprecated | CLASS_NewerVersionExists | CLASS_Hidden) ||
			Cast<UBlueprint>(Class->ClassGeneratedBy))
		{
			return;
		}

		for (FProperty& Property : GetClassProperties(Class))
		{
			if (const FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property))
			{
				if (ObjectProperty->PropertyClass->IsChildOf<UVoxelGraph>())
				{
					AssetClasses.Add(Class, ObjectProperty);
					break;
				}
			}
			else if (const FSoftObjectProperty* SoftObjectProperty = CastField<FSoftObjectProperty>(Property))
			{
				if (SoftObjectProperty->PropertyClass->IsChildOf<UVoxelGraph>())
				{
					AssetClasses.Add(Class, SoftObjectProperty);
					break;
				}
			}
		}
	});
}

TSharedRef<SDockTab> FVoxelGraphSearchManager::SpawnGlobalSearchWidgetTab(const FSpawnTabArgs& SpawnTabArgs)
{
	TSharedRef<SDockTab> NewTab = SNew(SDockTab)
		.TabRole(NomadTab)
		.Label(INVTEXT("Find in Voxel Graphs"))
		.ToolTipText(INVTEXT("Search for a string in all Voxel Graph assets"));

	const TSharedRef<SVoxelGraphSearch> Widget = SNew(SVoxelGraphSearch).IsGlobalSearch(true);
	NewTab->SetContent(Widget);
	GlobalSearchWidget = Widget;

	return NewTab;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

TSharedPtr<SVoxelGraphSearch> FVoxelGraphSearchManager::OpenGlobalSearch() const
{
	const TSharedPtr<SDockTab> NewTab = FGlobalTabmanager::Get()->TryInvokeTab(GlobalSearchTabId);
	if (ensure(NewTab))
	{
		return StaticCastSharedRef<SVoxelGraphSearch>(NewTab->GetContent());
	}

	return nullptr;
}

TMap<UObject*, UVoxelGraph*> FVoxelGraphSearchManager::GetAllLookupGraphs() const
{
	const FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));

	TArray<FAssetData> AssetDatas;

	FARFilter Filter;
	for (const auto& It : AssetClasses)
	{
		Filter.ClassPaths.Add(It.Key->GetClassPathName());
	}
	Filter.bRecursiveClasses = true;

	AssetRegistryModule.Get().GetAssets(Filter, AssetDatas);

	TMap<UObject*, UVoxelGraph*> Assets;
	for (const FAssetData& AssetData : AssetDatas)
	{
		UObject* Asset = AssetData.GetAsset();
		if (UVoxelGraph* Graph = Cast<UVoxelGraph>(Asset))
		{
			Assets.Add(Asset, Graph);
			continue;
		}

		const FProperty* Property = AssetClasses.FindRef(Asset->GetClass());
		if (!ensure(Property))
		{
			continue;
		}

		UVoxelGraph* Graph = nullptr;
		if (const FObjectProperty* ObjectProperty = CastField<FObjectProperty>(Property))
		{
			if (!ensure(ObjectProperty->PropertyClass->IsChildOf<UVoxelGraph>()))
			{
				continue;
			}

			Graph = Cast<UVoxelGraph>(ObjectProperty->GetObjectPropertyValue_InContainer(Asset));
		}
		else if (const FSoftObjectProperty* SoftObjectProperty = CastField<FSoftObjectProperty>(Property))
		{
			if (!ensure(SoftObjectProperty->PropertyClass->IsChildOf<UVoxelGraph>()))
			{
				continue;
			}

			Graph = Cast<UVoxelGraph>(SoftObjectProperty->GetObjectPropertyValue_InContainer(Asset));
		}

		if (!Graph)
		{
			continue;
		}

		Assets.Add(Asset, Graph);
	}

	return Assets;
}