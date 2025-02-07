// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TargetSystemComponent.generated.h"

/**
 * ���� Target���� ���� Target�� �����ϴ� ������ ��Ÿ���� enum Ŭ�����Դϴ�.
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
	/** ����׸� ����ϴ��� ��Ÿ���� �Լ��Դϴ�. */
	bool IsDrawDebug() const;

private:
	/** ����׸� �ֽ�ȭ �ϴ� �Լ��Դϴ�. */
	void UpdateDrawDebug();

private:
	/** ������� ����� �����ϴ� �����Դϴ�. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TargetingSystem|Debug", Meta = (AllowPrivateAccess = "true"))
	bool bDrawDebug;

	/** Target���� ������ ������ ����(�ʷϻ�)�� Target(������)�� ��Ÿ���� ����� ���Ǿ��� ũ���Դϴ�. */
	UPROPERTY(EditAnywhere, Category = "TargetingSystem|Debug")
	float DebugTargetSphereSize;

	/** ����� ���Ǿ��� ���е�(�������� �Ϻ��� ��)�Դϴ�. */
	UPROPERTY(EditAnywhere, Category = "TargetingSystem|Debug")
	float DebugTargetSphereSegment;

public:
	/**
	 * ������� ����� �����ϴ� �Լ��Դϴ�.
	 * @param bFlag true�� ��� ����׸� ����ϰ� false�� ��� ����׸� ������� �ʽ��ϴ�.
	 */
	UFUNCTION(BlueprintCallable, Category = "TargetingSystem|Debug")
	void SetDrawDebug(bool bFlag);
#pragma endregion 

#pragma region PlayerReference
private:
	/** �÷��̾� ���۷������� �ʱ�ȭ�ϴ� �Լ��Դϴ�. */
	void InitializePlayerReference();

private:
	/** �� Component�� ����ϴ� Owner�� PTPlayerCharacter Ŭ������ ĳ������ �����Դϴ�. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TargetingSystem|PlayerReference", Meta = (AllowPrivateAccess = "true"))
	class APlayerCharacter* mPlayerOwner;

	/** �÷��̾ ����ϴ� �÷��̾� ��Ʈ�ѷ��Դϴ�. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TargetingSystem|PlayerReference", Meta = (AllowPrivateAccess = "true"))
	class AMotePlayerController* mPlayerController;

	/** �÷��̾ ����ϴ� �÷��̾� ī�޶� �Ŵ����Դϴ�. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TargetingSystem|PlayerReference", Meta = (AllowPrivateAccess = "true"))
	class APlayerCameraManager* mPlayerCameraManager;
#pragma endregion

#pragma region TargetingSystem
public:
	/** Target�� LockOn ������ ��Ÿ���� �Լ��Դϴ�. */
	bool IsLockOnTarget() const;

	/** Target�� DynamicLockOn ������ ��Ÿ���� �Լ��Դϴ�. */
	bool IsDynamicLockOnTarget() const;

	/** LockOnTarget�� �����ϴ� �Լ��Դϴ�. */
	void ExecuteLockOnTarget();

	/** LockOnTarget�� ����ϴ� �Լ��Դϴ�. */
	void CancelLockOnTarget();

	// Ÿ�� ������ä�� ����/����
	void HoldTargetLockOn();

	void HoldTargetLockOff();

	/**
	 * �Է��ϴ� ��ġ�� ĳ������ ī�޶� ȸ�� ���� ���� LockOn�ϴ� Target�� �����մϴ�.
	 * @param InputMode �Է� ��ġ�Դϴ�.
	 * @param TurnValue ī�޶� ȸ�� ���Դϴ�.
	 */
	void ChangeLockOnTargetForTurnValue(float TurnValue);

private:
	/** ī�޶� �ֽ�ȭ�ϴ� �Լ��Դϴ�. */
	void UpdateCamera(float DeltaTime);

	/** LockOn�ϴ� Target�� ī�޶� �����ϴ� ���� �ֽ�ȭ�ϴ� �Լ��Դϴ�. */
	void UpdateCameraLock();

	/** LockOn�ϴ� Target�� ī�޶� �������� �����ϴ� ���� �ֽ�ȭ�ϴ� �Լ��Դϴ�. */
	void UpdateDynamicCameraLock(float DeltaTime);

	/** Target�� ī�޶� �����ϴ� �Լ��Դϴ�. */
	void EnableCameraLock();

	/** Target�� ������ ī�޶��� ������ �����ϴ� �Լ��Դϴ�. */
	void DisableCameraLock();

	/**
	 * �Է¹��� ���ڸ� Target���� �����ϴ� �Լ��Դϴ�.
	 * @param NewTarget Target���� ������ �����Դϴ�.
	 */
	void SetTarget(AActor* NewTarget);

	/** Target���� ������ ������ ���͵��� Ž���ϰ� Ž���� ���͵� �� ȭ���� �߾ӿ� �����ϴ� ���͸� ã�� ��ȯ�ϴ� �Լ��Դϴ�. */
	AActor* FindTarget() const;

	/**
	 * Target���� ������ ������ ���͵��� Ž���Ͽ� Array�� �����ϰ� ��ȯ�ϴ� �Լ��Դϴ�.
	 * @param bInScreenPosition true�� ��� ȭ�� �ȿ� �����ϴ� ���͵鸸 Ž���մϴ�. false�� ��� Ž������ ���� �ִ� ��� ���͸� Ž���մϴ�.
	 * @return Target���� ������ ������ ���͵��� ������ Array�Դϴ�.
	 */
	TArray<AActor*> SearchTargetableActors(bool bInScreenPosition = true) const;

	/**
	 * ���� Target�� �������� �Էµ� ���⿡ �ִ� ���ο� Target�� ã�� ��ȯ�ϴ� �Լ��Դϴ�.
	 * @param NewTargetDirection ���ο� Target�� ã�� �����Դϴ�.
	 */
	AActor* FindDirectionalTarget(ETargetDirection NewTargetDirection);

	/**
	 * ���ڷ� ���� NewTargetableActor�� �÷��̾� ī�޶󿡼� Target�� �������� ���ʿ� �ִ��� �����ʿ� �ִ��� ��ȯ�ϴ� �Լ��Դϴ�.
	 * @param NewTargetableActor ��ġ�� �ľ��� �����Դϴ�.
	 */
	ETargetDirection WhichSideOfTarget(AActor* NewTargetableActor) const;

	/**
	 * ������ �������� ���ο� Target�� �������� �Լ��Դϴ�.
	 * @param TargetableActors Target���� ������ ������ ���͵��Դϴ�.
	 * @param bIsLargestDot true�� ��� ���� ū ����, false�� ��� ���� ���� ������ �������� ����մϴ�.
	 */
	AActor* GetTargetByDotProduct(TArray<AActor*> TargetableActors, bool bIsLargestDot);

	/**
	 * �÷��̾� ĳ���Ϳ��� Target���� ���ϴ� ���Ϳ� �÷��̾� ĳ���Ϳ��� ���ڷ� ���� NewTargetableActor�� ���ϴ� ���Ϳ�
	 * ������ ����ϴ� �Լ��Դϴ�.
	 * @param NewTargetableActor ���͸� ���� �����Դϴ�.
	 */
	float CalculateDotProductToTarget(AActor* NewTargetableActor) const;

	/**
	 * ���ڷ� ���� ������ ȭ�鿡���� ��ġ�� �ش� ���Ͱ� ȭ�鿡 �����ϴ����� ��ȯ�ϴ� �Լ��Դϴ�.
	 * @param SearchActor ȭ�鿡���� ��ġ�� ã�� �����Դϴ�.
	 * @return Get<0> ���ڷ� ���� ������ ȭ�鿡���� ��ġ�Դϴ�.
	 * @return Get<1> ���ڷ� ���� ���Ͱ� ȭ�鿡 �����ϴ����� ��Ÿ���ϴ�.
	 */
	TTuple<FVector2D, bool> GetScreenPositionOfActor(AActor* SearchActor) const;

	// /**
	//  * ȭ�� ���� ������ ��ġ�� ����Ʈ�� �����ϴ� �Ǵ��ϴ� �Լ��Դϴ�.
	//  * @param ActorScreenPosition ȭ�� ���� ������ ��ġ�Դϴ�.
	//  */
	// bool IsInViewport(FVector2D ActorScreenPosition) const;

	/**
	 * ȭ����� ������ ��ġ�� ����Ʈ�� �����ϴ��� ��Ÿ���� �Լ��Դϴ�.
	 * ScreenRatio�� ���� 0.0f�� �ƴ� ��� ����Ʈ �߾ӿ��� �ش� ���� ȭ��ũ���� ������ŭ�� ������ ���Ͱ� �����ϴ��� ��Ÿ���ϴ�.
	 * @param ActorScreenPosition ȭ�鿡���� ������ ��ġ�Դϴ�.
	 * @param ScreenRatio ȭ�� ũ���� ������ ��Ÿ���� ���Դϴ�. 0.0f�� ��� ����Ʈ�� �ִ�ũ�⸦ ����ϸ� 0.2f�� ��� �߾ӿ��� ����Ʈ�� �����¿츦 0.2f ������ŭ ���� ũ�⸦ ����մϴ�.
	 * @return ���Ͱ� ����Ʈ�� �����ϴ��� ��Ÿ���ϴ�.
	 */
	bool IsInViewport(FVector2D ActorScreenPosition, float ScreenRatio = 0.0f) const;

	/**
	 * ���ڷ� ���� ���͸� �ٶ󺸴� ȸ�� ���� ���� ����ϴ� �Լ��Դϴ�.
	 * @param InterpToTarget �ٶ� �����Դϴ�.
	 * @return ���ڷ� ���� ���͸� �ٶ󺸴� ȸ�� ���Դϴ�.
	 */
	FRotator CalculateInterpToTarget(AActor* InterpToTarget) const;

private:
	/** Target�� LockOn ������ ��Ÿ���� �����Դϴ�. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TargetingSystem", Meta = (AllowPrivateAccess = "true"))
	bool bIsLockOnTarget;

	/** Target�� LockOn�ϸ鼭 ī�޶� �����Ӱ� �����ϴ� DynamicLockOn ������ ��Ÿ���� �����Դϴ�. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TargetingSystem", Meta = (AllowPrivateAccess = "true"))
	bool bDynamicLockOnTarget;

	/** DynamicLockOn�� �����ϴ� ȭ�� ������ ��Ÿ���� �����Դϴ�. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TargetingSystem", Meta = (AllowPrivateAccess = "true"))
	float StopDynamicCameraLockScreenRatio;

	/** LockOn�ϴ� Target�Դϴ�. */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "TargetingSystem", Meta = (AllowPrivateAccess = "true"))
	AActor* Target;

	/** Target���� ������ ������ ���͵��� Ž���ϴ� �ִ� Ž�� �����Դϴ�. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TargetingSystem", Meta = (AllowPrivateAccess = "true"))
	float MaxSearchTargetableDistance;

	/** Target�� LockOn�� �� Target�� �ٶ󺸴� �����ӵ��Դϴ�. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TargetingSystem", Meta = (AllowPrivateAccess = "true"))
	float InterpSpeed;

	/**
	 * ���������� Target�� ������ �ð��Դϴ�.
	 * ���콺�� Target�� ������ �� ����մϴ�.
	 */
	UPROPERTY(BlueprintReadWrite, Category = "TargetingSystem|MouseInput", Meta = (AllowPrivateAccess = "true"))
	float LastTimeChangeTarget;

	/**
	 * ���������� Target�� ������ ������ �ð��Դϴ�.
	 * ���콺�� Target�� ������ �� ����մϴ�.
	 */
	UPROPERTY(BlueprintReadWrite, Category = "TargetingSystem|MouseInput", Meta = (AllowPrivateAccess = "true"))
	float TimeSinceLastChangeTarget;

	/**
	 * Target�� �����ϱ� ���� �ʿ��� ���콺�� �ּ� ȸ�� �Է� ���Դϴ�.
	 * ���콺�� Target�� ������ �� ����մϴ�.
	 */
	UPROPERTY(BlueprintReadWrite, Category = "TargetingSystem|MouseInput", Meta = (AllowPrivateAccess = "true"))
	float MinMouseValueToChangeTarget;

	/**
	 * �ٸ� Target���� �����ϱ� ���� �����̸� �ִ� �ð��Դϴ�.
	 * �ʹ� ������ �ٸ� Target���� �����ϴ� ���� �����մϴ�.
	 * ���콺�� Target�� ������ �� ����մϴ�.
	 */
	UPROPERTY(BlueprintReadWrite, Category = "TargetingSystem|MouseInput", Meta = (AllowPrivateAccess = "true"))
	float MinDelayTimeToChangeTarget;

	/**
	 * ���������� Target�� ������ �� �����е��� �Ƴ��α� ��ƽ�� �߸��� �Ǿ����� ��Ÿ���� �����Դϴ�.
	 * �����е��� �Ƴ��α� ��ƽ�� �߸��� ��� true�� �����˴ϴ�.
	 * �����е�� Target�� ������ �� ����մϴ�.
	 */
	UPROPERTY(BlueprintReadWrite, Category = "TargetingSystem|GamepadInput", Meta = (AllowPrivateAccess = "true"))
	bool bAnalogNeutralSinceLastSwitchTarget;

	/**
	 * Target�� �����ϱ� ���� �ʿ��� �Ƴ��α� ��ƽ�� �ּ� �Է� ���Դϴ�.
	 * �����е�� Target�� ������ �� ����մϴ�.
	 */
	UPROPERTY(BlueprintReadWrite, Category = "TargetingSystem|GamepadInput", Meta = (AllowPrivateAccess = "true"))
	float MinAnalogValueToChangeTarget;

public:
	/**
	 * �Է¹��� ���ڷ� bDynamicLockOnCamera�� �����ϴ� �Լ��Դϴ�.
	 * @param bFlag bDynamicLockOnCamera�� �����ϴ� �Ű������Դϴ�.
	 */
	void SetDynamicLockOnTarget(bool bFlag);

	/** Target�� ��ȯ�ϴ� �Լ��Դϴ�. */
	AActor* GetTarget() const;
#pragma endregion 

};
