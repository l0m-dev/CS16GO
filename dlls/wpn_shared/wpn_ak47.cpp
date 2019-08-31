#include "precompiled.h"

LINK_ENTITY_TO_CLASS(weapon_ak47, CAK47, CCSAK47)

void CAK47::Spawn()
{
	Precache();

	m_iId = WEAPON_AK47;
	SET_MODEL(edict(), "models/w_ak47.mdl");

	m_iDefaultAmmo = AK47_DEFAULT_GIVE;
	m_flAccuracy = 0.07f;
	m_iShotsFired = 0;

	// Get ready to fall down
	FallInit();

	// extend
	CBasePlayerWeapon::Spawn();
}

void CAK47::Precache()
{
	PRECACHE_MODEL("models/v_ak47.mdl");
	PRECACHE_MODEL("models/w_ak47.mdl");

	PRECACHE_SOUND("weapons/ak47-1.wav");
	PRECACHE_SOUND("weapons/ak47-2.wav");
	PRECACHE_SOUND("weapons/ak47_clipout.wav");
	PRECACHE_SOUND("weapons/ak47_clipin.wav");
	PRECACHE_SOUND("weapons/ak47_boltpull.wav");

	m_iShell = PRECACHE_MODEL("models/rshell.mdl");
	m_usFireAK47 = PRECACHE_EVENT(1, "events/ak47.sc");
}

int CAK47::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "762Nato";
	p->iMaxAmmo1 = MAX_AMMO_762NATO;
	p->pszAmmo2 = nullptr;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = AK47_MAX_CLIP;
	p->iSlot = 0;
	p->iPosition = 1;
	p->iId = m_iId = WEAPON_AK47;
	p->iFlags = 0;
	p->iWeight = AK47_WEIGHT;

	return 1;
}

BOOL CAK47::Deploy()
{
	m_flAccuracy = 0.07;
	m_iShotsFired = 0;
	iShellOn = 1;

	return DefaultDeploy("models/v_ak47.mdl", "models/p_ak47.mdl", AK47_DRAW, "ak47", UseDecrement() != FALSE);
}

void CAK47::SecondaryAttack()
{
	;
}

void CAK47::PrimaryAttack()
{
	if (m_iShotsFired == 0)
		m_flAccuracy = 0.07;

	if (!(m_pPlayer->pev->flags & FL_ONGROUND))
	{
		AK47Fire(0.04 + (0.4 * m_flAccuracy), 0.0955, FALSE);
	}
	else if (m_pPlayer->pev->velocity.Length2D() > 140)
	{
		AK47Fire(0.04 + (0.07 * m_flAccuracy), 0.0955, FALSE);
	}
	else
	{
		AK47Fire(0.0275 * m_flAccuracy, 0.0955, FALSE);
	}
}

void CAK47::AK47Fire(float flSpread, float flCycleTime, BOOL fUseAutoAim)
{
	Vector vecAiming, vecSrc, vecDir;
	int flag;

	m_bDelayFire = true;
	m_iShotsFired++;

	m_flAccuracy = ((m_iShotsFired * m_iShotsFired * m_iShotsFired) / 200) + 0.35f;

	if (m_flAccuracy > 1.25f)
		m_flAccuracy = 1.25f;

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
	m_pPlayer->pev->effects |= EF_MUZZLEFLASH;
	m_pPlayer->SetAnimation(PLAYER_ATTACK1);

	UTIL_MakeVectors(m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle);

	vecSrc = m_pPlayer->GetGunPosition();
	vecAiming = gpGlobals->v_forward;

	vecDir = m_pPlayer->FireBullets3(vecSrc, vecAiming, flSpread, 8192, 2, BULLET_PLAYER_762MM,
		AK47_DAMAGE, AK47_RANGE_MODIFER, m_pPlayer->pev, false, m_pPlayer->random_seed);

#ifdef CLIENT_WEAPONS
	flag = FEV_NOTHOST;
#else
	flag = 0;
#endif

	PLAYBACK_EVENT_FULL(flag, m_pPlayer->edict(), m_usFireAK47, 0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y,
		int(m_pPlayer->pev->punchangle.x * 100), int(m_pPlayer->pev->punchangle.y * 100), FALSE, FALSE);

	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = BRIGHT_GUN_FLASH;

	m_flNextPrimaryAttack = m_flNextSecondaryAttack = GetNextAttackDelay(flCycleTime);

	if (!m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
	{
		m_pPlayer->SetSuitUpdate("!HEV_AMO0", SUIT_SENTENCE, SUIT_REPEAT_OK);
	}

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.9f;

	float upForce = 1.2;

	switch (m_iShotsFired)
	{
	case 0:
		//V_PunchAngles(1);
		break;
	case 1:
		V_PunchAngles(1);
		break;
	case 2:
		V_PunchAngles(1.5);
		break;
	case 3:
		V_PunchAngles(2);
		break;
	case 4:
		V_PunchAngles(2);
		break;
	case 5:
		V_PunchAngles(2);
		break;
	case 6:
		V_PunchAngles(2);
		break;
	case 7:
		V_PunchAngles(2);
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

void CAK47::Reload()
{
#ifdef REGAMEDLL_FIXES
	// to prevent reload if not enough ammo
	if (m_pPlayer->ammo_762nato <= 0)
		return;
#endif

	if (DefaultReload(iMaxClip(), AK47_RELOAD, AK47_RELOAD_TIME))
	{
		m_pPlayer->SetAnimation(PLAYER_RELOAD);

		m_flAccuracy = 0.07f;
		m_iShotsFired = 0;
		m_bDelayFire = false;
	}
}

void CAK47::WeaponIdle()
{
	ResetEmptySound();
	m_pPlayer->GetAutoaimVector(AUTOAIM_10DEGREES);

	if (m_flTimeWeaponIdle <= UTIL_WeaponTimeBase())
	{
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 20.0f;
		SendWeaponAnim(AK47_IDLE1, UseDecrement() != FALSE);
	}
}
