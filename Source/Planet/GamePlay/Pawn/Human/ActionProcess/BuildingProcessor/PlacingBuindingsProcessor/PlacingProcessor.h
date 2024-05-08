// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include <mutex>
#include <atomic>

#include "CoreMinimal.h"

#include "HumanProcessor.h"
#include <SceneElement.h>

class AHumanCharacter;
class ABuildingBase;
class ABuildingCaptureData;
class UBuildInteractionWidgetBase;
class UPlaceWidgetBase;

struct FSceneTool;

namespace HumanProcessor
{
	struct FCaptureInfo
	{
		// 附著在捕捉點上時
		TSharedPtr<FCapturesInfo::FCaptureInfo>CaptureInfoSPtr;

		// 附著在地面時
		FHitResult HitResult;
	};

	class FPlacingProcessor : public FHumanProcessor
	{
	private:

	public:

		GENERATIONCLASSINFO(FPlacingProcessor, FHumanProcessor);

		FPlacingProcessor(FOwnerPawnType* CharacterPtr);

		virtual void EnterAction()override;

		virtual void QuitAction()override;

		virtual void BeginDestroy();

	public:

		virtual void AddPitchInput(const FInputActionValue& InputActionValue)override;

		virtual void AddYawInput(const FInputActionValue& InputActionValue)override;

		virtual void MoveForward(const FInputActionValue& InputActionValue)override;

		virtual void MoveRight(const FInputActionValue& InputActionValue)override;

		virtual void ESCKeyPressed()override;

		virtual void QKeyPressed()override;

		virtual void GKeyPressed()override;

		virtual void PressedNumKey(int32 NumKey)override;

		virtual void MouseLeftPressed()override;

		virtual void SetHoldItemsData(const TSharedPtr<FSceneTool>& HoldItemsSPtr);

		virtual void SetPlaceBuildItem(const FSceneTool& Item);

	protected:

		virtual void OnPlaceItemPrev();

		virtual void AsyncFindCaptureComponent(const TArray<FHitResult>& OutHitsAry);

		virtual void AttachActor(const FCaptureInfo& CaptureInfo);

		void AsyncFindCaptureLoop();

		void SetBuildSharedDataCollision(bool bIsEnable);

		FCaptureInfo FindCaptureImp(const TArray<FHitResult>& CurHitsAru);

		std::mutex HitsAryMutex;

		std::condition_variable HitsAryCV;

		std::mutex CopyAryMutex;

		TArray<FHitResult> HitsAry;

		std::atomic<bool>bIsRequestQuit = false;

		FTimerHandle TimerHandle;

		ABuildingBase* BuildTargetPtr = nullptr;

		uint8 ExplicitCaptureIndex = 0;

		bool bCanPlace = false;

		TSharedPtr<FSceneTool> HoldItemsSPtr;

	};

}