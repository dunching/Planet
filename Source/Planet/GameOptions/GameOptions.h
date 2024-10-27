// Zowee. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include <GenerateType.h>

#include "GameplayTagContainer.h"
#include "SceneElement.h"

#include "GameOptions.generated.h"

UCLASS(BlueprintType, Blueprintable)
class PLANET_API UGameOptions : public UObject
{
	GENERATED_BODY()
public:

	static UGameOptions* GetInstance();
	
	// 
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "ActiveSkill")
	int32 ResetCooldownTime = 1;

	// 攻击范围的偏移，比如某个技能施法范围在550，人物需要移动到 550 - MoveToAttaclAreaOffset里面去
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "MoveToAttaclArea")
	int32 MoveToAttaclAreaOffset = 50;
	
	// 模拟角色 HP 变化值低于此值时不显示跳字
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "MoveToAttaclArea")
	int32 NPC_HP_Display_MoveToAttaclAreaOffset = 10;
	
	// 生命值低于此百分比时，显示‘重伤“效果
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "MoveToAttaclArea")
	int32 LowerHP_Percent = 20;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "MoveToAttaclArea")
	bool bIsAllowAdjustTime = true;

};
