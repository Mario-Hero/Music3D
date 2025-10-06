// Fill out your copyright notice in the Description page of Project Settings.


#include "WindowResize.h"
#include <Kismet/GameplayStatics.h>

// Sets default values
AWindowResize::AWindowResize()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

// Called when the game starts or when spawned
void AWindowResize::BeginPlay()
{
	Super::BeginPlay();
    // 在源文件中的初始化函数里
    if (GEngine && GEngine->GameViewport)
    {
        FViewport* Viewport = GEngine->GameViewport->Viewport;
        if (Viewport)
        {
            // 绑定回调函数
            Viewport->ViewportResizedEvent.AddUObject(this, &AWindowResize::OnViewportResized);
        }
        Window = GEngine->GameViewport->GetWindow();
		Window->GetOnWindowActivatedEvent().AddUFunction(this, FName("OnWindowActive"));
		Window->GetOnWindowDeactivatedEvent().AddUFunction(this, FName("OnWindowDeactivated"));
        World = GetWorld();
    }
	
}

// Called every frame
void AWindowResize::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWindowResize::OnViewportResized(FViewport* Viewport, uint32 num)
{
    if (Viewport)
    {
        int32 NewWidth = Viewport->GetSizeXY().X;
        int32 NewHeight = Viewport->GetSizeXY().Y;
        Window->Resize(FVector2D(NewWidth, (NewWidth*9)/16));
    }
}

void AWindowResize::OnWindowActive()
{
    GetWorld()->Exec(GetWorld(), TEXT("t.MaxFPS 360"));
    UGameplayStatics::SetEnableWorldRendering(World, true);
}

void AWindowResize::OnWindowDeactivated()
{
    GetWorld()->Exec(GetWorld(), TEXT("t.MaxFPS 3"));
    UGameplayStatics::SetEnableWorldRendering(World, false);
}

