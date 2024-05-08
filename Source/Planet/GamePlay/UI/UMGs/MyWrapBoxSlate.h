
#pragma once

#include "CoreMinimal.h"

#include "Widgets/Layout/SWrapBox.h"

class SMyWrapBox : public SWrapBox
{
public:

	virtual void OnArrangeChildren(const FGeometry& AllottedGeometry, FArrangedChildren& ArrangedChildren) const override;

private:

	class FChildArranger;
	friend class SMyWrapBox::FChildArranger;
};
