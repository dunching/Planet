// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Misc/MessageDialog.h"
#include "VoxelEngineVersionHelpers.h"

// Engine one doesn't support restricted pointers
template<typename T>
FORCEINLINE void Swap(T* RESTRICT& A, T* RESTRICT& B)
{
	T* RESTRICT Temp = A;
	A = B;
	B = Temp;
}

#if VOXEL_ENGINE_VERSION <= 502
struct FMessageDialogCompat : public FMessageDialog
{
	using FMessageDialog::Open;

	static EAppReturnType::Type Open(
		const EAppMsgType::Type MessageType,
		const EAppReturnType::Type DefaultValue,
		const FText& Message,
		const FText& Title)
	{
		return Open(MessageType, DefaultValue, Message, &Title);
	}
};

#define FMessageDialog FMessageDialogCompat
#endif