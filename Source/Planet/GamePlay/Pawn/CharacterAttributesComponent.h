// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "GenerateType.h"
#include "ItemProxy_Minimal.h"
#include "CharacterAttibutes.h"

#include "CharacterAttributesComponent.generated.h"

class IPlanetControllerInterface;
class UGAEvent_Received;
class UAS_Character;
class UGE_CharacterInitail;

struct FCharacterAttributes;

UCLASS(BlueprintType, Blueprintable)
class PLANET_API UCharacterAttributesComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	using FOwnerType = ACharacterBase;

	using FPawnType = ACharacterBase;

	UCharacterAttributesComponent(const FObjectInitializer& ObjectInitializer);

	static FName ComponentName;

	const UAS_Character * GetCharacterAttributes()const;

	// 基础状态回复
	void ProcessCharacterAttributes();

	float GetRate()const;
	
protected:
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void BeginPlay()override;

	virtual void TickComponent(
		float DeltaTime,
		enum ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	)override;

	// 初始化GE
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GE")
	TSubclassOf<UGE_CharacterInitail>GE_InitailCharacterClass;

	// 自动回复GE
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GE")
	TSubclassOf<UGameplayEffect>GE_CharacterReplyClass;
   
private:

};