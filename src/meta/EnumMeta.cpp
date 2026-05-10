#include "EnumMeta.h"
#include <src/utils/Logger.h>

//todo maybe should make this automatic
EnumMeta::EnumMeta() {
	addEnum("ContactLayer", {
		"None",
		"Player",
		"Enemy",
		"Projectile",			//hits player and enemy
		"PlayerProjectile",
		"EnemyProjectile",
		"Wall",
		"Collectable"});

	addEnum("ShapeType", { "Circle","Box" });
	addEnum("BodyType", { "Static","Kinematic","Dynamic" });
}

void EnumMeta::addEnum(std::string_view name, std::initializer_list<std::string_view> list) {
	if (map.contains(name))Logger::error_throw("Enum already exists: {}", name);
	Map<enum_t> enumMap;
	int i = 0;
	for (auto& e : list) {
		enumMap[e] = i++;
	}
	map[name] = std::move(enumMap);
}

std::optional<EnumMeta::enum_t> EnumMeta::try_get(std::string_view name, std::string_view enumName) {
	if (auto it1 = inst().map.find(name); it1 != inst().map.end()) {
		auto& mmap = it1->second;
		if (auto it2 = mmap.find(enumName); it2 != mmap.end()) {
			return it2->second;
		}
	}
	return std::nullopt;
}