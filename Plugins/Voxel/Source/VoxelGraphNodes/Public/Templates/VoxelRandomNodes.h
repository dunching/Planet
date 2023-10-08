// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelISPCNode.h"
#include "VoxelTemplateNode.h"
#include "VoxelFunctionLibrary.h"
#include "Buffer/VoxelBaseBuffers.h"
#include "VoxelRandomNodes.generated.h"

UENUM(BlueprintType)
enum class EVoxelRandomVectorType : uint8
{
	// Vector will have uniform X, Y and Z values
	Uniform,
	// Vector will have random X, Y and Z values
	Free,
	// X and Y will be the same random value, Z will be another
	LockXY
};

USTRUCT(BlueprintType)
struct FVoxelRandomVectorSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Config")
	EVoxelRandomVectorType Type = EVoxelRandomVectorType::Uniform;

	// Specifies the range of X value, from minimum to maximum
	UPROPERTY(EditAnywhere, Category = "Config")
	FVoxelFloatRange RangeX = { 1.0f, 1.0f };

	// Specifies the range of Y value, from minimum to maximum
	UPROPERTY(EditAnywhere, Category = "Config")
	FVoxelFloatRange RangeY = { 1.0f, 1.0f };

	// Specifies the range of Z value, from minimum to maximum
	UPROPERTY(EditAnywhere, Category = "Config")
	FVoxelFloatRange RangeZ = { 1.0f, 1.0f };
};

///////////////////////////////////////////////////////////////////////////////

UCLASS()
class UVoxelRandomNodesFunctionLibrary : public UVoxelFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(Category = "Random", meta = (NativeMakeFunc))
	FVoxelRandomVectorSettings MakeRandomVectorSettings(
		const EVoxelRandomVectorType Type,
		const FVoxelFloatRange& RangeX,
		const FVoxelFloatRange& RangeY,
		const FVoxelFloatRange& RangeZ) const;

	UFUNCTION(Category = "Random", meta = (NativeBreakFunc))
	EVoxelRandomVectorType BreakRandomVectorSettings(
		const FVoxelRandomVectorSettings& Value,
		FVoxelFloatRange& RangeX,
		FVoxelFloatRange& RangeY,
		FVoxelFloatRange& RangeZ) const;
};

///////////////////////////////////////////////////////////////////////////////

// Generates a random float between Min and Max
USTRUCT(Category = "Random")
struct FVoxelNode_RandFloat : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVoxelSeed, Seed, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(FVoxelFloatRange, Range, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(float, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = RandRange({Seed}, {Range})";
	}
};

///////////////////////////////////////////////////////////////////////////////

USTRUCT(meta = (Internal))
struct FVoxelNode_RandomVector : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelFloatRange, RangeX, nullptr);
	VOXEL_INPUT_PIN(FVoxelFloatRange, RangeY, nullptr);
	VOXEL_INPUT_PIN(FVoxelFloatRange, RangeZ, nullptr);
	VOXEL_INPUT_PIN(EVoxelRandomVectorType, Type, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(FVoxelSeed, Seed, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(FVector, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		FString Result;
		Result += "switch ({Type})";
		Result += "{";
		Result += "default:";
		Result += "{";
		Result += "	check(false);";
		Result += "	{ReturnValue}.x = 0.f;";
		Result += "	{ReturnValue}.y = 0.f;";
		Result += "	{ReturnValue}.z = 0.f;";
		Result += "	break;";
		Result += "}";
		Result += "case EVoxelRandomVectorType_Uniform:";
		Result += "{";
		Result += "	{ReturnValue}.x = RandRange({Seed}, {RangeX});";
		Result += "	{ReturnValue}.y = {ReturnValue}.x;";
		Result += "	{ReturnValue}.z = {ReturnValue}.x;";
		Result += "	break;";
		Result += "}";
		Result += "case EVoxelRandomVectorType_Free:";
		Result += "{";
		Result += "	{ReturnValue}.x = RandRange(MurmurHash32({Seed}, 0), {RangeX});";
		Result += "	{ReturnValue}.y = RandRange(MurmurHash32({Seed}, 1), {RangeY});";
		Result += "	{ReturnValue}.z = RandRange(MurmurHash32({Seed}, 2), {RangeZ});";
		Result += "	break;";
		Result += "}";
		Result += "case EVoxelRandomVectorType_LockXY:";
		Result += "{";
		Result += "	{ReturnValue}.x = RandRange(MurmurHash32({Seed}, 0), {RangeX});";
		Result += "	{ReturnValue}.y = {ReturnValue}.x;";
		Result += "	{ReturnValue}.z = RandRange(MurmurHash32({Seed}, 1), {RangeZ});";
		Result += "	break;";
		Result += "}";
		Result += "}";
		return Result;
	}
};

///////////////////////////////////////////////////////////////////////////////

USTRUCT(Category = "Random")
struct FVoxelTemplateNode_RandomVector : public FVoxelTemplateNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelRandomVectorSettings, Settings, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(FVoxelSeed, Seed, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(FVector, ReturnValue);

public:
	virtual FPin* ExpandPins(FNode& Node, TArray<FPin*> Pins, const TArray<FPin*>& AllPins) const override;
};

///////////////////////////////////////////////////////////////////////////////

USTRUCT(Category = "Random")
struct FVoxelNode_RandomRotator : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelFloatRange, Roll, nullptr);
	VOXEL_INPUT_PIN(FVoxelFloatRange, Pitch, nullptr);
	VOXEL_INPUT_PIN(FVoxelFloatRange, Yaw, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(FVoxelSeed, Seed, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(FQuat, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = MakeQuaternionFromEuler(RandRange(MurmurHash32({Seed}, 0), {Pitch}), RandRange(MurmurHash32({Seed}, 1), {Yaw}), RandRange(MurmurHash32({Seed}, 2), {Roll}));";
	}
};