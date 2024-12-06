// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include <mutex>
#include <atomic>

#include "CoreMinimal.h"

#include "ItemProxy_Minimal.h"
#include "BuildingBaseProcessor.h"

namespace HumanProcessor
{
	class FUseAxeProcessor :  public FBuildingBaseProcessor
	{
	private:

	public:

		GENERATIONCLASSINFO(FUseAxeProcessor, FBuildingBaseProcessor);

		FUseAxeProcessor(FOwnerPawnType* CharacterPtr);

		virtual void EnterAction()override;

		virtual void QuitAction()override;

		virtual void BeginDestroy();

	public:

	};

}