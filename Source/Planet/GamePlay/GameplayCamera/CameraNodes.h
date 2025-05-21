// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Core/CameraNode.h"
#include "Core/CameraParameters.h"

#include "CameraNodes.generated.h"

class UCameraValueInterpolator;
class UInput2DCameraNode;

/**
 * A camera node that can rotate the camera in yaw and pitch based on player input.
 */
UCLASS()
class PLANET_API UCameraNode_XYFH : public UCameraNode
{
	GENERATED_BODY()

protected:

	// UCameraNode interface.
	virtual FCameraNodeEvaluatorPtr OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const override;

public:

};

