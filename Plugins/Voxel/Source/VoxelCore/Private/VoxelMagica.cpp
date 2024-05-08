// Copyright Voxel Plugin, Inc. All Rights Reserved.

#include "VoxelMagica.h"

// This code is heavily inspired from https://github.com/jpaver/opengametools, whose license you can find below
/* -------------------------------------------------------------------------------------------------------------------------------------------------

    MIT License

    Copyright (c) 2019 Justin Paver

    Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
    IN THE SOFTWARE.

------------------------------------------------------------------------------------------------------------------------------------------------- */

namespace Voxel::Magica
{

// Some older .vox files will not store a palette, in which case the following palette will be used!
static const uint8_t DefaultPalette[256 * 4] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xcc, 0xff, 0xff, 0xff, 0x99, 0xff, 0xff, 0xff, 0x66, 0xff, 0xff, 0xff, 0x33, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xcc, 0xff, 0xff, 0xff, 0xcc, 0xcc, 0xff,
    0xff, 0xcc, 0x99, 0xff, 0xff, 0xcc, 0x66, 0xff, 0xff, 0xcc, 0x33, 0xff, 0xff, 0xcc, 0x00, 0xff, 0xff, 0x99, 0xff, 0xff, 0xff, 0x99, 0xcc, 0xff, 0xff, 0x99, 0x99, 0xff, 0xff, 0x99, 0x66, 0xff,
    0xff, 0x99, 0x33, 0xff, 0xff, 0x99, 0x00, 0xff, 0xff, 0x66, 0xff, 0xff, 0xff, 0x66, 0xcc, 0xff, 0xff, 0x66, 0x99, 0xff, 0xff, 0x66, 0x66, 0xff, 0xff, 0x66, 0x33, 0xff, 0xff, 0x66, 0x00, 0xff,
    0xff, 0x33, 0xff, 0xff, 0xff, 0x33, 0xcc, 0xff, 0xff, 0x33, 0x99, 0xff, 0xff, 0x33, 0x66, 0xff, 0xff, 0x33, 0x33, 0xff, 0xff, 0x33, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xcc, 0xff,
    0xff, 0x00, 0x99, 0xff, 0xff, 0x00, 0x66, 0xff, 0xff, 0x00, 0x33, 0xff, 0xff, 0x00, 0x00, 0xff, 0xcc, 0xff, 0xff, 0xff, 0xcc, 0xff, 0xcc, 0xff, 0xcc, 0xff, 0x99, 0xff, 0xcc, 0xff, 0x66, 0xff,
    0xcc, 0xff, 0x33, 0xff, 0xcc, 0xff, 0x00, 0xff, 0xcc, 0xcc, 0xff, 0xff, 0xcc, 0xcc, 0xcc, 0xff, 0xcc, 0xcc, 0x99, 0xff, 0xcc, 0xcc, 0x66, 0xff, 0xcc, 0xcc, 0x33, 0xff, 0xcc, 0xcc, 0x00, 0xff,
    0xcc, 0x99, 0xff, 0xff, 0xcc, 0x99, 0xcc, 0xff, 0xcc, 0x99, 0x99, 0xff, 0xcc, 0x99, 0x66, 0xff, 0xcc, 0x99, 0x33, 0xff, 0xcc, 0x99, 0x00, 0xff, 0xcc, 0x66, 0xff, 0xff, 0xcc, 0x66, 0xcc, 0xff,
    0xcc, 0x66, 0x99, 0xff, 0xcc, 0x66, 0x66, 0xff, 0xcc, 0x66, 0x33, 0xff, 0xcc, 0x66, 0x00, 0xff, 0xcc, 0x33, 0xff, 0xff, 0xcc, 0x33, 0xcc, 0xff, 0xcc, 0x33, 0x99, 0xff, 0xcc, 0x33, 0x66, 0xff,
    0xcc, 0x33, 0x33, 0xff, 0xcc, 0x33, 0x00, 0xff, 0xcc, 0x00, 0xff, 0xff, 0xcc, 0x00, 0xcc, 0xff, 0xcc, 0x00, 0x99, 0xff, 0xcc, 0x00, 0x66, 0xff, 0xcc, 0x00, 0x33, 0xff, 0xcc, 0x00, 0x00, 0xff,
    0x99, 0xff, 0xff, 0xff, 0x99, 0xff, 0xcc, 0xff, 0x99, 0xff, 0x99, 0xff, 0x99, 0xff, 0x66, 0xff, 0x99, 0xff, 0x33, 0xff, 0x99, 0xff, 0x00, 0xff, 0x99, 0xcc, 0xff, 0xff, 0x99, 0xcc, 0xcc, 0xff,
    0x99, 0xcc, 0x99, 0xff, 0x99, 0xcc, 0x66, 0xff, 0x99, 0xcc, 0x33, 0xff, 0x99, 0xcc, 0x00, 0xff, 0x99, 0x99, 0xff, 0xff, 0x99, 0x99, 0xcc, 0xff, 0x99, 0x99, 0x99, 0xff, 0x99, 0x99, 0x66, 0xff,
    0x99, 0x99, 0x33, 0xff, 0x99, 0x99, 0x00, 0xff, 0x99, 0x66, 0xff, 0xff, 0x99, 0x66, 0xcc, 0xff, 0x99, 0x66, 0x99, 0xff, 0x99, 0x66, 0x66, 0xff, 0x99, 0x66, 0x33, 0xff, 0x99, 0x66, 0x00, 0xff,
    0x99, 0x33, 0xff, 0xff, 0x99, 0x33, 0xcc, 0xff, 0x99, 0x33, 0x99, 0xff, 0x99, 0x33, 0x66, 0xff, 0x99, 0x33, 0x33, 0xff, 0x99, 0x33, 0x00, 0xff, 0x99, 0x00, 0xff, 0xff, 0x99, 0x00, 0xcc, 0xff,
    0x99, 0x00, 0x99, 0xff, 0x99, 0x00, 0x66, 0xff, 0x99, 0x00, 0x33, 0xff, 0x99, 0x00, 0x00, 0xff, 0x66, 0xff, 0xff, 0xff, 0x66, 0xff, 0xcc, 0xff, 0x66, 0xff, 0x99, 0xff, 0x66, 0xff, 0x66, 0xff,
    0x66, 0xff, 0x33, 0xff, 0x66, 0xff, 0x00, 0xff, 0x66, 0xcc, 0xff, 0xff, 0x66, 0xcc, 0xcc, 0xff, 0x66, 0xcc, 0x99, 0xff, 0x66, 0xcc, 0x66, 0xff, 0x66, 0xcc, 0x33, 0xff, 0x66, 0xcc, 0x00, 0xff,
    0x66, 0x99, 0xff, 0xff, 0x66, 0x99, 0xcc, 0xff, 0x66, 0x99, 0x99, 0xff, 0x66, 0x99, 0x66, 0xff, 0x66, 0x99, 0x33, 0xff, 0x66, 0x99, 0x00, 0xff, 0x66, 0x66, 0xff, 0xff, 0x66, 0x66, 0xcc, 0xff,
    0x66, 0x66, 0x99, 0xff, 0x66, 0x66, 0x66, 0xff, 0x66, 0x66, 0x33, 0xff, 0x66, 0x66, 0x00, 0xff, 0x66, 0x33, 0xff, 0xff, 0x66, 0x33, 0xcc, 0xff, 0x66, 0x33, 0x99, 0xff, 0x66, 0x33, 0x66, 0xff,
    0x66, 0x33, 0x33, 0xff, 0x66, 0x33, 0x00, 0xff, 0x66, 0x00, 0xff, 0xff, 0x66, 0x00, 0xcc, 0xff, 0x66, 0x00, 0x99, 0xff, 0x66, 0x00, 0x66, 0xff, 0x66, 0x00, 0x33, 0xff, 0x66, 0x00, 0x00, 0xff,
    0x33, 0xff, 0xff, 0xff, 0x33, 0xff, 0xcc, 0xff, 0x33, 0xff, 0x99, 0xff, 0x33, 0xff, 0x66, 0xff, 0x33, 0xff, 0x33, 0xff, 0x33, 0xff, 0x00, 0xff, 0x33, 0xcc, 0xff, 0xff, 0x33, 0xcc, 0xcc, 0xff,
    0x33, 0xcc, 0x99, 0xff, 0x33, 0xcc, 0x66, 0xff, 0x33, 0xcc, 0x33, 0xff, 0x33, 0xcc, 0x00, 0xff, 0x33, 0x99, 0xff, 0xff, 0x33, 0x99, 0xcc, 0xff, 0x33, 0x99, 0x99, 0xff, 0x33, 0x99, 0x66, 0xff,
    0x33, 0x99, 0x33, 0xff, 0x33, 0x99, 0x00, 0xff, 0x33, 0x66, 0xff, 0xff, 0x33, 0x66, 0xcc, 0xff, 0x33, 0x66, 0x99, 0xff, 0x33, 0x66, 0x66, 0xff, 0x33, 0x66, 0x33, 0xff, 0x33, 0x66, 0x00, 0xff,
    0x33, 0x33, 0xff, 0xff, 0x33, 0x33, 0xcc, 0xff, 0x33, 0x33, 0x99, 0xff, 0x33, 0x33, 0x66, 0xff, 0x33, 0x33, 0x33, 0xff, 0x33, 0x33, 0x00, 0xff, 0x33, 0x00, 0xff, 0xff, 0x33, 0x00, 0xcc, 0xff,
    0x33, 0x00, 0x99, 0xff, 0x33, 0x00, 0x66, 0xff, 0x33, 0x00, 0x33, 0xff, 0x33, 0x00, 0x00, 0xff, 0x00, 0xff, 0xff, 0xff, 0x00, 0xff, 0xcc, 0xff, 0x00, 0xff, 0x99, 0xff, 0x00, 0xff, 0x66, 0xff,
    0x00, 0xff, 0x33, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xcc, 0xff, 0xff, 0x00, 0xcc, 0xcc, 0xff, 0x00, 0xcc, 0x99, 0xff, 0x00, 0xcc, 0x66, 0xff, 0x00, 0xcc, 0x33, 0xff, 0x00, 0xcc, 0x00, 0xff,
    0x00, 0x99, 0xff, 0xff, 0x00, 0x99, 0xcc, 0xff, 0x00, 0x99, 0x99, 0xff, 0x00, 0x99, 0x66, 0xff, 0x00, 0x99, 0x33, 0xff, 0x00, 0x99, 0x00, 0xff, 0x00, 0x66, 0xff, 0xff, 0x00, 0x66, 0xcc, 0xff,
    0x00, 0x66, 0x99, 0xff, 0x00, 0x66, 0x66, 0xff, 0x00, 0x66, 0x33, 0xff, 0x00, 0x66, 0x00, 0xff, 0x00, 0x33, 0xff, 0xff, 0x00, 0x33, 0xcc, 0xff, 0x00, 0x33, 0x99, 0xff, 0x00, 0x33, 0x66, 0xff,
    0x00, 0x33, 0x33, 0xff, 0x00, 0x33, 0x00, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xcc, 0xff, 0x00, 0x00, 0x99, 0xff, 0x00, 0x00, 0x66, 0xff, 0x00, 0x00, 0x33, 0xff, 0xee, 0x00, 0x00, 0xff,
    0xdd, 0x00, 0x00, 0xff, 0xbb, 0x00, 0x00, 0xff, 0xaa, 0x00, 0x00, 0xff, 0x88, 0x00, 0x00, 0xff, 0x77, 0x00, 0x00, 0xff, 0x55, 0x00, 0x00, 0xff, 0x44, 0x00, 0x00, 0xff, 0x22, 0x00, 0x00, 0xff,
    0x11, 0x00, 0x00, 0xff, 0x00, 0xee, 0x00, 0xff, 0x00, 0xdd, 0x00, 0xff, 0x00, 0xbb, 0x00, 0xff, 0x00, 0xaa, 0x00, 0xff, 0x00, 0x88, 0x00, 0xff, 0x00, 0x77, 0x00, 0xff, 0x00, 0x55, 0x00, 0xff,
    0x00, 0x44, 0x00, 0xff, 0x00, 0x22, 0x00, 0xff, 0x00, 0x11, 0x00, 0xff, 0x00, 0x00, 0xee, 0xff, 0x00, 0x00, 0xdd, 0xff, 0x00, 0x00, 0xbb, 0xff, 0x00, 0x00, 0xaa, 0xff, 0x00, 0x00, 0x88, 0xff,
    0x00, 0x00, 0x77, 0xff, 0x00, 0x00, 0x55, 0xff, 0x00, 0x00, 0x44, 0xff, 0x00, 0x00, 0x22, 0xff, 0x00, 0x00, 0x11, 0xff, 0xee, 0xee, 0xee, 0xff, 0xdd, 0xdd, 0xdd, 0xff, 0xbb, 0xbb, 0xbb, 0xff,
    0xaa, 0xaa, 0xaa, 0xff, 0x88, 0x88, 0x88, 0xff, 0x77, 0x77, 0x77, 0xff, 0x55, 0x55, 0x55, 0xff, 0x44, 0x44, 0x44, 0xff, 0x22, 0x22, 0x22, 0xff, 0x11, 0x11, 0x11, 0xff, 0x00, 0x00, 0x00, 0xff,
};

struct FTransformNode
{
	FString Name;
	FIntTransformFrameData Transform;
	int32 LayerId = -1;
	bool bHidden = false;

	int32 ChildId = -1;
};
struct FGroupNode
{
    TVoxelArray<int32> ChildIds;
};
struct FShapeNode
{
	TFrameData<int32> ModelId;
};

using FNode = TVariant<FEmptyVariantState, FTransformNode, FGroupNode, FShapeNode>;

class FDict
{
public:
    FDict() = default;

    FString FindString(FName Name, const FString& Default = {}) const
	{
		const FString* Value = Map.Find(Name);
        if (!Value)
        {
	        return Default;
        }

        return *Value;
	}
    bool FindBool(FName Name, bool bDefault = false) const
	{
		const FString* Value = Map.Find(Name);
        if (!Value)
        {
	        return bDefault;
        }

        ensure(*Value == "0" || *Value == "1");
        return *Value == "1";
	}
    int32 FindInt(FName Name, int32 Default = 0) const
	{
		const FString* Value = Map.Find(Name);
        if (!Value)
        {
	        return Default;
        }

        return FCString::Atoi(**Value);
	}

    FIntTransform ReadTransform() const
    {
		FIntTransform Transform = FIntTransform::Identity();

		const FString Rotation = FindString(STATIC_FNAME("_r"));
		const FString Translation = FindString(STATIC_FNAME("_t"));

		if (!Rotation.IsEmpty())
        {
            /**
             * bit | value
             * 0-1 : 1 : index of the non-zero entry in the first row
             * 2-3 : 2 : index of the non-zero entry in the second row
             * 4   : 0 : the sign in the first row (0 : positive; 1 : negative)
             * 5   : 1 : the sign in the second row (0 : positive; 1 : negative)
             * 6   : 1 : the sign in the third row (0 : positive; 1 : negative)
             */
			const int32 PackedRotationBits = FCString::Atoi(*Rotation);

			const int32 Row0Index = (PackedRotationBits >> 0) & 3;
			const int32 Row1Index = (PackedRotationBits >> 2) & 3;
			const int32 Row2Index =
				Row0Index == 0 ? Row1Index == 1 ? 2 : 1 :
				Row0Index == 1 ? Row1Index == 2 ? 0 : 2 :
				Row0Index == 2 ? Row1Index == 0 ? 1 : 0 :
				ensure(false);

            ensure(Row0Index != Row1Index);
            ensure(Row0Index != Row2Index);
            ensure(Row1Index != Row2Index);

            check(0 <= Row0Index && Row0Index < 3);
            check(0 <= Row1Index && Row1Index < 3);
            check(0 <= Row2Index && Row2Index < 3);

			FIntVector Row0{ ForceInit };
			FIntVector Row1{ ForceInit };
			FIntVector Row2{ ForceInit };

			Row0[Row0Index] = 1;
			Row1[Row1Index] = 1;
			Row2[Row2Index] = 1;

			if (PackedRotationBits & (1 << 4)) { Row0 = -Row0; }
			if (PackedRotationBits & (1 << 5)) { Row1 = -Row1; }
			if (PackedRotationBits & (1 << 6)) { Row2 = -Row2; }

            Transform.M[0][0] = Row0[0];
            Transform.M[0][1] = Row0[1];
            Transform.M[0][2] = Row0[2];

            Transform.M[1][0] = Row1[0];
            Transform.M[1][1] = Row1[1];
            Transform.M[1][2] = Row1[2];

            Transform.M[2][0] = Row2[0];
            Transform.M[2][1] = Row2[1];
            Transform.M[2][2] = Row2[2];
		}

		if (!Translation.IsEmpty())
        {
	        TArray<FString> Array;
            Translation.ParseIntoArray(Array, TEXT(" "));
            if (ensure(Array.Num() == 3))
            {
	            const int32 X = FCString::Atoi(*Array[0]);
	            const int32 Y = FCString::Atoi(*Array[1]);
	            const int32 Z = FCString::Atoi(*Array[2]);

                Transform.M[0][3] = X;
                Transform.M[1][3] = Y;
                Transform.M[2][3] = Z;
            }
		}

		return Transform;
    }

private:
    TMap<FName, FString> Map;

    friend class FReader;
};

class FReader
{
public:
	explicit FReader(const TVoxelArrayView<const uint8>& Data)
		: Data(Data)
	{
	}

    bool IsEndOfFile() const
	{
        ensure(Offset <= Data.Num());
        return Offset >= Data.Num();
	}
    bool IsValid() const
	{
		return bIsValid;
	}
    int64 Pos() const
	{
		return Offset;
	}
    void Seek(int64 InOffset)
	{
		Offset += InOffset;
        if (!ensure(Offset <= Data.Num()))
        {
	        bIsValid = false;
        }
	}

    template<typename T>
    T Read()
	{
		static_assert(TIsTriviallyDestructible<T>::Value, "");
        T Value = {};
        this->Read(&Value, sizeof(Value));
        return Value;
	}
    template<typename T>
    TVoxelArray<T> ReadArray(int32 Num)
	{
		static_assert(TIsTriviallyDestructible<T>::Value, "");

        TVoxelArray<T> Array;
        Array.Reserve(Num);
        Array.SetNumUninitialized(Num);

        this->Read(Array.GetData(), Num * sizeof(T));

        return Array;
	}
    template<typename T>
    T ReadStaticArray()
	{
		static_assert(TIsTriviallyDestructible<typename T::ElementType>::Value, "");

        T Value{ NoInit };
        this->Read(&Value, sizeof(Value));
        return Value;
	}

    FString ReadString()
	{
		const int32 Num = Read<int32>();
        if (!ensure(Num >= 0))
        {
	        bIsValid = false;
            return {};
        }

        FString String;
        String.Reserve(Num);

        for (int32 Index = 0; Index < Num; Index++)
        {
	        String += Read<char>();
        }

        return String;
	}

    FDict ReadDict()
	{
		const int32 Num = Read<int32>();
        if (!ensure(Num >= 0))
        {
	        bIsValid = false;
            return {};
        }

        FDict Dict;
        Dict.Map.Reserve(Num);

        for (int32 Index = 0; Index < Num; Index++)
        {
            const FName Key = *ReadString();
            const FString Value = ReadString();

            ensure(!Dict.Map.Contains(Key));
	        Dict.Map.Add(Key, Value);
        }

        return Dict;
	}

private:
    bool bIsValid = true;
    int64 Offset = 0;
    const TVoxelArrayView<const uint8> Data;

	void Read(void* OutData, int64 Size)
	{
        if (Offset + Size > Data.Num())
        {
            LOG_VOXEL(Error, "Failed to read %lld bytes", Size);
            ensureVoxelSlow(false);
	        bIsValid = false;
        }

        if (!IsValid())
        {
	        FMemory::Memzero(OutData, Size);
            return;
        }

        ensure(Data.IsValidIndex(Offset + Size - 1));
        FMemory::Memcpy(OutData, &Data[Offset], Size);

        Offset += Size;
	}
};

inline void GenerateInstances(
    int32 NodeId,
    const FTransformNode* LastTransformNode,
    const TSharedPtr<FGroup>& ParentGroup,
    FMagicaScene& Scene,
	const TVoxelArray<TSharedPtr<FNode>>& Nodes)
{
	const TSharedPtr<FNode> Node = Nodes[NodeId];
	if (!ensure(Node))
	{
		return;
	}

	if (Node->IsType<FTransformNode>())
	{
		ensure(!LastTransformNode);
		const FTransformNode& TransformNode = Node->Get<FTransformNode>();
        GenerateInstances(TransformNode.ChildId, &TransformNode, ParentGroup, Scene, Nodes);
	}
	else if (Node->IsType<FGroupNode>())
	{
		const FGroupNode& GroupNode = Node->Get<FGroupNode>();

		const TSharedRef<FGroup> Group = MakeVoxelShared<FGroup>();
		Scene.Groups.Add(Group);

        if (LastTransformNode)
		{
			Group->Name = LastTransformNode->Name;
			Group->bHidden = LastTransformNode->bHidden;
			Group->Transform = LastTransformNode->Transform;

			if (LastTransformNode->LayerId != -1)
			{
				Group->Layer = Scene.Layers[LastTransformNode->LayerId];
			}
		}
        else if (NodeId == 0)
        {
			Group->Name = "Root";
        }

		Group->ParentGroup = ParentGroup;

        for (const int32 ChildId : GroupNode.ChildIds)
        {
        	GenerateInstances(ChildId, nullptr, Group, Scene, Nodes);
        }
	}
	else
	{
		check(Node->IsType<FShapeNode>());
		const FShapeNode& ShapeNode = Node->Get<FShapeNode>();

		const TSharedRef<FInstance> Instance = MakeVoxelShared<FInstance>();

        if (LastTransformNode)
		{
			Instance->Name = LastTransformNode->Name;
			Instance->bHidden = LastTransformNode->bHidden;
			Instance->Transform = LastTransformNode->Transform;

			if (LastTransformNode->LayerId != -1)
			{
				Instance->Layer = Scene.Layers[LastTransformNode->LayerId];
			}
		}

        for (const TFrameData<int32>::FFrame& Frame : ShapeNode.ModelId.Frames)
        {
			if (!ensure(Frame.Data != -1))
			{
                continue;
            }
			const TSharedPtr<FModel> Model = Scene.Models[Frame.Data];
            if (!Model)
            {
                // Happens if NumVoxelsInChunk is 0
                continue;
            }

            Instance->Model.Frames.Add({ Model, Frame.FrameIndex });
		}
		Instance->ParentGroup = ParentGroup;

        if (Instance->Model.Frames.Num() > 0)
        {
			Scene.Instances.Add(Instance);
        }
	}
}

#define CHUNK_ID(Name) ((Name[0] << 0) | (Name[1] << 8) | (Name[2] << 16) | (Name[3] << 24))

bool ReadScene(const TVoxelArrayView<const uint8>& Data, FMagicaScene& OutScene)
{
    FReader Reader(Data);

    // Load and validate file header and file version.
    const uint32 FileHeader = Reader.Read<uint32>();
    const uint32 FileVersion = Reader.Read<uint32>();
    if (FileHeader != CHUNK_ID("VOX "))
    {
	    LOG_VOXEL(Error, "Invalid magica voxel header");
        return false;
    }
    if (FileVersion < 150 || FileVersion > 200)
    {
	    LOG_VOXEL(Error, "Unsupported magica voxel version: %u", FileVersion);
        return false;
    }

    TOptional<FIntVector> Size;
	TOptional<TVoxelStaticArray<uint8, 256>> IndexMap;
    TVoxelArray<TSharedPtr<FNode>> Nodes;

    const auto MakeNode = [&](int32 NodeId, auto TypeInst) -> auto&
    {
        using Type = VOXEL_GET_TYPE(TypeInst);

		if (!Nodes.IsValidIndex(NodeId))
		{
			Nodes.SetNum(NodeId + 1);
		}
        ensure(!Nodes[NodeId]);

		const TSharedRef<FNode> NodePtr = MakeVoxelShared<FNode>(TInPlaceType<Type>());
		Nodes[NodeId] = NodePtr;
		return NodePtr->Get<Type>();
    };

    TVoxelArray<TSharedPtr<FModel>>& Models = OutScene.Models;
    TVoxelArray<TSharedPtr<FLayer>>& Layers = OutScene.Layers;

    FPalette& Palette = OutScene.Palette;
    // Copy the default palette into the scene. It may get overwritten by a palette chunk later
    static_assert(sizeof(DefaultPalette) == FPalette::Num() * FPalette::GetTypeSize(), "");
    FMemory::Memcpy(Palette.GetData(), DefaultPalette, Palette.Num() * Palette.GetTypeSize());

    // Parse chunks until we reach the end of the file/buffer
    while (!Reader.IsEndOfFile() && Reader.IsValid())
    {
		// read the fields common to all chunks
		const int32 ChunkId = Reader.Read<int32>();
		const int32 ChunkSize = Reader.Read<int32>();
		const int32 ChunkChildSize = Reader.Read<int32>();
        const FString ChunkIdName = FString(sizeof(ChunkId), reinterpret_cast<const char*>(&ChunkId));

        const int64 CurrentReaderPos = Reader.Pos();
        ON_SCOPE_EXIT
        {
            ensureMsgf(Reader.Pos() == CurrentReaderPos + ChunkSize, TEXT("%s"), *ChunkIdName);
        };

        // process the chunk.
        switch (ChunkId)
        {
            case CHUNK_ID("MAIN"):
            {
                ensure(ChunkSize == 0);
                break;
            }
            case CHUNK_ID("SIZE"):
            {
                ensure(ChunkSize == sizeof(FIntVector) && ChunkChildSize == 0);
                Size = Reader.Read<FIntVector>();
                break;
            }
            case CHUNK_ID("XYZI"):
            {
                ensure(ChunkChildSize == 0 && Size.IsSet()); // must have read a SIZE chunk prior to XYZI.

                const int32 NumVoxelsInChunk = Reader.Read<int32>();
                if (NumVoxelsInChunk != 0)
                {
                    const TSharedRef<FModel> Model = MakeVoxelShared<FModel>();
                    Model->Size = *Size;
                    Models.Add(Model);

                    Model->Voxels = Reader.ReadArray<FVoxel>(NumVoxelsInChunk);
                }
                else
                {
                    Models.Add(nullptr);
                }
                break;
            }
            case CHUNK_ID("RGBA"):
            {
                ensure(ChunkSize == sizeof(FPalette));
                Palette = Reader.ReadStaticArray<FPalette>();
                break;
            }
            case CHUNK_ID("nTRN"):
            {
                const int32 NodeId = Reader.Read<int32>();
                FTransformNode& Node = MakeNode(NodeId, FTransformNode());

            	const FDict NodeDict = Reader.ReadDict();

                Node.Name = NodeDict.FindString(STATIC_FNAME("_name"));
                Node.bHidden = NodeDict.FindBool(STATIC_FNAME("_hidden"));

                Node.ChildId = Reader.Read<int32>();

            	const int32 ReservedId = Reader.Read<int32>();
                ensure(ReservedId == -1);

            	Node.LayerId = Reader.Read<int32>();

                const int32 NumFrames = Reader.Read<int32>();
                for (int32 Index = 0; Index < NumFrames; Index++)
				{
					const FDict FrameDict = Reader.ReadDict();
                    const int32 FrameIndex = FrameDict.FindInt(STATIC_FNAME("_f"));

                    OutScene.NumFrames = FMath::Max(OutScene.NumFrames, FrameIndex + 1);
                    Node.Transform.Frames.Add({ FrameDict.ReadTransform(), FrameIndex });
				}
                Node.Transform.SortFrames();

                break;
            }
            case CHUNK_ID("nGRP"):
            {
                const int32 NodeId = Reader.Read<int32>();
                FGroupNode& Node = MakeNode(NodeId, FGroupNode());

                // Unused
            	const FDict Dict = Reader.ReadDict();

                const int32 NumChildNodes = Reader.Read<int32>();

                if (NumChildNodes > 0)
                {
                    Node.ChildIds = Reader.ReadArray<int32>(NumChildNodes);
                }
                break;
            }
            case CHUNK_ID("nSHP"):
            {
                const int32 NodeId = Reader.Read<int32>();
                FShapeNode& Node = MakeNode(NodeId, FShapeNode());

                // Unused
            	const FDict NodeDict = Reader.ReadDict();

                const int32 NumModels = Reader.Read<int32>();
                for (int32 ModelIndex = 0; ModelIndex < NumModels; ModelIndex++)
				{
					const int32 ModelId = Reader.Read<int32>();
					ensure(Models.IsValidIndex(ModelId));

					const FDict ModelDict = Reader.ReadDict();
                    const int32 FrameIndex = ModelDict.FindInt(STATIC_FNAME("_f"));

                    OutScene.NumFrames = FMath::Max(OutScene.NumFrames, FrameIndex + 1);
					Node.ModelId.Frames.Add({ ModelId, FrameIndex });
				}
                Node.ModelId.SortFrames();

                break;
            }
            case CHUNK_ID("IMAP"):
            {
                ensure(ChunkSize == 256);
                IndexMap = Reader.ReadStaticArray<TVoxelStaticArray<uint8, 256>>();
                break;
            }
            case CHUNK_ID("LAYR"):
            {
                const int32 LayerId = Reader.Read<int32>();
            	const FDict Dict = Reader.ReadDict();
                const int32 ReservedId = Reader.Read<int32>();
                ensure(ReservedId == -1);

                const TSharedRef<FLayer> Layer = MakeVoxelShared<FLayer>();
                Layer->Name = Dict.FindString(STATIC_FNAME("_name"));
                Layer->bHidden = Dict.FindBool(STATIC_FNAME("_hidden"));

				if (!Layers.IsValidIndex(LayerId))
				{
					Layers.SetNum(LayerId + 1);
				}
				ensure(!Layers[LayerId]);
				Layers[LayerId] = Layer;

                break;
            }
            case CHUNK_ID("MATL"):
            case CHUNK_ID("MATT"):
            case CHUNK_ID("NOTE"):
            case CHUNK_ID("rOBJ"):
            case CHUNK_ID("rCAM"):
            {
                Reader.Seek(ChunkSize);
                break;
            }
            default:
            {
				ensureMsgf(false, TEXT("%s"), *ChunkIdName);
                Reader.Seek(ChunkSize);
                break;
            }
        }
    }

    if (!Reader.IsValid())
    {
	    return false;
    }

    if (!ensure(Nodes[0]) ||
        !ensure(Nodes[0]->IsType<FTransformNode>()))
    {
        LOG_VOXEL(Error, "Invalid root node");
    	return false;
    }

    if (Nodes.Num() > 0)
    {
		GenerateInstances(0, nullptr, nullptr, OutScene, Nodes);
    }
    else
    {
	    if (!ensure(Models.Num() == 1))
	    {
		    LOG_VOXEL(Error, "Empty scene");
            return false;
	    }

		const TSharedRef<FInstance> Instance = MakeVoxelShared<FInstance>();
        Instance->Model.Frames.Add({ Models[0], 0 });
        OutScene.Instances.Add(Instance);
    }

    // To support index-level assumptions (eg. artists using top 16 colors for color/palette cycling,
    // other ranges for emissive etc), we must ensure the order of colors that the artist sees in the
    // magicavoxel tool matches the actual index we'll end up using here. Unfortunately, magicavoxel
    // does an unexpected thing when remapping colors in the editor using ctrl+drag within the palette.
    // Instead of remapping all indices in all models, it just keeps track of a display index to actual
    // palette map and uses that to show reordered colors in the palette window. This is how that
    // map works:
    //   displaycolor[k] = paletteColor[imap[k]]
    // To ensure our indices are in the same order as displayed by magicavoxel within the palette
    // window, we apply the mapping from the IMAP chunk both to the color palette and indices within each
    // voxel model.
    if (IndexMap)
    {
        // the imap chunk maps from display index to actual index.
		// generate an inverse index map (maps from actual index to display index)
		TVoxelStaticArray<uint8, 256> IndexMapReverse{ NoInit };
        for (int32 Index = 0; Index < 256; Index++)
        {
            IndexMapReverse[(*IndexMap)[Index]] = Index;
        }

        // reorder colors in the palette so the palette contains colors in display order
		FPalette OldPalette = Palette;
        for (int32 Index = 0; Index < 256; Index++)
        {
            const int32 RemappedIndex = ((*IndexMap)[Index] + 255) & 0xFF;
            Palette[Index] = OldPalette[RemappedIndex];
        }

        // ensure that all models are remapped so they are using display order palette indices.
        for (const TSharedPtr<FModel>& Model : Models)
        {
			if (!Model)
			{
				continue;
			}

            for (FVoxel& Voxel : Model->Voxels)
            {
				Voxel.Value = IndexMapReverse[Voxel.Value];
            }
		}
    }
    else
    {
        // There's still an offset of one
		FPalette OldPalette = Palette;
        for (int32 Index = 0; Index < 256; Index++)
        {
            const int32 RemappedIndex = (Index + 255) & 0xFF;
            Palette[Index] = OldPalette[RemappedIndex];
        }
    }

    return true;
}

#undef CHUNK_ID

}