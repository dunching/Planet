
#include "CreateRequest.h"

#include "Async/Async.h"

FCreateRequest::FCreateRequest(int32 NewNeedVal, FItemType NewItemType):
	NeedVal(NewNeedVal),
	ItemType(NewItemType)
{
}

void FCreateRequest::Cancel()
{
	bIsCancel = true;
}

void FCreateRequest::SetUpdateCreateStateCB(const FUpdateCreateStateCB& NewUpdateCreateStateCB)
{
	UpdateCreateStateCB = NewUpdateCreateStateCB;
}

float FCreateRequest::UpdateCreateState(ECreateState CreateState, int32 AddVal)
{
	float Progress = 0.f;

	if (UpdateCreateStateCB)
	{
		CurVal += AddVal;
		Progress = static_cast<float>(CurVal) / static_cast<float>(NeedVal);
		AsyncTask(ENamedThreads::GameThread, [&, CreateState, Progress]() {
			UpdateCreateStateCB(CreateState, Progress);
			});
	}

	return Progress;
}
