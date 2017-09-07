#ifndef WEAPONMANAGER_H
#define WEAPONMANAGER_H
#include <vector>
#include "Weapon.h"

namespace Logic
{
class WeaponManager
{
private:
	std::vector<std::pair<Weapon, Weapon>> m_weapons;
public:
	void usewWaponPrimary();
	void useWeaponSecondary();
};
}
#endif
