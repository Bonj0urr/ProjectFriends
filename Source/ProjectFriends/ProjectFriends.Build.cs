// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ProjectFriends : ModuleRules
{
	public ProjectFriends(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });

        PublicIncludePaths.AddRange(new string[]
        {
            "ProjectFriends/Public/Core",
            "ProjectFriends/Public/Components",
            "ProjectFriends/Public/Items"
        });
    }
}
