// Fill out your copyright notice in the Description page of Project Settings.

using System.IO;
using UnrealBuildTool;

public class TagLib2Library : ModuleRules
{
    public TagLib2Library(ReadOnlyTargetRules Target) : base(Target)
    {
        Type = ModuleType.External;

        PublicIncludePaths.Add(Path.Combine(ModuleDirectory, "include"));

        if (Target.Platform == UnrealTargetPlatform.Win64)
        {
            // Add the import library
            PublicAdditionalLibraries.Add(Path.Combine(ModuleDirectory, "lib", "Win64", "tag.lib"));

            // Delay-load the DLL, so we can load it from the right place first
            PublicDelayLoadDLLs.Add("tag.dll");
            PublicDelayLoadDLLs.Add("zlib1.dll");

            // Ensure that the DLL is staged along with the executable
            RuntimeDependencies.Add(Path.Combine(ModuleDirectory, "bin", "Win64", "tag.dll"));
            RuntimeDependencies.Add(Path.Combine(ModuleDirectory, "bin", "Win64", "zlib1.dll"));
        }
        else if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            PublicDelayLoadDLLs.Add(Path.Combine(ModuleDirectory, "lib", "Mac", "tag.dylib"));
            //RuntimeDependencies.Add(Path.Combine(ModuleDirectory, "bin", "Mac", "tag.dll"));
        }
        else if (Target.Platform == UnrealTargetPlatform.Linux)
        {
            PublicDelayLoadDLLs.Add(Path.Combine(ModuleDirectory, "lib", "Linux", "tag.so"));
        }
    }
}
