// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../GameInfo.h"
#include "GameFramework/Actor.h"
#include "SkillStone.generated.h"

UCLASS()
class MOTE_API ASkillStone : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASkillStone();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* mMesh;

	UPROPERTY(VisibleAnywhere)
	UCapsuleComponent* mCapsuleComponent;

	UPROPERTY(VisibleAnywhere)
	UNiagaraSystem* mSkillStoneFXSystem;

	UPROPERTY()
	UNiagaraComponent* mSkillStoneFXComponent;

	UPROPERTY()
	UNiagaraSystem* mSkillStoneLoadFXSystem;

	UPROPERTY()
	UNiagaraComponent* mSkillStoneLoadFXComponent;

	UPROPERTY(VisibleAnywhere)
	UNiagaraSystem* mSkillStoneBreakFXSystem;

	UPROPERTY()
	UNiagaraComponent* mSkillStoneBreakFXComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UGeometryCollectionComponent* mGeometryComponent;

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UGeometryCollection* mGeometryCollection;
	
	class APlayerCharacter* MotePlayer;

protected:
	bool mIsOverlap = false;
	bool mIsTakeStone = false;
	bool mCanTake = false;
	float mTakeTime = 0.f;
	float mOverlapTime = 0.f;


private:
	UFUNCTION()
	void BeginOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void EndOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

protected:
	void ChangeMesh();
};
