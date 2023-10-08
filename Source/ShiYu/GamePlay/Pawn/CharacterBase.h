// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Character/GravityCharacter.h"

#include "PawnDataStruct.h"
#include "ItemType.h"
#include "CharacterBase.generated.h"

class AEquipmentBase;
class UAnimInstanceBase;
class FHoldItemsData;
class ASceneObj;
class UPawnIteractionComponent;
class UEquipmentInteractionComponent;
class UHoldItemComponent;
class UInputProcessorSubSystem;
class UZYInputComponent;
class UEquipmentSocksComponent;
class USceneObjPropertyComponent;

UCLASS(BlueprintType, Blueprintable)
class SHIYU_API UPawnIteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UPawnIteractionComponent(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay()override;

	EFactionType GetFactionType()const { return CurFactionType; }

	void SetFactionType(EFactionType NewFactionType) { CurFactionType = NewFactionType; }

	EEquipmentType GetDefaultEquipType()const { return DefaultEquipType; }

	void SetDefaultEquipType(EEquipmentType NewItemsType) { DefaultEquipType = NewItemsType; }

	EAttackState GetAttackState()const { return AttackState; }

	bool Attack(int32 Val);

	static FName ComponentName;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "property")
		EFactionType CurFactionType = EFactionType::kNone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "property")
		EEquipmentType DefaultEquipType = EEquipmentType::kNone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "property")
		EAttackState AttackState = EAttackState::kNone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "property")
		float HP = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "property")
		float VertigoVal = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "property")
		float HungerVal = 0.f;

private:

};

UCLASS()
class SHIYU_API ACharacterBase : public AGravityCharacter
{
	GENERATED_BODY()

public:

	ACharacterBase(const FObjectInitializer& ObjectInitializer);

	virtual ~ACharacterBase();

	template<typename Type = UAnimInstanceBase>
	Type* GetAnimationIns();

	template<typename Type = UHoldItemComponent>
	Type* GetHoldItemComponent()
	{
		return Cast<Type>(HoldItemComponentPtr);
	}

	template<typename Type = UPawnIteractionComponent>
	Type* GetPawnPropertyComponent()
	{
		return  Cast<Type>(PawnPropertyComponentPtr);
	}

	template<typename Type = UEquipmentSocksComponent>
	Type* GetEquipmentSocksComponent()
	{
		return Cast<Type>(EquipmentSocksComponentPtr);
	}

protected:

	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void PossessedBy(AController* NewController) override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	void AddMappingContext();

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Anim")
		float BaseTurnRate = 45.f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "Anim")
		float BaseLookUpRate = 45.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = InputProcess)
		UZYInputComponent* InputComponentPtr = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Interactuib)
		UPawnIteractionComponent* PawnPropertyComponentPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
		USceneObjPropertyComponent* ItemInteractionComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Equip)
		UEquipmentSocksComponent* EquipmentSocksComponentPtr = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Data)
		UHoldItemComponent* HoldItemComponentPtr = nullptr;

private:

};

template<typename Type>
Type* ACharacterBase::GetAnimationIns()
{
	auto CurSkeletalMeshCompPtr = GetMesh();
	if (CurSkeletalMeshCompPtr)
	{
		return Cast<Type>(CurSkeletalMeshCompPtr->GetAnimInstance());
	}

	return nullptr;
}
