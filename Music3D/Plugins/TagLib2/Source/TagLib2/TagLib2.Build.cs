// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.IO;

public class TagLib2 : ModuleRules
{
    private string ModulePath
    {
        get { return ModuleDirectory; }
    }

    private string SourceThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../../Source/ThirdParty/")); }
    }

    private string BinariesThirdPartyPath
    {
        get { return Path.GetFullPath(Path.Combine(ModulePath, "../../Binaries/ThirdParty/")); }
    }

    public TagLib2(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
                Path.Combine(SourceThirdPartyPath, "TagLib2Library", "include"),
				// ... add public include paths required here ...
			}
			);
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
                Path.Combine(SourceThirdPartyPath, "TagLib2Library", "include"),
				// ... add other private include paths required here ...
			}
			);

		if (Target.Platform == UnrealTargetPlatform.Win64 || Target.Platform == UnrealTargetPlatform.Win32)
		{
            PublicAdditionalLibraries.Add(Path.Combine(SourceThirdPartyPath, "TagLib2Library", "lib", "Win64", "tag.lib"));
            PublicDelayLoadDLLs.Add("tag.dll");
            PublicDelayLoadDLLs.Add("zlib1.dll");
            RuntimeDependencies.Add(Path.Combine(BinariesThirdPartyPath, "TagLib2Library", "Win64", "zlib1.dll"));
            RuntimeDependencies.Add(Path.Combine(BinariesThirdPartyPath, "TagLib2Library", "Win64", "tag.dll"));
        }
        PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"TagLib2Library",
				"CoreUObject", 
				"Engine"
				// ... add other public dependencies that you statically link with here ...
			}
			);
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
                "ImageWrapper",
                "Projects"
				// ... add private dependencies that you statically link with here ...	
			}
			);
		
		
		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
