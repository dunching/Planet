// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelFunctionLibrary.h"
#include "Buffer/VoxelFloatBuffers.h"
#include "VoxelBasicFunctionLibrary.generated.h"

UCLASS()
class VOXELGRAPHCORE_API UVoxelBasicFunctionLibrary : public UVoxelFunctionLibrary
{
	GENERATED_BODY()

public:
	// Create unique seeds from float positions
	UFUNCTION(Category = "Random")
	FVoxelSeedBuffer HashPosition(
		const FVoxelVectorBuffer& Position,
		const FVoxelSeed& Seed,
		int32 RoundingDecimals = 3) const;

	UFUNCTION(Category = "Random")
	FVoxelVectorBuffer RandUnitVector(const FVoxelSeedBuffer& Seed) const;

	UFUNCTION(Category = "Misc")
	int32 GetLOD() const;

	UFUNCTION(Category = "Misc")
	FVoxelBox GetPointChunkBounds() const;

	UFUNCTION(Category = "Misc")
	ECollisionEnabled::Type GetCollisionEnabled(const FBodyInstance& BodyInstance) const;

public:
	UFUNCTION(Category = "Misc")
	bool IsPreviewScene() const;

	// False for editor preview, true for PIE, standalone...
	UFUNCTION(Category = "Misc")
	bool IsGameWorld() const;

	// Opposite of IsGameWorld: true for editor preview, false for PIE, packaged, standalone...
	UFUNCTION(Category = "Misc")
	bool IsEditorWorld() const;

	UFUNCTION(Category = "Misc")
	bool IsHiddenInEditor() const;

public:
	// Standalone: a game without networking, with one or more local players. Still considered a server because it has all server functionality
	UFUNCTION(Category = "Network")
	bool IsStandalone() const;

	// Dedicated server: server with no local players
	UFUNCTION(Category = "Network")
	bool IsDedicatedServer() const;

	// Listen server: a server that also has a local player who is hosting the game, available to other players on the network
	UFUNCTION(Category = "Network")
	bool IsListenServer() const;

	// Network client: client connected to a remote server.
	UFUNCTION(Category = "Network")
	bool IsClient() const;

	// Check if this is a standalone game, a dedicated server or a listen server
	UFUNCTION(Category = "Network")
	bool IsServer() const;

public:
	// Returns the transform of the current actor/brush
	UFUNCTION(Category = "Actor")
	FTransform GetTransform() const;

public:
	UFUNCTION(meta = (Internal))
	FVoxelRuntimePinValue ToBuffer(const FVoxelRuntimePinValue& Value) const;
};