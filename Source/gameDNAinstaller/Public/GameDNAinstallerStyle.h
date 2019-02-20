// gameDNA installer
// Created by Patryk Stepniewski
// Copyright (c) 2014-2019 gameDNA Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

class FGameDNAinstallerStyle
{
public:
	/** Initialize */
	static void Initialize();
	
	/** Shutdown */
	static void Shutdown();	

	/** Reload Textures */
	static void ReloadTextures();
	
	/** Get */
	static const ISlateStyle& Get();
	
	/** Get Style Set Name */
	static FName GetStyleSetName();
	
private:
	/** Create */
	static TSharedRef<class FSlateStyleSet> Create();
	
	/** Style Instance */
	static TSharedPtr<class FSlateStyleSet> StyleInstance;
};
