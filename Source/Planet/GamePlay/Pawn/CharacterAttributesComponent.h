// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "GenerateType.h"
#include <SceneElement.h>
#include "CharacterAttibutes.h"

#include "CharacterAttributesComponent.generated.h"

class IPlanetControllerInterface;
class UGAEvent_Received;

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

	TSharedPtr<FCharacterAttributes> GetCharacterAttributes()const;

	// 基础状态回复
	void ProcessCharacterAttributes();

	TSharedPtr<FCharacterAttributes> CharacterAttributesSPtr;

protected:

	virtual void BeginPlay()override;

	virtual void TickComponent(
		float DeltaTime,
		enum ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	)override;

	// 将Client需要（显示）的数据从Server上同步过去
	UFUNCTION(Client, Reliable)
	void OnPropertyChanged(
		ECharacterPropertyType CharacterPropertyType,
		int32 CurrentValue
	);

private:

};