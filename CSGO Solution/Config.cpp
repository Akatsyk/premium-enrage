#include "Config.hpp"
#include "Settings.hpp"

#include "SDK/Utils/JSON.hpp"
#include <functional>
#include <unordered_map>

#define CREATE_SERIALIZER( t, fun ) { _S( #t ), [ ]( std::any const& a ) -> std::string { return fun( std::any_cast< t const& >( a ) ); } }
#define CREATE_DESERIALIZER( t, fun ) { _S( #t ), fun }

#define SET_COLOR( json, color ) \
json[ _S( "r" ) ] = color[ 0 ];\
json[ _S( "g" ) ] = color[ 1 ];\
json[ _S( "b" ) ] = color[ 2 ];\
json[ _S( "a" ) ] = color[ 3 ];\

#define LOAD_COLOR( json, color ) \
color[ 0 ] = json[ _S( "r" ) ];\
color[ 1 ] = json[ _S( "g" ) ];\
color[ 2 ] = json[ _S( "b" ) ];\
color[ 3 ] = json[ _S( "a" ) ];\

static std::unordered_map< std::string, std::function< std::string( std::any const& ) >> SerializeVisitors
{
	CREATE_SERIALIZER( int, [ ]( int x ) -> std::string
	{
		return std::to_string( x );
	} ),
	CREATE_SERIALIZER( int32_t, [ ]( int32_t x ) -> std::string
	{
		return std::to_string( x );
	} ),
	CREATE_SERIALIZER( bool, [ ]( bool x ) -> std::string
	{
		return x ? _S( "true" ) : _S( "false" );
	} ),
	CREATE_SERIALIZER( Color, [ ]( Color c ) -> std::string
	{
		nlohmann::json r;
		r[0] = c.r( );
		r[1] = c.g( );
		r[2] = c.b( );
		r[3] = c.a( );
		return r.dump( );
	} ), 
	CREATE_SERIALIZER( C_PlayerSettings, [ ]( C_PlayerSettings aVisualSettings ) -> std::string
	{
		nlohmann::json jVisual;
		jVisual[ _S( "Draw Box" ) ] = aVisualSettings.m_BoundaryBox;
		jVisual[ _S( "Draw Name" ) ] = aVisualSettings.m_RenderName;
		jVisual[ _S( "Draw Weapon Text" ) ] = aVisualSettings.m_RenderWeaponText;
		jVisual[ _S( "Draw Weapon Icon" ) ] = aVisualSettings.m_RenderWeaponIcon;
		jVisual[ _S( "Draw Healthbar" ) ] = aVisualSettings.m_RenderHealthBar;
		jVisual[ _S( "Draw Healthbar Text" ) ] = aVisualSettings.m_RenderHealthText;
		jVisual[ _S( "Draw Ammobar" ) ] = aVisualSettings.m_RenderAmmoBar;
		jVisual[ _S( "Draw Ammobar Text" ) ] = aVisualSettings.m_RenderAmmoBarText;
		jVisual[ _S( "Glow style" ) ] = aVisualSettings.m_iGlowStyle;
		jVisual[ _S( "Render glow" ) ] = aVisualSettings.m_bRenderGlow;

		SET_COLOR( jVisual[ _S( "Weapon Text Color" ) ], aVisualSettings.m_aWeaponText );
		SET_COLOR( jVisual[ _S( "Weapon Icon Color" ) ], aVisualSettings.m_aWeaponIcon );
		SET_COLOR( jVisual[ _S( "Healthbar Color" ) ], aVisualSettings.m_aHealthBar );
		SET_COLOR( jVisual[ _S( "Healthbar text Color" ) ], aVisualSettings.m_aHealthText );
		SET_COLOR( jVisual[ _S( "Ammobar Color" ) ], aVisualSettings.m_aAmmoBar );
		SET_COLOR( jVisual[ _S( "Ammobar text Color" ) ], aVisualSettings.m_aAmmoBarText );
		SET_COLOR( jVisual[ _S( "Glow Color" ) ], aVisualSettings.m_aGlow );
		
		return jVisual.dump( );
	} ),
	CREATE_SERIALIZER( C_KeyBind, [ ]( C_KeyBind c ) -> std::string 
	{
		nlohmann::json jKeyBind;
		jKeyBind[ _S( "Bind" ) ] = c.m_iKeySelected;
		jKeyBind[ _S( "Mode" ) ] = c.m_iModeSelected;
		return jKeyBind.dump( );
	} ),
	CREATE_SERIALIZER( float_t, [ ]( float_t flValue ) -> std::string
	{
		return std::to_string( flValue );
	} ),
	CREATE_SERIALIZER( float, [ ]( float flValue ) -> std::string
	{
		return std::to_string( flValue );
	} ),
	CREATE_SERIALIZER( std::string, [ ]( std::string szValue ) -> std::string
	{
		return szValue;
	} )
};

static std::unordered_map< std::string, std::function< void( const std::string& s, std::any* ) >> DeSerializeVisitors
{
	CREATE_DESERIALIZER( int, [ ]( std::string content, std::any* ptr ) -> void
	{
		ptr->emplace< int >( std::atoi( content.c_str( ) ) );
	} ),
	CREATE_DESERIALIZER( int32_t, [ ]( std::string content, std::any* ptr ) -> void
	{
		ptr->emplace< int32_t >( std::atoi( content.c_str( ) ) );
	} ),
	CREATE_DESERIALIZER( bool, [ ]( std::string content, std::any* ptr ) -> void
	{
		ptr->emplace< bool >( content == _S( "true" ) );
	} ),
	CREATE_DESERIALIZER( Color, [ ]( std::string content, std::any* ptr ) -> void
	{
		nlohmann::json r = nlohmann::json::parse( content );
		Color clr( r[ 0 ].get< int >( ), r[ 1 ].get< int >( ), r[ 2 ].get< int >( ), r[ 3 ].get< int >( ) );
		ptr->emplace< Color >( clr );
	} ),
	CREATE_DESERIALIZER( C_KeyBind, [ ]( std::string content, std::any* ptr ) -> void
	{
		nlohmann::json jKeyBind = nlohmann::json::parse( content );

		C_KeyBind KeyBind;
		KeyBind.m_iKeySelected = jKeyBind[ _S( "Bind" ) ].get< int >( );
		KeyBind.m_iModeSelected = jKeyBind[ _S( "Mode" ) ].get< int >( );

		ptr->emplace< C_KeyBind >( KeyBind );
	} ),
	CREATE_DESERIALIZER( C_PlayerSettings, [ ]( std::string content, std::any* ptr ) -> void
	{
		nlohmann::json jVisual = nlohmann::json::parse( content );

		C_PlayerSettings aVisualSettings;
		aVisualSettings.m_BoundaryBox = jVisual[ _S( "Draw Box" ) ].get< bool >( );
		aVisualSettings.m_RenderName = jVisual[ _S( "Draw Name" ) ].get< bool >( );
		aVisualSettings.m_RenderWeaponText = jVisual[ _S( "Draw Weapon Text" ) ].get< bool >( );
		aVisualSettings.m_RenderWeaponIcon = jVisual[ _S( "Draw Weapon Icon" ) ].get< bool >( );
		aVisualSettings.m_RenderHealthBar = jVisual[ _S( "Draw Healthbar" ) ].get< bool >( );
		aVisualSettings.m_RenderHealthText = jVisual[ _S( "Draw Healthbar Text" ) ].get< bool >( );
		aVisualSettings.m_RenderAmmoBar = jVisual[ _S( "Draw Ammobar" ) ].get< bool >( );
		aVisualSettings.m_RenderAmmoBarText = jVisual[ _S( "Draw Ammobar Text" ) ].get< bool >( );
		aVisualSettings.m_bRenderGlow = jVisual[ _S( "Render glow" ) ].get< bool >( );
		aVisualSettings.m_iGlowStyle = jVisual[ _S( "Glow style" ) ].get< int >( );

		LOAD_COLOR( jVisual[ _S( "Weapon Text Color" ) ], aVisualSettings.m_aWeaponText );
		LOAD_COLOR( jVisual[ _S( "Weapon Icon Color" ) ], aVisualSettings.m_aWeaponIcon );
		LOAD_COLOR( jVisual[ _S( "Healthbar Color" ) ], aVisualSettings.m_aHealthBar );
		LOAD_COLOR( jVisual[ _S( "Healthbar text Color" ) ], aVisualSettings.m_aHealthText );
		LOAD_COLOR( jVisual[ _S( "Ammobar Color" ) ], aVisualSettings.m_aAmmoBar );
		LOAD_COLOR( jVisual[ _S( "Ammobar text Color" ) ], aVisualSettings.m_aAmmoBarText );
		LOAD_COLOR( jVisual[ _S( "Glow Color" ) ], aVisualSettings.m_aGlow );
		
		ptr->emplace< C_PlayerSettings >( aVisualSettings );
	} ),
	CREATE_DESERIALIZER( float_t, [ ]( std::string content, std::any* ptr ) -> void
	{
		ptr->emplace< float_t >( std::atof( content.c_str( ) ) );
	} ),
	CREATE_DESERIALIZER( float, [ ]( std::string content, std::any* ptr ) -> void
	{
		ptr->emplace< float >( std::atof( content.c_str( ) ) );
	} ),
	CREATE_DESERIALIZER( std::string, [ ]( std::string content, std::any* ptr ) -> void
	{
		ptr->emplace< std::string >( content );
	} )
};

#include <fstream>
void C_ConfigSystem::SaveConfig( )
{
	nlohmann::json jConfig;
	for ( auto& item : m_aItems )
	{
		nlohmann::json cur;

		cur[ 1 ] = item.Type;
		cur[ 0 ] = item.Name;

		if ( const auto it = SerializeVisitors.find( item.Type ); it != SerializeVisitors.cend( ) )
		{
			cur[ 2 ] = it->second( item.Var );
		}
		else 
		{
			cur[ 2 ] = "???";
		}

		jConfig[ _S( "Automatic" ) ].push_back(cur);
	}
	
	nlohmann::json& jManual = jConfig[ _S( "Manual" ) ];
	for ( int i = 0; i < g_Settings->m_aEquipment.size( ); i++ )
		jManual[ _S( "Equipment" ) ][ std::to_string( i ) ] = g_Settings->m_aEquipment[ i ];
	
	for ( int i = 0; i < g_Settings->m_aWorldRemovals.size( ); i++ )
		jManual[ _S( "Removals" ) ][ std::to_string( i ) ] = g_Settings->m_aWorldRemovals[ i ];

	for ( int i = 0; i < g_Settings->m_aInverterConditions.size( ); i++ )
	{
		jManual[ _S( "Inverters" ) ][ std::to_string( i ) ] = g_Settings->m_aInverterConditions[ i ];
		jManual[ _S( "Fakelags" ) ][ std::to_string( i ) ] = g_Settings->m_aFakelagTriggers[ i ];
	}

	for ( int i = 0; i < g_Settings->m_aChamsSettings.size( ); i++ )
	{
		jManual[ _S( "Chams settings" ) ][ std::to_string( i ) ][ _S( "Render chams" ) ] = g_Settings->m_aChamsSettings[ i ].m_bRenderChams;
		jManual[ _S( "Chams settings" ) ][ std::to_string( i ) ][ _S( "Main material" ) ] = g_Settings->m_aChamsSettings[ i ].m_iMainMaterial;

		for ( int j = 0; j < 4; j++ )
		{
			jManual[ _S( "Chams settings" ) ][ std::to_string( i ) ][ std::to_string( j ) ] = g_Settings->m_aChamsSettings[ i ].m_Color[ j ];
		
			jManual[ _S( "Chams settings" ) ][ std::to_string( i ) ][ _S( "Modifiers" ) ][ std::to_string( j ) ] = g_Settings->m_aChamsSettings[ i ].m_aModifiers[ j ];
			SET_COLOR( jManual[ _S( "Chams settings" ) ][ std::to_string( i ) ][ _S( "Modifiers Colors" ) ][ std::to_string( j ) ], g_Settings->m_aChamsSettings[ i ].m_aModifiersColors[ j ] );
		}
	}

	for ( int i = 0; i < g_Settings->m_aRageSettings.size( ); i++ )
	{
		jManual[ _S( "Rage settings" ) ][ std::to_string( i ) ][ _S( "Enabled" ) ] = g_Settings->m_aRageSettings[ i ].m_bEnabled;
		jManual[ _S( "Rage settings" ) ][ std::to_string( i ) ][ _S( "Lethal safety" ) ] = g_Settings->m_aRageSettings[ i ].m_bLethalSafety;
		jManual[ _S( "Rage settings" ) ][ std::to_string( i ) ][ _S( "Prefer safe" ) ] = g_Settings->m_aRageSettings[ i ].m_bPreferSafe;
		jManual[ _S( "Rage settings" ) ][ std::to_string( i ) ][ _S( "Prefer body" ) ] = g_Settings->m_aRageSettings[ i ].m_bPreferBody;
		jManual[ _S( "Rage settings" ) ][ std::to_string( i ) ][ _S( "Autostop" ) ] = g_Settings->m_aRageSettings[ i ].m_bAutoStop;
		jManual[ _S( "Rage settings" ) ][ std::to_string( i ) ][ _S( "Autoscope" ) ] = g_Settings->m_aRageSettings[ i ].m_bAutoScope;
		jManual[ _S( "Rage settings" ) ][ std::to_string( i ) ][ _S( "Mindamage" ) ] = g_Settings->m_aRageSettings[ i ].m_iMinDamage;
		jManual[ _S( "Rage settings" ) ][ std::to_string( i ) ][ _S( "Mindamage override" ) ] = g_Settings->m_aRageSettings[ i ].m_iMinDamageOverride;
		jManual[ _S( "Rage settings" ) ][ std::to_string( i ) ][ _S( "Hitchance" ) ] = g_Settings->m_aRageSettings[ i ].m_iHitChance;
		jManual[ _S( "Rage settings" ) ][ std::to_string( i ) ][ _S( "DT Hitchance" ) ] = g_Settings->m_aRageSettings[ i ].m_iDoubleTapHitChance;
		jManual[ _S( "Rage settings" ) ][ std::to_string( i ) ][ _S( "Accuracy" ) ] = g_Settings->m_aRageSettings[ i ].m_iAccuracyBoost;
		jManual[ _S( "Rage settings" ) ][ std::to_string( i ) ][ _S( "Head scale" ) ] = g_Settings->m_aRageSettings[ i ].m_iHeadScale;
		jManual[ _S( "Rage settings" ) ][ std::to_string( i ) ][ _S( "Body scale" ) ] = g_Settings->m_aRageSettings[ i ].m_iBodyScale;
	
		for ( int j = 0; j < 6; j++ )
		{
			jManual[ _S( "Rage settings" ) ][ std::to_string( i ) ][ _S( "Hitboxes" ) ][ std::to_string( j ) ] = g_Settings->m_aRageSettings[ i ].m_Hitboxes[ j ];
			jManual[ _S( "Rage settings" ) ][ std::to_string( i ) ][ _S( "Safe Hitboxes" ) ][ std::to_string( j ) ] = g_Settings->m_aRageSettings[ i ].m_SafeHitboxes[ j ];
			jManual[ _S( "Rage settings" ) ][ std::to_string( i ) ][ _S( "Multipoints" ) ][ std::to_string( j ) ] = g_Settings->m_aRageSettings[ i ].m_Multipoints[ j ];
		}

		for ( int j = 0; j < 2; j++ )
			jManual[ _S( "Rage settings" ) ][ std::to_string( i ) ][ _S( "Autostop options" ) ][ std::to_string( j ) ] = g_Settings->m_aRageSettings[ i ].m_AutoStopOptions[ j ];

		for ( int j = 0; j < 2; j++ )
			jManual[ _S( "Rage settings" ) ][ std::to_string( i ) ][ _S( "Doubletap options" ) ][ std::to_string( j ) ] = g_Settings->m_aRageSettings[ i ].m_DoubleTapOptions[ j ];
	}

	std::ofstream out_file( "config.cfg", std::ios::out | std::ios::trunc);
	out_file << jConfig.dump(4);
	out_file.close();
}

void C_ConfigSystem::LoadConfig( )
{
	nlohmann::json jConfig;

	std::ifstream config_file( "config.cfg", std::ios::in);
	config_file >> jConfig;
	config_file.close();

	for ( auto& item : jConfig[ _S( "Automatic" ) ] )
	{
		const std::string item_identifier = item[ 0 ].get< std::string >( );
		const std::string item_type = item[ 1 ].get< std::string >( );
		const std::string inner = item[ 2 ].get< std::string >( );

		auto item_index = -1;
		for ( auto i = 0u; i < m_aItems.size( ); i++ )
		{
			if ( m_aItems.at( i ).Name == item_identifier )
			{
				item_index = i;
				break;
			}
		}

		if ( item_index < 0 )
			continue;

		auto& cur_item = m_aItems.at( item_index );
		if ( const auto it = DeSerializeVisitors.find(item_type); it != DeSerializeVisitors.cend( ) )
		{
			it->second( inner, &cur_item.Var );
		}
	}

	nlohmann::json jManual = jConfig[ _S( "Manual" ) ];
	for ( int i = 0; i < g_Settings->m_aEquipment.size( ); i++ )
		 g_Settings->m_aEquipment[ i ] = jManual[ _S( "Equipment" ) ][ std::to_string( i ) ].get < bool >( );
	
	for ( int i = 0; i < g_Settings->m_aWorldRemovals.size( ); i++ )
		g_Settings->m_aWorldRemovals[ i ] = jManual[ _S( "Removals" ) ][ std::to_string( i ) ].get < bool >( );

	for ( int i = 0; i < g_Settings->m_aFakelagTriggers.size( ); i++ )
		g_Settings->m_aFakelagTriggers[ i ] = jManual[ _S( "Fakelags" ) ][ std::to_string( i ) ].get < bool > ( );

	for ( int i = 0; i < g_Settings->m_aInverterConditions.size( ); i++ )
		g_Settings->m_aInverterConditions[ i ] = jManual[ _S( "Inverters" ) ][ std::to_string( i ) ].get < bool > ( );
		
	for ( int i = 0; i < jManual[ _S( "Chams settings" ) ].size( ); i++ )
	{
		g_Settings->m_aChamsSettings[ i ].m_bRenderChams = jManual[ _S( "Chams settings" ) ][ std::to_string( i ) ][ _S( "Render chams" ) ].get < bool >( );
		g_Settings->m_aChamsSettings[ i ].m_iMainMaterial = jManual[ _S( "Chams settings" ) ][ std::to_string( i ) ][ _S( "Main material" ) ].get < int >( );

		for ( int j = 0; j < 4; j++ )
		{
			g_Settings->m_aChamsSettings[ i ].m_Color[ j ] = jManual[ _S( "Chams settings" ) ][ std::to_string( i ) ][ std::to_string( j ) ].get < int >( );
		
			if ( jManual[ _S( "Chams settings" ) ][ std::to_string( i ) ][ _S( "Modifiers" ) ].is_object( ) )
			{
				g_Settings->m_aChamsSettings[ i ].m_aModifiers[ j ] = jManual[ _S( "Chams settings" ) ][ std::to_string( i ) ][ _S( "Modifiers" ) ][ std::to_string( j ) ].get < bool >( );
				LOAD_COLOR( jManual[ _S( "Chams settings" ) ][ std::to_string( i ) ][ _S( "Modifiers Colors" ) ][ std::to_string( j ) ], g_Settings->m_aChamsSettings[ i ].m_aModifiersColors[ j ] );
			}
		}
	}

	for ( int i = 0; i < g_Settings->m_aRageSettings.size( ); i++ )
	{
		g_Settings->m_aRageSettings[ i ].m_bEnabled = jManual[ _S( "Rage settings" ) ][ std::to_string( i ) ][ _S( "Enabled" ) ].get < bool >( );
		g_Settings->m_aRageSettings[ i ].m_bLethalSafety = jManual[ _S( "Rage settings" ) ][ std::to_string( i ) ][ _S( "Lethal safety" ) ].get < bool >( );
		g_Settings->m_aRageSettings[ i ].m_bPreferSafe = jManual[ _S( "Rage settings" ) ][ std::to_string( i ) ][ _S( "Prefer safe" ) ].get < bool >( );
		g_Settings->m_aRageSettings[ i ].m_bPreferBody = jManual[ _S( "Rage settings" ) ][ std::to_string( i ) ][ _S( "Prefer body" ) ].get < bool >( );
		g_Settings->m_aRageSettings[ i ].m_bAutoScope = jManual[ _S( "Rage settings" ) ][ std::to_string( i ) ][ _S( "Autoscope" ) ].get < bool >( );
		g_Settings->m_aRageSettings[ i ].m_bAutoStop = jManual[ _S( "Rage settings" ) ][ std::to_string( i ) ][ _S( "Autostop" ) ].get < bool >( );
		if ( jManual[ _S( "Rage settings" ) ][ std::to_string( i ) ][ _S( "Mindamage" ) ].is_number_integer( ) )
			g_Settings->m_aRageSettings[ i ].m_iMinDamage = jManual[ _S( "Rage settings" ) ][ std::to_string( i ) ][ _S( "Mindamage" ) ].get < int >( );
		g_Settings->m_aRageSettings[ i ].m_iMinDamageOverride = jManual[ _S( "Rage settings" ) ][ std::to_string( i ) ][ _S( "Mindamage override" ) ].get < int >( );
		g_Settings->m_aRageSettings[ i ].m_iHitChance = jManual[ _S( "Rage settings" ) ][ std::to_string( i ) ][ _S( "Hitchance" ) ].get < int >( );
		g_Settings->m_aRageSettings[ i ].m_iDoubleTapHitChance = jManual[ _S( "Rage settings" ) ][ std::to_string( i ) ][ _S( "DT Hitchance" ) ].get < int >( );
		g_Settings->m_aRageSettings[ i ].m_iAccuracyBoost = jManual[ _S( "Rage settings" ) ][ std::to_string( i ) ][ _S( "Accuracy" ) ].get < int >( );
		g_Settings->m_aRageSettings[ i ].m_iHeadScale = jManual[ _S( "Rage settings" ) ][ std::to_string( i ) ][ _S( "Head scale" ) ].get < int >( );
		g_Settings->m_aRageSettings[ i ].m_iBodyScale = jManual[ _S( "Rage settings" ) ][ std::to_string( i ) ][ _S( "Body scale" ) ].get < int >( );
	
		for ( int j = 0; j < 6; j++ )
		{
			g_Settings->m_aRageSettings[ i ].m_Hitboxes[ j ] = jManual[ _S( "Rage settings" ) ][ std::to_string( i ) ][ _S( "Hitboxes" ) ][ std::to_string( j ) ].get < bool >( );
			g_Settings->m_aRageSettings[ i ].m_SafeHitboxes[ j ] = jManual[ _S( "Rage settings" ) ][ std::to_string( i ) ][ _S( "Safe Hitboxes" ) ][ std::to_string( j ) ].get < bool >( );
			g_Settings->m_aRageSettings[ i ].m_Multipoints[ j ] = jManual[ _S( "Rage settings" ) ][ std::to_string( i ) ][ _S( "Multipoints" ) ][ std::to_string( j ) ].get < bool >( );
		}

		for ( int j = 0; j < 2; j++ )
			g_Settings->m_aRageSettings[ i ].m_AutoStopOptions[ j ] = jManual[ _S( "Rage settings" ) ][ std::to_string( i ) ][ _S( "Autostop options" ) ][ std::to_string( j ) ].get < bool >( );

		for ( int j = 0; j < 2; j++ )
			g_Settings->m_aRageSettings[ i ].m_DoubleTapOptions[ j ] = jManual[ _S( "Rage settings" ) ][ std::to_string( i ) ][ _S( "Doubletap options" ) ][ std::to_string( j ) ].get < bool >( );
	}
}