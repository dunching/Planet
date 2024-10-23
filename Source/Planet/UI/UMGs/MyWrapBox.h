
#pragma once

#include "CoreMinimal.h"

#include "Components/WrapBox.h"

#include "MyWrapBox.generated.h"

UCLASS(MinimalAPI)
class UMyWrapBox : public UWrapBox
{
	GENERATED_BODY()

public:
	
	bool bIsPositiveSequence = true;

	virtual TSharedRef<SWidget> RebuildWidget() override;

	virtual void SynchronizeProperties()override;

protected:

};
