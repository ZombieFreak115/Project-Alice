#include "concept_declarations.hpp"
#include "system_state.hpp"
#include "military.hpp"
#include "economy.hpp"
#include "province.hpp"
#include "military_templates.hpp"
#include "construction.hpp"
#include "supply_route_templates.hpp"
#include "supply_route.hpp"
#include "economy_templates.hpp"
#include "nations_templates.hpp"

namespace supply_routes {


// union of either a army or navy. Used for type erasure when prioritizing supplies between military units
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



// local functions for accessing resuable buffers, as a way of aliasing them
template<concepts::any_dcon_id_type<dcon::market_id> market_type>
auto local_stockpile_available_goods_get(const sys::state& state, market_type market, dcon::commodity_id com_id) {
	return state.world.market_get_commodity_float_buffer_1(market, com_id);
}

template<concepts::any_dcon_id_type<dcon::market_id> market_type, concepts::normal_or_vector_value_type<float> float_type>
void local_stockpile_available_goods_set(sys::state& state, market_type market, dcon::commodity_id com_id, float_type val) {
	state.world.market_set_commodity_float_buffer_1(market, com_id, val);
}

template<concepts::any_dcon_id_type<dcon::nation_id> nation_type>
auto nation_stockpile_available_goods_get(const sys::state& state, nation_type nation, dcon::commodity_id com_id) {
	return state.world.nation_get_commodity_float_buffer_1(nation, com_id);
}

template<concepts::any_dcon_id_type<dcon::nation_id> nation_type, concepts::normal_or_vector_value_type<float> float_type>
void nation_stockpile_available_goods_set(sys::state& state, nation_type nation, dcon::commodity_id com_id, float_type val) {
	state.world.nation_set_commodity_float_buffer_1(nation, com_id, val);
}


template<typename unit_id_type>
auto unit_supply_need_get(const sys::state& state, unit_id_type unit, dcon::unit_supply_commodity_id com_id) {
	if constexpr(concepts::any_dcon_id_type<unit_id_type, dcon::army_id>) {
		return state.world.army_get_unit_supply_commodity_float_buffer_1(unit, com_id);
	}
	else if constexpr(concepts::any_dcon_id_type<unit_id_type, dcon::navy_id>) {
		return state.world.navy_get_unit_supply_commodity_float_buffer_1(unit, com_id);
	}
	else {
		static_assert(false, "Unsupported type");
	}
}

template<typename unit_id_type, concepts::normal_or_vector_value_type<float> float_type>
void unit_supply_need_set(sys::state& state, unit_id_type unit, dcon::unit_supply_commodity_id com_id, float_type val) {
	if constexpr(concepts::any_dcon_id_type<unit_id_type, dcon::army_id>) {
		state.world.army_set_unit_supply_commodity_float_buffer_1(unit, com_id, val);
	}
	else if constexpr(concepts::any_dcon_id_type<unit_id_type, dcon::navy_id>) {
		state.world.navy_set_unit_supply_commodity_float_buffer_1(unit, com_id, val);
	}
	else {
		static_assert(false, "Unsupported type");
	}
}


template<typename unit_id_type>
auto unit_reinforcement_need_get(const sys::state& state, unit_id_type unit, dcon::unit_build_commodity_id com_id) {
	if constexpr(concepts::any_dcon_id_type<unit_id_type, dcon::army_id>) {
		return state.world.army_get_unit_build_commodity_float_buffer_1(unit, com_id);
	} else if constexpr(concepts::any_dcon_id_type<unit_id_type, dcon::navy_id>) {
		return state.world.navy_get_unit_build_commodity_float_buffer_1(unit, com_id);
	} else {
		static_assert(false, "Unsupported type");
	}
}

template<typename unit_id_type, concepts::normal_or_vector_value_type<float> float_type>
void unit_reinforcement_need_set(sys::state& state, unit_id_type unit, dcon::unit_build_commodity_id com_id, float_type val) {
	if constexpr(concepts::any_dcon_id_type<unit_id_type, dcon::army_id>) {
		state.world.army_set_unit_build_commodity_float_buffer_1(unit, com_id, val);
	} else if constexpr(concepts::any_dcon_id_type<unit_id_type, dcon::navy_id>) {
		state.world.navy_set_unit_build_commodity_float_buffer_1(unit, com_id, val);
	} else {
		static_assert(false, "Unsupported type");
	}
}

template<military::unit_priority priority, concepts::any_dcon_id_type<dcon::nation_id> nation_id_type>
auto nation_unit_prio_need_get(const sys::state& state, nation_id_type n, dcon::unit_supply_and_build_commodity_id com_id) {
	if constexpr(priority == military::unit_priority::low_priority) {
		return state.world.nation_get_unit_supply_and_build_commodity_float_buffer_1(n, com_id);
	}
	else if constexpr(priority == military::unit_priority::normal_priority) {
		return state.world.nation_get_unit_supply_and_build_commodity_float_buffer_2(n, com_id);
	}
	else if constexpr(priority == military::unit_priority::high_priority) {
		return state.world.nation_get_unit_supply_and_build_commodity_float_buffer_3(n, com_id);
	}
}

template<military::unit_priority priority, concepts::any_dcon_id_type<dcon::nation_id> nation_id_type, concepts::normal_or_vector_value_type<float> float_type>
void nation_unit_prio_need_set(sys::state& state, nation_id_type n, dcon::unit_supply_and_build_commodity_id com_id, float_type val) {
	if constexpr(priority == military::unit_priority::low_priority) {
		state.world.nation_set_unit_supply_and_build_commodity_float_buffer_1(n, com_id, val);
	}
	else if constexpr(priority == military::unit_priority::normal_priority) {
		state.world.nation_set_unit_supply_and_build_commodity_float_buffer_2(n, com_id, val);
	}
	else if constexpr(priority == military::unit_priority::high_priority) {
		state.world.nation_set_unit_supply_and_build_commodity_float_buffer_3(n, com_id, val);
	}
}
template<concepts::military_construction_type construction_type>
const economy::commodity_amounts& constructions_need_get(const sys::state& state, construction_type con) {
	if constexpr(std::is_same_v<construction_type, dcon::province_land_construction_id>) {
		return state.world.province_land_construction_get_commodity_amounts_buffer_1(con);
	}
	else if constexpr(std::is_same_v<construction_type, dcon::province_naval_construction_id>) {
		return state.world.province_naval_construction_get_commodity_amounts_buffer_1(con);
	}
}

template<concepts::military_construction_type construction_type>
economy::commodity_amounts& constructions_need_get(sys::state& state, construction_type con) {
	return const_cast<economy::commodity_amounts&>(constructions_need_get(static_cast<const sys::state&>(state), con));
}


template<concepts::any_dcon_id_type<dcon::nation_id> nation_id_type>
auto nation_construction_need_get(const sys::state& state, nation_id_type n, dcon::unit_build_commodity_id com_id) {
	return state.world.nation_get_unit_build_commodity_float_buffer_1(n, com_id);
}

template<concepts::any_dcon_id_type<dcon::nation_id> nation_id_type, concepts::normal_or_vector_value_type<float> float_type>
void nation_construction_need_set(sys::state& state, nation_id_type n, dcon::unit_build_commodity_id com_id, float_type val) {
	state.world.nation_set_unit_build_commodity_float_buffer_1(n, com_id, val);
}


template<concepts::any_dcon_id_type<dcon::nation_id> nation_id_type>
auto nation_unit_expected_satisfaction_get(const sys::state& state, nation_id_type n, dcon::unit_supply_and_build_commodity_id com_id) {
	return state.world.nation_get_unit_supply_and_build_commodity_float_buffer_4(n, com_id);
}

template<concepts::any_dcon_id_type<dcon::nation_id> nation_id_type, concepts::normal_or_vector_value_type<float> float_type>
void nation_unit_expected_satisfaction_set(sys::state& state, nation_id_type n, dcon::unit_supply_and_build_commodity_id com_id, float_type val) {
	state.world.nation_set_unit_supply_and_build_commodity_float_buffer_4(n, com_id, val);
}


template<concepts::any_dcon_id_type<dcon::nation_id> nation_id_type>
auto nation_construction_expected_satisfaction_get(const sys::state& state, nation_id_type n, dcon::unit_build_commodity_id com_id) {
	return state.world.nation_get_unit_build_commodity_float_buffer_2(n, com_id);
}

template<concepts::any_dcon_id_type<dcon::nation_id> nation_id_type, concepts::normal_or_vector_value_type<float> float_type>
void nation_construction_expected_satisfaction_set(sys::state& state, nation_id_type n, dcon::unit_build_commodity_id com_id, float_type val) {
	state.world.nation_set_unit_build_commodity_float_buffer_2(n, com_id, val);
}

template<concepts::military_unit unit_id_type>
auto unit_best_stockpiles_get(const sys::state& state, unit_id_type unit) {
	if constexpr(std::is_same_v<unit_id_type, dcon::army_id>) {
		return state.world.army_get_state_instance_vector_buffer_1(unit);
	}
	if constexpr(std::is_same_v<unit_id_type, dcon::navy_id>) {
		return state.world.navy_get_state_instance_vector_buffer_1(unit);
	}
}
template<concepts::military_unit unit_id_type>
auto unit_best_stockpiles_get(sys::state& state, unit_id_type unit) {
	if constexpr(std::is_same_v<unit_id_type, dcon::army_id>) {
		return state.world.army_get_state_instance_vector_buffer_1(unit);
	}
	if constexpr(std::is_same_v<unit_id_type, dcon::navy_id>) {
		return state.world.navy_get_state_instance_vector_buffer_1(unit);
	}
}

template<concepts::military_construction_type construction_type>
auto construction_best_stockpiles_get(const sys::state& state, construction_type con) {
	if constexpr(std::is_same_v<construction_type, dcon::province_land_construction_id>) {
		return state.world.province_land_construction_get_state_instance_vector_buffer_1(con);
	}
	if constexpr(std::is_same_v<construction_type, dcon::province_naval_construction_id>) {
		return state.world.province_naval_construction_get_state_instance_vector_buffer_1(con);
	}
}
template<concepts::military_construction_type construction_type>
auto construction_best_stockpiles_get(sys::state& state, construction_type con) {
	if constexpr(std::is_same_v<construction_type, dcon::province_land_construction_id>) {
		return state.world.province_land_construction_get_state_instance_vector_buffer_1(con);
	}
	if constexpr(std::is_same_v<construction_type, dcon::province_naval_construction_id>) {
		return state.world.province_naval_construction_get_state_instance_vector_buffer_1(con);
	}
}




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

template<concepts::unit_supply_or_build_commodity_type commodity_type, concepts::military_supply_route_type route_type>
float military_route_get_buffered_goods(const sys::state& state, route_type route, commodity_type commodity_id) {
	auto fat_route = fatten(state.world, route);
	if constexpr(std::is_same_v<commodity_type, dcon::unit_supply_commodity_id>) {
		return fat_route.get_buffered_supply_goods(commodity_id);
	}
	else if constexpr(std::is_same_v<commodity_type, dcon::unit_build_commodity_id>) {
		return fat_route.get_buffered_reinforcement_goods(commodity_id);
	}
	
}
template float military_route_get_buffered_goods(const sys::state& state, dcon::army_supply_route_id route, dcon::unit_supply_commodity_id sup_id);
template float military_route_get_buffered_goods(const sys::state& state, dcon::navy_supply_route_id route, dcon::unit_supply_commodity_id sup_id);
template float military_route_get_buffered_goods(const sys::state& state, dcon::army_supply_route_id route, dcon::unit_build_commodity_id sup_id);
template float military_route_get_buffered_goods(const sys::state& state, dcon::navy_supply_route_id route, dcon::unit_build_commodity_id sup_id);

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

float get_enemy_blockade_power(const sys::state& state, dcon::province_id prov, dcon::nation_id nation_as) {
	assert(province::is_sea(state, prov) && !province::province_is_deep_waters(state, prov));
	// TODO: actually compute blockade power
	return military::navy_strength_present<military::battle_allowed::yes, military::retreat_allowed::no, military::participants_included::enemies>(state, prov, nation_as) * 1.0f;
}

float get_enemy_convoy_raiding_power(const sys::state& state, dcon::province_id prov, dcon::nation_id nation_as) {
	assert(province::is_sea(state, prov));
	// TODO: actually compute blockade power
	return military::navy_strength_present<military::battle_allowed::yes, military::retreat_allowed::no, military::participants_included::enemies>(state, prov, nation_as) * 1.0f;
}

float compute_efficiency(float consumed, float available) {
	if(consumed == 0.0f) {
		if(available == 0.0f) {
			return 0.0f;
		} else {
			return 1.0f;
		}
	} else {
		return std::min(available / consumed, 1.0f);
	}
}


float naval_supply_speed(const sys::state& state, dcon::nation_id nation_as) {
	return std::max(state.world.nation_get_modifier_values(nation_as, sys::national_mod_offsets::naval_supply_speed_add) * state.world.nation_get_modifier_values(nation_as, sys::national_mod_offsets::naval_supply_speed_mul) * (state.world.nation_get_modifier_values(nation_as, sys::national_mod_offsets::naval_supply_speed_percent) + 1.0f), 0.0f);
}


float land_supply_speed(const sys::state& state, dcon::nation_id nation_as) {
	return std::max(state.world.nation_get_modifier_values(nation_as, sys::national_mod_offsets::land_supply_speed_add) * state.world.nation_get_modifier_values(nation_as, sys::national_mod_offsets::land_supply_speed_mul) * (state.world.nation_get_modifier_values(nation_as, sys::national_mod_offsets::land_supply_speed_percent) + 1.0f), 0.0f);
}

float port_supply_capacity_mult_blockaded_modifier(const sys::state& state, dcon::province_id port_prov, dcon::nation_id nation_as) {
	assert(province::is_port(state, port_prov));
	auto port_to_prov = state.world.province_get_port_to(port_prov);
	auto enemy_blockade_power = get_enemy_blockade_power(state, port_to_prov, nation_as);
	return navy_port_supply_capacity_blockade_threshold > 0.0f ? std::max((navy_port_supply_capacity_blockade_threshold - enemy_blockade_power) / navy_port_supply_capacity_blockade_threshold, 0.f) : 1.0f;
}
float port_supply_capacity_mult_supply_access_modifier(const sys::state& state, dcon::province_id port_prov, dcon::nation_id nation_as) {
	assert(province::is_port(state, port_prov));
	bool has_access = province::has_supply_access_to_province(state, nation_as, port_prov);
	return has_access ? 1.0f : 0.0f;
}


float port_supply_capacity_in_province(const sys::state& state, dcon::province_id port_prov, dcon::nation_id nation_as) {
	assert(province::is_port(state, port_prov));
	float access_mult = port_supply_capacity_mult_supply_access_modifier(state, port_prov, nation_as);
	if(access_mult == 0.0f) {
		return 0.0f;
	}
	float capacity_add = state.world.province_get_modifier_values(port_prov, sys::provincial_mod_offsets::port_supply_capacity_add) + state.world.nation_get_modifier_values(nation_as, sys::national_mod_offsets::national_port_supply_capacity_add);
	float capacity_percent = state.world.province_get_modifier_values(port_prov, sys::provincial_mod_offsets::port_supply_capacity_percent) + state.world.nation_get_modifier_values(nation_as, sys::national_mod_offsets::national_port_supply_capacity_percent) + 1.0f;
	float capacity_mul = state.world.province_get_modifier_values(port_prov, sys::provincial_mod_offsets::port_supply_capacity_mul) * state.world.nation_get_modifier_values(nation_as, sys::national_mod_offsets::national_port_supply_capacity_mul) * port_supply_capacity_mult_blockaded_modifier(state, port_prov, nation_as) * access_mult;
	return std::max(capacity_add * capacity_percent * capacity_mul, 0.0f);
}


float port_supply_capacity_efficiency(const sys::state& state, dcon::province_id port_prov, dcon::nation_id nation_as) {
	float used_capacity = state.world.province_get_used_port_supply_capacity(port_prov);
	float available_capacity = port_supply_capacity_in_province(state, port_prov, nation_as);
	return compute_efficiency(used_capacity, available_capacity);
}


float supply_throughput_mult_access_modifier(const sys::state& state, dcon::province_id province, dcon::nation_id nation_as) {
	bool has_access = province::has_supply_access_to_province(state, nation_as, province);
	return has_access ? 1.0f : 0.0f;
}

float supply_throughput_mult_hostile_troops_modifier(const sys::state& state, dcon::province_id prov, dcon::nation_id nation_as) {
	assert(prov);
	assert(nation_as);

	if(province::is_sea(state, prov)) {
		return 1.0f; // Cannot have hostile troops in sea provinces. Blockades of ports are handled with a malus to port supply capacity, convoy raiding is handled as supply attrition
	} else {
		float enemy_strength_present = military::army_strength_present<military::battle_allowed::yes, military::retreat_allowed::no, military::participants_included::enemies>(state, prov, nation_as);
		return army_supply_throughput_blockade_threshold > 0.0f ? std::max((army_supply_throughput_blockade_threshold - enemy_strength_present) / army_supply_throughput_blockade_threshold, 0.f) : 1.0f;
	}
}


float supply_throughput_in_province(const sys::state& state, dcon::province_id province, dcon::nation_id nation_as) {

	float access_mul_mod = supply_throughput_mult_access_modifier(state, province, nation_as);
	if(access_mul_mod == 0.0f) {
		return 0.0f;
	}
	bool is_sea = province::is_sea(state, province);
	auto nation_add_mod = (is_sea ? state.world.nation_get_modifier_values(nation_as, sys::national_mod_offsets::national_naval_supply_throughput_add) : state.world.nation_get_modifier_values(nation_as, sys::national_mod_offsets::national_land_supply_throughput_add));
	auto nation_percent_mod = (is_sea ? state.world.nation_get_modifier_values(nation_as, sys::national_mod_offsets::national_naval_supply_throughput_percent) : state.world.nation_get_modifier_values(nation_as, sys::national_mod_offsets::national_land_supply_throughput_percent));
	auto nation_mul_mod = (is_sea ? state.world.nation_get_modifier_values(nation_as, sys::national_mod_offsets::national_naval_supply_throughput_mul) : state.world.nation_get_modifier_values(nation_as, sys::national_mod_offsets::national_land_supply_throughput_mul));
	float add_modifiers = state.world.province_get_modifier_values(province, sys::provincial_mod_offsets::supply_throughput_add) + nation_add_mod;
	float percent_modifiers = state.world.province_get_modifier_values(province, sys::provincial_mod_offsets::supply_throughput_percent) + nation_percent_mod + 1.0f;
	float mult_modifiers = access_mul_mod * supply_throughput_mult_hostile_troops_modifier(state, province, nation_as) * state.world.province_get_modifier_values(province, sys::provincial_mod_offsets::supply_throughput_mul) * nation_mul_mod;
	return std::max(add_modifiers * percent_modifiers * mult_modifiers, 0.0f);
}

float supply_throughput_efficiency(const sys::state& state, dcon::province_id prov, dcon::nation_id nation_as) {
	assert(prov);
	assert(nation_as);
	float used_supply_throughput = state.world.province_get_used_supply_throughput(prov);
	float throughput = supply_throughput_in_province(state, prov, nation_as);
	return compute_efficiency(used_supply_throughput, throughput);
}


float supply_loss_add_convoy_raiding(const sys::state& state, dcon::province_id province, dcon::nation_id nation_as) {
	assert(province::is_sea(state, province));
	return get_enemy_convoy_raiding_power(state, province, nation_as) * convoy_raiding_supply_loss;
}
float supply_loss_add_hostile_armies(const sys::state& state, dcon::province_id province, dcon::nation_id nation_as) {
	assert(province::is_land(state, province));
	return military::army_strength_present<military::battle_allowed::yes, military::retreat_allowed::no, military::participants_included::enemies>(state, province, nation_as)* hostile_army_supply_loss;
}

float supply_loss_in_province(const sys::state& state, dcon::province_id province, dcon::nation_id nation_as) {
	bool province_is_sea = province::is_sea(state, province);
	float national_add_mod = (province_is_sea ? state.world.nation_get_modifier_values(nation_as, sys::national_mod_offsets::national_naval_supply_loss_add) : state.world.nation_get_modifier_values(nation_as, sys::national_mod_offsets::national_land_supply_loss_add));
	float national_percent_mod = (province_is_sea ? state.world.nation_get_modifier_values(nation_as, sys::national_mod_offsets::national_naval_supply_loss_percent) : state.world.nation_get_modifier_values(nation_as, sys::national_mod_offsets::national_land_supply_loss_percent));
	float national_mul_mod = (province_is_sea ? state.world.nation_get_modifier_values(nation_as, sys::national_mod_offsets::national_naval_supply_loss_mul) : state.world.nation_get_modifier_values(nation_as, sys::national_mod_offsets::national_land_supply_loss_mul));

	float hostile_units_add = (province_is_sea ? supply_loss_add_convoy_raiding(state, province, nation_as) : supply_loss_add_hostile_armies(state, province, nation_as));
	float add_mods = state.world.province_get_modifier_values(province, sys::provincial_mod_offsets::supply_loss_add) + national_add_mod + hostile_units_add;
	float percent_mods = state.world.province_get_modifier_values(province, sys::provincial_mod_offsets::supply_loss_percent) + national_percent_mod + 1.0f;
	float mul_mods = state.world.province_get_modifier_values(province, sys::provincial_mod_offsets::supply_loss_mul) * national_mul_mod;
	return std::max(add_mods * percent_mods * mul_mods, 0.0f);
}

float adjacency_avg_supply_loss(const sys::state& state, dcon::province_id prov_1, dcon::province_id prov_2, dcon::nation_id nation_as) {
	assert(state.world.get_province_adjacency_by_province_pair(prov_1, prov_2));
	auto avg_supply_attr = (supply_loss_in_province(state, prov_1, nation_as) + supply_loss_in_province(state, prov_2, nation_as)) / 2.0f;
	return avg_supply_attr;
}
float adjacency_avg_supply_loss(const sys::state& state, dcon::province_adjacency_id province_adj, dcon::nation_id nation_as) {
	auto prov_1 = state.world.province_adjacency_get_connected_provinces(province_adj, 0);
	auto prov_2 = state.world.province_adjacency_get_connected_provinces(province_adj, 1);
	return adjacency_avg_supply_loss(state, prov_1, prov_2, nation_as);
}

float adjacency_net_supply_loss(const sys::state& state, dcon::province_adjacency_id province_adj, dcon::nation_id nation_as) {
	auto prov_1 = state.world.province_adjacency_get_connected_provinces(province_adj, 0);
	auto prov_2 = state.world.province_adjacency_get_connected_provinces(province_adj, 1);
	auto avg_supply_loss_per_km = adjacency_avg_supply_loss(state, province_adj, nation_as) / state.map_state.map_data.world_circumference; // Get supply loss measured in loss per km
	auto distance = state.world.province_adjacency_get_distance_km(province_adj) * military::get_avg_movement_cost_modifier(state, nation_as, prov_1, prov_2);
	assert(std::isfinite(distance * avg_supply_loss_per_km));
	return distance * avg_supply_loss_per_km;
}

bool supply_route_path_available(const sys::state& state, std::span<const dcon::province_id> path, dcon::province_id origin_prov, dcon::province_id destination) {
	// If destination and origin are diffrent, and path size is 0, that means no path is available.
	return !(destination != origin_prov && path.size() == 0);
}
template<concepts::supply_route_type route_type>
bool supply_route_path_available(const sys::state& state, route_type route) {
	dcon::province_id dest = supply_route_get_destination(route);
	dcon::province_id origin_prov = supply_route_get_origin(route);
	auto fat_route = fatten(state.world, route);
	// If destination and origin are diffrent, and path size is 0, that means no path is available.
	return supply_route_path_available(state, fat_route.get_path(), origin_prov, dest);
}

float calculate_supply_route_throughput(const sys::state& state, std::span<const dcon::province_id> path, dcon::province_id origin_prov, dcon::province_id destination, dcon::nation_id controller) {
	assert(origin_prov);
	assert(destination);
	if(!supply_route_path_available(state, path, origin_prov, destination)) {
		return 0.0f;
	}
	float smallest_supply_throughput = 1.0f;
	for(uint32_t i = 0; i < path.size(); i++) {
		auto prov = path[i];
		// The next prov after this one is either the next one in the path, or if the path has no more items, its the destination. Destination is not directly part of the path
		// A unit/construction shall be affected if the port it is standing on is occupied, but it should not be affected if the province it is standing on is occupied (as long as there are adjacent friendly provinces for supply to pass through). Otherwise you would no tbe able to get any supplies when fighting a battle just across your border
		auto next_prov = (i + 1 < path.size() ? path[i + 1] : destination);
		assert(province::provinces_are_adjacent(state, prov, next_prov));
		float province_throughput = supply_throughput_efficiency(state, prov, controller);
		smallest_supply_throughput = std::min(smallest_supply_throughput, province_throughput);
		//limit throughput to the port capacity efficiency, if this or the next province are a port and are connected
		if(province::is_port_connected_to(state, prov, next_prov)) {
			smallest_supply_throughput = std::min(smallest_supply_throughput, port_supply_capacity_efficiency(state,prov, controller));
		}
		else if(province::is_port_connected_to(state, next_prov, prov)) {
			smallest_supply_throughput = std::min(smallest_supply_throughput, port_supply_capacity_efficiency(state, next_prov, controller));
		}
		
	}
	return smallest_supply_throughput;
}

float calculate_supply_route_supply_loss(const sys::state& state, std::span<const dcon::province_id> path, dcon::province_id origin_prov, dcon::province_id destination, dcon::nation_id controller) {
	assert(origin_prov);
	assert(destination);
	if(!supply_route_path_available(state, path, origin_prov, destination)) {
		return 1.0f;
	}
	float total_attrition_mod = 0.0f;
	for(uint32_t i = 0; i < path.size(); i++) {
		auto prov = path[i];
		auto next_prov = (i + 1 < path.size() ? path[i + 1] : destination);
		auto adj = state.world.get_province_adjacency_by_province_pair(prov, next_prov);
		assert(adj);
		total_attrition_mod += adjacency_net_supply_loss(state, adj, controller);
		assert(std::isfinite(total_attrition_mod));
		
	}
	return std::max(1.0f - total_attrition_mod, max_supply_route_loss);
}


void add_used_supply_throughput_and_used_port_capacity(sys::state& state, std::span<const dcon::province_id> path,  dcon::province_id destination_prov, float volume) {
	for(uint32_t i = 0; i < path.size(); i++) {
		dcon::province_id prov = path[i];
		dcon::province_id next_prov = (i + 1 < path.size() ? path[i + 1] : destination_prov);
		state.world.province_set_used_supply_throughput(prov, state.world.province_get_used_supply_throughput(prov) + volume);
		if(province::is_port_connected_to(state, prov, next_prov)) {
			state.world.province_set_used_port_supply_capacity(prov, state.world.province_get_used_port_supply_capacity(prov) + volume);
		} else if(province::is_port_connected_to(state, next_prov, prov)) {
			state.world.province_set_used_port_supply_capacity(next_prov, state.world.province_get_used_port_supply_capacity(next_prov) + volume);
		}

	}
}


template<concepts::supply_route_type route_type>
void update_supply_route_throughput_attrition(sys::state& state, route_type r, dcon::nation_id controller) {

	auto route = fatten(state.world, r);
	auto origin_prov = supply_route_get_origin(state, r);
	auto route_dest = supply_route_get_destination(state, r);
	auto path = route.get_path();
	auto path_span = std::span<const dcon::province_id>(path.begin(), path.end());
	auto supply_loss = calculate_supply_route_supply_loss(state, path_span, origin_prov, route_dest, controller);
	auto throughput = calculate_supply_route_throughput(state, path_span, origin_prov, route_dest, controller);
	route.set_supply_loss(supply_loss);
	route.set_throughput(throughput);
}

// Updates the path for a supply route. Does NOT update the route throughput or route supply loss
template<concepts::supply_route_type route_type>
void update_supply_route_path(sys::state& state, float expected_volume, route_type r) {

	auto route = fatten(state.world, r);
	auto market = route.get_origin();
	auto state_inst = market.get_zone_from_local_market();
	dcon::nation_id route_owner = supply_routes::supply_route_get_owner(state, r);
	dcon::province_id destination_location = supply_routes::supply_route_get_destination(state, r);
	static thread_local std::vector<dcon::province_id> path;
	path.clear();
	province::make_military_supply_path(state, state_inst, destination_location, route_owner, expected_volume, path);
	auto existing_path = route.get_path();
	existing_path.clear();
	existing_path.load_range(path.data(), path.data() + path.size());
	route.set_path_out_of_date(false);
}

template<concepts::supply_route_type route_type>
void delete_supply_route(sys::state& state, route_type route) {
	if constexpr(std::is_same_v<route_type, dcon::army_supply_route_id>) {
		state.world.delete_army_supply_route(route);
	}
	else if constexpr(std::is_same_v<route_type, dcon::navy_supply_route_id>) {
		state.world.delete_navy_supply_route(route);
	}
	else if constexpr(std::is_same_v<route_type, dcon::land_construction_supply_route_id>) {
		state.world.delete_land_construction_supply_route(route);
	}
	else if constexpr(std::is_same_v<route_type, dcon::naval_construction_supply_route_id>) {
		state.world.delete_naval_construction_supply_route(route);
	}
	else {
		static_assert(false, "Unsupported route type");
	}
}
template<concepts::military_unit unit_type>
void create_supply_route_path(const sys::state& state, unit_type unit, dcon::market_id origin, float expected_volume, std::vector<dcon::province_id>& path_out) {
	auto state_inst = state.world.market_get_zone_from_local_market(origin);
	auto capital = state.world.state_instance_get_capital(state_inst);
	dcon::province_id unit_loc = military::unit_get_location(state, unit);
	dcon::nation_id controller = military::unit_get_controller(state, unit);
	province::make_military_supply_path(state, state_inst, unit_loc, controller, expected_volume, path_out);
}
template<concepts::military_construction_type construction_type>
void create_supply_route_path(const sys::state& state, construction_type c, dcon::market_id origin, float expected_volume, std::vector<dcon::province_id>& path_out) {
	auto state_inst = state.world.market_get_zone_from_local_market(origin);
	auto capital = state.world.state_instance_get_capital(state_inst);
	dcon::province_id con_loc = economy::construction_get_location(state, c);
	dcon::nation_id controller = economy::construction_get_controller(state, c);
	province::make_military_supply_path(state, state_inst, con_loc, controller, expected_volume, path_out);
}


dcon::army_supply_route_id create_supply_route(sys::state& state, dcon::army_id unit, dcon::market_id origin, const std::vector<dcon::province_id>& path) {
	auto id = fatten(state.world, state.world.force_create_army_supply_route(unit, origin));
	id.get_path().load_range(path.data(), path.data() + path.size());
	id.set_is_active(false);
	id.set_inactive_days(0);
	id.set_path_out_of_date(false);
	return id.id;
}

dcon::navy_supply_route_id create_supply_route(sys::state& state, dcon::navy_id unit, dcon::market_id origin, const std::vector<dcon::province_id>& path) {
	auto id = fatten(state.world, state.world.force_create_navy_supply_route(unit, origin));
	id.get_path().load_range(path.data(), path.data() + path.size());
	id.set_is_active(false);
	id.set_inactive_days(0);
	id.set_path_out_of_date(false);
	return id.id;
}

dcon::land_construction_supply_route_id create_supply_route(sys::state& state, dcon::province_land_construction_id con, dcon::market_id origin, const std::vector<dcon::province_id>& path) {
	auto id = fatten(state.world, state.world.force_create_land_construction_supply_route(con, origin));
	auto type = state.world.province_land_construction_get_type(con);
	auto arr_size = state.military_definitions.unit_base_definitions[type].build_cost.size_used();
	id.get_buffered_goods().resize(arr_size);
	id.get_path().load_range(path.data(), path.data() + path.size());
	id.set_is_active(false);
	id.set_inactive_days(0);
	id.set_path_out_of_date(false);
	return id.id;
}

dcon::naval_construction_supply_route_id create_supply_route(sys::state& state, dcon::province_naval_construction_id con, dcon::market_id origin, const std::vector<dcon::province_id>& path) {
	auto id = fatten(state.world, state.world.force_create_naval_construction_supply_route(con, origin));
	auto type = state.world.province_naval_construction_get_type(con);
	auto arr_size = state.military_definitions.unit_base_definitions[type].build_cost.size_used();
	id.get_buffered_goods().resize(arr_size);
	id.get_path().load_range(path.data(), path.data() + path.size());
	id.set_is_active(false);
	id.set_inactive_days(0);
	id.set_path_out_of_date(false);
	return id.id;
}



template<concepts::military_unit unit_type>
bool unit_needs_left(const sys::state& state, unit_type unit) {
	
	for(auto com_id : state.world.in_unit_supply_commodity) {
		float commodity_need = unit_supply_need_get(state, unit, com_id);
		if(commodity_need != 0.0f) {
			return true;
		}
	}
	for(auto com_id : state.world.in_unit_build_commodity) {
		float commodity_need = unit_reinforcement_need_get(state, unit, com_id);
		if(commodity_need != 0.0f) {
			return true;
		}
	}
	return false;
};

template<concepts::military_construction_type construction_type>
bool construction_needs_left(const sys::state& state, construction_type con) {
	dcon::unit_type_id type = economy::construction_get_type(state, con);
	const economy::commodity_amounts& needs_left = constructions_need_get(state, con);
	const auto& unit_build_costs = state.military_definitions.unit_base_definitions[type].build_cost;
	for(uint32_t i = 0; i < unit_build_costs.set_size; i++) {
		auto commodity_id = unit_build_costs.commodity_type[i];
		if(commodity_id) {
			if(needs_left[i] != 0.0f) {
				return true;
			}
		}
		else {
			break;
		}
	}
	return false;
};

template<concepts::military_unit unit_type>
float military_goods_potential_volume_in_govt_stockpile(const sys::state& state, dcon::market_id origin, unit_type unit) {
	float potential_volume = 0.0f;
	state.world.for_each_unit_supply_commodity([&](dcon::unit_supply_commodity_id com_id) {
		auto base_commodity = state.world.unit_supply_commodity_get_base_commodity(com_id);
		float amount_wanted = unit_supply_need_get(state, unit, com_id);
		auto available_stockpile_amount = local_stockpile_available_goods_get(state, origin, base_commodity);
		potential_volume += std::min(amount_wanted, available_stockpile_amount);
	});
	state.world.for_each_unit_build_commodity([&](dcon::unit_build_commodity_id com_id) {
		auto base_commodity = state.world.unit_build_commodity_get_base_commodity(com_id);
		float amount_wanted = unit_reinforcement_need_get(state, unit, com_id);
		auto available_stockpile_amount = local_stockpile_available_goods_get(state, origin, base_commodity);
		potential_volume += std::min(amount_wanted, available_stockpile_amount);

	});
	return potential_volume;
};

template<concepts::military_construction_type construction_type>
float construction_goods_potential_volume_in_govt_stockpile(const sys::state& state, dcon::market_id origin, construction_type c) {
	float potential_volume = 0.0f;
	const auto& construction_needs = constructions_need_get(state, c);
	dcon::unit_type_id type = economy::construction_get_type(state, c);
	const auto& build_cost = state.military_definitions.unit_base_definitions[type].build_cost;
	for(uint32_t i = 0; i < build_cost.set_size; i++) {
		dcon::commodity_id commodity = build_cost.commodity_type[i];
		if(commodity) {
			float amount_wanted = construction_needs[i];
			auto available_stockpile_amount = local_stockpile_available_goods_get(state, origin, commodity);
			potential_volume += std::min(amount_wanted, available_stockpile_amount);
		}
	}
	return potential_volume;
};

// Updates the supply route buffered goods, volume and subtracts the goods consumed from the stockpile buffers of all potential supply routes connected to the military unit.
// Supply type decides whether to update supply, or reinforcement goods
template<concepts::military_unit unit_type>
void update_military_unit_routes_satisfaction(sys::state& state, unit_type unit,  dcon::nation_id nation) {



	auto process_route = [&]<concepts::military_supply_route_type route_type, concepts::military_unit unit_id_type>(dcon::market_id market, route_type route, unit_id_type unit) {
		// Update satisfaction of the route by decrementing the market govt stockpile satisfaction buffer to keep track of how many goods are left.
			// The containers storing the buffered supply&reinforcement for the route is assumed the same indexes as the respective commodity types in military_definitions (commodity_ids container)
			// That means we can simply iterate over the container with indexes and expect the goods_needed and buffered_supply/reinforcement containers to line up
		auto for_each_func = [&]<typename F>(F&& func) {
			state.world.for_each_unit_supply_commodity(func);
			state.world.for_each_unit_build_commodity(func);
			
		};
		auto fat_route = fatten(state.world, route);
		for_each_func([&](auto com_id) {
			dcon::commodity_id base_com_id = economy::unit_commodity_get_base_commodity(state, com_id);
			assert(base_com_id);
			float amount_needed = [&]() {
				if constexpr(std::is_same_v<decltype(com_id), dcon::unit_supply_commodity_id>) {
					return unit_supply_need_get(state, unit, com_id);
				}
				else if constexpr(std::is_same_v<decltype(com_id), dcon::unit_build_commodity_id>) {
					return unit_reinforcement_need_get(state, unit, com_id);
				}

			}();
			if(amount_needed <= 0.0f) {
				return;
			}
			float stockpile_buffer_amount = local_stockpile_available_goods_get(state, market, base_com_id);
			// The amount to consume is the minimum of the desired amount or the amount available in stockpile
			float to_consume = std::min(amount_needed, stockpile_buffer_amount);
			// Compute how much to consume to compensate for the expected loss on the route.
			assert(stockpile_buffer_amount - to_consume >= 0.0f);
			// Update stockpile buffer to reflect the amount that will be subtracted later
			local_stockpile_available_goods_set(state, market, base_com_id, stockpile_buffer_amount - to_consume);
			float total_stockpile_buffer_count = nation_stockpile_available_goods_get(state, nation, base_com_id);
			nation_stockpile_available_goods_set(state, nation, base_com_id, std::max(total_stockpile_buffer_count - to_consume, 0.0f));;
			if constexpr(std::is_same_v<decltype(com_id), dcon::unit_supply_commodity_id>) {
				fat_route.set_buffered_supply_goods(com_id, fat_route.get_buffered_supply_goods(com_id) + to_consume);
			}
			if constexpr(std::is_same_v<decltype(com_id), dcon::unit_build_commodity_id>) {
				fat_route.set_buffered_reinforcement_goods(com_id, fat_route.get_buffered_reinforcement_goods(com_id) + to_consume);
			}

			fat_route.set_volume(fat_route.get_volume() + to_consume);
			// Subtract from route need
			if constexpr(std::is_same_v<decltype(com_id), dcon::unit_supply_commodity_id>) {
				unit_supply_need_set(state, unit, com_id, amount_needed - to_consume);
			} else if constexpr(std::is_same_v<decltype(com_id), dcon::unit_build_commodity_id>) {
				unit_reinforcement_need_set(state, unit, com_id, amount_needed - to_consume);
			}
			else {
				static_assert(false, "Invalid type");
			}
		});
	};
	auto unit_location = military::unit_get_location(state, unit);

	auto stockpiles_buffer = unit_best_stockpiles_get(state, unit);
	for(auto stockpile_state : stockpiles_buffer) {
		auto market = state.world.state_instance_get_market_from_local_market(stockpile_state);
		auto origin_prov = state.world.state_instance_get_capital(stockpile_state);
		bool need_more_goods = unit_needs_left(state, unit);

		if(!need_more_goods) {
			break;
		}

		float potential_volume = military_goods_potential_volume_in_govt_stockpile(state, market, unit);
		if(potential_volume == 0.0f) {
			continue;
		}

		auto r = [&]() {
			if constexpr(std::is_same_v<unit_type, dcon::army_id>)
				return state.world.get_army_supply_route_by_origin_army_pair(unit, market);
			else if constexpr(std::is_same_v<unit_type, dcon::navy_id>) {
				return state.world.get_navy_supply_route_by_origin_navy_pair(unit, market);
			}
		}();
		// No current existing route. Pathfind and create one if an available path exists
		if(!r) {
			static std::vector<dcon::province_id> path{};
			path.clear();
			create_supply_route_path(state, unit, market, potential_volume, path);
			if(!supply_route_path_available(state, path, origin_prov, unit_location)) {
				// skip to next iteration if no path was available
				continue;
			}
			r = create_supply_route(state, unit, market, path);
		}

		auto route = fatten(state.world, r);
		assert(!route.get_is_active());
		// If the path requires an immediate update, then do it now
		if(route.get_path_out_of_date()) {
			update_supply_route_path(state, potential_volume, route.id);
		}
		if(!supply_route_path_available(state, route.get_path(), origin_prov, unit_location)) {
			// skip to next iteration if no path was available
			continue;
		}
	
		process_route(market, route.id, unit);
		auto path = route.get_path();
		// Update used supply throughput by adding the volume
		dcon::province_id route_dest = supply_route_get_destination(state, r);
		add_used_supply_throughput_and_used_port_capacity(state, path, route_dest, route.get_volume());
		route.set_is_active(true);
	}
}
template<concepts::supply_route_type route_type>
bool should_delete_route(const sys::state& state, route_type route) {
	auto fat_route = fatten(state.world, route);
	dcon::nation_id route_owner = supply_route_get_owner(state, route);
	dcon::province_id route_origin = supply_route_get_origin(state, route);
	dcon::nation_id route_origin_controller = state.world.province_get_nation_from_province_control(route_origin);
	// A supply route shall be deleted if it has been inactive for 230 days or more, OR if the route owner does not control the stockpile the route is connected to
	return fat_route.get_inactive_days() >= 230 || route_owner != route_origin_controller;
}




// Updates the supply route buffered goods, volume and subtracts the goods consumed from the stockpile buffers of all potential supply routes connected to the military unit.
// Supply type decides whether to update supply, or reinforcement goods
template<concepts::military_construction_type construction_type>
void update_construction_routes_satisfaction(sys::state& state, construction_type conc, dcon::nation_id nation) {

	if(!economy::can_advance_construction(state, conc)) {
		return;
	}

	auto construction = fatten(state.world, conc);
	auto con_location = economy::construction_get_location(state, construction.id);
	auto type = construction.get_type();
	const economy::commodity_set& build_cost = state.military_definitions.unit_base_definitions[type].build_cost;
	auto& construction_need = constructions_need_get(state, conc);

	auto process_route = [&]<concepts::construction_supply_route_type route_type>(dcon::market_id market, route_type route) {

		// Update satisfaction of the route by decrementing the market govt stockpile satisfaction buffer to keep track of how many goods are left.
			// The containers storing the buffered goods for the route is assumed the same indexes as the respective build cost for the unit type which the construction is building
			// That means we can simply iterate over the container with indexes and expect the goods_needed and buffered_goods containers to line up

		auto fat_route = fatten(state.world, route);
		economy::commodity_amounts& buffered_goods = fat_route.get_buffered_goods();

		for(uint32_t i = 0; i < build_cost.set_size; i++) {
			dcon::commodity_id com_id = build_cost.commodity_type[i];
			if(com_id) {
				float amount_needed = construction_need[i];
				if(amount_needed <= 0.0f) {
					continue;
				}
				float stockpile_buffer_amount = local_stockpile_available_goods_get(state, market, com_id);
				// The amount to consume is the minimum of the desired amount or the amount available in stockpile.
				float to_consume = std::min(amount_needed, stockpile_buffer_amount);
				assert(stockpile_buffer_amount - to_consume >= 0.0f);
				// Update stockpile buffer to reflect the amount that will be subtracted later
				local_stockpile_available_goods_set(state, market, com_id, stockpile_buffer_amount - to_consume);
				float total_stockpile_buffer_count = nation_stockpile_available_goods_get(state, nation, com_id);
				nation_stockpile_available_goods_set(state, nation, com_id, std::max(total_stockpile_buffer_count - to_consume, 0.0f));
				buffered_goods[i] += to_consume;
				fat_route.set_volume(fat_route.get_volume() + to_consume);

				// Subtract from route need
				construction_need[i] -= to_consume;
			} else {
				break;
			}
		}
	};
	auto stockpiles_buffer = construction_best_stockpiles_get(state, conc);
	for(auto stockpile_state : stockpiles_buffer) {
		auto market = state.world.state_instance_get_market_from_local_market(stockpile_state);
		auto origin_prov = state.world.state_instance_get_capital(stockpile_state);
		bool need_more_goods = construction_needs_left(state, conc);
		if(!need_more_goods) {
			break;
		}
		float potential_volume = construction_goods_potential_volume_in_govt_stockpile(state, market, conc);
		if(potential_volume == 0.0f) {
			continue;
		}
		auto r = [&]() {
			if constexpr(std::is_same_v<construction_type, dcon::province_land_construction_id>) {
				return state.world.get_land_construction_supply_route_by_origin_construction_pair(construction, market);
			} else if constexpr(std::is_same_v<construction_type, dcon::province_naval_construction_id>) {
				return state.world.get_naval_construction_supply_route_by_origin_construction_pair(construction, market);
			}
		}();

		// No current existing route. Pathfind and create one if an available path exists
		if(!r) {
			static std::vector<dcon::province_id> path{};
			path.clear();
			create_supply_route_path(state, construction.id, market, potential_volume, path);
			if(!supply_route_path_available(state, path, origin_prov, con_location)) {
				// skip to next iteration if no path was available
				continue;
			}
			r = create_supply_route(state, construction.id, market, path);
		}

		auto route = fatten(state.world, r);
		assert(!route.get_is_active());
		// If the path requires an immediate update, then do it now
		if(route.get_path_out_of_date()) {
			update_supply_route_path(state, potential_volume, route.id);
		}
		if(!supply_route_path_available(state, route.get_path(), origin_prov, con_location)) {
			// skip to next iteration if no path was available
			continue;
		}
		process_route(market, route.id);
		auto path = route.get_path();
		// Update used supply throughput by adding the volume
		dcon::province_id route_dest = supply_route_get_destination(state, route.id);
		add_used_supply_throughput_and_used_port_capacity(state, path, route_dest, route.get_volume());
		route.set_is_active(true);
	}
}

template<concepts::military_construction_type construction_type>
void accumulate_construction_requirements(sys::state& state, construction_type c) {
	auto construction = fatten(state.world, c);
	auto nation = construction.get_nation();
	auto type = construction.get_type();
	dcon::province_id location = economy::construction_get_location(state, c);
	auto build_cost_mult = economy::build_cost_multiplier(state, location, false);
	const economy::commodity_set& currently_fufilled = construction.get_purchased_goods();
	const auto& base_build_cost = state.military_definitions.unit_base_definitions[type].build_cost;
	auto& required_buffer = constructions_need_get(state, construction.id);
	for(uint32_t i = 0; i < base_build_cost.set_size; i++) {
		if(base_build_cost.commodity_type[i]) {
			assert(base_build_cost.commodity_type[i] == currently_fufilled.commodity_type[i]);
			float fufilled = currently_fufilled.commodity_amounts[i];
			float full_cost = base_build_cost.commodity_amounts[i] * build_cost_mult;
			float remaining_cost = std::max(full_cost - fufilled, 0.0f);
			float can_consume_per_day = full_cost;
			float to_consume = std::min(remaining_cost, can_consume_per_day);
			required_buffer[i] += to_consume;
		} else {
			break;
		}
	}
}
template<concepts::military_unit unit_type >
void accumulate_military_requirements(sys::state& state, unit_type u) {
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
			auto supply_id = state.world.commodity_get_unit_supply_commodity(cid);
			assert(supply_id);

			float full_supply_req = amount * total_sup_mods * supply_consumption;
			unit_supply_need_set(state, unit.id, supply_id, unit_supply_need_get(state, unit.id, supply_id) + full_supply_req);
		});
		build_cost.for_each_commodity([&](dcon::commodity_id cid, float amount) {
			auto build_id = state.world.commodity_get_unit_build_commodity(cid);
			assert(build_id);
			float full_reinf_req = amount * potential_reinforcement * reinforcement_consumption;
			unit_reinforcement_need_set(state, unit.id, build_id.id, unit_reinforcement_need_get(state, unit.id, build_id.id) + full_reinf_req);
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

	static thread_local ve::vectorizable_buffer<float, dcon::unit_supply_commodity_id> available_supply_goods_buffer(state.world.unit_supply_commodity_size());
	{
		static thread_local uint32_t old_count = state.world.unit_supply_commodity_size();
		auto new_count = state.world.unit_supply_commodity_size();
		if(new_count > old_count) {
			available_supply_goods_buffer = state.world.unit_supply_commodity_make_vectorizable_float_buffer();
			old_count = new_count;
		}
	}
	static thread_local ve::vectorizable_buffer<float, dcon::unit_build_commodity_id> available_reinforcement_goods_buffer(state.world.unit_build_commodity_size());
	{
		static thread_local uint32_t old_count = state.world.unit_build_commodity_size();
		auto new_count = state.world.unit_build_commodity_size();
		if(new_count > old_count) {
			available_reinforcement_goods_buffer = state.world.unit_build_commodity_make_vectorizable_float_buffer();
			old_count = new_count;
		}
	}
	state.world.execute_serial_over_unit_supply_commodity([&](auto ids) {
		available_supply_goods_buffer.set(ids, 0.0f);
	});
	state.world.execute_serial_over_unit_build_commodity([&](auto ids) {
		available_reinforcement_goods_buffer.set(ids, 0.0f);
	});

	// Sum up available supply and reinforcement goods from routes
	for(auto route : routes) {
		if(route.get_is_active()) {
			state.world.for_each_unit_supply_commodity([&](dcon::unit_supply_commodity_id com_id) {
				float current_avail = available_supply_goods_buffer.get(com_id);
				float buffered_amount = route.get_buffered_supply_goods(com_id);
				available_supply_goods_buffer.set(com_id, current_avail + (buffered_amount * route.get_throughput() * route.get_supply_loss()));
			});
			state.world.for_each_unit_build_commodity([&](dcon::unit_build_commodity_id com_id) {
				float current_avail = available_reinforcement_goods_buffer.get(com_id);
				float buffered_amount = route.get_buffered_reinforcement_goods(com_id);
				available_reinforcement_goods_buffer.set(com_id, current_avail + (buffered_amount * route.get_throughput() * route.get_supply_loss()));
			});
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
				auto sup_com_id = state.world.commodity_get_unit_supply_commodity(com_id);
				assert(sup_com_id);
				desired_amount *= supply_goods_cost_mod * supply_consumption_rate;
				float max_available = available_supply_goods_buffer.get(sup_com_id);
				float to_consume = std::min(max_available, desired_amount);
				assert(max_available - to_consume >= 0.0f);
				available_supply_goods_buffer.set(sup_com_id, max_available - to_consume);
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
				auto build_com_id = state.world.commodity_get_unit_build_commodity(com_id);
				assert(build_com_id);
				desired_amount *= reinf_goods_cost_mod * reinforcement_consumption_rate;
				float max_available = available_reinforcement_goods_buffer.get(build_com_id);
				float to_consume = std::min(max_available, desired_amount);
				assert(max_available - to_consume >= 0.0f);
				available_reinforcement_goods_buffer.set(build_com_id, max_available - to_consume);
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
	dcon::province_id location = economy::construction_get_location(state, c);
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
		if(route.get_is_active()) {
			const auto& route_goods = route.get_buffered_goods();
			for(uint32_t j = 0; j < build_costs.set_size; j++) {
				dcon::commodity_id com_id = build_costs.commodity_type[j];
				assert(build_costs.commodity_type[j] == current_fufilled.commodity_type[j]);
				if(com_id) {
					float& current_amount = current_fufilled.commodity_amounts[j];
					float route_amount = route_goods[j] * route.get_throughput() * route.get_supply_loss();
					current_amount += route_amount;
				} else {
					break;
				}
			}
		}
	}
}



void update_supply_routes_daily(sys::state& state) {

	// STEP 1: initialize buffers in parallel
	auto begin = std::chrono::steady_clock::now();
	concurrency::parallel_invoke(
	[&]() {
		state.world.execute_serial_over_army_supply_route([&](auto route_ids) {
			state.world.army_supply_route_set_volume(route_ids, ve::fp_vector{ 0.0f });
			state.world.army_supply_route_set_is_active(route_ids, ve::vbitfield_type{ 0 });
			state.world.for_each_unit_supply_commodity([&](dcon::unit_supply_commodity_id com_id) {
				state.world.army_supply_route_set_buffered_supply_goods(route_ids, com_id, 0.0f);
			});
			state.world.for_each_unit_build_commodity([&](dcon::unit_build_commodity_id com_id) {
				state.world.army_supply_route_set_buffered_reinforcement_goods(route_ids, com_id, 0.0f);
			});
		});
	},

	[&]() {
		state.world.execute_serial_over_navy_supply_route([&](auto route_ids) {
			state.world.navy_supply_route_set_volume(route_ids, ve::fp_vector{ 0.0f });
			state.world.navy_supply_route_set_is_active(route_ids, ve::vbitfield_type{ 0 });
			state.world.for_each_unit_supply_commodity([&](dcon::unit_supply_commodity_id com_id) {
				state.world.navy_supply_route_set_buffered_supply_goods(route_ids, com_id, 0.0f);
			});
			state.world.for_each_unit_build_commodity([&](dcon::unit_build_commodity_id com_id) {
				state.world.navy_supply_route_set_buffered_reinforcement_goods(route_ids, com_id, 0.0f);
			});
		});
	},
	[&]() {
		state.world.execute_serial_over_land_construction_supply_route([&](auto route_ids) {
			state.world.land_construction_supply_route_set_volume(route_ids, ve::fp_vector{ 0.0f });
			state.world.land_construction_supply_route_set_is_active(route_ids, ve::vbitfield_type{ 0 });
			state.world.for_each_commodity([&](dcon::commodity_id com_id) {
				ve::apply([&](auto route) {
					economy::commodity_amounts& goods = state.world.land_construction_supply_route_get_buffered_goods(route);
					std::fill(goods.begin(), goods.end(), 0.0f);
				}, route_ids);
			});
		});
		state.world.execute_serial_over_naval_construction_supply_route([&](auto route_ids) {
			state.world.naval_construction_supply_route_set_volume(route_ids, ve::fp_vector{ 0.0f });
			state.world.naval_construction_supply_route_set_is_active(route_ids, ve::vbitfield_type{ 0 });
			state.world.for_each_commodity([&](dcon::commodity_id com_id) {
				ve::apply([&](auto route) {
					economy::commodity_amounts& goods = state.world.naval_construction_supply_route_get_buffered_goods(route);
					std::fill(goods.begin(), goods.end(), 0.0f);
				}, route_ids);
			});
		});
	},
	[&]() {
		state.world.execute_serial_over_market([&](auto markets) {
			state.world.for_each_commodity([&](dcon::commodity_id com_id) {
				auto to_apply = state.world.market_get_government_stockpile(markets, com_id);
				local_stockpile_available_goods_set(state, markets, com_id, to_apply);
			});
		});
	},
	[&]() {
		state.world.execute_serial_over_army([&](auto armies) {
			state.world.for_each_unit_supply_commodity([&](dcon::unit_supply_commodity_id com_id) {
				unit_supply_need_set(state, armies, com_id, ve::fp_vector{ 0.0f });
			});
			state.world.for_each_unit_build_commodity([&](dcon::unit_build_commodity_id com_id) {
				unit_reinforcement_need_set(state, armies, com_id, ve::fp_vector{ 0.0f });
			});
			ve::apply([&](dcon::army_id army) {
				if(state.world.army_is_valid(army)) {
					auto buffer = unit_best_stockpiles_get(state, army);
					buffer.clear();
				}
			}, armies);
		});
	},
	[&]() {
		state.world.execute_serial_over_navy([&](auto navies) {
			state.world.for_each_unit_supply_commodity([&](dcon::unit_supply_commodity_id com_id) {
				unit_supply_need_set(state, navies, com_id, ve::fp_vector{0.0f });
			});
			state.world.for_each_unit_build_commodity([&](dcon::unit_build_commodity_id com_id) {
				unit_reinforcement_need_set(state, navies, com_id, ve::fp_vector{ 0.0f });
			});
			ve::apply([&](dcon::navy_id navy) {
				if(state.world.navy_is_valid(navy)) {
					auto buffer = unit_best_stockpiles_get(state, navy);
					buffer.clear();
				}
			}, navies);
		});
	},
	[&]() {
		state.world.execute_serial_over_province_land_construction([&](auto cons) {
			ve::apply([&](dcon::province_land_construction_id con) {
				if(state.world.province_land_construction_is_valid(con)) {
					auto& constructions_need = constructions_need_get(state, con);
					auto type = economy::construction_get_type(state, con);
					auto size = state.military_definitions.unit_base_definitions[type].build_cost.size_used();
					constructions_need.resize(size);
					std::fill(constructions_need.begin(), constructions_need.end(), 0.0f);
					auto stockpile_buffer = construction_best_stockpiles_get(state, con);
					stockpile_buffer.clear();
				}
			}, cons);
		});
	},
	[&]() {
		state.world.execute_serial_over_province_naval_construction([&](auto cons) {
			ve::apply([&](dcon::province_naval_construction_id con) {
				if(state.world.province_naval_construction_is_valid(con)) {
					auto& constructions_need = constructions_need_get(state, con);
					auto type = economy::construction_get_type(state, con);
					auto size = state.military_definitions.unit_base_definitions[type].build_cost.size_used();
					constructions_need.resize(size);
					std::fill(constructions_need.begin(), constructions_need.end(), 0.0f);
					auto stockpile_buffer = construction_best_stockpiles_get(state, con);
					stockpile_buffer.clear();
				}
			}, cons);
		});
	},
	[&]() {
		state.world.execute_serial_over_nation([&](auto nations) {
			state.world.for_each_unit_build_commodity([&](dcon::unit_build_commodity_id com_id) {
				nation_construction_need_set(state, nations, com_id, ve::fp_vector{ 0.0f });
				nation_construction_expected_satisfaction_set(state, nations, com_id, ve::fp_vector{0.0f });
			});
			state.world.for_each_unit_supply_and_build_commodity([&](dcon::unit_supply_and_build_commodity_id com_id) {
				nation_unit_expected_satisfaction_set(state, nations, com_id, ve::fp_vector{ 0.0f });
				nation_unit_prio_need_set<military::unit_priority::low_priority>(state, nations, com_id, ve::fp_vector{ 0.0f });
				nation_unit_prio_need_set<military::unit_priority::normal_priority>(state, nations, com_id, ve::fp_vector{ 0.0f });
				nation_unit_prio_need_set<military::unit_priority::high_priority>(state, nations, com_id, ve::fp_vector{ 0.0f });
			});
			state.world.for_each_commodity([&](dcon::commodity_id com_id) {
				auto to_apply = state.world.nation_get_total_stockpiles(nations, com_id);
				nation_stockpile_available_goods_set(state, nations, com_id, to_apply);
			});
		});
	},
	[&]() {
		state.world.execute_serial_over_province([&](auto provs) {
			state.world.province_set_used_supply_throughput(provs, 0.0f);
			state.world.province_set_used_port_supply_capacity(provs, 0.0f);
		});
	}
	);

	auto end = std::chrono::steady_clock::now();
	state.console_log(std::string("STEP 1 time: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())));
	begin = std::chrono::steady_clock::now();

	// STEP 2: Compute the closest stockpile states to each military unit and construction, and accumulate all the goods required by each of them into buffers. Run them in parallel

	military::parallel_for_each_unit(state, [&](auto unit) {
		dcon::nation_id nation = military::unit_get_controller(state, unit);
		if constexpr(std::is_same_v<decltype(unit), dcon::army_id>) {
			// Only armies can have rebels. Ignore those
			if(!nation) {
				return;
			}
		}
		dcon::province_id location = military::unit_get_location(state, unit);

		auto stockpile_buffer = unit_best_stockpiles_get(state, unit);

		economy::get_closest_available_market_states(state, stockpile_buffer, nation, location);
		accumulate_military_requirements(state, unit);
	});
	economy::parallel_for_each_unit_construction(state, [&](auto construction) {
		dcon::unit_type_id type = economy::construction_get_type(state, construction);
		const auto& build_costs = state.military_definitions.unit_base_definitions[type].build_cost;
		dcon::nation_id nation = economy::construction_get_controller(state, construction);
		dcon::province_id location = economy::construction_get_location(state, construction);
		auto stockpile_buffer = construction_best_stockpiles_get(state, construction);

		economy::get_closest_available_market_states(state, stockpile_buffer, nation, location);
		accumulate_construction_requirements(state, construction);
	});

	end = std::chrono::steady_clock::now();
	state.console_log(std::string("STEP 2 time: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())));

	begin = std::chrono::steady_clock::now();

	// STEP 3: Split the total goods required by units in each of the priority brackets (high, medium, low) per-nation, and accumulate total goods requried for constructions per-nation

	static std::vector<unit> low_prio_units;
	static std::vector<unit> normal_prio_units;
	static std::vector<unit> high_prio_units;
	low_prio_units.clear();
	normal_prio_units.clear();
	high_prio_units.clear();

	// Lambda for accumulating each military unit
	auto accumulate_prioritized_unit_supply = [&]<military::unit_priority priority, concepts::military_unit unit_type>(unit_type u, dcon::nation_id controller) {
		auto unit = fatten(state.world, u);
		state.world.for_each_unit_supply_commodity([&](dcon::unit_supply_commodity_id com_id) {
			auto sup_and_build_id = state.world.unit_supply_commodity_get_supply_and_build_commodity(com_id);
			nation_unit_prio_need_set<priority>(state, controller, sup_and_build_id, nation_unit_prio_need_get<priority>(state, controller, sup_and_build_id) + unit_supply_need_get(state, u, com_id));
			
		});
		state.world.for_each_unit_build_commodity([&](dcon::unit_build_commodity_id com_id) {
			auto sup_and_build_id = state.world.unit_build_commodity_get_supply_and_build_commodity(com_id);
			nation_unit_prio_need_set<priority>(state, controller, sup_and_build_id, nation_unit_prio_need_get<priority>(state, controller, sup_and_build_id) + unit_reinforcement_need_get(state, u, com_id));
		});
	};

	// Lambda for accumulating each military construction
	auto accumulate_construction_supply = [&]<concepts::military_construction_type construction_type>(construction_type construction) {
		dcon::unit_type_id constructing_unit_id = economy::construction_get_type(state, construction);
		dcon::nation_id nation = economy::construction_get_controller(state, construction);
		assert(nation);
		const auto& build_costs = state.military_definitions.unit_base_definitions[constructing_unit_id].build_cost;
		for(uint32_t j = 0; j < build_costs.set_size; j++) {
			auto com_id = build_costs.commodity_type[j];
			if(com_id) {
				auto build_com_id = state.world.commodity_get_unit_build_commodity(com_id);
				assert(build_com_id);
				float construction_need = constructions_need_get(state, construction)[j];
				nation_construction_need_set(state, nation, build_com_id, nation_construction_need_get(state, nation, build_com_id) + construction_need);
			} else {
				break;
			}
		}
	};

	// Accumulating military units and military constructions can be done in two seperate threads as they don't overlap
	concurrency::parallel_invoke(
		[&]() {
			military::for_each_unit(state, [&](auto mil_unit) {
				auto fat_unit = fatten(state.world, mil_unit);
				dcon::nation_id controller = military::unit_get_controller(state, mil_unit);
				if constexpr(std::is_same_v<decltype(mil_unit), dcon::army_id>) {
					if(!controller) {
						return;
					}
				}
				switch(fat_unit.get_supply_priority()) {
				case military::unit_priority::low_priority:
					accumulate_prioritized_unit_supply.template operator() < military::unit_priority::low_priority > (mil_unit, controller);
					low_prio_units.emplace_back(unit{ mil_unit });
					break;
				case military::unit_priority::normal_priority:
					accumulate_prioritized_unit_supply.template operator() < military::unit_priority::normal_priority > (mil_unit, controller);
					normal_prio_units.emplace_back(unit{ mil_unit });
					break;
				case military::unit_priority::high_priority:
					accumulate_prioritized_unit_supply.template operator() < military::unit_priority::high_priority > (mil_unit, controller);
					high_prio_units.emplace_back(unit{ mil_unit });
					break;
				}
			});
		},
		[&]() {
			economy::for_each_unit_construction(state, [&](auto con) {
				accumulate_construction_supply(con);
			});
		}
	);

	end = std::chrono::steady_clock::now();
	state.console_log(std::string("STEP 3 time: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())));
	begin = std::chrono::steady_clock::now();


	// STEP 4: update the paths of any supply routes which are deemed out-of-date or has no path. They are deemed out of date if anything significant happens to disrupt it and an update is scheduled. 
	// Weekly province updates are issued once per week at diffrent intervals, ie army supply routes are done at the 1st day of the week, navy supply routes on the 2nd day etc
	// Updates triggered by unit movement is done daily 

	auto day_of_week = state.current_date.value % 7;
	if(day_of_week == 0) {

		// Update paths which are flagged to require an update once per week
		parallel_for_each_supply_route(state, [&](auto route) {
			auto fat_route = fatten(state.world, route);
			for(dcon::province_id prov : fat_route.get_path()) {
				if(state.world.province_get_supply_route_requires_weekly_update(prov)) {
					fat_route.set_path_out_of_date(true);
					return; // Leave loop iteration 
				}
				auto nations_to_update = state.world.province_get_nation_routes_to_be_updated(prov);
				dcon::nation_id owner = supply_route_get_owner(state, route);
				if(state.world.nation_get_supply_routes_requires_path_update(owner)) {
					fat_route.set_path_out_of_date(true);
					return;
				}
				if(auto found = std::find(nations_to_update.begin(), nations_to_update.end(), owner); found != nations_to_update.end()) {
					fat_route.set_path_out_of_date(true);
					return;
				}
			}

		});
		state.world.execute_serial_over_province([&](auto prov_ids) {
			state.world.province_set_supply_route_requires_daily_update(prov_ids, ve::vbitfield_type{ 0 });
			state.world.province_set_supply_route_requires_weekly_update(prov_ids, ve::vbitfield_type{ 0 });
			ve::apply([&](dcon::province_id prov) {
				state.world.province_get_nation_routes_to_be_updated(prov).clear();
			}, prov_ids);
		});
	}
	end = std::chrono::steady_clock::now();
	state.console_log(std::string("STEP 4 time: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())));

	begin = std::chrono::steady_clock::now();



	// STEP 5: Update the actual goods satisfaction for all units and constructions. It will remove goods presumed to be consumed from the buffer.
	// If a route does not exist already between the stockpile and the unit/construction, it will be created
	// Units must be done in order of supply/reinforcement priority
	// Must be done serially as it will compute new paths and claim commodities in stockpiles as nessecary, in order of priority.

	// Lambda to compute the expected satisfaction of military units of a priority. Will be used to spread out the commodities abit more
	auto compute_nations_unit_expected_satisfaction = [&]<military::unit_priority priority>() {
		state.world.execute_parallel_over_nation([&](auto nations) {
			auto nation_valid = nations::exists(state, nations);
			// compute military units' expected satisfaction for a priority category for all nations
			state.world.for_each_unit_supply_and_build_commodity([&](dcon::unit_supply_and_build_commodity_id com_id) {
				dcon::commodity_id base_com_id = state.world.unit_supply_and_build_commodity_get_base_commodity(com_id);
				ve::fp_vector supply_required = nation_unit_prio_need_get<priority>(state, nations, com_id);
				auto expected_sat = ve::select(supply_required == 0.0f, 1.0f, ve::min(nation_stockpile_available_goods_get(state, nations, base_com_id) / supply_required, 1.0f));
				expected_sat = ve::select(nation_valid, expected_sat, 0.0f);
				nation_unit_expected_satisfaction_set(state, nations, com_id, expected_sat);
			});
		});
		const std::vector<unit>& unit_vec = [&]() -> const std::vector<unit>& {
			if constexpr(priority == military::unit_priority::low_priority) {
				return low_prio_units;
			} else if constexpr(priority == military::unit_priority::normal_priority) {
				return normal_prio_units;
			} else if constexpr(priority == military::unit_priority::high_priority) {
				return high_prio_units;
			}
		}();
		// update the supply&reinf needs for each military unit, using the computed buffered values
		concurrency::parallel_for_each(unit_vec.begin(), unit_vec.end(), [&](unit unit) {
			if(unit.is_army) {
				dcon::army_id army = unit.content.army;
				dcon::nation_id controller = state.world.army_get_controller_from_army_control(army);
				assert(controller);
				state.world.for_each_unit_supply_commodity([&](dcon::unit_supply_commodity_id com_id) {
					auto supply_build_id = state.world.unit_supply_commodity_get_supply_and_build_commodity(com_id);
					float expected_sat = nation_unit_expected_satisfaction_get(state, controller, supply_build_id);
					float current_req_supply = unit_supply_need_get(state, army, com_id);
					unit_supply_need_set(state, army, com_id, current_req_supply * expected_sat);
				});
				state.world.for_each_unit_build_commodity([&](dcon::unit_build_commodity_id com_id) {
					auto supply_build_id = state.world.unit_build_commodity_get_supply_and_build_commodity(com_id);
					float expected_sat = nation_unit_expected_satisfaction_get(state, controller, supply_build_id);
					float current_req_supply = unit_reinforcement_need_get(state, army, com_id);
					unit_reinforcement_need_set(state, army, com_id, current_req_supply * expected_sat);
				});
			}
			else {
				dcon::navy_id navy = unit.content.navy;
				dcon::nation_id controller = state.world.navy_get_controller_from_navy_control(navy);
				assert(controller);
				state.world.for_each_unit_supply_commodity([&](dcon::unit_supply_commodity_id com_id) {
					auto supply_build_id = state.world.unit_supply_commodity_get_supply_and_build_commodity(com_id);
					float expected_sat = nation_unit_expected_satisfaction_get(state, controller, supply_build_id);
					float current_req_supply = unit_supply_need_get(state, navy, com_id);
					unit_supply_need_set(state, navy, com_id, current_req_supply * expected_sat);
				});
				state.world.for_each_unit_build_commodity([&](dcon::unit_build_commodity_id com_id) {
					auto supply_build_id = state.world.unit_build_commodity_get_supply_and_build_commodity(com_id);
					float expected_sat = nation_unit_expected_satisfaction_get(state, controller, supply_build_id);
					float current_req_supply = unit_reinforcement_need_get(state, navy, com_id);
					unit_reinforcement_need_set(state, navy, com_id, current_req_supply * expected_sat);
				});
			}
		});

	};
	// Lambda to compute the expected satisfaction of military constructions. Will be used to spread out the commodities abit more
	auto compute_nations_construction_expected_satisfaction = [&]() {
		state.world.execute_parallel_over_nation([&](auto nations) {
			auto nation_valid = nations::exists(state, nations);
			// compute military constructions expected satisfaction for all nations
			state.world.for_each_unit_build_commodity([&](dcon::unit_build_commodity_id com_id) {
				dcon::commodity_id base_com_id = state.world.unit_build_commodity_get_base_commodity(com_id);
				auto supply_required = nation_construction_need_get(state, nations, com_id);
				auto expected_sat = ve::select(supply_required == 0.0f, 1.0f, ve::min(nation_stockpile_available_goods_get(state, nations, base_com_id) / supply_required, 1.0f));
				expected_sat = ve::select(nation_valid, expected_sat, 0.0f);
				nation_construction_expected_satisfaction_set(state, nations, com_id, expected_sat);
			});
		});
		economy::parallel_for_each_unit_construction(state, [&](auto con) {
			if(economy::can_advance_construction(state, con)) {
				dcon::unit_type_id type = economy::construction_get_type(state, con);
				economy::commodity_amounts& current_req_goods = constructions_need_get(state, con);
				auto controller = economy::construction_get_controller(state, con);
				const auto& build_cost = state.military_definitions.unit_base_definitions[type].build_cost;
				for(uint32_t i = 0; i < build_cost.set_size; i++) {
					auto com_id = build_cost.commodity_type[i];
					if(com_id) {
						auto build_com_id = state.world.commodity_get_unit_build_commodity(com_id);
						float expected_sat = nation_construction_expected_satisfaction_get(state, controller, build_com_id);
						current_req_goods[i] *= expected_sat;
					} else {
						break;
					}
				}
			}
		});

	};

	auto process_prioritized_unit_vector = [&](std::span<const unit> units_to_process) {
		for(auto unit : units_to_process) {
			if(unit.is_army) {
				auto army = unit.content.army;
				update_military_unit_routes_satisfaction(state, army, state.world.army_get_controller_from_army_control(army));
			} else {
				auto navy = unit.content.navy;
				update_military_unit_routes_satisfaction(state, navy, state.world.navy_get_controller_from_navy_control(navy));
			}
		}
	};

	auto process_constructions = [&]() {
		economy::for_each_unit_construction(state, [&](auto construction) {
			dcon::nation_id controller = economy::construction_get_controller(state, construction);
			update_construction_routes_satisfaction(state, construction, controller);
		});
	};

	// Pricess units first, in order of priority
	if(!high_prio_units.empty()) {
		compute_nations_unit_expected_satisfaction.template operator() < military::unit_priority::high_priority > ();
		process_prioritized_unit_vector(high_prio_units);
	}
	if(!normal_prio_units.empty()) {
		compute_nations_unit_expected_satisfaction.template operator() < military::unit_priority::normal_priority > ();
		process_prioritized_unit_vector(normal_prio_units);
	}
	if(!low_prio_units.empty()) {
		compute_nations_unit_expected_satisfaction.template operator() < military::unit_priority::low_priority > ();
		process_prioritized_unit_vector(low_prio_units);
	}
	// Lastly, process unit constructions
	compute_nations_construction_expected_satisfaction();
	process_constructions();


	end = std::chrono::steady_clock::now();
	state.console_log(std::string("STEP 5 time: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())));
	begin = std::chrono::steady_clock::now();

	// STEP 6: Update the amount of days a route has been inactive, and delete routes which are deemed to be too inactive (has not moved any volume of goods for a certain amount of days)

	parallel_for_each_supply_route(state, [&](auto r) {
		auto route = fatten(state.world, r);
		uint8_t new_inactive_days = (route.get_is_active() ? uint8_t(0) : route.get_inactive_days() + uint8_t(1));
		route.set_inactive_days(new_inactive_days);
		
	});
	// Finally, delete unused routes serially. Iterate from the end to compact as we go
	for(uint32_t i = state.world.army_supply_route_size(); i-- > 0;) {
		auto route = dcon::army_supply_route_id{ dcon::army_supply_route_id::value_base_t(i) };
		if(should_delete_route(state, route)) {
			delete_supply_route(state, route);
		}
	}
	for(uint32_t i = state.world.navy_supply_route_size(); i-- > 0;) {
		auto route = dcon::navy_supply_route_id{ dcon::navy_supply_route_id::value_base_t(i) };
		if(should_delete_route(state, route)) {
			delete_supply_route(state, route);
		}
	}
	for(uint32_t i = state.world.land_construction_supply_route_size(); i-- > 0;) {
		auto route = dcon::land_construction_supply_route_id{ dcon::land_construction_supply_route_id::value_base_t(i) };
		if(should_delete_route(state, route)) {
			delete_supply_route(state, route);
		}
	}
	for(uint32_t i = state.world.naval_construction_supply_route_size(); i-- > 0;) {
		auto route = dcon::naval_construction_supply_route_id{ dcon::naval_construction_supply_route_id::value_base_t(i) };
		if(should_delete_route(state, route)) {
			delete_supply_route(state, route);
		}
	}

	end = std::chrono::steady_clock::now();
	state.console_log(std::string("STEP 6 time: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())));
	begin = std::chrono::steady_clock::now();


	// STEP 7 : Update route throughput and route attrition values.
	// Can be done in parallel
	parallel_for_each_supply_route(state, [&](auto route) {
		dcon::nation_id controller = supply_route_get_owner(state, route);
		auto fat_route = fatten(state.world, route);
		if(fat_route.get_is_active()) {
			// Only update active routes. Inactive routes should be effectively invisible, they are cached for a few days before deletion to avoid re-creating routes&paths that switch between being active and inactive
			update_supply_route_throughput_attrition(state, route, controller);
		}
	});

	end = std::chrono::steady_clock::now();
	state.console_log(std::string("STEP 7 time: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())));
	begin = std::chrono::steady_clock::now();


	// STEP 8: subtract from stockpiles the actual buffered amount which each route has taken
	// Must take into account the actual throughput, and only subtract the percentage of goods which were possible to move

	for_each_unit_supply_route(state, [&](auto r) {
		auto route = fatten(state.world, r);
		if(route.get_is_active()) {
			dcon::nation_id route_owner = supply_route_get_owner(state, route);
			dcon::market_id origin_market = route.get_origin();
			float throughput = route.get_throughput();
			state.world.for_each_unit_supply_commodity([&](dcon::unit_supply_commodity_id com_id) {
				auto base_com_id = economy::unit_commodity_get_base_commodity(state, com_id);
				float amount_to_sub = route.get_buffered_supply_goods(com_id) * throughput;
				economy::subtract_government_stockpile(state, route_owner, origin_market, base_com_id, amount_to_sub);
			});
			state.world.for_each_unit_build_commodity([&](dcon::unit_build_commodity_id com_id) {
				auto base_com_id = economy::unit_commodity_get_base_commodity(state, com_id);
				float amount_to_sub = route.get_buffered_reinforcement_goods(com_id) * throughput;
				economy::subtract_government_stockpile(state, route_owner, origin_market, base_com_id, amount_to_sub);
			});
		}
	});

	for_each_construction_supply_route(state, [&](auto r) {
		auto route = fatten(state.world, r);
		if(route.get_is_active()) {
			dcon::nation_id route_owner = supply_route_get_owner(state, route);
			dcon::market_id origin_market = route.get_origin();
			auto construction = route.get_construction();
			float throughput = route.get_throughput();
			const auto& unit_build_goods = state.military_definitions.unit_base_definitions[construction.get_type()].build_cost;
			const auto& route_goods_amounts = route.get_buffered_goods();
			for(uint32_t i = 0; i < unit_build_goods.set_size; i++) {
				auto com_id = unit_build_goods.commodity_type[i];
				if(com_id) {
					economy::subtract_government_stockpile(state, route_owner, origin_market, com_id, route_goods_amounts[i] * throughput);
				}
				else {
					break;
				}
			}
		}
	});

	end = std::chrono::steady_clock::now();
	state.console_log(std::string("STEP 8 time: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())));

	// STEP 9: Update each army/navy supply & reinforcement satisfaction and advance constructions, by computing how much of their required commodities they were able to receive from all supply routes

	// Start processing each army/navy and applying reinforcement/supply satisfaction
	// Do military units
	begin = std::chrono::steady_clock::now();
	military::parallel_for_each_unit(state, [&](auto unit) {
		update_unit_commodity_satisfaction(state, unit);
	});
	// Then constructions
	economy::parallel_for_each_unit_construction(state, [&](auto construction) {
		update_construction_commodity_satisfaction(state, construction);
	});

	end = std::chrono::steady_clock::now();
	state.console_log(std::string("STEP 9 time: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())));

}

}
