// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"

template<typename ValueType>
class TScopeValue
{
public:

	TScopeValue()
	{

	}

	TScopeValue(const ValueType& ValueRef)
	{
		OriginalValue = &ValueRef;
	}

	TScopeValue(ValueType&& ValueRef)
	{

	}

	~TScopeValue()
	{
		ReStore();
	}

	void SetValue(ValueType& ValueRef)
	{
		OriginalValue = ValueRef;
		ValuePtr = &ValueRef;
	}

	void ReStore()
	{
		if (ValuePtr)
		{
			*ValuePtr = OriginalValue;
		}
		else
		{
			ValuePtr = nullptr;
		}
	}

private:

	ValueType* ValuePtr = nullptr;

	ValueType OriginalValue;

};
