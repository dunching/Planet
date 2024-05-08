 
 #include "MyWrapBox.h"
 
 #include "Components/WrapBoxSlot.h"
 
 #include "MyWrapBoxSlate.h"
 
 TSharedRef<SWidget> UMyWrapBox::RebuildWidget()
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
