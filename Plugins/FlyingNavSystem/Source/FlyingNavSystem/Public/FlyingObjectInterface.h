// Copyright Ben Sutherland 2024. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FlyingNavSystemModule.h"
#include "SVOQuerySettings.h"
#include "AI/Navigation/NavQueryFilter.h"

#include "FlyingObjectInterface.generated.h"

UINTERFACE(BlueprintType)
class FLYINGNAVSYSTEM_API UFlyingObjectInterface : public UInterface
{
	GENERATED_BODY()
};

class FLYINGNAVSYSTEM_API IFlyingObjectInterface
{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Pathfinding")
    FSVOQuerySettings GetPathfindingQuerySettings() const;

	virtual FSVOQuerySettings GetPathfindingQuerySettings_Implementation() const = 0;
};

// Dummy class to make sure I haven't missed anything important. Seems to be mostly recast and detour specific
class FLYINGNAVSYSTEM_API FFlyingQueryFilter : public INavigationQueryFilterInterface
{
public:
	FFlyingQueryFilter() {}
	virtual ~FFlyingQueryFilter() override {}

	virtual void Reset() override {}

	// I want to know if these functions get called
	virtual void SetAreaCost(uint8 AreaType, float Cost) override {						printw("QueryFilter for FlyingNavSystem not supported"); }
	virtual void SetFixedAreaEnteringCost(uint8 AreaType, float Cost) override {		printw("QueryFilter for FlyingNavSystem not supported"); }
	virtual void SetExcludedArea(uint8 AreaType) override {								printw("QueryFilter for FlyingNavSystem not supported"); }
	virtual void SetAllAreaCosts(const float* CostArray, const int32 Count) override{	printw("QueryFilter for FlyingNavSystem not supported"); }
	virtual void GetAllAreaCosts(float* CostArray, float* FixedCostArray, const int32 Count) const override { printw("QueryFilter for FlyingNavSystem not supported"); }
	virtual void SetBacktrackingEnabled(const bool bBacktracking) override {			printw("QueryFilter for FlyingNavSystem not supported"); }
	virtual bool IsBacktrackingEnabled() const override {								printw("QueryFilter for FlyingNavSystem not supported"); return false; }
	virtual float GetHeuristicScale() const override {									printw("QueryFilter for FlyingNavSystem not supported"); return 1.f; }
	virtual bool IsEqual(const INavigationQueryFilterInterface* Other) const override { printw("QueryFilter for FlyingNavSystem not supported"); return true; }
	virtual void SetIncludeFlags(uint16 Flags) override {								printw("QueryFilter for FlyingNavSystem not supported"); }
	virtual uint16 GetIncludeFlags() const override {									printw("QueryFilter for FlyingNavSystem not supported"); return 0; }
	virtual void SetExcludeFlags(uint16 Flags) override {								printw("QueryFilter for FlyingNavSystem not supported"); }
	virtual uint16 GetExcludeFlags() const override {									printw("QueryFilter for FlyingNavSystem not supported"); return 0; }
	virtual FVector GetAdjustedEndLocation(const FVector& EndLocation) const override { printw("QueryFilter for FlyingNavSystem not supported"); return EndLocation; }
	virtual INavigationQueryFilterInterface* CreateCopy() const override {				printw("QueryFilter for FlyingNavSystem not supported"); return new FFlyingQueryFilter();}
};
