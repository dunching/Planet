// Zowee. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"

#include <GenerateType.h>

#include "GameplayTagContainer.h"
#include "ItemProxy_Minimal.h"

#include "GameOptions.generated.h"

USTRUCT(BlueprintType, Blueprintable)
struct PLANET_API FGameplayFeatureKeyMap
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FKey Key;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FString CMD;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FString Description;
};

UCLASS(BlueprintType, Blueprintable)
class PLANET_API UGameOptions : public UGameUserSettings
{
	GENERATED_BODY()

public:
	UGameOptions(const FObjectInitializer& ObjectInitializer);

	static UGameOptions* GetInstance();

	virtual void PostCDOContruct() override;

	virtual void PostInitProperties() override;
	
	TArray<FGameplayFeatureKeyMap> GetGameplayFeatureKeyMapAry()const;

	TMap<FGameplayTag, FKey>GetActionKeyMap()const;
	
	// 
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "ActiveSkill")
	int32 ResetCooldownTime = 1;

	// 攻击范围的偏移，比如某个技能施法范围在550，人物需要移动到 550 - MoveToAttaclAreaOffset里面去
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "MoveToAttaclArea")
	int32 MoveToAttaclAreaOffset = 50;

	// 模拟角色 HP 变化值低于此值时不显示跳字
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "MoveToAttaclArea")
	int32 NPC_HP_Display_MoveToAttaclAreaOffset = 10;

	// 生命值低于此百分比时，显示‘重伤“效果
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "MoveToAttaclArea")
	int32 LowerHP_Percent = 20;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "MoveToAttaclArea")
	bool bIsAllowAdjustTime = true;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "MoveSpeed")
	int32 MaxMoveSpeed = 1000;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "MoveSpeed")
	int32 MaxPerformSpeed = 300;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Keys")
	FKey DashKey = EKeys::LeftShift;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Keys")
	FKey RunKey = EKeys::LeftControl;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Keys")
	FKey FocusTarget = EKeys::Tab;

	static int32 MaxTeammateNum;

protected:
	/**
	 * 
	 */
	// UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TArray<FGameplayFeatureKeyMap> GameplayFeatureKeyMapAry;

	/**
	 * 技能槽对应的按键
	 */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TMap<FGameplayTag, FKey> ActionKeyMap;

private:
};
