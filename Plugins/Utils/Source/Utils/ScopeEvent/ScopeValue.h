// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"

template<typename ValueType>
class TScopeValue
{
public:

	TScopeValue(ValueType& ValueRef):
		ValueRef(ValueRef)
	{
		OriginalValue = ValueRef;
	}

	TScopeValue(ValueType& ValueRef, ValueType&& NewValue,ValueType&& Value):
		ValueRef(ValueRef),
		OriginalValue(Value)
	{
		OriginalValue = ValueRef;
		ValueRef = NewValue;
		bUesCustomValue = true;
	}

	~TScopeValue()
	{
		ReStore();
	}

	void ReStore()
	{
		ValueRef = OriginalValue;
	}

private:

	bool bUesCustomValue = false;

	ValueType & ValueRef;

	ValueType OriginalValue;

};
