// gameDNA installer
// Created by Patryk Stepniewski
// Copyright (c) 2014-2019 gameDNA Ltd. All Rights Reserved.

#include "GameDNAinstallerSave.h"

UGameDNAinstallerSave::UGameDNAinstallerSave(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SaveSlotName = TEXT("GameDNAinstaller");
	UserIndex = 0;

	PluginsRevisions = TMap<FString, int32>();
}
