// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <bitset>

#include "CoreMinimal.h"

#include "UIInterfaces.h"

#include "GenerateTypes.h"
#include "ItemProxy_Minimal.h"
#include <MenuInterface.h>

#include "BackpackMenu.generated.h"

class UBackpackConsumableIcon;
class UBackpackToolIcon;
class UAllocationSkillsMenu;
class UButton;

struct FSceneObjContainer;
struct FSceneProxyContainer;
struct FSkillProxy;
struct FCharacterProxy;
struct FWeaponProxy;

enum EBackpackViewFilter : uint8
{
	kWeapon,
	kSkill,
	kConsumable,
	kMaterial,
	kAll,
};

/**
 *
 */
UCLASS()
class PLANET_API UBackpackMenu :
	public UUserWidget_Override,
	public IMenuInterface
{
	GENERATED_BODY()

public:
	using FOnDragIconDelegate = TCallbackHandleContainer<void(
		bool,
		const TSharedPtr<FBasicProxy>&


		
		)>;

	using FOnSelectedProxy = TCallbackHandleContainer<void(
		const TSharedPtr<FBasicProxy>&


		
		)>;

	virtual void NativeConstruct() override;

	virtual void EnableMenu() override;

	virtual void DisEnableMenu() override;

	virtual EMenuType GetMenuType() const override final;

	bool bIsPlayerMenu = true;

	TSharedPtr<FCharacterProxy> CurrentProxyPtr = nullptr;

	UAllocationSkillsMenu* AllocationSkillsMenuPtr = nullptr;

protected:
	void BindEvent();

	UFUNCTION()
	void OnWeaponBtnCliked();

	UFUNCTION()
	void OnSkillBtnCliked();

	UFUNCTION()
	void OnConsumableBtnCliked();

	UFUNCTION()
	void OnMaterialBtnCliked();

	UFUNCTION()
	void OnShowAllBtnCliked();

private:
	UPROPERTY(meta = (BindWidget))
	UButton* MaterialBtn = nullptr;

	TArray<TSharedPtr<FBasicProxy>> GetProxys() const;

	void ResetUIByData_Skill();

	void AddSkillProxy(
		const TSharedPtr<FSkillProxy>& ProxySPtr
		);

	void ResetUIByData_Weapon();

	void AddWeaponProxy(
		const TSharedPtr<FWeaponProxy>& ProxySPtr
		);

	void ResetUIByData_Consumable();

	void AddConsumableProxy(
		const TSharedPtr<FConsumableProxy>& ProxySPtr
		);

	void ResetUIByData_Material();

	void AddMaterialProxy(
		const TSharedPtr<FMaterialProxy>& ProxySPtr,
		EProxyModifyType ProxyModifyType
		);

	void ResetUIByData_All();

	void AddProxy(
		const TSharedPtr<FBasicProxy>& ProxySPtr
		);

	void UpdateProxy(
		const TSharedPtr<FBasicProxy>& ProxySPtr
		);

	void RemoveProxy(
		const TSharedPtr<FBasicProxy>& ProxySPtr
		);

	std::bitset<8> BackpackViewFilterFlags = 0;

	using FOnWeaponProxyChangedDelagateHandle = TCallbackHandleContainer<void(
		const TSharedPtr<
			FWeaponProxy>&,
		EProxyModifyType
		)
	>::FCallbackHandleSPtr;

	FOnWeaponProxyChangedDelagateHandle OnWeaponProxyChangedDelagateHandle;

	using FOnMaterialProxyChangedDelagateHandle = TCallbackHandleContainer<void(
		const TSharedPtr<
			FMaterialProxy>&,
		EProxyModifyType,
		int32
		)
	>::FCallbackHandleSPtr;

	FOnMaterialProxyChangedDelagateHandle OnMaterialProxyChangedDelagateHandle;

	using FOnConsumableProxyChangedDelagateHandle = TCallbackHandleContainer<void(
		const TSharedPtr<
			FConsumableProxy>&,
		EProxyModifyType,
		int32
		)
	>::FCallbackHandleSPtr;

	FOnConsumableProxyChangedDelagateHandle OnConsumableProxyChangedDelagateHandle;

	using FOnActiveSkillProxyChangedDelagateHandle = TCallbackHandleContainer<void(
		const TSharedPtr<
			FSkillProxy>&,
		EProxyModifyType
		)
	>::FCallbackHandleSPtr;

	FOnActiveSkillProxyChangedDelagateHandle OnActiveSkillProxyChangedDelagateHandle;

	using FOnPassiveSkillProxyChangedDelagateHandle = TCallbackHandleContainer<void(
		const TSharedPtr<
			FSkillProxy>&,
		EProxyModifyType
		)
	>::FCallbackHandleSPtr;

	FOnPassiveSkillProxyChangedDelagateHandle OnPassiveSkillProxyChangedDelagateHandle;
};
