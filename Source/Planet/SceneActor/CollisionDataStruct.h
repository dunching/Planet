// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/EngineTypes.h"

const FName PickableItem = TEXT("PickableItem");
const FName EquipmentItem = TEXT("EquipmentItem");
const FName ProjecttileItem = TEXT("ProjecttileItem");
const FName TrackItem = TEXT("TrackItem");
const FName BuildingItem = TEXT("BuildingItem");
const FName VehicleItem = TEXT("Vehicle");

const ECollisionChannel Pawn_Object = ECollisionChannel::ECC_Pawn;
const ECollisionChannel CapturePoint_Object = ECollisionChannel::ECC_GameTraceChannel1;
const ECollisionChannel Equipment_Object = ECollisionChannel::ECC_GameTraceChannel2;
const ECollisionChannel Projectile_Object = ECollisionChannel::ECC_GameTraceChannel7;
const ECollisionChannel Building_Object = ECollisionChannel::ECC_GameTraceChannel9;
const ECollisionChannel VoxelWorld_Object = ECollisionChannel::ECC_GameTraceChannel10;
const ECollisionChannel ResouceBox_Object = ECollisionChannel::ECC_GameTraceChannel11;

const ECollisionChannel SceneActor_Channel = ECollisionChannel::ECC_GameTraceChannel6;
