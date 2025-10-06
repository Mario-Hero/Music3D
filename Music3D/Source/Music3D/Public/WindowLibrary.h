// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "WindowLibrary.generated.h"


UCLASS()
class MUSIC3D_API UWindowLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Window")
	static bool SetWindowMinimize();

	UFUNCTION(BlueprintCallable, Category = "Window")
	static bool SetWindowMaximize();

	UFUNCTION(BlueprintCallable, Category = "Window")
	static bool SetWindowRestore();

	UFUNCTION(BlueprintCallable, Category = "Window")
	static void WindowMode();
};
