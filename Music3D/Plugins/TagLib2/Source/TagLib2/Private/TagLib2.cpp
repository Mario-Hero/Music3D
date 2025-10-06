// Copyright Epic Games, Inc. All Rights Reserved.

#include "TagLib2.h"
#include "Core.h"
#include "TagLib2Library/include/tag.h"
#include "TagLib2Library/include/fileref.h"
#include "TagLib2Library/include/tfilestream.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Engine/Texture2D.h"
#include "ImageUtils.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FTagLib2Module"

void FTagLib2Module::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	// Get the base directory of this plugin
	FString BaseDir = IPluginManager::Get().FindPlugin("TagLib2")->GetBaseDir();

	// Add on the relative location of the third party dll and load it
	FString LibraryPath;
	FString DependPath;
#if PLATFORM_WINDOWS
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/TagLib2Library/Win64/tag.dll"));
	DependPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/TagLib2Library/Win64/zlib1.dll"));
#elif PLATFORM_MAC
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Source/ThirdParty/TagLib2Library/Mac/Release/tag.dylib"));
#elif PLATFORM_LINUX
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Binaries/ThirdParty/TagLib2Library/Linux/x86_64-unknown-linux-gnu/tag.so"));
#endif // PLATFORM_WINDOWS
	if (!DependPath.IsEmpty() && FPlatformProcess::GetDllHandle(*DependPath))
	{
		ExampleLibraryHandle = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath) : nullptr;
		if (ExampleLibraryHandle)
		{
			// Call the test function in the third party library that opens a message box
		}
		else
		{
			FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ThirdPartyLibraryError", "Failed to load tag.dll"));
		}
	}
	else {
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ThirdPartyLibraryError", "Failed to load zlib1.dll"));
	}
	
}

void FTagLib2Module::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	// Free the dll handle
	FPlatformProcess::FreeDllHandle(ExampleLibraryHandle);
	ExampleLibraryHandle = nullptr;
}


IImageWrapperPtr GetImageWrapperByExtention(const FString& InImagePath)
{
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	if (InImagePath.EndsWith("png"))
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);
	}
	else if (InImagePath.EndsWith("jpg") || InImagePath.EndsWith("jpeg"))
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::JPEG);
	}
	else if (InImagePath.EndsWith("bmp"))
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::BMP);
	}
	else if (InImagePath.EndsWith("ico"))
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::ICO);

	}
	else if (InImagePath.EndsWith("exr"))
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::EXR);
	}
	else if (InImagePath.EndsWith("icns"))
	{
		return ImageWrapperModule.CreateImageWrapper(EImageFormat::ICNS);
	}
	return nullptr;
}


UTexture2D* CreateTextureFromBuffer(const TagLib::ByteVector& byteVector)
{
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	const auto formatList = { EImageFormat::PNG, EImageFormat::JPEG };
	for (auto format : formatList) {
		IImageWrapperPtr ImageWrapper = ImageWrapperModule.CreateImageWrapper(format);
		if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(byteVector.data(), byteVector.size()))
		{
			TArray64<uint8>* RawData = new TArray64<uint8>();
			const ERGBFormat InFormat = ERGBFormat::RGBA;
			if (ImageWrapper->GetRaw(InFormat, 8, *RawData))
			{
				auto Texture = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_R8G8B8A8);
				if (Texture != nullptr)
				{
					//IsValid = true;
					void* TextureData = Texture->PlatformData->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
					FMemory::Memcpy(TextureData, RawData->GetData(), RawData->Num());
					Texture->PlatformData->Mips[0].BulkData.Unlock();
					Texture->UpdateResource();
					return Texture;
				}
				else {
					UE_LOG(LogTemp, Warning, TEXT("Failed to create texture"));
				}
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("Failed to get raw image data"));
			}
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Invalid image format"));
		}
	}
	return nullptr;
}

inline FString toFString(const TagLib::String& str)
{
	if (str.isEmpty()) {
		return FString("");
	}
	const std::wstring wstr = str.toCWString();
	if (wstr.empty()) {
		return FString("");
	}
	return FString(wstr.c_str());
}


FMusicTag UTagLib::GetMP3Tag(const FString& path)
{
	const TCHAR* pathTChar = *path;
	FMusicTag tag;
	TagLib::FileRef f(pathTChar);
	if(f.isNull()) {
		UE_LOG(LogTemp, Warning, TEXT("fileref is null: %s"), *path);
		return tag;
	}
	const TagLib::Tag* musicTag = f.tag();
	if (musicTag) {
		tag.title = toFString(musicTag->title());
		tag.artist = toFString(musicTag->artist());
		tag.album = toFString(musicTag->album());
		tag.comment = toFString(musicTag->comment());
		tag.genre = toFString(musicTag->genre());
		tag.year = musicTag->year();
		tag.track = musicTag->track();
		const auto& pictures = musicTag->complexProperties("PICTURE");
		if (!pictures.isEmpty()) {
			const auto& picMap = pictures.front(); // 取第一个封面
			auto mime = picMap.find("mimeType");
			auto mimeType = FString("jpeg");
			if (mime != picMap.end()) {
				mimeType = toFString(mime->second.toString());
				UE_LOG(LogTemp, Display, TEXT("mimeType: %s"), *mimeType);
			}
			const auto pictureType = picMap.find("pictureType");
			if (pictureType != picMap.end()) {
				const auto type = pictureType->second.toString();
				UE_LOG(LogTemp, Display, TEXT("pictureType: %s"), *toFString(type));
			}
			const auto it = picMap.find("data");
			if (it != picMap.end()) {
				bool ok = false;
				if(it->second.type() != TagLib::Variant::ByteVector) {
					UE_LOG(LogTemp, Warning, TEXT("data in picture is not ByteVector: %s"), *path);
					return tag;
				}
				auto byteVector = it->second.value<TagLib::ByteVector>(&ok);
				if (ok) {
					byteVector.resize(byteVector.size()+2, 0);
					UE_LOG(LogTemp, Display, TEXT("picture size: %u"), byteVector.size());
					/*
					// For debug: write picture to local file
					std::ofstream picOut;
					picOut.open("C:/CloudMusic/out.jpg", std::ios_base::out | std::ios_base::binary);
					picOut << byteVector;
					picOut.close();
					*/
					tag.cover = CreateTextureFromBuffer(byteVector);
				}
				else {
					UE_LOG(LogTemp, Warning, TEXT("data in picture to ByteVector fail: %s"), *path);
				}
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("no data in picture: %s"), *path);
			}
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("cover is empty: %s"), *path);
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Get tag failed: %s"), *path);
	}
	return tag;
}

void UTagLib::PrintTag(const FMusicTag& tag) {
	UE_LOG(LogTemp, Display, TEXT("title: %s"), *tag.title);
	UE_LOG(LogTemp, Display, TEXT("artist: %s"), *tag.artist);
	UE_LOG(LogTemp, Display, TEXT("album: %s"), *tag.album);
	UE_LOG(LogTemp, Display, TEXT("comment: %s"), *tag.comment);
	UE_LOG(LogTemp, Display, TEXT("genre: %s"), *tag.genre);
	UE_LOG(LogTemp, Display, TEXT("year: %d"), tag.year);
	UE_LOG(LogTemp, Display, TEXT("track: %d"), tag.track);
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FTagLib2Module, TagLib2)
