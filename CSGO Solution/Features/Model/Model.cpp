#include "Model.hpp"
#include "../Menu.hpp"
#include "../Visuals/Chams.hpp"
#include "../SDK/Math/Math.hpp"

std::array < Vector4D, 6 > aWhiteArray = 
{
	Vector4D(0.4f, 0.4f, 0.4f, 1.0f ),
	Vector4D(0.4f, 0.4f, 0.4f, 1.0f ),
	Vector4D(0.4f, 0.4f, 0.4f, 1.0f ),
	Vector4D(0.4f, 0.4f, 0.4f, 1.0f ),
	Vector4D(0.4f, 0.4f, 0.4f, 1.0f ),
	Vector4D(0.4f, 0.4f, 0.4f, 1.0f ),
};

std::array < float_t, 3 > aColorModulation =
{
	1.0f,
	1.0f,
	1.0f
}; 

void C_DrawModel::Instance( )
{
	if ( !g_Menu->IsMenuOpened( ) )
		return;

	if ( !m_PreviewTexture )
	{
		g_Globals.m_Interfaces.m_MaterialSystem->BeginRenderTargetAllocation( );

		m_PreviewTexture =  g_Globals.m_Interfaces.m_MaterialSystem->CreateNamedRenderTargetTextureEx(
			_S( "Preview" ),
			350, 575,
			RT_SIZE_NO_CHANGE,
			g_Globals.m_Interfaces.m_MaterialSystem->GetBackBufferFormat( ),
			MATERIAL_RT_DEPTH_SHARED,
			TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT,
			CREATERENDERTARGETFLAGS_HDR
		);

		g_Globals.m_Interfaces.m_MaterialSystem->FinishRenderTargetAllocation( );
	}

	if ( !m_CubemapTexture )
		m_CubemapTexture = g_Globals.m_Interfaces.m_MaterialSystem->FindTexture( _S( "editor/cubemap.hdr" ), _S( TEXTURE_GROUP_CUBE_MAP ) );

	auto CreateModel =  reinterpret_cast< void( __thiscall* )( void* ) >( g_Globals.m_AddressList.m_CreateModel );
	if ( !m_PreviewModel )
	{
		m_PreviewModel = static_cast< C_MergedMDL* >( g_Globals.m_Interfaces.m_MemAlloc->Alloc( 0x75C ) );
		CreateModel( m_PreviewModel );

		m_PreviewModel->SetMDL( _S( "models/player/custom_player/uiplayer/animset_uiplayer.mdl" ) );
		m_PreviewModel->SetMergedMDL( _S( "models/player/custom_player/legacy/ctm_fbi_variantb.mdl" ) ); 
		m_PreviewModel->SetMergedMDL( _S( "models/weapons/w_pist_elite.mdl" ) ); 
	
		m_PreviewModel->SetSequence( 32, false );
		m_PreviewModel->SetupBonesForAttachmentQueries( );
	}

	m_PreviewModel->RootMDL.flTime += g_Globals.m_Interfaces.m_GlobalVars->m_flFrameTime / 2.0f;

	m_ViewSetup.iX = 0;
	m_ViewSetup.iY = 0;
	m_ViewSetup.iWidth = 350;
	m_ViewSetup.iHeight = 575;
	m_ViewSetup.bOrtho = false;
	m_ViewSetup.flFOV = 70.f;
	m_ViewSetup.vecOrigin = Vector( -65.0f, 2.0f, 50 );
	m_ViewSetup.angView = QAngle( 0, 0, 0 );
	m_ViewSetup.flNearZ = 7.0f;
	m_ViewSetup.flFarZ = 1000.f;
	m_ViewSetup.bDoBloomAndToneMapping = true;

	CMatRenderContextPtr pRenderContext( g_Globals.m_Interfaces.m_MaterialSystem );
	
	pRenderContext->PushRenderTargetAndViewport( );
	pRenderContext->SetRenderTarget( m_PreviewTexture );

	pRenderContext->BindLocalCubemap( m_CubemapTexture );
	pRenderContext->SetLightingOrigin( -65.0f, 2.0f, 50.0f );
	pRenderContext->SetIntRenderingParameter( 10, 0 );

	Frustum dummyFrustum;
	g_Globals.m_Interfaces.m_RenderView->Push3DView( pRenderContext, m_ViewSetup, VIEW_CLEAR_COLOR | VIEW_CLEAR_DEPTH | VIEW_CLEAR_STENCIL, m_PreviewTexture, dummyFrustum );

	pRenderContext->ClearColor4ub( false, false, false, false );
	pRenderContext->ClearBuffers( true, true, true );
	pRenderContext->SetAmbientLightCube( aWhiteArray.data( ) );

	g_Globals.m_Interfaces.m_StudioRender->SetAmbientLightColors( aWhiteArray.data( ) );
	g_Globals.m_Interfaces.m_StudioRender->SetLocalLights( 0, nullptr );

	matrix3x4_t matPlayerView = { };
	Math::AngleMatrix( QAngle( 0, g_Globals.m_Model.m_flModelRotation, 0 ), matPlayerView, Vector( 0, 0, 0 ) );

	g_Globals.m_Interfaces.m_ModelRender->SuppressEngineLighting( true );
	if ( m_ChamsSettings.m_bRenderChams )
		g_Chams->ApplyMaterial( m_ChamsSettings.m_iMainMaterial, false, Color( m_ChamsSettings.m_Color ), true );
		
	g_Globals.m_Model.m_bDrawModel = true;

	m_PreviewModel->Draw( matPlayerView );
	if ( m_ChamsSettings.m_bRenderChams )
	{
		if ( m_ChamsSettings.m_aModifiers[ 0 ] )
		{
			g_Chams->ApplyMaterial( 2, true, Color( m_ChamsSettings.m_aModifiersColors[ 0 ] ), true );
			m_PreviewModel->Draw( matPlayerView );
		}

		if ( m_ChamsSettings.m_aModifiers[ 1 ] )
		{
			g_Chams->ApplyMaterial( 3, true, Color( m_ChamsSettings.m_aModifiersColors[ 1 ] ), true );
			m_PreviewModel->Draw( matPlayerView );
		}

		if ( m_ChamsSettings.m_aModifiers[ 2 ] )
		{
			g_Chams->ApplyMaterial( 4, true, Color( m_ChamsSettings.m_aModifiersColors[ 2 ] ), true );
			m_PreviewModel->Draw( matPlayerView );
		}
      
		if ( m_ChamsSettings.m_aModifiers[ 3 ] )
		{
			g_Chams->ApplyMaterial( 5, true, Color( m_ChamsSettings.m_aModifiersColors[ 3 ] ), true );
			m_PreviewModel->Draw( matPlayerView );
		}
	}

	g_Globals.m_Model.m_bDrawModel = false;

	g_Globals.m_Interfaces.m_ModelRender->SuppressEngineLighting( false );

	g_Globals.m_Interfaces.m_RenderView->PopView( pRenderContext, dummyFrustum );
	pRenderContext->BindLocalCubemap( nullptr );

	pRenderContext->PopRenderTargetAndViewport( );
	pRenderContext->Release( );
}

void C_MergedMDL::SetupBonesForAttachmentQueries( )
{
	return ( ( void( __thiscall* )( void* ) )( g_Globals.m_AddressList.m_SetupBonesForAttachmentQueries ) )( this );
}

bool& C_MaterialSystem::DisableRenderTargetAllocationForever( )
{
	return *reinterpret_cast< bool* >( reinterpret_cast< std::uintptr_t >( this ) + g_Globals.m_AddressList.m_DisableRenderTargetAllocationForever );
}

void C_MergedMDL::SetMergedMDL( const char* szModelPath, CCustomMaterialOwner* pCustomMaterialOwner, void* pProxyData )
{
	return ( ( void( __thiscall* )( void*, const char*, CCustomMaterialOwner*, void*, bool ) )( g_Globals.m_AddressList.m_SetMergedMDL ) )( this, szModelPath, pCustomMaterialOwner, pProxyData, false );
}