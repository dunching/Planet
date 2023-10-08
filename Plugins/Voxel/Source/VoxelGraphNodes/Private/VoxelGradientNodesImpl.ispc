// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelMinimal.isph"

// We need data to be aligned by 8x
// For gradient, we need to query 6 values per input: -X +X -Y +Y -Z +Z
// We stagger 4 inputs together, giving us a 24 elements block
// -X0 +X0 -X1 +X1 -X2 +X2 -X3 +X3 -Y0 +Y0 -Y1 +Y1 -Y2 +Y2 -Y3 +Y3 -Z0 +Z0 -Z1 +Z1 -Z2 +Z2 -Z3 +Z3

export void VoxelNode_GetGradient_Sparse_SplitPositions(
	const uniform float PositionX[],
	const uniform float PositionY[],
	const uniform float PositionZ[],
	const uniform int32 Num,
	const uniform float HalfStep,
	uniform float OutPositionX[],
	uniform float OutPositionY[],
	uniform float OutPositionZ[])
{
	check(Num % 4 == 0);

#if TARGET_WIDTH == 8
	const varying float HalfStepOffset = MakeRegister(
		-HalfStep,
		+HalfStep,
		-HalfStep,
		+HalfStep,
		-HalfStep,
		+HalfStep,
		-HalfStep,
		+HalfStep);

	const uniform int32 NumBlocks = Num / 8;
	for (uniform int32 BlockIndex = 0; BlockIndex < NumBlocks; BlockIndex++)
	{
		const varying float BlockX = PositionX[8 * BlockIndex + programIndex];
		const varying float BlockY = PositionY[8 * BlockIndex + programIndex];
		const varying float BlockZ = PositionZ[8 * BlockIndex + programIndex];

		{
			const uniform int32 Index = 8 * BlockIndex + 0;

			const varying int32 Perm = MakeRegister(0, 0, 1, 1, 2, 2, 3, 3);

			const varying float X = shuffle(BlockX, Perm);
			const varying float Y = shuffle(BlockY, Perm);
			const varying float Z = shuffle(BlockZ, Perm);

			OutPositionX[6 * Index + 0 + programIndex] = X + HalfStepOffset;
			OutPositionX[6 * Index + 8 + programIndex] = X;
			OutPositionX[6 * Index + 16 + programIndex] = X;

			OutPositionY[6 * Index + 0 + programIndex] = Y;
			OutPositionY[6 * Index + 8 + programIndex] = Y + HalfStepOffset;
			OutPositionY[6 * Index + 16 + programIndex] = Y;

			OutPositionZ[6 * Index + 0 + programIndex] = Z;
			OutPositionZ[6 * Index + 8 + programIndex] = Z;
			OutPositionZ[6 * Index + 16 + programIndex] = Z + HalfStepOffset;
		}

		{
			const uniform int32 Index = 8 * BlockIndex + 4;

			const varying int32 Perm = MakeRegister(4, 4, 5, 5, 6, 6, 7, 7);

			const varying float X = shuffle(BlockX, Perm);
			const varying float Y = shuffle(BlockY, Perm);
			const varying float Z = shuffle(BlockZ, Perm);

			OutPositionX[6 * Index + 0 + programIndex] = X + HalfStepOffset;
			OutPositionX[6 * Index + 8 + programIndex] = X;
			OutPositionX[6 * Index + 16 + programIndex] = X;

			OutPositionY[6 * Index + 0 + programIndex] = Y;
			OutPositionY[6 * Index + 8 + programIndex] = Y + HalfStepOffset;
			OutPositionY[6 * Index + 16 + programIndex] = Y;

			OutPositionZ[6 * Index + 0 + programIndex] = Z;
			OutPositionZ[6 * Index + 8 + programIndex] = Z;
			OutPositionZ[6 * Index + 16 + programIndex] = Z + HalfStepOffset;
		}
	}

	if (Num % 8 != 0)
	{
		const uniform int32 Index = 8 * NumBlocks;
		const varying int32 Perm = MakeRegister(0, 0, 1, 1, 2, 2, 3, 3);

		const varying float X = shuffle(PositionX[Index + programIndex], Perm);
		const varying float Y = shuffle(PositionY[Index + programIndex], Perm);
		const varying float Z = shuffle(PositionZ[Index + programIndex], Perm);

		OutPositionX[6 * Index + 0 + programIndex] = X + HalfStepOffset;
		OutPositionX[6 * Index + 8 + programIndex] = X;
		OutPositionX[6 * Index + 16 + programIndex] = X;

		OutPositionY[6 * Index + 0 + programIndex] = Y;
		OutPositionY[6 * Index + 8 + programIndex] = Y + HalfStepOffset;
		OutPositionY[6 * Index + 16 + programIndex] = Y;

		OutPositionZ[6 * Index + 0 + programIndex] = Z;
		OutPositionZ[6 * Index + 8 + programIndex] = Z;
		OutPositionZ[6 * Index + 16 + programIndex] = Z + HalfStepOffset;
	}
#elif TARGET_WIDTH == 4
	const varying float HalfStepOffset = MakeRegister(
		-HalfStep,
		+HalfStep,
		-HalfStep,
		+HalfStep);

	for (uniform int32 Index = 0; Index < Num; Index += 4)
	{
		const varying float X = PositionX[Index + programIndex];
		const varying float Y = PositionY[Index + programIndex];
		const varying float Z = PositionZ[Index + programIndex];

		const varying int32 Perm0 = MakeRegister(0, 0, 1, 1);
		const varying int32 Perm1 = MakeRegister(2, 2, 3, 3);

		const varying float X0 = shuffle(X, Perm0);
		const varying float X1 = shuffle(X, Perm1);
		const varying float Y0 = shuffle(Y, Perm0);
		const varying float Y1 = shuffle(Y, Perm1);
		const varying float Z0 = shuffle(Z, Perm0);
		const varying float Z1 = shuffle(Z, Perm1);

		OutPositionX[6 * Index + 0 + 0 + programIndex] = X0 + HalfStepOffset;
		OutPositionX[6 * Index + 0 + 4 + programIndex] = X1 + HalfStepOffset;
		OutPositionX[6 * Index + 8 + 0 + programIndex] = X0;
		OutPositionX[6 * Index + 8 + 4 + programIndex] = X1;
		OutPositionX[6 * Index + 16 + 0 + programIndex] = X0;
		OutPositionX[6 * Index + 16 + 4 + programIndex] = X1;

		OutPositionY[6 * Index + 0 + 0 + programIndex] = Y0;
		OutPositionY[6 * Index + 0 + 4 + programIndex] = Y1;
		OutPositionY[6 * Index + 8 + 0 + programIndex] = Y0 + HalfStepOffset;
		OutPositionY[6 * Index + 8 + 4 + programIndex] = Y1 + HalfStepOffset;
		OutPositionY[6 * Index + 16 + 0 + programIndex] = Y0;
		OutPositionY[6 * Index + 16 + 4 + programIndex] = Y1;

		OutPositionZ[6 * Index + 0 + 0 + programIndex] = Z0;
		OutPositionZ[6 * Index + 0 + 4 + programIndex] = Z1;
		OutPositionZ[6 * Index + 8 + 0 + programIndex] = Z0;
		OutPositionZ[6 * Index + 8 + 4 + programIndex] = Z1;
		OutPositionZ[6 * Index + 16 + 0 + programIndex] = Z0 + HalfStepOffset;
		OutPositionZ[6 * Index + 16 + 4 + programIndex] = Z1 + HalfStepOffset;
	}
#else
#error "Unsupported TARGET_WIDTH"
	unsupportedTargetWidth();
#endif
}

export void VoxelNode_GetGradient_Sparse(
	const uniform float Values[],
	const uniform int32 Num,
	const uniform float Step,
	uniform float OutX[],
	uniform float OutY[],
	uniform float OutZ[])
{
	check(Num % 24 == 0);

#if TARGET_WIDTH == 8
	for (uniform int32 BlockIndex = 0; BlockIndex < Num; BlockIndex += 24)
	{
		const varying int32 IndexX = BlockIndex + 0 + programIndex;
		const varying int32 IndexY = BlockIndex + 8 + programIndex;
		const varying int32 IndexZ = BlockIndex + 16 + programIndex;

		const varying float ValueX = Values[IndexX];
		const varying float ValueY = Values[IndexY];
		const varying float ValueZ = Values[IndexZ];

		const varying int32 PermA = MakeRegister(0, 0, 2, 2, 4, 4, 6, 6);
		const varying int32 PermB = MakeRegister(1, 1, 3, 3, 5, 5, 7, 7);

		const varying float DeltaX = (shuffle(ValueX, PermB) - shuffle(ValueX, PermA)) / Step;
		const varying float DeltaY = (shuffle(ValueY, PermB) - shuffle(ValueY, PermA)) / Step;
		const varying float DeltaZ = (shuffle(ValueZ, PermB) - shuffle(ValueZ, PermA)) / Step;

		OutX[IndexX] = DeltaX;
		OutX[IndexY] = DeltaX;
		OutX[IndexZ] = DeltaX;

		OutY[IndexX] = DeltaY;
		OutY[IndexY] = DeltaY;
		OutY[IndexZ] = DeltaY;

		OutZ[IndexX] = DeltaZ;
		OutZ[IndexY] = DeltaZ;
		OutZ[IndexZ] = DeltaZ;
	}
#elif TARGET_WIDTH == 4
	for (uniform int32 BlockIndex = 0; BlockIndex < Num; BlockIndex += 24)
	{
		const varying int32 IndexX0 = BlockIndex + 0 + 0 + programIndex;
		const varying int32 IndexX1 = BlockIndex + 0 + 4 + programIndex;
		const varying int32 IndexY0 = BlockIndex + 8 + 0 + programIndex;
		const varying int32 IndexY1 = BlockIndex + 8 + 4 + programIndex;
		const varying int32 IndexZ0 = BlockIndex + 16 + 0 + programIndex;
		const varying int32 IndexZ1 = BlockIndex + 16 + 4 + programIndex;

		const varying float ValueX0 = Values[IndexX0];
		const varying float ValueX1 = Values[IndexX1];
		const varying float ValueY0 = Values[IndexY0];
		const varying float ValueY1 = Values[IndexY1];
		const varying float ValueZ0 = Values[IndexZ0];
		const varying float ValueZ1 = Values[IndexZ1];

		const varying int32 PermA = MakeRegister(0, 0, 2, 2);
		const varying int32 PermB = MakeRegister(1, 1, 3, 3);

		const varying float DeltaX0 = (shuffle(ValueX0, PermB) - shuffle(ValueX0, PermA)) / Step;
		const varying float DeltaX1 = (shuffle(ValueX1, PermB) - shuffle(ValueX1, PermA)) / Step;
		const varying float DeltaY0 = (shuffle(ValueY0, PermB) - shuffle(ValueY0, PermA)) / Step;
		const varying float DeltaY1 = (shuffle(ValueY1, PermB) - shuffle(ValueY1, PermA)) / Step;
		const varying float DeltaZ0 = (shuffle(ValueZ0, PermB) - shuffle(ValueZ0, PermA)) / Step;
		const varying float DeltaZ1 = (shuffle(ValueZ1, PermB) - shuffle(ValueZ1, PermA)) / Step;

		OutX[IndexX0] = DeltaX0;
		OutX[IndexX1] = DeltaX1;
		OutX[IndexY0] = DeltaX0;
		OutX[IndexY1] = DeltaX1;
		OutX[IndexZ0] = DeltaX0;
		OutX[IndexZ1] = DeltaX1;

		OutY[IndexX0] = DeltaY0;
		OutY[IndexX1] = DeltaY1;
		OutY[IndexY0] = DeltaY0;
		OutY[IndexY1] = DeltaY1;
		OutY[IndexZ0] = DeltaY0;
		OutY[IndexZ1] = DeltaY1;

		OutZ[IndexX0] = DeltaZ0;
		OutZ[IndexX1] = DeltaZ1;
		OutZ[IndexY0] = DeltaZ0;
		OutZ[IndexY1] = DeltaZ1;
		OutZ[IndexZ0] = DeltaZ0;
		OutZ[IndexZ1] = DeltaZ1;
	}
#else
#error "Unsupported TARGET_WIDTH"
	unsupportedTargetWidth();
#endif
}

export void VoxelNode_GetGradient_Sparse_Collapse(
	const uniform float Values[],
	const uniform int32 Num,
	const uniform float Step,
	uniform float OutX[],
	uniform float OutY[],
	uniform float OutZ[])
{
	check(Num % 24 == 0);

#if TARGET_WIDTH == 8
	for (uniform int32 Index = 0; Index < Num / 24; Index++)
	{
		const uniform int32 BlockIndex = 24 * Index;

		const varying int32 IndexX = BlockIndex + 0 + programIndex;
		const varying int32 IndexY = BlockIndex + 8 + programIndex;
		const varying int32 IndexZ = BlockIndex + 16 + programIndex;

		const varying float ValueX = Values[IndexX];
		const varying float ValueY = Values[IndexY];
		const varying float ValueZ = Values[IndexZ];

		const varying int32 PermA = MakeRegister(0, 2, 4, 6, 0, 0, 0, 0);
		const varying int32 PermB = MakeRegister(1, 3, 5, 7, 0, 0, 0, 0);

		const varying float DeltaX = (shuffle(ValueX, PermB) - shuffle(ValueX, PermA)) / Step;
		const varying float DeltaY = (shuffle(ValueY, PermB) - shuffle(ValueY, PermA)) / Step;
		const varying float DeltaZ = (shuffle(ValueZ, PermB) - shuffle(ValueZ, PermA)) / Step;

		OutX[4 * Index + 0] = extract(DeltaX, 0);
		OutX[4 * Index + 1] = extract(DeltaX, 1);
		OutX[4 * Index + 2] = extract(DeltaX, 2);
		OutX[4 * Index + 3] = extract(DeltaX, 3);

		OutY[4 * Index + 0] = extract(DeltaY, 0);
		OutY[4 * Index + 1] = extract(DeltaY, 1);
		OutY[4 * Index + 2] = extract(DeltaY, 2);
		OutY[4 * Index + 3] = extract(DeltaY, 3);

		OutZ[4 * Index + 0] = extract(DeltaZ, 0);
		OutZ[4 * Index + 1] = extract(DeltaZ, 1);
		OutZ[4 * Index + 2] = extract(DeltaZ, 2);
		OutZ[4 * Index + 3] = extract(DeltaZ, 3);
	}
#elif TARGET_WIDTH == 4
	for (uniform int32 Index = 0; Index < Num / 24; Index++)
	{
		const uniform int32 BlockIndex = 24 * Index;

		const varying int32 IndexX0 = BlockIndex + 0 + 0 + programIndex;
		const varying int32 IndexX1 = BlockIndex + 0 + 4 + programIndex;
		const varying int32 IndexY0 = BlockIndex + 8 + 0 + programIndex;
		const varying int32 IndexY1 = BlockIndex + 8 + 4 + programIndex;
		const varying int32 IndexZ0 = BlockIndex + 16 + 0 + programIndex;
		const varying int32 IndexZ1 = BlockIndex + 16 + 4 + programIndex;

		const varying float ValueX0 = Values[IndexX0];
		const varying float ValueX1 = Values[IndexX1];
		const varying float ValueY0 = Values[IndexY0];
		const varying float ValueY1 = Values[IndexY1];
		const varying float ValueZ0 = Values[IndexZ0];
		const varying float ValueZ1 = Values[IndexZ1];

		const varying int32 PermA = MakeRegister(0, 2, 0, 0);
		const varying int32 PermB = MakeRegister(1, 3, 0, 0);

		const varying float DeltaX0 = (shuffle(ValueX0, PermB) - shuffle(ValueX0, PermA)) / Step;
		const varying float DeltaX1 = (shuffle(ValueX1, PermB) - shuffle(ValueX1, PermA)) / Step;
		const varying float DeltaY0 = (shuffle(ValueY0, PermB) - shuffle(ValueY0, PermA)) / Step;
		const varying float DeltaY1 = (shuffle(ValueY1, PermB) - shuffle(ValueY1, PermA)) / Step;
		const varying float DeltaZ0 = (shuffle(ValueZ0, PermB) - shuffle(ValueZ0, PermA)) / Step;
		const varying float DeltaZ1 = (shuffle(ValueZ1, PermB) - shuffle(ValueZ1, PermA)) / Step;

		OutX[4 * Index + 0] = extract(DeltaX0, 0);
		OutX[4 * Index + 1] = extract(DeltaX0, 1);
		OutX[4 * Index + 2] = extract(DeltaX1, 0);
		OutX[4 * Index + 3] = extract(DeltaX1, 1);

		OutY[4 * Index + 0] = extract(DeltaY0, 0);
		OutY[4 * Index + 1] = extract(DeltaY0, 1);
		OutY[4 * Index + 2] = extract(DeltaY1, 0);
		OutY[4 * Index + 3] = extract(DeltaY1, 1);

		OutZ[4 * Index + 0] = extract(DeltaZ0, 0);
		OutZ[4 * Index + 1] = extract(DeltaZ0, 1);
		OutZ[4 * Index + 2] = extract(DeltaZ1, 0);
		OutZ[4 * Index + 3] = extract(DeltaZ1, 1);
	}
#else
#error "Unsupported TARGET_WIDTH"
	unsupportedTargetWidth();
#endif
}