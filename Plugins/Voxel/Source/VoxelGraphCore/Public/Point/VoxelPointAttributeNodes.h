// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelMinimal.h"
#include "VoxelTemplateNode.h"
#include "Point/VoxelPointSet.h"
#include "Buffer/VoxelClassBuffer.h"
#include "Buffer/VoxelStaticMeshBuffer.h"
#include "VoxelPointAttributeNodes.generated.h"

USTRUCT(Category = "Point")
struct VOXELGRAPHCORE_API FVoxelNode_SetPointAttribute : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelPointSet, In, nullptr);
	VOXEL_INPUT_PIN(FName, Name, "MyAttribute");
	VOXEL_INPUT_PIN(FVoxelWildcardBuffer, Value, nullptr);
	VOXEL_OUTPUT_PIN(FVoxelPointSet, Out);

	virtual FName GetAttributeName(const FName Name) const
	{
		return Name;
	}

#if WITH_EDITOR
	virtual FVoxelPinTypeSet GetPromotionTypes(const FVoxelPin& Pin) const override;
#endif
};

USTRUCT(Category = "Point")
struct VOXELGRAPHCORE_API FVoxelNode_GetPointAttribute : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FName, Name, "MyAttribute");
	VOXEL_OUTPUT_PIN(FVoxelWildcardBuffer, Value);

	virtual bool IsPureNode() const override
	{
		return true;
	}
	virtual FName GetAttributeName(const FName Name) const
	{
		return Name;
	}

#if WITH_EDITOR
	virtual FVoxelPinTypeSet GetPromotionTypes(const FVoxelPin& Pin) const override;
#endif
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// Set a property to be passed to the point actor, per instance
USTRUCT(Category = "Point")
struct VOXELGRAPHCORE_API FVoxelNode_SetActorProperty : public FVoxelNode_SetPointAttribute
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	virtual FName GetAttributeName(const FName Name) const override
	{
		return FVoxelPointAttributes::MakeActor(Name);
	}
};

USTRUCT(Category = "Point")
struct VOXELGRAPHCORE_API FVoxelNode_GetActorProperty : public FVoxelNode_GetPointAttribute
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	virtual FName GetAttributeName(const FName Name) const override
	{
		return FVoxelPointAttributes::MakeActor(Name);
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(Category = "Point")
struct VOXELGRAPHCORE_API FVoxelNode_SetParentPointAttribute : public FVoxelNode_SetPointAttribute
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	virtual FName GetAttributeName(const FName Name) const override
	{
		return FVoxelPointAttributes::MakeParent(Name);
	}
};

USTRUCT(Category = "Point")
struct VOXELGRAPHCORE_API FVoxelNode_GetParentPointAttribute : public FVoxelNode_GetPointAttribute
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	virtual FName GetAttributeName(const FName Name) const override
	{
		return FVoxelPointAttributes::MakeParent(Name);
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(Category = "Point", meta = (Abstract))
struct VOXELGRAPHCORE_API FVoxelTemplateNode_SetPointAttributeBase : public FVoxelTemplateNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelPointSet, In, nullptr);
	VOXEL_OUTPUT_PIN(FVoxelPointSet, Out);

	virtual bool IsPureNode() const override
	{
		return false;
	}

	virtual FPin* ExpandPins(FNode& Node, TArray<FPin*> Pins, const TArray<FPin*>& AllPins) const override;
	virtual FName GetAttributeName() const VOXEL_PURE_VIRTUAL({});
};

USTRUCT(Category = "Point", meta = (Abstract))
struct VOXELGRAPHCORE_API FVoxelTemplateNode_GetPointAttributeBase : public FVoxelTemplateNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	virtual FPin* ExpandPins(FNode& Node, TArray<FPin*> Pins, const TArray<FPin*>& AllPins) const override;
	virtual FName GetAttributeName() const VOXEL_PURE_VIRTUAL({});
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelTemplateNode_SetPointMesh : public FVoxelTemplateNode_SetPointAttributeBase
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelStaticMeshBuffer, Mesh, nullptr);

	virtual FName GetAttributeName() const override
	{
		return FVoxelPointAttributes::Mesh;
	}
};

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelTemplateNode_GetPointMesh : public FVoxelTemplateNode_GetPointAttributeBase
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_OUTPUT_PIN(FVoxelStaticMeshBuffer, Mesh);

	virtual FName GetAttributeName() const override
	{
		return FVoxelPointAttributes::Mesh;
	}
};

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelTemplateNode_GetParentPointMesh : public FVoxelTemplateNode_GetPointAttributeBase
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_OUTPUT_PIN(FVoxelStaticMeshBuffer, Mesh);

	virtual FName GetAttributeName() const override
	{
		return FVoxelPointAttributes::MakeParent(FVoxelPointAttributes::Mesh);
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelTemplateNode_SetPointPosition : public FVoxelTemplateNode_SetPointAttributeBase
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelVectorBuffer, Position, nullptr);

	virtual FName GetAttributeName() const override
	{
		return FVoxelPointAttributes::Position;
	}
};

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelTemplateNode_GetPointPosition : public FVoxelTemplateNode_GetPointAttributeBase
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_OUTPUT_PIN(FVoxelVectorBuffer, Position);

	virtual FName GetAttributeName() const override
	{
		return FVoxelPointAttributes::Position;
	}
};

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelTemplateNode_GetParentPointPosition : public FVoxelTemplateNode_GetPointAttributeBase
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_OUTPUT_PIN(FVoxelVectorBuffer, Position);

	virtual FName GetAttributeName() const override
	{
		return FVoxelPointAttributes::MakeParent(FVoxelPointAttributes::Position);
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelTemplateNode_SetPointRotation : public FVoxelTemplateNode_SetPointAttributeBase
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelQuaternionBuffer, Rotation, nullptr);

	virtual FName GetAttributeName() const override
	{
		return FVoxelPointAttributes::Rotation;
	}
};

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelTemplateNode_GetPointRotation : public FVoxelTemplateNode_GetPointAttributeBase
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_OUTPUT_PIN(FVoxelQuaternionBuffer, Rotation);

	virtual FName GetAttributeName() const override
	{
		return FVoxelPointAttributes::Rotation;
	}
};

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelTemplateNode_GetParentPointRotation : public FVoxelTemplateNode_GetPointAttributeBase
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_OUTPUT_PIN(FVoxelQuaternionBuffer, Rotation);

	virtual FName GetAttributeName() const override
	{
		return FVoxelPointAttributes::MakeParent(FVoxelPointAttributes::Rotation);
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelTemplateNode_SetPointScale : public FVoxelTemplateNode_SetPointAttributeBase
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelVectorBuffer, Scale, FVector::OneVector);

	virtual FName GetAttributeName() const override
	{
		return FVoxelPointAttributes::Scale;
	}
};

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelTemplateNode_GetPointScale : public FVoxelTemplateNode_GetPointAttributeBase
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_OUTPUT_PIN(FVoxelVectorBuffer, Scale);

	virtual FName GetAttributeName() const override
	{
		return FVoxelPointAttributes::Scale;
	}
};

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelTemplateNode_GetParentPointScale : public FVoxelTemplateNode_GetPointAttributeBase
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_OUTPUT_PIN(FVoxelVectorBuffer, Scale);

	virtual FName GetAttributeName() const override
	{
		return FVoxelPointAttributes::MakeParent(FVoxelPointAttributes::Scale);
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelTemplateNode_SetPointNormal : public FVoxelTemplateNode_SetPointAttributeBase
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelVectorBuffer, Normal, FVector::UpVector);

	virtual FName GetAttributeName() const override
	{
		return FVoxelPointAttributes::Normal;
	}
};

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelTemplateNode_GetPointNormal : public FVoxelTemplateNode_GetPointAttributeBase
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_OUTPUT_PIN(FVoxelVectorBuffer, Normal);

	virtual FName GetAttributeName() const override
	{
		return FVoxelPointAttributes::Normal;
	}
};

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelTemplateNode_GetParentPointNormal : public FVoxelTemplateNode_GetPointAttributeBase
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_OUTPUT_PIN(FVoxelVectorBuffer, Normal);

	virtual FName GetAttributeName() const override
	{
		return FVoxelPointAttributes::MakeParent(FVoxelPointAttributes::Normal);
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelTemplateNode_SetActorClass : public FVoxelTemplateNode_SetPointAttributeBase
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelClassBuffer, Class, nullptr, BaseClass<AActor>);

	virtual FName GetAttributeName() const override
	{
		return FVoxelPointAttributes::ActorClass;
	}
};

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelTemplateNode_GetActorClass : public FVoxelTemplateNode_GetPointAttributeBase
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_OUTPUT_PIN(FVoxelClassBuffer, Class, BaseClass<AActor>);

	virtual FName GetAttributeName() const override
	{
		return FVoxelPointAttributes::ActorClass;
	}
};

USTRUCT()
struct VOXELGRAPHCORE_API FVoxelTemplateNode_GetParentActorClass : public FVoxelTemplateNode_GetPointAttributeBase
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_OUTPUT_PIN(FVoxelClassBuffer, Class, BaseClass<AActor>);

	virtual FName GetAttributeName() const override
	{
		return FVoxelPointAttributes::MakeParent(FVoxelPointAttributes::ActorClass);
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// Set a custom data to be passed to the material, per instance
USTRUCT(Category = "Point")
struct VOXELGRAPHCORE_API FVoxelNode_SetPointCustomData : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelPointSet, In, nullptr);
	VOXEL_INPUT_PIN(int32, Index, 0);
	VOXEL_INPUT_PIN(FVoxelFloatBuffer, Value, nullptr);
	VOXEL_OUTPUT_PIN(FVoxelPointSet, Out);
};

USTRUCT(Category = "Point")
struct VOXELGRAPHCORE_API FVoxelNode_GetPointCustomData : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(int32, Index, 0);
	VOXEL_OUTPUT_PIN(FVoxelFloatBuffer, Value);

	virtual bool IsPureNode() const override
	{
		return true;
	}
};

USTRUCT(Category = "Point")
struct VOXELGRAPHCORE_API FVoxelNode_GetParentPointCustomData : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(int32, Index, 0);
	VOXEL_OUTPUT_PIN(FVoxelFloatBuffer, Value);

	virtual bool IsPureNode() const override
	{
		return true;
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(Category = "Point")
struct VOXELGRAPHCORE_API FVoxelNode_GetPointSeed : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_OUTPUT_PIN(FVoxelSeedBuffer, Value);

	virtual bool IsPureNode() const override
	{
		return true;
	}
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

USTRUCT(Category = "Point")
struct VOXELGRAPHCORE_API FVoxelNode_ApplyTranslation : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelPointSet, In, nullptr);
	VOXEL_INPUT_PIN(FVoxelVectorBuffer, Translation, nullptr);
	VOXEL_OUTPUT_PIN(FVoxelPointSet, Out);
};

USTRUCT(Category = "Point")
struct VOXELGRAPHCORE_API FVoxelNode_ApplyRotation : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelPointSet, In, nullptr);
	VOXEL_INPUT_PIN(FVoxelQuaternionBuffer, Rotation, nullptr);
	VOXEL_OUTPUT_PIN(FVoxelPointSet, Out);
};

USTRUCT(Category = "Point")
struct VOXELGRAPHCORE_API FVoxelNode_ApplyScale : public FVoxelNode
{
	GENERATED_BODY()
	GENERATED_VOXEL_NODE_BODY()

	VOXEL_INPUT_PIN(FVoxelPointSet, In, nullptr);
	VOXEL_INPUT_PIN(FVoxelVectorBuffer, Scale, nullptr);
	VOXEL_OUTPUT_PIN(FVoxelPointSet, Out);
};