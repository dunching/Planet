// Copyright Voxel Plugin, Inc. All Rights Reserved.

#pragma once

#include "VoxelCoreEditorMinimal.h"
#include "Styling/SlateStyle.h"
#include "Styling/SlateStyleMacros.h"

#define VOXEL_INITIALIZE_STYLE(Name) \
	INTELLISENSE_ONLY(void Name();) \
	class FVoxel ## Name ## Style : public FVoxelStyleSet \
	{ \
	public: \
		FVoxel ## Name ## Style() : FVoxelStyleSet(# Name) \
		{ \
			Init(); \
		} \
		void Init(); \
	}; \
	VOXEL_RUN_ON_STARTUP_EDITOR(Register ## Name ## Style) \
	{ \
		const TSharedRef<FVoxel ## Name ## Style> Style = MakeVoxelShared<FVoxel ## Name ## Style>(); \
		ConstCast(FVoxelEditorStyle::Get()).InitModule(*Style, MakeLambdaDelegate([]() -> TSharedPtr<FVoxelStyleSet> \
		{ \
			return MakeVoxelShared<FVoxel ## Name ## Style>(); \
		})); \
	} \
	void FVoxel ## Name ## Style::Init()

class FVoxelStyleSet;

DECLARE_DELEGATE_RetVal(TSharedPtr<FVoxelStyleSet>, FOnVoxelStyleReinitialize)

class VOXELCOREEDITOR_API FVoxelStyleSet : public FSlateStyleSet
{
public:
	FVoxelStyleSet(const FName& InStyleSetName);

	void InitModule(FVoxelStyleSet& StyleSets, const FOnVoxelStyleReinitialize& Delegate);

private:
	TArray<FOnVoxelStyleReinitialize> Styles;

	friend class FVoxelEditorStyle;
};

class VOXELCOREEDITOR_API FVoxelEditorStyle
{
public:
	static void Register();
	static void Unregister();
	static void Shutdown();

	static void ReloadTextures();

	static const FVoxelStyleSet& Get();

	static void ReinitializeStyle();

public:
	template<typename T>
	static const T& GetWidgetStyle(FName PropertyName, const ANSICHAR* Specifier = nullptr)
	{
		return Get().GetWidgetStyle<T>(PropertyName, Specifier);
	}

	static float GetFloat(FName PropertyName, const ANSICHAR* Specifier = nullptr)
	{
		return Get().GetFloat(PropertyName, Specifier);
	}

	static FVector2D GetVector(FName PropertyName, const ANSICHAR* Specifier = nullptr)
	{
		return Get().GetVector(PropertyName, Specifier);
	}

	static const FLinearColor& GetColor(FName PropertyName, const ANSICHAR* Specifier = nullptr)
	{
		return Get().GetColor(PropertyName, Specifier);
	}

	static const FSlateColor GetSlateColor(FName PropertyName, const ANSICHAR* Specifier = nullptr)
	{
		return Get().GetSlateColor(PropertyName, Specifier);
	}

	static const FMargin& GetMargin(FName PropertyName, const ANSICHAR* Specifier = nullptr)
	{
		return Get().GetMargin(PropertyName, Specifier);
	}

	static const FSlateBrush* GetBrush(FName PropertyName, const ANSICHAR* Specifier = nullptr)
	{
		return Get().GetBrush(PropertyName, Specifier);
	}

	static const TSharedPtr<FSlateDynamicImageBrush> GetDynamicImageBrush(FName BrushTemplate, FName TextureName, const ANSICHAR* Specifier = nullptr)
	{
		return Get().GetDynamicImageBrush(BrushTemplate, TextureName, Specifier);
	}

	static const TSharedPtr<FSlateDynamicImageBrush> GetDynamicImageBrush(FName BrushTemplate, const ANSICHAR* Specifier, UTexture2D* TextureResource, FName TextureName)
	{
		return Get().GetDynamicImageBrush(BrushTemplate, Specifier, TextureResource, TextureName);
	}

	static const TSharedPtr<FSlateDynamicImageBrush> GetDynamicImageBrush(FName BrushTemplate, UTexture2D* TextureResource, FName TextureName)
	{
		return Get().GetDynamicImageBrush(BrushTemplate, TextureResource, TextureName);
	}

	static const FSlateSound& GetSound(FName PropertyName, const ANSICHAR* Specifier = nullptr)
	{
		return Get().GetSound(PropertyName, Specifier);
	}

	static FSlateFontInfo GetFontStyle(FName PropertyName, const ANSICHAR* Specifier = nullptr)
	{
		return Get().GetFontStyle(PropertyName, Specifier);
	}

	static const FSlateBrush* GetDefaultBrush()
	{
		return Get().GetDefaultBrush();
	}

	static const FSlateBrush* GetNoBrush()
	{
		return FStyleDefaults::GetNoBrush();
	}

	static const FSlateBrush* GetOptionalBrush(FName PropertyName, const ANSICHAR* Specifier = nullptr, const FSlateBrush* const DefaultBrush = FStyleDefaults::GetNoBrush())
	{
		return Get().GetOptionalBrush(PropertyName, Specifier, DefaultBrush);
	}

	static void GetResources(TArray<const FSlateBrush*>& OutResources)
	{
		return Get().GetResources(OutResources);
	}

	static const FName& GetStyleSetName()
	{
		return Get().GetStyleSetName();
	}
};