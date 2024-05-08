// Zowee. All Rights Reserved.
#pragma once

#include <functional>

#include "CoreMinimal.h"

namespace ThreadLibrary
{
	void PLANET_API AsyncExecuteInGameThread(const std::function<void()>& Func);

	void PLANET_API SyncExecuteInGameThread(const std::function<void()>& Func);
}