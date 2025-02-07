// Fill out your copyright notice in the Description page of Project Settings.


#include "SubGameMode.h"
#include "AI/MonsterDataManager.h"
#include "Player/PlayerCharacter.h"
#include "Player/MotePlayerState.h"
#include "GameMode/MoteGameInstance.h"
#include "Controller/MotePlayerController.h"
#include "Player/PlayerAnimInstance.h"
#include "../Sound/SoundSubsystem.h"

ASubGameMode::ASubGameMode()
{
	DefaultPawnClass = APlayerCharacter::StaticClass();
	PlayerStateClass = AMotePlayerState::StaticClass();
	PlayerControllerClass = AMotePlayerController::StaticClass();
	CMonsterDataManager::GetInst()->Init();
}

void ASubGameMode::BeginPlay()
{
	AMotePlayerController* PlayerController = Cast<AMotePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	if (IsValid(PlayerController))
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(PlayerController->GetCharacter());
		if (Player)
		{
			Player->GetArm()->SetRelativeRotation(FRotator(-5.0, 90.0, 0.0));
			Player->GetArm()->SetRelativeLocation(FVector(0.0, 0.0, 300.0));
			Player->GetArm()->TargetArmLength = 1000.f;
			Player->GetArm()->CameraLagSpeed = 0.0f;

			// Starting Pose
			Cast<UPlayerAnimInstance>(Player->GetMesh()->GetAnimInstance())->OnStartPose();
		}
	}

	USoundSubsystem* BGM = GetGameInstance()->GetSubsystem<USoundSubsystem>();
	if (BGM)
	{
		BGM->PlayBGM(TEXT("TutorialBGM"), 0.75f, 5.f);
	}
}
