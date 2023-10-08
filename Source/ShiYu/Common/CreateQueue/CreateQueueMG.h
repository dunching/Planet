// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <mutex>

#include "CoreMinimal.h"

#include "GenerateType.h"
#include "CreateRequest.h"
#include "CreateCommonType.h"
#include <ItemType.h>

class SHIYU_API FCreateQueueMG
{
public:

	static TSharedPtr<FCreateQueueMG> CreateInst();

	void SetSameTimeCreateNum(uint8 Num);

	TSharedPtr<FCreateRequest> Add(FItemType ItemType);

	void Clear();

	void Pause();

	void SetCreateSpeed(const TSharedPtr<FCreateSpeed>& NewSPCreateSpeedPtr);

private:

	FCreateQueueMG();

	FCreateQueueMG& operator=(const FCreateQueueMG&) = delete;

	FCreateQueueMG& operator=(FCreateQueueMG&&) = delete;

	FCreateQueueMG(const FCreateQueueMG&) = delete;

	FCreateQueueMG(FCreateQueueMG&&) = delete;

	void UpdateImp();

	TSharedPtr<FCreateSpeed>SPCreateSpeedPtr;

	TArray<TSharedPtr<FCreateRequest>>CreateAry;

	ECreateState CreateState = ECreateState::kCancel;

	std::mutex Mutex;

	int32 CreateSpeed = 10;

	uint8 CreateNum = 1;

};
