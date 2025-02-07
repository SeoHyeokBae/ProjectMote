// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../GameInfo.h"
#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"

UCLASS()
class MOTE_API ABaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	ABaseWeapon();

protected:
	UPROPERTY(VisibleAnywhere)
	USceneComponent* mRoot;

	//UPROPERTY(VisibleAnywhere)
	//USkeletalMeshComponent* mMesh;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* mMesh;

	UPROPERTY(VisibleAnywhere)
	UArrowComponent* mBasePoint;

	UPROPERTY(VisibleAnywhere)
	UArrowComponent* mTipPoint;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
	UArrowComponent* GetBasePoint() { return mBasePoint; }
	UArrowComponent* GetTipPoint() { return mTipPoint; }

	//USkeletalMeshComponent* GetMesh() const { return mMesh; }
	UStaticMeshComponent* GetMesh() const { return mMesh; }

	USceneComponent* GetWeapon() const { return mRoot; }
};
