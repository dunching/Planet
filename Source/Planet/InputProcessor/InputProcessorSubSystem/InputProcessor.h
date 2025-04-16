// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include <atomic>

#include "CoreMinimal.h"
#include "InputActionValue.h"

#include "GenerateType.h"
#include "Tools.h"
#include "Planet.h"

struct FInputKeyEventArgs;

class FViewport;
class APawn;

class FInputProcessor
{
public:

	GENERATIONCLASSINFOONLYTHIS(FInputProcessor);

	using FOwnerPawnType = APawn;

	using FOnQuitComplete = std::function<void()>;

	FInputProcessor(FOwnerPawnType* CharacterPtr);

	virtual ~FInputProcessor();

	FInputProcessor(const FInputProcessor&) = delete;

	FInputProcessor(FInputProcessor&&) = delete;

	FInputProcessor& operator=(const FInputProcessor&) = delete;

	FInputProcessor& operator=(FInputProcessor&&) = delete;

	template<typename Type = FOwnerPawnType>
	Type* GetOwnerActor() { return Cast<Type>(OnwerPawnPtr); }

	// 
	virtual void EnterAction();

	// 
	virtual void ReEnterAction();

	// 
	virtual void QuitAction();

	// 
	virtual void BeginDestroy();

	virtual bool InputKey(
		const FInputKeyEventArgs& EventArgs
	) ;

	virtual bool InputAxis(
		FViewport* Viewport,
		FInputDeviceId InputDevice,
		FKey Key,
		float Delta,
		float DeltaTime,
		int32 NumSamples = 1,
		bool bGamepad = false
	) ;

    bool GetIsComplete()const;

	bool GetIsRequestQuit()const;

	void SetPawn(FOwnerPawnType*NewPawnPtr);

	void UnRegisterTicker();

protected:

	void IncreaseAsyncTaskNum();
	
	void ReduceAsyncTaskNum();

	virtual void TickImp(float Delta);

	void SwitchShowCursor(bool bIsShowCursor);

	FOwnerPawnType* OnwerPawnPtr = nullptr;

private:

	bool Tick(float Delta);

    std::atomic<bool>bIsRequestQuit = false;

	std::atomic<uint8>AsyncTaskNum = 0;

	const float Frequency = 1.f / 60;

	FTSTicker::FDelegateHandle TickDelegateHandle;
};
