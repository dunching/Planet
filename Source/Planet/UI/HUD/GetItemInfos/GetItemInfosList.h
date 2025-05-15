// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "MyUserWidget.h"

#include "UIInterfaces.h"
#include "HUDInterface.h"
#include "LayoutInterfacetion.h"

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
	public ILayoutItemInterfacetion
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	virtual void NativeConstruct() override;
#else
	virtual void NativePreConstruct()override;
#endif

	void SetPlayerCharacter(ACharacterBase*PlayeyCharacterPtr);
	
	void OnWeaponProxyChanged(
		const TSharedPtr<FWeaponProxy>& ProxyPtr,
		EProxyModifyType ProxyModifyType
		);

	void OnSkillProxyChanged(
		const TSharedPtr<FSkillProxy>& ProxyPtr,
		EProxyModifyType ProxyModifyType
		);

	void OnCoinProxyChanged(
		const TSharedPtr<FCoinProxy>& ProxyPtr,
		EProxyModifyType ProxyModifyType,
		int32 Num
		);

	void OnConsumableProxyChanged(
		const TSharedPtr<FConsumableProxy>& ProxyPtr,
		EProxyModifyType ProxyModifyType
		);

	void OnGourpmateProxyChanged(
		const TSharedPtr<FCharacterProxy>& ProxyPtr,
		EProxyModifyType ProxyModifyType
		);

protected:
	void OnRemovedItem();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<USoundWave>GetCoinsSoundRef = nullptr;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	TSubclassOf<UGetItemInfosItem> GetItemInfosClass;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "UI ")
	int32 MaxDisplayNum = 8;

	TArray<TSharedPtr<FBasicProxy>> OrderAry;

	TArray<TPair<TWeakPtr<FWeaponProxy>, EProxyModifyType>> WeaponPendingAry;

	TArray<TPair<TWeakPtr<FSkillProxy>, EProxyModifyType>> SkillPendingAry;

	TArray<TTuple<TWeakPtr<FCoinProxy>, EProxyModifyType, int32>> CoinPendingAry;

	TArray<TPair<TWeakPtr<FConsumableProxy>, EProxyModifyType>> ConsumablePendingAry;

	TArray<TPair<TWeakPtr<FCharacterProxy>, EProxyModifyType>> CharacterPendingAry;
};
