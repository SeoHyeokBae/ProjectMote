// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SkeletalToStaticMesh.generated.h"

UCLASS()
class MOTE_API ASkeletalToStaticMesh : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASkeletalToStaticMesh();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UPoseableMeshComponent* mPoseableComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION(BlueprintCallable)
	void Init(USkeletalMeshComponent* SkeletalMeshComponent);



};
