// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../../../GameInfo.h"
#include "GameFramework/Actor.h"
#include "MonsterSpawner.generated.h"

UCLASS()
class MOTE_API AMonsterSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMonsterSpawner();

protected:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* mRoot;

	UPROPERTY(VisibleAnywhere)
	UParticleSystem* ElementalSpawnEffect;

#if WITH_EDITORONLY_DATA
	UPROPERTY()
	TObjectPtr<UArrowComponent> ArrowComponent;
#endif
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AAICharacterBase>	mSpawnClass;

	class AAICharacterBase* mSpawnActor = nullptr;

	int32 mCount = 0;

	float mTimeAcc = 0.f;
	
	UPROPERTY(EditAnywhere, Category = InitSetting)
	float mInterval = 2.5f;

	UPROPERTY(EditAnywhere, Category = InitSetting)
	float mSpawnRadius = 5000.f;

	UPROPERTY(EditAnywhere, Category = InitSetting)
	int32 mMaxSpawn = 4;

	FTimerHandle StartTimerHandle;
	FTimerHandle SpawnTimerHandle;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	void SpawnRandomLocation();

};
