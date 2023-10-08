// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelBuffer.h"
#include "VoxelObjectPinType.h"
#include "VoxelObjectWithGuid.h"
#include "VoxelParameterProvider.h"
#include "VoxelMaterialDefinitionInterface.generated.h"

class UVoxelMaterialDefinition;
class UVoxelMaterialDefinitionInstance;

UCLASS(Abstract, BlueprintType, meta = (AssetColor = Red))
class VOXELGRAPHCORE_API UVoxelMaterialDefinitionInterface
	: public UVoxelObjectWithGuid
	, public IVoxelParameterProvider
{
	GENERATED_BODY()

public:
	virtual UVoxelMaterialDefinition* GetDefinition() const VOXEL_PURE_VIRTUAL({});

public:
	UMaterialInterface* GetPreviewMaterial();

private:
	TSharedPtr<FVoxelMaterialRef> PrivatePreviewMaterial;
	TWeakObjectPtr<UMaterialInterface> PrivatePreviewMaterialParent;
};

constexpr uint16 GVoxelInvalidMaterialId = MAX_uint16;

USTRUCT(DisplayName = "Voxel Material Definition")
struct FVoxelMaterialDefinitionRef
{
	GENERATED_BODY()

	UPROPERTY()
	uint16 Index = 0;
};
checkStatic(sizeof(FVoxelMaterialDefinitionRef) == sizeof(uint16));

DECLARE_VOXEL_TERMINAL_BUFFER(FVoxelMaterialDefinitionBuffer, FVoxelMaterialDefinitionRef);

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelMaterialDefinitionBuffer final : public FVoxelSimpleTerminalBuffer
{
	GENERATED_BODY()
	GENERATED_VOXEL_TERMINAL_BUFFER_BODY(FVoxelMaterialDefinitionBuffer, FVoxelMaterialDefinitionRef);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelMaterialDefinitionDynamicMaterialParameter : public FVoxelDynamicMaterialParameter
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

	FSimpleMulticastDelegate OnChangedMulticast;

	virtual void Apply(FName Name, UMaterialInstanceDynamic& Instance) const override;
	virtual void AddOnChanged(const FSimpleDelegate& OnChanged) override;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT()
struct FVoxelMaterialParameterData : public FVoxelVirtualStruct
{
	GENERATED_BODY()
	GENERATED_VIRTUAL_STRUCT_BODY()

	struct FCachedParameters
	{
		TVoxelMap<FName, float> ScalarParameters;
		TVoxelMap<FName, FVector4> VectorParameters;
		TVoxelMap<FName, TWeakObjectPtr<UTexture>> TextureParameters;
	};
	virtual void Fixup() {}
	virtual void CacheParameters(
		FName Name,
		FCachedParameters& InOutParameters) const VOXEL_PURE_VIRTUAL();
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

constexpr int32 GVoxelMaterialDefinitionMax = 1 << 12;

class VOXELGRAPHCORE_API FVoxelMaterialDefinitionManager : public FVoxelSingleton
{
public:
	const TSharedRef<FVoxelMaterialDefinitionDynamicMaterialParameter> DynamicParameter = MakeVoxelShared<FVoxelMaterialDefinitionDynamicMaterialParameter>();

	FVoxelMaterialDefinitionManager();

	FVoxelMaterialDefinitionRef Register_GameThread(UVoxelMaterialDefinitionInterface& Material);
	UVoxelMaterialDefinitionInterface* GetMaterial_GameThread(const FVoxelMaterialDefinitionRef& Ref);
	TWeakObjectPtr<UVoxelMaterialDefinitionInterface> GetMaterial_AnyThread(const FVoxelMaterialDefinitionRef& Ref);

	//~ Begin FVoxelSingleton Interface
	virtual void Tick() override;
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	//~ End FVoxelSingleton Interface

	void QueueMaterialRefresh();
	void QueueRebuildTextures(UVoxelMaterialDefinition& Definition);

	void CacheParameters();
	void SetAllParameters(UMaterialInstanceDynamic& Instance);

	void LogIds();

private:
	bool bMaterialRefreshQueued = false;
	TSet<UVoxelMaterialDefinition*> MaterialDefinitionsToRebuild;

	TVoxelArray<UVoxelMaterialDefinitionInterface*> Materials;
	TVoxelMap<UVoxelMaterialDefinitionInterface*, FVoxelMaterialDefinitionRef> MaterialRefs;

	TUniquePtr<FVoxelMaterialParameterData::FCachedParameters> CachedParameters;

	FVoxelFastCriticalSection CriticalSection;
	TVoxelArray<TWeakObjectPtr<UVoxelMaterialDefinitionInterface>> WeakMaterials_RequiresLock;
};

extern VOXELGRAPHCORE_API FVoxelMaterialDefinitionManager* GVoxelMaterialDefinitionManager;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DECLARE_VOXEL_OBJECT_PIN_TYPE(FVoxelMaterialDefinitionRef);

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelMaterialDefinitionRefPinType : public FVoxelObjectPinType
{
	GENERATED_BODY()

	DEFINE_VOXEL_OBJECT_PIN_TYPE(FVoxelMaterialDefinitionRef, UVoxelMaterialDefinitionInterface)
	{
		if (bSetObject)
		{
			Object = GVoxelMaterialDefinitionManager->GetMaterial_AnyThread(Struct);
		}
		else
		{
			Struct = GVoxelMaterialDefinitionManager->Register_GameThread(*Object);
		}
	}
};