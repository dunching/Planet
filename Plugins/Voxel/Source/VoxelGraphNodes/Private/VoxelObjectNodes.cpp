// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelObjectNodes.h"
#include "VoxelObjectPinType.h"

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_IsValidObject, Result)
{
	if (AreTemplatePinsBuffers())
	{
		const TValue<FVoxelBuffer> Objects = GetNodeRuntime().Get<FVoxelBuffer>(ObjectPin, Query);
		return VOXEL_ON_COMPLETE(Objects)
		{
			if (!ensure(Objects->GetInnerType().IsStruct()))
			{
				VOXEL_MESSAGE(Error, "{0}: Object pin must be Object type", this);
				return {};
			}

			const FVoxelObjectPinType* ObjectPinType = FVoxelObjectPinType::StructToPinType().FindRef(Objects->GetInnerType().GetStruct());
			if (!ensure(ObjectPinType))
			{
				VOXEL_MESSAGE(Error, "{0}: Object pin must be Object type", this);
				return {};
			}

			FVoxelBoolBufferStorage Result;
			Result.Allocate(Objects->Num());

			for (int32 Index = 0; Index < Objects->Num(); Index++)
			{
				Result[Index] = !ObjectPinType->GetWeakObject(Objects->GetGeneric(Index).GetStructView()).IsExplicitlyNull();
			}

			return FVoxelRuntimePinValue::Make(FVoxelBoolBuffer::Make(Result));
		};
	}
	else
	{
		const FVoxelFutureValue Object = GetNodeRuntime().Get(ObjectPin, Query);
		return VOXEL_ON_COMPLETE(Object)
		{
			if (!ensure(Object.GetType().IsStruct()))
			{
				VOXEL_MESSAGE(Error, "{0}: Object pin must be Object type", this);
				return {};
			}

			const FVoxelObjectPinType* ObjectPinType = FVoxelObjectPinType::StructToPinType().FindRef(Object.GetType().GetStruct());
			if (!ensure(ObjectPinType))
			{
				VOXEL_MESSAGE(Error, "{0}: Object pin must be Object type", this);
				return {};
			}

			const bool bIsValid = !ObjectPinType->GetWeakObject(Object.GetStructView()).IsExplicitlyNull();
			return FVoxelRuntimePinValue::Make(bIsValid);
		}; 
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#if WITH_EDITOR
FVoxelPinTypeSet FVoxelNode_IsValidObject::GetPromotionTypes(const FVoxelPin& Pin) const
{
	if (Pin.Name == ResultPin)
	{
		FVoxelPinTypeSet OutTypes;
		OutTypes.Add<bool>();
		OutTypes.Add<FVoxelBoolBuffer>();
		return OutTypes;
	}

	return FVoxelPinTypeSet::AllObjects();
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

DEFINE_VOXEL_NODE_COMPUTE(FVoxelNode_EqualBase_Object, Result)
{
	if (AreTemplatePinsBuffers())
	{
		const TValue<FVoxelBuffer> A = GetNodeRuntime().Get<FVoxelBuffer>(APin, Query);
		const TValue<FVoxelBuffer> B = GetNodeRuntime().Get<FVoxelBuffer>(BPin, Query);
		return VOXEL_ON_COMPLETE(A, B)
		{
			CheckVoxelBuffersNum(*A, *B);

			if (!ensure(A->GetInnerType().IsStruct()))
			{
				VOXEL_MESSAGE(Error, "{0}: Object pin must be Object type", this);
				return {};
			}

			const FVoxelObjectPinType* ObjectPinType = FVoxelObjectPinType::StructToPinType().FindRef(A->GetInnerType().GetStruct());
			if (!ensure(ObjectPinType))
			{
				VOXEL_MESSAGE(Error, "{0}: Object pin must be Object type", this);
				return {};
			}

			if (A->GetInnerType() != B->GetInnerType())
			{
				return FVoxelRuntimePinValue::Make(FVoxelBoolBuffer::Make(GetDifferentTypesValues()));
			}

			FVoxelBoolBufferStorage Result;
			Result.Allocate(A->Num());

			for (int32 Index = 0; Index < A->Num(); Index++)
			{
				const bool bResult = EqualityCheck(ObjectPinType->GetWeakObject(A->GetGeneric(Index).GetStructView()), ObjectPinType->GetWeakObject(B->GetGeneric(Index).GetStructView()));
				Result[Index] = bResult;
			}

			return FVoxelRuntimePinValue::Make(FVoxelBoolBuffer::Make(Result));
		};
	}
	else
	{
		const FVoxelFutureValue A = GetNodeRuntime().Get(APin, Query);
		const FVoxelFutureValue B = GetNodeRuntime().Get(BPin, Query);
		return VOXEL_ON_COMPLETE(A, B)
		{
			if (!ensure(A.GetType().IsStruct()))
			{
				VOXEL_MESSAGE(Error, "{0}: Object pin must be Object type", this);
				return {};
			}

			const FVoxelObjectPinType* ObjectPinType = FVoxelObjectPinType::StructToPinType().FindRef(A.GetType().GetStruct());
			if (!ensure(ObjectPinType))
			{
				VOXEL_MESSAGE(Error, "{0}: Object pin must be Object type", this);
				return {};
			}

			if (A.GetType() != B.GetType())
			{
				return FVoxelRuntimePinValue::Make(GetDifferentTypesValues());
			}

			const bool bResult = EqualityCheck(ObjectPinType->GetWeakObject(A.GetStructView()), ObjectPinType->GetWeakObject(B.GetStructView()));
			return FVoxelRuntimePinValue::Make(bResult);
		}; 
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

#if WITH_EDITOR
FVoxelPinTypeSet FVoxelNode_EqualBase_Object::GetPromotionTypes(const FVoxelPin& Pin) const
{
	if (Pin.Name == ResultPin)
	{
		FVoxelPinTypeSet OutTypes;
		OutTypes.Add<bool>();
		OutTypes.Add<FVoxelBoolBuffer>();
		return OutTypes;
	}

	return FVoxelPinTypeSet::AllObjects();
}

void FVoxelNode_EqualBase_Object::PromotePin(FVoxelPin& Pin, const FVoxelPinType& NewType)
{
	if (Pin.Name != ResultPin)
	{
		GetPin(APin).SetType(NewType);
		GetPin(BPin).SetType(NewType);
	}

	Super::PromotePin(Pin, NewType);
}
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool FVoxelNode_Equal_Object::EqualityCheck(const TWeakObjectPtr<UObject>& A, const TWeakObjectPtr<UObject>& B) const
{
	return A == B;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

bool FVoxelNode_NotEqual_Object::EqualityCheck(const TWeakObjectPtr<UObject>& A, const TWeakObjectPtr<UObject>& B) const
{
	return A != B;
}