// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "TargetAimSystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class MOTE_API UTargetAimSystemComponent : public USphereComponent
{
	GENERATED_BODY()

public:
	UTargetAimSystemComponent();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UWidgetComponent* mLockOnWidget;

protected:
	virtual void InitializeComponent() override;

public:
	/** LockOnWidget�� ��ȯ�ϴ� �Լ��Դϴ�. */
	UFUNCTION(BlueprintCallable)
	FORCEINLINE class UWidgetComponent* GetLockOnWidget() const
	{
		return mLockOnWidget;
	}
};
