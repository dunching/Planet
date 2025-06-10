// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UIInterfaces.h"
#include "Talent_FASI.h"

#include "TalentIcon.generated.h"

struct FStreamableHandle;

struct FCharacterProxy;

class UTextBlock;
class UBorder;

/**
 *
 */
UCLASS()
class PLANET_API UTalentIcon : public UUserWidget_Override
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
	
	void Update();

	void Reset();

	TSharedPtr<FCharacterProxy> CurrentProxyPtr = nullptr;

	/**
	 * 前置条件是否满足
	 */
	bool bPreviousIsOK = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSet<UTalentIcon*> NextSocletIconSet;

protected:
	virtual void SetIsEnabled(bool bInIsEnabled) override;
	
	virtual void NativeConstruct() override;

	virtual FReply NativeOnMouseButtonDown(
		const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent
		) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString DescriptionStr = TEXT("Value+{Value}");

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DescriptionText = nullptr;

	UPROPERTY(meta = (BindWidget))
	UBorder* DisEnable = nullptr;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTag TalentSocket;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 MaxNum = 3;

	FDelegateHandle OnValueChanged;

private:
	void UpdateNum();
};
