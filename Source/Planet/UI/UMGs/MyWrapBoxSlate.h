
#pragma once

#include "CoreMinimal.h"

#include "Widgets/Layout/SWrapBox.h"

class SMyWrapBox : public SWrapBox
{
public:

	virtual void OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const override;

	bool bIsPositiveSequence = true;

private:

	class FMyChildArranger;
	friend class SMyWrapBox::FMyChildArranger;

};
