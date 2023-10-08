
#include "CombustionEngine.h"

 #include "Curves/CurveFloat.h"
 
 UCombustionEngine::UCombustionEngine(const FObjectInitializer& ObjectInitializer)
 	:Super(ObjectInitializer)
 {
	 PrimaryComponentTick.bCanEverTick = true;
 }
 
 void UCombustionEngine::TickComponent(
 	float DeltaTime,
 	ELevelTick TickType,
 	FActorComponentTickFunction* ThisTickFunction
 )
 {
 	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    UpdateThrottle(DeltaTime);
    UpdateEngineTorque();
 }
 
 void UCombustionEngine::UpdateThrottle(float DeltaTime)
 {
 	float Value = 0;
 	if (FMath::IsNearlyZero(InputThrottle))
 	{
 		Value = -2.0;
 	}
 	else
 	{
 		Value = 1.0;
 	}
 
 	EngineThrottle = FMath::Clamp(EngineThrottle + (DeltaTime * Value), 0.f, 1.f);
 }
 
 bool UCombustionEngine::SetThrottle(float Throttle)
 {
 	InputThrottle = Throttle;
 	return false;
 }
 
 float UCombustionEngine::GetThrottle()
 {
 	return EngineThrottle;
 }
 
 void UCombustionEngine::UpdateEngineTorque()
 {
 	EngineTorque = EngineTorqueCurve->GetFloatValue(0) * EngineThrottle * 100.f;
 }
 
 float UCombustionEngine::GetEngineTorque()
 {
 	return EngineTorque;
 }

