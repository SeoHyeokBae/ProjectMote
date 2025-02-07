// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SoundSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class MOTE_API USoundSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
    USoundSubsystem();

private:
    UAudioComponent* AudioComponent; //BGM 전용
    UAudioComponent* PauseAudioComponent; //PauseBGM 전용

    class USoundMix* mSoundMix;
    class USoundClass* MasterSoundClass;
    class USoundClass* VFXSoundClass;
    class USoundClass* BGMSoundClass;

    float MasterVolume = 1.f;
    float BGMVolume = 1.f;
    float VFXVolume = 1.f;

private:
    UDataTable* SoundDataTable = nullptr;

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable, Category = "SoundControl")
    void PlayVFXSound(const FName& SoundName, float VolumeMultiplier = 1.0f, FVector Location = FVector::ZeroVector);
    UFUNCTION(BlueprintCallable, Category = "SoundControl")
    void PlayBGM(const FName& SoundName, float VolumeMultiplier = 1.0f, float FadeInDuration = 0.0f);
    UFUNCTION(BlueprintCallable, Category = "SoundControl")
    void StopBGM(float FadeOutDuration = 0.0f);
    UFUNCTION(BlueprintCallable, Category = "SoundControl")
    void SetPauseBGM(bool IsPlay);
    UFUNCTION(BlueprintCallable, Category = "SoundControl")
    void ResumeBGM(bool IsPlay);
    UFUNCTION(BlueprintCallable, Category = "SoundControl")
    void SetResetVolume();

    FORCEINLINE float GetMasterVolume() { return MasterVolume; }
    FORCEINLINE float GetBGMVolume() { return BGMVolume; }
    FORCEINLINE float GetVFXVolume() { return VFXVolume; }

    void SetMasterVolume(float Value);
    void SetBGMVolume(float Value);
    void SetVFXVolume(float Value);
};
