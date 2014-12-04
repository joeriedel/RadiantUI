

using UnrealBuildTool;
using System.Collections.Generic;

public class RadiantUI_SDKEditorTarget : TargetRules
{
	public RadiantUI_SDKEditorTarget(TargetInfo Target)
	{
		Type = TargetType.Editor;
	}

	//
	// TargetRules interface.
	//

	public override void SetupBinaries(
		TargetInfo Target,
		ref List<UEBuildBinaryConfiguration> OutBuildBinaryConfigurations,
		ref List<string> OutExtraModuleNames
		)
	{
		OutExtraModuleNames.AddRange( new string[] { "RadiantUI_SDK" } );
	}
}
