// Fill out your copyright notice in the Description page of Project Settings.


#include "TitleGameMode.h"
#include "Controller/MoteTitleController.h"
#include "../Player/PlayerTitlePawn.h"
#include "../Sound/SoundSubsystem.h"

ATitleGameMode::ATitleGameMode()
{
	DefaultPawnClass = nullptr;
	DefaultPawnClass = APlayerTitlePawn::StaticClass();
	PlayerControllerClass = AMoteTitleController::StaticClass();
}

void ATitleGameMode::BeginPlay()
{
	Super::BeginPlay();

	USoundSubsystem* BGM = GetGameInstance()->GetSubsystem<USoundSubsystem>();
	if (BGM)
	{
		BGM->PlayBGM(TEXT("TitleBGM"), 0.4f, 12.5f);
	}
}
