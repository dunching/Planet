// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "GenerateType.h"
#include <ItemType.h>

#include "HoldItemComponent.generated.h"

// 持有“物品”的数据集
class FHoldItemsData
{

public:

	// 所在所持有的物品、变化的物品、变化的类型
	using FOnItemChangeNotifyCB = std::function<void(const FHoldItemsData&, EItemChangeType)>;

	void AddItem(const FItemAry& TempItemAry);

	void RemoveItem(const FItemAry& TempItemAry);

	const FItemAry& GetItemsAry()const;

	const FCreatingMap& GetCreateMap()const;

	const FCanbeCreatinedSet& GetCanCreateMap()const;

	bool CanCreateItem(FItemNum ItemsType)const;

	void CreatedItem(FItemNum ItemsType);

	void UnCreatedItem(FItemNum ItemsType);

	int32 GetItemCount(FItemNum ItemsType)const;

	void SetItemChangeCB(
		EOnItemChangeNotityObjType OnItemChangeNotityObjType,
		const FOnItemChangeNotifyCB& OnItemChangeNotifyCB
	);

	void RemoveItemChangeCB(
		EOnItemChangeNotityObjType OnItemChangeNotityObjType
	);

	void ClearItemChangeCB();

	void InitCreateMap(const FCreatingMap& NewCreateMap);

	void InitCanBeCreatedSet(const FCanbeCreatinedSet& NewCanBeCreatedSet);

	void InitItemMap(const FItemAry& NewItemMap);

private:

	void OnChange(const FItemAry& ChangeItemMap, EItemChangeType ItemChangeType);

	// 所有持有物品
	FItemAry ItemAry;

	// 制作物品所需的材料
	FCreatingMap CreateMap;

	// 可以制作的物品
	FCanbeCreatinedSet CanCreateSet;

	TMap<EOnItemChangeNotityObjType, FOnItemChangeNotifyCB>OnItemChangeNotifyCBMap;

};

UCLASS(BlueprintType, Blueprintable)
class UHoldItemComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UHoldItemComponent(const FObjectInitializer& ObjectInitializer);

	TSharedPtr<FHoldItemsData>& GetHoldItemProperty() { return SPHoldItemPropertyPtr; };

	static FName ComponentName;

protected:

	virtual void BeginPlay()override;

private:

	TSharedPtr<FHoldItemsData>SPHoldItemPropertyPtr;

};