/*
------------------------------------------------------------------------------------------
	Copyright (c) 2014 Vinyl Games Studio
                


    Author: Mikhail Kutuzov
	Date:	5/16/2014 12:31:56 PM
------------------------------------------------------------------------------------------
*/

#ifndef __WEAPON_STATS_H__
#define __WEAPON_STATS_H__

#define WEAPON_LENGTH				30.0f

#include "Stats.h"
#include "Effect.h"

#include "../ToneArmEngine/InitializationCategory.h"

namespace merrymen
{
/*
------------------------------------------------------------------------------------------
	WeaponStats

	Struct that stores stats of in-game weapons.
------------------------------------------------------------------------------------------
*/

enum class ShootingType : unsigned char
{
	Automatic = (unsigned char)'1',
	Semi_automatic
};

enum class LoadingUnit : unsigned char {

	Clip = (unsigned char)'1',
	Bullet
};

enum class WeaponID : unsigned char
{
	Pistol,
	Shotgun,
	SniperRifle,
	AssaultRifle
};

template <class T>
struct WeaponStats : public Stats {

	ShootingType		ShootingType;
	LoadingUnit			LoadingUnit;
	unsigned short		Damage;
	unsigned short		Ammo;
	unsigned short		TotalAmmo;
	unsigned short		MaxAmmo;
	unsigned short		ClipSize;
	float				Cooldown;
	float				ReloadTime;
	float				FireSpreadAngleHip;
	float				FireSpreadAngleAim;
	float				AimingDistance;
	float				ZoomOutFactor;
	float				FOVAngleModifier;
	float				Recoil;
	
	T*					Owner;

	// constructors
						WeaponStats();
						WeaponStats(T* const owner, const vgs::InitializationCategory& category);

	bool				Update(float dt);
	bool				UpdateEffects(float dt) override;

	void				ApplyEffect(const Effect& effect, vgs::GameObject* const author = nullptr) override;
	void				UnapplyEffect(Effect* const effect, vgs::GameObject* const author = nullptr) override;

	WeaponStats&		operator=(const WeaponStats& other);
};

//
// default constructor
//
template <class T>
WeaponStats<T>::WeaponStats() :
	ShootingType(ShootingType::Automatic),
	LoadingUnit(LoadingUnit::Clip),
	Owner(nullptr),
	Damage(0),
	Ammo(0),
	TotalAmmo(),
	MaxAmmo(0),
	ClipSize(0),
	Cooldown(0.0f),
	ReloadTime(0.0f),
	FireSpreadAngleHip(0.0f),
	FireSpreadAngleAim(0.0f),
	AimingDistance(0.0f),
	ZoomOutFactor(0.0f),
	FOVAngleModifier(0.0f),
	Recoil(0.0f)
{}

//
// parametrized constructor for reading values from a .ini file
//
template <class T>
WeaponStats<T>::WeaponStats(T* const owner, const vgs::InitializationCategory& category) :
	Owner(owner)
{
	ShootingType		= (merrymen::ShootingType)category.GetStringValueForKey("shooting_type")[0];
	LoadingUnit			= (merrymen::LoadingUnit)category.GetStringValueForKey("loading_unit")[0];
	Damage				= category.GetIntValueForKey("damage");
	Ammo = ClipSize		= category.GetIntValueForKey("clip_size");
	MaxAmmo	= TotalAmmo	= category.GetIntValueForKey("max_ammo");
	Cooldown			= category.GetFloatValueForKey("cooldown");
	ReloadTime			= category.GetFloatValueForKey("reload_time");
	FireSpreadAngleHip	= category.GetFloatValueForKey("fire_spread_angle_hip");
	FireSpreadAngleAim	= category.GetFloatValueForKey("fire_spread_angle_aim");
	AimingDistance		= category.GetFloatValueForKey("aiming_distance");
	ZoomOutFactor		= category.GetFloatValueForKey("aiming_zoom_out_factor");
	FOVAngleModifier	= category.GetFloatValueForKey("aiming_fov_angle_modifier");
	Recoil				= category.GetFloatValueForKey("recoil");
}

//
// overloaded assignment operator
//
template <class T>
WeaponStats<T>& WeaponStats<T>::operator=(const WeaponStats<T>& other) {

	if (this != &other) {
			
		ShootingType		= other.ShootingType;
		LoadingUnit			= other.LoadingUnit;
		Damage				= other.Damage;
		Ammo	 			= other.Ammo;
		ClipSize			= other.ClipSize;
		MaxAmmo	 			= other.MaxAmmo;
		TotalAmmo			= other.TotalAmmo;
		Cooldown			= other.Cooldown;
		ReloadTime			= other.ReloadTime;
		FireSpreadAngleHip	= other.FireSpreadAngleHip;
		FireSpreadAngleAim	= other.FireSpreadAngleAim;
		AimingDistance		= other.AimingDistance;
		ZoomOutFactor		= other.ZoomOutFactor;
		FOVAngleModifier	= other.FOVAngleModifier;
		Recoil				= other.Recoil;
	}

	return *this;
}

//
// updates the stats of the weapon
//
template <class T>
bool WeaponStats<T>::Update(float dt) {

	bool statsChanged = UpdateEffects(dt);

	// reloading goes on
	if (Owner->IsReloading()) {

		bool reloadingFinished = false;

		if (LoadingUnit == LoadingUnit::Bullet) {

			// time to load one bullet
			if (ReloadTime - Owner->m_reloadTimer > ReloadTime / ClipSize) {
				
				// load one bullet
				if (Ammo < ClipSize && TotalAmmo > 0) {

					Ammo++;
					TotalAmmo--;
					statsChanged = true;
					Owner->m_reloadTimer = ReloadTime;
				}

				// reloading is finished now
				if (Ammo == ClipSize || TotalAmmo == 0) {
					reloadingFinished = true;
				}
			}
		}
		else if (LoadingUnit == LoadingUnit::Clip) {

			// reloading is finished
			if (Owner->m_reloadTimer <= 0) {

				// set ammo and total ammo to proper values
				if (TotalAmmo + Ammo >= ClipSize) {
			 
					TotalAmmo -= ClipSize - Ammo;
					Ammo = ClipSize;
				}
				else {

					Ammo += TotalAmmo;
					TotalAmmo = 0;
				}

				statsChanged = true;
				reloadingFinished = true;
			}
		}

		if (reloadingFinished) {
			Owner->FinishReloading();
		}
	}

	return statsChanged;
}

//
// updates applied effects
//
template <class T>
bool WeaponStats<T>::UpdateEffects(float dt) {

	bool statsChanged = false;

	// update applied effects
	for (auto effect : AppliedEffects) {

		// chech if the effect timer has reached zero and this effect needs to be unapplied
		if (effect.first->Update(dt)) {

			UnapplyEffect(effect.first, effect.second);
			statsChanged = true;
		}
	}

	return statsChanged;
}

//
// determines the way a buff/debuff is applied to the stats
//
template <class T>
void WeaponStats<T>::ApplyEffect(const Effect& effect, vgs::GameObject* const author) {

	SpecificEffectType effectType = SpecificEffectType(effect.Type, effect.Reason);

	// check whether an effect of that type can be applied to the stats or not
	if (CountSimilarAppliedEffects(effect) < Effect::EffectLimits[effectType]) {

		switch (effect.Type) {

		case EffectType::ExtraAmmo:

			TotalAmmo = std::min(MaxAmmo, (unsigned short)(TotalAmmo + effect.Value));
			break;

		default:
			break;
		}
	}
}

//
// determines waht happens when the effect timer expires
//
template <class T>
void WeaponStats<T>::UnapplyEffect(Effect* const effect, vgs::GameObject* const author) {

	// remove the effect from the list of applied effects
	AppliedEffects.erase(effect);

	// unapply the effect
	/*switch (effect->Type) {

	}*/
}

}

#endif