// Fill out your copyright notice in the Description page of Project Settings.


#include "WindowLibrary.h"
#include "Engine/GameEngine.h"
#include "EngineGlobals.h"
#include "GameFramework/GameUserSettings.h" // 添加此头文件

bool UWindowLibrary::SetWindowMinimize() {
	UGameEngine* gameEngine = Cast<UGameEngine>(GEngine);
	if (gameEngine) {
		TSharedPtr<SWindow> windowPtr = gameEngine->GameViewportWindow.Pin();
		SWindow* window = windowPtr.Get();
		if (window) {
			window->Minimize(); // 调用 Minimize 方法最小化窗口
			return true;
		}
	}
	return false;
}

bool UWindowLibrary::SetWindowMaximize() {
	UGameEngine* gameEngine = Cast<UGameEngine>(GEngine);
	if (gameEngine) {
		TSharedPtr<SWindow> windowPtr = gameEngine->GameViewportWindow.Pin();
		SWindow* window = windowPtr.Get();
		if (window) {
			window->Maximize(); // 调用 Maximize 方法最大化窗口
			return true;
		}
	}
	return false;
}

bool UWindowLibrary::SetWindowRestore()
{
	UGameEngine* gameEngine = Cast<UGameEngine>(GEngine);
	if (gameEngine) {
		TSharedPtr<SWindow> windowPtr = gameEngine->GameViewportWindow.Pin();
		SWindow* window = windowPtr.Get();
		if (window) {
			window->Restore(); // 调用 Restore 方法还原窗口
			return true;
		}
	}
	return false;
}

void UWindowLibrary::WindowMode()
{
	UGameUserSettings* Settings = GEngine->GetGameUserSettings();
	Settings->SetFullscreenMode(EWindowMode::Windowed); // 切换到窗口模式
	Settings->ApplySettings(false);
}

