#pragma once
#include "func_utils.hpp"

namespace functions {
	namespace weapons {
		fixed_containers::FixedMap<uint64_t, float, 100> default_recoil;
		fixed_containers::FixedMap<uint64_t, float, 100> default_spread;
		fixed_containers::FixedMap<uint64_t, float, 100> default_reload;

		void update_recoil(CWeaponInfo* aWep, uint64_t name) {
			if (g_config->weapons.recoil) {
				if (default_recoil.find(name) == default_recoil.end()) {
					default_recoil[name] = aWep->getRecoil();
				}
				float new_recoil = (g_config->weapons.manager_type == 0)
					? default_recoil[name] / 100 * g_config->weapons.modify_values.recoil_percent
					: 0.f;
				aWep->setRecoil(new_recoil);
			}
			else if (aWep->getRecoil() != default_recoil[name]) {
				aWep->setRecoil(default_recoil[name]);
			}
		}

		void update_spread(CWeaponInfo* aWep, uint64_t name) {
			if (g_config->weapons.spread) {
				if (default_spread.find(name) == default_spread.end()) {
					default_spread[name] = aWep->getSpread();
				}
				float new_spread = (g_config->weapons.manager_type == 0)
					? default_spread[name] / 100 * g_config->weapons.modify_values.spread_percent
					: 0.f;
				aWep->setSpread(new_spread);
			}
			else if (aWep->getSpread() != default_spread[name]) {
				aWep->setSpread(default_spread[name]);
			}
		}

		void update_reload_speed(CWeaponInfo* aWep, uint64_t name) {
			if (g_config->weapons.reload) {
				if (default_reload.find(name) == default_reload.end()) {
					default_reload[name] = aWep->getReloadSpeed();
				}
				float new_reload = (g_config->weapons.manager_type == 0)
					? default_reload[name] * (g_config->weapons.modify_values.reload_speed_percent / 100) + 1
					: 100.f;
				aWep->setReloadSpeed(new_reload);
			}
			else if (aWep->getReloadSpeed() != default_reload[name]) {
				aWep->setReloadSpeed(default_reload[name]);
			}
		}

		void update_range(CWeaponInfo* aWep) {
			if (g_config->weapons.weapon_range) {
				aWep->Range((g_config->weapons.manager_type == 0)
					? g_config->weapons.modify_values.weapon_range_meters
					: 9999.f);
			}
		}

		void tick() {
			if (g_config->weapons.recoil || g_config->weapons.spread || g_config->weapons.weapon_range || g_config->weapons.reload) {
				CWeaponManager* wep = local.player->weapon();
				if (IsValidPtr(wep) && IsValidPtr(wep->_WeaponInfo)) {
					auto aWep = wep->_WeaponInfo;
					auto name = aWep->GetWeaponHash();
					if (IsValidPtr(aWep)) {
						update_recoil(aWep, name);
						update_spread(aWep, name);
						update_reload_speed(aWep, name);
						update_range(aWep);
					}
				}
			}
		}
	}
}
