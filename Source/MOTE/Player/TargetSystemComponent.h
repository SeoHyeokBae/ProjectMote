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

	// 내적 기준으로 Target을 가져옴, bIsLargestDot 가장 크면 true, 가장 작으면 false
	AActor* GetTargetByDotProduct(TArray<AActor*> TargetableActors, bool bIsLargestDot);

	float CalculateDotProductToTarget(AActor* NewTargetableActor) const;

	// 3D액터를 2D화면 위치로, 화면에 존재 여부 bool 
	TTuple<FVector2D, bool> GetScreenPositionOfActor(AActor* SearchActor) const;

	// 액터가 뷰포트 안에 있는지, ScreenRatio 의 값은 해당 값만큼 상하좌우 떨어진 비율 (0.25f 이면 안쪽으로 0.25f만큼 축소)
	bool IsInViewport(FVector2D ActorScreenPosition, float ScreenRatio = 0.0f) const;

	// 타켓 액터로 회전보간
	FRotator CalculateInterpToTarget(AActor* InterpToTarget) const;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TargetingSystem", Meta = (AllowPrivateAccess = "true"))
	bool bIsLockOnTarget;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TargetingSystem", Meta = (AllowPrivateAccess = "true"))
	bool bDynamicLockOnTarget;

	// DynamicLockOn을 중지하는 화면 비율을
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TargetingSystem", Meta = (AllowPrivateAccess = "true"))
	float StopDynamicCameraLockScreenRatio;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TargetingSystem", Meta = (AllowPrivateAccess = "true"))
	AActor* Target;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TargetingSystem", Meta = (AllowPrivateAccess = "true"))
	float MaxSearchTargetableDistance;

	// Target을 LockOn할 때 Target을 바라보는 보간속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TargetingSystem", Meta = (AllowPrivateAccess = "true"))
	float InterpSpeed;

	
	// 마지막으로 Target을 변경한 시간
	UPROPERTY(BlueprintReadWrite, Category = "TargetingSystem|MouseInput", Meta = (AllowPrivateAccess = "true"))
	float LastTimeChangeTarget;

	// 마지막으로 Target을 변경한 이후의 시간
	UPROPERTY(BlueprintReadWrite, Category = "TargetingSystem|MouseInput", Meta = (AllowPrivateAccess = "true"))
	float TimeSinceLastChangeTarget;

	// Target을 변경하기 위해 필요한 마우스의 최소 회전 입력 값
	UPROPERTY(BlueprintReadWrite, Category = "TargetingSystem|MouseInput", Meta = (AllowPrivateAccess = "true"))
	float MinMouseValueToChangeTarget;

	// 마우스로 Target을 변경할 때 딜레이
	UPROPERTY(BlueprintReadWrite, Category = "TargetingSystem|MouseInput", Meta = (AllowPrivateAccess = "true"))
	float MinDelayTimeToChangeTarget;

	// 마우스회전 값으로 타켓팅 취소
	UPROPERTY(BlueprintReadWrite, Category = "TargetingSystem|MouseInput", Meta = (AllowPrivateAccess = "true"))
	float MinCancelToTarget;

public:
	void SetDynamicLockOnTarget(bool bFlag);

	AActor* GetTarget() const;
#pragma endregion 

};
