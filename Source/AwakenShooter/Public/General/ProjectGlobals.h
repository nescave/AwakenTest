#pragma once

namespace ASCollisionProfile
{
	const FName Enemies_ProfileName = FName(TEXT("Enemies"));
	const FName Gun_ProfileName = FName(TEXT("Gun"));
}

namespace ASTraceChannel
{
	constexpr ECollisionChannel Projectiles = ECC_GameTraceChannel1;
	constexpr ECollisionChannel Interactions = ECC_GameTraceChannel2;
	constexpr ECollisionChannel Enemy = ECC_GameTraceChannel3;
	constexpr ECollisionChannel EnemyTrace = ECC_GameTraceChannel4;
}
