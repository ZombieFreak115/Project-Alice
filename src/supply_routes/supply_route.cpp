#include "concept_declarations.hpp"
#include "system_state.hpp"
#include "military.hpp"
#include "economy.hpp"
#include "province.hpp"
#include "military_templates.hpp"
#include "construction.hpp"
#include "supply_route_templates.hpp"
#include "supply_route.hpp"

namespace supply_routes {

struct unit {
	bool is_army = false;
	union unit_type_union {
		dcon::army_id army;
		dcon::navy_id navy{};
		constexpr unit_type_union() = default;
	} content;

	template<concepts::military_unit unit_type>
	constexpr unit(unit_type unit) {
		if constexpr(std::is_same<unit_type, dcon::army_id>::value) {
			is_army = true;
			content.army = unit;
		} else if constexpr(std::is_same<unit_type, dcon::navy_id>::value) {
			is_army = false;
			content.navy = unit;
		}
	}
	constexpr unit() = default;
};


dcon::province_id supply_route_get_destination(const sys::state& state, dcon::army_supply_route_id route) {
	return state.world.army_get_location_from_army_location(state.world.army_supply_route_get_army(route));
}
dcon::province_id supply_route_get_destination(const sys::state& state, dcon::navy_supply_route_id route) {
	return state.world.navy_get_location_from_navy_location(state.world.navy_supply_route_get_navy(route));
}
dcon::province_id supply_route_get_destination(const sys::state& state, dcon::land_construction_supply_route_id route) {
	dcon::province_land_construction_id construction = state.world.land_construction_supply_route_get_construction(route);
	return state.world.pop_get_province_from_pop_location(state.world.province_land_construction_get_pop(construction));
}
dcon::province_id supply_route_get_destination(const sys::state& state, dcon::naval_construction_supply_route_id route) {
	dcon::province_naval_construction_id construction = state.world.naval_construction_supply_route_get_construction(route);
	return state.world.province_naval_construction_get_province(construction);
}

dcon::province_id supply_route_get_origin(const sys::state& state, dcon::army_supply_route_id route) {
	auto origin = state.world.army_supply_route_get_origin(route);
	auto state_origin = state.world.market_get_zone_from_local_market(origin);
	return state.world.state_instance_get_capital(state_origin);
}
dcon::province_id supply_route_get_origin(const sys::state& state, dcon::navy_supply_route_id route) {
	auto origin = state.world.navy_supply_route_get_origin(route);
	auto state_origin = state.world.market_get_zone_from_local_market(origin);
	return state.world.state_instance_get_capital(state_origin);
}
dcon::province_id supply_route_get_origin(const sys::state& state, dcon::land_construction_supply_route_id route) {
	auto origin = state.world.land_construction_supply_route_get_origin(route);
	auto state_origin = state.world.market_get_zone_from_local_market(origin);
	return state.world.state_instance_get_capital(state_origin);
}
dcon::province_id supply_route_get_origin(const sys::state& state, dcon::naval_construction_supply_route_id route) {
	auto origin = state.world.naval_construction_supply_route_get_origin(route);
	auto state_origin = state.world.market_get_zone_from_local_market(origin);
	return state.world.state_instance_get_capital(state_origin);
}

template<concepts::commodity_amount_military_supply_or_build_union_array_type commodity_array_type, concepts::military_supply_route_type route_type>
const commodity_array_type& military_route_get_buffered_goods(const sys::state& state, route_type route) {
	auto fat_route = fatten(state.world, route);
	if constexpr(std::is_same_v<commodity_array_type, economy::supply_cost_union_commodity_amount_array>) {
		return fat_route.get_buffered_supply_goods();
	} else if constexpr(std::is_same_v<commodity_array_type, economy::build_cost_union_commodity_amount_array>) {
		return fat_route.get_buffered_reinforcement_goods();
	}
}
template const economy::supply_cost_union_commodity_amount_array& military_route_get_buffered_goods(const sys::state& state, dcon::army_supply_route_id route);
template const economy::supply_cost_union_commodity_amount_array& military_route_get_buffered_goods(const sys::state& state, dcon::navy_supply_route_id route);
template const economy::build_cost_union_commodity_amount_array& military_route_get_buffered_goods(const sys::state& state, dcon::army_supply_route_id route);
template const economy::build_cost_union_commodity_amount_array& military_route_get_buffered_goods(const sys::state& state, dcon::navy_supply_route_id route);

template<concepts::commodity_amount_military_supply_or_build_union_array_type commodity_array_type, concepts::military_supply_route_type route_type>
commodity_array_type& military_route_get_buffered_goods(sys::state& state, route_type route) {
	return const_cast<commodity_array_type&>(military_route_get_buffered_goods<commodity_array_type>(static_cast<const sys::state&>(state), route));
}
template economy::supply_cost_union_commodity_amount_array& military_route_get_buffered_goods(sys::state& state, dcon::army_supply_route_id route);
template economy::supply_cost_union_commodity_amount_array& military_route_get_buffered_goods(sys::state& state, dcon::navy_supply_route_id route);
template economy::build_cost_union_commodity_amount_array& military_route_get_buffered_goods(sys::state& state, dcon::army_supply_route_id route);
template economy::build_cost_union_commodity_amount_array& military_route_get_buffered_goods(sys::state& state, dcon::navy_supply_route_id route);


dcon::nation_id supply_route_get_owner(const sys::state& state, dcon::army_supply_route_id route) {
	return state.world.army_get_controller_from_army_control(state.world.army_supply_route_get_army(route));
}
dcon::nation_id supply_route_get_owner(const sys::state& state, dcon::navy_supply_route_id route) {
	return state.world.navy_get_controller_from_navy_control(state.world.navy_supply_route_get_navy(route));
}
dcon::nation_id supply_route_get_owner(const sys::state& state, dcon::land_construction_supply_route_id route) {
	dcon::province_land_construction_id construction = state.world.land_construction_supply_route_get_construction(route);
	return state.world.province_land_construction_get_nation(construction);
}
dcon::nation_id supply_route_get_owner(const sys::state& state, dcon::naval_construction_supply_route_id route) {
	dcon::province_naval_construction_id construction = state.world.naval_construction_supply_route_get_construction(route);
	return state.world.province_naval_construction_get_nation(construction);
}

float naval_supply_speed(const sys::state& state, dcon::nation_id nation_as) {
	auto fastest_unit = state.world.nation_get_fastest_unlocked_transport_unit(nation_as);
	return base_naval_supply_speed + state.world.nation_get_unit_stats(nation_as, fastest_unit).maximum_speed * fastest_transport_unit_supply_speed_mult;
}


float land_supply_speed(const sys::state& state, dcon::nation_id nation_as) {
	auto fastest_unit = state.world.nation_get_fastest_unlocked_land_unit(nation_as);
	return base_land_supply_speed + state.world.nation_get_unit_stats(nation_as, fastest_unit).maximum_speed * fastest_land_unit_supply_speed_mult;
}


float port_supply_capacity_modifier(const sys::state& state, dcon::province_id port_prov) {
	constexpr float port_supply_throughput_per_naval_base = 10.0f;
	constexpr float base_port_supply_throughput = 0.1f;
	assert(state.world.province_get_port_to(port_prov)); // Should always be a port prov
	// TODO: make it a modifier on the naval base instead of just taking the naval base level
	auto naval_base_id = uint8_t(economy::province_building_type::naval_base);
	auto size = state.world.province_get_building_level(port_prov, naval_base_id);
	return port_supply_throughput_per_naval_base * size + base_port_supply_throughput;
}





float get_enemy_blockade_power(const sys::state& state, dcon::province_id prov, dcon::nation_id nation_as) {
	assert(province::is_sea(state, prov) && !province::province_is_deep_waters(state, prov));
	// TODO: actually compute blockade power
	return military::navy_strength_present<military::battle_allowed::yes, military::retreat_allowed::no, military::participants_included::enemies>(state, prov, nation_as) * 1.0f;
}


float supply_throughput_speed_modifier(const sys::state& state, dcon::province_id province, dcon::nation_id nation_as) {
	bool is_sea = province::is_sea(state, province);
	float movement_cost = province::movement_cost(state, province);
	float speed = (is_sea ? naval_supply_speed(state, nation_as) : land_supply_speed(state, nation_as));
	return (is_sea ? supply_throughput_per_km_naval_speed : supply_throughput_per_km_land_speed) * (speed / movement_cost);
}

float supply_throughput_infrastructure_modifier(const sys::state& state, dcon::province_id province, dcon::nation_id nation_as) {
	float infrastructure = province::get_infrastructure(state, province) * 100.0f;
	return supply_throughput_infrastructure * infrastructure;
}


// Draft values
float supply_throughput_modifier(const sys::state& state, dcon::province_id province, dcon::nation_id nation_as) {
	auto is_sea = province::is_sea(state, province);
	float from_base = (is_sea ? sea_base_supply_thoughput : land_base_supply_thoughput);
	float from_speed = supply_throughput_speed_modifier(state, province, nation_as);
	float from_infra = is_sea ? 1.0f : supply_throughput_infrastructure_modifier(state,  province, nation_as);
	return std::max(from_speed + from_infra, 0.0f);
}

float supply_throughput_percentage_access_modifier(const sys::state& state, dcon::province_id province, dcon::nation_id nation_as) {
	bool has_access = province::has_supply_access_to_province(state, nation_as, province);
	float siege_progress = state.world.province_get_siege_progress(province);
	return has_access ? 1.0f - siege_progress : 0.0f;
}

float supply_throughput_percentage_blockade_modifier(const sys::state& state, dcon::province_id prov, dcon::nation_id nation_as) {
	// prev_prov may be invalid to signifiy no previous province
	assert(prov);
	assert(nation_as);

	if(province::is_sea(state, prov)) {
		if(province::province_is_deep_waters(state, prov)) {
			return 1.0f; // Can't blockade deep waters (for balance)
		}
		else {
			float blockade_power = military::navy_strength_present<military::battle_allowed::yes, military::retreat_allowed::no, military::participants_included::enemies>(state, prov, nation_as);
			float province_size_km2 = state.map_state.map_data.province_area_km2[province::to_map_id(prov)];
			float blockade_mod = blockade_power * province_size_km2 / 1000.0f;
			return navy_supply_throughput_coastal_sea_blockade_threshold > 0.0f ? std::max((navy_supply_throughput_coastal_sea_blockade_threshold - blockade_mod) / navy_supply_throughput_coastal_sea_blockade_threshold, 0.f) : 1.0f;
		}
	} else {
		float enemy_strength_present = military::army_strength_present<military::battle_allowed::yes, military::retreat_allowed::no, military::participants_included::enemies>(state, prov, nation_as);
		return army_supply_throughput_blockade_threshold > 0.0f ? std::max((army_supply_throughput_blockade_threshold - enemy_strength_present) / army_supply_throughput_blockade_threshold, 0.f) : 1.0f;
	}
}

float supply_throughput_percentage_modifier(const sys::state& state, dcon::province_id prov, dcon::nation_id nation_as) {
	// prev_prov may be invalid to signifiy no previous province
	assert(prov);
	assert(nation_as);

	float from_access = supply_throughput_percentage_access_modifier(state, prov, nation_as);
	float from_blockade = supply_throughput_percentage_blockade_modifier(state, prov, nation_as);
	float result =  std::max(1.0f - ((1.0f - from_access) + (1.0f - from_blockade)), 0.0f);
	assert(result >= 0.0f && result <= 1.0f);
	return result;

}
float combined_supply_throughput_modifier(const sys::state& state, dcon::province_id prov, dcon::nation_id nation_as) {
	return supply_throughput_modifier(state, prov, nation_as) * supply_throughput_percentage_modifier(state, prov, nation_as);
}

float supply_throughput_efficiency(const sys::state& state, dcon::province_id prov, dcon::nation_id nation_as) {
	// prev_prov may be invalid to signifiy no previous province
	assert(prov);
	assert(nation_as);
	float used_supply_throughput = state.world.province_get_used_supply_throughput(prov);
	float throughput = combined_supply_throughput_modifier(state, prov, nation_as);
	float effective_throughput_rate = std::min(throughput / (used_supply_throughput == 0.0f ? 1.0f : used_supply_throughput), 1.0f);
	return effective_throughput_rate;
}

float supply_throughput_efficiency_with_extra_weight(const sys::state& state, dcon::province_id prov, dcon::nation_id nation_as, float extra_used_throughput) {
	// prev_prov may be invalid to signifiy no previous province
	assert(prov);
	assert(nation_as);
	float used_supply_throughput = state.world.province_get_used_supply_throughput(prov) + extra_used_throughput;
	float throughput = combined_supply_throughput_modifier(state, prov, nation_as);
	float effective_throughput_rate = std::min(throughput / (used_supply_throughput == 0.0f ? 1.0f : used_supply_throughput), 1.0f);
	return effective_throughput_rate;
}

float province_supply_attrition_modifier(const sys::state& state, dcon::province_id province, dcon::nation_id nation_as) {
	bool province_is_sea = province::is_sea(state, province);
	if(province_is_sea) {
		auto enemy_strength_present = military::navy_strength_present<military::battle_allowed::no, military::retreat_allowed::no, military::participants_included::enemies>(state, province, nation_as);
		return std::clamp(1.0f - (base_sea_supply_attrition + hostile_navy_supply_attrition * (enemy_strength_present / 100.0f)), 0.1f, 1.0f);
	}
	auto enemy_strength_present = military::army_strength_present<military::battle_allowed::no, military::retreat_allowed::no, military::participants_included::enemies>(state, province, nation_as);

	auto province_is_occupied = (state.world.province_get_nation_from_province_control(province) != state.world.province_get_nation_from_province_ownership(province));
	float total_militancy = state.world.province_get_demographics(province, demographics::militancy);
	float total_pop = state.world.province_get_demographics(province, demographics::total);
	float avg_militancy = (total_pop == 0.0f ? 0.0f : total_militancy / total_pop);
	float control_level = state.world.province_get_control_ratio(province);
	return std::clamp(1.0f - (base_land_supply_attrition + control_level_supply_attrition * (1.0f - control_level) + militancy_supply_attrition * avg_militancy + hostile_army_supply_attrition * enemy_strength_present), 0.1f, 1.0f);
}

float avg_adjacency_supply_attrition_modifier(const sys::state& state, dcon::province_id prov_1, dcon::province_id prov_2, dcon::nation_id nation_as) {
	assert(state.world.get_province_adjacency_by_province_pair(prov_1, prov_2));
	auto avg_supply_attr = (province_supply_attrition_modifier(state, prov_1, nation_as) + province_supply_attrition_modifier(state, prov_2, nation_as)) / 2.0f;
	return avg_supply_attr;
}
float avg_adjacency_supply_attrition_modifier(const sys::state& state, dcon::province_adjacency_id province_adj, dcon::nation_id nation_as) {
	auto prov_1 = state.world.province_adjacency_get_connected_provinces(province_adj, 0);
	auto prov_2 = state.world.province_adjacency_get_connected_provinces(province_adj, 1);
	return avg_adjacency_supply_attrition_modifier(state, prov_1, prov_2, nation_as);
}

float adjacency_supply_attrition_modifier(const sys::state& state, dcon::province_adjacency_id province_adj, dcon::nation_id nation_as) {
	auto prov_1 = state.world.province_adjacency_get_connected_provinces(province_adj, 0);
	auto prov_2 = state.world.province_adjacency_get_connected_provinces(province_adj, 1);
	auto avg_supply_attr = avg_adjacency_supply_attrition_modifier(state, province_adj, nation_as);
	auto distance = state.world.province_adjacency_get_distance_km(province_adj) * military::get_avg_movement_cost_modifier(state, nation_as, prov_1, prov_2);
	assert(std::isfinite(distance * avg_supply_attr));
	return std::powf(avg_supply_attr, distance);
}

float calculate_supply_route_throughput(const sys::state& state, std::span<const dcon::province_id> path, dcon::province_id origin_prov, dcon::province_id destination, dcon::nation_id controller) {
	assert(origin_prov);
	assert(destination);
	// If destination and origin are diffrent, and path size is 0, that means no path is available.
	if(destination != origin_prov && path.size() == 0) {
		return 0.0f;
	}
	float smallest_supply_throughput = 1.0f;
	for(uint32_t i = 0; i < path.size(); i++) {
		auto prov = path[i];
		float province_throughput = supply_throughput_efficiency(state, prov, controller);
		smallest_supply_throughput = std::min(smallest_supply_throughput, province_throughput);
	}
	return smallest_supply_throughput;
}

float calculate_supply_route_attrition(const sys::state& state, std::span<const dcon::province_id> path, dcon::province_id origin_prov, dcon::province_id destination, dcon::nation_id controller) {
	assert(origin_prov);
	assert(destination);
	// If destination and origin are diffrent, and path size is 0, that means no path is available.
	if(destination != origin_prov && path.size() == 0) {
		return 1.0f;
	}
	float total_attrition_mod = 1.0f;
	for(uint32_t i = 0; i < path.size(); i++) {
		auto prov = path[i];
		if(i + 1 < path.size()) {
			auto next_prov = path[i + 1];
			auto adj = state.world.get_province_adjacency_by_province_pair(prov, next_prov);
			assert(adj);
			total_attrition_mod *= adjacency_supply_attrition_modifier(state, adj, controller);
			assert(std::isfinite(total_attrition_mod));
		}
	}
	return std::max(total_attrition_mod, 0.5f); // capped at 50% loss
}

template<concepts::military_unit unit_type>
struct pending_military_supply_route {
	unit_type unit{};
	dcon::market_id origin{};
	float volume = 0.0f;
	float route_throughput = 0.0f;
	float route_attrition = 0.0f;
	std::vector<dcon::province_id> path;
	economy::supply_cost_union_commodity_amount_array buffered_supply_goods{};
	economy::build_cost_union_commodity_amount_array buffered_reinforcement_goods{};
};
template<concepts::military_construction_type construction_type>
struct pending_construction_supply_route {
	construction_type construction{};
	dcon::market_id origin{};
	float volume = 0.0f;
	float route_throughput = 0.0f;
	float route_attrition = 0.0f;
	std::vector<dcon::province_id> path;
	economy::unit_build_cost_commodity_amount_array buffered_goods{};

};

constexpr float used_throughput_integer_precision = 1000000.f; // 6 digits of precision when converting the used throughput to fixed point to keep deterministic results when combining later

void add_used_supply_throughput_to_buffer_from_path(sys::state& state, std::span<const dcon::province_id> path, float amount, tagged_vector<int64_t, dcon::province_id>& buffer) {
	int64_t used_throughput_increase = int64_t(amount * used_throughput_integer_precision);
	std::for_each(path.begin(), path.end(), [&](dcon::province_id prov) {
		buffer[prov] += used_throughput_increase;
	});
}

void remove_used_supply_throughput_to_buffer_from_path(sys::state& state, std::span<const dcon::province_id> path, float amount, tagged_vector<int64_t, dcon::province_id>& buffer) {
	int64_t used_throughput_increase = int64_t(amount * used_throughput_integer_precision);
	std::for_each(path.begin(), path.end(), [&](dcon::province_id prov) {
		buffer[prov] -= used_throughput_increase;
	});
}


template<concepts::supply_route_type route_type>
void update_supply_route_throughput_attrition(sys::state& state, route_type r, dcon::nation_id controller) {

	auto route = fatten(state.world, r);
	auto origin_prov = supply_route_get_origin(state, r);
	auto route_dest = supply_route_get_destination(state, r);
	auto path = route.get_path();
	auto path_span = std::span<const dcon::province_id>(path.begin(), path.end());
	auto route_attr = calculate_supply_route_attrition(state, path_span, origin_prov, route_dest, controller);
	auto throughput = calculate_supply_route_throughput(state, path_span, origin_prov, route_dest, controller);
	route.set_route_attrition(route_attr);
	route.set_throughput(throughput);
}

template<concepts::supply_route_type route_type>
void update_supply_route_path(sys::state& state, route_type r) {

	auto route = fatten(state.world, r);
	auto market = route.get_origin();
	auto state_inst = market.get_zone_from_local_market();
	dcon::nation_id route_owner = supply_routes::supply_route_get_owner(state, r);
	dcon::province_id destination_location = supply_routes::supply_route_get_destination(state, r);
	float volume = route.get_volume();
	static thread_local std::vector<dcon::province_id> path;
	path.clear();
	province::make_military_supply_path(state, state_inst, destination_location, route_owner, volume, path);
	auto existing_path = route.get_path();
	existing_path.clear();
	existing_path.load_range(path.data(), path.data() + path.size());
	update_supply_route_throughput_attrition(state, r, route_owner);
	route.set_path_out_of_date(false);
}


template<concepts::supply_route_type route_type>
bool path_needs_update(const sys::state& state, route_type r) {
	auto route = fatten(state.world, r);
	auto path_out_of_date = route.get_path_out_of_date();
	auto throughput = route.get_throughput();
	// Throughput of 0 means either no path available, or the path available is useless
	return path_out_of_date || throughput == 0.0f;
}

// Tests if the route needs an update, and updates it if it does
template<concepts::supply_route_type route_type>
void try_update_route_path(sys::state& state, route_type r, tagged_vector<int64_t, dcon::province_id>& province_throughput_changes ) {
	if(path_needs_update(state, r)) {
		//Try make new path
		update_supply_route_path(state, r, province_throughput_changes);
	}
}


// Creates a "pending" supply route, which has the same fields as the supply route in DCON. Use this to buffer routes to be created later in a parallel loop
template<concepts::military_unit unit_type>
pending_military_supply_route<unit_type> create_pending_supply_route(const sys::state& state, unit_type unit, dcon::market_id origin, float expected_volume) {
	static thread_local std::vector<dcon::province_id> path{};
	path.clear();
	auto state_inst = state.world.market_get_zone_from_local_market(origin);
	auto capital = state.world.state_instance_get_capital(state_inst);
	dcon::province_id unit_loc = military::unit_get_location(state, unit);
	dcon::nation_id controller = military::unit_get_controller(state, unit);
	province::make_military_supply_path(state, state_inst, unit_loc, controller, expected_volume, path);
	pending_military_supply_route<unit_type> pending_route{
		.unit = unit,
		.origin = origin,
		.volume = 0.0f,
		.route_throughput = calculate_supply_route_throughput(state, path,  capital, unit_loc, controller),
		.route_attrition = calculate_supply_route_attrition(state, path,  capital, unit_loc, controller),
		.path = std::vector<dcon::province_id>(path),
		.buffered_supply_goods = economy::supply_cost_union_commodity_amount_array(state.military_definitions.military_supply_goods.size()),
		.buffered_reinforcement_goods = economy::build_cost_union_commodity_amount_array(state.military_definitions.military_build_goods.size())
	};
	return pending_route;
}

// Creates a "pending" supply route, which has the same fields as the supply route in DCON. Use this to buffer routes to be created later in a parallel loop
template<concepts::military_construction_type construction_type>
pending_construction_supply_route<construction_type> create_pending_supply_route(const sys::state& state, construction_type c, dcon::market_id origin, float expected_volume) {
	auto construction = fatten(state.world, c);
	auto type = construction.get_type();
	uint32_t goods_array_size = state.military_definitions.unit_base_definitions[type].build_cost.size_used();
	static thread_local std::vector<dcon::province_id> path{};
	path.clear();
	auto state_inst = state.world.market_get_zone_from_local_market(origin);
	auto capital = state.world.state_instance_get_capital(state_inst);
	dcon::province_id con_loc = economy::construction_get_location(state, c);
	dcon::nation_id controller = economy::construction_get_controller(state, c);
	province::make_military_supply_path(state, state_inst, con_loc, controller, expected_volume, path);
	pending_construction_supply_route<construction_type> pending_route{
		.construction = construction,
		.origin = origin,
		.volume = 0.0f,
		.route_throughput = calculate_supply_route_throughput(state, path,  capital, con_loc, controller),
		.route_attrition = calculate_supply_route_attrition(state, path,  capital, con_loc, controller),
		.path = std::vector<dcon::province_id>(path),
		.buffered_goods = economy::unit_build_cost_commodity_amount_array(goods_array_size)
	};
	return pending_route;
}

//// Creates a army supply route and pathfinds automatically, while setting relavent fields on provinces it passes through
//dcon::army_supply_route_id create_supply_route(sys::state& state, dcon::army_id army, dcon::market_id origin) {
//	assert(state.world.army_get_controller_from_army_control(army));
//	auto route = state.world.force_create_army_supply_route(army, origin);
//	// Resize to fit with the total amount of possible supply/reinforcement goods 
//	state.world.army_supply_route_get_buffered_reinforcement_goods(route).resize(state.military_definitions.military_build_goods.size());
//	state.world.army_supply_route_get_buffered_supply_goods(route).resize(state.military_definitions.military_supply_goods.size());
//	update_supply_route_path(state, route);
//	update_supply_route_throughput_attrition(state, route, state.world.army_get_controller_from_army_control(army));
//	return route;
//}
//// Creates a navy supply route and pathfinds automatically, while setting relavent fields on provinces it passes through
//dcon::navy_supply_route_id create_supply_route(sys::state& state, dcon::navy_id navy, dcon::market_id origin) {
//	assert(state.world.navy_get_controller_from_navy_control(navy));
//	auto route = state.world.force_create_navy_supply_route(navy, origin);
//	state.world.navy_supply_route_get_buffered_reinforcement_goods(route).resize(state.military_definitions.military_build_goods.size());
//	state.world.navy_supply_route_get_buffered_supply_goods(route).resize(state.military_definitions.military_supply_goods.size());
//	update_supply_route_path(state, route);
//	update_supply_route_throughput_attrition(state, route, state.world.navy_get_controller_from_navy_control(navy));
//	return route;
//}
//
//// Creates a land construction supply route and pathfinds automatically, while setting relavent fields on provinces it passes through
//dcon::land_construction_supply_route_id create_supply_route(sys::state& state, dcon::province_land_construction_id construction, dcon::market_id origin) {
//	auto route = state.world.force_create_land_construction_supply_route(construction, origin);
//	auto type = state.world.province_land_construction_get_type(construction);
//	const auto& build_cost = state.military_definitions.unit_base_definitions[type].build_cost;
//	auto& route_goods = state.world.land_construction_supply_route_get_buffered_goods(route);
//	// Push back one element for each commodity in the build_cost for the unit type, so they have the same size
//	build_cost.for_each_commodity([&](dcon::commodity_id, float) {
//		route_goods.push_back_unsafe(0.0f);
//	});
//
//	update_supply_route_path(state, route);
//	update_supply_route_throughput_attrition(state, route, state.world.province_land_construction_get_nation(construction));
//	return route;
//}
//
//// Creates a naval construction supply route and pathfinds automatically, while setting relavent fields on provinces it passes through
//dcon::naval_construction_supply_route_id create_supply_route(sys::state& state, dcon::province_naval_construction_id construction, dcon::market_id origin) {
//	auto route = state.world.force_create_naval_construction_supply_route(construction, origin);
//	auto type = state.world.province_naval_construction_get_type(construction);
//	const auto& build_cost = state.military_definitions.unit_base_definitions[type].build_cost;
//	auto& route_goods = state.world.naval_construction_supply_route_get_buffered_goods(route);
//	// Push back one element for each commodity in the build_cost for the unit type, so they have the same size
//	build_cost.for_each_commodity([&](dcon::commodity_id, float) {
//		route_goods.push_back_unsafe(0.0f);
//	});
//
//	update_supply_route_path(state, route);
//	update_supply_route_throughput_attrition(state, route, state.world.province_naval_construction_get_nation(construction));
//	return route;
//}





// Expensive updates, done monthly
void update_supply_routes_monthly(sys::state& state) {
	// update paths for supply routes which do not have full throughput
	/*state.world.for_each_army_supply_route([&](dcon::army_supply_route_id route) {
		auto throughput = state.world.army_supply_route_get_throughput(route);
		if(throughput != 1.0f) {
			update_army_supply_route_path(state, route);
		}
	});
	state.world.for_each_navy_supply_route([&](dcon::navy_supply_route_id route) {
		auto throughput = state.world.navy_supply_route_get_throughput(route);
		if(throughput != 1.0f) {
			update_navy_supply_route_path(state, route);
		}
	});*/
}


template<concepts::commodity_amount_array_type buffer_type>
bool any_needs_left(const sys::state& state, const buffer_type& goods_needs) {
	for(uint32_t i = 0; i < goods_needs.size(); i++) {
		if(goods_needs[i] > 0.0f) {
			return true;
		}
	}
	return false;
};

template<concepts::commodity_amount_military_union_array_type buffer_type>
float military_goods_potential_volume_in_govt_stockpile(const sys::state& state, dcon::market_id origin, const buffer_type& supply_route_need) {
	float potential_volume = 0.0f;
	using commodity_ids_type = concepts::military_commodity_amount_to_id_union<buffer_type>::type;
	const commodity_ids_type& commodity_ids = military::get_military_commodities_union<commodity_ids_type>(state);
	for(uint32_t i = 0; i < commodity_ids.size(); i++) {
		dcon::commodity_id commodity = commodity_ids[i];
		assert(commodity);
		float amount_wanted = supply_route_need[i];
		auto available_stockpile_amount = state.world.market_get_govt_stockpile_satisfaction_buffer(origin, commodity);
		potential_volume += std::min(amount_wanted, available_stockpile_amount);
	}
	return potential_volume;
};

template<concepts::military_construction_type construction_type>
float construction_goods_potential_volume_in_govt_stockpile(const sys::state& state, dcon::market_id origin, construction_type c, const economy::unit_build_cost_commodity_amount_array& supply_route_need) {
	float potential_volume = 0.0f;
	auto construction = fatten(state.world, c);
	dcon::unit_type_id type = construction.get_type();
	const auto& build_cost = state.military_definitions.unit_base_definitions[type].build_cost;
	for(uint32_t i = 0; i < supply_route_need.size(); i++) {
		dcon::commodity_id commodity = build_cost.commodity_type[i];
		assert(commodity);
		float amount_wanted = supply_route_need[i];
		auto available_stockpile_amount = state.world.market_get_govt_stockpile_satisfaction_buffer(origin, commodity);
		potential_volume += std::min(amount_wanted, available_stockpile_amount);
	}
	return potential_volume;
};

template<concepts::military_construction_type construction_type>
bool construction_goods_potential_in_govt_stockpile(const sys::state& state, dcon::market_id origin, construction_type c, const economy::unit_build_cost_commodity_amount_array& supply_route_need) {
	auto construction = fatten(state.world, c);
	dcon::unit_type_id type = construction.get_type();
	const auto& build_cost = state.military_definitions.unit_base_definitions[type].build_cost;
	for(uint32_t i = 0; i < supply_route_need.size(); i++) {
		dcon::commodity_id commodity = build_cost.commodity_type[i];
		assert(commodity);
		float amount_wanted = supply_route_need[i];
		auto available_stockpile_amount = state.world.market_get_govt_stockpile_satisfaction_buffer(origin, commodity);
		if(amount_wanted > 0.0f && available_stockpile_amount > 0.0001f) {
			return true;
		}
	}
	return false;
};

template<concepts::commodity_amount_military_union_array_type buffer_type>
bool military_goods_potential_in_govt_stockpile(const sys::state& state, dcon::market_id origin, const buffer_type& supply_route_need) {
	using commodity_ids_type = concepts::military_commodity_amount_to_id_union<buffer_type>::type;
	const commodity_ids_type& commodity_ids = military::get_military_commodities_union<commodity_ids_type>(state);
	for(uint32_t i = 0; i < commodity_ids.size(); i++) {
		dcon::commodity_id commodity = commodity_ids[i];
		assert(commodity);
		float amount_wanted = supply_route_need[i];
		auto available_stockpile_amount = state.world.market_get_govt_stockpile_satisfaction_buffer(origin, commodity);
		if(amount_wanted > 0.0f && available_stockpile_amount > 0.0001f) {
			return true;
		}
	}
	return false;
};




// Updates the supply route buffered goods, volume and subtracts the goods consumed from the stockpile buffers of all potential supply routes connected to the military unit.
// Supply type decides whether to update supply, or reinforcement goods
template<concepts::military_unit unit_type>
void update_military_unit_routes_satisfaction(sys::state& state, unit_type unit,  dcon::nation_id nation, economy::supply_cost_union_commodity_amount_array& supply_needs, economy::build_cost_union_commodity_amount_array& reinf_needs, const economy::supply_and_build_cost_union_commodity_amount_array& needs_multiplier, std::span<const dcon::state_instance_id> stockpiles_buffer, std::vector<pending_military_supply_route<unit_type>>& pending_routes) {



	auto process_route = [&]<concepts::commodity_amount_military_supply_or_build_union_array_type commodity_array_type>(dcon::market_id market, commodity_array_type& goods_needs, commodity_array_type& buffered_goods, float& volume, float goods_attrition, float throughput) {
		// Update satisfaction of the route by decrementing the market govt stockpile satisfaction buffer to keep track of how many goods are left.
			// The containers storing the buffered supply&reinforcement for the route is assumed the same indexes as the respective commodity types in military_definitions (commodity_ids container)
			// That means we can simply iterate over the container with indexes and expect the goods_needed and buffered_supply/reinforcement containers to line up

		using commodity_ids_type = concepts::military_commodity_amount_to_id_union<commodity_array_type>::type;
		const commodity_ids_type& commodity_ids = military::get_military_commodities_union<commodity_ids_type>(state);
		for(uint32_t i = 0; i < commodity_ids.size(); i++) {
			auto com_id = commodity_ids[i];
			assert(com_id);

			float& amount_needed = goods_needs[i];
			if(amount_needed <= 0.0f) {
				continue;
			}
			//float should_consume = amount_needed * needs_multiplier[index];
			float stockpile_buffer_amount = state.world.market_get_govt_stockpile_satisfaction_buffer(market, com_id);
			// The amount to consume is the minimum of the desired amount or the amount available in stockpile, multiplied by the route throughput which limits how many supplies may be routed
			float to_consume = std::min(amount_needed, stockpile_buffer_amount) * throughput;
			// Compute how much to consume to compensate for the expected attrition on the route.
			float to_consume_w_attrition = std::min(amount_needed / goods_attrition, stockpile_buffer_amount) * throughput;
			assert(stockpile_buffer_amount - to_consume >= 0.0f);
			assert(stockpile_buffer_amount - to_consume_w_attrition >= 0.0f);
			// Update stockpile buffer to reflect the amount that will be subtracted later
			state.world.market_set_govt_stockpile_satisfaction_buffer(market, com_id, stockpile_buffer_amount - to_consume_w_attrition);
			float total_stockpile_buffer_count = state.world.nation_get_temp_total_stockpiles_buffer(nation, com_id);
			state.world.nation_set_temp_total_stockpiles_buffer(nation, com_id, std::max(total_stockpile_buffer_count - to_consume_w_attrition, 0.0f));
			buffered_goods[i] += to_consume_w_attrition;
			
			volume += to_consume_w_attrition;
			// Subtract from route need. Subtract the non-attrition compenstated amount as that is the amount we expect will actually make it through the route
			amount_needed -= to_consume;
		}
	};
	const auto& supply_goods_ids = military::get_military_commodities_union<economy::supply_cost_union_commodity_id_array>(state);
	const auto& reinf_goods_ids = military::get_military_commodities_union<economy::build_cost_union_commodity_id_array>(state);
	for(uint32_t i = 0; i < supply_goods_ids.size(); i++) {
		auto com_id = supply_goods_ids[i];
		auto union_index = state.world.commodity_get_unit_supply_build_goods_index(com_id);
		supply_needs[i] *= needs_multiplier[union_index];
	}
	for(uint32_t i = 0; i < reinf_goods_ids.size(); i++) {
		auto com_id = reinf_goods_ids[i];
		auto union_index = state.world.commodity_get_unit_supply_build_goods_index(com_id);
		reinf_needs[i] *= needs_multiplier[union_index];
	}


	for(auto stockpile_state : stockpiles_buffer) {
		auto market = state.world.state_instance_get_market_from_local_market(stockpile_state);
		bool need_more_goods = any_needs_left(state, supply_needs) || any_needs_left(state, reinf_needs);

		if(!need_more_goods) {
			break;
		}
		bool potential_goods = military_goods_potential_in_govt_stockpile(state, market, supply_needs) || military_goods_potential_in_govt_stockpile(state, market, reinf_needs);
		if(!potential_goods) {
			continue;
		}

		auto r = [&]() {
			if constexpr(std::is_same_v<unit_type, dcon::army_id>)
				return state.world.get_army_supply_route_by_origin_army_pair(unit, market);
			else if constexpr(std::is_same_v<unit_type, dcon::navy_id>) {
				return state.world.get_navy_supply_route_by_origin_navy_pair(unit, market);
			}
		}();
		// If route already exists, then work on that
		if(r) {
			auto route = fatten(state.world, r);
			if(route.get_throughput() == 0.0f) {
				// Not worth creating as it has no throughput (no path available)
				continue;
			}
	
			process_route(market, supply_needs, route.get_buffered_supply_goods(), route.get_volume(), route.get_route_attrition(), route.get_throughput());
			process_route(market, reinf_needs, route.get_buffered_reinforcement_goods(), route.get_volume(), route.get_route_attrition(), route.get_throughput());
			route.set_is_active(true);
		}
		// Otherwise, create a pending route, and work on that, then push it to the vector to be created later
		else {
			float potential_volume = military_goods_potential_volume_in_govt_stockpile(state, market, supply_needs) +
									 military_goods_potential_volume_in_govt_stockpile(state, market, reinf_needs);
			pending_military_supply_route<unit_type> new_pending_route = create_pending_supply_route(state, unit, market, potential_volume);
			if(new_pending_route.route_throughput == 0.0f) {
				// Not worth creating as it has no throughput (no path available)
				continue;
			}
			process_route(market, supply_needs, new_pending_route.buffered_supply_goods, new_pending_route.volume, new_pending_route.route_attrition, new_pending_route.route_throughput);
			process_route(market, reinf_needs, new_pending_route.buffered_reinforcement_goods, new_pending_route.volume, new_pending_route.route_attrition, new_pending_route.route_throughput);
			pending_routes.push_back(std::move(new_pending_route));
			
		}
	}
}




// Updates the supply route buffered goods, volume and subtracts the goods consumed from the stockpile buffers of all potential supply routes connected to the military unit.
// Supply type decides whether to update supply, or reinforcement goods
template<concepts::military_construction_type construction_type>
void update_construction_routes_satisfaction(sys::state& state, construction_type conc, dcon::nation_id nation, economy::unit_build_cost_commodity_amount_array& goods_needs, const economy::build_cost_union_commodity_amount_array& needs_multiplier, std::span<const dcon::state_instance_id> stockpiles_buffer, std::vector<pending_construction_supply_route<construction_type>>& pending_routes) {

	if(!economy::can_advance_construction(state, conc)) {
		return;
	}


	auto construction = fatten(state.world, conc);
	auto type = construction.get_type();
	const economy::commodity_set& build_cost = state.military_definitions.unit_base_definitions[type].build_cost;

	for(uint32_t i = 0; i < build_cost.set_size; i++) {
		auto com_id = build_cost.commodity_type[i];
		if(com_id) {
			auto build_union_index = state.world.commodity_get_unit_build_goods_index(com_id);
			goods_needs[i] *= needs_multiplier[build_union_index];
		}
		else {
			break;
		}
	}

	for(auto stockpile_state : stockpiles_buffer) {
		auto market = state.world.state_instance_get_market_from_local_market(stockpile_state);
		bool need_more_goods = any_needs_left(state, goods_needs);
		if(!need_more_goods) {
			break;
		}
		bool potential_goods = construction_goods_potential_in_govt_stockpile(state, market, conc, goods_needs);
		if(!potential_goods) {
			continue;
		}
		auto r = [&]() {
			if constexpr(std::is_same_v<construction_type, dcon::province_land_construction_id>) {
				return state.world.get_land_construction_supply_route_by_origin_construction_pair(construction, market);
			} else if constexpr(std::is_same_v<construction_type, dcon::province_naval_construction_id>) {
				return state.world.get_naval_construction_supply_route_by_origin_construction_pair(construction, market);
			}
		}();
		// If route already exists, then work on that
		if(r) {
			auto route = fatten(state.world, r);
			if(route.get_throughput() == 0.0f) {
				continue;
			}

			// Update satisfaction of the route by decrementing the market govt stockpile satisfaction buffer to keep track of how many goods are left.
			// The containers storing the buffered goods for the route is assumed the same indexes as the respective build cost for the unit type which the construction is building
			// That means we can simply iterate over the container with indexes and expect the goods_needed and buffered_goods containers to line up
			for(uint32_t i = 0; i < build_cost.set_size; i++) {
				dcon::commodity_id com_id = build_cost.commodity_type[i];
				if(com_id) {
					float& amount_needed = goods_needs[i];
					if(amount_needed <= 0.0f) {
						continue;
					}
					float stockpile_buffer_amount = state.world.market_get_govt_stockpile_satisfaction_buffer(market, com_id);
					// The amount to consume is the minimum of the desired amount or the amount available in stockpile, multiplied by the route throughput which limits how many supplies may be routed
					float to_consume = std::min(amount_needed, stockpile_buffer_amount) * route.get_throughput();
					// Compute how much to consume to compensate for the expected attrition on the route.
					float to_consume_w_attrition = std::min(amount_needed / route.get_route_attrition(), stockpile_buffer_amount) * route.get_throughput();
					assert(stockpile_buffer_amount - to_consume >= 0.0f);
					assert(stockpile_buffer_amount - to_consume_w_attrition >= 0.0f);
					// Update stockpile buffer to reflect the amount that will be subtracted later
					state.world.market_set_govt_stockpile_satisfaction_buffer(market, com_id, stockpile_buffer_amount - to_consume_w_attrition);
					float total_stockpile_buffer_count = state.world.nation_get_temp_total_stockpiles_buffer(nation, com_id);
					state.world.nation_set_temp_total_stockpiles_buffer(nation, com_id, std::max(total_stockpile_buffer_count - to_consume_w_attrition, 0.0f));


					route.get_buffered_goods()[i] += to_consume_w_attrition;

					route.set_volume(route.get_volume() + to_consume_w_attrition);
					// Subtract from route need. Subtract the non-attrition compenstated amount as that is the amount we expect will actually make it through the route
					amount_needed -= to_consume;
				} else {
					break;
				}
			}
			route.set_is_active(true);
		}
		// Otherwise, create a pending route, and work on that, then push it to the vector
		else {
			float potential_volume = construction_goods_potential_volume_in_govt_stockpile(state, market, conc, goods_needs);
			pending_construction_supply_route<construction_type> new_pending_route = create_pending_supply_route(state, conc, market, potential_volume);
			if(new_pending_route.route_throughput == 0.0f) {
				continue;
			}
			// Update satisfaction of the route by decrementing the market govt stockpile satisfaction buffer to keep track of how many goods are left.
			// The containers storing the buffered goods for the route is assumed the same indexes as the respective build cost for the unit type which the construction is building
			// That means we can simply iterate over the container with indexes and expect the goods_needed and buffered_goods containers to line up
			for(uint32_t i = 0; i < build_cost.set_size; i++) {
				dcon::commodity_id com_id = build_cost.commodity_type[i];
				if(com_id) {
					float& amount_needed = goods_needs[i];
					if(amount_needed <= 0.0f) {
						continue;
					}
					float stockpile_buffer_amount = state.world.market_get_govt_stockpile_satisfaction_buffer(market, com_id);
					// The amount to consume is the minimum of the desired amount or the amount available in stockpile, multiplied by the route throughput which limits how many supplies may be routed
					float to_consume = std::min(amount_needed, stockpile_buffer_amount) * new_pending_route.route_throughput;
					// Compute how much to consume to compensate for the expected attrition on the route.
					float to_consume_w_attrition = std::min(amount_needed / new_pending_route.route_attrition, stockpile_buffer_amount) * new_pending_route.route_throughput;
					assert(stockpile_buffer_amount - to_consume >= 0.0f);
					assert(stockpile_buffer_amount - to_consume_w_attrition >= 0.0f);
					// Update stockpile buffer to reflect the amount that will be subtracted later
					state.world.market_set_govt_stockpile_satisfaction_buffer(market, com_id, stockpile_buffer_amount - to_consume_w_attrition);
					float total_stockpile_buffer_count = state.world.nation_get_temp_total_stockpiles_buffer(nation, com_id);
					state.world.nation_set_temp_total_stockpiles_buffer(nation, com_id, std::max(total_stockpile_buffer_count - to_consume_w_attrition, 0.0f));

					new_pending_route.buffered_goods[i] += to_consume_w_attrition;

					new_pending_route.volume += to_consume_w_attrition;
					// Subtract from route need. Subtract the non-attrition compenstated amount as that is the amount we expect will actually make it through the route
					amount_needed -= to_consume;
				} else {
					break;
				}
			}
			pending_routes.push_back(std::move(new_pending_route));
		}
	}
}

template<concepts::military_construction_type construction_type>
void accumulate_construction_requirements(const sys::state& state, construction_type c, economy::unit_build_cost_commodity_amount_array& buffer) {
	auto construction = fatten(state.world, c);
	buffer.clear();
	auto nation = construction.get_nation();
	auto type = construction.get_type();
	dcon::province_id location = economy::construction_get_location(state, c);
	auto build_cost_mult = economy::build_cost_multiplier(state, location, false);
	const economy::commodity_set& currently_fufilled = construction.get_purchased_goods();
	const auto& base_build_cost = state.military_definitions.unit_base_definitions[type].build_cost;
	for(uint32_t i = 0; i < base_build_cost.set_size; i++) {
		if(base_build_cost.commodity_type[i]) {
			assert(base_build_cost.commodity_type[i] == currently_fufilled.commodity_type[i]);
			float fufilled = currently_fufilled.commodity_amounts[i];
			float full_cost = base_build_cost.commodity_amounts[i] * build_cost_mult;
			float remaining_cost = std::max(full_cost - fufilled, 0.0f);
			float can_consume_per_day = full_cost;
			float to_consume = std::min(remaining_cost, can_consume_per_day);
			buffer.push_back(to_consume);
		} else {
			break;
		}
	}
}
template<concepts::military_unit unit_type >
void accumulate_military_requirements(const sys::state& state, unit_type u, economy::supply_cost_union_commodity_amount_array& supply_buffer, economy::build_cost_union_commodity_amount_array& reinf_buffer) {
	auto unit = fatten(state.world, u);
	dcon::nation_id nation = military::unit_get_controller(state, unit.id);
	dcon::province_id location = military::unit_get_location(state, unit.id);

	float supply_consumption;
	float reinforcement_consumption;
	if constexpr(std::is_same_v<unit_type, dcon::army_id>) {
		if(!nation) {
			return;
		}
		supply_consumption = state.world.nation_get_land_supply_consumption(nation) / 100.0f;
		reinforcement_consumption = state.world.nation_get_land_reinforcement_consumption(nation) / 100.0f;
	} else if constexpr(std::is_same_v<unit_type, dcon::navy_id>) {
		supply_consumption = state.world.nation_get_naval_supply_consumption(nation) / 100.0f;
		reinforcement_consumption = state.world.nation_get_naval_reinforcement_consumption(nation) / 100.0f;
	}
	auto unit_membership = military::unit_get_membership(state, unit.id);
	for(auto r : unit_membership) {
		// Accumulate the commodities needed
		auto subunit = [&]() {
			if constexpr(std::is_same_v<unit_type, dcon::army_id>) {
				return r.get_regiment();
			}
			else if constexpr(std::is_same_v<unit_type, dcon::navy_id>) {
				return r.get_ship();
			}
		}();
		dcon::unit_type_id type = subunit.get_type();
		const auto& supply_cost = state.military_definitions.unit_base_definitions[type].supply_cost;
		float total_sup_mods = military::get_supply_cost_modifiers(state, subunit.id);

		const auto& build_cost = state.military_definitions.unit_base_definitions[type].build_cost;
		float potential_reinforcement = military::estimate_reinforcement<military::interval_estimation::daily, military::supply_estimation::full_supply_always, false>(state, subunit.id);
		// Update the commodites required
		supply_cost.for_each_commodity([&](dcon::commodity_id cid, float amount) {
			auto supply_idx = state.world.commodity_get_unit_supply_goods_index(cid);
			assert(supply_idx >= 0);

			float full_supply_req = amount * total_sup_mods * supply_consumption;
			supply_buffer[supply_idx] += full_supply_req;
		});
		build_cost.for_each_commodity([&](dcon::commodity_id cid, float amount) {
			auto reinf_idx = state.world.commodity_get_unit_build_goods_index(cid);
			assert(reinf_idx >= 0);

			float full_reinf_req = amount * potential_reinforcement * reinforcement_consumption;
			reinf_buffer[reinf_idx] += full_reinf_req;
		});
	}
}
template<concepts::military_unit unit_type>
void update_unit_commodity_satisfaction(sys::state& state, unit_type u) {
	auto unit = fatten(state.world, u);
	auto unit_membership = military::unit_get_membership(state, u);
	dcon::nation_id nation = military::unit_get_controller(state, unit.id);
	if constexpr(std::is_same_v<unit_type, dcon::army_id>) {
		// Rebels always have no supply or reinforcement for now
		if(!nation) {
			for(auto r : unit_membership) {
				auto reg = r.get_regiment();
				reg.set_supply_satisfaction(0.0f);
				reg.set_last_supply_cost_modifier(0.0f);
				reg.set_reinforcement_satisfaction(0.0f);
				reg.set_total_pending_reinforcement(0.0f);
				reg.set_last_potential_reinforcement(0.0f);
			}
			return;
		}
	}
	auto routes = military::unit_get_supply_routes(state, unit.id);
	float supply_consumption_rate;
	float reinforcement_consumption_rate;
	if constexpr(std::is_same_v<unit_type, dcon::army_id>) {
		supply_consumption_rate = state.world.nation_get_land_supply_consumption(nation) / 100.0f;
		reinforcement_consumption_rate = state.world.nation_get_land_reinforcement_consumption(nation) / 100.0f;
	} else if constexpr(std::is_same_v<unit_type, dcon::navy_id>) {
		supply_consumption_rate = state.world.nation_get_naval_supply_consumption(nation) / 100.0f;
		reinforcement_consumption_rate = state.world.nation_get_naval_reinforcement_consumption(nation) / 100.0f;
	}

	const auto& supply_commodity_ids = military::get_military_commodities_union<economy::supply_cost_union_commodity_id_array>(state);
	const auto& reinf_commodity_ids = military::get_military_commodities_union<economy::build_cost_union_commodity_id_array>(state);

	economy::supply_cost_union_commodity_amount_array available_supply_goods_buffer(supply_commodity_ids.size());
	economy::build_cost_union_commodity_amount_array available_reinforcement_goods_buffer(reinf_commodity_ids.size());

	// Sum up available supply and reinforcement goods from routes
	for(auto route : routes) {
		for(uint32_t i = 0; i < supply_commodity_ids.size(); i++) {
			dcon::commodity_id com_id = supply_commodity_ids[i];
			assert(com_id);
			available_supply_goods_buffer[i] += (route.get_buffered_supply_goods()[i] * route.get_route_attrition());
		}
		for(uint32_t i = 0; i < reinf_commodity_ids.size(); i++) {
			dcon::commodity_id com_id = reinf_commodity_ids[i];
			assert(com_id);
			available_reinforcement_goods_buffer[i] += (route.get_buffered_reinforcement_goods()[i] * route.get_route_attrition());
		}
	}

	// Consume supplies or reinforcement goods
	for(auto r : unit_membership) {
		auto subunit = [&]() {
			if constexpr(std::is_same_v<unit_type, dcon::army_id>) {
				return r.get_regiment();
			} else if constexpr(std::is_same_v<unit_type, dcon::navy_id>) {
				return r.get_ship();
			}
		}();
		
		dcon::unit_type_id type = subunit.get_type();
		{
			// Compute supply satisfaction
			float supply_goods_cost_mod = military::get_supply_cost_modifiers(state, subunit);
			const economy::commodity_set& supply_goods_cost = military::unit_type_get_commodity_costs<military::unit_consumption_type::supply>(state, type);
			float total_supply_goods_desired = 0.0f;
			float total_supply_goods_consumed = 0.0f;
			supply_goods_cost.for_each_commodity([&](dcon::commodity_id com_id, float desired_amount) {
				int16_t index = state.world.commodity_get_unit_supply_goods_index(com_id);
				assert(index >= 0);
				desired_amount *= supply_goods_cost_mod * supply_consumption_rate;
				float max_available = available_supply_goods_buffer[index];
				float to_consume = std::min(max_available, desired_amount);
				assert(max_available - to_consume >= 0.0f);
				available_supply_goods_buffer[index] = max_available - to_consume;
				total_supply_goods_desired += desired_amount;
				total_supply_goods_consumed += to_consume;
			});
			float supply_satisfaction;
			if(total_supply_goods_desired == 0.0f) {
				supply_satisfaction = supply_consumption_rate;
			} else {
				supply_satisfaction = total_supply_goods_consumed / total_supply_goods_desired * supply_consumption_rate;
			}
			subunit.set_supply_satisfaction(supply_satisfaction);
			subunit.set_last_supply_cost_modifier(supply_goods_cost_mod);
		}
		{
			// And then compute reinforcement satisfaction
			const economy::commodity_set& reinf_goods_cost = military::unit_type_get_commodity_costs<military::unit_consumption_type::reinforcement>(state, type);
			float reinf_goods_cost_mod = military::estimate_reinforcement<military::interval_estimation::daily, military::supply_estimation::full_supply_always, false>(state, subunit);
			float total_reinf_goods_desired = 0.0f;
			float total_reinf_goods_consumed = 0.0f;
			reinf_goods_cost.for_each_commodity([&](dcon::commodity_id com_id, float desired_amount) {
				int16_t index = state.world.commodity_get_unit_build_goods_index(com_id);
				assert(index >= 0);
				desired_amount *= reinf_goods_cost_mod * reinforcement_consumption_rate;
				float max_available = available_reinforcement_goods_buffer[index];
				float to_consume = std::min(max_available, desired_amount);
				assert(max_available - to_consume >= 0.0f);
				available_reinforcement_goods_buffer[index] = max_available - to_consume;
				total_reinf_goods_desired += desired_amount;
				total_reinf_goods_consumed += to_consume;
			});
			float reinf_satisfaction;
			if(total_reinf_goods_desired == 0.0f) {
				reinf_satisfaction = reinforcement_consumption_rate;
			} else {
				reinf_satisfaction = total_reinf_goods_consumed / total_reinf_goods_desired * reinforcement_consumption_rate;
			}
			float added_pending_reinforcement = reinf_satisfaction * reinf_goods_cost_mod;
			subunit.set_reinforcement_satisfaction(reinf_satisfaction);
			subunit.set_total_pending_reinforcement(subunit.get_total_pending_reinforcement() + added_pending_reinforcement);
			subunit.set_last_potential_reinforcement(reinf_goods_cost_mod);
			assert(std::isfinite(subunit.get_total_pending_reinforcement()));
		}
	}
}
template<concepts::military_construction_type construction_type>
void update_construction_commodity_satisfaction(sys::state& state, construction_type c) {
	auto construction = fatten(state.world, c);
	dcon::province_id location;
	if constexpr(std::is_same_v<construction_type, dcon::province_land_construction_id>) {
		location = construction.get_pop().get_province_from_pop_location();
	} else if constexpr(std::is_same_v<construction_type, dcon::province_naval_construction_id>) {
		location = construction.get_province();
	}
	auto routes = [&]() {
		if constexpr(std::is_same_v<construction_type, dcon::province_land_construction_id>) {
			return construction.get_land_construction_supply_route();
		} else if constexpr(std::is_same_v<construction_type, dcon::province_naval_construction_id>) {
			return construction.get_naval_construction_supply_route();
		}
	}();
	auto nation = construction.get_nation();
	auto& current_fufilled = construction.get_purchased_goods();
	auto type = construction.get_type();
	const auto& build_costs = state.military_definitions.unit_base_definitions[type].build_cost;
	for(auto route : routes) {
		const auto& route_goods = route.get_buffered_goods();
		for(uint32_t j = 0; j < build_costs.set_size; j++) {
			dcon::commodity_id com_id = build_costs.commodity_type[j];
			assert(build_costs.commodity_type[j] == current_fufilled.commodity_type[j]);
			if(com_id) {
				float& current_amount = current_fufilled.commodity_amounts[j];
				float route_amount = route_goods[j] * route.get_route_attrition();
				current_amount += route_amount;
			} else {
				break;
			}
		}
	}
}

dcon::army_supply_route_id create_supply_route_from_pending(sys::state& state, const pending_military_supply_route<dcon::army_id>& pending) {
	auto id = fatten(state.world, state.world.force_create_army_supply_route(pending.unit, pending.origin));
	id.set_volume(pending.volume);
	id.set_route_attrition(pending.route_attrition);
	id.set_throughput(pending.route_throughput);
	id.set_buffered_supply_goods(pending.buffered_supply_goods);
	id.set_buffered_reinforcement_goods(pending.buffered_reinforcement_goods);
	id.get_path().load_range(pending.path.data(), pending.path.data() + pending.path.size());
	id.set_is_active(true);
	id.set_inactive_days(0);
	return id;
}
dcon::navy_supply_route_id create_supply_route_from_pending(sys::state& state, const pending_military_supply_route<dcon::navy_id>& pending) {
	auto id = fatten(state.world, state.world.force_create_navy_supply_route(pending.unit, pending.origin));
	id.set_volume(pending.volume);
	id.set_route_attrition(pending.route_attrition);
	id.set_throughput(pending.route_throughput);
	id.set_buffered_supply_goods(pending.buffered_supply_goods);
	id.set_buffered_reinforcement_goods(pending.buffered_reinforcement_goods);
	id.get_path().load_range(pending.path.data(), pending.path.data() + pending.path.size());
	id.set_is_active(true);
	id.set_inactive_days(0);
	return id;
}
dcon::land_construction_supply_route_id create_supply_route_from_pending(sys::state& state, const pending_construction_supply_route<dcon::province_land_construction_id>& pending) {
	auto id = fatten(state.world, state.world.force_create_land_construction_supply_route(pending.construction, pending.origin));
	id.set_volume(pending.volume);
	id.set_route_attrition(pending.route_attrition);
	id.set_throughput(pending.route_throughput);
	id.set_buffered_goods(pending.buffered_goods);
	id.get_path().load_range(pending.path.data(), pending.path.data() + pending.path.size());
	id.set_is_active(true);
	id.set_inactive_days(0);
	return id;
}
dcon::naval_construction_supply_route_id create_supply_route_from_pending(sys::state& state, const pending_construction_supply_route<dcon::province_naval_construction_id>& pending) {
	auto id = fatten(state.world, state.world.force_create_naval_construction_supply_route(pending.construction, pending.origin));
	id.set_volume(pending.volume);
	id.set_route_attrition(pending.route_attrition);
	id.set_throughput(pending.route_throughput);
	id.set_buffered_goods(pending.buffered_goods);
	id.get_path().load_range(pending.path.data(), pending.path.data() + pending.path.size());
	id.set_is_active(true);
	id.set_inactive_days(0);
	return id;
}



struct closest_stockpiles_buffer {
	tagged_vector<std::vector<dcon::state_instance_id>, dcon::army_id> army_closest_stockpiles;
	tagged_vector<std::vector<dcon::state_instance_id>, dcon::navy_id> navy_closest_stockpiles;
	tagged_vector<std::vector<dcon::state_instance_id>, dcon::province_land_construction_id> land_construction_closest_stockpiles;
	tagged_vector<std::vector<dcon::state_instance_id>, dcon::province_naval_construction_id> naval_construction_closest_stockpiles;
};
struct supply_route_consumption_buffer {
	tagged_vector<economy::supply_cost_union_commodity_amount_array, dcon::army_id> army_supply_need;
	tagged_vector<economy::build_cost_union_commodity_amount_array, dcon::army_id> army_reinforcement_need;
	tagged_vector<economy::supply_cost_union_commodity_amount_array, dcon::navy_id> navy_supply_need;
	tagged_vector<economy::build_cost_union_commodity_amount_array, dcon::navy_id> navy_reinforcement_need;
	tagged_vector<economy::supply_and_build_cost_union_commodity_amount_array, dcon::nation_id> nation_unit_low_prio_need;
	tagged_vector<economy::supply_and_build_cost_union_commodity_amount_array, dcon::nation_id> nation_unit_normal_prio_need;
	tagged_vector<economy::supply_and_build_cost_union_commodity_amount_array, dcon::nation_id> nation_unit_high_prio_need;
	tagged_vector<economy::unit_build_cost_commodity_amount_array, dcon::province_land_construction_id> land_constructions_need;
	tagged_vector<economy::unit_build_cost_commodity_amount_array, dcon::province_naval_construction_id> naval_constructions_need;
	tagged_vector<economy::build_cost_union_commodity_amount_array, dcon::nation_id> nation_construction_need;
};



void update_supply_routes_daily(sys::state& state) {

	// STEP 1: initialize buffers in parallel
	auto begin = std::chrono::steady_clock::now();
	concurrency::parallel_invoke(
	[&]() {
		state.world.execute_serial_over_army_supply_route([&](auto route_ids) {
			state.world.army_supply_route_set_volume(route_ids, ve::fp_vector{ 0.0f });
			ve::apply([&](auto route) {
				// Clear commodity amounts. It should not be possible to have new goods appear as build costs for the same unit type at runtime so we can keep the same size
				economy::build_cost_union_commodity_amount_array& reinf_goods = state.world.army_supply_route_get_buffered_reinforcement_goods(route);
				economy::supply_cost_union_commodity_amount_array& supply_goods = state.world.army_supply_route_get_buffered_supply_goods(route);
				std::fill(reinf_goods.begin(), reinf_goods.end(), 0.0f);
				std::fill(supply_goods.begin(), supply_goods.end(), 0.0f);
			}, route_ids);
		});
	},

	[&]() {
		state.world.execute_serial_over_navy_supply_route([&](auto route_ids) {
			state.world.navy_supply_route_set_volume(route_ids, ve::fp_vector{ 0.0f });
			ve::apply([&](auto route) {

				economy::build_cost_union_commodity_amount_array& reinf_goods = state.world.navy_supply_route_get_buffered_reinforcement_goods(route);
				economy::supply_cost_union_commodity_amount_array& supply_goods = state.world.navy_supply_route_get_buffered_supply_goods(route);
				std::fill(reinf_goods.begin(), reinf_goods.end(), 0.0f);
				std::fill(supply_goods.begin(), supply_goods.end(), 0.0f);
			}, route_ids);
		});
	},
	[&]() {
		state.world.execute_serial_over_land_construction_supply_route([&](auto route_ids) {
			state.world.land_construction_supply_route_set_volume(route_ids, ve::fp_vector{ 0.0f });
			state.world.for_each_commodity([&](dcon::commodity_id com_id) {
				ve::apply([&](auto route) {

					economy::unit_build_cost_commodity_amount_array& goods = state.world.land_construction_supply_route_get_buffered_goods(route);
					std::fill(goods.begin(), goods.end(), 0.0f);
				}, route_ids);
			});
		});
		state.world.execute_serial_over_naval_construction_supply_route([&](auto route_ids) {
			state.world.naval_construction_supply_route_set_volume(route_ids, ve::fp_vector{ 0.0f });
			state.world.for_each_commodity([&](dcon::commodity_id com_id) {
				ve::apply([&](auto route) {

					economy::unit_build_cost_commodity_amount_array& goods = state.world.naval_construction_supply_route_get_buffered_goods(route);
					std::fill(goods.begin(), goods.end(), 0.0f);
				}, route_ids);
			});
		});
	},
	[&]() {
		state.world.execute_serial_over_market([&](auto markets) {
			state.world.for_each_commodity([&](dcon::commodity_id com_id) {
				auto to_apply = state.world.market_get_government_stockpile(markets, com_id);
				state.world.market_set_govt_stockpile_satisfaction_buffer(markets, com_id, to_apply);
			});
		});
	},
	[&]() {
		state.world.execute_serial_over_army_supply_route([&](auto routes) {
			state.world.army_supply_route_set_is_active(routes, ve::vbitfield_type{ 0 });
		});
	},
	[&]() {
		state.world.execute_serial_over_navy_supply_route([&](auto routes) {
			state.world.navy_supply_route_set_is_active(routes, ve::vbitfield_type{ 0 });
		});
	},
	[&]() {
		state.world.execute_serial_over_land_construction_supply_route([&](auto routes) {
			state.world.land_construction_supply_route_set_is_active(routes, ve::vbitfield_type{ 0 });
		});
	},
	[&]() {
		state.world.execute_serial_over_naval_construction_supply_route([&](auto routes) {
			state.world.naval_construction_supply_route_set_is_active(routes, ve::vbitfield_type{ 0 });
		});
	},
	[&]() {
		state.world.execute_serial_over_nation([&](auto nations) {
			state.world.for_each_commodity([&](dcon::commodity_id com_id) {
				auto to_apply = state.world.nation_get_total_stockpiles(nations, com_id);
				state.world.nation_set_temp_total_stockpiles_buffer(nations, com_id, to_apply);
			});
		});
	}
	);
	auto end = std::chrono::steady_clock::now();
	state.console_log(std::string("STEP 1 time: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())));
	begin = std::chrono::steady_clock::now();

	static supply_route_consumption_buffer consumption_buffer;
	static closest_stockpiles_buffer stockpiles_buffer;
	stockpiles_buffer.army_closest_stockpiles.resize(state.world.army_size());
	stockpiles_buffer.navy_closest_stockpiles.resize(state.world.navy_size());
	stockpiles_buffer.naval_construction_closest_stockpiles.resize(state.world.province_naval_construction_size());
	stockpiles_buffer.land_construction_closest_stockpiles.resize(state.world.province_land_construction_size());

	consumption_buffer.army_supply_need.resize(state.world.army_size());
	consumption_buffer.army_reinforcement_need.resize(state.world.army_size());
	consumption_buffer.navy_supply_need.resize(state.world.navy_size());
	consumption_buffer.navy_reinforcement_need.resize(state.world.navy_size());
	consumption_buffer.land_constructions_need.resize(state.world.province_land_construction_size());
	consumption_buffer.naval_constructions_need.resize(state.world.province_naval_construction_size());

	consumption_buffer.nation_unit_low_prio_need.resize(state.world.nation_size());
	consumption_buffer.nation_unit_normal_prio_need.resize(state.world.nation_size());
	consumption_buffer.nation_unit_high_prio_need.resize(state.world.nation_size());
	consumption_buffer.nation_construction_need.resize(state.world.nation_size());

	// STEP 2: Compute the closest stockpile states to each military unit and construction, and accumulate all the goods required by each of them into buffers. Run them in parallel
	// We must resize each buffer in each iteration (even if its most likely a no-op) as the user may create a scenario of a diffrent mod on the same application instance, which would crash if buffers are not appropriately sized
	concurrency::parallel_for(uint32_t(0), state.world.army_size(), [&](uint32_t i) {
		dcon::army_id army = dcon::army_id{ dcon::army_id::value_base_t(i) };
		if(!state.world.army_is_valid(army)) {
			return;
		}
		auto nation = state.world.army_get_controller_from_army_control(army);
		if(!nation) {
			return;
		}
		auto location = state.world.army_get_location_from_army_location(army);

		stockpiles_buffer.army_closest_stockpiles[army].clear();
		consumption_buffer.army_supply_need[army].resize(state.military_definitions.military_supply_goods.size());
		consumption_buffer.army_reinforcement_need[army].resize(state.military_definitions.military_build_goods.size());

		std::fill(consumption_buffer.army_supply_need[army].begin(), consumption_buffer.army_supply_need[army].end(), 0.0f);
		std::fill(consumption_buffer.army_reinforcement_need[army].begin(), consumption_buffer.army_reinforcement_need[army].end(), 0.0f);
		economy::get_closest_available_market_states(state, stockpiles_buffer.army_closest_stockpiles[army], nation, location);
		accumulate_military_requirements(state, army, consumption_buffer.army_supply_need[army], consumption_buffer.army_reinforcement_need[army]);
	});

	concurrency::parallel_for(uint32_t(0), state.world.navy_size(), [&](uint32_t i) {
		dcon::navy_id navy = dcon::navy_id{ dcon::navy_id::value_base_t(i) };
		if(!state.world.navy_is_valid(navy)) {
			return;
		}
		auto nation = state.world.navy_get_controller_from_navy_control(navy);
		auto location = state.world.navy_get_location_from_navy_location(navy);
		stockpiles_buffer.navy_closest_stockpiles[navy].clear();
		consumption_buffer.navy_supply_need[navy].resize(state.military_definitions.military_supply_goods.size());
		consumption_buffer.navy_reinforcement_need[navy].resize(state.military_definitions.military_build_goods.size());

		std::fill(consumption_buffer.navy_supply_need[navy].begin(), consumption_buffer.navy_supply_need[navy].end(), 0.0f);
		std::fill(consumption_buffer.navy_reinforcement_need[navy].begin(), consumption_buffer.navy_reinforcement_need[navy].end(), 0.0f);
		economy::get_closest_available_market_states(state, stockpiles_buffer.navy_closest_stockpiles[navy], nation, location);
		accumulate_military_requirements(state, navy, consumption_buffer.navy_supply_need[navy], consumption_buffer.navy_reinforcement_need[navy]);
	});

	concurrency::parallel_for(uint32_t(0), state.world.province_land_construction_size(), [&](uint32_t i) {
		dcon::province_land_construction_fat_id construction = fatten(state.world, dcon::province_land_construction_id{ dcon::province_land_construction_id::value_base_t(i) });
		if(!construction.is_valid()) {
			return;
		}
		auto type = construction.get_type();
		const auto& build_costs = state.military_definitions.unit_base_definitions[type].build_cost;
		auto nation = construction.get_nation();
		auto location = construction.get_pop().get_province_from_pop_location();
		stockpiles_buffer.land_construction_closest_stockpiles[construction].clear();
		consumption_buffer.land_constructions_need[construction].resize(build_costs.size_used());

		std::fill(consumption_buffer.land_constructions_need[construction].begin(), consumption_buffer.land_constructions_need[construction].end(), 0.0f);
		economy::get_closest_available_market_states(state, stockpiles_buffer.land_construction_closest_stockpiles[construction], nation, location);
		accumulate_construction_requirements(state, construction.id, consumption_buffer.land_constructions_need[construction]);

	});
	concurrency::parallel_for(uint32_t(0), state.world.province_naval_construction_size(), [&](uint32_t i) {
		dcon::province_naval_construction_fat_id construction = fatten(state.world, dcon::province_naval_construction_id{ dcon::province_naval_construction_id::value_base_t(i) });
		if(!construction.is_valid()) {
			return;
		}
		auto type = construction.get_type();
		const auto& build_costs = state.military_definitions.unit_base_definitions[type].build_cost;
		auto nation = construction.get_nation();
		auto location = construction.get_province();
		stockpiles_buffer.naval_construction_closest_stockpiles[construction].clear();
		consumption_buffer.naval_constructions_need[construction].resize(build_costs.size_used());

		std::fill(consumption_buffer.naval_constructions_need[construction].begin(), consumption_buffer.naval_constructions_need[construction].end(), 0.0f);
		economy::get_closest_available_market_states(state, stockpiles_buffer.naval_construction_closest_stockpiles[construction], nation, location);
		accumulate_construction_requirements(state, construction.id, consumption_buffer.naval_constructions_need[construction]);
	});

	end = std::chrono::steady_clock::now();
	state.console_log(std::string("STEP 2 time: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())));

	begin = std::chrono::steady_clock::now();

	// STEP 3: Split the total goods required by units in each of the priority brackets (high, medium, low), and accumulate total goods requried for constructions per-nation

	static tagged_vector<std::vector<unit>, dcon::nation_id> low_prio_units;
	static tagged_vector<std::vector<unit>, dcon::nation_id> normal_prio_units;
	static tagged_vector<std::vector<unit>, dcon::nation_id> high_prio_units;
	low_prio_units.resize(state.world.nation_size());
	normal_prio_units.resize(state.world.nation_size());
	high_prio_units.resize(state.world.nation_size());

	concurrency::parallel_for(uint32_t(0), state.world.nation_size(), [&](uint32_t i) {
		dcon::nation_id nation = dcon::nation_id{ dcon::nation_id::value_base_t(i) };
		if(!nations::exists(state, nation)) {
			return;
		}
		consumption_buffer.nation_unit_low_prio_need[nation].resize(state.military_definitions.military_supply_build_goods.size());
		std::fill(consumption_buffer.nation_unit_low_prio_need[nation].begin(), consumption_buffer.nation_unit_low_prio_need[nation].end(), 0.0f);
		consumption_buffer.nation_unit_normal_prio_need[nation].resize(state.military_definitions.military_supply_build_goods.size());
		std::fill(consumption_buffer.nation_unit_normal_prio_need[nation].begin(), consumption_buffer.nation_unit_normal_prio_need[nation].end(), 0.0f);
		consumption_buffer.nation_unit_high_prio_need[nation].resize(state.military_definitions.military_supply_build_goods.size());
		std::fill(consumption_buffer.nation_unit_high_prio_need[nation].begin(), consumption_buffer.nation_unit_high_prio_need[nation].end(), 0.0f);

		consumption_buffer.nation_construction_need[nation].resize(state.military_definitions.military_build_goods.size());
		std::fill(consumption_buffer.nation_construction_need[nation].begin(), consumption_buffer.nation_construction_need[nation].end(), 0.0f);

		low_prio_units[nation].clear();
		normal_prio_units[nation].clear();
		high_prio_units[nation].clear();

		const auto& commodity_ids = state.military_definitions.military_supply_build_goods;

		auto accumulate_prioritized_unit_supply = [&](economy::supply_and_build_cost_union_commodity_amount_array& total_supply_required_buffer, const economy::supply_cost_union_commodity_amount_array& unit_supply_need, const economy::build_cost_union_commodity_amount_array& unit_reinf_need ) {
			for(uint32_t j = 0; j < commodity_ids.size(); j++) {
				auto com_id = commodity_ids[j];
				auto supply_index = state.world.commodity_get_unit_supply_goods_index(com_id);
				auto reinf_index = state.world.commodity_get_unit_build_goods_index(com_id);
				total_supply_required_buffer[j] += (supply_index != -1 ? unit_supply_need[supply_index] : 0.0f);
				total_supply_required_buffer[j] += (reinf_index != -1 ? unit_reinf_need[reinf_index] : 0.0f);
			}
		};

		auto accumulate_construction_supply = [&](dcon::unit_type_id constructing_unit_id, economy::build_cost_union_commodity_amount_array& total_supply_required_buffer, const economy::unit_build_cost_commodity_amount_array& construction_supply_need) {
			const auto& build_costs = state.military_definitions.unit_base_definitions[constructing_unit_id].build_cost;
			for(uint32_t j = 0; j < build_costs.set_size; j++) {
				auto com_id = build_costs.commodity_type[j];
				if(com_id) {
					auto build_costs_index = state.world.commodity_get_unit_build_goods_index(com_id);
					assert(build_costs_index >= 0);
					total_supply_required_buffer[build_costs_index] += construction_supply_need[j];
				}
				else {
					break;
				}
			}
		};
		for(auto construction : state.world.nation_get_province_land_construction(nation)) {
			accumulate_construction_supply(construction.get_type(), consumption_buffer.nation_construction_need[nation], consumption_buffer.land_constructions_need[construction]);
		}
		for(auto construction : state.world.nation_get_province_naval_construction(nation)) {
			accumulate_construction_supply(construction.get_type(), consumption_buffer.nation_construction_need[nation], consumption_buffer.naval_constructions_need[construction]);
		}



		for(auto a : state.world.nation_get_army_control(nation)) {
			auto army = a.get_army();
			switch(army.get_supply_priority()) {
			case military::unit_priority::low_priority:
				accumulate_prioritized_unit_supply(consumption_buffer.nation_unit_low_prio_need[nation], consumption_buffer.army_supply_need[army], consumption_buffer.army_reinforcement_need[army]);
				low_prio_units[nation].emplace_back(unit{ army.id });
				break;
			case military::unit_priority::normal_priority:
				accumulate_prioritized_unit_supply(consumption_buffer.nation_unit_normal_prio_need[nation], consumption_buffer.army_supply_need[army], consumption_buffer.army_reinforcement_need[army]);
				normal_prio_units[nation].emplace_back(unit{ army.id });
				break;
			case military::unit_priority::high_priority:
				accumulate_prioritized_unit_supply(consumption_buffer.nation_unit_high_prio_need[nation], consumption_buffer.army_supply_need[army], consumption_buffer.army_reinforcement_need[army]);
				high_prio_units[nation].emplace_back(unit{ army.id });
				break;
			}
		}
		for(auto a : state.world.nation_get_navy_control(nation)) {
			auto navy = a.get_navy();
			switch(navy.get_supply_priority()) {
			case military::unit_priority::low_priority:
				accumulate_prioritized_unit_supply(consumption_buffer.nation_unit_low_prio_need[nation], consumption_buffer.navy_supply_need[navy], consumption_buffer.navy_reinforcement_need[navy]);
				low_prio_units[nation].emplace_back(unit{ navy.id });
				break;
			case military::unit_priority::normal_priority:
				accumulate_prioritized_unit_supply(consumption_buffer.nation_unit_normal_prio_need[nation], consumption_buffer.navy_supply_need[navy], consumption_buffer.navy_reinforcement_need[navy]);
				normal_prio_units[nation].emplace_back(unit{ navy.id });
				break;
			case military::unit_priority::high_priority:
				accumulate_prioritized_unit_supply(consumption_buffer.nation_unit_high_prio_need[nation], consumption_buffer.navy_supply_need[navy], consumption_buffer.navy_reinforcement_need[navy]);
				high_prio_units[nation].emplace_back(unit{ navy.id });
				break;
			}
		}
	});


	// STEP 4: update the paths of any supply routes which are deemed out-of-date or has no path. They are deemed out of date if anything significant happens to disrupt it and an update is scheduled. 
	// Weekly province updates are issued once per week at diffrent intervals, ie army supply routes are done at the 1st day of the week, navy supply routes on the 2nd day etc
	// Updates triggered by unit movement is done daily 
	end = std::chrono::steady_clock::now();
	state.console_log(std::string("STEP 3 time: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())));
	begin = std::chrono::steady_clock::now();

	auto day_of_week = state.current_date.value % 7;
	if(day_of_week == 0) {
		// Update paths which are flagged to require an update once per week
		parallel_for_each_supply_route(state, [&](auto route) {
			auto fat_route = fatten(state.world, route);
			for(auto prov : fat_route.get_path()) {
				if(state.world.province_get_supply_route_requires_weekly_update(prov)) {
					fat_route.set_path_out_of_date(true);
					return; // Leave loop iteration 
				}
			}

		});
		state.world.execute_serial_over_province([&](auto prov_ids) {
			state.world.province_set_supply_route_requires_daily_update(prov_ids, ve::vbitfield_type{ 0 });
			state.world.province_set_supply_route_requires_weekly_update(prov_ids, ve::vbitfield_type{ 0 });
		});
	}

	static std::vector<dcon::army_supply_route_id> army_routes_to_update;
	static std::vector<dcon::navy_supply_route_id> navy_routes_to_update;
	static std::vector<dcon::land_construction_supply_route_id> land_construction_routes_to_update;
	static std::vector<dcon::naval_construction_supply_route_id> naval_construction_routes_to_update;
	army_routes_to_update.clear();
	navy_routes_to_update.clear();
	land_construction_routes_to_update.clear();
	naval_construction_routes_to_update.clear();

	// Update paths which is in need of an update. Accumulate them and then process in parallel
	for_each_supply_route_container(state, [&](auto route, auto& container) {
		if(path_needs_update(state, route)) {
			container.push_back(route);
		}

	}, army_routes_to_update, navy_routes_to_update, land_construction_routes_to_update, naval_construction_routes_to_update);

	concurrency::parallel_for_each(army_routes_to_update.begin(), army_routes_to_update.end(), [&](auto route) {
		update_supply_route_path(state, route);
	});
	concurrency::parallel_for_each(navy_routes_to_update.begin(), navy_routes_to_update.end(), [&](auto route) {
		update_supply_route_path(state, route);
	});
	concurrency::parallel_for_each(land_construction_routes_to_update.begin(), land_construction_routes_to_update.end(), [&](auto route) {
		update_supply_route_path(state, route);
	});
	concurrency::parallel_for_each(naval_construction_routes_to_update.begin(), naval_construction_routes_to_update.end(), [&](auto route) {
		update_supply_route_path(state, route);
	});
	end = std::chrono::steady_clock::now();
	state.console_log(std::string("STEP 4 time: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())));
	
	// STEP 5: Update route throughput and route attrition values.
	// Can be done in parallel
	begin = std::chrono::steady_clock::now();
	parallel_for_each_supply_route(state, [&](auto route) {
		dcon::nation_id controller = supply_route_get_owner(state, route);
		update_supply_route_throughput_attrition(state, route, controller);

	});

	end = std::chrono::steady_clock::now();
	state.console_log(std::string("STEP 5 time: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())));

	static tagged_vector<std::vector<pending_military_supply_route<dcon::army_id>>, dcon::nation_id> pending_army_routes;
	static tagged_vector<std::vector<pending_military_supply_route<dcon::navy_id>>, dcon::nation_id> pending_navy_routes;
	static tagged_vector<std::vector<pending_construction_supply_route<dcon::province_land_construction_id>>, dcon::nation_id> pending_land_construction_routes;
	static tagged_vector<std::vector<pending_construction_supply_route<dcon::province_naval_construction_id>>, dcon::nation_id> pending_naval_construction_routes;

	pending_army_routes.resize(state.world.nation_size());
	pending_navy_routes.resize(state.world.nation_size());
	pending_land_construction_routes.resize(state.world.nation_size());
	pending_naval_construction_routes.resize(state.world.nation_size());

	// STEP 6: Update the actual goods satisfaction for all units and constructions, when taking into account throughput and attrition for each nation
	// Units must be done in order of supply/reinforcement priority
	// Can be done parallel over each nation as for now, route origin are only allowed to come from your own controlled states and nowhere else
	begin = std::chrono::steady_clock::now();

	// Do unit supply&reinforcement first
	concurrency::parallel_for(uint32_t(0), state.world.nation_size(), [&](uint32_t i) {

		auto process_prioritized_unit_vector = [&](dcon::nation_id nation, std::span<const unit> units_to_process, const economy::supply_and_build_cost_union_commodity_amount_array& total_supply_required, economy::supply_and_build_cost_union_commodity_amount_array& expected_satisfaction_buffer) {
			const auto& commodity_ids = state.military_definitions.military_supply_build_goods;
			for(uint32_t j = 0; j < commodity_ids.size(); j++) {
				auto com_id = commodity_ids[j];
				float supply_required = total_supply_required[j];
				expected_satisfaction_buffer[j] = (supply_required == 0.0f ? 1.0f : std::min(state.world.nation_get_temp_total_stockpiles_buffer(nation, com_id) / supply_required, 1.0f));
			}
			for(auto unit : units_to_process) {
				if(unit.is_army) {
					auto army = unit.content.army;
					update_military_unit_routes_satisfaction(state, army, nation, consumption_buffer.army_supply_need[army], consumption_buffer.army_reinforcement_need[army], expected_satisfaction_buffer, stockpiles_buffer.army_closest_stockpiles[army], pending_army_routes[nation]);
				} else {
					auto navy = unit.content.navy;
					update_military_unit_routes_satisfaction(state, navy, nation, consumption_buffer.navy_supply_need[navy], consumption_buffer.navy_reinforcement_need[navy], expected_satisfaction_buffer, stockpiles_buffer.navy_closest_stockpiles[navy], pending_navy_routes[nation]);
				}
			}
		};


		dcon::nation_fat_id nation = fatten(state.world, dcon::nation_id{ dcon::nation_id::value_base_t(i) });
		pending_army_routes[nation].clear();
		pending_navy_routes[nation].clear();
		if(!nations::exists(state, nation)) {
			return;
		}
		const auto& commodity_ids = state.military_definitions.military_supply_build_goods;
		economy::supply_and_build_cost_union_commodity_amount_array expected_satisfaction(commodity_ids.size());
		// Process them in order, highest priority first
		process_prioritized_unit_vector(nation, high_prio_units[nation], consumption_buffer.nation_unit_high_prio_need[nation], expected_satisfaction);
		process_prioritized_unit_vector(nation, normal_prio_units[nation], consumption_buffer.nation_unit_normal_prio_need[nation], expected_satisfaction);
		process_prioritized_unit_vector(nation, low_prio_units[nation], consumption_buffer.nation_unit_low_prio_need[nation], expected_satisfaction);
	});
	// Then create the pending routes, so that the next parallel loop can know about these new routes
	state.world.for_each_nation([&](dcon::nation_id nation) {
		for(const auto& pending_route : pending_army_routes[nation]) {
			create_supply_route_from_pending(state, pending_route);
		}
		for(const auto& pending_route : pending_navy_routes[nation]) {
			create_supply_route_from_pending(state, pending_route);
		}
	});


	// Then constructions
	concurrency::parallel_for(uint32_t(0), state.world.nation_size(), [&](uint32_t i) {

		dcon::nation_fat_id nation = fatten(state.world, dcon::nation_id{ dcon::nation_id::value_base_t(i) });
		pending_army_routes[nation].clear();
		pending_navy_routes[nation].clear();
		pending_land_construction_routes[nation].clear();
		pending_naval_construction_routes[nation].clear();
		if(!nations::exists(state, nation)) {
			return;
		}

		const auto& build_goods_ids = state.military_definitions.military_build_goods;

		economy::build_cost_union_commodity_amount_array expected_satisfaction_buffer(build_goods_ids.size());


		for(uint32_t j = 0; j < build_goods_ids.size(); j++) {
			auto com_id = build_goods_ids[j];
			float supply_required = consumption_buffer.nation_construction_need[nation][j];
			expected_satisfaction_buffer[j] = (supply_required == 0.0f ? 1.0f : std::min(state.world.nation_get_temp_total_stockpiles_buffer(nation, com_id) / supply_required, 1.0f));
		}
		for(auto construction : state.world.nation_get_province_land_construction(nation)) {
			update_construction_routes_satisfaction(state, construction.id, nation, consumption_buffer.land_constructions_need[construction],expected_satisfaction_buffer, stockpiles_buffer.land_construction_closest_stockpiles[construction], pending_land_construction_routes[nation]);
		}
		for(auto construction : state.world.nation_get_province_naval_construction(nation)) {
			update_construction_routes_satisfaction(state, construction.id, nation, consumption_buffer.naval_constructions_need[construction], expected_satisfaction_buffer, stockpiles_buffer.naval_construction_closest_stockpiles[construction], pending_naval_construction_routes[nation]);
		}
		
	});
	// Then create the constructin pending routes
	state.world.for_each_nation([&](dcon::nation_id nation) {
		for(const auto& pending_route : pending_land_construction_routes[nation]) {
			create_supply_route_from_pending(state, pending_route);
		}
		for(const auto& pending_route : pending_naval_construction_routes[nation]) {
			create_supply_route_from_pending(state, pending_route);
		}
	});

	end = std::chrono::steady_clock::now();
	state.console_log(std::string("STEP 6 time: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())));
	begin = std::chrono::steady_clock::now();

	// STEP 7: Update the amount of days a route has been inactive, and delete routes which are deemed to be too inactive (has not moved any volume of goods for a certain amount of days)

	parallel_for_each_supply_route(state, [&](auto r) {
		auto route = fatten(state.world, r);
		uint8_t new_inactive_days = (route.get_is_active() ? uint8_t(0) : route.get_inactive_days() + uint8_t(1));
		route.set_inactive_days(new_inactive_days);
		
	});
	// Finally, delete unused routes serially. Iterate from the end to compact as we go
	for(uint32_t i = state.world.army_supply_route_size(); i-- > 0;) {
		dcon::army_supply_route_fat_id route = fatten(state.world, dcon::army_supply_route_id{ dcon::army_supply_route_id::value_base_t(i) });
		if(route.get_inactive_days() >= 5) {
			state.world.delete_army_supply_route(route);
		}
	}
	for(uint32_t i = state.world.navy_supply_route_size(); i-- > 0;) {
		dcon::navy_supply_route_fat_id route = fatten(state.world, dcon::navy_supply_route_id{ dcon::navy_supply_route_id::value_base_t(i) });
		if(route.get_inactive_days() >= 5) {
			state.world.delete_navy_supply_route(route);
		}
	}
	for(uint32_t i = state.world.land_construction_supply_route_size(); i-- > 0;) {
		dcon::land_construction_supply_route_fat_id route = fatten(state.world, dcon::land_construction_supply_route_id{ dcon::land_construction_supply_route_id::value_base_t(i) });
		if(route.get_inactive_days() >= 5) {
			state.world.delete_land_construction_supply_route(route);
		}
	}
	for(uint32_t i = state.world.naval_construction_supply_route_size(); i-- > 0;) {
		dcon::naval_construction_supply_route_fat_id route = fatten(state.world, dcon::naval_construction_supply_route_id{ dcon::naval_construction_supply_route_id::value_base_t(i) });
		if(route.get_inactive_days() >= 5) {
			state.world.delete_naval_construction_supply_route(route);
		}
	}
		


	
	end = std::chrono::steady_clock::now();
	state.console_log(std::string("STEP 7 time: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())));
	begin = std::chrono::steady_clock::now();


	// STEP 8: subtract from stockpiles the actual buffered amount which each route has taken
	// Can be done in parallel over each nation
	concurrency::parallel_for(uint32_t(0), state.world.market_size(), [&](uint32_t i) {
		dcon::market_id market{ dcon::market_id::value_base_t(i) };
		state.world.for_each_commodity([&](dcon::commodity_id commodity_id) {
			auto to_apply = state.world.market_get_govt_stockpile_satisfaction_buffer(market, commodity_id);
			// Consume the goods. We know that this amount is exactly the amount left after consumption, and it should not be less than zero
			// We dont use the usual "set_government_stockpile" function as we have kept track of both national and local stockpile changes and can apply both
			assert(to_apply >= 0.0f);
			state.world.market_set_government_stockpile(market, commodity_id, to_apply);
		});

	});
	concurrency::parallel_for(uint32_t(0), state.world.nation_size(), [&](uint32_t i) {
		dcon::nation_id nation{ dcon::nation_id::value_base_t(i) };
		if(!nations::exists(state, nation)) {
			return;
		}

		state.world.for_each_commodity([&](dcon::commodity_id commodity_id) {
			auto to_apply = state.world.nation_get_temp_total_stockpiles_buffer(nation, commodity_id);
			// Consume the goods. We know that this amount is exactly the amount left after consumption, and it should not be less than zero
			assert(to_apply >= 0.0f);
			state.world.nation_set_total_stockpiles(nation, commodity_id, to_apply);
		});

	});
	end = std::chrono::steady_clock::now();
	state.console_log(std::string("STEP 8 time: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())));

	// STEP 9: Update each army/navy supply & reinforcement satisfaction and advance constructions, by computing how much of their required commodities they were able to receive from all supply routes

	// Start processing each army/navy and applying reinforcement/supply satisfaction
	// Do armies
	begin = std::chrono::steady_clock::now();
	concurrency::parallel_for(uint32_t(0), state.world.army_size(), [&](uint32_t i) {
		dcon::army_id army{ dcon::army_id::value_base_t(i) };
		if(!state.world.army_is_valid(army)) {
			return;
		}
		update_unit_commodity_satisfaction(state, army);
	});
	// Then navies
	concurrency::parallel_for(uint32_t(0), state.world.navy_size(), [&](uint32_t i) {
		dcon::navy_id navy{ dcon::navy_id::value_base_t(i) };
		if(!state.world.navy_is_valid(navy)) {
			return;
		}
		update_unit_commodity_satisfaction(state, navy);
	});

	// Then land constructions
	concurrency::parallel_for(uint32_t(0), state.world.province_land_construction_size(), [&](uint32_t i) {
		auto construction = fatten(state.world, dcon::province_land_construction_id{ dcon::province_land_construction_id::value_base_t(i) });
		if(!construction.is_valid()) {
			return;
		}
		update_construction_commodity_satisfaction(state, construction.id);
	});
	// Then naval constructions
	concurrency::parallel_for(uint32_t(0), state.world.province_naval_construction_size(), [&](uint32_t i) {
		auto construction = fatten(state.world, dcon::province_naval_construction_id{ dcon::province_naval_construction_id::value_base_t(i) });
		if(!construction.is_valid()) {
			return;
		}
		update_construction_commodity_satisfaction(state, construction.id);

	});
	end = std::chrono::steady_clock::now();
	state.console_log(std::string("STEP 9 time: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())));


	// STEP 10: Initialize used supply throughput values, to be used on the next day.
	begin = std::chrono::steady_clock::now();

	state.world.execute_serial_over_province([&](auto province_ids) {
		state.world.province_set_used_supply_throughput(province_ids, 0.0f);
	});
		
	concurrency::combinable<tagged_vector<int64_t, dcon::province_id>> used_supply_throughput_changes([&]() {
		return tagged_vector<int64_t, dcon::province_id>(state.world.province_size());
	});

	parallel_for_each_supply_route(state, [&](auto route) {
		auto fat_route = fatten(state.world, route);
		add_used_supply_throughput_to_buffer_from_path(state, fat_route.get_path(), fat_route.get_volume(), used_supply_throughput_changes.local());
	});
	// Combine the changes in province supply throughput computed in parallel
	static tagged_vector<int64_t, dcon::province_id> final_province_supply_throughput;
	final_province_supply_throughput.resize(state.world.province_size());
	std::fill(final_province_supply_throughput.begin(), final_province_supply_throughput.end(), 0);
	used_supply_throughput_changes.combine_each([&](const tagged_vector<int64_t, dcon::province_id>& local) {
		for(auto prov : state.world.in_province) {
			final_province_supply_throughput[prov] += local[prov];
		}
	});

	for(auto prov : state.world.in_province) {
		prov.set_used_supply_throughput(std::max(prov.get_used_supply_throughput() + float(final_province_supply_throughput[prov]) / used_throughput_integer_precision, 0.0f));
	}
	end = std::chrono::steady_clock::now();
	state.console_log(std::string("STEP 10 time: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())));


}




}
