#pragma once
#include "../SDK/Includes.hpp"
#include "../Settings.hpp"

class C_DrawModel
{
public:
	virtual void Instance( );
	virtual ITexture* GetTexture( ) { return this->m_PreviewTexture; };
	virtual void SetChamsSettings( C_ChamsSettings Settings ) { this->m_ChamsSettings = Settings; };
	virtual void SetGlow( int glow ) { this->m_iGlow = glow; };
	virtual void SetGlowColor( Color glow ) { this->m_GlowColor = glow; };
private:
	ITexture* m_PreviewTexture = nullptr;
	ITexture* m_CubemapTexture = nullptr;
	C_MergedMDL* m_PreviewModel = nullptr;

	C_ViewSetup m_ViewSetup = { };
	C_ChamsSettings m_ChamsSettings = { };

	int32_t m_iGlow = 0;
	Color m_GlowColor = Color( 255, 255, 255, 255 );
};

inline C_DrawModel* g_DrawModel = new C_DrawModel( );