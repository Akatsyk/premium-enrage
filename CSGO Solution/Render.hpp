#pragma once
#include <mutex>
#include "../Menu.hpp"
#include "SDK/Math/Vector.hpp"
#include "SDK/Utils/Color/Color.hpp"

#include "Data/IconBytes.hpp"
#include "Data/RobotoBold.hpp"
#include "Data/Roboto.hpp"
#include "Data/WeaponIcon.h"
#include "Data/LogIcons.h"
#include "Data/BombFont.h"
#include "Data/Icons.hpp"

inline std::mutex g_RenderMutex;

class C_Render
{
public:
	virtual void Instance( );
	virtual void RenderText( std::string strText, ImVec2 vecTextPosition, Color aTextColor, bool bForceCentered, bool bForceOutlined, ImFont* pDrawFont, bool bCalledFromMenu = false );
	virtual void RenderLine( float x1, float y1, float x2, float y2, Color aColor, float_t flThickness );
	virtual void RenderCircle3DFilled( const Vector& vecOrigin, float_t flRadius, Color aColor );
	virtual void RenderCircle3D( Vector vecPosition, int32_t iPointCount, float_t flRadius, Color aColor );
	virtual void RenderCircle2D( Vector vecPosition, int32_t iPointCount, float_t flRadius, Color aColor );
	virtual void RenderCircle2DFilled( Vector vecPosition, int32_t iPointCount, float_t flRadius, Color aColor );
	virtual void RenderTriangle( ImVec2 vecFirst, ImVec2 vecSecond, ImVec2 vecThird, Color aColor );
	virtual void RenderRectFilled( float x1, float y1, float x2, float y2, Color aColor );
	virtual void RenderRect(float x1, float y1, float x2, float y2, Color aColor);
	virtual void RenderArc(float x, float y, float radius, float min_angle, float max_angle, Color col, float thickness);
	virtual void ReloadFonts( );
	virtual void ForceReload( std::string szFontName, int nSize );

	virtual ImColor GetU32( Color aColor );

	virtual void Initialize( );
	virtual ImDrawList* GetDrawList( );
private:
	bool m_bReload = false;

	int m_nFontSize = 0;
	std::string m_szFontName = "";
	std::string m_szLastFont = "";

	ImDrawList* m_DrawList;
	ImDrawList* m_DrawListAct;
	ImDrawList* m_DrawListRender;
};

inline C_Render* g_Render = new C_Render( );