// gameDNA installer
// Created by Patryk Stepniewski
// Copyright (c) 2014-2019 gameDNA Ltd. All Rights Reserved.

#include "SGameDNAinstallerMarshaller.h"
#include "EditorStyleSet.h"
#include "Editor.h"
#include "Runtime/Core/Public/Internationalization/Text.h"
#include "GameDNAinstallerStyle.h"
#include "Runtime/SlateCore/Public/Types/SlateStructs.h"


TSharedRef<FGameDNAinstallerTextLayoutMarshaller> FGameDNAinstallerTextLayoutMarshaller::Create()
{
	return MakeShareable(new FGameDNAinstallerTextLayoutMarshaller());
}

FGameDNAinstallerTextLayoutMarshaller::FGameDNAinstallerTextLayoutMarshaller()
	: TextLayout(nullptr)
	, CachedNumMessages(0)
{
}

FGameDNAinstallerTextLayoutMarshaller::~FGameDNAinstallerTextLayoutMarshaller()
{
}

void FGameDNAinstallerTextLayoutMarshaller::SetText(const FString& SourceString, FTextLayout& TargetTextLayout)
{
	TextLayout = &TargetTextLayout;
	AppendMessagesToTextLayout(Messages);
}

void FGameDNAinstallerTextLayoutMarshaller::GetText(FString& TargetString, const FTextLayout& SourceTextLayout)
{
	SourceTextLayout.GetAsText(TargetString);
}

void FGameDNAinstallerTextLayoutMarshaller::AppendMessageToTextLayout(const TSharedPtr<FString>& InMessage)
{
	if (!IsDirty())
	{
		CachedNumMessages++;
	}

	const FTextBlockStyle& MessageTextStyle = FEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("Log.Normal");

	TSharedRef<FString> LineText = InMessage.ToSharedRef();

	TArray<TSharedRef<IRun>> Runs;
	Runs.Add(FSlateTextRun::Create(FRunInfo(), LineText, MessageTextStyle));

	TextLayout->AddLine(FSlateTextLayout::FNewLineData(MoveTemp(LineText), MoveTemp(Runs)));
}

void FGameDNAinstallerTextLayoutMarshaller::AppendMessagesToTextLayout(const TArray<TSharedPtr<FString>>& InMessages)
{
	TArray<FTextLayout::FNewLineData> LinesToAdd;
	LinesToAdd.Reserve(InMessages.Num());

	int32 NumAddedMessages = 0;

	for (const auto& CurrentMessage : InMessages)
	{
		++NumAddedMessages;

		const FTextBlockStyle& MessageTextStyle = FEditorStyle::Get().GetWidgetStyle<FTextBlockStyle>("Log.Normal");

		TSharedRef<FString> LineText = CurrentMessage.ToSharedRef();

		TArray<TSharedRef<IRun>> Runs;
		Runs.Add(FSlateTextRun::Create(FRunInfo(), LineText, MessageTextStyle));

		LinesToAdd.Emplace(MoveTemp(LineText), MoveTemp(Runs));
	}

	if (!IsDirty())
	{
		CachedNumMessages += NumAddedMessages;
	}

	TextLayout->AddLines(LinesToAdd);
}

bool FGameDNAinstallerTextLayoutMarshaller::AppendMessage(const TCHAR* InText)
{
	TArray<TSharedPtr<FString>> NewMessages;
	NewMessages.Add(MakeShareable(new FString(InText)));

	const bool bWasEmpty = Messages.Num() == 0;
	Messages.Append(NewMessages);

	if (TextLayout)
	{
		if (bWasEmpty)
		{
			TextLayout->ClearLines();
		}

		AppendMessagesToTextLayout(NewMessages);

		return true;
	}
	else
	{
		MarkMessagesCacheAsDirty();
		MakeDirty();

		return false;
	}
}

void FGameDNAinstallerTextLayoutMarshaller::MarkMessagesCacheAsDirty()
{
	bNumMessagesCacheDirty = true;
}


void FGameDNAinstallerTextLayoutMarshaller::ClearMessages()
{
	Messages.Empty();
	MakeDirty();
}

void FGameDNAinstallerTextLayoutMarshaller::CountMessages()
{
	if (!bNumMessagesCacheDirty)
	{
		return;
	}

	CachedNumMessages = 0;

	for (const auto& CurrentMessage : Messages)
	{
		CachedNumMessages++;
	}

	bNumMessagesCacheDirty = false;
}

int32 FGameDNAinstallerTextLayoutMarshaller::GetNumMessages() const
{
	return Messages.Num();
}
