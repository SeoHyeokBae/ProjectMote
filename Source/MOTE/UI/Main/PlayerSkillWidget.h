// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "../UIInfo.h"
#include "../../GameInfo.h"
#include "Blueprint/UserWidget.h"
#include "PlayerSkillWidget.generated.h"

/**
 * 
 */
UCLASS()
class MOTE_API UPlayerSkillWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct();

protected:
	UPROPERTY(VisibleAnywhere)
	UImage* mSkillFrame;

	UPROPERTY(VisibleAnywhere)
	UImage* mSkillBack;

	UPROPERTY(VisibleAnywhere)
	UImage* mSkillMeteor;

	bool mGetMeteor = false;

public:
	void OnGetMeteor() { mGetMeteor = true; };
};
