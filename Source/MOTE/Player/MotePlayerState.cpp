// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/MotePlayerState.h"
#include "GameMode/MoteGameInstance.h"
#include "../UI/ETC/DmgTextActor.h"

UDataTable* AMotePlayerState::mPlayerInfoTable = nullptr;

AMotePlayerState::AMotePlayerState()
{
	if (!mPlayerInfoTable)
	{
		static ConstructorHelpers::FObjectFinder<UDataTable>
			InfoTableAsset(TEXT("/Script/Engine.DataTable'/Game/Player/Data/DT_PlayerInfo.DT_PlayerInfo'"));

		if (InfoTableAsset.Succeeded())
			mPlayerInfoTable = InfoTableAsset.Object;
	}
}

void AMotePlayerState::BeginPlay()
{
	Super::BeginPlay();

	UMoteGameInstance* GameInst = GetWorld()->GetGameInstance<UMoteGameInstance>();

	if (IsValid(GameInst))
	{
		FString	PlayerName = GameInst->GetPlayerName();
		mName = PlayerName;
	}

	if (IsValid(mPlayerInfoTable))
	{
		FName	RowName;
		RowName = FName(mName);

		const FPlayerInfoTable* Info =
			mPlayerInfoTable->FindRow<FPlayerInfoTable>(RowName, TEXT(""));

		if (Info)
		{
			mAttack = Info->mAttack;
			mDefense = Info->mDefense;
			mHP = Info->mHP;
			mHPMax = Info->mHP;
			mStamina = Info->mStamina;
			mStaminaMax = Info->mStamina;
			mMoveSpeed = Info->mMoveSpeed;
			mAttackDistance = Info->mAttackDistance;
		}
	}
}

void AMotePlayerState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AMotePlayerState::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

bool AMotePlayerState::SetHP(float Amount, bool IsIncrease, bool IsVisible)
{
	// 데미지 표시
	if (IsVisible && Cast<APlayerController>(Owner)->GetCharacter())
	{
		FActorSpawnParameters SpawnParams;
		ADmgTextActor* DmgText = GetWorld()->SpawnActor<ADmgTextActor>(ADmgTextActor::StaticClass(), Cast<APlayerController>(Owner)->GetCharacter()->GetActorTransform(), SpawnParams);
		DmgText->InputDamage(Amount);
		if (IsIncrease)
			DmgText->ChangeColor(FLinearColor::Green);
	}

	if (IsIncrease)
	{
		mHP += Amount;
	}
	else
	{
		mHP -= Amount;
	}

	bool result = false; // true == Dead

	if (mHP >= mHPMax)
		mHP = mHPMax;

	if (mHP <= 0.f)
	{
		mHP = 0.f;
		result = true; // deplete all, ToDo Groggy Status
	}


	if (mHPDelegate.IsBound())
	{
		mHPDelegate.Broadcast(mHP/ mHPMax);
	}
	
	return result;
}

bool AMotePlayerState::SetStamina(float Amount, bool IsIncrease)
{
	if (IsIncrease)
	{
		mStamina += Amount;
	}
	else
	{
		mStamina -= Amount;
	}

	bool result = false;

	if (mStamina >= mStaminaMax)
		mStamina = mStaminaMax;

	if (mStamina <= 0.f)
	{
		mStamina = 0.f;
		result = true; // deplete all, ToDo Groggy Status
	}

	if (mStaminaDelegate.IsBound())
	{
		mStaminaDelegate.Broadcast(mStamina);
	}

	return result;
}

