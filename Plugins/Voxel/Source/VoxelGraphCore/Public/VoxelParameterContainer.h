// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelGraphInterface.h"
#include "VoxelMinimal.h"
#include "VoxelParameter.h"
#include "VoxelParameterPath.h"
#include "VoxelParameterProvider.h"
#include "VoxelParameterContainer.generated.h"

USTRUCT()
struct FVoxelParameterValueOverride
{
	GENERATED_BODY()

	UPROPERTY()
	bool bEnable = false;

	UPROPERTY()
	FName CachedName;

	UPROPERTY()
	FName CachedCategory;

	UPROPERTY()
	FVoxelPinValue Value;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

UCLASS()
class VOXELGRAPHCORE_API UVoxelParameterContainer
	: public UActorComponent
	, public IVoxelParameterProvider
{
	GENERATED_BODY()

public:
	UPROPERTY()
	bool bAlwaysEnabled = false;

	UPROPERTY()
	TObjectPtr<UObject> Provider;

	UPROPERTY()
	TMap<FVoxelParameterPath, FVoxelParameterValueOverride> ValueOverrides;

public:
	FSimpleMulticastDelegate OnChanged;
	FSimpleMulticastDelegate OnProviderChanged;

	template<typename T>
	T* GetTypedProvider() const
	{
		checkStatic(TIsDerivedFrom<T, IVoxelParameterProvider>::Value);
		ensure(!Provider || Provider->IsA<T>());
		return Cast<T>(Provider);
	}
	void SetProvider(IVoxelParameterProvider* NewProvider);

	void Fixup();
	void FixupProvider();

	//~ Begin UObject Interface
	virtual void PostLoad() override;
#if WITH_EDITOR
	virtual void PostEditUndo() override;
#endif
	//~ End UObject Interface

	//~ Begin IVoxelParameterProvider Interface
	virtual void AddOnChanged(const FSimpleDelegate& Delegate) override;
	virtual TSharedPtr<IVoxelParameterRootView> GetParameterViewImpl(const FVoxelParameterPath& BasePath) override;
	//~ End IVoxelParameterProvider Interface

public:
	// Use SetChecked if possible
	template<typename T>
	bool Set(const FName Name, const T& Value, FString* OutError = nullptr)
	{
		return this->Set(Name, FVoxelPinValue::Make(Value), OutError);
	}
	template<typename T>
	void SetChecked(const FName Name, const T& Value)
	{
		FString Error;
		if (!this->Set(Name, FVoxelPinValue::Make(Value), &Error))
		{
			ensureMsgf(false, TEXT("%s"), *Error);
		}
	}
	bool Set(
		FName Name,
		FVoxelPinValue Value,
		FString* OutError = nullptr);

private:
	FSharedVoidPtr DelegatePtr;
	TWeakObjectPtr<UObject> LastProvider;
};