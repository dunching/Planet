// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UIInterfaces.h"
#include "Talent_FASI.h"

#include "TalentIcon.generated.h"

struct FStreamableHandle;

/**
 *
 */
UCLASS()
class PLANET_API UTalentIcon : public UMyUserWidget
{
	GENERATED_BODY()

public:

	// 
	using FDelegateHandle = TCallbackHandleContainer<void(UTalentIcon*, bool)>;

	void ResetPoint();

	FTalentHelper GetTalentHelper()const;

protected:

	virtual void NativeConstruct()override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)override;

	void ResetUI(const FTalentHelper& TalentHelper);

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EPointType PointType = EPointType::kSkill;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EPointSkillType PointSkillType = EPointSkillType::kNuQi;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EPointPropertyType PointPropertyType = EPointPropertyType::kLiDao;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTag IconSocket;

	FDelegateHandle OnValueChanged;

};