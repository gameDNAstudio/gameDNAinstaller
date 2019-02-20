// gameDNA installer
// Created by Patryk Stepniewski
// Copyright (c) 2014-2019 gameDNA Ltd. All Rights Reserved.

#include "GameDNAinstallerStyle.h"
#include "Runtime/Engine/Public/Slate/SlateGameResources.h"
#include "Styling/SlateStyleRegistry.h"
#include "Framework/Text/SlateTextRun.h"
#include "Runtime/Projects/Public/Interfaces/IPluginManager.h"

TSharedPtr<FSlateStyleSet> FGameDNAinstallerStyle::StyleInstance = NULL;

void FGameDNAinstallerStyle::Initialize()
{
	if (!StyleInstance.IsValid())
	{
		StyleInstance = Create();
		FSlateStyleRegistry::RegisterSlateStyle(*StyleInstance);
	}
}

void FGameDNAinstallerStyle::Shutdown()
{
	FSlateStyleRegistry::UnRegisterSlateStyle(*StyleInstance);
	ensure(StyleInstance.IsUnique());
	StyleInstance.Reset();
}

FName FGameDNAinstallerStyle::GetStyleSetName()
{
	static FName StyleSetName(TEXT("GameDNAinstallerStyle"));
	return StyleSetName;
}

#define IMAGE_BRUSH( RelativePath, ... ) FSlateImageBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BOX_BRUSH( RelativePath, ... ) FSlateBoxBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define BORDER_BRUSH( RelativePath, ... ) FSlateBorderBrush( Style->RootToContentDir( RelativePath, TEXT(".png") ), __VA_ARGS__ )
#define TTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".ttf") ), __VA_ARGS__ )
#define OTF_FONT( RelativePath, ... ) FSlateFontInfo( Style->RootToContentDir( RelativePath, TEXT(".otf") ), __VA_ARGS__ )

const FVector2D Icon16x16(16.0f, 16.0f);
const FVector2D Icon20x20(20.0f, 20.0f);
const FVector2D Icon40x40(40.0f, 40.0f);

TSharedRef<FSlateStyleSet> FGameDNAinstallerStyle::Create()
{
	TSharedRef<FSlateStyleSet> Style = MakeShareable(new FSlateStyleSet("GameDNAinstallerStyle"));
	Style->SetContentRoot(IPluginManager::Get().FindPlugin("gameDNAinstaller")->GetBaseDir() / TEXT("Resources"));

	// Logo
	Style->Set("gameDNAinstaller.Logo", new IMAGE_BRUSH(TEXT("Graphics/gameDNAinstaller_logo"), FVector2D(300, 186)));
	
	// Icon 
	Style->Set("gameDNAinstaller.OpenPluginWindow", new IMAGE_BRUSH(TEXT("Graphics/gameDNAinstaller_icon"), Icon40x40));

	// Main Text Style
	auto MainTextStyle = FTextBlockStyle()
		.SetColorAndOpacity(FLinearColor::White)
		.SetShadowOffset(FVector2D::UnitVector)
		.SetShadowColorAndOpacity(FLinearColor::Black)
		.SetFont(TTF_FONT("Fonts/Lato-Black", 10));
	Style->Set("gameDNAinstaller.MainTextStyle", MainTextStyle);

	// Plugin Button
	FSlateColorBrush PluginButtonColorBrushNormal = FSlateColorBrush(FColor(255, 255, 255));
	PluginButtonColorBrushNormal.TintColor = FSlateColor(FColor(191, 32, 40));
	FSlateColorBrush PluginButtonColorBrushHovered = FSlateColorBrush(FColor(255, 255, 255));
	PluginButtonColorBrushHovered.TintColor = FSlateColor(FColor(217, 37, 47));
	FSlateColorBrush PluginButtonColorBrushPressed = FSlateColorBrush(FColor(255, 255, 255));
	PluginButtonColorBrushPressed.TintColor = FSlateColor(FColor(167, 29, 36));
	FSlateColorBrush PluginButtonColorBrushDisabled = FSlateColorBrush(FColor(255, 255, 255));
	PluginButtonColorBrushDisabled.TintColor = FSlateColor(FColor(102, 20, 24));
	auto PluginButton = FButtonStyle()
		.SetNormal(PluginButtonColorBrushNormal)
		.SetNormalPadding(FMargin(3, 3, 3, 3))
		.SetHovered(PluginButtonColorBrushHovered)
		.SetPressed(PluginButtonColorBrushPressed)
		.SetPressedPadding(FMargin(3, 4, 3, 2))
		.SetDisabled(PluginButtonColorBrushDisabled);
	Style->Set("gameDNAinstaller.PluginButtonStyle", PluginButton);

	// Plugin Install Button
	FSlateColorBrush PluginInstallButtonColorBrushNormal = FSlateColorBrush(FColor(255, 255, 255));
	PluginInstallButtonColorBrushNormal.TintColor = FSlateColor(FColor(82, 168, 82));
	FSlateColorBrush PluginInstallButtonColorBrushHovered = FSlateColorBrush(FColor(255, 255, 255));
	PluginInstallButtonColorBrushHovered.TintColor = FSlateColor(FColor(92, 186, 92));
	FSlateColorBrush PluginInstallButtonColorBrushPressed = FSlateColorBrush(FColor(255, 255, 255));
	PluginInstallButtonColorBrushPressed.TintColor = FSlateColor(FColor(71, 147, 71));
	FSlateColorBrush PluginInstallButtonColorBrushDisabled = FSlateColorBrush(FColor(255, 255, 255));
	PluginInstallButtonColorBrushDisabled.TintColor = FSlateColor(FColor(46, 92, 46));
	auto PluginInstallButton = FButtonStyle()
		.SetNormal(PluginInstallButtonColorBrushNormal)
		.SetNormalPadding(FMargin(3, 3, 3, 3))
		.SetHovered(PluginInstallButtonColorBrushHovered)
		.SetPressed(PluginInstallButtonColorBrushPressed)
		.SetPressedPadding(FMargin(3, 4, 3, 2))
		.SetDisabled(PluginInstallButtonColorBrushDisabled);
	Style->Set("gameDNAinstaller.PluginInstallButtonStyle", PluginInstallButton);

	// Plugin Button Text
	auto PluginButtonText = FTextBlockStyle()
		.SetFont(TTF_FONT("Fonts/Lato-Black", 9))
		.SetColorAndOpacity(FLinearColor::White)
		.SetShadowOffset(FVector2D::UnitVector)
		.SetShadowColorAndOpacity(FLinearColor::Black);
	Style->Set("gameDNAinstaller.PluginButtonTextStyle", PluginButtonText);

	return Style;
}

#undef IMAGE_BRUSH
#undef BOX_BRUSH
#undef BORDER_BRUSH
#undef TTF_FONT
#undef OTF_FONT

void FGameDNAinstallerStyle::ReloadTextures()
{
	if ( FSlateApplication::IsInitialized() && FSlateApplication::Get().GetRenderer() )
	{
		FSlateApplication::Get().GetRenderer()->ReloadTextureResources();
	}
}

const ISlateStyle& FGameDNAinstallerStyle::Get()
{
	return *StyleInstance;
}
