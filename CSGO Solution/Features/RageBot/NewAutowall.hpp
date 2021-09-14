#pragma once
#include "../SDK/Includes.hpp"
#include "../SDK/Math/Math.hpp"

namespace NewAutoWall
{
	static bool has_armour ( C_BasePlayer * player , int hitgroup ) 
	{
		if ( hitgroup == HITGROUP_HEAD && player->m_bHasHelmet ( ) )
			return true;
		else if ( hitgroup >= HITGROUP_CHEST && hitgroup <= HITGROUP_RIGHTARM )
			return true;

		return false;
	}

	static float scale_damage ( C_BasePlayer * player , float damage , float armour_ratio , int hitgroup ) {
		float heavy_ratio {} , bonus_ratio {} , ratio {} , new_damage {};

		switch ( hitgroup ) {
		case HITGROUP_HEAD:
			if ( player->m_bHasHeavyArmor ( ) )
				damage *= 2.0f;
			else
				damage *= 4.f;
			break;
		case HITGROUP_STOMACH:
			damage *= 1.25f;
			break;
		case HITGROUP_LEFTLEG:
		case HITGROUP_RIGHTLEG:
			damage *= 0.75f;
			break;
		}

		if ( !player->m_ArmourValue ( ) )
			return std::floor ( damage );

		if ( has_armour ( player , hitgroup ) ) {
			heavy_ratio = 1.f;
			bonus_ratio = 0.5f;
			ratio = armour_ratio * 0.5f;

			if ( player->m_bHasHeavyArmor ( ) ) {
				bonus_ratio = 0.33f;
				ratio = armour_ratio * 0.25f;
				heavy_ratio = 0.33f;
				new_damage = ( damage * ratio ) * 0.85f;
			} else {
				new_damage = damage * ratio;
			}

			if ( ( ( damage - new_damage ) * ( heavy_ratio * bonus_ratio ) ) > player->m_ArmourValue ( ) )
				new_damage = damage - ( player->m_ArmourValue ( ) / bonus_ratio );

			damage = new_damage;
		}

		return std::floor ( damage );
	}

	static void ClipTraceToPlayers( Vector vecStart, Vector vecEnd, CGameTrace* Trace, int nMask )
	{
		Ray_t Ray;
		Ray.Init( vecStart, vecEnd );

		CGameTrace NewTrace;
		std::array < uintptr_t, 5 > aFilter
		=
		{
			*( std::uintptr_t* )( g_Globals.m_AddressList.m_TraceFilterSkipTwoEntities ),
			NULL,
			NULL,
			NULL,
			( std::uintptr_t )( Trace->hit_entity ),
		};

		CTraceFilter* pTraceFilter = ( CTraceFilter* )( aFilter.data( ) );
		for ( int iPlayerID = 1; iPlayerID <= g_Globals.m_Interfaces.m_GlobalVars->m_iMaxClients; iPlayerID++ )
		{
			C_BasePlayer* pPlayer = C_BasePlayer::GetPlayerByIndex( iPlayerID );
			if ( !pPlayer || !pPlayer->IsPlayer( ) || !pPlayer->IsAlive( ) || pPlayer->IsDormant( ) || pPlayer->m_iTeamNum( ) == g_Globals.m_LocalPlayer->m_iTeamNum( ) )
				continue;

			if ( pTraceFilter && !pTraceFilter->ShouldHitEntity( pPlayer, NULL ) )
				continue;

			float_t flRange = Math::DistanceToRay( pPlayer->WorldSpaceCenter( ), vecStart, vecEnd );
			if ( flRange < 0.0f || flRange > 60.0f )
				continue;

			g_Globals.m_Interfaces.m_EngineTrace->ClipRayToEntity( Ray, 0x40000000 | nMask, pPlayer, &NewTrace );
			if ( NewTrace.fraction > Trace->fraction )
				std::memcpy( Trace, &NewTrace, sizeof( CGameTrace ) );
		}
	}

	static bool TraceToExit( Vector vecStart, Vector & vecEnd, Vector vecDirection, trace_t* pEnterTrace, trace_t* pExitTrace )
	{
		Vector          vecNewEnd;
		int				nFirstContents = -1;

		float_t flDistance = 0.0f;
		while ( flDistance <= 90.0f )
		{
			// increase distance
			flDistance += 4.0f;

			// calc new end
			vecEnd = vecStart + ( vecDirection * flDistance );

			// cache contents
			int nContents = g_Globals.m_Interfaces.m_EngineTrace->GetPointContents( vecEnd, 0x4600400B, nullptr );
			if ( nFirstContents == -1 )
				nFirstContents = nContents;

			// check contents
			if ( nContents & 0x600400B && ( !( nContents & 0x40000000 ) || nFirstContents == nContents ) )
				continue;

			// new end
			vecNewEnd = vecEnd - ( vecDirection * 4.0f );

			// trace line
			g_Globals.m_Interfaces.m_EngineTrace->TraceLine( vecEnd, vecStart, 0x4600400B, g_Globals.m_LocalPlayer, NULL, pExitTrace );
		
			// clip trace to player
			ClipTraceToPlayers( vecNewEnd, vecEnd, pExitTrace, 0x4600400B );
			
			// check solid and falgs
			if ( pExitTrace->startsolid && pExitTrace->surface.flags & SURF_HITBOX )
			{
				std::array < uintptr_t, 5 > aSkipTwoEntities 
				=
				{
					*( std::uintptr_t* )( g_Globals.m_AddressList.m_TraceFilterSkipTwoEntities ),
					NULL,
					NULL,
					NULL,
					( std::uintptr_t )( pExitTrace->hit_entity ),
				};

				Ray_t NewRay;
				NewRay.Init( vecEnd, vecStart );

				g_Globals.m_Interfaces.m_EngineTrace->TraceRay( NewRay, 0x600400B, ( CTraceFilter* )( aSkipTwoEntities.data( ) ), pExitTrace );
				if ( pExitTrace->DidHit( ) && !pExitTrace->startsolid )
				{
					vecEnd = pExitTrace->endpos;
					return true;
				}

				continue;
			}

			if ( !pExitTrace->DidHit( ) || pExitTrace->startsolid )
			{
				if ( pEnterTrace->hit_entity != g_Globals.m_Interfaces.m_EntityList->GetClientEntity( 0 ) ) 
				{
					if ( pExitTrace->hit_entity && ( ( C_BaseEntity* )( pExitTrace->hit_entity ) )->IsBreakableEntity( ) )
					{
						pExitTrace->surface.surfaceProps = pEnterTrace->surface.surfaceProps;
						pExitTrace->endpos = vecStart + vecDirection;

						return true;
					}
				}

				continue;
			}

			if ( pExitTrace->surface.flags & 0x80 )
			{
				if ( pEnterTrace->hit_entity && ( ( C_BaseEntity* )( pEnterTrace->hit_entity ) )->IsBreakableEntity( ) && pExitTrace->hit_entity && ( ( C_BaseEntity* )( pExitTrace->hit_entity ) )->IsBreakableEntity( ) )
				{
					vecEnd = pExitTrace->endpos;
					return true;
				}

				if ( !( pEnterTrace->surface.flags & 0x80u ) )
					continue;
			}

			if ( pExitTrace->plane.normal.Dot( vecDirection ) > 1.0f )
				return false;

			vecEnd -= vecDirection * ( pExitTrace->fraction * 4.0f );
			return true;
		}

		return false;
	}

	struct pen_out_t 
	{
		C_BasePlayer * m_to;
		float m_damage = 0.f;
		int m_hitgroup = 0;
		bool m_ran = false;
		bool m_penetrate = false;
		bool m_success = false;
	};

	struct pen_argument_t {
		/* input data */
		C_BasePlayer * m_from , * m_to;
		Vector m_pos , m_shoot_position;
		float m_damage , m_damage_penetration;
		bool m_can_penetrate;
		bool m_center;

		/* output data */
		pen_out_t m_out_data {};
	};

	struct multithreaded_pen_input_t {
		pen_argument_t * m_pen_argb;
		bool * m_done;
	};

	inline pen_argument_t xhair_penetration {};

	static void run_penetration ( pen_argument_t * argument ) {
		float damage , penetration , penetration_mod , player_damage , remaining , trace_len {} , total_pen_mod , damage_mod , modifier , damage_lost;
		int pen { 4 } , enter_material , exit_material;
		bool nodraw , grate;
		argument->m_out_data.m_ran = true;

		surfacedata_t * enter_surface , * exit_surface;
		Vector start , dir , end , pen_end;
		trace_t trace , exit_trace;
		C_BaseCombatWeapon * weapon;
		C_CSWeaponData * weapon_info; 
		std::array < std::uintptr_t, 4 > aFilter
			=
		{
			*(std::uintptr_t*)(g_Globals.m_AddressList.m_TraceFilterSimple),
			(uintptr_t)(g_Globals.m_LocalPlayer),
			NULL,
			NULL
		};

		if ( !g_Globals.m_LocalPlayer ) {
			argument->m_out_data.m_success = false;
			return;
		}

		/* if we are tracing from our local player perspective. */
		if ( argument->m_from == g_Globals.m_LocalPlayer ) {
			weapon = g_Globals.m_LocalPlayer->m_hActiveWeapon( ).Get( );
			if ( !weapon ) {
				argument->m_out_data.m_success = false;
				return;
			}

			weapon_info = weapon->GetWeaponData ( );
			if ( !weapon_info ) {
				argument->m_out_data.m_success = false;
				return;
			}

			start = argument->m_shoot_position;
		}

		/* not local player. */
		else {
			weapon = argument->m_from->m_hActiveWeapon( ).Get ( );
			if ( !weapon ) {
				argument->m_out_data.m_success = false;
				return;
			}

			/* get weapon info. */
			weapon_info = weapon->GetWeaponData ( );
			if ( !weapon_info ) {
				argument->m_out_data.m_success = false;
				return;
			}

			/* set trace start. */
			start = argument->m_shoot_position;
		}

		/* get some weapon data. */
		damage = ( float ) weapon_info->m_iDamage;
		penetration = weapon_info->m_flPenetration;

		/* used later in calculations. */
		penetration_mod = fmax ( 0.f , ( 3.f / penetration ) * 1.25f );

		/* get direction to end point. */
		dir = ( argument->m_pos - start ).Normalized ( );

		/* OPTIMIZATION: if we're below minimum damage even for head, stop */
		while ( damage > 0.f && ( damage * 4 ) >= argument->m_damage_penetration && ( damage * 4 ) >= argument->m_damage ) {
			/* calculating remaining len. */
			remaining = weapon_info->m_flRange - trace_len;

			/* set trace end. */
			end = start + ( dir * remaining );

			g_Globals.m_Interfaces.m_EngineTrace->TraceLine( start, end, 0x4600400B, g_Globals.m_LocalPlayer, NULL, &trace );
			if ( trace.fraction == 1.f ) {
				argument->m_out_data.m_success = false;
				return;
			}

			/* check for player hitboxes extending outside their collision bounds. */
			/* if no target is passed we clip the trace to a specific player, otherwise we clip the trace to any player. */
			if ( argument->m_to )
				ClipTraceToPlayers( start, end + ( dir * 40.0f ), &trace, 0x4600400B );

			/* calculate damage based on the distance the bullet traveled. */
			trace_len += trace.fraction * remaining;
			damage *= std::pow ( weapon_info->m_flRangeModifier , trace_len / 500.f );

			/* if a target was passed. */
			if ( argument->m_to ) {
				/* validate that we hit the target we aimed for. */
				/* @TODO: valid hitgroup check here -swoopae */
				if ( trace.hit_entity && trace.hit_entity == argument->m_to && ( trace.hitgroup != 0 && trace.hitgroup != 10 ) ) {
					int group = ( weapon->m_iItemDefinitionIndex( ) == WEAPON_TASER ) ? HITGROUP_GENERIC : trace.hitgroup;

					/* scale damage based on the hitgroup we hit. */
					player_damage = scale_damage ( argument->m_to , damage , weapon_info->m_flArmorRatio , group );

					// set result data for when we hit a player. */
					argument->m_out_data.m_penetrate = pen != 4;
					argument->m_out_data.m_hitgroup = group;
					argument->m_out_data.m_damage = player_damage;
					argument->m_out_data.m_to = argument->m_to;

					/* non-penetrate damage. */
					if ( pen == 4 ) {
						argument->m_out_data.m_success = player_damage >= argument->m_damage;
						return;
					}

					argument->m_out_data.m_success = player_damage >= argument->m_damage_penetration;
					return;
				}
			}

			/* no target was passed, check for any player hit or just get final damage done. */
			else {
				argument->m_out_data.m_penetrate = pen != 4;

				auto player = reinterpret_cast< C_BasePlayer * >( trace.hit_entity );

				/* maybe add team checks / other checks / etc. */
				if ( player && player->EntIndex( ) && player->EntIndex( ) > 0 && player->EntIndex ( ) < 65 && ( trace.hitgroup != 0 && trace.hitgroup != 10 ) ) {
					int group = ( weapon->m_iItemDefinitionIndex ( ) == WEAPON_TASER ) ? HITGROUP_GENERIC : trace.hitgroup;

					player_damage = scale_damage ( player , damage , weapon_info->m_flArmorRatio , group );

					/* set result data for when we hit a player. */
					argument->m_out_data.m_hitgroup = group;
					argument->m_out_data.m_damage = player_damage;
					argument->m_out_data.m_to = player;

					/* non-penetrate damage. */
					if ( pen == 4 ) {
						argument->m_out_data.m_success = player_damage >= argument->m_damage;
						return;
					}

					argument->m_out_data.m_success = player_damage >= argument->m_damage_penetration;
					return;
				}

				/* if we've reached here then we didn't hit a player yet, set damage and hitgroup. */
				argument->m_out_data.m_damage = damage;
			}

			/* don't run pen code if it's not wanted. */
			if ( !argument->m_can_penetrate ) {
				argument->m_out_data.m_success = false;
				return;
			}

			/* get surface at entry point. */
			enter_surface = g_Globals.m_Interfaces.m_PropPhysics->GetSurfaceData ( trace.surface.surfaceProps );

			/* this happens when we're too far away from a surface and can penetrate walls or the surface's pen modifier is too low. */
			if ( ( trace_len > 3000.f && penetration ) || enter_surface->game.flPenetrationModifier < 0.1f ) {
				argument->m_out_data.m_success = false;
				return;
			}

			/* store data about surface flags / contents. */
			nodraw = ( trace.surface.flags & SURF_NODRAW );
			grate = ( trace.contents & CONTENTS_GRATE );

			/* get material at entry point. */
			enter_material = enter_surface->game.material;

			/* some extra stuff the game does. */
			if ( !pen && !nodraw && !grate && enter_material != CHAR_TEX_GRATE && enter_material != CHAR_TEX_GLASS ) {
				argument->m_out_data.m_success = false;
				return;
			}

			// no more pen. */
			if ( penetration <= 0.f || pen <= 0 ) {
				argument->m_out_data.m_success = false;
				return;
			}

			/* try to penetrate object. */
			if ( !TraceToExit( trace.endpos, pen_end, dir, &trace, &exit_trace ) ) {
				if ( !( g_Globals.m_Interfaces.m_EngineTrace->GetPointContents ( pen_end , MASK_SHOT_HULL ) & MASK_SHOT_HULL ) ) {
					argument->m_out_data.m_success = false;
					return;
				}
			}

			/* get surface / material at exit point. */
			exit_surface = g_Globals.m_Interfaces.m_PropPhysics->GetSurfaceData ( exit_trace.surface.surfaceProps );
			exit_material = exit_surface->game.material;

			/* @TODO; check for CHAR_TEX_FLESH and ff_damage_bullet_penetration / ff_damage_reduction_bullets convars? */
			/*             also need to check !isbasecombatweapon too. */
			/* @TODO ABOVE^^^^^ */
			if ( enter_material == CHAR_TEX_GRATE || enter_material == CHAR_TEX_GLASS ) {
				total_pen_mod = 3.f;
				damage_mod = 0.05f;
			}

			else if ( nodraw || grate ) {
				total_pen_mod = 1.f;
				damage_mod = 0.16f;
			}

			else {
				total_pen_mod = ( enter_surface->game.flPenetrationModifier + exit_surface->game.flPenetrationModifier ) * 0.5f;
				damage_mod = 0.16f;
			}

			/* thin metals, wood and plastic get a penetration bonus. */
			if ( enter_material == exit_material ) {
				if ( exit_material == CHAR_TEX_CARDBOARD || exit_material == CHAR_TEX_WOOD )
					total_pen_mod = 3.f;

				else if ( exit_material == CHAR_TEX_PLASTIC )
					total_pen_mod = 2.f;
			}

			/* set some local vars. */
			trace_len = ( exit_trace.endpos - trace.endpos ).LengthSqr ( );
			modifier = fmaxf ( 1.f / total_pen_mod , 0.f );

			/* this calculates how much damage we've lost depending on thickness of the wall, our penetration, damage, and the modifiers set earlier */
			damage_lost = fmaxf (
				( ( modifier * trace_len ) / 24.f )
				+ ( ( damage * damage_mod )
				+ ( fmaxf ( 3.75 / penetration , 0.f ) * 3.f * modifier ) ) , 0.f );

			/* subtract from damage. */
			damage -= fmax ( 0.f , damage_lost );
			if ( damage < 1.f ) {
				argument->m_out_data.m_success = false;
				return;
			}

			/* set new start pos for successive trace. */
			start = exit_trace.endpos;

			/* decrement pen. */
			--pen;
		}

		argument->m_out_data.m_success = false;
		return;
	}
}