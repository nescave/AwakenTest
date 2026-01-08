// Fill out your copyright notice in the Description page of Project Settings.


#include "General/DebugCVars.h"

namespace FASCVars
{
	int32 ASDebugDraw = 0;
	static FAutoConsoleVariableRef CVarDebugDraw(TEXT("AS.DebugDraw"),ASDebugDraw,
	TEXT("Controls all debug draws in AS project."), ECVF_Default);
	float ASDebugDrawDuration = 2.f;
	static FAutoConsoleVariableRef CVarDebugDrawDuration(TEXT("AS.DebugDraw.Duration"),ASDebugDrawDuration,
	TEXT("Controls all debug draw durations."), ECVF_Default);
	int32 ASDrawSlideVelocity = 1;
	static FAutoConsoleVariableRef CVarDrawSlideVelocity(TEXT("AS.DebugDraw.SlideVelocity"),ASDrawSlideVelocity,
		TEXT("Draw slide velocity vector."), ECVF_Default);
	
	int32 ASDrawSlideHit = 1;
	static FAutoConsoleVariableRef CVarDrawSlideHit(TEXT("AS.DebugDraw.SlideHit"),ASDrawSlideHit,
		TEXT("Draw slide hit box in front of the character."), ECVF_Default);
	int32 ASDrawDetailWallRun = 1;
	static FAutoConsoleVariableRef CVarDrawDetailWallRun(TEXT("AS.DebugDraw.DetailWallRun"),ASDrawDetailWallRun,
		TEXT("Draw wall run detail."), ECVF_Default);
	int32 ASDrawWallCatchTests = 0;
	static FAutoConsoleVariableRef CVarDrawWallCatchTests(TEXT("AS.DebugDraw.WallCatchTests"),ASDrawWallCatchTests,
		TEXT("Draw wall catch tests."), ECVF_Default);
	int32 ASDrawHangingPoint = 1;
	static FAutoConsoleVariableRef CVarDrawHangingPoint(TEXT("AS.DebugDraw.HangingPoint"),ASDrawHangingPoint,
		TEXT("Draw hanging point and normal. Green if it is ledge, blue otherwise"), ECVF_Default);
	int32 ASDrawWallRunDirection = 1;
	static FAutoConsoleVariableRef CVarDrawWallRunDirection(TEXT("AS.DebugDraw.WallRunDirection"),ASDrawWallRunDirection,
		TEXT("Draw wall run direction arrow."), ECVF_Default);
	int32 ASDrawHitBoxes = 1;
	static FAutoConsoleVariableRef CVarDrawHitBoxes(TEXT("AS.DebugDraw.HitBoxes"),ASDrawHitBoxes,
		TEXT("Draw all hitboxes, players and enemies."), ECVF_Default);
	int32 ASDrawShotTraces = 1;
	static FAutoConsoleVariableRef CVarDrawShotTraces(TEXT("AS.DebugDraw.ShotTraces"),ASDrawShotTraces,
		TEXT("Draw shot traces."), ECVF_Default);
	int32 ASDrawInteractionsZone = 0;
	static FAutoConsoleVariableRef CVarDrawInteractionsZone(TEXT("AS.DebugDraw.InteractionsZone"),ASDrawInteractionsZone,
		TEXT("Draw interactions zone."), ECVF_Default);
	int32 ASDrawDetailedInteractions = 1;
	static FAutoConsoleVariableRef CVarDrawDetailedInteractions(TEXT("AS.DebugDraw.DetailedInteractions"),ASDrawDetailedInteractions,
		TEXT("Draw detailed interactions."), ECVF_Default);
	int32 ASDrawVelocityClamping = 0;
	static FAutoConsoleVariableRef CVarDrawVelocityClamping(TEXT("AS.DebugDraw.VelocityClamping"),ASDrawVelocityClamping,
		TEXT("Draw velocity clamping."), ECVF_Default);
	int32 ASDrawBotTargeting = 0;
	static FAutoConsoleVariableRef CVarDrawBotTargeting(TEXT("AS.DebugDraw.BotTargeting"),ASDrawBotTargeting,
		TEXT("Draw bot targeting."), ECVF_Default);
	int32 ASDrawBotPathfinding = 0;
	static FAutoConsoleVariableRef CVarDrawBotPathfinding(TEXT("AS.DebugDraw.BotPathfinding"),ASDrawBotPathfinding,
		TEXT("Draw bot pathfinding."), ECVF_Default);
	int32 ASDrawFireCone = 0;
	static FAutoConsoleVariableRef CVarDrawFireCone(TEXT("AS.DebugDraw.FireCone"),ASDrawFireCone,
		TEXT("Draw fire cone."), ECVF_Default);
	int32 ASDrawThrowTargeting = 0;
	static FAutoConsoleVariableRef CVarDrawThrowTargeting(TEXT("AS.DebugDraw.ThrowTargeting"),ASDrawThrowTargeting,
		TEXT("Draw throw targeting."), ECVF_Default);
	int32 AILineOfSightTraces = 0;
	static FAutoConsoleVariableRef CVarLineOfSight(TEXT("AS.AI.LineOfSightTraces"),AILineOfSightTraces,
		TEXT("Line of sight check."), ECVF_Default);
	
	int32 ASGodMode = 0;
	static FAutoConsoleVariableRef CVarGodMode(TEXT("AS.GodMode"), ASGodMode,
			TEXT("GodMode. No damage taken."), ECVF_Default);
 
}
