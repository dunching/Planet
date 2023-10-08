// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelEditorMinimal.h"
#include "VoxelGraphInterface.h"
#include "Toolkits/VoxelSimpleAssetToolkit.h"
#include "VoxelGraphInterfaceToolkit.generated.h"

class AVoxelActor;

USTRUCT()
struct FVoxelGraphInterfaceToolkit : public FVoxelToolkit
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

	UPROPERTY()
	TObjectPtr<UVoxelGraphInterface> Asset;

public:
	//~ Begin FVoxelToolkit Interface
	virtual TArray<FMode> GetModes() const override;
	//~ End FVoxelToolkit Interface
};

USTRUCT(meta = (Internal))
struct FVoxelGraphInterfacePreviewToolkit : public FVoxelSimpleAssetToolkit
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

	UPROPERTY()
	TObjectPtr<UVoxelGraphInterface> Asset;

public:
	virtual ~FVoxelGraphInterfacePreviewToolkit() override;

	//~ Begin FVoxelSimpleAssetToolkit Interface
	virtual bool ShowFloor() const override { return false; }
	virtual void SetupPreview() override;
	virtual TOptional<float> GetInitialViewDistance() const override;
	//~ End FVoxelSimpleAssetToolkit Interface

public:
	FORCEINLINE UWorld* GetWorld() const
	{
		return CachedWorld.Get();
	}
	FORCEINLINE USceneComponent* GetRootComponent() const
	{
		return PrivateRootComponent;
	}

public:
	template<typename T>
	T* SpawnActor()
	{
		UWorld* World = GetWorld();
		if (!ensure(World))
		{
			return nullptr;
		}

		FActorSpawnParameters Parameters;
		Parameters.bNoFail = true;
		Parameters.ObjectFlags = RF_Transient;
		Parameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Parameters.Name = MakeUniqueObjectName(World, T::StaticClass(), FName(T::StaticClass()->GetName() + "_PreviewScene"));
		T* Actor = World->SpawnActor<T>(Parameters);
		if (!ensure(Actor))
		{
			return nullptr;
		}

		PrivateActors.Add(Actor);
		return Actor;
	}
	template<typename T>
	T* CreateComponent()
	{
		USceneComponent* RootComponent = GetRootComponent();
		if (!ensure(RootComponent))
		{
			return nullptr;
		}

		AActor* Actor = RootComponent->GetOwner();
		if (!ensure(Actor))
		{
			return nullptr;
		}

		T* Component = NewObject<T>(Actor, NAME_None, RF_Transient);
		if (!ensure(Component))
		{
			return nullptr;
		}

		Component->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		Component->SetWorldTransform(FTransform::Identity);
		Component->RegisterComponent();
		return Component;
	}

private:
	TWeakObjectPtr<UWorld> CachedWorld;

	UPROPERTY()
	TObjectPtr<AVoxelActor> VoxelActor;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> PrivateActors;

	UPROPERTY()
	TObjectPtr<USceneComponent> PrivateRootComponent;
};