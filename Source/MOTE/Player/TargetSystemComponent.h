// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TargetSystemComponent.generated.h"

/**
 * 현재 Target에서 다음 Target이 존재하는 방향을 나타내는 enum 클래스입니다.
 */
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
	/** 디버그를 출력하는지 나타내는 함수입니다. */
	bool IsDrawDebug() const;

private:
	/** 디버그를 최신화 하는 함수입니다. */
	void UpdateDrawDebug();

private:
	/** 디버그의 출력을 결정하는 변수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TargetingSystem|Debug", Meta = (AllowPrivateAccess = "true"))
	bool bDrawDebug;

	/** Target으로 지정이 가능한 액터(초록색)와 Target(붉은색)을 나타내는 디버그 스피어의 크기입니다. */
	UPROPERTY(EditAnywhere, Category = "TargetingSystem|Debug")
	float DebugTargetSphereSize;

	/** 디버그 스피어의 정밀도(높을수록 완벽한 구)입니다. */
	UPROPERTY(EditAnywhere, Category = "TargetingSystem|Debug")
	float DebugTargetSphereSegment;

public:
	/**
	 * 디버그의 출력을 설정하는 함수입니다.
	 * @param bFlag true일 경우 디버그를 출력하고 false일 경우 디버그를 출력하지 않습니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "TargetingSystem|Debug")
	void SetDrawDebug(bool bFlag);
#pragma endregion 

#pragma region PlayerReference
private:
	/** 플레이어 레퍼런스들을 초기화하는 함수입니다. */
	void InitializePlayerReference();

private:
	/** 이 Component를 사용하는 Owner를 PTPlayerCharacter 클래스로 캐스팅한 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TargetingSystem|PlayerReference", Meta = (AllowPrivateAccess = "true"))
	class APlayerCharacter* mPlayerOwner;

	/** 플레이어가 사용하는 플레이어 컨트롤러입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TargetingSystem|PlayerReference", Meta = (AllowPrivateAccess = "true"))
	class AMotePlayerController* mPlayerController;

	/** 플레이어가 사용하는 플레이어 카메라 매니저입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TargetingSystem|PlayerReference", Meta = (AllowPrivateAccess = "true"))
	class APlayerCameraManager* mPlayerCameraManager;
#pragma endregion

#pragma region TargetingSystem
public:
	/** Target을 LockOn 중인지 나타내는 함수입니다. */
	bool IsLockOnTarget() const;

	/** Target을 DynamicLockOn 중인지 나타내는 함수입니다. */
	bool IsDynamicLockOnTarget() const;

	/** LockOnTarget을 실행하는 함수입니다. */
	void ExecuteLockOnTarget();

	/** LockOnTarget을 취소하는 함수입니다. */
	void CancelLockOnTarget();

	// 타켓 유지한채로 락온/오프
	void HoldTargetLockOn();

	void HoldTargetLockOff();

	/**
	 * 입력하는 장치와 캐릭터의 카메라 회전 값에 따라 LockOn하는 Target을 변경합니다.
	 * @param InputMode 입력 장치입니다.
	 * @param TurnValue 카메라 회전 값입니다.
	 */
	void ChangeLockOnTargetForTurnValue(float TurnValue);

private:
	/** 카메라를 최신화하는 함수입니다. */
	void UpdateCamera(float DeltaTime);

	/** LockOn하는 Target에 카메라를 고정하는 것을 최신화하는 함수입니다. */
	void UpdateCameraLock();

	/** LockOn하는 Target에 카메라를 동적으로 고정하는 것을 최신화하는 함수입니다. */
	void UpdateDynamicCameraLock(float DeltaTime);

	/** Target에 카메라를 고정하는 함수입니다. */
	void EnableCameraLock();

	/** Target에 고정된 카메라의 고정을 해제하는 함수입니다. */
	void DisableCameraLock();

	/**
	 * 입력받은 인자를 Target으로 설정하는 함수입니다.
	 * @param NewTarget Target으로 설정할 액터입니다.
	 */
	void SetTarget(AActor* NewTarget);

	/** Target으로 지정이 가능한 액터들을 탐색하고 탐색한 액터들 중 화면의 중앙에 존재하는 액터를 찾아 반환하는 함수입니다. */
	AActor* FindTarget() const;

	/**
	 * Target으로 지정이 가능한 액터들을 탐색하여 Array에 저장하고 반환하는 함수입니다.
	 * @param bInScreenPosition true일 경우 화면 안에 존재하는 액터들만 탐색합니다. false일 경우 탐색범위 내에 있는 모든 액터를 탐색합니다.
	 * @return Target으로 지정이 가능한 액터들을 저장한 Array입니다.
	 */
	TArray<AActor*> SearchTargetableActors(bool bInScreenPosition = true) const;

	/**
	 * 현재 Target을 기준으로 입력된 방향에 있는 새로운 Target을 찾아 반환하는 함수입니다.
	 * @param NewTargetDirection 새로운 Target을 찾을 방향입니다.
	 */
	AActor* FindDirectionalTarget(ETargetDirection NewTargetDirection);

	/**
	 * 인자로 받은 NewTargetableActor가 플레이어 카메라에서 Target을 기준으로 왼쪽에 있는지 오른쪽에 있는지 반환하는 함수입니다.
	 * @param NewTargetableActor 위치를 파악할 액터입니다.
	 */
	ETargetDirection WhichSideOfTarget(AActor* NewTargetableActor) const;

	/**
	 * 내적을 기준으로 새로운 Target을 가져오는 함수입니다.
	 * @param TargetableActors Target으로 지정이 가능한 액터들입니다.
	 * @param bIsLargestDot true일 경우 가장 큰 내적, false일 경우 가장 작은 내적을 기준으로 사용합니다.
	 */
	AActor* GetTargetByDotProduct(TArray<AActor*> TargetableActors, bool bIsLargestDot);

	/**
	 * 플레이어 캐릭터에서 Target으로 향하는 벡터와 플레이어 캐릭터에서 인자로 받은 NewTargetableActor로 향하는 벡터와
	 * 내적을 계산하는 함수입니다.
	 * @param NewTargetableActor 벡터를 비교할 액터입니다.
	 */
	float CalculateDotProductToTarget(AActor* NewTargetableActor) const;

	/**
	 * 인자로 받은 액터의 화면에서의 위치와 해당 액터가 화면에 존재하는지를 반환하는 함수입니다.
	 * @param SearchActor 화면에서의 위치를 찾을 액터입니다.
	 * @return Get<0> 인자로 받은 액터의 화면에서의 위치입니다.
	 * @return Get<1> 인자로 받은 액터가 화면에 존재하는지를 나타냅니다.
	 */
	TTuple<FVector2D, bool> GetScreenPositionOfActor(AActor* SearchActor) const;

	// /**
	//  * 화면 상의 액터의 위치가 뷰포트에 존재하는 판단하는 함수입니다.
	//  * @param ActorScreenPosition 화면 상의 액터의 위치입니다.
	//  */
	// bool IsInViewport(FVector2D ActorScreenPosition) const;

	/**
	 * 화면상의 액터의 위치가 뷰포트에 존재하는지 나타내는 함수입니다.
	 * ScreenRatio의 값이 0.0f이 아닐 경우 뷰포트 중앙에서 해당 값의 화면크기의 비율만큼의 범위에 액터가 존재하는지 나타냅니다.
	 * @param ActorScreenPosition 화면에서의 액터의 위치입니다.
	 * @param ScreenRatio 화면 크기의 비율을 나타내는 값입니다. 0.0f일 경우 뷰포트의 최대크기를 사용하며 0.2f일 경우 중앙에서 뷰포트의 상하좌우를 0.2f 비율만큼 줄인 크기를 사용합니다.
	 * @return 액터가 뷰포트에 존재하는지 나타냅니다.
	 */
	bool IsInViewport(FVector2D ActorScreenPosition, float ScreenRatio = 0.0f) const;

	/**
	 * 인자로 받은 액터를 바라보는 회전 보간 값을 계산하는 함수입니다.
	 * @param InterpToTarget 바라볼 액터입니다.
	 * @return 인자로 받은 액터를 바라보는 회전 값입니다.
	 */
	FRotator CalculateInterpToTarget(AActor* InterpToTarget) const;

private:
	/** Target을 LockOn 중인지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TargetingSystem", Meta = (AllowPrivateAccess = "true"))
	bool bIsLockOnTarget;

	/** Target을 LockOn하면서 카메라를 자유롭게 조작하는 DynamicLockOn 중인지 나타내는 변수입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TargetingSystem", Meta = (AllowPrivateAccess = "true"))
	bool bDynamicLockOnTarget;

	/** DynamicLockOn을 중지하는 화면 비율을 나타내는 변수입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TargetingSystem", Meta = (AllowPrivateAccess = "true"))
	float StopDynamicCameraLockScreenRatio;

	/** LockOn하는 Target입니다. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TargetingSystem", Meta = (AllowPrivateAccess = "true"))
	AActor* Target;

	/** Target으로 지정이 가능한 액터들을 탐색하는 최대 탐색 범위입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TargetingSystem", Meta = (AllowPrivateAccess = "true"))
	float MaxSearchTargetableDistance;

	/** Target을 LockOn할 때 Target을 바라보는 보간속도입니다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TargetingSystem", Meta = (AllowPrivateAccess = "true"))
	float InterpSpeed;

	/**
	 * 마지막으로 Target을 변경한 시간입니다.
	 * 마우스로 Target을 변경할 때 사용합니다.
	 */
	UPROPERTY(BlueprintReadWrite, Category = "TargetingSystem|MouseInput", Meta = (AllowPrivateAccess = "true"))
	float LastTimeChangeTarget;

	/**
	 * 마지막으로 Target을 변경한 이후의 시간입니다.
	 * 마우스로 Target을 변경할 때 사용합니다.
	 */
	UPROPERTY(BlueprintReadWrite, Category = "TargetingSystem|MouseInput", Meta = (AllowPrivateAccess = "true"))
	float TimeSinceLastChangeTarget;

	/**
	 * Target을 변경하기 위해 필요한 마우스의 최소 회전 입력 값입니다.
	 * 마우스로 Target을 변경할 때 사용합니다.
	 */
	UPROPERTY(BlueprintReadWrite, Category = "TargetingSystem|MouseInput", Meta = (AllowPrivateAccess = "true"))
	float MinMouseValueToChangeTarget;

	/**
	 * 다른 Target으로 변경하기 전에 딜레이를 주는 시간입니다.
	 * 너무 빠르게 다른 Target으로 변경하는 것을 방지합니다.
	 * 마우스로 Target을 변경할 때 사용합니다.
	 */
	UPROPERTY(BlueprintReadWrite, Category = "TargetingSystem|MouseInput", Meta = (AllowPrivateAccess = "true"))
	float MinDelayTimeToChangeTarget;

	/**
	 * 마지막으로 Target을 변경한 후 게임패드의 아날로그 스틱이 중립이 되었는지 나타내는 변수입니다.
	 * 게임패드의 아날로그 스틱이 중립일 경우 true로 설정됩니다.
	 * 게임패드로 Target을 변경할 때 사용합니다.
	 */
	UPROPERTY(BlueprintReadWrite, Category = "TargetingSystem|GamepadInput", Meta = (AllowPrivateAccess = "true"))
	bool bAnalogNeutralSinceLastSwitchTarget;

	/**
	 * Target을 변경하기 위해 필요한 아날로그 스틱의 최소 입력 값입니다.
	 * 게임패드로 Target을 변경할 때 사용합니다.
	 */
	UPROPERTY(BlueprintReadWrite, Category = "TargetingSystem|GamepadInput", Meta = (AllowPrivateAccess = "true"))
	float MinAnalogValueToChangeTarget;

public:
	/**
	 * 입력받은 인자로 bDynamicLockOnCamera를 설정하는 함수입니다.
	 * @param bFlag bDynamicLockOnCamera를 설정하는 매개변수입니다.
	 */
	void SetDynamicLockOnTarget(bool bFlag);

	/** Target을 반환하는 함수입니다. */
	AActor* GetTarget() const;
#pragma endregion 

};
