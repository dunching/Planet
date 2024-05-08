// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelSurface.h"
#include "VoxelExecNode.h"
#include "VoxelChunkSpawner.h"
#include "VoxelDetailTexture.h"
#include "VoxelPhysicalMaterial.h"
#include "Material/VoxelMaterial.h"
#include "Rendering/VoxelMeshSettings.h"
#include "Collision/VoxelCollider.h"
#include "Collision/VoxelCollisionComponent.h"
#include "VoxelMarchingCubeExecNode.generated.h"

struct FVoxelMesh;
class UVoxelMeshComponent;

USTRUCT()
struct VOXELGRAPHNODES_API FVoxelMarchingCubeExecNodeMesh
{
	GENERATED_BODY()

	TSharedPtr<const FVoxelMesh> Mesh;
	TSharedPtr<const FVoxelMeshSettings> MeshSettings;
	TSharedPtr<const FVoxelCollider> Collider;
	TSharedPtr<const FBodyInstance> BodyInstance;
};

// This node is entirely disabled on dedicated servers
USTRUCT(DisplayName = "Generate Marching Cube Surface")
struct VOXELGRAPHNODES_API FVoxelMarchingCubeExecNode : public FVoxelExecNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	// Defines where to spawn chunks & at what LOD
	// If not set will default to a Screen Size Chunk Spawner
	VOXEL_INPUT_PIN(FVoxelChunkSpawner, ChunkSpawner, nullptr, ConstantPin, OptionalPin);
	// The voxel size to use: distance in cm between 2 vertices when rendering
	VOXEL_INPUT_PIN(float, VoxelSize, 100.f, ConstantPin);

	VOXEL_INPUT_PIN(FVoxelSurface, Surface, nullptr, VirtualPin);
	VOXEL_INPUT_PIN(FVoxelMaterial, Material, nullptr, VirtualPin);

	// Collision presets to use on the rendered chunks
	// If set to NoCollision will not compute collision at all
	// If your game is multiplayer or has NPCs you need to set this to NoCollision and setup invoker-based collision instead
	// https://docs.voxelplugin.com/basics/navmesh-and-collision
	VOXEL_INPUT_PIN(FBodyInstance, BodyInstance, nullptr, VirtualPin);
	VOXEL_INPUT_PIN(FVoxelPhysicalMaterialBuffer, PhysicalMaterial, nullptr, VirtualPin);

	// Mesh settings, used to tune mesh component settings like CastShadow, ReceiveDecals...
	VOXEL_INPUT_PIN(FVoxelMeshSettings, MeshSettings, nullptr, VirtualPin, AdvancedDisplay);
	VOXEL_INPUT_PIN(FVoxelMaterialIdDetailTextureRef, MaterialIdDetailTexture, "/Voxel/Default/DefaultMaterialIdTexture.DefaultMaterialIdTexture", VirtualPin, AdvancedDisplay);
	VOXEL_INPUT_PIN(FVoxelNormalDetailTextureRef, NormalDetailTexture, "/Voxel/Default/DefaultNormalTexture.DefaultNormalTexture", VirtualPin, AdvancedDisplay);
	// The node will query the distance at the chunk corners (slightly offset inside) to determine if the chunk needs to be rendered at all
	// This value controls by how much to inflate the distance used by these distance checks
	// If you're getting holes in your voxel mesh, try increasing this a bit (typically, setting to 2 or 3)
	// Don't increase too much, will add a lot of additional cost!
	VOXEL_INPUT_PIN(float, DistanceChecksTolerance, 1.f, VirtualPin, AdvancedDisplay);
	// If true, will try to make transitions perfect by querying the right LOD for border values
	// More expensive! Only use if you see holes between LODs
	VOXEL_INPUT_PIN(bool, PerfectTransitions, false, VirtualPin, AdvancedDisplay);
	VOXEL_INPUT_PIN(bool, GenerateDistanceFields, false, VirtualPin, AdvancedDisplay);
	VOXEL_INPUT_PIN(float, DistanceFieldBias, 0.f, VirtualPin, AdvancedDisplay);
	// Priority offset, added to the task distance from camera
	// Closest tasks are computed first, so set this to a very low value (eg, -1000000) if you want it to be computed first
	VOXEL_INPUT_PIN(double, PriorityOffset, 0, ConstantPin, AdvancedDisplay);

	TValue<FVoxelMarchingCubeExecNodeMesh> CreateMesh(
		const FVoxelQuery& InQuery,
		float VoxelSize,
		int32 ChunkSize,
		const FVoxelBox& Bounds) const;
	virtual TVoxelUniquePtr<FVoxelExecNodeRuntime> CreateExecRuntime(const TSharedRef<const FVoxelExecNode>& SharedThis) const override;
};

class VOXELGRAPHNODES_API FVoxelMarchingCubeExecNodeRuntime : public TVoxelExecNodeRuntime<FVoxelMarchingCubeExecNode>
{
public:
	using Super::Super;

	//~ Begin FVoxelExecNodeRuntime Interface
	virtual void Create() override;
	virtual void Destroy() override;
	virtual void Tick(FVoxelRuntime& Runtime) override;
	//~ End FVoxelExecNodeRuntime Interface

private:
	const TSharedRef<FVoxelChunkActionQueue> ChunkActionQueue = MakeVoxelShared<FVoxelChunkActionQueue>();

	TSharedPtr<FVoxelChunkSpawner> ChunkSpawner;
	float VoxelSize = 0.f;

	struct FChunkInfo
	{
		const FVoxelChunkId ChunkId = FVoxelChunkId::New();
		const int32 LOD;
		const int32 ChunkSize;
		const FVoxelBox Bounds;

		FChunkInfo(
			const int32 LOD,
			const int32 ChunkSize,
			const FVoxelBox& Bounds)
			: LOD(LOD)
			, ChunkSize(ChunkSize)
			, Bounds(Bounds)
		{
		}
		~FChunkInfo()
		{
			ensure(!Mesh.IsValid());
			ensure(!MeshComponent.IsValid());
			ensure(!CollisionComponent.IsValid());
			ensure(OnCompleteArray.Num() == 0);
		}

		TVoxelDynamicValue<FVoxelMarchingCubeExecNodeMesh> Mesh;
		uint8 TransitionMask = 0;
		TWeakObjectPtr<UVoxelMeshComponent> MeshComponent;
		TWeakObjectPtr<UVoxelCollisionComponent> CollisionComponent;
		TVoxelArray<TSharedPtr<const TVoxelUniqueFunction<void()>>> OnCompleteArray;

		void FlushOnComplete();
	};

	FVoxelFastCriticalSection ChunkInfos_CriticalSection;
	TMap<FVoxelChunkId, TSharedPtr<FChunkInfo>> ChunkInfos;

	struct FQueuedMesh
	{
		FVoxelChunkId ChunkId;
		TSharedPtr<const FVoxelMarchingCubeExecNodeMesh> Mesh;
	};
	using FQueuedMeshes = TQueue<FQueuedMesh, EQueueMode::Mpsc>;
	const TSharedRef<FQueuedMeshes> QueuedMeshes = MakeVoxelShared<FQueuedMeshes>();

	FGraphEventRef ProcessActionsGraphEvent;

	void ProcessMeshes(FVoxelRuntime& Runtime);
	void ProcessActions(FVoxelRuntime* Runtime, bool bIsInGameThread);
	void ProcessAction(FVoxelRuntime* Runtime, const FVoxelChunkAction& Action);
};