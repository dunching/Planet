// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#define GENERATIONCLASSINFO(ThisClassType, ParentClassType) \
using ThisClass = ThisClassType; \
using Super = ParentClassType;

#define GENERATIONCLASSINFOONLYTHIS(ThisClassType) \
using ThisClass = ThisClassType; 

extern UTILS_API bool GIsExiting;

UTILS_API UWorld* GetWorldImp();
