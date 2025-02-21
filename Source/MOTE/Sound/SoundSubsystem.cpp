// Fill out your copyright notice in the Description page of Project Settings.


#include "SoundSubsystem.h"
#include "SoundInfo.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundMix.h"
#include "Sound/SoundClass.h"

USoundSubsystem::USoundSubsystem()
{
    static ConstructorHelpers::FObjectFinder<UDataTable>
        SoundDataTableAsset(TEXT("/Script/Engine.DataTable'/Game/Sound/DT_SoundInfo.DT_SoundInfo'"));

    if (SoundDataTableAsset.Succeeded())
        SoundDataTable = SoundDataTableAsset.Object;
}

void USoundSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

    mSoundMix = Cast<USoundMix>(StaticLoadObject(USoundMix::StaticClass(), nullptr, TEXT("/Script/Engine.SoundMix'/Game/Sound/SoundMix.SoundMix'")));

    for (auto SoundClassAdj : mSoundMix->SoundClassEffects)
    {
        USoundClass* SoundClass = SoundClassAdj.SoundClassObject;
        FString SoundClassName = SoundClass->GetName();
        if (SoundClassName.Contains("Master"))
        {
            MasterSoundClass = SoundClassAdj.SoundClassObject;
        }
        else if (SoundClassName.Contains("Effect"))
        {
            VFXSoundClass = SoundClassAdj.SoundClassObject;
        }
        else if (SoundClassName.Contains("BGM"))
        {
            BGMSoundClass = SoundClassAdj.SoundClassObject;
        }
    }
}

void USoundSubsystem::Deinitialize()
{
    Super::Deinitialize();
}


void USoundSubsystem::PlayVFXSound(const FName& SoundName, float VolumeMultiplier, FVector Location)
{
    if (!SoundDataTable) return;

    FSoundData* SoundData = SoundDataTable->FindRow<FSoundData>(TEXT("EffectSound"), TEXT(""));
    if (SoundData)
    {
        USoundBase** Sound = SoundData->SoundDatas.Find(SoundName);
        if (Sound && *Sound)
        {
            // 3D 재생
            if (Location != FVector::ZeroVector)
            {
                UGameplayStatics::PlaySoundAtLocation(this,*Sound, Location, VolumeMultiplier);
            }
            // 2D 재생
            else
            {
                UGameplayStatics::PlaySound2D(this, *Sound, VolumeMultiplier);
            }
        }
        else
            UE_LOG(LogTemp, Warning, TEXT("Failed to load SoundDataTable"));
    }
}

void USoundSubsystem::PlayBGM(const FName& SoundName, float VolumeMultiplier, float FadeInDuration)
{
    if (!SoundDataTable) return;

    FSoundData* SoundData = SoundDataTable->FindRow<FSoundData>(TEXT("BGM"), TEXT(""));
    if (SoundData)
    {
        USoundBase** Sound = SoundData->SoundDatas.Find(SoundName);
        if (Sound && *Sound)
        {
            if (SoundName == FName("DeathBGM"))
            {
                if (mSoundMix)
                {
                    // 사망시 백그라운드로 재생될 이펙트사운드볼륨 조절 
					UGameplayStatics::SetSoundMixClassOverride(GetWorld(), mSoundMix, VFXSoundClass, VFXVolume * 0.2f);
					//UGameplayStatics::PushSoundMixModifier(GetWorld(), mSoundMix);
                }
            }
            
            AudioComponent = UGameplayStatics::SpawnSound2D(this, *Sound, VolumeMultiplier);
            if (AudioComponent)
            {
                AudioComponent->FadeIn(FadeInDuration, VolumeMultiplier);
            }
        }
        else
            UE_LOG(LogTemp, Warning, TEXT("Failed to load SoundDataTable"));
    }
}

void USoundSubsystem::StopBGM(float FadeOutDuration)
{
    if (!SoundDataTable) return;

    if (AudioComponent && AudioComponent->IsPlaying())
    {
        AudioComponent->FadeOut(FadeOutDuration, 0.0f);
    }
}

void USoundSubsystem::SetPauseBGM(bool IsPlay)
{
    if (!SoundDataTable) return;
    FSoundData* SoundData = SoundDataTable->FindRow<FSoundData>(TEXT("BGM"), TEXT(""));
    if (SoundData)
    {
        USoundBase** Sound = SoundData->SoundDatas.Find(FName("PauseBGM"));
        if (Sound && *Sound)
        {
            if (!PauseAudioComponent)
            {
                PauseAudioComponent = UGameplayStatics::SpawnSound2D(this, *Sound, 0.75f);
                if (PauseAudioComponent)
                    PauseAudioComponent->FadeIn(1.5f, 0.75f);
            }
            else
            {
                IsPlay ? PauseAudioComponent->FadeIn(1.5f, 0.75f) : PauseAudioComponent->FadeOut(1.f, 0.f);
            }
           
        }
    }

}

void USoundSubsystem::ResumeBGM(bool IsPlay)
{
    if (!AudioComponent)
        return;

    !IsPlay ? AudioComponent->SetPaused(true) : AudioComponent->SetPaused(false);
}

void USoundSubsystem::SetResetVolume()
{
    // ReStart때 사용됨
    UGameplayStatics::SetSoundMixClassOverride(GetWorld(), mSoundMix, VFXSoundClass, VFXVolume, 1.f, 0.f);

}

void USoundSubsystem::SetMasterVolume(float Value)
{
    if (MasterVolume == Value) return;
    MasterVolume = Value;

    UGameplayStatics::SetSoundMixClassOverride(GetWorld(), mSoundMix, MasterSoundClass, MasterVolume, 1.f, 0.f);
}

void USoundSubsystem::SetBGMVolume(float Value)
{
    if (BGMVolume == Value) return;
    BGMVolume = Value;

    UGameplayStatics::SetSoundMixClassOverride(GetWorld(), mSoundMix, BGMSoundClass, BGMVolume, 1.f, 0.f);
}

void USoundSubsystem::SetVFXVolume(float Value)
{
    if (VFXVolume == Value) return;
    VFXVolume = Value;

    UGameplayStatics::SetSoundMixClassOverride(GetWorld(), mSoundMix, VFXSoundClass, VFXVolume,1.f, 0.f);
}
