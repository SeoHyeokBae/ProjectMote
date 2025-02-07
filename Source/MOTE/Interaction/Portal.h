// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../GameInfo.h"
#include "GameFramework/Actor.h"
#include "Portal.generated.h"

UCLASS()
class MOTE_API APortal : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APortal();

protected:
	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* mPortal;

	UPROPERTY(VisibleAnywhere)
	UNiagaraSystem* mPortalFXSystem;

	UPROPERTY(VisibleAnywhere)
	UNiagaraSystem* mOverlapPortalFXSystem;	
	
	UPROPERTY()
	UNiagaraComponent* mOverlapPortalFXComponent;

	UPROPERTY(VisibleAnywhere)
	UNiagaraSystem* mSmokeFXSystem;

	UPROPERTY()
	UNiagaraComponent* mSmokeFXComponent;

	float mOverlapTime = 0.f;
	bool mCanNextLevel = true;
	bool mIsPortalEnd = false;

protected:
	UPROPERTY(VisibleAnywhere)
	UParticleSystem* mPortalGolem;

	UPROPERTY(VisibleAnywhere)
	UParticleSystem* mPortalGolemExplosion;

	UPROPERTY(VisibleAnywhere)
	UParticleSystem* mPortalGolemGround;

	UPROPERTY()
	UParticleSystemComponent* mPortalParticleComp;

	AActor* mPlayer;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult &SweepResult);

	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);
};
