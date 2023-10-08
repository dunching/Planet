// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelISPCNode.h"
#include "VoxelTemplateNode.h"
#include "Buffer/VoxelBaseBuffers.h"
#include "VoxelInterpolationNodes.generated.h"

UENUM(BlueprintType, DisplayName = "Voxel Interpolation Type")
enum class EVoxelInterpolationType : uint8
{
	Linear UMETA(ToolTip = "Simple linear interpolation"),
	SmoothStep UMETA(Tooltip = "Smooth Step interpolation"),
	Step UMETA(ToolTip = "Simple step interpolation"),
	SinusoidalIn UMETA(ToolTip = "Sinusoidal in interpolation"),
	SinusoidalOut UMETA(ToolTip = "Sinusoidal out interpolation"),
	SinusoidalInOut UMETA(ToolTip = "Sinusoidal in/out interpolation"),
	EaseIn UMETA(ToolTip = "Smoothly accelerates, but does not decelerate into the target. Ease amount controlled by BlendExp."),
	EaseOut UMETA(ToolTip = "Immediately accelerates, but smoothly decelerates into the target. Ease amount controlled by BlendExp."),
	EaseInOut UMETA(ToolTip = "Smoothly accelerates and decelerates. Ease amount controlled by BlendExp."),
	ExpoIn UMETA(ToolTip = "Easing in using an exponential"),
	ExpoOut UMETA(ToolTip = "Easing out using an exponential"),
	ExpoInOut UMETA(ToolTip = "Easing in/out using an exponential method"),
	CircularIn UMETA(ToolTip = "Easing is based on a half circle"),
	CircularOut UMETA(ToolTip = "Easing is based on an inverted half circle"),
	CircularInOut UMETA(ToolTip = "Easing is based on two half circles"),
};

USTRUCT(meta = (Internal))
struct VOXELGRAPHNODES_API FVoxelNode_Interpolate : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(EVoxelInterpolationType, InterpolationType, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(float, A, 0.f);
	VOXEL_TEMPLATE_INPUT_PIN(float, B, 1.f);
	VOXEL_TEMPLATE_INPUT_PIN(float, Alpha, 0.f);
	VOXEL_TEMPLATE_INPUT_PIN(float, Exp, 2.f);
	VOXEL_TEMPLATE_INPUT_PIN(int32, Steps, 2);
	VOXEL_TEMPLATE_OUTPUT_PIN(float, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		Code.AddInclude("VoxelInterpolationNodesImpl.isph");

		FString Result;
		Result += "switch ({InterpolationType})";
		Result += "{";
		Result += "default: check(false); {ReturnValue} = 0.f; break;";
		Result += "case EVoxelInterpolationType_Linear: {ReturnValue} = lerp({A}, {B}, {Alpha}); break;";
		Result += "case EVoxelInterpolationType_SmoothStep: {ReturnValue} = SmoothStep({A}, {B}, {Alpha}); break;";
		Result += "case EVoxelInterpolationType_Step: {ReturnValue} = InterpStep({A}, {B}, {Alpha}, {Steps}); break;";
		Result += "case EVoxelInterpolationType_SinusoidalIn: {ReturnValue} = InterpSinIn({A}, {B}, {Alpha}); break;";
		Result += "case EVoxelInterpolationType_SinusoidalOut: {ReturnValue} = InterpSinOut({A}, {B}, {Alpha}); break;";
		Result += "case EVoxelInterpolationType_SinusoidalInOut: {ReturnValue} = InterpSinInOut({A}, {B}, {Alpha}); break;";
		Result += "case EVoxelInterpolationType_EaseIn: {ReturnValue} = InterpEaseIn({A}, {B}, {Alpha}, {Exp}); break;";
		Result += "case EVoxelInterpolationType_EaseOut: {ReturnValue} = InterpEaseOut({A}, {B}, {Alpha}, {Exp}); break;";
		Result += "case EVoxelInterpolationType_EaseInOut: {ReturnValue} = InterpEaseInOut({A}, {B}, {Alpha}, {Exp}); break;";
		Result += "case EVoxelInterpolationType_ExpoIn: {ReturnValue} = InterpExpoIn({A}, {B}, {Alpha}); break;";
		Result += "case EVoxelInterpolationType_ExpoOut: {ReturnValue} = InterpExpoOut({A}, {B}, {Alpha}); break;";
		Result += "case EVoxelInterpolationType_ExpoInOut: {ReturnValue} = InterpExpoInOut({A}, {B}, {Alpha}); break;";
		Result += "case EVoxelInterpolationType_CircularIn: {ReturnValue} = InterpCircularIn({A}, {B}, {Alpha}); break;";
		Result += "case EVoxelInterpolationType_CircularOut: {ReturnValue} = InterpCircularOut({A}, {B}, {Alpha}); break;";
		Result += "case EVoxelInterpolationType_CircularInOut: {ReturnValue} = InterpCircularInOut({A}, {B}, {Alpha}); break;";
		Result += "}";
		return Result;
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(Category = "Math|Misc", meta = (Keywords = "lerp linear smoothstep step sin ease expo circular"))
struct VOXELGRAPHNODES_API FVoxelTemplateNode_Interpolate : public FVoxelTemplateNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(EVoxelInterpolationType, InterpolationType, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(FVoxelWildcard, A, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(FVoxelWildcard, B, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(FVoxelWildcard, Alpha, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(float, Exp, 2.f);
	VOXEL_TEMPLATE_INPUT_PIN(int32, Steps, 2);
	VOXEL_TEMPLATE_OUTPUT_PIN(FVoxelWildcard, Result);

public:
	virtual FPin* ExpandPins(FNode& Node, TArray<FPin*> Pins, const TArray<FPin*>& AllPins) const override;

#if WITH_EDITOR
	virtual FVoxelPinTypeSet GetPromotionTypes(const FVoxelPin& Pin) const override;
	virtual void PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType) override;
#endif

	virtual UScriptStruct* GetInnerNode() const VOXEL_PURE_VIRTUAL({});

public:
#if WITH_EDITOR
	class FDefinition : public Super::FDefinition
	{
	public:
		GENERATED_VOXEL_NODE_DEFINITION_BODY(FVoxelTemplateNode_Interpolate);

		virtual bool IsPinVisible(const UEdGraphPin* Pin, const UEdGraphNode* Node) override;
	};
#endif
};