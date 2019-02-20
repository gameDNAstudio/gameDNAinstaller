// gameDNA installer
// Created by Patryk Stepniewski
// Copyright (c) 2014-2019 gameDNA Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "GameDNAinstallerStyle.h"

class FGameDNAinstallerCommands : public TCommands<FGameDNAinstallerCommands>
{
public:
	FGameDNAinstallerCommands()
		: TCommands<FGameDNAinstallerCommands>(TEXT("gameDNAinstaller"), NSLOCTEXT("Contexts", "gameDNAinstaller", "SDK installer for gameDNA's plugins"), NAME_None, FGameDNAinstallerStyle::GetStyleSetName())
	{
	}

	/** Register Commands */
	virtual void RegisterCommands() override;

public:
	TSharedPtr<FUICommandInfo> OpenPluginWindow;
};
