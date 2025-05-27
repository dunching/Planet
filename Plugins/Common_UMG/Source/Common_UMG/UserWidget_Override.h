
#pragma once

#include "CoreMinimal.h"

#include "Blueprint\UserWidget.h"

#include "UserWidget_Override.generated.h"

struct FStreamableHandle;

class UTexture2D;
class UImage;

/**
 *
 */
UCLASS()
class COMMON_UMG_API UUserWidget_Override : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeDestruct()override;

protected:

	void AsyncLoadText(const TSoftObjectPtr<UTexture2D>& Texture, UImage*ImagePtr);

private:
	
	TArray<TSharedPtr<FStreamableHandle>> AsyncLoadTextureHandleAry;

};
