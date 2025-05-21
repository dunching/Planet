// Copyright Epic Games, Inc. All Rights Reserved.

#include "CameraNodes.h"

#include "Core/CameraEvaluationContext.h"
#include "Core/CameraOperation.h"
#include "Kismet/GameplayStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CameraNodes)

namespace UE::Cameras
{
	class FCameraNode_XYFH_Evaluator : public FCameraNodeEvaluator
	{
		UE_DECLARE_CAMERA_NODE_EVALUATOR(PLANET_API, FCameraNode_XYFH_Evaluator)

	protected:

		virtual void OnRun(const FCameraNodeEvaluationParams& Params, FCameraNodeEvaluationResult& OutResult) override;
		
	private:

		APlayerController* GetPlayerController(TSharedPtr<const FCameraEvaluationContext> EvaluationContext) const;

	};

	void FCameraNode_XYFH_Evaluator::OnRun(
		const FCameraNodeEvaluationParams& Params,
		FCameraNodeEvaluationResult& OutResult
		)
	{
		APlayerController* PlayerController = GetPlayerController(Params.EvaluationContext);
		if (PlayerController)
		{
			FVector out_Location;
			FRotator out_Rotation;
			PlayerController->GetActorEyesViewPoint(out_Location, out_Rotation);

			FTransform Transform;

			Transform.SetLocation(out_Location);
			Transform.SetRotation(out_Rotation.Quaternion());
			
			OutResult.CameraPose.SetTransform(Transform);
		}
	}

	APlayerController* FCameraNode_XYFH_Evaluator::GetPlayerController(
		TSharedPtr<const FCameraEvaluationContext> EvaluationContext
		) const
	{
		if (EvaluationContext)
		{
			return EvaluationContext->GetPlayerController();
		}
		return nullptr;
	}

	UE_DEFINE_CAMERA_NODE_EVALUATOR(FCameraNode_XYFH_Evaluator)
}

FCameraNodeEvaluatorPtr UCameraNode_XYFH::OnBuildEvaluator(FCameraNodeEvaluatorBuilder& Builder) const
{
	using namespace UE::Cameras;
	return Builder.BuildEvaluator<FCameraNode_XYFH_Evaluator>();
}

