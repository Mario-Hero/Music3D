// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ImageUtils.h" 
#include "TagLib2.generated.h"

// 添加BlueprintType可以直接在蓝图中使用Break/Make等函数
USTRUCT(BlueprintType)
struct FMusicTag
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString title = FString("");

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString artist = FString("");

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString album = FString("");

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString comment = FString("");

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FString genre = FString("");

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int year = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	int track = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UTexture2D* cover = nullptr;
};

class FTagLib2Module : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;


private:
	/** Handle to the test dll we will load */
	void*	ExampleLibraryHandle;
};

UCLASS()
class UTagLib : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Music|ID3")
	static FMusicTag GetMP3Tag(const FString& path);
	UFUNCTION(BlueprintCallable, Category = "Music|ID3")
	static void PrintTag(const FMusicTag& tag);
};
