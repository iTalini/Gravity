// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Gravity : ModuleRules
{
	public Gravity(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "UnrealEd", "InputCore", "HeadMountedDisplay", "PhysicsCore", "UMG", "Slate", "SlateCore"});
	}
}
