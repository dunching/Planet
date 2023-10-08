// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelVoxelizedMeshAsset.h"
#include "VoxelVoxelizedMeshData.h"

#include "MeshDescription.h"
#include "Engine/StaticMesh.h"
#include "Misc/ConfigCacheIni.h"
#include "AssetRegistry/AssetData.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Serialization/BufferArchive.h"

#if WITH_EDITOR
#include "AssetToolsModule.h"
#include "EditorReimportHandler.h"
#include "DerivedDataCacheInterface.h"
#endif

DEFINE_VOXEL_FACTORY(UVoxelVoxelizedMeshAsset);

TSharedPtr<const FVoxelVoxelizedMeshData> UVoxelVoxelizedMeshAsset::GetMeshData() const
{
	if (MeshData)
	{
		if (MeshData->VoxelSize != VoxelSize ||
			MeshData->MaxSmoothness != MaxSmoothness ||
			MeshData->VoxelizerSettings != VoxelizerSettings)
		{
			MeshData.Reset();
		}
	}

	if (!MeshData)
	{
		TryCreateMeshData();
	}

	return MeshData;
}

#if WITH_EDITOR
VOXEL_RUN_ON_STARTUP_GAME(RegisterVoxelVoxelizedMeshAssetOnReimport)
{
	FReimportManager::Instance()->OnPostReimport().AddLambda([](UObject* Asset, bool bSuccess)
	{
		UVoxelVoxelizedMeshAsset::OnReimport();
	});
}

void UVoxelVoxelizedMeshAsset::OnReimport()
{
	ForEachObjectOfClass<UVoxelVoxelizedMeshAsset>([&](const UVoxelVoxelizedMeshAsset* Asset)
	{
		Asset->MeshData.Reset();
	});
}
#endif

void UVoxelVoxelizedMeshAsset::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	bool bCooked = Ar.IsCooking();
	Ar << bCooked;

	if (bCooked && !IsTemplate() && !Ar.IsCountingMemory())
	{
		if (Ar.IsLoading())
		{
			const TSharedRef<FVoxelVoxelizedMeshData> NewMeshData = MakeVoxelShared<FVoxelVoxelizedMeshData>();
			NewMeshData->Serialize(Ar);
			MeshData = NewMeshData;
		}
#if WITH_EDITOR
		else if (Ar.IsSaving())
		{
			if (!MeshData)
			{
				TryCreateMeshData();
			}
			if (!MeshData)
			{
				MeshData = MakeVoxelShared<FVoxelVoxelizedMeshData>();
			}
			ConstCast(*MeshData).Serialize(Ar);
		}
#endif
	}
}

#if WITH_EDITOR
void UVoxelVoxelizedMeshAsset::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	VOXEL_FUNCTION_COUNTER();

	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.ChangeType == EPropertyChangeType::Interactive)
	{
		return;
	}

	// In case mesh changed
	MeshData.Reset();

	// Refresh DataSize/MemorySizeInMB
	(void)GetMeshData();
}
#endif

void UVoxelVoxelizedMeshAsset::TryCreateMeshData() const
{
	VOXEL_FUNCTION_COUNTER();
	ensure(!MeshData);

	UStaticMesh* LoadedMesh = Mesh.LoadSynchronous();
	if (!LoadedMesh)
	{
		return;
	}

#if WITH_EDITOR
	FString KeySuffix;

	const FStaticMeshSourceModel& SourceModel = LoadedMesh->GetSourceModel(0);
	if (ensure(SourceModel.GetMeshDescriptionBulkData()))
	{
		KeySuffix += "MD";
		KeySuffix += SourceModel.GetMeshDescriptionBulkData()->GetIdString();
	}

	{
		FBufferArchive Writer;
		Writer << ConstCast(VoxelSize);
		Writer << ConstCast(MaxSmoothness);
		Writer << ConstCast(VoxelizerSettings);

		KeySuffix += "_" + FString::FromHexBlob(Writer.GetData(), Writer.Num());
	}

	const FString DerivedDataKey = FDerivedDataCacheInterface::BuildCacheKey(
		TEXT("VOXEL_VOXELIZED_MESH"),
		TEXT("08F933C446024945833DF1C52EF6806E"),
		*KeySuffix);

	TArray<uint8> DerivedData;
	if (GetDerivedDataCacheRef().GetSynchronous(*DerivedDataKey, DerivedData, GetPathName()))
	{
		FMemoryReader Ar(DerivedData);

		const TSharedRef<FVoxelVoxelizedMeshData> NewMeshData = MakeVoxelShared<FVoxelVoxelizedMeshData>();
		NewMeshData->Serialize(Ar);
		MeshData = NewMeshData;
	}
	else
	{
		MeshData = FVoxelVoxelizedMeshData::VoxelizeMesh(*LoadedMesh, VoxelSize, MaxSmoothness, VoxelizerSettings);
		if (!MeshData)
		{
			return;
		}

		FBufferArchive Writer;
		ConstCast(*MeshData).Serialize(Writer);
		GetDerivedDataCacheRef().Put(*DerivedDataKey, Writer, GetPathName());
	}

	DataSize = MeshData->Size;
	MemorySizeInMB = MeshData->GetAllocatedSize() / double(1 << 20);
#else
	MeshData = FVoxelVoxelizedMeshData::VoxelizeMesh(*LoadedMesh, VoxelSize, MaxSmoothness, VoxelizerSettings);
#endif
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

UVoxelVoxelizedMeshAsset* UVoxelVoxelizedMeshAssetUserData::GetAsset(UStaticMesh& Mesh)
{
	if (const UVoxelVoxelizedMeshAssetUserData* AssetUserData = Mesh.GetAssetUserData<UVoxelVoxelizedMeshAssetUserData>())
	{
		if (UVoxelVoxelizedMeshAsset* Asset = AssetUserData->Asset.LoadSynchronous())
		{
			if (Asset->Mesh == &Mesh)
			{
				return Asset;
			}
		}
	}

	UVoxelVoxelizedMeshAsset* Asset = nullptr;

	// Try to find an existing one
	{
		TArray<FAssetData> AssetDatas;
		FARFilter Filter;
		Filter.ClassPaths.Add(UVoxelVoxelizedMeshAsset::StaticClass()->GetClassPathName());
		Filter.TagsAndValues.Add(GET_MEMBER_NAME_CHECKED(UVoxelVoxelizedMeshAsset, Mesh), Mesh.GetPathName());

		const IAssetRegistry& AssetRegistry = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
		AssetRegistry.GetAssets(Filter, AssetDatas);

		if (AssetDatas.Num() > 1)
		{
			TArray<UObject*> Assets;
			for (const FAssetData& AssetData : AssetDatas)
			{
				Assets.Add(AssetData.GetAsset());
			}
			VOXEL_MESSAGE(Warning, "More than 1 voxelized mesh asset for mesh {0} found: {1}", Mesh, Assets);
		}

		for (const FAssetData& AssetData : AssetDatas)
		{
			UObject* Object = AssetData.GetAsset();
			if (!ensure(Object) ||
				!ensure(Object->IsA<UVoxelVoxelizedMeshAsset>()))
			{
				continue;
			}

			Asset = CastChecked<UVoxelVoxelizedMeshAsset>(Object);
			break;
		}

		if (!Asset &&
			AssetRegistry.IsLoadingAssets())
		{
			// Otherwise new assets are created for the same mesh
			VOXEL_MESSAGE(Error, "Asset registry is still loading assets - please refresh the voxel actor (Ctrl F5) once assets are loaded");
			return nullptr;
		}
	}

	if (!Asset)
	{
#if WITH_EDITOR
		// Create a new voxelized asset

		FString PackageName = FPackageName::ObjectPathToPackageName(Mesh.GetPathName());
		if (!PackageName.StartsWith("/Game/"))
		{
			// Don't create assets in the engine
			PackageName = "/Game/VoxelizedMeshes/" + Mesh.GetName();
		}

		Asset = FVoxelObjectUtilities::CreateNewAsset_Direct<UVoxelVoxelizedMeshAsset>(PackageName, "_Voxelized");

		if (!Asset)
		{
			return nullptr;
		}

		Asset->Mesh = &Mesh;
#else
		// TODO Support this
		VOXEL_MESSAGE(Error, "Trying to create a voxelized mesh from {0} at runtime. Please do it in editor instead", Mesh);
		return nullptr;
#endif
	}

	ensure(Asset->Mesh == &Mesh);

	UVoxelVoxelizedMeshAssetUserData* AssetUserData = NewObject<UVoxelVoxelizedMeshAssetUserData>(&Mesh);
	AssetUserData->Asset = Asset;

	Mesh.AddAssetUserData(AssetUserData);
	Mesh.MarkPackageDirty();

	return Asset;
}