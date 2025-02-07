// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../GameInfo.h"
#include "GameFramework/PlayerController.h"
#include "MotePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class MOTE_API AMotePlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UMainWidget>	mMainWidgetClass;
	class UMainWidget* mMainWidget;	

	TSubclassOf<class UPlayerPauseWidget>	mPauseWidgetClass;
	class UPlayerPauseWidget* mPauseWidget;

	UPROPERTY(VisibleAnywhere)
	TSubclassOf<class UPlayerSkillWidget>	mPlayerSkillWidgetClass;
	class UPlayerSkillWidget* mPlayerSkillWidget;


	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> mCursorWidgetClass;

	UPROPERTY(VisibleAnywhere)
	class UUserWidget* mCursorWidget;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UUserWidget> mArrowCursorWidgetClass;

	UPROPERTY(VisibleAnywhere)
	class UUserWidget* mArrowCursorWidget;

	TSubclassOf<class UPlayerDeathWidget>	mDeathWidgetClass;
	class UPlayerDeathWidget* mDeathWidget;

	TSubclassOf<class UPlayerClearWidget>	mClearWidgetClass;
	class UPlayerClearWidget* mClearWidget;

	TSubclassOf<class UOptionsWidget>	mOptionsWidgetClass;
	class UOptionsWidget* mOptionsWidget;

public:
	AMotePlayerController();

protected:
	virtual void BeginPlay() override;
	
public:
	virtual void UpdateRotation(float DeltaTime) override;

	// Converts a rotation from world space to gravity relative space.
	UFUNCTION(BlueprintPure)
	static FRotator GetGravityRelativeRotation(FRotator Rotation, FVector GravityDirection);

	// Converts a rotation from gravity relative space to world space.
	UFUNCTION(BlueprintPure)
	static FRotator GetGravityWorldRotation(FRotator Rotation, FVector GravityDirection);

	UFUNCTION(BlueprintCallable, Category = "Widgets")
	class UMainWidget* GetMainWidget() { return mMainWidget; }
	class UPlayerSkillWidget* GetPlayerSkillWidget() { return mPlayerSkillWidget; }

public:
	void InVisibleMainWidget();
	void InVisiblePauseWidget();
	void VisibleDeathWidget();
	void VisiblePauseWidget();
	void VisibleClearWidget();
	void VisibleOptionsWidget(UUserWidget* Widget);

private:
	FVector LastFrameGravity = FVector::ZeroVector;
};
