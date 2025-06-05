// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "LayoutCommon.generated.h"

UENUM(BlueprintType)
enum class ELayoutCommon : uint8
{
	kEmptyLayout,			// 空白的模式，
	kActionLayout,			// 默认的模式，
	kMenuLayout,			// 查看菜单
	kEndangeredLayout,		// 重伤状态
	kOptionLayout,			// 选择条目状态
	kConversationLayout,	// 对话状态
	kTransactionLayout,		// 商店交易状态
	kBuildingLayout,
	kViewTasksLayout,		// 任务模式
	kTransitionLayout,		// 切换、过度界面
	kObserverLayout,		// 观战
};
