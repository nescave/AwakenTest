// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class AwakenShooterEditorTarget : TargetRules
{
	public AwakenShooterEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.V6;
		IncludeOrderVersion = EngineIncludeOrderVersion.Unreal5_7;
		UndefinedIdentifierWarningLevel = WarningLevel.Error;

		ExtraModuleNames.Add("AwakenShooter");
	}
}
