
#include "ThreadLibrary.h"

#include <mutex>
#include<latch>

#include "Async/Async.h"
#include "Kismet/KismetStringLibrary.h"

#include "LogHelper/LogWriter.h"

void ThreadLibrary::AsyncExecuteInGameThread(const std::function<void()>& Func)
{
	if (IsInGameThread())
	{
		Func();
	}
	else
	{
		AsyncTask(ENamedThreads::Type::GameThread, [=]()
			{
				Func();
			});
	}
}

void ThreadLibrary::SyncExecuteInGameThread(const std::function<void()>& Func)
{
	if (IsInGameThread())
	{
		Func();
	}
	else
	{
		std::latch latch{ 2 };
		AsyncTask(ENamedThreads::Type::GameThread, [&, Func]()
			{
				ON_SCOPE_EXIT
				{
		latch.arrive_and_wait();
				};
				Func();
			});
		latch.arrive_and_wait();
	}
}
