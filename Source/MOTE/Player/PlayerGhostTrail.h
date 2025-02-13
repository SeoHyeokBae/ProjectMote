// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../GameInfo.h"
#include "GameFramework/Actor.h"
#include "PlayerGhostTrail.generated.h"

UCLASS()
class MOTE_API APlayerGhostTrail : public AActor
{
	GENERATED_BODY()
	
public:	
	APlayerGhostTrail();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
	void Init(USkeletalMeshComponent* SkeletalMeshComponent);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UPoseableMeshComponent* mPoseableMeshComponent;

	UMaterialInstance* mMtrlInstance;
	TArray<UMaterialInstanceDynamic*> mMaterials;

	bool	mIsSpawned = false;
	float	mFadeCountDown;
	float	mFadeOutTime = 0.5f;

};
