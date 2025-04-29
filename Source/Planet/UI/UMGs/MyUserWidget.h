
#pragma once

#include "CoreMinimal.h"

#include "Blueprint\UserWidget.h"

#include "MyUserWidget.generated.h"

struct FStreamableHandle;

class UTexture2D;
class UImage;

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

	void AsyncLoadText(const TSoftObjectPtr<UTexture2D>& Texture, UImage*ImagePtr);

private:
	
	TArray<TSharedPtr<FStreamableHandle>> AsyncLoadTextureHandleAry;

};
