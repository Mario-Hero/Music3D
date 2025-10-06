// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "ImageUtils.h" 
#include "FunctionLibrary.generated.h"

/**
 *
 */
UCLASS()
class MUSIC3D_API UFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
private:
	static TSharedPtr<IImageWrapper> GetImageWrapperByExtention(const FString& InImagePath);
public:
	UFUNCTION(BlueprintPure, Category = "File")
	static bool IsMusic(const FString& path);
	UFUNCTION(BlueprintCallable, Category = "Music3D|Cover")
	static UTexture2D* LoadTexture2D(const FString& path);

	UFUNCTION(BlueprintCallable, Category = "Music3D|Volume")
	static void SetUnfocusedVolumeMultiplier();

	UFUNCTION(BlueprintCallable, Category = "Music3D|Cover")
	static FString FindCoverFile(const FString& path);

	UFUNCTION(BlueprintCallable, Category = "Music3D|ProgressBar")
	static FColor GetMainColor(UTexture2D* texture);

	UFUNCTION(BlueprintCallable, Category = "Music3D|ProgressBar")
	static FString Int2Time(const int time);

	UFUNCTION(BlueprintCallable, Category = "File")
	static TArray<FString> GetAllMusicInFolder(const FString& folder);

	UFUNCTION(BlueprintPure, Category = "Math")
	static float LerpLimit(const float A, const float B, const float alpha, const bool direction);
	UFUNCTION(BlueprintPure, Category = "Math")
	static bool InRange(const float A, const float B, const float C);

	UFUNCTION(BlueprintPure, Category = "Math")
	static FRotator AddRotatorDirect(const FRotator& a, const FRotator& b);
};
