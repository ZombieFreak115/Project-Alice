#pragma once

#include "concept_declarations.hpp"
#include "system_state.hpp"

// Only put trivial light templates in here, leave larger templates for military_templates.hpp

namespace military {


template<concepts::military_unit unit_type>
dcon::province_id unit_location(const sys::state& state, unit_type unit) {
	if constexpr(std::is_same_v<unit_type, dcon::army_id>) {
		return state.world.army_get_location_from_army_location(unit);
	} else if constexpr(std::is_same_v<unit_type, dcon::navy_id>) {
		return state.world.navy_get_location_from_navy_location(unit);
	}
}

template<concepts::military_unit unit_type>
dcon::nation_id unit_controller(const sys::state& state, unit_type unit) {
	if constexpr(std::is_same_v<unit_type, dcon::army_id>) {
		return state.world.army_get_controller_from_army_control(unit);
	} else if constexpr(std::is_same_v<unit_type, dcon::navy_id>) {
		return state.world.navy_get_controller_from_navy_control(unit);
	}
}

}
