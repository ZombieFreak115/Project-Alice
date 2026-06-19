#pragma once

#include "concept_declarations.hpp"
#include "system_state.hpp"
#include "military_constants.hpp"

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
auto unit_supply_routes(const sys::state& state, unit_type unit) {
	if constexpr(std::is_same_v<unit_type, dcon::army_id>) {
		return state.world.army_get_army_supply_route(unit);
	} else if constexpr(std::is_same_v<unit_type, dcon::navy_id>) {
		return state.world.navy_get_navy_supply_route(unit);
	}
}

template<concepts::military_unit unit_type>
auto unit_membership(const sys::state& state, unit_type unit) {
	if constexpr(std::is_same_v<unit_type, dcon::army_id>) {
		return state.world.army_get_army_membership(unit);
	} else if constexpr(std::is_same_v<unit_type, dcon::navy_id>) {
		return state.world.navy_get_navy_membership(unit);
	}
}

template<concepts::military_subunit subunit_type>
auto subunit_membership(const sys::state& state, subunit_type unit) {
	if constexpr(std::is_same_v<subunit_type, dcon::regiment_id>) {
		return state.world.regiment_get_army_from_army_membership(unit);
	} else if constexpr(std::is_same_v<subunit_type, dcon::ship_id>) {
		return state.world.ship_get_navy_from_navy_membership(unit);
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


template<unit_consumption_type consumption_type>
economy::commodity_set& unit_type_commodity_costs(sys::state& state, dcon::unit_type_id type) {
	if constexpr(consumption_type == unit_consumption_type::supply) {
		return state.military_definitions.unit_base_definitions[type].supply_cost;
	}
	else if constexpr(consumption_type == unit_consumption_type::reinforcement) {
		return state.military_definitions.unit_base_definitions[type].build_cost;
	}
}

template<unit_consumption_type consumption_type>
const economy::commodity_set& unit_type_commodity_costs(const sys::state& state, dcon::unit_type_id type) {
	return unit_type_commodity_costs<consumption_type>(const_cast<sys::state&>(state), type);
}

// Gets the union of all IDs required by all units for either supply, reinforcement (build goods) or both.
template<commodity_consumption_type consumption_type>
economy::huge_commodity_id_array& get_military_commodities_union(sys::state& state) {
	if constexpr(consumption_type == commodity_consumption_type::supply) {
		return state.military_definitions.military_supply_goods;
	} else if constexpr(consumption_type == commodity_consumption_type::reinforcement) {
		return state.military_definitions.military_build_goods;
	}
	else if constexpr(consumption_type == commodity_consumption_type::both) {
		return state.military_definitions.military_supply_build_goods;
	}
}
template<commodity_consumption_type consumption_type>
const economy::huge_commodity_id_array& get_military_commodities_union(const sys::state& state) {
	return get_military_commodities_union<consumption_type>(const_cast<sys::state&>(state));
}

}


