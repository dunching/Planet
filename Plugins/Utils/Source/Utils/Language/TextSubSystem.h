// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <atomic>

#include "CoreMinimal.h"

#include "TextSubSystem.generated.h"

enum class ELanguageType :uint8
{
	kChinese,
	kEnglish,
};

UCLASS()
class UTILS_API UTextSubSystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:

	static UTextSubSystem* GetInstance();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void ReadData();

	void SupplementDefaultData();

	FString GetText(const FString& Text)const;

	TMap<FString, FString>TextMap;

	ELanguageType CurrentLanguageType = ELanguageType::kChinese;

};
