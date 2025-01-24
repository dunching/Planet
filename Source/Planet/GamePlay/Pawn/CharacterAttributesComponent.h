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
	
	virtual void SetCharacterID(const FGuid& InCharacterID);

	FGuid GetCharacterID()const;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Pawn")
	FGameplayTag CharacterGrowthAttribute = FGameplayTag::EmptyTag;

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
   
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Anim")
	float BaseTurnRate = 45.f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Anim")
	float BaseLookUpRate = 45.f;

	// Character的ID
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	FGuid CharacterID;

	// Character的类别
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly)
	FString CharacterCategory;
	
private:

	UFUNCTION()
	void OnRep_CharacterID();

};

UCLASS()
class UGameplayStatics_Character : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	static PLANET_API ACharacterBase* GetCharacterByID(
		const UObject* WorldContextObject,
		TSubclassOf<ACharacterBase> ActorClass,
		const FGuid&CharacterID
		);
};