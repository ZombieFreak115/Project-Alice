#pragma once

#include "concept_declarations.hpp"
#include "system_state.hpp"

namespace supply_routes { 

template<concepts::supply_route_type route_type>
dcon::province_id supply_route_destination(const sys::state& state, route_type route) {
	if constexpr(std::is_same_v<route_type, dcon::army_supply_route_id>) {
		return state.world.army_get_location_from_army_location(state.world.army_supply_route_get_army(route));
	} else if constexpr(std::is_same_v<route_type, dcon::navy_supply_route_id>) {
		return state.world.navy_get_location_from_navy_location(state.world.navy_supply_route_get_navy(route));
	} else if constexpr(std::is_same_v<route_type, dcon::land_construction_supply_route_id>) {
		dcon::province_land_construction_id construction = state.world.land_construction_supply_route_get_construction(route);
		return state.world.pop_get_province_from_pop_location(state.world.province_land_construction_get_pop(construction));
	} else if constexpr(std::is_same_v<route_type, dcon::naval_construction_supply_route_id>) {
		dcon::province_naval_construction_id construction = state.world.naval_construction_supply_route_get_construction(route);
		return state.world.province_naval_construction_get_province(construction);
	}
}

template<concepts::supply_route_type route_type>
dcon::province_id supply_route_origin(const sys::state& state, route_type route) {
	auto fat_route = fatten(state.world, route);
	return fat_route.get_origin().get_zone_from_local_market().get_capital();
}

template<concepts::supply_route_type route_type>
dcon::nation_id supply_route_owner(const sys::state& state, route_type route) {
	if constexpr(std::is_same_v<route_type, dcon::army_supply_route_id>) {
		return state.world.army_get_controller_from_army_control(state.world.army_supply_route_get_army(route));
	} else if constexpr(std::is_same_v<route_type, dcon::navy_supply_route_id>) {
		return state.world.navy_get_controller_from_navy_control(state.world.navy_supply_route_get_navy(route));
	} else if constexpr(std::is_same_v<route_type, dcon::land_construction_supply_route_id>) {
		dcon::province_land_construction_id construction = state.world.land_construction_supply_route_get_construction(route);
		return state.world.province_land_construction_get_nation(construction);
	} else if constexpr(std::is_same_v<route_type, dcon::naval_construction_supply_route_id>) {
		dcon::province_naval_construction_id construction = state.world.naval_construction_supply_route_get_construction(route);
		return state.world.province_naval_construction_get_nation(construction);
	}
}

}
