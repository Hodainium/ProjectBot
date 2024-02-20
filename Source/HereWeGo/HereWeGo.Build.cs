// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class HereWeGo : ModuleRules
{
	public HereWeGo(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput", "GameplayTags", "UMG", "Text3D", "GameplayTasks", "GameplayAbilities", "NetCore", "Slate", "SlateCore", "Niagara", "PhysicsCore", "RamaMeleePlugin", "GameplayMessageRuntime", "AsyncMixin", "AIModule", "CommonInput" });

		PrivateDependencyModuleNames.AddRange(new string[] { "CommonUI" });

        PrivateDependencyModuleNames.AddRange(new string[] { "ModularGameplay", "ModularGameplayActors", "CommonGame", "CommonUser" });

        //PublicIncludePaths.AddRange(new string[] { "RamaMeleePlugin" });

        // Uncomment if you are using Slate UI
        // PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });

        // Uncomment if you are using online features
        PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
