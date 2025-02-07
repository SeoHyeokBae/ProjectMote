// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameMode.h"
#include "AI/MonsterDataManager.h"
#include "Player/PlayerCharacter.h"
#include "Player/MotePlayerState.h"
#include "GameMode/MoteGameInstance.h"
#include "Controller/MotePlayerController.h"

AMainGameMode::AMainGameMode()
{
	DefaultPawnClass = APlayerCharacter::StaticClass();
	PlayerStateClass = AMotePlayerState::StaticClass();
	PlayerControllerClass = AMotePlayerController::StaticClass();
	CMonsterDataManager::GetInst()->Init();
	
}

void AMainGameMode::BeginPlay()
{
	AMotePlayerController* PlayerController = Cast<AMotePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (IsValid(PlayerController))
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(PlayerController->GetCharacter());
		if (Player)
		{

		}
	}
}