
#pragma once

#include "CoreMinimal.h"

#include "Blueprint\UserWidget.h"

#include "MyUserWidget.generated.h"

struct FStreamableHandle;

/**
 *
 */
UCLASS()
class PLANET_API UMyUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeDestruct()override;

protected:

	TArray<TSharedPtr<FStreamableHandle>> AsyncLoadTextureHandleAry;

};
