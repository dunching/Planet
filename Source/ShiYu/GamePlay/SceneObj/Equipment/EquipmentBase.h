// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "SceneObj/SceneObj.h"
#include "Pawn/PawnDataStruct.h"
#include "Common/GenerateType.h"

#include "EquipmentBase.generated.h"

class AHumanCharacter;
class USkeletalMesh;
class UClass;

UCLASS()
class SHIYU_API UEquipmentInteractionComponent : public USceneObjPropertyComponent
{
	GENERATED_BODY()

public:

	UEquipmentInteractionComponent(const FObjectInitializer& ObjectInitializer);

	int32 GetAttackPower()const {
		return AttackPower;
	}

	FName GetSocketName()const;

	EEquipmentType GetEquipmenttype() const
	{
		return CurrentEquipmentType;
	}

	EAnimCMDType GetAnimCMDType() const
	{
		return AnimCMDType;
	}

	EItemSocketType GetItemSocketType() const
	{
		return ItemSocketType;
	}

protected:

	virtual void BeginPlay()override;

	UFUNCTION(BlueprintNativeEvent)
		int32 GetInitialAttackPower();

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		FName SocksName;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		EEquipmentType CurrentEquipmentType = EEquipmentType::kNone;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		EAnimCMDType AnimCMDType = EAnimCMDType::kNone;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
		EItemSocketType ItemSocketType = EItemSocketType::kNone;

private:

	int32 AttackPower = 0;

};

UCLASS()
class SHIYU_API AEquipmentBase : public ASceneObj
{
	GENERATED_BODY()

public:

	AEquipmentBase(const FObjectInitializer& ObjectInitializer);

	virtual void EquipItemToCharacter(AHumanCharacter* CharacterPtr);

	template<typename Type = ACharacterBase>
	Type* GetOwnerActor() {
		return Cast<Type>(OnwerPawnPtr);
	}

	virtual void DoAction(EEquipmentActionType ActionType);

protected:

	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	ACharacterBase* OnwerPawnPtr = nullptr;

private:

};
