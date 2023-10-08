// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelISPCNode.h"
#include "Buffer/VoxelFloatBuffers.h"
#include "VoxelNoiseNodes.generated.h"

USTRUCT(Category = "Math|Seed", meta = (CompactNodeTitle = "MIX"))
struct VOXELGRAPHNODES_API FVoxelNode_MixSeeds : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVoxelSeed, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(FVoxelSeed, B, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(FVoxelSeed, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = MurmurHash32(MurmurHash32({A}) ^ {B})";
	}
};

USTRUCT(Category = "Math|Seed", meta = (ShowInShortList))
struct VOXELGRAPHNODES_API FVoxelNode_MakeSeeds : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVoxelSeed, Seed, nullptr);

	FVoxelNode_MakeSeeds()
	{
		FixupSeedPins();
	}

	virtual FVoxelComputeValue CompileCompute(FName PinName) const override;

	virtual void PostSerialize() override
	{
		FixupSeedPins();

		Super::PostSerialize();

		FixupSeedPins();
	}

public:
	TArray<FVoxelPinRef> ResultPins;

	UPROPERTY()
	int32 NumNewSeeds = 1;

	void FixupSeedPins();

#if WITH_EDITOR
	class FDefinition : public Super::FDefinition
	{
	public:
		GENERATED_VOXEL_NODE_DEFINITION_BODY(FVoxelNode_MakeSeeds);

		virtual bool CanAddInputPin() const override
		{
			return true;
		}
		virtual void AddInputPin() override
		{
			Node.NumNewSeeds++;
			Node.FixupSeedPins();
		}

		virtual bool CanRemoveInputPin() const override
		{
			return Node.NumNewSeeds > 1;
		}
		virtual void RemoveInputPin() override
		{
			Node.NumNewSeeds--;
			Node.FixupSeedPins();
		}
	};
#endif
};

USTRUCT(Category = "Noise")
struct VOXELGRAPHNODES_API FVoxelNode_PerlinNoise2D : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVector2D, Position, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(FVoxelSeed, Seed, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(float, Value);

	virtual FString GenerateCode(FCode& Code) const override
	{
		Code.AddInclude("VoxelNoiseNodesImpl.isph");
		return "{Value} = GetPerlin2D({Seed}, {Position})";
	}
};

USTRUCT(Category = "Noise")
struct VOXELGRAPHNODES_API FVoxelNode_PerlinNoise3D : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVector, Position, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(FVoxelSeed, Seed, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(float, Value);

	virtual FString GenerateCode(FCode& Code) const override
	{
		Code.AddInclude("VoxelNoiseNodesImpl.isph");
		return "{Value} = GetPerlin3D({Seed}, {Position})";
	}
};

// Two dimensional cellular noise
USTRUCT(Category = "Noise")
struct VOXELGRAPHNODES_API FVoxelNode_CellularNoise2D : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	// Position at which to calculate output noise
	VOXEL_TEMPLATE_INPUT_PIN(FVector2D, Position, nullptr);
	// How irregular the cells are, needs to be between 0 and 1 to avoid glitches
	VOXEL_TEMPLATE_INPUT_PIN(float, Jitter, 0.9f);
	// Used to randomize the output noise
	VOXEL_TEMPLATE_INPUT_PIN(FVoxelSeed, Seed, nullptr);
	// Distance to nearest cell center at position
	VOXEL_TEMPLATE_OUTPUT_PIN(float, Value);

	virtual FString GenerateCode(FCode& Code) const override
	{
		Code.AddInclude("VoxelNoiseNodesImpl.isph");
		return "{Value} = GetCellularNoise2D({Seed}, {Position}, {Jitter})";
	}
};

// Three dimensional cellular noise
USTRUCT(Category = "Noise")
struct VOXELGRAPHNODES_API FVoxelNode_CellularNoise3D : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	// Position at which to calculate output noise
	VOXEL_TEMPLATE_INPUT_PIN(FVector, Position, nullptr);
	// How irregular the cells are, needs to be between 0 and 1 to avoid glitches
	VOXEL_TEMPLATE_INPUT_PIN(float, Jitter, 0.9f);
	// Used to randomize the output noise
	VOXEL_TEMPLATE_INPUT_PIN(FVoxelSeed, Seed, nullptr);
	// Distance to nearest cell center at position
	VOXEL_TEMPLATE_OUTPUT_PIN(float, Value);

	virtual FString GenerateCode(FCode& Code) const override
	{
		Code.AddInclude("VoxelNoiseNodesImpl.isph");
		return "{Value} = GetCellularNoise3D({Seed}, {Position}, {Jitter})";
	}
};

USTRUCT(Category = "Noise")
struct VOXELGRAPHNODES_API FVoxelNode_TrueDistanceCellularNoise2D : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVector2D, Position, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(float, Jitter, 0.9f);
	VOXEL_TEMPLATE_INPUT_PIN(FVoxelSeed, Seed, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(float, Value);
	VOXEL_TEMPLATE_OUTPUT_PIN(FVector2D, CellPosition);

	virtual FString GenerateCode(FCode& Code) const override
	{
		Code.AddInclude("VoxelNoiseNodesImpl.isph");
		return "{Value} = GetTrueDistanceCellularNoise2D({Seed}, {Position}, {Jitter}, &{CellPosition})";
	}
};