// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

// 是否应该为 UObject ？
class  FTalent_Base
{
public:

	virtual ~FTalent_Base();

protected:

	int32 CurrentLevel = 1;

};
