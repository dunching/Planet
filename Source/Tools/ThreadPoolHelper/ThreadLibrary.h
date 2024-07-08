// Zowee. All Rights Reserved.
#pragma once

#include <functional>

#include "CoreMinimal.h"

namespace ThreadLibrary
{
	void TOOLS_API AsyncExecuteInGameThread(const std::function<void()>& Func);

	void TOOLS_API SyncExecuteInGameThread(const std::function<void()>& Func);
}