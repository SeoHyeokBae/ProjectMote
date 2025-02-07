// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../GameInfo.h"
#include "InputActionValue.h"
#include "GameFramework/Character.h"
#include "../AI/AIMonsterBase.h"
#include "PlayerCharacter.generated.h"


UCLASS()
class MOTE_API APlayerCharacter : public ACharacter, public ICombatInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* mArm;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent*	mCamera;

	FRotator mCameraRotator = FRotator::ZeroRotator;

	UPROPERTY(VisibleAnywhere)
	USpotLightComponent* mLight;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* mCloak_R;

	UPROPERTY(VisibleAnywhere)
	USkeletalMeshComponent* mCloak_L;

	UPROPERTY(VisibleAnywhere)
	class ABaseWeapon*		mWeapon;

	UPROPERTY(VisibleAnywhere)
	class AMotePlayerState* mState; 

	UPROPERTY(VisibleAnywhere)
	UArrowComponent*		mArrow;

	UPROPERTY(VisibleAnywhere)
	class APlayerTrailFX*	mTrailFX;

	UPROPERTY(EditAnywhere) 
	UParticleSystem* mDashExplosionEffect;

	UPROPERTY(VisibleAnywhere)
	UDecalComponent*			mAimComponent;

	UPROPERTY(VisibleAnywhere)
	UMaterialInstance*			mAimMtrlInst;

	UPROPERTY(VisibleAnywhere)
	UMaterialInstanceDynamic*	mAimMtrlInstDynamic;
	
	UPROPERTY(VisibleAnywhere)
	UWidgetComponent*	mStaminaComponent;

	UPROPERTY(VisibleAnywhere)
	class UPlayerStaminaWidget* mStaminaWidget;

	UPROPERTY(VisibleAnywhere)
	UParticleSystem* mSpawnParticle;

	UPROPERTY(VisibleAnywhere)
	class UTargetSystemComponent* mTargetingSystem;

	UPROPERTY(EditDefaultsOnly, Category = Material)
	UMaterialInstance* mScreenDamage;

	UPROPERTY(VisibleAnywhere, Category = "ScreenEffect")
	UMaterialInstanceDynamic* mScreenDamageDynamic;

	UPROPERTY(EditDefaultsOnly, Category = Material)
	UMaterialInstance* mScreenDamageFire;

	UPROPERTY(VisibleAnywhere, Category = "ScreenEffect")
	UMaterialInstanceDynamic* mScreenDamageFireDynamic;

	UPROPERTY(EditDefaultsOnly, Category = Material)
	UMaterialInstance* mScreenDashAttack;

	UPROPERTY(VisibleAnywhere, Category = "ScreenEffect")
	UMaterialInstanceDynamic* mScreenDashAttackDynamic;

	UPROPERTY(VisibleAnywhere, Category = "ScreenEffect")
	APostProcessVolume* mScreenEffect;

	UPROPERTY(VisibleAnywhere, Category = "Gravity")
	class UCustomGravityComponent* mGravityComponent;

	UPROPERTY()
	bool mIsDead = false;

	bool IsZoomSound = false;
// ¡‹¿Œ¡‹æ∆øÙ
protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UTimelineComponent>	mZoomComponent;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UCurveFloat>			mCameraZoomCurve;

	UPROPERTY(EditDefaultsOnly)
	float mBlendTimeOnZoomIn = 0.125f;			// ¡‹¿Œ,æ∆øÙ Ω√∞£
	UPROPERTY(EditDefaultsOnly)
	float mFieldOfViewOnZoomIn = 110.f;			// ¡‹¿Œ«ﬂ¿ª ∂ß FOV ∞™
	UPROPERTY(EditDefaultsOnly)
	float mTargetArmLenghtOnZoomIn = 250.f;		// ¡‹¿Œ«ﬂ¿ª ∂ß Ω∫«¡∏µæœ ±Ê¿Ã
	UPROPERTY(EditDefaultsOnly)
	FVector mSocketOffsetOnZoomIn = FVector(0.0,150.0,-120.0); // ¡‹¿Œ«ﬂ¿ª ∂ß º“ƒœ ¿ßƒ°
	
	// ±‚¡∏∞™
	bool	mIsZoom;
	float	mTargetArmLength;
	float	mFieldOfView;
	FVector mSocketOffset;

	FOnTimelineFloat mCameraZoomHandler;

	bool IsCameraShake = false;

	const float MinCameraDist = 1700.f;
	const float MaxCameraDist = 5000.f;
	const float DefaultArmLength = 1000.f;
	const float MaxArmLength = 3000.f;
	const FVector DefaultArmLoc = FVector(0.0, 0.0, 300.0);
	const FVector MaxArmLoc = FVector(0.0, 0.0, 950.0);

	AStaticMeshActor* mPlanet;
	FVector mGravityDir = FVector(0.0f, 0.0f, -1.0f);

	bool IsLockOn = false;
	bool mIsTrunCamera = false;
	bool mIsLookUpCamera = false;
	bool mIsReadyChangeTarget = false;
	bool mGuide = false;
	bool mCanInteraction = false;
	bool mPressedInteraction = false;
	bool mIsPuase = false;

	uint8 mTeamID = 1;

	bool mIsLowHelth = false;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TSubclassOf<class UCameraShakeBase >> mCameraShake;
	TObjectPtr<class UPlayerAnimInstance>	mAnimInstance;
	
public:
	APlayerCharacter();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void PostInitializeComponents() override;


protected:
	void MoveAction(const FInputActionValue& Value);
	void RotationAction(const FInputActionValue& Value);

	void AttackAction();
	void DashAction();
	
	void JumpStartAction();
	void JumpAction();
	void FloatAction();

	void AimAction();
	void AimActionCanceled();
	void AimActionCompleted();

	void ExecuteLockOnAction();

	void BossKill();
	void ShowGuide();
	void PressInteraction();
	void PressPause();
	void Respawn();

	void LowHelthIndicate(float Percent);


protected:
	void NormalAttack();
	void DashAttack();
	void FloatDashAttack();
	void UpdateStamina(float DeltaTime);
	void TakeDamageInteraction(float DamageAmount);

	virtual void Landed(const FHitResult& Hit) override;

public:
	void GripWeapon(bool Grip);

	bool GetIsLowHelth() { return mIsLowHelth; }

	void SetScreenDamage(bool Play);
	void SetScreenFireDamage(bool Play);
	bool IsScreenDamage();
	bool IsScreenFireDamage();


	FORCEINLINE void CanInteraction() { mCanInteraction = true; }
	FORCEINLINE void CanNotInteraction() { mCanInteraction = false; }
	FORCEINLINE bool PressedInteraction() { return mPressedInteraction; }
	FORCEINLINE void SetPressedInteraction(bool Pressed) { mPressedInteraction = Pressed; }

	bool IsDead() { return mIsDead; }
public:
	bool GetIsZoom() { return mIsZoom; }
	void ZoomIn();
	void ZoomOut();

	void OffCameraShake() { IsCameraShake = false; }
	void OnCameraShake() { IsCameraShake = true; }

	FRotator GetCameraRotator() const { return mCameraRotator; }
	void SetCameraRotator(FRotator Rot) { mCameraRotator = Rot; }
	class APlayerTrailFX* GetTrailFX() { return mTrailFX; }

	void UpdateCameraView(float DeltaTime);
protected:
	UFUNCTION()
	void ZoomInByAiming(float Alpha);

public:
	USpringArmComponent*	GetArm() { return mArm; }
	UArrowComponent*		GetArrow() const { return mArrow; }
	UWidgetComponent*		GetStaminaComponent() { return mStaminaComponent; }
	ABaseWeapon*			GetWeapon() { return mWeapon; }
	FVector					GetPlanetGravityDir() const { return mGravityDir; }
	void					SetPlanetGravityDir(FVector Gravity) { mGravityDir = Gravity; }

protected:
	UFUNCTION()
	void DashOnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
	
public:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
			class AController* EventInstigator, AActor* DamageCauser);

	UFUNCTION()
	virtual void HitInteraction() override;
	UFUNCTION()
	virtual void HitStopStart() override;
	UFUNCTION()
	virtual void HitStopEnd() override { CustomTimeDilation = 1.0f; }

	void OnShield();
	void OffShield();

	bool IsMoveCamera() { return (mIsTrunCamera || mIsLookUpCamera); }

	void SetTeamID(uint8 TeamID)
	{
		mTeamID = TeamID;
	}

	virtual FGenericTeamId GetGenericTeamId() const;

private:
	void Create();
	void Load();
	void Init();

};
