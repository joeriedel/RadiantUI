// Copyright 2014 Joseph Riedel, Inc. All Rights Reserved.

using System.IO;

namespace UnrealBuildTool.Rules
{
	public class RadiantUI : ModuleRules
	{
		public RadiantUI(TargetInfo Target)
		{
			PublicIncludePaths.AddRange(
				new string[] {
				}
			);

			PrivateIncludePaths.AddRange(
				new string[] {
					"RadiantUI/Private"
				}
			);

			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
					"CoreUObject",
                    "Engine",
					"SlateCore",
					"Slate"
				}
			);

			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"InputCore",
                    "RenderCore",
                    "ShaderCore",
                    "RHI"
				}
			);

			if ((Target.Configuration == UnrealTargetConfiguration.Debug) || (Target.Configuration == UnrealTargetConfiguration.DebugGame)) 
			{
				Definitions.Add("RADIANTUI_DEBUG=1");
			}
		}
	}
}