// gameDNA installer
// Created by Patryk Stepniewski
// Copyright (c) 2014-2019 gameDNA Ltd. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameDNAinstallerCore.h"

DECLARE_LOG_CATEGORY_EXTERN(LogGameDNAinstaller, Log, All);

typedef TSharedPtr<FGameDNAinstallerCore, ESPMode::ThreadSafe> FGameDNAinstallerCorePtr;

class FGameDNAinstallerModule : public IModuleInterface
{
private: 
	static FGameDNAinstallerModule* Singleton;

public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static inline FGameDNAinstallerModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FGameDNAinstallerModule>("gameDNAinstaller");
	}

	/** Get Core */
	FGameDNAinstallerCorePtr GetCore();

	/** Plugin Button Clicked */
	void PluginButtonClicked();

private:
	void AddToolbarExtension(FToolBarBuilder& Builder);
	void AddMenuExtension(FMenuBuilder& Builder);

	TSharedRef<class SDockTab> OnSpawnPluginTab(const class FSpawnTabArgs& SpawnTabArgs);

private:
	TSharedPtr<class FUICommandList> PluginCommands;
	FGameDNAinstallerCorePtr Core;
};
