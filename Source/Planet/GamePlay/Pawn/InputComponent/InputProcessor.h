// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include <atomic>

#include "CoreMinimal.h"
#include "InputActionValue.h"

#include "GenerateType.h"
#include "Planet.h"

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

	// 进入当前状态
	virtual void EnterAction();

	// 重新进入
	virtual void ReEnterAction();

	// 通知退出
	virtual void QuitAction();

	// 开始析构
	virtual void BeginDestroy();

	virtual void InputKey(const FInputKeyParams& Params);

	virtual void MoveForward(const FInputActionValue& InputActionValue);

	virtual void MoveRight(const FInputActionValue& InputActionValue);

	virtual void MoveUp(const FInputActionValue& InputActionValue);

	virtual void MouseWheel(const FInputActionValue& InputActionValue);

	virtual void AddPitchInput(const FInputActionValue& InputActionValue);

	virtual void AddYawInput(const FInputActionValue& InputActionValue);

	virtual void F1KeyPressed();

	virtual void F2KeyPressed();

	virtual void QKeyPressed();

	virtual void WKeyPressed();

	virtual void WKeyReleased();

	virtual void EKeyPressed();

	virtual void EKeyReleased();

	virtual void RKeyPressed();

	virtual void TKeyPressed();

	virtual void YKeyPressed();

	virtual void AKeyPressed();

	virtual void AKeyReleased();

	virtual void SKeyPressed();

	virtual void SKeyReleased();

	virtual void DKeyPressed();

	virtual void DKeyReleased();

	virtual void FKeyPressed();

	virtual void CKeyPressed();

	virtual void VKeyPressed();

	virtual void BKeyPressed();

	virtual void GKeyPressed();

	virtual void HKeyPressed();

	virtual void ESCKeyPressed();

	virtual void TabKeyPressed();

	virtual void LCtrlKeyPressed();

	virtual void LCtrlKeyReleased();

	virtual void LAltKeyPressed();

	virtual void LAltKeyReleased();

	virtual void LShiftKeyPressed();

	virtual void LShiftKeyReleased();

	virtual void SpaceKeyPressed();

	virtual void SpaceKeyReleased();

	virtual void MouseLeftPressed();

	virtual void MouseLeftReleased();

	virtual void MouseRightPressed();

	virtual void MouseRightReleased();

	virtual void PressedNumKey(int32 NumKey);

    bool GetIsComplete()const;

	bool GetIsRequestQuit()const;

	void SetPawn(FOwnerPawnType*NewPawnPtr);

protected:

	void IncreaseAsyncTaskNum();
	
	void ReduceAsyncTaskNum();

	virtual void TickImp(float Delta);

	FOwnerPawnType* OnwerPawnPtr = nullptr;

private:

	bool Tick(float Delta);

    std::atomic<bool>bIsRequestQuit = false;

	std::atomic<uint8>AsyncTaskNum = 0;

	const float Frequency = 1.f / 60;

	FTSTicker::FDelegateHandle TickDelegateHandle;
};
