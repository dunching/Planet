// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelCurveNode.h"
#include "VoxelDependency.h"
#include "VoxelCurveNodeImpl.ispc.generated.h"

void FVoxelCurve::ComputeRuntimeData()
{
	if (Wrapper)
	{
		return;
	}

	Wrapper = MakeShared<FVoxelCurveDataWrapper>(FVoxelDependency::Create(
			STATIC_FNAME("Curve"),
			STATIC_FNAME("Inline Curve")));

	Wrapper->Update(Curve);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

struct FVoxelCurveData
{
	TSharedPtr<const FRichCurve> Curve;

	FString Error;
	float DefaultValue = 0.f;
	TVoxelArray<ispc::FRichCurveKey> Keys;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

class FVoxelCurveManager : public FVoxelSingleton
{
public:
	//~ Begin FVoxelSingleton Interface
	virtual void Initialize() override
	{
#if WITH_EDITOR
		FCoreUObjectDelegates::OnObjectPropertyChanged.AddLambda([this](UObject* Object, const FPropertyChangedEvent& PropertyChangedEvent)
		{
			if (PropertyChangedEvent.ChangeType == EPropertyChangeType::Interactive)
			{
				return;
			}

			const UCurveFloat* Curve = Cast<UCurveFloat>(Object);
			if (!Curve)
			{
				return;
			}

			UpdateWrapper_GameThread(*Curve);
		});
#endif
	}
	//~ End FVoxelSingleton Interface

	TSharedRef<FVoxelCurveDataWrapper> GetWrapper_GameThread(const UCurveFloat& Curve)
	{
		VOXEL_FUNCTION_COUNTER();
		check(IsInGameThread());

		TSharedPtr<FVoxelCurveDataWrapper>& Wrapper = ObjectToWrapper.FindOrAdd(&Curve);
		if (Wrapper)
		{
			return Wrapper.ToSharedRef();
		}

		Wrapper = MakeVoxelShared<FVoxelCurveDataWrapper>(FVoxelDependency::Create(
			STATIC_FNAME("Curve"),
			Curve.GetFName()));

		UpdateWrapper_GameThread(Curve);

		return Wrapper.ToSharedRef();
	}
	void UpdateWrapper_GameThread(const UCurveFloat& CurveObject) const
	{
		VOXEL_FUNCTION_COUNTER();
		check(IsInGameThread());

		const TSharedPtr<FVoxelCurveDataWrapper> Wrapper = ObjectToWrapper.FindRef(&CurveObject);
		if (!Wrapper)
		{
			return;
		}

		Wrapper->Update(CurveObject.FloatCurve);
	}

private:
	TMap<TWeakObjectPtr<const UCurveFloat>, TSharedPtr<FVoxelCurveDataWrapper>> ObjectToWrapper;
};

FVoxelCurveManager* GVoxelCurveManager = MakeVoxelSingleton(FVoxelCurveManager);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelCurveDataWrapper::Update(const FRichCurve& Curve)
{
	VOXEL_FUNCTION_COUNTER();

	const TSharedRef<FVoxelCurveData> Data = MakeVoxelShared<FVoxelCurveData>();
	Data->Curve = MakeSharedCopy(Curve);
	Data->DefaultValue = Curve.DefaultValue;

	for (const FRichCurveKey& Key : Curve.Keys)
	{
		if (Key.TangentWeightMode != RCTWM_WeightedNone)
		{
			Data->Error += "Weighted tangents are not supported - please disable Fast Curve\n";
		}

		ispc::FRichCurveKey ISPCKey;
		ISPCKey.Time = Key.Time;
		ISPCKey.Value = Key.Value;
		ISPCKey.ArriveTangent = Key.ArriveTangent;
		ISPCKey.LeaveTangent = Key.LeaveTangent;

		switch (Key.InterpMode)
		{
		default: ensure(false);
		case RCIM_Linear: ISPCKey.InterpMode = ispc::RCIM_Linear; break;
		case RCIM_Constant: ISPCKey.InterpMode = ispc::RCIM_Constant; break;
		case RCIM_Cubic: ISPCKey.InterpMode = ispc::RCIM_Cubic; break;
		}

		Data->Keys.Add(ISPCKey);
	}

	// Add dummy keys for infinity
	if (Curve.Keys.Num() >= 2)
	{
		const double MaxTime = FMath::Max3(
			1.e3f,
			FMath::Abs(Curve.Keys[0].Time),
			FMath::Abs(Curve.Keys.Last().Time));

		if (Curve.PreInfinityExtrap == RCCE_Linear)
		{
			ispc::FRichCurveKey ISPCKey;
			ISPCKey.Time = -MaxTime;

			const double DeltaTime = Curve.Keys[1].Time - Curve.Keys[0].Time;
			if (FMath::IsNearlyZero(DeltaTime))
			{
				ISPCKey.Value = Curve.Keys[0].Value;
			}
			else
			{
				const double DeltaValue = Curve.Keys[1].Value - Curve.Keys[0].Value;
				const double Slope = DeltaValue / DeltaTime;

				ISPCKey.Value = Slope * (-MaxTime - double(Curve.Keys[0].Time)) + Curve.Keys[0].Value;
			}

			ISPCKey.ArriveTangent = 0;
			ISPCKey.LeaveTangent = 0;
			ISPCKey.InterpMode = ispc::RCIM_Linear;
			Data->Keys.Insert(ISPCKey, 0);
		}
		else if (Curve.PreInfinityExtrap == RCCE_Constant)
		{
			ispc::FRichCurveKey ISPCKey;
			ISPCKey.Time = -MaxTime;
			ISPCKey.Value = Curve.Keys[0].Value;
			ISPCKey.ArriveTangent = 0;
			ISPCKey.LeaveTangent = 0;
			ISPCKey.InterpMode = ispc::RCIM_Constant;
			Data->Keys.Insert(ISPCKey, 0);
		}
		else
		{
			Data->Error += "Unsupported PreInfinityExtrap: " + UEnum::GetValueAsString(Curve.PreInfinityExtrap) + " - please disable Fast Curve\n";
		}

		if (Curve.PostInfinityExtrap == RCCE_Linear)
		{
			ispc::FRichCurveKey ISPCKey;
			ISPCKey.Time = MaxTime;

			const double DeltaTime = Curve.Keys.Last(1).Time - Curve.Keys.Last().Time;

			if (FMath::IsNearlyZero(DeltaTime))
			{
				ISPCKey.Value = Curve.Keys.Last().Value;
			}
			else
			{
				const double DeltaValue = Curve.Keys.Last(1).Value - Curve.Keys.Last().Value;
				const double Slope = DeltaValue / DeltaTime;

				ISPCKey.Value = Slope * (MaxTime - double(Curve.Keys.Last().Time)) + Curve.Keys.Last().Value;
			}

			ISPCKey.ArriveTangent = 0;
			ISPCKey.LeaveTangent = 0;
			ISPCKey.InterpMode = ispc::RCIM_Linear;
			Data->Keys.Add(ISPCKey);
		}
		else if (Curve.PostInfinityExtrap == RCCE_Constant)
		{
			ispc::FRichCurveKey ISPCKey;
			ISPCKey.Time = MaxTime;
			ISPCKey.Value = Curve.Keys.Last().Value;
			ISPCKey.ArriveTangent = 0;
			ISPCKey.LeaveTangent = 0;
			ISPCKey.InterpMode = ispc::RCIM_Constant;
			Data->Keys.Add(ISPCKey);
		}
		else
		{
			Data->Error += "Unsupported PostInfinityExtrap: " + UEnum::GetValueAsString(Curve.PostInfinityExtrap) + " - please disable Fast Curve\n";
		}
	}

	Data->Error.RemoveFromEnd("\n");

	{
		VOXEL_SCOPE_LOCK(CriticalSection);
		Data_RequiresLock = Data;
	}

	Dependency->Invalidate();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void FVoxelCurveRefPinType::Convert(
	const bool bSetObject,
	TWeakObjectPtr<UCurveFloat>& Object,
	FVoxelCurveRef& Struct) const
{
	if (bSetObject)
	{
		Object = Struct.Object;
	}
	else
	{
		Struct.Object = Object;
		Struct.Curve.Wrapper = GVoxelCurveManager->GetWrapper_GameThread(*Object);
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelCurve UVoxelCurveFunctionLibrary::MakeCurveFromAsset(const FVoxelCurveRef& Asset) const
{
	if (!Asset.Curve.Wrapper)
	{
		VOXEL_MESSAGE(Error, "{0}: Curve is null", this);
		return {};
	}

	return Asset.Curve;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

FVoxelFloatBuffer UVoxelCurveFunctionLibrary::SampleCurve(
	const FVoxelFloatBuffer& Value,
	const FVoxelCurve& Curve,
	const bool bFastCurve) const
{
	if (!Curve.Wrapper)
	{
		VOXEL_MESSAGE(Error, "{0}: Curve is null", this);
		return 0.f;
	}

	TSharedPtr<const FVoxelCurveData> Data;
	{
		VOXEL_SCOPE_LOCK(Curve.Wrapper->CriticalSection);
		Data = Curve.Wrapper->Data_RequiresLock;
	}

	GetQuery().GetDependencyTracker().AddDependency(Curve.Wrapper->Dependency);

	if (!bFastCurve)
	{
		FVoxelFloatBufferStorage ReturnValue;
		ReturnValue.Allocate(Value.Num());

		for (int32 Index = 0; Index < Value.Num(); Index++)
		{
			ReturnValue[Index] = Data->Curve->Eval(Value[Index]);
		}

		return FVoxelFloatBuffer::Make(ReturnValue);
	}

	if (!Data->Error.IsEmpty())
	{
		VOXEL_MESSAGE(Error, "{0}: {1}", this, Data->Error);
		return 0.f;
	}
	if (Data->Keys.Num() == 0)
	{
		return Data->DefaultValue;
	}
	if (Data->Keys.Num() == 1)
	{
		return Data->Keys[0].Value;
	}

	FVoxelFloatBufferStorage ReturnValue;
	ReturnValue.Allocate(Value.Num());

	ForeachVoxelBufferChunk(Value.Num(), [&](const FVoxelBufferIterator& Iterator)
	{
		ispc::VoxelCurveFunctionLibrary_SampleCurve(
			Data->Keys.GetData(),
			Data->Keys.Num(),
			Value.GetData(Iterator),
			Iterator.Num(),
			ReturnValue.GetData(Iterator));
	});

	return FVoxelFloatBuffer::Make(ReturnValue);
}