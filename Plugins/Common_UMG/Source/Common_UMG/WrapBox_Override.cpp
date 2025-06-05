
#include "WrapBox_Override.h"

#include "Components/WrapBoxSlot.h"

#include "MyWrapBoxSlate.h"

TSharedRef<SWidget> UWrapBox_Override::RebuildWidget()
{
	PRAGMA_DISABLE_DEPRECATION_WARNINGS
		MyWrapBox = SNew(SMyWrapBox)
		.UseAllottedSize(!bExplicitWrapSize)
		.PreferredSize(WrapSize)
		.HAlign(HorizontalAlignment)
		.Orientation(Orientation);
	PRAGMA_ENABLE_DEPRECATION_WARNINGS

		for (UPanelSlot* PanelSlot : Slots)
		{
			if (UWrapBoxSlot* TypedSlot = Cast<UWrapBoxSlot>(PanelSlot))
			{
				TypedSlot->Parent = this;
				TypedSlot->BuildSlot(MyWrapBox.ToSharedRef());
			}
		}

	return MyWrapBox.ToSharedRef();
}

PRAGMA_DISABLE_DEPRECATION_WARNINGS
void UWrapBox_Override::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	if (!MyWrapBox.IsValid())
	{
		return;
	}

	TSharedPtr<class SMyWrapBox> MyWrapBoxSPtr(MyWrapBox, dynamic_cast<SMyWrapBox*>(MyWrapBox.Get()));

	MyWrapBoxSPtr->bIsPositiveSequence = bIsPositiveSequence;
}
PRAGMA_ENABLE_DEPRECATION_WARNINGS
