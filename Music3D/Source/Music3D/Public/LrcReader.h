// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LrcReader.generated.h"

// 定义歌词条目结构体
USTRUCT(BlueprintType)
struct FLyricEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Lyric")
	float Time; // 单位：秒

	UPROPERTY(BlueprintReadOnly, Category = "Lyric")
	FString Text; // 歌词文本

	// 默认构造函数
	FLyricEntry() : Time(0.0f) {}

	// 带参构造函数
	FLyricEntry(float InTime, const FString& InText)
		: Time(InTime), Text(InText){
	}

	// 重载小于运算符，用于排序
	bool operator<(const FLyricEntry& Other) const
	{
		return Time < Other.Time;
	}
};

UCLASS()
class MUSIC3D_API ULrcReader : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// 解析LRC文件并填充到数组
	UFUNCTION(BlueprintCallable, Category = "Music|LRC")
	static TArray<FLyricEntry> ParseLrcFile(const FString& FilePath);
	// 寻找LRC文件
	UFUNCTION(BlueprintCallable, Category = "Music|LRC")

	static FString FindLrcFile(const FString& InFilePath);
	// 根据LRC数组和当前时间获取当前歌词索引
	UFUNCTION(BlueprintCallable, Category = "Music|LRC")
	static int GetIndexLrc(const TArray<FLyricEntry>& lrc, const float time);

	// 根据LRC数组和当前索引返回歌词字符串
	UFUNCTION(BlueprintCallable, Category = "Music|LRC")
	static TArray<FString> GetLrcString(const TArray<FLyricEntry>& lrc, const int index);
};
