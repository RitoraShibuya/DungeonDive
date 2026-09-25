// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class Trial_Lesson : ModuleRules
{
	public Trial_Lesson(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });
	}
}
