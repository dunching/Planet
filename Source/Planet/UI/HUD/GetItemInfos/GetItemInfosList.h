// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"

#include "UIInterfaces.h"
#include "HUDInterface.h"

#include "GetItemInfosList.generated.h"

class UHUD_TeamMateInfo;

class UGetItemInfosItem;

struct FBasicProxy;
struct FCoinProxy;
struct FSkillProxy;
struct FConsumableProxy;

UCLASS()
class PLANET_API UGetItemInfosList :
	public UMyUserWidget,
	public IHUDInterface
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	virtual void NativeConstruct() override;
#else
	virtual void NativePreConstruct()override;
#endif

	virtual void ResetUIByData() override;

	void OnSkillProxyChanged(const TSharedPtr<FSkillProxy>& ProxyPtr, bool bIsAdd);

	void OnCoinProxyChanged(const TSharedPtr<FCoinProxy>& ProxyPtr, bool bIsAdd, int32 Num);

	void OnConsumableProxyChanged(const TSharedPtr<FConsumableProxy>& ProxyPtr, EProxyModifyType ProxyModifyType);

	void OnGourpmateProxyChanged(const TSharedPtr<FCharacterProxy>& ProxyPtr, bool bIsAdd);

protected:
	void OnRemovedItem();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UGetItemInfosItem> GetItemInfosClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	int32 MaxDisplayNum = 8;

	TArray<TSharedPtr<FBasicProxy>> OrderAry;

	TArray<TPair<TWeakPtr<FSkillProxy>, bool>> SkillPendingAry;

	TArray<TTuple<TWeakPtr<FCoinProxy>, bool, int32>> CoinPendingAry;

	TArray<TPair<TWeakPtr<FConsumableProxy>, EProxyModifyType>> ConsumablePendingAry;

	TArray<TPair<TWeakPtr<FCharacterProxy>, bool>> CharacterPendingAry;
};
