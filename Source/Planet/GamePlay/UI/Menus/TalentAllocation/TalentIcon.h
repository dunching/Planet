// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UIInterfaces.h"
#include "TalentUnit.h"

#include "TalentIcon.generated.h"

struct FStreamableHandle;

/**
 *
 */
UCLASS()
class PLANET_API UTalentIcon : public UUserWidget
{
	GENERATED_BODY()

public:

protected:

	virtual void NativeConstruct()override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)override;

	void ResetUI(const FTalentHelper& TalentHelper);

	FTalentHelper GetTalentHelper()const;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EPointType PointType = EPointType::kSkill;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EPointSkillType PointSkillType = EPointSkillType::kNuQi;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EPointPropertyType PointPropertyType = EPointPropertyType::kLiDao;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 CurrentLevel = 0;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTag IconSocket;

};