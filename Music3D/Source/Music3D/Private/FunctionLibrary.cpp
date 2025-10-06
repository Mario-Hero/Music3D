// Fill out your copyright notice in the Description page of Project Settings.


#include "FunctionLibrary.h"
#include "Misc/Paths.h"
#include "HAL/FileManager.h"
#include "DesktopPlatform/Public/IDesktopPlatform.h"
#include "DesktopPlatform/Public/DesktopPlatformModule.h"
#include "Misc/App.h"

TSharedPtr<IImageWrapper> UFunctionLibrary::GetImageWrapperByExtention(const FString& InImagePath)
{
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	if (InImagePath.EndsWith(".png"))
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
	}
	else if (InImagePath.EndsWith(".jpg") || InImagePath.EndsWith(".jpeg"))
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
	}
	else if (InImagePath.EndsWith(".bmp"))
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::BMP);
	}
	else if (InImagePath.EndsWith(".ico"))
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::ICO);

	}
	else if (InImagePath.EndsWith(".exr"))
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::EXR);
	}
	else if (InImagePath.EndsWith(".icns"))
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::ICNS);
	}
	return nullptr;
}
bool UFunctionLibrary::IsMusic(const FString& name)
{
	return name.EndsWith(TEXT(".mp3"), ESearchCase::IgnoreCase) ||
		name.EndsWith(TEXT(".wav"), ESearchCase::IgnoreCase) ||
		name.EndsWith(TEXT(".flac"), ESearchCase::IgnoreCase) ||
		name.EndsWith(TEXT(".aac"), ESearchCase::IgnoreCase) ||
		name.EndsWith(TEXT(".ogg"), ESearchCase::IgnoreCase) ||
		name.EndsWith(TEXT(".ape"), ESearchCase::IgnoreCase) ||
		name.EndsWith(TEXT(".m4a"), ESearchCase::IgnoreCase);
}
UTexture2D* UFunctionLibrary::LoadTexture2D(const FString& path)
{
	UTexture2D* Texture = nullptr;
	if (!FPlatformFileManager::Get().GetPlatformFile().FileExists(*path))
	{
		return nullptr;
	}
	TArray<uint8> RawFileData;
	if (!FFileHelper::LoadFileToArray(RawFileData, *path))
	{
		return nullptr;
	}
	TSharedPtr<IImageWrapper> ImageWrapper = GetImageWrapperByExtention(path);
	if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(RawFileData.GetData(), RawFileData.Num()))
	{
		TArray64<uint8>* RawData = new TArray64<uint8>();
		const ERGBFormat InFormat = ERGBFormat::RGBA;
		if (ImageWrapper->GetRaw(InFormat, 8, *RawData))
		{
			Texture = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_R8G8B8A8);
			if (Texture != nullptr)
			{
				//IsValid = true;
				/*OutWidth = ImageWrapper->GetWidth();
				OutHeight = ImageWrapper->GetHeight();*/
				void* TextureData = Texture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
				FMemory::Memcpy(TextureData, RawData->GetData(), RawData->Num());
				Texture->PlatformData->Mips[0].BulkData.Unlock();
				Texture->UpdateResource();
			}
		}
	}
	return Texture;
}

void UFunctionLibrary::SetUnfocusedVolumeMultiplier()
{
	FApp::SetUnfocusedVolumeMultiplier(1.0f);
}

FString UFunctionLibrary::FindCoverFile(const FString& FilePath)
{
	// 获取文件所在目录
	FString Directory = FPaths::GetPath(FilePath);

	// 获取平台文件接口
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	// 存储找到的cover文件
	FString FoundCoverFile;

	// 遍历目录中的所有文件
	PlatformFile.IterateDirectory(*Directory, [&FoundCoverFile](const TCHAR* Filename, bool bIsDirectory) -> bool
		{
			if (!bIsDirectory) // 只处理文件，忽略目录
			{
				// 获取纯文件名（不含路径）
				FString BaseName = FPaths::GetBaseFilename(Filename);

				// 不区分大小写比较文件名是否为"cover"
				if (BaseName.Equals(TEXT("cover"), ESearchCase::IgnoreCase))
				{
					FoundCoverFile = Filename;
					return false; // 找到后停止遍历
				}
			}
			return true; // 继续遍历
		});

	return FoundCoverFile;
}

FColor UFunctionLibrary::GetMainColor(UTexture2D* texture)
{
	if (!texture || !texture->PlatformData) return FColor::Black;

	FTexture2DMipMap& Mip = texture->PlatformData->Mips[0];
	uint8* Data = static_cast<uint8*>(Mip.BulkData.Lock(LOCK_READ_ONLY));

	int32 Width = texture->GetSizeX();
	int32 Height = texture->GetSizeY();
	int32 max = Width * Height;
	double b = 0, g = 0, r = 0;
	const int step = 10;
	const double total = max / step;
	for (int32 i = 0; i < max; i += step)
	{
		int index = 4 * i;
		r += Data[index + 0];
		g += Data[index + 1];
		b += Data[index + 2];
	}
	FColor PixelColor;
	PixelColor.R = int(r / total);
	PixelColor.G = int(g / total);
	PixelColor.B = int(b / total);
	PixelColor.A = 255;

	Mip.BulkData.Unlock();
	return PixelColor;
}

FString UFunctionLibrary::Int2Time(const int time)
{
	return FString::Printf(TEXT("%02d:%02d"), time / 60, time % 60);
}

float UFunctionLibrary::LerpLimit(const float A, const float B, float alpha, const bool direction) {
	if (alpha > 1.0) {
		return direction ? B : A;
	}
	if (alpha < 0.0) {
		return direction ? A : B;
	}
	return direction ? FMath::Lerp(A, B, alpha) : FMath::Lerp(B, A, alpha);
}

bool UFunctionLibrary::InRange(const float A, const float B, const float C)
{
	if (A > B) {
		return C >= B && C <= A;
	}
	else {
		return C >= A && C <= B;
	}
}

FRotator UFunctionLibrary::AddRotatorDirect(const FRotator& a, const FRotator& b)
{
	return FRotator(a.Pitch + b.Pitch, a.Yaw + b.Yaw, a.Roll + b.Roll);
}

TArray<FString> UFunctionLibrary::GetAllMusicInFolder(const FString& folder)
{
	TArray<FString> Mp3Files;
	// 获取平台文件接口
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

	// 遍历文件夹，收集所有音乐文件
	PlatformFile.IterateDirectoryRecursively(*folder, [&Mp3Files](const TCHAR* FilenameOrDirectory, bool bIsDirectory) {
		if (!bIsDirectory)
		{
			FString FileName(FilenameOrDirectory);
			if (UFunctionLibrary::IsMusic(FileName))
			{
				Mp3Files.Add(FileName);
			}
		}
		return true; // 继续迭代
		});

	return Mp3Files;
}
