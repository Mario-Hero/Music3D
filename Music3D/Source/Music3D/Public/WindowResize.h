// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WindowResize.generated.h"

UCLASS()
class MUSIC3D_API AWindowResize : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWindowResize();
	UObject* World = nullptr;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void OnViewportResized(FViewport* Viewport, uint32 num);

	TSharedPtr<SWindow> Window = nullptr;

	UFUNCTION()
	void OnWindowActive();
	UFUNCTION()
	void OnWindowDeactivated();

};
