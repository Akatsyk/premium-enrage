#include "Chams.hpp"
#include "../SDK/Game/KeyValues.hpp"
#include "../Animations/LocalAnimations.hpp"
#include "../Exploits/Exploits.hpp"

void C_Chams::OnModelRender( IMatRenderContext* pCtx, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4_t* aMatrix )
{
	bool bIsPlayer = strstr( pInfo.pModel->szName, _S( "models/player" ) );
    bool bIsArm = strstr( pInfo.pModel->szName, _S( "arms" ) );
    bool bIsWeapon = strstr( pInfo.pModel->szName, _S( "weapons/v_" ) );

	C_BaseEntity* pBaseEntity = reinterpret_cast < C_BaseEntity* > ( g_Globals.m_Interfaces.m_EntityList->GetClientEntity( pInfo.entity_index ) );
	if ( pBaseEntity )
	{
		if ( pBaseEntity->IsPlayer( ) )
			bIsPlayer = true;
	}

	if ( bIsPlayer )
    {
	    C_BasePlayer* pPlayer = ( C_BasePlayer* )( pBaseEntity );
	    if ( !pPlayer )
		{
			g_Globals.m_Hooks.m_Originals.m_DrawModelExecute_Model( g_Globals.m_Interfaces.m_ModelRender, pCtx, pState, pInfo, aMatrix );
			return g_Globals.m_Interfaces.m_ModelRender->ForcedMaterialOverride( nullptr );
		}
		
		bool bIsValid = ( pPlayer->IsAlive( ) && !pPlayer->IsDormant( ) && pPlayer->IsPlayer( ) ) || ( pPlayer->GetClientClass( )->m_ClassID == ClassId_CCSRagdoll && g_Settings->m_bDrawRagdolls );
	    if ( !bIsValid )
		{
			g_Globals.m_Hooks.m_Originals.m_DrawModelExecute_Model( g_Globals.m_Interfaces.m_ModelRender, pCtx, pState, pInfo, aMatrix );
			return g_Globals.m_Interfaces.m_ModelRender->ForcedMaterialOverride( nullptr );
		}

		C_ChamsSettings aVisible = C_ChamsSettings( );
		C_ChamsSettings aInvisible = C_ChamsSettings( );
		
		if ( pPlayer == g_Globals.m_LocalPlayer )
			aVisible = g_Settings->m_aChamsSettings[ 6 ];
		else if ( pPlayer->m_iTeamNum( ) == g_Globals.m_LocalPlayer->m_iTeamNum( ) )
		{
			aVisible = g_Settings->m_aChamsSettings[ 4 ];
			aInvisible = g_Settings->m_aChamsSettings[ 5 ];
		}
		else
		{
			aVisible = g_Settings->m_aChamsSettings[ 0 ];
			aInvisible = g_Settings->m_aChamsSettings[ 1 ];
		}

		if ( aInvisible.m_bRenderChams )
			this->DrawModel( aInvisible, pCtx, pState, pInfo, aMatrix, true, true );

		if ( g_Settings->m_aChamsSettings[ 7 ].m_bRenderChams )
			if ( g_Globals.m_LocalPlayer == pPlayer )
				this->DrawModel( g_Settings->m_aChamsSettings[ 7 ], pCtx, pState, pInfo, g_LocalAnimations->GetDesyncMatrix( ).data( ), true, false );

		if ( g_Settings->m_aChamsSettings[ 8 ].m_bRenderChams )
			if ( g_Globals.m_LocalPlayer == pPlayer )
				this->DrawModel( g_Settings->m_aChamsSettings[ 8 ], pCtx, pState, pInfo, g_LocalAnimations->GetLagMatrix( ).data( ), true, false );

		if ( g_Settings->m_aChamsSettings[ 2 ].m_bRenderChams )
		{
			std::array < matrix3x4_t, MAXSTUDIOBONES > aBacktrackMatrix;
			if ( this->GetInterpolatedMatrix( pPlayer, aBacktrackMatrix.data( ) ) )
				this->DrawModel( g_Settings->m_aChamsSettings[ 2 ], pCtx, pState, pInfo, aBacktrackMatrix.data( ), true, true );
		}

		if ( aVisible.m_bRenderChams )
			this->DrawModel( aVisible, pCtx, pState, pInfo, aMatrix, false, false );
	}
	else if ( bIsArm && g_Settings->m_aChamsSettings[ 9 ].m_bRenderChams )
		this->DrawModel( g_Settings->m_aChamsSettings[ 9 ], pCtx, pState, pInfo, aMatrix, false, false );

	g_Globals.m_Hooks.m_Originals.m_DrawModelExecute_Model( g_Globals.m_Interfaces.m_ModelRender, pCtx, pState, pInfo, aMatrix );
	return g_Globals.m_Interfaces.m_ModelRender->ForcedMaterialOverride( nullptr );
}

void C_Chams::DrawModel( C_ChamsSettings Settings, IMatRenderContext* pCtx, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4_t* aMatrix, bool bForceNull, bool bXQZ )
{
	this->ApplyMaterial( Settings.m_iMainMaterial, bXQZ, Settings.m_Color );
	g_Globals.m_Hooks.m_Originals.m_DrawModelExecute_Model( g_Globals.m_Interfaces.m_ModelRender, pCtx, pState, pInfo, aMatrix );

    if ( Settings.m_aModifiers[ 0 ] )
    {
		this->ApplyMaterial( 2, bXQZ, Settings.m_aModifiersColors[ 0 ] );
		g_Globals.m_Hooks.m_Originals.m_DrawModelExecute_Model( g_Globals.m_Interfaces.m_ModelRender, pCtx, pState, pInfo, aMatrix );
    }

    if ( Settings.m_aModifiers[ 1 ] )
    {
        this->ApplyMaterial( 3, bXQZ, Settings.m_aModifiersColors[ 1 ] );
		g_Globals.m_Hooks.m_Originals.m_DrawModelExecute_Model( g_Globals.m_Interfaces.m_ModelRender, pCtx, pState, pInfo, aMatrix );
    }

    if ( Settings.m_aModifiers[ 2 ] )
    {
        this->ApplyMaterial( 4, bXQZ, Settings.m_aModifiersColors[ 2 ] );
		g_Globals.m_Hooks.m_Originals.m_DrawModelExecute_Model( g_Globals.m_Interfaces.m_ModelRender, pCtx, pState, pInfo, aMatrix );
    }
      
    if ( Settings.m_aModifiers[ 3 ] )
    {
		this->ApplyMaterial( 5, bXQZ, Settings.m_aModifiersColors[ 3 ] );
		g_Globals.m_Hooks.m_Originals.m_DrawModelExecute_Model( g_Globals.m_Interfaces.m_ModelRender, pCtx, pState, pInfo, aMatrix );
	}
     
	if ( !bForceNull )
		return;

	const float aWhite[ 3 ]
			=
		{
			1.0f,
			1.0f,
			1.0f
		};
	
	g_Globals.m_Interfaces.m_RenderView->SetBlend( 1.0f );
	g_Globals.m_Interfaces.m_RenderView->SetColorModulation( aWhite );

	return g_Globals.m_Interfaces.m_ModelRender->ForcedMaterialOverride( NULL );
}

#define MACRO_CONCAT( x, y ) x##y
#define CREATE_MATERIAL( var, name, content ) C_KeyValues* MACRO_CONCAT( KV, var ) = new C_KeyValues( name ); \
    MACRO_CONCAT( KV, var )->LoadFromBuffer( MACRO_CONCAT( KV, var ), name, content ); \
    var = g_Globals.m_Interfaces.m_MaterialSystem->CreateMaterial( name, MACRO_CONCAT( KV, var ) ); \
	var->IncrementReferenceCount( );\

void C_Chams::CreateMaterials( )
{
    CREATE_MATERIAL( m_pGlowMaterial, _S( "ogclub_glow" ), _S( R"#( "VertexLitGeneric" 
        {
          "$additive" "1"
          "$envmap" "models/effects/cube_white"
          "$envmaptint" "[1 1 1]"
          "$envmapfresnel" "1"
          "$envmapfresnelminmaxexp" "[0 1 2]"
        })#" ) );
    CREATE_MATERIAL( m_pFlatMaterial, _S( "ogclub_flat" ), _S( "UnlitGeneric { }" ) );

    this->m_pRegularMaterial = g_Globals.m_Interfaces.m_MaterialSystem->FindMaterial( _S( "debug/debugambientcube" ), _S( TEXTURE_GROUP_MODEL ) );
    this->m_pGhostMaterial = g_Globals.m_Interfaces.m_MaterialSystem->FindMaterial( _S( "dev/glow_armsrace" ), _S( TEXTURE_GROUP_OTHER ) );
    this->m_pGlassMaterial = g_Globals.m_Interfaces.m_MaterialSystem->FindMaterial( _S( "models/inventory_items/trophy_majors/gloss" ), _S( TEXTURE_GROUP_MODEL ) );
    this->m_pPulseMaterial = g_Globals.m_Interfaces.m_MaterialSystem->FindMaterial( _S( "models/inventory_items/dogtags/dogtags_outline" ), _S( TEXTURE_GROUP_MODEL ) );
}

void C_Chams::ApplyMaterial( int32_t iMaterial, bool bIgnoreZ, Color aColor, bool bCustom, bool bShotChams )
{
	C_Material* pRenderMaterial = nullptr;
    switch ( iMaterial )
    {
        case 0: pRenderMaterial = m_pFlatMaterial; break;
        case 1: pRenderMaterial = m_pRegularMaterial; break;
        case 2: pRenderMaterial = m_pGlowMaterial; break;
        case 3: pRenderMaterial = m_pGhostMaterial; break;
        case 4: pRenderMaterial = m_pGlassMaterial; break;
        case 5: pRenderMaterial = m_pPulseMaterial; break;
		case 6: pRenderMaterial = m_pHaloMaterial; break;
	}

    bool bHasBeenFound = false;
    if ( !pRenderMaterial )
        return;

    C_MaterialVar* pMaterialVar = pRenderMaterial->FindVar( _S( "$envmaptint" ), &bHasBeenFound );
    if ( !pMaterialVar )
        return;

    if ( bHasBeenFound && iMaterial > 1 )
        pMaterialVar->SetVecValue( aColor.r( ) / 255.0f, aColor.g( ) / 255.0f, aColor.b( ) / 255.0f );
    
	const float aBlendColor[ 3 ]
	=
	{
		( float_t )( aColor.r( ) ) / 255.0f, 
		( float_t )( aColor.g( ) ) / 255.0f,
		( float_t )( aColor.b( ) ) / 255.0f
	};
	
    pRenderMaterial->SetMaterialVarFlag( MATERIAL_VAR_IGNOREZ, bIgnoreZ && !bCustom );
	if ( bCustom )
	{
		pRenderMaterial->ColorModulate( aColor.r( ) / 255.0f, aColor.g( ) / 255.0f, aColor.b( ) / 255.0f );
		pRenderMaterial->AlphaModulate( aColor.a( ) / 255.0f );

		return g_Globals.m_Interfaces.m_StudioRender->ForcedMaterialOverride( pRenderMaterial );
	}

	pRenderMaterial->ColorModulate( 1.0f, 1.0f, 1.0f );
	pRenderMaterial->AlphaModulate( 1.0f );

	if ( bShotChams )
	{ 
		g_Globals.m_Interfaces.m_RenderView->SetColorModulation( aBlendColor );
		g_Globals.m_Interfaces.m_RenderView->SetBlend( aColor.a( ) / 255.0f );
		return g_Globals.m_Interfaces.m_ModelRender->ForcedMaterialOverride( pRenderMaterial );
	}

    g_Globals.m_Interfaces.m_RenderView->SetColorModulation( aBlendColor );
	g_Globals.m_Interfaces.m_RenderView->SetBlend( aColor.a( ) / 255.0f );
	return g_Globals.m_Interfaces.m_ModelRender->ForcedMaterialOverride( pRenderMaterial );
}

#include "../SDK/Math/Math.hpp"
#include "../Animations/LagCompensation.hpp"
#include "../Networking/Networking.hpp"

bool CompareRecord( C_LagRecord& First, C_LagRecord& Second )
{
    return First.m_SimulationTime > Second.m_SimulationTime;
}

bool C_Chams::GetInterpolatedMatrix( C_BasePlayer* pPlayer, matrix3x4_t* aMatrix )
{
    if ( pPlayer->EntIndex( ) > 60 || pPlayer->EntIndex( ) < 1 || !g_Globals.m_LocalPlayer->IsAlive( ) || pPlayer->m_iTeamNum( ) == g_Globals.m_LocalPlayer->m_iTeamNum( ) )
        return false;

    auto aLagRecords = g_Globals.m_CachedPlayerRecords[ pPlayer->EntIndex( ) ];
    if ( aLagRecords.size( ) < 2 )
        return false;

    std::sort( aLagRecords.begin( ), aLagRecords.end( ), CompareRecord );

    C_NetChannelInfo* pNetChannelInfo = g_Globals.m_Interfaces.m_EngineClient->GetNetChannelInfo( );
    if ( !pNetChannelInfo )
        return false;

	for ( auto iterator = aLagRecords.rbegin( ); iterator < aLagRecords.rend( ); iterator++ ) 
    {
		bool bIsEnd = iterator + 1 == aLagRecords.rend( );
        if ( !g_LagCompensation->IsValidTime( iterator->m_SimulationTime ) || bIsEnd ) 
            continue;
        
		if ( iterator->m_Origin.DistTo( pPlayer->GetAbsOrigin( ) ) < 1.f )
			return false;

		Vector next = bIsEnd ? pPlayer->m_vecOrigin( ) : ( iterator + 1 )->m_Origin;
		float time_next = bIsEnd ? pPlayer->m_flSimulationTime( ) : ( iterator + 1 )->m_SimulationTime;

		float total_latency = pNetChannelInfo->GetAvgLatency( 0 ) + pNetChannelInfo->GetAvgLatency( 1 );
		total_latency = std::clamp( total_latency, 0.f, 0.2f );

		float correct = total_latency + max( g_Globals.m_ConVars.m_ClInterp->GetFloat( ), g_Globals.m_ConVars.m_ClInterpRatio->GetFloat( ) / g_Globals.m_ConVars.m_ClUpdateRate->GetFloat( ) );
		float time_delta = time_next - iterator->m_SimulationTime;
		float add = bIsEnd ? 1.f : time_delta;
		float deadtime = iterator->m_SimulationTime + correct + add;

        float curtime = TICKS_TO_TIME( g_Globals.m_LocalPlayer->m_nTickBase( ) - g_ExploitSystem->GetShiftAmount( ) );
		float delta = deadtime - curtime;

		float mul = 1.f / add;
		Vector lerp = Math::Interpolate( next, iterator->m_Origin, std::clamp( delta * mul, 0.f, 1.f ) );

		matrix3x4_t ret[ 128 ];
		std::memcpy( ret,
			iterator->m_Matricies[ ROTATE_SERVER ].data( ),
			sizeof( ret ) );

		for( size_t i{ }; i < 128; ++i ) 
		{
			Vector vecMatrixDelta = iterator->m_Matricies[0][i].GetOrigin() - iterator->m_Origin;
			ret[ i ].SetOrigin( vecMatrixDelta + lerp );
		}

		std::memcpy( aMatrix,
			ret,
			sizeof( ret ) );

		return true;
	}

	return false;
}