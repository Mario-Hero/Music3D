// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VolumeControl.generated.h"

UCLASS()
class MUSIC3D_API AVolumeControl : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AVolumeControl();
	UStaticMeshComponent* Controller;
	TArray<UStaticMeshComponent*> Lighter;

private:
	UMaterialInterface* LightMaterialInterface;
	TArray<UMaterialInstanceDynamic*> LightDynMaterials;
	const float ControllerScale = 0.07f;
	const float LightScale = 0.004f;
	const float LightRadius = 11.f;
	const float AngleOffset = -90.f;
	const float ControllerStartAngle = 60.f;
	const float ControllerEndAngle = 300.f;
	const float VolumeStart = 0.f;
	const float VolumeEnd = 1.f;
	const float StartAngle = ControllerStartAngle + AngleOffset;
	const float EndAngle = ControllerEndAngle + AngleOffset;
	const int LightCount = 10;
	bool bIsHovered = false;
	bool bIsPress = false;
	bool bKeyUpPress = false;
	bool bKeyDownPress = false;
	UAudioComponent* AudioComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	float CurrentVolume = 1.0f; // 当前音量，范围0.0到1.0

	// 鼠标悬停开始
	UFUNCTION()
	void OnBeginCursorOver(UPrimitiveComponent* TouchedComponent);

	// 鼠标悬停结束
	UFUNCTION()
	void OnEndCursorOver(UPrimitiveComponent* TouchedComponent);

	// 鼠标按下
	UFUNCTION()
	void OnPress();

	// 鼠标释放
	UFUNCTION()
	void OnRelease();

	// 处理输入轴事件（鼠标滚轮）
	UFUNCTION()
	void OnInputAxis(float Delta);

	// 处理输入轴事件（鼠标上下移动）
	UFUNCTION()
	void OnInputMouseY(float Delta);

	UFUNCTION(BlueprintCallable)
	void UpdateSoundWave(UAudioComponent* audio);

	UFUNCTION(BlueprintCallable)
	void UpdateVolumeDelta(float delta);

	void KeyUp();

	void KeyDown();

	void KeyUpRelease();

	void KeyDownRelease();

	// 更新音量的函数
	void UpdateVolume(float NewVolume);

};
