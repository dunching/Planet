// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#pragma once

#include <string>
#include <mutex>

#include "CoreMinimal.h"

#if UE_BUILD_SHIPPING && PRINTINVOKETIME
#define WRITEINVOKELINE()
#else
#define WRITEINVOKELINE() 
#endif

#if UE_BUILD_SHIPPING && PRINTINVOKETIME
#define PRINTFUNC
#else
#define PRINTFUNC()\
{\
UE_LOG(LogTemp, Log, TEXT("%s %s %d"), *FString(__FILE__), *FString(__FUNCDNAME__), __LINE__); \
}
#endif

#if UE_BUILD_SHIPPING && PRINTINVOKETIME
#define PRINTFUNCSTR
#else
#define PRINTFUNCSTR(Str)
#endif

#if UE_BUILD_SHIPPING && PRINTINVOKETIME
#define WRITESCOPERUNDELTATIME()
#else
#define WRITESCOPERUNDELTATIME()
#endif

#if UE_BUILD_SHIPPING && PRINTINVOKETIME
#define WRITEFUNCCALL()
#else
#define WRITEFUNCCALL()
#endif

#define PRINTINVOKEINFO() {\
UE_LOG(LogTemp, Log, TEXT("Func:%s Line:%d"), ANSI_TO_TCHAR(__FUNCTION__), __LINE__)\
}

#define PRINTINVOKEWITHSTR(Str) {\
UE_LOG(LogTemp, Log, TEXT("Func:%s Line:%d->%s"), ANSI_TO_TCHAR(__FUNCTION__), __LINE__, *Str)\
}
