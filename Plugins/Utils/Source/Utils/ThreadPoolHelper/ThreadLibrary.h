// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
#pragma once

#include <functional>

#include "CoreMinimal.h"

namespace ThreadLibrary
{
	void UTILS_API AsyncExecuteInGameThread(const std::function<void()>& Func);

	void UTILS_API SyncExecuteInGameThread(const std::function<void()>& Func);
}