// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <GameplayTagContainer.h>
#include <Subsystems/GameInstanceSubsystem.h>

#include "GameplayTagsSubSystem.generated.h"

UCLASS()
class PLANET_API UGameplayTagsSubSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	static UGameplayTagsSubSystem* GetInstance();

#pragma region Locamotion Tags
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities Tag")
	FGameplayTag Jump =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Locomotion.Jump")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities Tag")
	FGameplayTag Running =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Locomotion.Run")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities Tag")
	FGameplayTag Dash =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Locomotion.Dash")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities Tag")
	FGameplayTag Mount =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Locomotion.Mount")));
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities Tag")
	FGameplayTag DisMount =
		FGameplayTag::RequestGameplayTag(FName(TEXT("Locomotion.DisMount")));
#pragma endregion 

#pragma region State Tags
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities Tag")
	FGameplayTag UsingConsumable =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.UsingConsumable")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities Tag")
	FGameplayTag Ridering =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.Ridering")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities Tag")
	FGameplayTag SuperArmor =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.Buff.SuperArmor")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities Tag")
	FGameplayTag FlyAway =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.Debuff.FlyAway")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities Tag")
	FGameplayTag DeathingTag = 
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.Deathing")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Abilities Tag")
	FGameplayTag InFightingTag = 
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.InFighting")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Gameplay Tag")
	FGameplayTag GameplayTag1 =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.ReleasingSkill.Continuous")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Gameplay Tag")
	FGameplayTag GameplayTag2 =
		FGameplayTag::RequestGameplayTag(FName(TEXT("State.ReleasingSkill.WeaponActive")));
#pragma endregion 

#pragma region UISocket Tags
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI Socket Tag")
	FGameplayTag WeaponActiveSocket1 =
		FGameplayTag::RequestGameplayTag(FName(TEXT("UI.SkillSocket.WeaponActiveSocket1")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI Socket Tag")
	FGameplayTag WeaponActiveSocket2 =
		FGameplayTag::RequestGameplayTag(FName(TEXT("UI.SkillSocket.WeaponActiveSocket2")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI Socket Tag")
	FGameplayTag ActiveSocket1 =
		FGameplayTag::RequestGameplayTag(FName(TEXT("UI.SkillSocket.ActiveSocket1")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI Socket Tag")
	FGameplayTag ActiveSocket2 =
		FGameplayTag::RequestGameplayTag(FName(TEXT("UI.SkillSocket.ActiveSocket2")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI Socket Tag")
	FGameplayTag ActiveSocket3 =
		FGameplayTag::RequestGameplayTag(FName(TEXT("UI.SkillSocket.ActiveSocket3")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI Socket Tag")
	FGameplayTag ActiveSocket4 =
		FGameplayTag::RequestGameplayTag(FName(TEXT("UI.SkillSocket.ActiveSocket4")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI Socket Tag")
	FGameplayTag Talent_NuQi_Socket =
		FGameplayTag::RequestGameplayTag(FName(TEXT("UI.SkillSocket.Talent_NuQi_Socket")));

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "UI Socket Tag")
	FGameplayTag Talent_YinYang_Socket =
		FGameplayTag::RequestGameplayTag(FName(TEXT("UI.SkillSocket.Talent_YinYang_Socket")));
#pragma endregion 
};