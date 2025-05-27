
#pragma once

#include "CoreMinimal.h"

#include "Components/WrapBox.h"

#include "WrapBox_Override.generated.h"

UCLASS()
class COMMON_UMG_API UWrapBox_Override : public UWrapBox
{
	GENERATED_BODY()

public:
	
	bool bIsPositiveSequence = true;

	virtual TSharedRef<SWidget> RebuildWidget() override;

	virtual void SynchronizeProperties()override;

protected:

};