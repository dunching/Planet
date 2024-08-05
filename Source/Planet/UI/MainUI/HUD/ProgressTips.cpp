
#include "ProgressTips.h"

#include "Components/ProgressBar.h"

const FName Progress = TEXT("Progress");

void UProgressTips::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (WaitTime < 0)
	{
		RemoveFromParent();
	}
	else
	{
		CurrentTime += InDeltaTime;
		if (CurrentTime >= WaitTime)
		{
			RemoveFromParent();
		}
		else
		{
			SetPercent(CurrentTime / WaitTime);
		}
	}
}

void UProgressTips::SetWaitTime(int32 val)
{
	WaitTime = val;
}

void UProgressTips::SetPercent(float Percent)
{
	{
		auto UIPtr = Cast<UProgressBar>(GetWidgetFromName(Progress));
		if (UIPtr)
		{
			UIPtr->SetPercent(Percent);
		}
	}
}
