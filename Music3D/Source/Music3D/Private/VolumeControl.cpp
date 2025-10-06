// Fill out your copyright notice in the Description page of Project Settings.


#include "VolumeControl.h"
#include "Components/AudioComponent.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
AVolumeControl::AVolumeControl()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	Controller = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Controller"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> KnobAsset(TEXT("StaticMesh'/Game/Models/Knob.Knob'"));
	static ConstructorHelpers::FObjectFinder<UMaterial> KnobMaterial(TEXT("Material'/Game/Material/VolumeControllerMaterial.VolumeControllerMaterial'"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> LightAsset(TEXT("StaticMesh'/Game/Models/VolumeLight.VolumeLight'"));
	static ConstructorHelpers::FObjectFinder<UMaterial> LightMaterial(TEXT("Material'/Game/Material/VolumeLightMaterial.VolumeLightMaterial'"));
	LightMaterialInterface = LightMaterial.Object;
	Controller->SetStaticMesh(KnobAsset.Object);
	Controller->SetMaterial(0, KnobMaterial.Object);
	Controller->SetRelativeScale3D(FVector(ControllerScale, ControllerScale, ControllerScale));
	Controller->SetRelativeLocation(FVector(0.f, 5.f, 0.f));
	Controller->SetRelativeRotation(FRotator(ControllerEndAngle, 0.f, 0.f));
	Controller->SetVisibility(true);
	Controller->SetCastShadow(false);
	Controller->SetupAttachment(RootComponent);
	for (int i = 0; i < LightCount; i++)
	{
		UStaticMeshComponent* light = CreateDefaultSubobject<UStaticMeshComponent>(*FString::Printf(TEXT("Light%d"), i));
		UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(LightMaterialInterface, light);
		light->SetStaticMesh(LightAsset.Object);
		light->SetRelativeScale3D(FVector(LightScale, LightScale, LightScale));
		const float angle = StartAngle + (EndAngle - StartAngle) / (LightCount - 1) * i;
		const float rad = FMath::DegreesToRadians(angle);
		const float radius = LightRadius;
		light->SetRelativeLocation(FVector(radius * FMath::Cos(rad), 0.f, radius * FMath::Sin(rad)));
		light->SetRelativeRotation(FRotator(angle, 0.f, 0.f));
		light->SetVisibility(true);
		light->SetupAttachment(RootComponent);
		DynMaterial->SetScalarParameterValue(FName(TEXT("Light")), 1.0f); //修改动态材质参数
		light->SetMaterial(0, DynMaterial);
		light->SetCastShadow(false);
		Lighter.Add(light);
		LightDynMaterials.Add(DynMaterial);
	}
	Controller->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Controller->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	// 绑定鼠标悬停事件
	Controller->OnBeginCursorOver.AddDynamic(this, &AVolumeControl::OnBeginCursorOver);
	Controller->OnEndCursorOver.AddDynamic(this, &AVolumeControl::OnEndCursorOver);
}

// Called when the game starts or when spawned
void AVolumeControl::BeginPlay()
{
	Super::BeginPlay();
	auto* world = GetWorld();
	if (!world) {
		UE_LOG(LogTemp, Warning, TEXT("World is null"));
		return;
	}
	APlayerController* PlayerController = world->GetFirstPlayerController();
	if (PlayerController)
	{
		// 绑定输入轴事件（鼠标滚轮）
		PlayerController->InputComponent->BindAxis("MouseWheel", this, &AVolumeControl::OnInputAxis);
		PlayerController->InputComponent->BindAxis("MouseY", this, &AVolumeControl::OnInputMouseY);
		PlayerController->InputComponent->BindAction("MouseLeft", IE_Released, this, &AVolumeControl::OnRelease);
		PlayerController->InputComponent->BindAction("MouseLeft", IE_Pressed, this, &AVolumeControl::OnPress);
		PlayerController->InputComponent->BindAction("KeyUp", IE_Pressed, this, &AVolumeControl::KeyUp);
		PlayerController->InputComponent->BindAction("KeyDown", IE_Pressed, this, &AVolumeControl::KeyDown);
		PlayerController->InputComponent->BindAction("KeyUp", IE_Released, this, &AVolumeControl::KeyUpRelease);
		PlayerController->InputComponent->BindAction("KeyDown", IE_Released, this, &AVolumeControl::KeyDownRelease);
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("PlayerController is null"));
	}
}

// Called every frame
void AVolumeControl::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(bKeyUpPress)
	{
		UpdateVolumeDelta(DeltaTime * 0.5f);
	}
	else if(bKeyDownPress)
	{
		UpdateVolumeDelta(-DeltaTime * 0.5f);
	}
}

void AVolumeControl::OnBeginCursorOver(UPrimitiveComponent* TouchedComponent)
{
	bIsHovered = true;
}

void AVolumeControl::OnEndCursorOver(UPrimitiveComponent* TouchedComponent)
{
	bIsHovered = false;
}

void AVolumeControl::OnPress()
{
	if (bIsHovered) {
		bIsPress = true;
	}
}

void AVolumeControl::OnRelease()
{
	bIsPress = false;
}

void AVolumeControl::OnInputAxis(float Delta)
{
	if (bIsHovered)
	{
		if (Delta != 0.f)
		{
			float NewVolume = CurrentVolume + Delta * 0.02f;
			if (NewVolume > VolumeEnd) NewVolume = VolumeEnd;
			if (NewVolume < VolumeStart) NewVolume = VolumeStart;
			UpdateVolume(NewVolume);
		}
	}
}

void AVolumeControl::OnInputMouseY(float Delta)
{
	if (Delta != 0.f) {
		if (bIsPress) {
			{
				float NewVolume = CurrentVolume + Delta * 0.02f;
				if (NewVolume > VolumeEnd) NewVolume = VolumeEnd;
				if (NewVolume < VolumeStart) NewVolume = VolumeStart;
				UpdateVolume(NewVolume);
			}
		}
	}

}

void AVolumeControl::UpdateSoundWave(UAudioComponent* audio)
{
	AudioComponent = audio;
	UpdateVolume(CurrentVolume);
}

void AVolumeControl::UpdateVolumeDelta(float delta)
{
	float NewVolume = CurrentVolume + delta;
	if (NewVolume > VolumeEnd) NewVolume = VolumeEnd;
	if (NewVolume < VolumeStart) NewVolume = VolumeStart;
	UpdateVolume(NewVolume);
}
void AVolumeControl::KeyUpRelease()
{
	bKeyUpPress = false;
}

void AVolumeControl::KeyDownRelease()
{
	bKeyDownPress = false;
}

void AVolumeControl::KeyUp()
{
	bKeyUpPress = true;
}

void AVolumeControl::KeyDown()
{
	bKeyDownPress = true;
}

void AVolumeControl::UpdateVolume(float NewVolume)
{
	const float OldVolume = CurrentVolume;
	CurrentVolume = NewVolume;
	const float CurrentRatio = (CurrentVolume - VolumeStart) / (VolumeEnd - VolumeStart);
	const float OldRatio = (OldVolume - VolumeStart) / (VolumeEnd - VolumeStart);
	const float ControllerAngle = ControllerStartAngle + (ControllerEndAngle - ControllerStartAngle) * CurrentRatio;
	Controller->SetRelativeRotation(FRotator(ControllerAngle, 0.f, 0.f));
	const int oldStep = int(OldRatio * LightCount);
	const int newStep = int(CurrentRatio * LightCount);
	const int rangeStart = FMath::Min(oldStep, newStep) - 1;
	const int rangeEnd = FMath::Max(oldStep, newStep) + 1;
	const float count = float(LightCount);
	for (int i = 0; i < LightCount; i++)
	{
		if (!(i <= rangeEnd && i >= rangeStart))
		{
			continue;
		}
		if (CurrentRatio >= float(i) / count)
		{
			if (CurrentRatio < float(i + 1) / count) {
				const float light = CurrentRatio * count - float(i);
				LightDynMaterials[i]->SetScalarParameterValue(FName(TEXT("Light")), light);
			}
			else {
				LightDynMaterials[i]->SetScalarParameterValue(FName(TEXT("Light")), 1.0f);
			}
		}
		else
		{
			LightDynMaterials[i]->SetScalarParameterValue(FName(TEXT("Light")), 0.f);
		}
		Lighter[i]->SetMaterial(0, LightDynMaterials[i]);
	}
	if (AudioComponent == nullptr) { return; }
	AudioComponent->SetVolumeMultiplier(CurrentVolume);
}

