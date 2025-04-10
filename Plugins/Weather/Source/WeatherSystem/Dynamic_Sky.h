// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include <map>
#include <functional>

#include "CoreMinimal.h"

#include "Dynamic_Sky.generated.h"

using FOnHourly = std::function<void(int32)>;

/**
 *	A library of the most common animation blueprint functions.
 */
UCLASS()
class WEATHER_API ADynamic_Sky : public AActor
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
	void SetDateAndTimeCPP(const FDateTime& DateTime);

	UFUNCTION(BlueprintImplementableEvent)
	int32 GetCurrentHour()const;

	UFUNCTION(BlueprintImplementableEvent)
	void SetUseRealWorldTime(bool bUseRealWorldTime);

	void AddtionalProcess();

	int32 AddOnHourly(const FOnHourly& OnHourly);

	void RemoveOnHourly(int32 ID);

	/**
	 * 调整UDS模拟的时间流逝速度
	 * @param Speed 
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void SetTimeSpeed(float Speed);

protected:

	UFUNCTION(BlueprintCallable)
	void OnHoury(int32 Hour);

	UFUNCTION(BlueprintCallable)
	void FirstCall();

	UFUNCTION(BlueprintCallable)
	void SecondCall();

	std::map<int32, FOnHourly>HourlyMap;

};
