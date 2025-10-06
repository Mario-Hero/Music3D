// Fill out your copyright notice in the Description page of Project Settings.


#include "LrcReader.h"
#include "CoreMinimal.h"
#include "HAL/FileManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include <algorithm>

FString ULrcReader::FindLrcFile(const FString& InFilePath)
{
	// 获取文件的目录和基本文件名（不带后缀）
	FString Directory = FPaths::GetPath(InFilePath);
	FString BaseName = FPaths::GetBaseFilename(InFilePath);

	// 构建目标LRC文件名
	FString LrcFileName = BaseName + TEXT(".lrc");
	FString LrcFilePath = FPaths::Combine(Directory, LrcFileName);

	// 检查文件是否存在
	if (FPlatformFileManager::Get().GetPlatformFile().FileExists(*LrcFilePath))
	{
		return LrcFilePath;
	}

	// 如果不存在，返回空字符串
	return FString();
}

int ULrcReader::GetIndexLrc(const TArray<FLyricEntry>& lrc, const float time)
{
	int l = 1, r = lrc.Num() - 1;
	while (l <= r)
	{
		int mid = l + ((r - l) >> 1);
		if (time >= lrc[mid].Time)    //upper的话等于的时候也返
			//回右边,因为要找到大于最后一个key。
			l = mid + 1;
		else
			r = mid - 1;
	}
	return FMath::Max(l - 1, 0);
}

TArray<FString> SplitStringByCommaAndSpace(const FString& InputString)
{
	// 先用逗号分割
	TArray<FString> CommaSplit;
	InputString.ParseIntoArray(CommaSplit, TEXT(","), true);
	TArray<FString> OutArray;
	// 然后对每个部分用空格分割
	for (const FString& Part : CommaSplit)
	{
		TArray<FString> SpaceSplit;
		Part.ParseIntoArray(SpaceSplit, TEXT(" "), true);

		// 过滤掉空字符串并添加到结果数组
		for (const FString& Word : SpaceSplit)
		{
			if (!Word.IsEmpty())
			{
				OutArray.Add(Word);
			}
		}
	}
	return OutArray;
}

TArray<FString> ULrcReader::GetLrcString(const TArray<FLyricEntry>& lrc, const int index)
{
	if (lrc.Num() == 0 || index < 0 || index >= lrc.Num()) {
		return TArray<FString>{ "", "", "" };
	}
	TArray<FString> arr;
	FString str = "";
	for (int i = FMath::Min(FMath::Max(index - 2, 0), lrc.Num() - 1); i < FMath::Min(lrc.Num(), index + 4); i++)
	{
		if (i == index)
		{
			arr.Add(str);
			arr.Add(lrc[i].Text + "\n");
			str.Reset();
		}
		else {
			str += lrc[i].Text + "\n";
		}
	}
	arr.Add(str);
	while (arr.Num() < 3) {
		arr.Add("");
	}
	return arr;
}

// 解析LRC文件并填充到数组
TArray<FLyricEntry> ULrcReader::ParseLrcFile(const FString& FilePath)
{
	// 读取文件内容
	FString FileContent;
	TArray<FLyricEntry> OutLyrics;
	if (!FFileHelper::LoadFileToString(FileContent, *FilePath))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load LRC file: %s"), *FilePath);
		return OutLyrics;
	}

	// 按行分割内容
	TArray<FString> Lines;
	FileContent.ParseIntoArrayLines(Lines);
	for (const FString& Line : Lines)
	{
		if (Line.IsEmpty())
		{
			continue;
		}

		// 处理标签行（例如：[ti:...]）
		if (Line.StartsWith("[ti:") || Line.StartsWith("[ar:") || Line.StartsWith("[al:") || Line.StartsWith("[by:") || Line.StartsWith("[offset:"))
		{
			// 这里可以解析元数据，根据需要处理
			continue;
		}

		// 处理时间标签行（例如：[01:45.32]歌词）
		// 使用正则表达式或字符串查找来解析
		int32 BracketStart = 0;
		int32 BracketEnd = 0;
		if (Line.FindChar('[', BracketStart) && Line.FindChar(']', BracketEnd) && BracketEnd > BracketStart)
		{
			// 提取时间标签部分
			FString TimeTag = Line.Mid(BracketStart + 1, BracketEnd - BracketStart - 1);
			FString LyricText = Line.Mid(BracketEnd + 1).TrimStartAndEnd();

			// 检查时间标签是否符合格式 [mm:ss.xx]
			TArray<FString> TimeParts;
			TimeTag.ParseIntoArray(TimeParts, TEXT(":"), true);
			if (TimeParts.Num() == 2)
			{
				const float Seconds = FCString::Atof(*TimeParts[1]);
				const int32 Minutes = FCString::Atoi(*TimeParts[0]);
				// 转换为总秒数（浮点数）
				const float TotalSeconds = Minutes * 60.0 + Seconds;

				// 创建歌词条目并添加到数组
				OutLyrics.Add(FLyricEntry(TotalSeconds, LyricText));
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("Invalid time format in LRC line: %s"), *Line);
			}

		}
	}
	return OutLyrics;
}