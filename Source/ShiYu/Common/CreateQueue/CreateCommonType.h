// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Common/GenerateType.h"

enum class ECreateState
{
	kStart,
	kCreating,
	kPause,
	kSuc,
	kFail,
	kCancel,
};

enum class ECreateQueueType
{
	kPawn,
};

class FCreateSpeed
{
public:

};