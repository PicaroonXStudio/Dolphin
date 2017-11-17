// Some copyright should be here...

using UnrealBuildTool;

public class Dolphin : ModuleRules
{
	public Dolphin(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
		
		PublicIncludePaths.AddRange(
			new string[] {
				"Dolphin/Public",
                "Dolphin/Bridge",
				// ... add public include paths required here ...
			}
            );
				
		
		PrivateIncludePaths.AddRange(
			new string[] {
				"Dolphin/Private",
				// ... add other private include paths required here ...
			}
            );
			
		
		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"PahoMQTT",
                "ProtoBuf",
                "SchweizerMesser",
				// ... add other public dependencies that you statically link with here ...
			}
            );
			
		
		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
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
