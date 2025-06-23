// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UserWidget_Override.h"

#include "CharacterAttributesComponent.h"
#include "HUDInterface.h"
#include "LayoutInterfacetion.h"

#include "UpgradeBoder.generated.h"

class UProgressBar;
class UButton;
class UTextBlock;
class UScrollBox;
class UMaterialIcon;

struct FPassiveSkillProxy;
struct FMaterialProxy;

UCLASS()
class PLANET_API UUpgradeBoder :
	public UUserWidget_Override
{
	GENERATED_BODY()

public:
	using FOnProxyChangedDelegateHandle = TCallbackHandleContainer<void(
		const TSharedPtr<
			FMaterialProxy>&,
		// 直接指定类型
		EProxyModifyType,
		int32
		)
	>::FCallbackHandleSPtr;

	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	void BindData(
		const TSharedPtr<FPassiveSkillProxy>& ProxySPtr
		);

	/**
	 * 更新添加的材料
	 * @param ProxySPtr 
	 * @param Num 
	 * @param bIsAdd 
	 * @return 
	 */
	int32 OnUpdateMaterial(
		const TSharedPtr<FMaterialProxy>& ProxySPtr,
		int32 Num,
		bool bIsAdd
		);

protected:
	UFUNCTION()
	void OnClickedUpgradeBtn();

	UFUNCTION()
	void OnClickedCancelBtn();

	void OnProxyChanged(
		const TSharedPtr<
			FMaterialProxy>& ProxySPtr,
		// 直接指定类型
		EProxyModifyType ProxyModifyType,
		int32 Num
		);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<UMaterialIcon> MaterialIconClass = nullptr;

	/**
	 * TODO 
	 * 0. 选材料界面
	 * 1. 升级中，等待Server执行成功回调
	 * 2. 完成界面，显示新增的词条
	 */
	// UPROPERTY(meta = (BindWidget))
	// UWidgetSwitcher* WidgetSwitcher = nullptr;

	UPROPERTY(meta = (BindWidget))
	UButton* UpgradeBtn = nullptr;

	UPROPERTY(meta = (BindWidget))
	UButton* CancelBtn = nullptr;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* ProgressBar = nullptr;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* OffsetProgressBar = nullptr;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentLevelText = nullptr;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* OffsetLevelText = nullptr;

	UPROPERTY(meta = (BindWidget))
	UScrollBox* ScrollBox = nullptr;

	/**
	 * 要升级的对象
	 */
	TSharedPtr<FPassiveSkillProxy> ProxySPtr = nullptr;

	/**
	 * 消耗的材料
	 * 数量
	 */
	TMap<TSharedPtr<FMaterialProxy>, uint32> CosumeProxysSet;

	int32 OffsetLevel = 0;

	FOnProxyChangedDelegateHandle OnProxyChangedDelegateHandle;
};
