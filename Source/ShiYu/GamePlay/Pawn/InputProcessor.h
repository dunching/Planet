// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include <atomic>

#include "CoreMinimal.h"
#include "InputActionValue.h"

#include "GenerateType.h"
#include "ShiYu.h"

class APawn;

class FInputProcessor
{
public:

	GENERATIONCLASSINFOONLYTHIS(FInputProcessor);

	using FOwnerPawnType = APawn;

	using FOnQuitComplete = std::function<void()>;

	FInputProcessor(APawn* CharacterPtr);

	virtual ~FInputProcessor();

	FInputProcessor(const FInputProcessor&) = delete;

	FInputProcessor(FInputProcessor&&) = delete;

	FInputProcessor& operator=(const FInputProcessor&) = delete;

	FInputProcessor& operator=(FInputProcessor&&) = delete;

	template<typename Type = APawn>
	Type* GetOwnerActor() { return Cast<Type>(OnwerPawnPtr); }

	// 进入当前状态
	virtual void EnterAction();

	// 重新进入
	virtual void ReEnterAction();

	// 通知退出
	virtual void QuitAction();

	// 开始析构
	virtual void BeginDestroy();

	virtual void MoveForward(const FInputActionValue& InputActionValue);

	virtual void MoveRight(const FInputActionValue& InputActionValue);

	virtual void MoveUp(const FInputActionValue& InputActionValue);

	virtual void MouseWheel(const FInputActionValue& InputActionValue);

	virtual void AddPitchInput(const FInputActionValue& InputActionValue);

	virtual void AddYawInput(const FInputActionValue& InputActionValue);

	virtual void WKeyPressed();

	virtual void WKeyReleased();

	virtual void AKeyPressed();

	virtual void AKeyReleased();

	virtual void SKeyPressed();

	virtual void SKeyReleased();

	virtual void DKeyPressed();

	virtual void DKeyReleased();

	virtual void QKeyPressed();

	virtual void EKeyPressed();

	virtual void EKeyReleased();

	virtual void RKeyPressed();

	virtual void TKeyPressed();

	virtual void YKeyPressed();

	virtual void FKeyPressed();

	virtual void CKeyPressed();

	virtual void VKeyPressed();

	virtual void BKeyPressed();

	virtual void GKeyPressed();

	virtual void ESCKeyPressed();

	virtual void LCtrlKeyPressed();

	virtual void LCtrlKeyReleased();

	virtual void LShiftKeyPressed();

	virtual void LShiftKeyReleased();

	virtual void SpaceKeyPressed();

	virtual void SpaceKeyReleased();

	virtual void MouseLeftReleased();

	virtual void MouseLeftPressed();

	virtual void MouseRightReleased();

	virtual void MouseRightPressed();

	virtual void PressedNumKey(int32 NumKey);

	std::atomic<bool>bNeedQuit = false;

	std::atomic<bool>bHaveDone = false;

protected:

	// 调用此函数才会析构此类
	void MarkHaveDone();

	const static EInputProcessorType InputProcessprType = EInputProcessorType::kNone;

	APawn* OnwerPawnPtr = nullptr;

private:

};
