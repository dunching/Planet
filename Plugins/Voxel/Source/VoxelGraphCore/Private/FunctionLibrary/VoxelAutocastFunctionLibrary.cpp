// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "FunctionLibrary/VoxelAutocastFunctionLibrary.h"
#include "VoxelGraphMigration.h"

VOXEL_RUN_ON_STARTUP_GAME(VoxelAutocastFunctionLibraryMigrations)
{
	REGISTER_VOXEL_FUNCTION_MIGRATION("To Vector (Vector2D)", UVoxelAutocastFunctionLibrary, Vector2DToVector);
	REGISTER_VOXEL_FUNCTION_MIGRATION("To Vector2D (Vector)", UVoxelAutocastFunctionLibrary, VectorToVector2D);
	REGISTER_VOXEL_FUNCTION_MIGRATION("To Color (Float)", UVoxelAutocastFunctionLibrary, FloatToLinearColor);
}