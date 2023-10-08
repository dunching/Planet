// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelISPCNode.h"
#include "VoxelTemplateNode.h"
#include "Buffer/VoxelFloatBuffers.h"
#include "Buffer/VoxelDoubleBuffers.h"
#include "VoxelVectorNodes.generated.h"

USTRUCT(meta = (Abstract))
struct VOXELGRAPHNODES_API FVoxelTemplateNode_AbstractVectorBase : public FVoxelTemplateNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_OUTPUT_PIN(FVoxelWildcard, ReturnValue);

public:
	virtual FPin* ExpandPins(FNode& Node, TArray<FPin*> Pins, const TArray<FPin*>& AllPins) const override;

#if WITH_EDITOR
	virtual FVoxelPinTypeSet GetPromotionTypes(const FVoxelPin& Pin) const override;
	virtual void PromotePin(FVoxelPin& InPin, const FVoxelPinType& NewType) override;
#endif

	virtual bool HasScalarOutput2D() const VOXEL_PURE_VIRTUAL({});
	virtual bool HasScalarOutput3D() const VOXEL_PURE_VIRTUAL({});

	virtual UScriptStruct* GetVector2DInnerNode() const VOXEL_PURE_VIRTUAL({});
	virtual UScriptStruct* GetDoubleVector2DInnerNode() const VOXEL_PURE_VIRTUAL({});

	virtual UScriptStruct* GetVector3DInnerNode() const VOXEL_PURE_VIRTUAL({});
	virtual UScriptStruct* GetDoubleVector3DInnerNode() const VOXEL_PURE_VIRTUAL({});
};

USTRUCT(meta = (Abstract))
struct VOXELGRAPHNODES_API FVoxelTemplateNode_VectorUnaryOperator : public FVoxelTemplateNode_AbstractVectorBase
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVoxelWildcard, Vector, nullptr);
};

USTRUCT(meta = (Abstract))
struct VOXELGRAPHNODES_API FVoxelTemplateNode_VectorBinaryOperator : public FVoxelTemplateNode_AbstractVectorBase
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVoxelWildcard, V1, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(FVoxelWildcard, V2, nullptr);
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(meta = (Internal))
struct FVoxelNode_CrossProduct_2D_Float : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVector2D, V1, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(FVector2D, V2, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(float, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {V1}.x * {V2}.y - {V1}.y * {V2}.x";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_CrossProduct_2D_Double : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVoxelDoubleVector2D, V1, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(FVoxelDoubleVector2D, V2, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(double, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {V1}.x * {V2}.y - {V1}.y * {V2}.x";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_CrossProduct_3D_Float : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVector, V1, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(FVector, V2, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(FVector, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = cross({V1}, {V2})";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_CrossProduct_3D_Double : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVoxelDoubleVector, V1, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(FVoxelDoubleVector, V2, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(FVoxelDoubleVector, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = cross({V1}, {V2})";
	}
};

USTRUCT(Category = "Math|Vector Operators")
struct VOXELGRAPHNODES_API FVoxelTemplateNode_CrossProduct : public FVoxelTemplateNode_VectorBinaryOperator
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	virtual bool HasScalarOutput2D() const override
	{
		return true;
	}
	virtual bool HasScalarOutput3D() const override
	{
		return false;
	}

	virtual UScriptStruct* GetVector2DInnerNode() const override
	{
		return FVoxelNode_CrossProduct_2D_Float::StaticStruct();
	}
	virtual UScriptStruct* GetDoubleVector2DInnerNode() const override
	{
		return FVoxelNode_CrossProduct_2D_Double::StaticStruct();
	}

	virtual UScriptStruct* GetVector3DInnerNode() const override
	{
		return FVoxelNode_CrossProduct_3D_Float::StaticStruct();
	}
	virtual UScriptStruct* GetDoubleVector3DInnerNode() const override
	{
		return FVoxelNode_CrossProduct_3D_Double::StaticStruct();
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(meta = (Internal))
struct FVoxelNode_DotProduct_2D_Float : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVector2D, V1, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(FVector2D, V2, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(float, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {V1}.x * {V2}.x + {V1}.y * {V2}.y";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_DotProduct_2D_Double : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVoxelDoubleVector2D, V1, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(FVoxelDoubleVector2D, V2, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(double, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {V1}.x * {V2}.x + {V1}.y * {V2}.y";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_DotProduct_3D_Float : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVector, V1, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(FVector, V2, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(float, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {V1}.x * {V2}.x + {V1}.y * {V2}.y + {V1}.z * {V2}.z";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_DotProduct_3D_Double : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVoxelDoubleVector, V1, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(FVoxelDoubleVector, V2, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(double, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = {V1}.x * {V2}.x + {V1}.y * {V2}.y + {V1}.z * {V2}.z";
	}
};

USTRUCT(Category = "Math|Vector Operators")
struct VOXELGRAPHNODES_API FVoxelTemplateNode_DotProduct : public FVoxelTemplateNode_VectorBinaryOperator
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	virtual bool HasScalarOutput2D() const override
	{
		return true;
	}
	virtual bool HasScalarOutput3D() const override
	{
		return true;
	}

	virtual UScriptStruct* GetVector2DInnerNode() const override
	{
		return FVoxelNode_DotProduct_2D_Float::StaticStruct();
	}
	virtual UScriptStruct* GetDoubleVector2DInnerNode() const override
	{
		return FVoxelNode_DotProduct_2D_Double::StaticStruct();
	}

	virtual UScriptStruct* GetVector3DInnerNode() const override
	{
		return FVoxelNode_DotProduct_3D_Float::StaticStruct();
	}
	virtual UScriptStruct* GetDoubleVector3DInnerNode() const override
	{
		return FVoxelNode_DotProduct_3D_Double::StaticStruct();
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(meta = (Internal))
struct FVoxelNode_Normalize_2D_Float : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVector2D, Vector, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(FVector2D, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = normalize({Vector})";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_Normalize_2D_Double : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVoxelDoubleVector2D, Vector, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(FVoxelDoubleVector2D, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = normalize({Vector})";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_Normalize_3D_Float : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVector, Vector, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(FVector, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = normalize({Vector})";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_Normalize_3D_Double : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVoxelDoubleVector, Vector, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(FVoxelDoubleVector, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = normalize({Vector})";
	}
};

USTRUCT(Category = "Math|Vector Operators")
struct VOXELGRAPHNODES_API FVoxelTemplateNode_Normalize : public FVoxelTemplateNode_VectorUnaryOperator
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	virtual bool HasScalarOutput2D() const override
	{
		return false;
	}
	virtual bool HasScalarOutput3D() const override
	{
		return false;
	}

	virtual UScriptStruct* GetVector2DInnerNode() const override
	{
		return FVoxelNode_Normalize_2D_Float::StaticStruct();
	}
	virtual UScriptStruct* GetDoubleVector2DInnerNode() const override
	{
		return FVoxelNode_Normalize_2D_Double::StaticStruct();
	}

	virtual UScriptStruct* GetVector3DInnerNode() const override
	{
		return FVoxelNode_Normalize_3D_Float::StaticStruct();
	}
	virtual UScriptStruct* GetDoubleVector3DInnerNode() const override
	{
		return FVoxelNode_Normalize_3D_Double::StaticStruct();
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(meta = (Internal))
struct FVoxelNode_Length_2D_Float : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVector2D, Vector, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(float, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = length({Vector})";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_Length_2D_Double : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVoxelDoubleVector2D, Vector, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(double, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = length({Vector})";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_Length_3D_Float : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVector, Vector, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(float, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = length({Vector})";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_Length_3D_Double : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVoxelDoubleVector, Vector, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(double, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = length({Vector})";
	}
};

USTRUCT(Category = "Math|Vector Operators")
struct VOXELGRAPHNODES_API FVoxelTemplateNode_Length : public FVoxelTemplateNode_VectorUnaryOperator
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	virtual bool HasScalarOutput2D() const override
	{
		return true;
	}
	virtual bool HasScalarOutput3D() const override
	{
		return true;
	}

	virtual UScriptStruct* GetVector2DInnerNode() const override
	{
		return FVoxelNode_Length_2D_Float::StaticStruct();
	}
	virtual UScriptStruct* GetDoubleVector2DInnerNode() const override
	{
		return FVoxelNode_Length_2D_Double::StaticStruct();
	}

	virtual UScriptStruct* GetVector3DInnerNode() const override
	{
		return FVoxelNode_Length_3D_Float::StaticStruct();
	}
	virtual UScriptStruct* GetDoubleVector3DInnerNode() const override
	{
		return FVoxelNode_Length_3D_Double::StaticStruct();
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(meta = (Internal))
struct FVoxelNode_LengthXY_3D_Float : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVector, Vector, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(float, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = length(MakeFloat2({Vector}.x, {Vector}.y))";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_LengthXY_3D_Double : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVoxelDoubleVector, Vector, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(double, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = length(MakeFloat2({Vector}.x, {Vector}.y))";
	}
};

USTRUCT(Category = "Math|Vector Operators")
struct VOXELGRAPHNODES_API FVoxelTemplateNode_LengthXY : public FVoxelTemplateNode_VectorUnaryOperator
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	virtual bool HasScalarOutput2D() const override
	{
		return true;
	}
	virtual bool HasScalarOutput3D() const override
	{
		return true;
	}

	virtual UScriptStruct* GetVector2DInnerNode() const override
	{
		return FVoxelNode_Length_2D_Float::StaticStruct();
	}
	virtual UScriptStruct* GetDoubleVector2DInnerNode() const override
	{
		return FVoxelNode_Length_2D_Double::StaticStruct();
	}

	virtual UScriptStruct* GetVector3DInnerNode() const override
	{
		return FVoxelNode_LengthXY_3D_Float::StaticStruct();
	}
	virtual UScriptStruct* GetDoubleVector3DInnerNode() const override
	{
		return FVoxelNode_LengthXY_3D_Double::StaticStruct();
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(meta = (Internal))
struct FVoxelNode_Distance_2D_Float : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVector2D, V1, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(FVector2D, V2, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(float, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = distance({V1}, {V2})";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_Distance_2D_Double : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVoxelDoubleVector2D, V1, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(FVoxelDoubleVector2D, V2, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(double, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = distance({V1}, {V2})";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_Distance_3D_Float : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVector, V1, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(FVector, V2, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(float, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = distance({V1}, {V2})";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_Distance_3D_Double : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVoxelDoubleVector, V1, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(FVoxelDoubleVector, V2, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(double, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = distance({V1}, {V2})";
	}
};

USTRUCT(Category = "Math|Vector Operators")
struct VOXELGRAPHNODES_API FVoxelTemplateNode_Distance : public FVoxelTemplateNode_VectorBinaryOperator
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	virtual bool HasScalarOutput2D() const override
	{
		return true;
	}
	virtual bool HasScalarOutput3D() const override
	{
		return true;
	}

	virtual UScriptStruct* GetVector2DInnerNode() const override
	{
		return FVoxelNode_Distance_2D_Float::StaticStruct();
	}
	virtual UScriptStruct* GetDoubleVector2DInnerNode() const override
	{
		return FVoxelNode_Distance_2D_Double::StaticStruct();
	}

	virtual UScriptStruct* GetVector3DInnerNode() const override
	{
		return FVoxelNode_Distance_3D_Float::StaticStruct();
	}
	virtual UScriptStruct* GetDoubleVector3DInnerNode() const override
	{
		return FVoxelNode_Distance_3D_Double::StaticStruct();
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(meta = (Internal))
struct FVoxelNode_Distance2D_3D_Float : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVector, V1, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(FVector, V2, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(float, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = distance(MakeFloat2({V1}.x, {V1}.y), MakeFloat2({V2}.x, {V2}.y))";
	}
};

USTRUCT(meta = (Internal))
struct FVoxelNode_Distance2D_3D_Double : public FVoxelISPCNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_TEMPLATE_INPUT_PIN(FVoxelDoubleVector, V1, nullptr);
	VOXEL_TEMPLATE_INPUT_PIN(FVoxelDoubleVector, V2, nullptr);
	VOXEL_TEMPLATE_OUTPUT_PIN(double, ReturnValue);

	virtual FString GenerateCode(FCode& Code) const override
	{
		return "{ReturnValue} = distance(MakeFloat2({V1}.x, {V1}.y), MakeFloat2({V2}.x, {V2}.y))";
	}
};

USTRUCT(Category = "Math|Vector Operators")
struct VOXELGRAPHNODES_API FVoxelTemplateNode_Distance2D : public FVoxelTemplateNode_VectorBinaryOperator
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	virtual bool HasScalarOutput2D() const override
	{
		return true;
	}
	virtual bool HasScalarOutput3D() const override
	{
		return true;
	}

	virtual UScriptStruct* GetVector2DInnerNode() const override
	{
		return FVoxelNode_Distance_2D_Float::StaticStruct();
	}
	virtual UScriptStruct* GetDoubleVector2DInnerNode() const override
	{
		return FVoxelNode_Distance_2D_Double::StaticStruct();
	}

	virtual UScriptStruct* GetVector3DInnerNode() const override
	{
		return FVoxelNode_Distance2D_3D_Float::StaticStruct();
	}
	virtual UScriptStruct* GetDoubleVector3DInnerNode() const override
	{
		return FVoxelNode_Distance2D_3D_Double::StaticStruct();
	}
};