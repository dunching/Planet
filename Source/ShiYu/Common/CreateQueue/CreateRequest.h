// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>

#include "CoreMinimal.h"

#include "GenerateType.h"
#include "CreateCommonType.h"
#include <ItemType.h>

class SHIYU_API FCreateRequest
{
public:

	using FUpdateCreateStateCB = std::function<void(ECreateState, float)>;

	FCreateRequest(int32 NewNeedVal, FItemType NewItemType);

	void Cancel();

	void SetUpdateCreateStateCB(const FUpdateCreateStateCB& NewUpdateCreateStateCB);

	float UpdateCreateState(ECreateState CreateState, int32 AddVal);

	bool GetIsCancel()const { return bIsCancel; };

	int32 GetCurVal()const {return CurVal;};

	FItemType GetItemsType() const { return ItemType; };

private:

	FUpdateCreateStateCB UpdateCreateStateCB;

	bool bIsCancel = false;

	int32 NeedVal = 100;

	int32 CurVal = 0;

	FItemType ItemType;

};
