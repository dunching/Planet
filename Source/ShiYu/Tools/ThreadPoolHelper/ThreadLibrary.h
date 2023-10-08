// Zowee. All Rights Reserved.
#pragma once

#include <functional>

#include "CoreMinimal.h"

namespace ThreadLibrary
{
	void SHIYU_API AsyncExecuteInGameThread(const std::function<void()>& Func);

	void SHIYU_API SyncExecuteInGameThread(const std::function<void()>& Func);
}