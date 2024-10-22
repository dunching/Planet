// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "TemplateHelper.h"

#include "BaseData.generated.h"

struct FScoped_BaseProperty_SaveUpdate;

USTRUCT(BlueprintType)
struct UTILS_API FBaseProperty
{
	GENERATED_USTRUCT_BODY()

public:

	friend FScoped_BaseProperty_SaveUpdate;

	FBaseProperty();

	FBaseProperty(int32 Value);

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);

	int32 GetCurrentValue() const;

	void SetCurrentValue(int32 NewValue);

	void AddCurrentValue(int32 val);

	TOnValueChangedCallbackContainer<int32> CallbackContainerHelper;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 CurrentValue = 0;

	bool bIsSaveUpdate = false;

protected:

};

struct FScoped_BaseProperty_SaveUpdate
{
	FScoped_BaseProperty_SaveUpdate(FBaseProperty& TargetRef)
		: TargetRef(TargetRef)
	{
		TargetRef.bIsSaveUpdate = true;
	}

	~FScoped_BaseProperty_SaveUpdate()
	{
		TargetRef.bIsSaveUpdate = false;

		TargetRef.CallbackContainerHelper.ValueChanged(TargetRef.CurrentValue, TargetRef.CurrentValue);
	}

private:
	FBaseProperty& TargetRef;
};
