#pragma once

#include "concept_declarations.hpp"
#include "system_state.hpp"

namespace economy {


template<concepts::construction_type construction_type>
dcon::province_id construction_location(const sys::state& state, construction_type c) {
	auto construction = fatten(state.world, c);
	if constexpr(std::is_same_v<construction_type, dcon::province_land_construction_id>) {
		return construction.get_pop().get_province_from_pop_location();
	} else if constexpr(std::is_same_v<construction_type, dcon::province_naval_construction_id> || std::is_same_v<construction_type, dcon::factory_construction_id> || std::is_same_v<construction_type, dcon::province_building_construction_id>) {
		return construction.get_province();
	}
}

template<concepts::construction_type construction_type>
dcon::nation_id construction_controller(const sys::state& state, construction_type c) {
	auto construction = fatten(state.world, c);
	return construction.get_nation();
}


}


