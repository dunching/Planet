// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/GameModeBase.h"

#include "ThreadHelper.h"

#include "UI/UIManagerSubSystem.h"

#include "ShiYuGameMode.generated.h"

extern SHIYU_API class AShiYuGameMode* GGameModePtr;

class FUIManager;

enum EThreadHelperType
{
	kSpawnArticle,		// ����AI
	KProcessAICMD,		// ִ��AI
	kCollectItemCV,		// AI�ռ���Ʒ���߳�
	kUpdateIteraction,	// ����������ҽ������߳�
	kThreadTypeCount,
};

/**
 *
 */
UCLASS()
class SHIYU_API AShiYuGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	AShiYuGameMode();

protected:

	virtual void OnConstruction(const FTransform& Transform)override;

	virtual void BeginPlay()override;

	virtual void BeginDestroy()override;

	UFUNCTION()
		void OnVoxelWorldGenerated();

	UFUNCTION()
		void OnVoxelWorldLoad();

private:

	void LoadGameImp();

	void SaveGameImp();

	FTimerHandle SaveGameTimer;

};
