// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UIInterfaces.h"
#include "Talent_FASI.h"

#include "TalentIcon.generated.h"

struct FStreamableHandle;

/**
 *
 */
UCLASS()
class PLANET_API UTalentIcon : public UMyUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * 插槽，增加/删除
	 * return 是否可以更改
	 */
	using FDelegateHandle = TDelegate<bool(
		UTalentIcon*,
		bool
		)>;

	void Reset();
	
	TSharedPtr<FCharacterProxy> CurrentProxyPtr = nullptr;

protected:
	virtual void NativeConstruct() override;

	virtual FReply NativeOnMouseButtonDown(
		const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent
		) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTalentIcon* NextSocletIcon = nullptr;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTag IconSocket;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 MaxNum = 3;

	FDelegateHandle OnValueChanged;

private:
	void UpdateNum();
};
