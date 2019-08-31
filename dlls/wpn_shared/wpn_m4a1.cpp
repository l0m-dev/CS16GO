#include "precompiled.h"

LINK_ENTITY_TO_CLASS(weapon_m4a1, CM4A1, CCSM4A1)

void CM4A1::Spawn()
{
	Precache();

	m_iId = WEAPON_M4A1;
	SET_MODEL(edict(), "models/w_m4a1.mdl");

	m_iDefaultAmmo = M4A1_DEFAULT_GIVE;
	m_flAccuracy = 0.2f;
	m_iShotsFired = 0;
	m_bDelayFire = true;

	// Get ready to fall down
	FallInit();

	// extend
	CBasePlayerWeapon::Spawn();
}

void CM4A1::Precache()
{
	PRECACHE_MODEL("models/v_m4a1.mdl");
	PRECACHE_MODEL("models/w_m4a1.mdl");

	PRECACHE_SOUND("weapons/m4a1-1.wav");
	PRECACHE_SOUND("weapons/m4a1_unsil-1.wav");
	PRECACHE_SOUND("weapons/m4a1_unsil-2.wav");
	PRECACHE_SOUND("weapons/m4a1_clipin.wav");
	PRECACHE_SOUND("weapons/m4a1_clipout.wav");
	PRECACHE_SOUND("weapons/m4a1_boltpull.wav");
	PRECACHE_SOUND("weapons/m4a1_deploy.wav");
	PRECACHE_SOUND("weapons/m4a1_silencer_on.wav");
	PRECACHE_SOUND("weapons/m4a1_silencer_off.wav");

	m_iShell = PRECACHE_MODEL("models/rshell.mdl");
	m_usFireM4A1 = PRECACHE_EVENT(1, "events/m4a1.sc");
}

int CM4A1::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "556Nato";
	p->iMaxAmmo1 = MAX_AMMO_556NATO;
	p->pszAmmo2 = nullptr;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = M4A1_MAX_CLIP;
	p->iSlot = 0;
	p->iPosition = 6;
	p->iId = m_iId = WEAPON_M4A1;
	p->iFlags = 0;
	p->iWeight = M4A1_WEIGHT;

	return 1;
}

BOOL CM4A1::Deploy()
{
	m_bDelayFire = true;
	m_flAccuracy = 0.2f;
	m_iShotsFired = 0;

	iShellOn = 1;

	if (m_iWeaponState & WPNSTATE_M4A1_SILENCED)
		return DefaultDeploy("models/v_m4a1.mdl", "models/p_m4a1.mdl", M4A1_DRAW, "rifle", UseDecrement() != FALSE);
	else
		return DefaultDeploy("models/v_m4a1.mdl", "models/p_m4a1.mdl", M4A1_UNSIL_DRAW, "rifle", UseDecrement() != FALSE);
}

void CM4A1::SecondaryAttack()
{
	if (m_iWeaponState & WPNSTATE_M4A1_SILENCED)
	{
		m_iWeaponState &= ~WPNSTATE_M4A1_SILENCED;
		SendWeaponAnim(M4A1_DETACH_SILENCER, UseDecrement() != FALSE);
		Q_strcpy(m_pPlayer->m_szAnimExtention, "rifle");
	}
	else
	{
		m_iWeaponState |= WPNSTATE_M4A1_SILENCED;
		SendWeaponAnim(M4A1_ATTACH_SILENCER, UseDecrement() != FALSE);
		Q_strcpy(m_pPlayer->m_szAnimExtention, "rifle");
	}

	m_flTimeWeaponIdle = m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 2.0f;
	m_flNextPrimaryAttack = GetNextAttackDelay(2.0);
}

void CM4A1::PrimaryAttack()
{
		if (!(m_pPlayer->pev->flags & FL_ONGROUND))
		{
			M4A1Fire(0.035 + (0.4 * m_flAccuracy), 0.0875, FALSE);
		}
		else if (m_pPlayer->pev->velocity.Length2D() > 140)
		{
			M4A1Fire(0.035 + (0.07 * m_flAccuracy), 0.0875, FALSE);
		}
		else
		{
			M4A1Fire(0.025 * m_flAccuracy, 0.0875, FALSE);
		}
}

void CM4A1::M4A1Fire(float flSpread, float flCycleTime, BOOL fUseAutoAim)
{
	Vector vecAiming, vecSrc, vecDir;
	int flag;

	m_bDelayFire = true;
	m_iShotsFired++;

	m_flAccuracy = ((m_iShotsFired * m_iShotsFired * m_iShotsFired) / 220) + 0.3f;

	if (m_flAccuracy > 1)
		m_flAccuracy = 1;

	if (m_iClip <= 0)
	{
		if (m_fFireOnEmpty)
		{
			PlayEmptySound();
			m_flNextPrimaryAttack = GetNextAttackDelay(0.2);
		}

		if (TheBots)
		{
			TheBots->OnEvent(EVENT_WEAPON_FIRED_ON_EMPTY, m_pPlayer);
		}

		return;
	}

	m_iClip--;
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

	vecSrc = m_pPlayer->GetGunPosition();
	vecAiming = gpGlobals->v_forward;

	if (m_iWeaponState & WPNSTATE_M4A1_SILENCED)
	{
		vecDir = m_pPlayer->FireBullets3(vecSrc, vecAiming, flSpread, 8192, 2, BULLET_PLAYER_556MM,
			M4A1_DAMAGE_SIL, M4A1_RANGE_MODIFER_SIL, m_pPlayer->pev, false, m_pPlayer->random_seed);
	}
	else
	{
		vecDir = m_pPlayer->FireBullets3(vecSrc, vecAiming, flSpread, 8192, 2, BULLET_PLAYER_556MM,
			M4A1_DAMAGE, M4A1_RANGE_MODIFER, m_pPlayer->pev, false, m_pPlayer->random_seed);

		m_pPlayer->pev->effects |= EF_MUZZLEFLASH;
	}

#ifdef CLIENT_WEAPONS
	flag = FEV_NOTHOST;
#else
	flag = 0;
#endif

#ifndef REGAMEDLL_FIXES
	m_pPlayer->ammo_556nato--;
#endif

	PLAYBACK_EVENT_FULL(flag, m_pPlayer->edict(), m_usFireM4A1, 0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y,
		int(m_pPlayer->pev->punchangle.x * 100), int(m_pPlayer->pev->punchangle.y * 100), (m_iWeaponState & WPNSTATE_M4A1_SILENCED) == WPNSTATE_M4A1_SILENCED, FALSE);

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = GetNextAttackDelay(flCycleTime);

	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
	{
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", SUIT_SENTENCE, SUIT_REPEAT_OK);
	}

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.5f;

	float upForce = 1.15;

	switch (m_iShotsFired)
	{
	case 0:
		//V_PunchAngles(1);
		break;
	case 1:
		V_PunchAngles(0.9);
		break;
	case 2:
		V_PunchAngles(1.4);
		break;
	case 3:
		V_PunchAngles(1.9);
		break;
	case 4:
		V_PunchAngles(1.9);
		break;
	case 5:
		V_PunchAngles(1.9);
		break;
	case 6:
		V_PunchAngles(1.9);
		break;
	case 7:
		V_PunchAngles(1.9);
		break;
	case 8:
		V_PunchAngles(upForce, 1);
		break;
	case 9:
		V_PunchAngles(upForce, 1);
		break;
	case 10:
		V_PunchAngles(upForce, 1);
		break;
	case 11:
		V_PunchAngles(upForce, 1);
		break;
	case 12:
		V_PunchAngles(upForce, 1);
		break;
	case 13:
		V_PunchAngles(upForce, 1);
		break;
	case 14:
		V_PunchAngles(upForce, 1);
		break;
	case 15:
		V_PunchAngles(upForce, 1);
		break;
	case 16:
		V_PunchAngles(upForce, -1);
		break;
	case 17:
		V_PunchAngles(upForce, -1);
		break;
	case 18:
		V_PunchAngles(upForce, -1);
		break;
	case 19:
		V_PunchAngles(upForce, -1);
		break;
	case 20:
		V_PunchAngles(upForce, -1);
		break;
	case 21:
		V_PunchAngles(upForce, -1);
		break;
	case 22:
		V_PunchAngles(upForce, -1);
		break;
	case 23:
		V_PunchAngles(upForce, -1);
		break;
	case 24:
		V_PunchAngles(upForce, -1);
		break;
	case 25:
		V_PunchAngles(upForce, 1);
		break;
	case 26:
		V_PunchAngles(upForce, 1);
		break;
	case 27:
		V_PunchAngles(upForce, 1);
		break;
	case 28:
		V_PunchAngles(upForce, 1);
		break;
	case 29:
		V_PunchAngles(upForce, 1);
		break;
	case 30:
		V_PunchAngles(upForce, 1);
		break;
	}
}

void CM4A1::Reload()
{
	if (m_pPlayer->ammo_556nato <= 0)
		return;

	if (DefaultReload(iMaxClip(), ((m_iWeaponState & WPNSTATE_M4A1_SILENCED) == WPNSTATE_M4A1_SILENCED) ? M4A1_RELOAD : M4A1_UNSIL_RELOAD, M4A1_RELOAD_TIME))
	{
		m_pPlayer->SetAnimation(PLAYER_RELOAD);

		m_flAccuracy = 0.2f;
		m_iShotsFired = 0;
		m_bDelayFire = false;
	}
}

void CM4A1::WeaponIdle()
{
	ResetEmptySound();
	m_pPlayer->GetAutoaimVector(AUTOAIM_10DEGREES);

	if (m_flTimeWeaponIdle > UTIL_WeaponTimeBase())
	{
		return;
	}

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 20.0f;
	SendWeaponAnim((m_iWeaponState & WPNSTATE_M4A1_SILENCED) == WPNSTATE_M4A1_SILENCED ? M4A1_IDLE : M4A1_UNSIL_IDLE, UseDecrement() != FALSE);
}

float CM4A1::GetMaxSpeed()
{
	return M4A1_MAX_SPEED;
}
