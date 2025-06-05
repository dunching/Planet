#include "STT_RotateToFaceEntry.h"

#include <NavigationSystem.h>
#include <EnvironmentQuery/EnvQueryManager.h>

#include "HumanAIController.h"
#include "HumanCharacter.h"
#include "AITask_ReleaseSkill.h"
#include "STE_Assistance.h"
#include "PlanetModule.h"
#include "PlanetPlayerController.h"
#include "STE_CharacterBase.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetMathLibrary.h"

namespace STT_RotateToFaceEntry
{
	FName Donut_InnerRadius = TEXT("Donut.InnerRadius");

	FName Donut_OuterRadius = TEXT("Donut.OuterRadius");
}

const UStruct* FSTT_RotateToFaceEntry::GetInstanceDataType() const
{
	return FInstanceDataType::StaticStruct();
}

EStateTreeRunStatus FSTT_RotateToFaceEntry::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (InstanceData.GloabVariable->TargetCharacterPtr.IsValid())
	{
		const float AngleTolerance = FMath::DegreesToRadians(5.0f);

		const auto PawnRot = InstanceData.CharacterPtr->GetActorRotation();

		const auto TargetRot = UKismetMathLibrary::MakeRotFromZX(
			FVector::UpVector,
			InstanceData.GloabVariable->TargetCharacterPtr->GetActorLocation() - InstanceData.CharacterPtr->
			GetActorLocation()
		);

		// Gravity?
		if (PawnRot.EqualsOrientation(TargetRot, AngleTolerance))
		{
			return EStateTreeRunStatus::Succeeded;
		}
	}
	else
	{
		return EStateTreeRunStatus::Failed;
	}

	return Super::Tick(Context, DeltaTime);
}

EStateTreeRunStatus FSTT_IsFaceToTarget::Tick(
	FStateTreeExecutionContext& Context,
	const float DeltaTime
) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData(*this);

	if (InstanceData.GloabVariable->TargetCharacterPtr.IsValid())
	{
		const float AngleTolerance = FMath::DegreesToRadians(5.0f);

		const auto PawnRot = InstanceData.CharacterPtr->GetActorRotation();

		const auto TargetRot = UKismetMathLibrary::MakeRotFromZX(
			FVector::UpVector,
			InstanceData.GloabVariable->TargetCharacterPtr->GetActorLocation() - InstanceData.CharacterPtr->
			GetActorLocation()
		);

		// Gravity?
		if (PawnRot.EqualsOrientation(TargetRot, AngleTolerance))
		{
			if (InstanceData.GloabVariable)
			{
				return EStateTreeRunStatus::Running;
			}
			else
			{
				return EStateTreeRunStatus::Succeeded;
			}
		}
	}
	else
	{
	}

	return EStateTreeRunStatus::Failed;
}
