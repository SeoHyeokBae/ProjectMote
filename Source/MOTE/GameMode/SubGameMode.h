// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../GameInfo.h"
#include "GameFramework/GameModeBase.h"
#include "SubGameMode.generated.h"

/**
 * 
 */
UCLASS()
class MOTE_API ASubGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ASubGameMode();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite) // 에디터 및 블루프린트에서 값 수정가능
		int32   mScore = 0;

protected:
	virtual void BeginPlay() override;

public:
	void AddScore(int32 Score)
	{
		mScore += Score;
	}

	int32 GetScore()
	{
		return mScore;
	}
};
