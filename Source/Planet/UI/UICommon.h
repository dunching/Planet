// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

enum EUIOrder :uint8
{
	kCharacter_State_HUD,
	kFightingTips,
	kFocus,
	kMainUI,
	kTableMenu,
};

template<typename InheritsType>
struct TGetSocketName
{
	static InheritsType Get()
	{
		InheritsType InheritsTypeIns;

		return InheritsTypeIns;
	}
};