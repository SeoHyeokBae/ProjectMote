// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../../GameInfo.h"
#include "GameFramework/Actor.h"
#include "Telegraph.generated.h"

UENUM()
enum class ETelegraphShape : uint8
{
	Square,
	Circle,
};

UCLASS()
class MOTE_API ATelegraph : public ADecalActor
{
	GENERATED_BODY()
	
public:	
	ATelegraph();

protected:
	UPROPERTY(VisibleAnywhere)
	UMaterialInstance* mAimMTI_Square;

	UPROPERTY(VisibleAnywhere)
	UMaterialInstance* mAimMTI_Circle;

	UPROPERTY(VisibleAnywhere)
	UMaterialInstanceDynamic* mAimMTI_Dynamic_Square;

	UPROPERTY(VisibleAnywhere)
	UMaterialInstanceDynamic* mAimMTI_Dynamic_Circle;

	float mPercent = 0.f;
	float mLifeTime = 0.5f;

	ETelegraphShape mShape = ETelegraphShape::Square;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
	void UpdatePercent();
	void SetShape(ETelegraphShape Shape);
	void SetLifeTime(float Time) { mLifeTime = Time; }

};
