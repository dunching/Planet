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

const ECollisionChannel PawnECC = ECollisionChannel::ECC_Pawn;
const ECollisionChannel Equipment = ECollisionChannel::ECC_GameTraceChannel2;
const ECollisionChannel PickableItemECC = ECollisionChannel::ECC_GameTraceChannel3;	
const ECollisionChannel Track = ECollisionChannel::ECC_GameTraceChannel4;
const ECollisionChannel TrackVehicle = ECollisionChannel::ECC_GameTraceChannel6;
const ECollisionChannel Projectile = ECollisionChannel::ECC_GameTraceChannel7;
const ECollisionChannel Building = ECollisionChannel::ECC_GameTraceChannel9;
const ECollisionChannel VoxelWorld = ECollisionChannel::ECC_GameTraceChannel10;
const ECollisionChannel CapturePoint = ECollisionChannel::ECC_GameTraceChannel1;
