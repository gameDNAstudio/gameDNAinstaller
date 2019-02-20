// gameDNA installer
// Created by Patryk Stepniewski
// Copyright (c) 2014-2019 gameDNA Ltd. All Rights Reserved.

#include "GameDNAinstaller.h"
#include "Runtime/Slate/Public/Framework/Commands/UIAction.h"
#include "LevelEditor.h"
#include "Kismet/GameplayStatics.h"
#include "Editor/WorkspaceMenuStructure/Public/WorkspaceMenuStructure.h"
#include "Editor/WorkspaceMenuStructure/Public/WorkspaceMenuStructureModule.h"
#include "Widgets/Docking/SDockTab.h"
#include "SGameDNAinstallerConsole.h"
#include "GameDNAinstallerStyle.h"
#include "GameDNAinstallerCommands.h"

#define LOCTEXT_NAMESPACE "gameDNAinstallerModule"

IMPLEMENT_MODULE(FGameDNAinstallerModule, gameDNAinstaller);

DEFINE_LOG_CATEGORY(LogGameDNAinstaller);

static const FName gameDNAinstallerTabName("gameDNA installer");

void FGameDNAinstallerModule::StartupModule()
{
	UE_LOG(LogGameDNAinstaller, Display, TEXT("gameDNA installer module started"));

	/** Initialize style and register commands */
	FGameDNAinstallerStyle::Initialize();
	FGameDNAinstallerStyle::ReloadTextures();
	FGameDNAinstallerCommands::Register();

	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FGameDNAinstallerCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FGameDNAinstallerModule::PluginButtonClicked),
		FCanExecuteAction());

	FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");

	{
		TSharedPtr<FExtender> MenuExtender = MakeShareable(new FExtender());
		MenuExtender->AddMenuExtension("EditLocalTabSpawners", EExtensionHook::After, PluginCommands, FMenuExtensionDelegate::CreateRaw(this, &FGameDNAinstallerModule::AddMenuExtension));
		LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(MenuExtender);
	}

	{
		TSharedPtr<FExtender> ToolbarExtender = MakeShareable(new FExtender);
		ToolbarExtender->AddToolBarExtension("Content", EExtensionHook::After, PluginCommands, FToolBarExtensionDelegate::CreateRaw(this, &FGameDNAinstallerModule::AddToolbarExtension));
		LevelEditorModule.GetToolBarExtensibilityManager()->AddExtender(ToolbarExtender);
	}

	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(gameDNAinstallerTabName, FOnSpawnTab::CreateRaw(this, &FGameDNAinstallerModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("gameDNAinstaller", "gameDNA installer"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);

	/** Initialize Core */
	Core = MakeShareable(new FGameDNAinstallerCore());
}

void FGameDNAinstallerModule::ShutdownModule()
{
	UE_LOG(LogGameDNAinstaller, Display, TEXT("gameDNA installer module shutdown"));

	FGameDNAinstallerStyle::Shutdown();
	FGameDNAinstallerCommands::Unregister();
	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(gameDNAinstallerTabName);
}

FGameDNAinstallerCorePtr FGameDNAinstallerModule::GetCore()
{
	return Core;
}

TSharedRef<SDockTab> FGameDNAinstallerModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	return SNew(SDockTab)
			.TabRole(ETabRole::NomadTab)
			[
				SNew(SGameDNAinstallerConsole)
			];
}

void FGameDNAinstallerModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->InvokeTab(gameDNAinstallerTabName);
}

void FGameDNAinstallerModule::AddMenuExtension(FMenuBuilder& Builder)
{
	Builder.AddMenuEntry(FGameDNAinstallerCommands::Get().OpenPluginWindow);
}

void FGameDNAinstallerModule::AddToolbarExtension(FToolBarBuilder& Builder)
{
	Builder.AddToolBarButton(FGameDNAinstallerCommands::Get().OpenPluginWindow);
}

#undef LOCTEXT_NAMESPACE
