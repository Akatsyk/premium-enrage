#include "../Hooks.hpp"

void __fastcall C_Hooks::hkDrawModelExecute_Studio( LPVOID pEcx, uint32_t, LPVOID pResults, DrawModelInfo_t* pInfo, matrix3x4_t* pBoneToWorld, float_t* flpFlexWeights, float_t* flpFlexDelayedWeights, Vector& vrModelOrigin, int32_t iFlags )
{
	if ( g_Globals.m_Model.m_bDrawModel && pInfo->m_pStudioHdr )
	{
		if ( strstr( pInfo->m_pStudioHdr->szName, _S( "weapons\\w_pist_elite.mdl" ) ) )
		{
			g_Globals.m_Interfaces.m_StudioRender->ForcedMaterialOverride( NULL );
			return g_Globals.m_Hooks.m_Originals.m_DrawModelExecute_Studio( g_Globals.m_Interfaces.m_StudioRender, pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags );
		}
	}

	return g_Globals.m_Hooks.m_Originals.m_DrawModelExecute_Studio( g_Globals.m_Interfaces.m_StudioRender, pResults, pInfo, pBoneToWorld, flpFlexWeights, flpFlexDelayedWeights, vrModelOrigin, iFlags );
}