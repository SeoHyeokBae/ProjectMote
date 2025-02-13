// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TargetSystemComponent.generated.h"

UENUM(BlueprintType)
enum class ETargetDirection : uint8
{
	TargetDirection_Left		UMETA(DisplayName = "Left"),
	TargetDirection_Right		UMETA(DisplayName = "Right")
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MOTE_API UTargetSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTargetSystemComponent();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#pragma region Debug
public:
	bool IsDrawDebug() const;

private:
	void UpdateDrawDebug();

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TargetingSystem|Debug", Meta = (AllowPrivateAccess = "true"))
	bool bDrawDebug;

	UPROPERTY(EditAnywhere, Category = "TargetingSystem|Debug")
	float DebugTargetSphereSize;

	UPROPERTY(EditAnywhere, Category = "TargetingSystem|Debug")
	float DebugTargetSphereSegment;

public:
	UFUNCTION(BlueprintCallable, Category = "TargetingSystem|Debug")
	void SetDrawDebug(bool bFlag);
#pragma endregion 

#pragma region PlayerReference
private:
	void InitializePlayerReference();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TargetingSystem|PlayerReference", Meta = (AllowPrivateAccess = "true"))
	class APlayerCharacter* mPlayerOwner;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TargetingSystem|PlayerReference", Meta = (AllowPrivateAccess = "true"))
	class AMotePlayerController* mPlayerController;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TargetingSystem|PlayerReference", Meta = (AllowPrivateAccess = "true"))
	class APlayerCameraManager* mPlayerCameraManager;
#pragma endregion

#pragma region TargetingSystem
public:
	bool IsLockOnTarget() const;
	bool IsDynamicLockOnTarget() const;
	
	void ExecuteLockOnTarget();
	void CancelLockOnTarget();
	
	void HoldTargetLockOn();
	void HoldTargetLockOff();

	void ChangeLockOnTargetForTurnValue(float TurnValue);

private:
	void UpdateCamera(float DeltaTime);
	void UpdateCameraLock();
	void UpdateDynamicCameraLock(float DeltaTime);
	void EnableCameraLock();
	void DisableCameraLock();

	void SetTarget(AActor* NewTarget);
	AActor* FindTarget() const;
	
	TArray<AActor*> SearchTargetableActors(bool bInScreenPosition = true) const;

	AActor* FindDirectionalTarget(ETargetDirection NewTargetDirection);

	ETargetDirection WhichSideOfTarget(AActor* NewTargetableActor) const;

	// ���� �������� Target�� ������, bIsLargestDot ���� ũ�� true, ���� ������ false
	AActor* GetTargetByDotProduct(TArray<AActor*> TargetableActors, bool bIsLargestDot);

	float CalculateDotProductToTarget(AActor* NewTargetableActor) const;

	// 3D���͸� 2Dȭ�� ��ġ��, ȭ�鿡 ���� ���� bool 
	TTuple<FVector2D, bool> GetScreenPositionOfActor(AActor* SearchActor) const;

	// ���Ͱ� ����Ʈ �ȿ� �ִ���, ScreenRatio �� ���� �ش� ����ŭ �����¿� ������ ���� (0.25f �̸� �������� 0.25f��ŭ ���)
	bool IsInViewport(FVector2D ActorScreenPosition, float ScreenRatio = 0.0f) const;

	// Ÿ�� ���ͷ� ȸ������
	FRotator CalculateInterpToTarget(AActor* InterpToTarget) const;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TargetingSystem", Meta = (AllowPrivateAccess = "true"))
	bool bIsLockOnTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TargetingSystem", Meta = (AllowPrivateAccess = "true"))
	bool bDynamicLockOnTarget;

	// DynamicLockOn�� �����ϴ� ȭ�� ������
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TargetingSystem", Meta = (AllowPrivateAccess = "true"))
	float StopDynamicCameraLockScreenRatio;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TargetingSystem", Meta = (AllowPrivateAccess = "true"))
	AActor* Target;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TargetingSystem", Meta = (AllowPrivateAccess = "true"))
	float MaxSearchTargetableDistance;

	// Target�� LockOn�� �� Target�� �ٶ󺸴� �����ӵ�
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TargetingSystem", Meta = (AllowPrivateAccess = "true"))
	float InterpSpeed;

	
	// ���������� Target�� ������ �ð�
	UPROPERTY(BlueprintReadWrite, Category = "TargetingSystem|MouseInput", Meta = (AllowPrivateAccess = "true"))
	float LastTimeChangeTarget;

	// ���������� Target�� ������ ������ �ð�
	UPROPERTY(BlueprintReadWrite, Category = "TargetingSystem|MouseInput", Meta = (AllowPrivateAccess = "true"))
	float TimeSinceLastChangeTarget;

	// Target�� �����ϱ� ���� �ʿ��� ���콺�� �ּ� ȸ�� �Է� ��
	UPROPERTY(BlueprintReadWrite, Category = "TargetingSystem|MouseInput", Meta = (AllowPrivateAccess = "true"))
	float MinMouseValueToChangeTarget;

	// ���콺�� Target�� ������ �� ������
	UPROPERTY(BlueprintReadWrite, Category = "TargetingSystem|MouseInput", Meta = (AllowPrivateAccess = "true"))
	float MinDelayTimeToChangeTarget;

	// ���콺ȸ�� ������ Ÿ���� ���
	UPROPERTY(BlueprintReadWrite, Category = "TargetingSystem|MouseInput", Meta = (AllowPrivateAccess = "true"))
	float MinCancelToTarget;

public:
	void SetDynamicLockOnTarget(bool bFlag);

	AActor* GetTarget() const;
#pragma endregion 

};
