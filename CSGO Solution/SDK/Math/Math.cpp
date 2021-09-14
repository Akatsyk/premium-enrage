#include "Math.hpp"
#include "../Globals.hpp"

void Math::FixMovement( void* pCmd )
{
	C_UserCmd* pUserCmd = reinterpret_cast < C_UserCmd* > ( pCmd );
	if ( !pUserCmd || !pUserCmd->m_nCommand )
		return;

	Vector view_fwd, view_right, view_up, cmd_fwd, cmd_right, cmd_up;
	auto angViewAngles = pUserCmd->m_angViewAngles;
	angViewAngles.Normalize();

	QAngle wish;
	g_Globals.m_Interfaces.m_EngineClient->GetViewAngles(&wish);

	AngleVectors(wish, view_fwd, view_right, view_up);
	AngleVectors(angViewAngles, cmd_fwd, cmd_right, cmd_up);

	const float v8 = sqrtf((view_fwd.x * view_fwd.x) + (view_fwd.y * view_fwd.y));
	const float v10 = sqrtf((view_right.x * view_right.x) + (view_right.y * view_right.y));
	const float v12 = sqrtf(view_up.z * view_up.z);

	const Vector norm_view_fwd((1.f / v8) * view_fwd.x, (1.f / v8) * view_fwd.y, 0.f);
	const Vector norm_view_right((1.f / v10) * view_right.x, (1.f / v10) * view_right.y, 0.f);
	const Vector norm_view_up(0.f, 0.f, (1.f / v12) * view_up.z);

	const float v14 = sqrtf((cmd_fwd.x * cmd_fwd.x) + (cmd_fwd.y * cmd_fwd.y));
	const float v16 = sqrtf((cmd_right.x * cmd_right.x) + (cmd_right.y * cmd_right.y));
	const float v18 = sqrtf(cmd_up.z * cmd_up.z);

	const Vector norm_cmd_fwd((1.f / v14) * cmd_fwd.x, (1.f / v14) * cmd_fwd.y, 0.f);
	const Vector norm_cmd_right((1.f / v16) * cmd_right.x, (1.f / v16) * cmd_right.y, 0.f);
	const Vector norm_cmd_up(0.f, 0.f, (1.f / v18) * cmd_up.z);

	const float v22 = norm_view_fwd.x * pUserCmd->m_flForwardMove;
	const float v26 = norm_view_fwd.y * pUserCmd->m_flForwardMove;
	const float v28 = norm_view_fwd.z * pUserCmd->m_flForwardMove;
	const float v24 = norm_view_right.x * pUserCmd->m_flSideMove;
	const float v23 = norm_view_right.y * pUserCmd->m_flSideMove;
	const float v25 = norm_view_right.z * pUserCmd->m_flSideMove;
	const float v30 = norm_view_up.x * pUserCmd->m_flUpMove;
	const float v27 = norm_view_up.z * pUserCmd->m_flUpMove;
	const float v29 = norm_view_up.y * pUserCmd->m_flUpMove;

	pUserCmd->m_flForwardMove = ((((norm_cmd_fwd.x * v24) + (norm_cmd_fwd.y * v23)) + (norm_cmd_fwd.z * v25))
		+ (((norm_cmd_fwd.x * v22) + (norm_cmd_fwd.y * v26)) + (norm_cmd_fwd.z * v28)))
		+ (((norm_cmd_fwd.y * v30) + (norm_cmd_fwd.x * v29)) + (norm_cmd_fwd.z * v27));
	pUserCmd->m_flSideMove = ((((norm_cmd_right.x * v24) + (norm_cmd_right.y * v23)) + (norm_cmd_right.z * v25))
		+ (((norm_cmd_right.x * v22) + (norm_cmd_right.y * v26)) + (norm_cmd_right.z * v28)))
		+ (((norm_cmd_right.x * v29) + (norm_cmd_right.y * v30)) + (norm_cmd_right.z * v27));
	pUserCmd->m_flUpMove = ((((norm_cmd_up.x * v23) + (norm_cmd_up.y * v24)) + (norm_cmd_up.z * v25))
		+ (((norm_cmd_up.x * v26) + (norm_cmd_up.y * v22)) + (norm_cmd_up.z * v28)))
		+ (((norm_cmd_up.x * v30) + (norm_cmd_up.y * v29)) + (norm_cmd_up.z * v27));

	pUserCmd->m_flForwardMove = std::clamp(pUserCmd->m_flForwardMove, -450.f, 450.f);
	pUserCmd->m_flSideMove = std::clamp(pUserCmd->m_flSideMove, -450.f, 450.f);
	pUserCmd->m_flUpMove = std::clamp(pUserCmd->m_flUpMove, -320.f, 320.f);
}

static bool screen_transform(const Vector& in, Vector& out)
{
	static auto& w2sMatrix = g_Globals.m_Interfaces.m_EngineClient->WorldToScreenMatrix();

	out.x = w2sMatrix.m[0][0] * in.x + w2sMatrix.m[0][1] * in.y + w2sMatrix.m[0][2] * in.z + w2sMatrix.m[0][3];
	out.y = w2sMatrix.m[1][0] * in.x + w2sMatrix.m[1][1] * in.y + w2sMatrix.m[1][2] * in.z + w2sMatrix.m[1][3];
	out.z = 0.0f;

	float w = w2sMatrix.m[3][0] * in.x + w2sMatrix.m[3][1] * in.y + w2sMatrix.m[3][2] * in.z + w2sMatrix.m[3][3];

	if (w < 0.001f) {
		out.x *= 100000;
		out.y *= 100000;
		return false;
	}

	out.x /= w;
	out.y /= w;

	return true;
}

bool Math::WorldToScreen(const Vector& in, Vector& out)
{
	if (screen_transform(in, out)) {
		int w, h;
		g_Globals.m_Interfaces.m_EngineClient->GetScreenSize(w, h);

		out.x = (w / 2.0f) + (out.x * w) / 2.0f;
		out.y = (h / 2.0f) - (out.y * h) / 2.0f;

		return true;
	}
	return false;
}