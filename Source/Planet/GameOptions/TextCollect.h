// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <atomic>

#include "CoreMinimal.h"

#define TEXTCOLLECT(Text) const FString Text = TEXT(#Text);

namespace TextCollect
{
	TEXTCOLLECT(BaseDamage);
	
	TEXTCOLLECT(Evade);

	TEXTCOLLECT(Critical);

	TEXTCOLLECT(GetSkill);

	TEXTCOLLECT(LoseSkill);

	TEXTCOLLECT(CharacterInteraction);

	TEXTCOLLECT(Return);
	
	TEXTCOLLECT(EntryActionProcessor);
	
	TEXTCOLLECT(ViewAllocationMenu);
	
	TEXTCOLLECT(ViewGroupmateMenu);
	
	TEXTCOLLECT(MainThread);
	
	TEXTCOLLECT(BrandThread);
}
