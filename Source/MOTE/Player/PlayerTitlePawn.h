// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../GameInfo.h"
#include "GameFramework/Pawn.h"
#include "PlayerTitlePawn.generated.h"

UCLASS()
class MOTE_API APlayerTitlePawn : public APawn
{
	GENERATED_BODY()

public:
	APlayerTitlePawn();

public:
	UPROPERTY(BlueprintReadWrite)
	USkeletalMeshComponent* mMesh;
private:

	UPROPERTY(VisibleAnywhere)
	UFloatingPawnMovement* mMovement;

	UPROPERTY(VisibleAnywhere)
	UArrowComponent* mArrow;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* mArm;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* mCamera;

	UPROPERTY(VisibleAnywhere)
	USpotLightComponent* mLight;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* mCloak_R;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* mCloak_L;

	UPROPERTY(EditAnywhere)
	TObjectPtr<AActor>	mStartCamera;

	UPROPERTY(VisibleAnywhere)
	UNiagaraSystem* mEffect;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:
	TObjectPtr<AActor> GetCamera() {return mStartCamera;}

	FORCEINLINE USkeletalMeshComponent* GetMesh() { return mMesh; }
	void EffectOn();

};
