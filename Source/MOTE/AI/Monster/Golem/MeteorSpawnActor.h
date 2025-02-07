// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../../../GameInfo.h"
#include "GameFramework/Actor.h"
#include "MeteorSpawnActor.generated.h"

UCLASS()
class MOTE_API AMeteorSpawnActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AMeteorSpawnActor();

protected:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* mRoot;

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	TObjectPtr<UArrowComponent> ArrowComponent;
#endif

	int32 mCount = 0;
	int32 mMax = 4;
	float mTimeAcc = 1.5f;
	float mInterval = 1.5f;


protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
	void RageMode();

protected:
	void SpawnRandomLocation();
	void SpawnRandomLocationNoNav();

};
