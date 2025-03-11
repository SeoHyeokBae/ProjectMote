// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillStone.h"
#include "../Controller/MotePlayerController.h"
#include "../UI/Main/MainWidget.h"
#include "../Player/PlayerCharacter.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "../Sound/SoundSubsystem.h"
#include "NiagaraSystemWidget.h"
#include "NiagaraUIComponent.h"

ASkillStone::ASkillStone()
{
	PrimaryActorTick.bCanEverTick = true;

	mCapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	mMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	mGeometryComponent = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryComponent"));

	static ConstructorHelpers::FObjectFinder<UGeometryCollection>
		Colloction(TEXT("/Script/GeometryCollectionEngine.GeometryCollection'/Game/AI/Monster/Boss/Meteor/GC_SkillStone.GC_SkillStone'"));

	if (Colloction.Succeeded())
		mGeometryCollection = Colloction.Object;
	
	static ConstructorHelpers::FObjectFinder<UStaticMesh>
		MeshAsset(TEXT("/Script/Engine.StaticMesh'/Game/AI/Monster/Boss/Meteor/SM_Meteor.SM_Meteor'"));

	if (MeshAsset.Succeeded())
		mMesh->SetStaticMesh(MeshAsset.Object);

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem>
		SkillStoneAsset(TEXT("/Script/Niagara.NiagaraSystem'/Game/Map/Planet/Test/NS_SkillStone.NS_SkillStone'"));

	if (SkillStoneAsset.Succeeded())
		mSkillStoneFXSystem = SkillStoneAsset.Object;

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem>
		SkillStoneLoadAsset(TEXT("/Script/Niagara.NiagaraSystem'/Game/Map/Planet/Test/NS_SkillMeteorLoad.NS_SkillMeteorLoad'"));

	if (SkillStoneLoadAsset.Succeeded())
		mSkillStoneLoadFXSystem = SkillStoneLoadAsset.Object;

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem>
		SkillStoneBreakAsset(TEXT("/Script/Niagara.NiagaraSystem'/Game/Map/Planet/Test/NS_SkillMeteorBreak.NS_SkillMeteorBreak'"));

	if (SkillStoneBreakAsset.Succeeded())
		mSkillStoneBreakFXSystem = SkillStoneBreakAsset.Object;

	SetRootComponent(mCapsuleComponent);

	mCapsuleComponent->SetCapsuleSize(1000.f, 500.f);
	mCapsuleComponent->SetHiddenInGame(true);
	mCapsuleComponent->SetCollisionProfileName(TEXT("Portal"));

	mMesh->SetupAttachment(RootComponent);
	mMesh->SetRelativeScale3D(FVector(0.4,0.4,0.8));
	mMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	mCapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &ASkillStone::BeginOverlap);
	mCapsuleComponent->OnComponentEndOverlap.AddDynamic(this, &ASkillStone::EndOverlap);
	
	mGeometryComponent->SetupAttachment(RootComponent);

}

void ASkillStone::BeginPlay()
{
	Super::BeginPlay();

	mGeometryComponent->SetRestCollection(mGeometryCollection);
	mGeometryComponent->SetSimulatePhysics(false);
	mGeometryComponent->SetVisibility(false);

	mSkillStoneFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
		mSkillStoneFXSystem, mCapsuleComponent, NAME_None, FVector(0.0, 0.0, 0.0), FRotator(0.0, 0.0, 0.0), EAttachLocation::KeepRelativeOffset, true);
}

void ASkillStone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!mCanTake)
	{
		mTakeTime += DeltaTime;
		if (mTakeTime > 6.f)
			mCanTake = true;
	}

	if (IsValid(mCapsuleComponent) && mIsOverlap && MotePlayer)
	{
		if (MotePlayer->PressedInteraction())
		{
			MotePlayer->SetPressedInteraction(false);
			mIsOverlap = false;
			mIsTakeStone = true;

			if (mSkillStoneFXComponent)
				mSkillStoneFXComponent->DestroyComponent();
			if (mSkillStoneLoadFXComponent)
				mSkillStoneLoadFXComponent->DestroyComponent();

			if (!mSkillStoneBreakFXComponent)
				mSkillStoneBreakFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
					mSkillStoneBreakFXSystem, mCapsuleComponent, NAME_None, FVector(0.0, 0.0, 0.0), FRotator(0.0, 0.0, 0.0), EAttachLocation::KeepRelativeOffset, true);

			AMotePlayerController* PlayerController = Cast<AMotePlayerController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
			if (IsValid(PlayerController))
			{
				PlayerController->GetMainWidget()->mSkillStoneImage->SetVisibility(ESlateVisibility::Visible);
				// UI 이펙트 재생
				PlayerController->GetMainWidget()->mNiagaraWidget->GetNiagaraComponent()->SetAutoActivateParticle(true);
				PlayerController->GetMainWidget()->mNiagaraStoneLight->GetNiagaraComponent()->SetAutoActivateParticle(true);

				FTimerHandle TimerHandle;
				GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, PlayerController]()
					{
						PlayerController->VisibleClearWidget();
						DisableInput(PlayerController);
						//Destroy();
					}, 2.5f, false);

			}

			// 기존 Mesh를 Fracture기능 Mesh로 교체
			if (mIsTakeStone)
			{
				mMesh->SetRenderCustomDepth(false);
				ChangeMesh();
			}
		}
	}
}

void ASkillStone::ChangeMesh()
{			
	if (mMesh)
	{
		mMesh->SetVisibility(false);
		mGeometryComponent->SetVisibility(true);

		if (mGeometryCollection)
		{
			UMaterialInstanceDynamic* MaterialInstance = mMesh->CreateAndSetMaterialInstanceDynamic(0);
			
			if (MaterialInstance)
			{
				mGeometryComponent->SetSimulatePhysics(true);

				mGeometryComponent->SetMaterial(0, MaterialInstance);
				MaterialInstance->SetVectorParameterValue(TEXT("BaseColor"), FLinearColor::Black);
				MaterialInstance->SetVectorParameterValue(TEXT("EmissiveColor"), FLinearColor::Black);
				mGeometryComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
				mGeometryComponent->AddRadialImpulse(GetActorLocation(), 300.f, 30000.f, ERadialImpulseFalloff::RIF_Linear);

				mMesh->DestroyComponent();
			}

			USoundSubsystem* BrokenSFX = GetGameInstance()->GetSubsystem<USoundSubsystem>();
			if (BrokenSFX)
			{
				BrokenSFX->PlayVFXSound(TEXT("Broken"), 0.5f, GetActorLocation());
			}
		}
	}
}

void ASkillStone::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!mCanTake)
		return;
	
	MotePlayer = Cast<APlayerCharacter>(OtherActor);
	if (!MotePlayer || mIsTakeStone)
		return;

	MotePlayer->CanInteraction();

	if (mMesh)
	{
		mMesh->SetRenderCustomDepth(true);
	}

	mSkillStoneLoadFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
		mSkillStoneLoadFXSystem, mCapsuleComponent, NAME_None, FVector(0.0, 0.0, 0.0), FRotator(0.0, 0.0, 0.0), EAttachLocation::KeepRelativeOffset, true);
	
	mIsOverlap = true;
}

void ASkillStone::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	MotePlayer = Cast<APlayerCharacter>(OtherActor);
	if (!MotePlayer)
		return;

	MotePlayer->CanNotInteraction();

	mIsOverlap = false;

	if (mMesh)
	{
		mMesh->SetRenderCustomDepth(false);
	}

	if (mSkillStoneLoadFXComponent)
		mSkillStoneLoadFXComponent->Deactivate();

	if (mSkillStoneBreakFXComponent)
		mSkillStoneBreakFXComponent->DestroyComponent();

	MotePlayer = nullptr;
}


