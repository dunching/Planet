// Copyright Ben Sutherland 2024. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#ifdef UE_INCLUDE_ORDER_5_3
#include "Runtime/Launch/Resources/Version.h"
#else
#include "Launch/Resources/Version.h"
#endif

DECLARE_LOG_CATEGORY_EXTERN(LogFlyingNavSystem, Log, All);

#define UE5 (ENGINE_MAJOR_VERSION == 5)
#define ATLEAST_UE5_1 (UE5 && ENGINE_MINOR_VERSION >= 1)
#define ATLEAST_UE5_2 (UE5 && ENGINE_MINOR_VERSION >= 2)
#define ATLEAST_UE5_3 (UE5 && ENGINE_MINOR_VERSION >= 3)
#define ATLEAST_UE5_4 (UE5 && ENGINE_MINOR_VERSION >= 4)

// PhysX/Chaos macros changed in UE5.1
#if ATLEAST_UE5_1
	#define WITH_PHYSX_FNS 1
	#define PHYSICS_INTERFACE_PHYSX_FNS 0
	#define WITH_CHAOS_FNS 1
#else
	#define WITH_PHYSX_FNS WITH_PHYSX
	#define PHYSICS_INTERFACE_PHYSX_FNS PHYSICS_INTERFACE_PHYSX
	#define WITH_CHAOS_FNS WITH_CHAOS
#endif

#if WITH_EDITOR
// Debug Utility for printing binary strings
static FString MakeBinaryString(int n) {
	FString S;
	while (n > 0) {
		S.Append(FString::FromInt(n % 2));
		n /= 2;
	}
	// Pad with 0
	while (S.Len() < 16)
	{
		S.Append(TEXT("0"));
	}
	return S.Reverse();
}
#endif

// Logging macros
#if !UE_BUILD_SHIPPING
#include "Engine/Engine.h"
#define print(text, ...) UE_LOG(LogFlyingNavSystem, Log, TEXT(text), ##__VA_ARGS__)
#define printw(text, ...) UE_LOG(LogFlyingNavSystem, Warning, TEXT(text), ##__VA_ARGS__)
#define printe(text, ...) UE_LOG(LogFlyingNavSystem, Error, TEXT(text), ##__VA_ARGS__)
#define printsc(key, text, ...) if (GEngine) GEngine->AddOnScreenDebugMessage(key, 6, FColor::Green, FString::Printf(TEXT(text), ##__VA_ARGS__));
#define printscw(key, text, ...) if (GEngine) GEngine->AddOnScreenDebugMessage(key, 6, FColor::Yellow, FString::Printf(TEXT(text), ##__VA_ARGS__));
#define printsce(key, text, ...) if (GEngine) GEngine->AddOnScreenDebugMessage(key, 6, FColor::Red, FString::Printf(TEXT(text), ##__VA_ARGS__).ToUpper(), true, FVector2D(3.f,3.f));
#else
#define print(text, ...)
#define printw(text, ...)
#define printe(text, ...)
#define printsc(key, text, ...)
#define printscw(key, text, ...)
#define printsce(key, text, ...)
#endif // !UE_BUILD_SHIPPING