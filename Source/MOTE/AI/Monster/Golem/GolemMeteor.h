// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../../../GameInfo.h"
#include "GameFramework/Actor.h"
#include "NiagaraSystem.h"
#include "NiagaraComponent.h"
#include "GolemMeteor.generated.h"

UCLASS()
class MOTE_API AGolemMeteor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGolemMeteor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* mMeteorCapsule;

	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* mMeteorMovement;

	UPROPERTY(VisibleAnywhere)
	UNiagaraSystem* mMeteorFXSystem;

	UPROPERTY(VisibleAnywhere)
	UNiagaraSystem* mExplosionFireFX;

	UPROPERTY(VisibleAnywhere)
	UNiagaraSystem* mExplosionGroundFX;

	UPROPERTY(VisibleAnywhere)
	TSubclassOf<UCameraShakeBase> mCameraShake;

	UPROPERTY(VisibleAnywhere)
	UDecalComponent* mAimDecalComponent;

	UPROPERTY(VisibleAnywhere)
	UMaterialInstance* mAimMtrlInst;

	UPROPERTY(VisibleAnywhere)
	UMaterialInstanceDynamic* mAimMtrlInstDynamic;

private:
	FVector mArrivalPoint;
	float mMaxLength = 0.f;
	float mPrevLength = 0.f;
	float mDeltaLength = 0.f;

	float mMinSpeed = 2000.f;
	float mMaxSpeed = 6500.f;
	float mAccSpeed = 1000.f;
	float mAttackDmg;

protected:
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	void DrawDecal();
	void UpdateDecalSize();
};
