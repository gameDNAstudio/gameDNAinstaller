// gameDNA installer
// Created by Patryk Stepniewski
// Copyright (c) 2014-2019 gameDNA Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_DELEGATE_OneParam(FOnGameDNAinstallerPluginButtonClicked, TSharedRef<struct FGameDNAinstallerPlugin>);
DECLARE_DELEGATE_OneParam(FOnGameDNAinstallerInstallationProgress, const TCHAR*);
DECLARE_DELEGATE_OneParam(FOnGameDNAinstallerInstallationCompleted, bool);
DECLARE_DELEGATE_OneParam(FOnGameDNAinstallerDownloadPluginsListCompleted, bool);

/** Plugin State */
enum EGameDNAInstallerPluginState
{
	NotInstalled,
	Installed,
	UpdateSDK,
	InstallSDK
};

/** Plugin */
struct FGameDNAinstallerPlugin
{
	/** Plugin's ID */
	FString Id;

	/** Plugin's Name */
	FString Name;

	/** Plugin's Website */
	FString Website;

	/** Plugin's State */
	EGameDNAInstallerPluginState State;

	/** Plugin's Revision */
	int32 Revision;

	/** Button Can Be Enabled */
	bool ButtonCanBeEnabled;

	FGameDNAinstallerPlugin()
	{
		Id = FString("Unknown");
		Name = FString("Unknown");
		Website = FString("Unknown");
		State = EGameDNAInstallerPluginState::NotInstalled;
		Revision = 0;
		ButtonCanBeEnabled = false;
	}

	FGameDNAinstallerPlugin(FString Id, FString Name, FString Website, EGameDNAInstallerPluginState State, bool ButtonCanBeEnabled)
	{
		this->Id = Id;
		this->Name = Name;
		this->Website = Website;
		this->State = State;
		this->ButtonCanBeEnabled = ButtonCanBeEnabled;
	}
};
