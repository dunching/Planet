#include "CreateQueueMG.h"
#include <thread>

FCreateQueueMG::FCreateQueueMG()
{
	SPCreateSpeedPtr = MakeShared<FCreateSpeed>();
}

void FCreateQueueMG::UpdateImp()
{
	for (; ;)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		switch (CreateState)
		{
		case ECreateState::kCreating:
		{
			std::lock_guard<std::mutex>Lock(Mutex);
			for (int32 Index = CreateAry.Num() - 1; Index >= 0; Index--)
			{
				auto& RequestRef = CreateAry[Index];

				if (RequestRef->GetIsCancel())
				{
					RequestRef->UpdateCreateState(
						ECreateState::kCancel,
						0.f
					);
					CreateAry.RemoveAt(Index);
				}
			}

			if (CreateAry.Num() > 0)
			{
				auto& RequestRef = CreateAry[0];
				auto ItemCreateSpeed = 10;
				if (RequestRef->GetCurVal() == 0)
				{
					RequestRef->UpdateCreateState(ECreateState::kStart, ItemCreateSpeed);
				}
				else if (RequestRef->UpdateCreateState(CreateState, ItemCreateSpeed) >= 1)
				{
					RequestRef->UpdateCreateState(ECreateState::kSuc, ItemCreateSpeed);
					CreateAry.RemoveAt(0);
					if (CreateAry.Num() == 0)
					{
						return;
					}
				}
			}
		}
			break;
		case ECreateState::kPause:
			break;
		default:
			break;
		}
	}
}

TSharedPtr<FCreateQueueMG> FCreateQueueMG::CreateInst()
{
	return MakeShareable<FCreateQueueMG>(new FCreateQueueMG);
}

void FCreateQueueMG::SetSameTimeCreateNum(uint8 Num)
{
	CreateNum = Num;
}

TSharedPtr<FCreateRequest> FCreateQueueMG::Add(FItemType ItemType)
{
	std::lock_guard<std::mutex>Lock(Mutex);
	if (CreateAry.Num() == 0)
	{
		CreateState = ECreateState::kCreating;
		std::thread UpdateThread(&FCreateQueueMG::UpdateImp, this);
		UpdateThread.detach();
	}

	auto CreateRequest = MakeShared<FCreateRequest>(0, ItemType);
	CreateAry.Add(CreateRequest);

	return CreateRequest;
}

void FCreateQueueMG::Clear()
{
	std::lock_guard<std::mutex>Lock(Mutex);

	for (auto Iter : CreateAry)
	{
		Iter->Cancel();
	}
}

void FCreateQueueMG::Pause()
{
	CreateState = ECreateState::kPause;
}

void FCreateQueueMG::SetCreateSpeed(const TSharedPtr<FCreateSpeed>& NewSPCreateSpeedPtr)
{
	SPCreateSpeedPtr = NewSPCreateSpeedPtr;
}
