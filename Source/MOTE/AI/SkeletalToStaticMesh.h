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
	ASkeletalToStaticMesh();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UPoseableMeshComponent* mPoseableComponent;

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
	void Init(USkeletalMeshComponent* SkeletalMeshComponent);



};
