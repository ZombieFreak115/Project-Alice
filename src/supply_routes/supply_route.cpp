#include "concept_declarations.hpp"
#include "system_state.hpp"
#include "military.hpp"
#include "economy.hpp"
#include "province.hpp"
#include "military_templates.hpp"

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
	/*constexpr unit(const unit&) = default;
	constexpr unit(unit&&) = default;
	constexpr unit& operator=(unit const&) = default;
	constexpr unit& operator=(unit&&) = default;*/
};





constexpr float infinite_supply_thoughput = 999999999999999.0f;
constexpr float base_naval_supply_throughput_per_km_speed = infinite_supply_thoughput;
constexpr float base_supply_throughput_per_km_speed = 10.0f; // Supply throughput per 1 km/h speed. Eg if set to 100 and a nation has a speed of 4 km/h, then the base is 400
constexpr float supply_throughput_infrastructure = 6.0f; // Extra supply throughput per 1% of infrastructure
constexpr float base_land_supply_attrition = 0.0000001f;
constexpr float base_sea_supply_attrition = 0.0f;

constexpr float control_level_supply_attrition = 0.0001f; // the supply loss % per km of travel if province control is 0%. Scales back to 0 at 100% control.
constexpr float militancy_supply_attrition = 0.00005f; // the supply loss % per km of travel per average militancy in the province.
constexpr float hostile_army_supply_attrition = 0.008f; // the supply loss % per km of travel per POP_SIZE_PER_REGIMENT enemy strength present in the land province
constexpr float hostile_navy_supply_attrition = 0.008f; // the supply loss % per km of travel per 100% enemy ship strength present in the sea province

// Draft values
float max_supply_throughput(const sys::state& state, dcon::province_id province, dcon::nation_id nation_as) {
	auto is_sea = province::is_sea(state, province);
	float speed;
	float movement_cost = province::movement_cost(state, province);
	if(is_sea) {
		speed = nations::naval_supply_speed(state, nation_as);
		return base_naval_supply_throughput_per_km_speed * (speed / movement_cost);
	}
	bool has_access = province::has_supply_access_to_province(state, nation_as, province);
	float infrastructure = province::get_infrastructure(state, province) * 100.0f;
	float siege_progress = state.world.province_get_siege_progress(province);
	speed = nations::land_supply_speed(state, nation_as);
	return std::max((base_supply_throughput_per_km_speed * (speed / movement_cost) + supply_throughput_infrastructure * infrastructure) * (has_access ? 1.0f - siege_progress : 0.0f), 0.0f);
}
float province_supply_attrition(const sys::state& state, dcon::province_id province, dcon::nation_id nation_as) {
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
float adjacency_supply_attrition(const sys::state& state, dcon::province_adjacency_id province_adj, dcon::nation_id nation_as) {
	auto prov_1 = state.world.province_adjacency_get_connected_provinces(province_adj, 0);
	auto prov_2 = state.world.province_adjacency_get_connected_provinces(province_adj, 1);
	auto distance = state.world.province_adjacency_get_distance_km(province_adj) * military::get_avg_movement_cost_modifier(state, nation_as, prov_1, prov_2);
	auto avg_supply_attr = (province_supply_attrition(state, prov_1, nation_as) + province_supply_attrition(state, prov_2, nation_as)) / 2.0f;
	assert(std::isfinite(distance * avg_supply_attr));
	return std::powf(avg_supply_attr, distance);
}




template<concepts::supply_route_type route_type>
void update_supply_route_throughput_attrition(sys::state& state, route_type r, dcon::nation_id controller) {

	auto route = fatten(state.world, r);
	auto path = route.get_path();
	auto origin_market = route.get_origin();
	auto origin_prov = supply_routes::supply_route_origin(state, r);
	assert(origin_prov);
	dcon::province_id destination = supply_routes::supply_route_destination(state, r);
	assert(destination);
	// If destination and origin are diffrent, and path size is 0, that means no path is available.
	if(destination != origin_prov && path.size() == 0) {
		route.set_throughput(0.0f);
		route.set_route_attrition(1.0f);
		return;
	}
	float total_attrition_mod = 1.0f;
	float smallest_supply_throughput = 1.0f;
	for(uint32_t i = 0; i < path.size(); i++) {
		auto prov = path[i];
		if(i + 1 < path.size()) {
			auto next_prov = path[i + 1];
			auto adj = state.world.get_province_adjacency_by_province_pair(prov, next_prov);
			assert(adj);
			total_attrition_mod *= adjacency_supply_attrition(state, adj, controller);
			assert(std::isfinite(total_attrition_mod));
		}
		float used_supply_throughput = state.world.province_get_used_supply_throughput(prov);
		float local_supply_throughput = std::min(max_supply_throughput(state, prov, controller) / (used_supply_throughput == 0.0f ? 1.0f : used_supply_throughput), 1.0f);
		smallest_supply_throughput = std::min(smallest_supply_throughput, local_supply_throughput);
	}
	route.set_route_attrition(total_attrition_mod);
	route.set_throughput(smallest_supply_throughput);
}

// Cannot be called in parallel as it may create & delete objects
template<concepts::supply_route_type route_type>
void update_supply_route_path(sys::state& state, route_type r) {

	auto route = fatten(state.world, r);
	auto market = route.get_origin();
	auto state_inst = market.get_zone_from_local_market();
	dcon::nation_id route_owner = supply_routes::supply_route_owner(state, r);
	dcon::province_id destination_location = supply_routes::supply_route_destination(state, r);
	auto volume = route.get_volume();
	static std::vector<dcon::province_id> path;
	path.clear();
	province::make_military_supply_path(state, state_inst, destination_location, route_owner, volume, path);
	auto existing_path = route.get_path();
	// Clear old path, and update used supply throughput
	std::for_each(existing_path.begin(), existing_path.end(), [&](dcon::province_id prov) {
		state.world.province_set_used_supply_throughput(prov, std::max(state.world.province_get_used_supply_throughput(prov) - volume, 0.0f));
	});
	// Apply new path, and update used supply throughput
	existing_path.clear();
	existing_path.load_range(path.data(), path.data() + path.size());
	std::for_each(path.begin(), path.end(), [&](dcon::province_id prov) {
		state.world.province_set_used_supply_throughput(prov, state.world.province_get_used_supply_throughput(prov) + volume);
	});
	route.set_path_out_of_date(false);
}
static int counter = 0;

// Tests if the route needs an update, and updates it if it does
template<concepts::supply_route_type route_type>
void try_update_route_path(sys::state& state, route_type r) {
	auto route = fatten(state.world, r);
	auto path_out_of_date = route.get_path_out_of_date();
	auto throughput = route.get_throughput();
	// Throughput of 0 means either no path available, or the path available is useless
	if(path_out_of_date || throughput == 0.0f) {
		//Try make new path
		counter++;
		update_supply_route_path(state, route.id);
	}
}


// Creates a army supply route and pathfinds automatically, while setting relavent fields on provinces it passes through
dcon::army_supply_route_id create_supply_route(sys::state& state, dcon::army_id army, dcon::market_id origin) {
	assert(state.world.army_get_controller_from_army_control(army));
	auto route = state.world.force_create_army_supply_route(army, origin);
	// Resize to fit with the total amount of possible supply/reinforcement goods 
	state.world.army_supply_route_get_buffered_reinforcement_goods(route).resize(state.military_definitions.military_build_goods.size());
	state.world.army_supply_route_get_buffered_supply_goods(route).resize(state.military_definitions.military_supply_goods.size());
	update_supply_route_path(state, route);
	update_supply_route_throughput_attrition(state, route, state.world.army_get_controller_from_army_control(army));
	return route;
}
// Creates a navy supply route and pathfinds automatically, while setting relavent fields on provinces it passes through
dcon::navy_supply_route_id create_supply_route(sys::state& state, dcon::navy_id navy, dcon::market_id origin) {
	assert(state.world.navy_get_controller_from_navy_control(navy));
	auto route = state.world.force_create_navy_supply_route(navy, origin);
	state.world.navy_supply_route_get_buffered_reinforcement_goods(route).resize(state.military_definitions.military_build_goods.size());
	state.world.navy_supply_route_get_buffered_supply_goods(route).resize(state.military_definitions.military_supply_goods.size());
	update_supply_route_path(state, route);
	update_supply_route_throughput_attrition(state, route, state.world.navy_get_controller_from_navy_control(navy));
	return route;
}

// Creates a land construction supply route and pathfinds automatically, while setting relavent fields on provinces it passes through
dcon::land_construction_supply_route_id create_supply_route(sys::state& state, dcon::province_land_construction_id construction, dcon::market_id origin) {
	auto route = state.world.force_create_land_construction_supply_route(construction, origin);
	auto type = state.world.province_land_construction_get_type(construction);
	const auto& build_cost = state.military_definitions.unit_base_definitions[type].build_cost;
	auto& route_goods = state.world.land_construction_supply_route_get_buffered_goods(route);
	// Push back one element for each commodity in the build_cost for the unit type, so they have the same size
	build_cost.for_each_commodity([&](dcon::commodity_id, float) {
		route_goods.push_back_unsafe(0.0f);
	});

	update_supply_route_path(state, route);
	update_supply_route_throughput_attrition(state, route, state.world.province_land_construction_get_nation(construction));
	return route;
}

// Creates a naval construction supply route and pathfinds automatically, while setting relavent fields on provinces it passes through
dcon::naval_construction_supply_route_id create_supply_route(sys::state& state, dcon::province_naval_construction_id construction, dcon::market_id origin) {
	auto route = state.world.force_create_naval_construction_supply_route(construction, origin);
	auto type = state.world.province_naval_construction_get_type(construction);
	const auto& build_cost = state.military_definitions.unit_base_definitions[type].build_cost;
	auto& route_goods = state.world.naval_construction_supply_route_get_buffered_goods(route);
	// Push back one element for each commodity in the build_cost for the unit type, so they have the same size
	build_cost.for_each_commodity([&](dcon::commodity_id, float) {
		route_goods.push_back_unsafe(0.0f);
	});

	update_supply_route_path(state, route);
	update_supply_route_throughput_attrition(state, route, state.world.province_naval_construction_get_nation(construction));
	return route;
}




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



// Updates route satisfaction by subtracting the wanted and available goods from the stockpile buffer and adding them to the route's buffered supply.
// Depending on the "consumption_type" template, it will only process either reinforcement or supply indivdiually if applicable
//template<supply_route_goods_type consumption_type, concepts::supply_route_type route_type, concepts::commodity_set_type buffer_type>
//void update_supply_route_satisfaction(sys::state& state, route_type r, buffer_type& route_need) {
//	auto route = fatten(state.world, r);
//	dcon::market_id market = route.get_origin();
//	for(uint32_t i = 0; i < economy::commodity_set::set_size; i++) {
//		if(route_need.commodity_type[i]) {
//			float& amount = route_need.commodity_amounts[i];
//			dcon::commodity_id com_id = route_need.commodity_type[i];
//			if(amount <= 0.0f) {
//				continue;
//			}
//			float stockpile_buffer_amount = state.world.market_get_govt_stockpile_satisfaction_buffer(market, com_id);
//			// The amount to consume is the minimum of the desired amount or the amount available in stockpile, multiplied by the route throughput which limits how many supplies may be routed
//			float to_consume = std::min(amount, stockpile_buffer_amount) * route.get_throughput();
//			// Compute how much to consume to compensate for the expected attrition on the route. For now,only constructions take attrition into account when consuming
//			float to_consume_w_attrition = std::min(amount / route.get_route_attrition(), stockpile_buffer_amount) * route.get_throughput();
//			assert(stockpile_buffer_amount - to_consume >= 0.0f);
//			assert(stockpile_buffer_amount - to_consume_w_attrition >= 0.0f);
//			// Update stockpile buffer to reflect the amount that will be subtracted later
//			state.world.market_set_govt_stockpile_satisfaction_buffer(market, com_id, stockpile_buffer_amount - to_consume_w_attrition);
//
//			route.get_buffered_goods().commodity_amounts[i] += to_consume_w_attrition;
//			route.set_volume(route.get_volume() + to_consume_w_attrition);
//
//
//			// Subtract from route need. Subtract the non-attrition compenstated amount as that is the amount we expect will actually make it through the route
//			amount -= to_consume;
//		} else {
//			break;
//		}
//	}
//	
//
//};



// Updates the supply route buffered goods, volume and subtracts the goods consumed from the stockpile buffers of all potential supply routes connected to the military unit.
// Supply type decides whether to update supply, or reinforcement goods
template<military::unit_consumption_type supply_type, concepts::military_unit unit_type>
void update_military_unit_routes_satisfaction(sys::state& state, unit_type unit, economy::huge_commodity_amount_array& goods_needs, std::vector<dcon::state_instance_id>& stockpiles_buffer) {
	for(auto stockpile_state : stockpiles_buffer) {
		auto market = state.world.state_instance_get_market_from_local_market(stockpile_state);
		auto r = [&]() {
			if constexpr(std::is_same_v<unit_type, dcon::army_id>)
				return state.world.get_army_supply_route_by_origin_army_pair(unit, market);
			else if constexpr(std::is_same_v<unit_type, dcon::navy_id>) {
				return state.world.get_navy_supply_route_by_origin_navy_pair(unit, market);
			}
			}();
		// skip if no route was created previously
		if(r) {
			auto route = fatten(state.world, r);
			// Do a check if we have satisfied all supply needs and can thus break
			bool need_more_goods = any_needs_left(state, goods_needs);
			if(!need_more_goods) {
				break;
			}
			economy::huge_commodity_id_array commodity_ids = [&]() {
				if constexpr(supply_type == military::unit_consumption_type::supply)
					return state.military_definitions.military_supply_goods;
				else if constexpr(supply_type == military::unit_consumption_type::reinforcement) {
					return state.military_definitions.military_build_goods;
				}
				}();
			// Update satisfaction of the route by decrementing the market govt stockpile satisfaction buffer to keep track of how many goods are left.
			// The containers storing the buffered supply&reinforcement for the route is assumed the same indexes as the respective commodity types in military_definitions (commodity_ids container)
			// That means we can simply iterate over the container with indexes and expect the goods_needed and buffered_supply/reinforcement containers to line up
			for(uint32_t i = 0; i < goods_needs.size(); i++) {
				auto com_id = commodity_ids[i];
				assert(com_id);

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
				if constexpr(supply_type == military::unit_consumption_type::supply) {
					route.get_buffered_supply_goods()[i] += to_consume_w_attrition;
				} else if constexpr(supply_type == military::unit_consumption_type::reinforcement) {
					route.get_buffered_reinforcement_goods()[i] += to_consume_w_attrition;
				}
				route.set_volume(route.get_volume() + to_consume_w_attrition);
				// Subtract from route need. Subtract the non-attrition compenstated amount as that is the amount we expect will actually make it through the route
				amount_needed -= to_consume;
			}
		}
	}
}




// Updates the supply route buffered goods, volume and subtracts the goods consumed from the stockpile buffers of all potential supply routes connected to the military unit.
// Supply type decides whether to update supply, or reinforcement goods
template<concepts::military_construction_type construction_type>
void update_construction_routes_satisfaction(sys::state& state, construction_type conc, economy::commodity_amount_array& goods_needs, std::vector<dcon::state_instance_id>& stockpiles_buffer) {
	auto construction = fatten(state.world, conc);
	auto type = construction.get_type();
	const auto& build_cost = state.military_definitions.unit_base_definitions[type].build_cost;
	for(auto stockpile_state : stockpiles_buffer) {
		auto market = state.world.state_instance_get_market_from_local_market(stockpile_state);
		auto r = [&]() {
			if constexpr(std::is_same_v<construction_type, dcon::province_land_construction_id>) {
				return state.world.get_land_construction_supply_route_by_origin_construction_pair(construction, market);
			} else if constexpr(std::is_same_v<construction_type, dcon::province_naval_construction_id>) {
				return state.world.get_naval_construction_supply_route_by_origin_construction_pair(construction, market);
			}
			}();
		// Skip if no route was created previously
		if(r) {
			auto route = fatten(state.world, r);
			// Do a check if we have satisfied all supply needs and can thus break
			bool need_more_goods = any_needs_left(state, goods_needs);
			if(!need_more_goods) {
				break;
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

					route.get_buffered_goods()[i] += to_consume_w_attrition;

					route.set_volume(route.get_volume() + to_consume_w_attrition);
					// Subtract from route need. Subtract the non-attrition compenstated amount as that is the amount we expect will actually make it through the route
					amount_needed -= to_consume;
				} else {
					break;
				}
			}
		}
	}
}

template<concepts::military_construction_type construction_type>
void accumulate_construction_requirements(const sys::state& state, construction_type c, economy::commodity_amount_array& buffer, std::vector<dcon::state_instance_id>& stockpiles_buffer) {
	auto construction = fatten(state.world, c);
	buffer.clear();
	auto nation = construction.get_nation();
	auto type = construction.get_type();
	dcon::province_id location;
	uint32_t construction_time;
	if constexpr(std::is_same_v<construction_type, dcon::province_land_construction_id>) {
		location = construction.get_pop().get_province_from_pop_location();
		construction_time = economy::land_unit_construction_time(state, type, nation);
	} else if constexpr(std::is_same_v<construction_type, dcon::province_naval_construction_id>) {
		location = construction.get_province();
		construction_time = economy::naval_unit_construction_time(state, type, nation);
	}
	auto build_cost_mult = economy::build_cost_multiplier(state, location, false);
	const auto& currently_fufilled = construction.get_purchased_goods();
	const auto& base_build_cost = state.military_definitions.unit_base_definitions[type].build_cost;
	for(uint32_t i = 0; i < base_build_cost.set_size; i++) {
		if(base_build_cost.commodity_type[i]) {
			assert(base_build_cost.commodity_type[i] == currently_fufilled.commodity_type[i]);
			float fufilled = currently_fufilled.commodity_amounts[i];
			float full_cost = base_build_cost.commodity_amounts[i] * build_cost_mult;
			float remaining_cost = std::max(full_cost - fufilled, 0.0f);
			float can_consume_per_day = full_cost / construction_time;
			float to_consume = std::min(remaining_cost, can_consume_per_day);
			buffer.push_back(to_consume);
		} else {
			break;
		}
	}
}
template<concepts::military_unit unit_type >
void accumulate_military_requirements(const sys::state& state, unit_type u, economy::huge_commodity_amount_array& supply_buffer, economy::huge_commodity_amount_array& reinf_buffer) {
	auto unit = fatten(state.world, u);
	dcon::nation_id nation;
	dcon::province_id location;

	float supply_consumption;
	float reinforcement_consumption;
	if constexpr(std::is_same_v<unit_type, dcon::army_id>) {
		nation = unit.get_controller_from_army_control();
		if(!nation) {
			return;
		}
		location = unit.get_location_from_army_location();
		supply_consumption = state.world.nation_get_land_supply_consumption(nation) / 100.0f;
		reinforcement_consumption = state.world.nation_get_land_reinforcement_consumption(nation) / 100.0f;
	} else if constexpr(std::is_same_v<unit_type, dcon::navy_id>) {
		nation = unit.get_controller_from_navy_control();
		location = unit.get_location_from_navy_location();
		supply_consumption = state.world.nation_get_naval_supply_consumption(nation) / 100.0f;
		reinforcement_consumption = state.world.nation_get_naval_reinforcement_consumption(nation) / 100.0f;
	}
	if constexpr(std::is_same_v<unit_type, dcon::army_id>) {
		for(auto r : state.world.army_get_army_membership(unit)) {
			// Accumulate the commodities needed
			auto regiment = r.get_regiment();
			auto type = state.world.regiment_get_type(regiment);
			const auto& supply_cost = state.military_definitions.unit_base_definitions[type].supply_cost;
			auto total_sup_mods = military::get_supply_cost_modifiers(state, regiment);

			const auto& build_cost = state.military_definitions.unit_base_definitions[type].build_cost;
			auto potential_reinforcement = military::estimate_reinforcement<military::interval_estimation::daily, military::supply_estimation::full_supply_always, false>(state, regiment);
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
	} else if constexpr(std::is_same_v<unit_type, dcon::navy_id>) {
		for(auto r : state.world.navy_get_navy_membership(unit)) {
			// Accumulate the commodities needed
			auto ship = r.get_ship();
			auto type = state.world.ship_get_type(ship);
			const auto& supply_cost = state.military_definitions.unit_base_definitions[type].supply_cost;
			auto total_sup_mods = military::get_supply_cost_modifiers(state, ship);

			const auto& build_cost = state.military_definitions.unit_base_definitions[type].build_cost;
			float potential_reinforcement = military::estimate_reinforcement<military::interval_estimation::daily, military::supply_estimation::full_supply_always, false>(state, ship);
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
}
template<military::unit_consumption_type consumption_type, concepts::military_unit unit_type>
void update_unit_commodity_satisfaction(sys::state& state, unit_type u) {
	auto unit = fatten(state.world, u);
	auto unit_membership = [&]() {
		if constexpr(std::is_same_v<unit_type, dcon::army_id>) {
			return unit.get_army_membership();
		} else if constexpr(std::is_same_v<unit_type, dcon::navy_id>) {
			return unit.get_navy_membership();
		}
		}();
	dcon::nation_id nation;
	if constexpr(std::is_same_v<unit_type, dcon::army_id>) {
		nation = unit.get_controller_from_army_control();
		// Rebels always have no supply or reinforcement for now
		if(!nation) {
			for(auto r : unit_membership) {
				auto reg = r.get_regiment();
				if constexpr(consumption_type == military::unit_consumption_type::supply) {
					reg.set_supply_satisfaction(0.0f);
					reg.set_last_supply_cost_modifier(0.0f);
				} else if constexpr(consumption_type == military::unit_consumption_type::reinforcement) {
					reg.set_reinforcement_satisfaction(0.0f);
					reg.set_total_pending_reinforcement(0.0f);
					reg.set_last_potential_reinforcement(0.0f);
				}
			}
		}
	} else if constexpr(std::is_same_v<unit_type, dcon::navy_id>) {
		nation = unit.get_controller_from_navy_control();
	}
	auto routes = [&]() {
		if constexpr(std::is_same_v<unit_type, dcon::army_id>) {
			return unit.get_army_supply_route();
		} else if constexpr(std::is_same_v<unit_type, dcon::navy_id>) {
			return unit.get_navy_supply_route();
		}
		}();
	float consumption_rate;
	if constexpr(consumption_type == military::unit_consumption_type::supply) {
		if constexpr(std::is_same_v<unit_type, dcon::army_id>) {
			consumption_rate = state.world.nation_get_land_supply_consumption(nation) / 100.0f;
		} else if constexpr(std::is_same_v<unit_type, dcon::navy_id>) {
			consumption_rate = state.world.nation_get_naval_supply_consumption(nation) / 100.0f;
		}
	} else if constexpr(consumption_type == military::unit_consumption_type::reinforcement) {
		if constexpr(std::is_same_v<unit_type, dcon::army_id>) {
			consumption_rate = state.world.nation_get_land_reinforcement_consumption(nation) / 100.0f;
		} else if constexpr(std::is_same_v<unit_type, dcon::navy_id>) {
			consumption_rate = state.world.nation_get_naval_reinforcement_consumption(nation) / 100.0f;
		}
	}


	const economy::huge_commodity_id_array& commodity_ids = [&]() {
		if constexpr(consumption_type == military::unit_consumption_type::supply) {
			return state.military_definitions.military_supply_goods;
		} else if constexpr(consumption_type == military::unit_consumption_type::reinforcement) {
			return state.military_definitions.military_build_goods;
		}
		}();

	economy::huge_commodity_amount_array available_goods_buffer(commodity_ids.size());

	for(auto route : routes) {
		for(uint32_t i = 0; i < commodity_ids.size(); i++) {
			dcon::commodity_id com_id = commodity_ids[i];
			assert(com_id);
			if constexpr(consumption_type == military::unit_consumption_type::supply) {
				available_goods_buffer[i] += (route.get_buffered_supply_goods()[i] * route.get_route_attrition());
			} else if constexpr(consumption_type == military::unit_consumption_type::reinforcement) {
				available_goods_buffer[i] += (route.get_buffered_reinforcement_goods()[i] * route.get_route_attrition());
			}

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
		float goods_cost_mod;
		if constexpr(consumption_type == military::unit_consumption_type::supply) {
			goods_cost_mod = military::get_supply_cost_modifiers(state, subunit);
		} else if constexpr(consumption_type == military::unit_consumption_type::reinforcement) {
			goods_cost_mod = military::estimate_reinforcement<military::interval_estimation::daily, military::supply_estimation::full_supply_always, false>(state, subunit);
		}
		dcon::unit_type_id type = subunit.get_type();
		const auto& goods_cost = [&]() {
			if constexpr(consumption_type == military::unit_consumption_type::supply) {
				return state.military_definitions.unit_base_definitions[type].supply_cost;
			} else if constexpr(consumption_type == military::unit_consumption_type::reinforcement) {
				return state.military_definitions.unit_base_definitions[type].build_cost;
			}
			}();
		float total_goods_desired = 0.0f;
		float total_goods_consumed = 0.0f;
		goods_cost.for_each_commodity([&](dcon::commodity_id com_id, float desired_amount) {
			int16_t index;
			if constexpr(consumption_type == military::unit_consumption_type::supply) {
				index = state.world.commodity_get_unit_supply_goods_index(com_id);
			} else if constexpr(consumption_type == military::unit_consumption_type::reinforcement) {
				index = state.world.commodity_get_unit_build_goods_index(com_id);
			}
			assert(index >= 0);
			desired_amount *= goods_cost_mod * consumption_rate;
			float max_available = available_goods_buffer[index];
			float to_consume = std::min(max_available, desired_amount);
			assert(max_available - to_consume >= 0.0f);
			available_goods_buffer[index] = max_available - to_consume;
			total_goods_desired += desired_amount;
			total_goods_consumed += to_consume;
		});
		float satisfaction;
		if(total_goods_desired == 0.0f) {
			satisfaction = consumption_rate;
		} else {
			satisfaction = total_goods_consumed / total_goods_desired * consumption_rate;
		}
		if constexpr(consumption_type == military::unit_consumption_type::supply) {
			subunit.set_supply_satisfaction(satisfaction);
			subunit.set_last_supply_cost_modifier(goods_cost_mod);
		} else if constexpr(consumption_type == military::unit_consumption_type::reinforcement) {
			float added_pending_reinforcement = satisfaction * goods_cost_mod;
			subunit.set_reinforcement_satisfaction(satisfaction);
			subunit.set_total_pending_reinforcement(subunit.get_total_pending_reinforcement() + added_pending_reinforcement);
			subunit.set_last_potential_reinforcement(goods_cost_mod);
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
template<military::unit_consumption_type consumption_type, concepts::commodity_amount_array_type buffer_type>
bool military_goods_potential_in_govt_stockpile(const sys::state& state, dcon::market_id origin, const buffer_type& supply_route_need) {
	economy::huge_commodity_id_array ids = [&]() {
		if constexpr(consumption_type == military::unit_consumption_type::supply) {
			return state.military_definitions.military_supply_goods;
		} else if constexpr(consumption_type == military::unit_consumption_type::reinforcement) {
			return state.military_definitions.military_build_goods;
		}
		}();
	for(uint32_t i = 0; i < supply_route_need.size(); i++) {
		dcon::commodity_id commodity = ids[i];
		assert(commodity);
		float amount_wanted = supply_route_need[i];
		auto available_stockpile_amount = state.world.market_get_government_stockpile(origin, commodity);
		if(amount_wanted > 0.0f && available_stockpile_amount > 0.0001f) {
			return true;
		}
	}
	return false;
};
template<concepts::commodity_amount_array_type buffer_type, concepts::military_construction_type construction_type>
bool construction_goods_potential_in_govt_stockpile(const sys::state& state, dcon::market_id origin, construction_type c, const buffer_type& supply_route_need) {
	auto construction = fatten(state.world, c);
	auto type = construction.get_type();
	const auto& build_cost = state.military_definitions.unit_base_definitions[type].build_cost;
	for(uint32_t i = 0; i < supply_route_need.size(); i++) {
		dcon::commodity_id commodity = build_cost.commodity_type[i];
		assert(commodity);
		float amount_wanted = supply_route_need[i];
		auto available_stockpile_amount = state.world.market_get_government_stockpile(origin, commodity);
		if(amount_wanted > 0.0f && available_stockpile_amount > 0.0001f) {
			return true;
		}

	}
	return false;
};


template<concepts::military_construction_type construction_type>
bool should_supply_route_be_created(sys::state& state, dcon::market_id market, construction_type c, const economy::commodity_amount_array& required_goods) {

	auto construction = fatten(state.world, c);
	auto state_inst = state.world.market_get_zone_from_local_market(market);
	dcon::nation_id stockpile_control = state.world.state_instance_get_nation_from_state_control(state_inst);
	dcon::nation_id dest_controller;
	if constexpr(std::is_same_v<construction_type, dcon::province_land_construction_id>) {
		dest_controller = construction.get_pop().get_province_from_pop_location().get_nation_from_province_control();
	} else if constexpr(std::is_same_v<construction_type, dcon::province_naval_construction_id>) {
		dest_controller = construction.get_province().get_nation_from_province_control();
	}
	if(stockpile_control != dest_controller) {
		return false;
	}
	if(!economy::can_advance_construction(state, c)) {
		return false;
	}
	return construction_goods_potential_in_govt_stockpile(state, market, c, required_goods);
}
template<concepts::military_unit unit_type>
bool should_supply_route_be_created(sys::state& state, dcon::market_id market, unit_type u, const economy::huge_commodity_amount_array& required_supply_goods, const economy::huge_commodity_amount_array& required_reinf_goods) {

	auto unit = fatten(state.world, u);
	auto state_inst = state.world.market_get_zone_from_local_market(market);
	dcon::nation_id stockpile_control = state.world.state_instance_get_nation_from_state_control(state_inst);
	dcon::nation_id dest_controller;
	if constexpr(std::is_same_v<unit_type, dcon::army_id>) {
		dest_controller = unit.get_controller_from_army_control();
		// armies may not be rebels
		if(!dest_controller) {
			return true;
		}
	} else if constexpr(std::is_same_v<unit_type, dcon::navy_id>) {
		dest_controller = unit.get_controller_from_navy_control();
	}
	if(stockpile_control != dest_controller) {
		return false;
	}
	return military_goods_potential_in_govt_stockpile<military::unit_consumption_type::supply>(state, market, required_supply_goods) || military_goods_potential_in_govt_stockpile<military::unit_consumption_type::reinforcement>(state, market, required_reinf_goods);
}


bool should_supply_route_be_deleted(sys::state& state, dcon::army_supply_route_id r, const economy::huge_commodity_amount_array& required_supply_goods, const economy::huge_commodity_amount_array& required_reinf_goods) {
	auto route = fatten(state.world, r);
	return !should_supply_route_be_created(state, route.get_origin(), route.get_army().id, required_supply_goods, required_reinf_goods);
}
bool should_supply_route_be_deleted(sys::state& state, dcon::navy_supply_route_id r, const economy::huge_commodity_amount_array& required_supply_goods, const economy::huge_commodity_amount_array& required_reinf_goods) {
	auto route = fatten(state.world, r);
	return !should_supply_route_be_created(state, route.get_origin(), route.get_navy().id, required_supply_goods, required_reinf_goods);
}
bool should_supply_route_be_deleted(sys::state& state, dcon::land_construction_supply_route_id r, const economy::commodity_amount_array& required_goods) {
	auto route = fatten(state.world, r);
	return !should_supply_route_be_created(state, route.get_origin(), route.get_construction().id, required_goods);
}
bool should_supply_route_be_deleted(sys::state& state, dcon::naval_construction_supply_route_id r, const economy::commodity_amount_array& required_goods) {
	auto route = fatten(state.world, r);
	return !should_supply_route_be_created(state, route.get_origin(), route.get_construction().id, required_goods);
}



struct closest_stockpiles_buffer {
	tagged_vector<std::vector<dcon::state_instance_id>, dcon::army_id> army_closest_stockpiles;
	tagged_vector<std::vector<dcon::state_instance_id>, dcon::navy_id> navy_closest_stockpiles;
	tagged_vector<std::vector<dcon::state_instance_id>, dcon::province_land_construction_id> land_construction_closest_stockpiles;
	tagged_vector<std::vector<dcon::state_instance_id>, dcon::province_naval_construction_id> naval_construction_closest_stockpiles;
};
struct army_navy_consumption_buffer {
	tagged_vector<economy::huge_commodity_amount_array, dcon::army_id> army_supply_need;
	tagged_vector<economy::huge_commodity_amount_array, dcon::army_id> army_reinforcement_need;
	tagged_vector<economy::huge_commodity_amount_array, dcon::navy_id> navy_supply_need;
	tagged_vector<economy::huge_commodity_amount_array, dcon::navy_id> navy_reinforcement_need;
	tagged_vector<economy::commodity_amount_array, dcon::province_land_construction_id> land_constructions_need;
	tagged_vector<economy::commodity_amount_array, dcon::province_naval_construction_id> naval_constructions_need;
};

void update_supply_routes_daily(sys::state& state) {

	// STEP 0: initialize buffers in parallel

	concurrency::parallel_invoke(
	[&]() {
		state.world.execute_serial_over_army_supply_route([&](auto route_ids) {
			state.world.army_supply_route_set_volume(route_ids, ve::fp_vector{ 0.0f });
			ve::apply([&](auto route) {
				// Clear commodity amounts. It should not be possible to have new goods appear as build costs for the same unit type at runtime so we can keep the same size
				economy::huge_commodity_amount_array& reinf_goods = state.world.army_supply_route_get_buffered_reinforcement_goods(route);
				economy::huge_commodity_amount_array& supply_goods = state.world.army_supply_route_get_buffered_supply_goods(route);
				std::fill(reinf_goods.begin(), reinf_goods.end(), 0.0f);
				std::fill(supply_goods.begin(), supply_goods.end(), 0.0f);
			}, route_ids);
		});
	},

	[&]() {
		state.world.execute_serial_over_navy_supply_route([&](auto route_ids) {
			state.world.navy_supply_route_set_volume(route_ids, ve::fp_vector{ 0.0f });
			ve::apply([&](auto route) {

				economy::huge_commodity_amount_array& reinf_goods = state.world.navy_supply_route_get_buffered_reinforcement_goods(route);
				economy::huge_commodity_amount_array& supply_goods = state.world.navy_supply_route_get_buffered_supply_goods(route);
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

					economy::commodity_amount_array& goods = state.world.land_construction_supply_route_get_buffered_goods(route);
					std::fill(goods.begin(), goods.end(), 0.0f);
				}, route_ids);
			});
		});
		state.world.execute_serial_over_naval_construction_supply_route([&](auto route_ids) {
			state.world.naval_construction_supply_route_set_volume(route_ids, ve::fp_vector{ 0.0f });
			state.world.for_each_commodity([&](dcon::commodity_id com_id) {
				ve::apply([&](auto route) {

					economy::commodity_amount_array& goods = state.world.naval_construction_supply_route_get_buffered_goods(route);
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
	}

	/*[&]() {
		state.world.execute_serial_over_province([&](auto province_ids) {
			state.world.province_set_used_supply_throughput(province_ids, 0.0f);
		});
	}*/
	);


	static army_navy_consumption_buffer consumption_buffer;
	static closest_stockpiles_buffer stockpiles_buffer;
	stockpiles_buffer.army_closest_stockpiles.resize(state.world.army_size());
	stockpiles_buffer.navy_closest_stockpiles.resize(state.world.navy_size());
	stockpiles_buffer.naval_construction_closest_stockpiles.resize(state.world.province_naval_construction_size());
	stockpiles_buffer.land_construction_closest_stockpiles.resize(state.world.province_land_construction_size());

	consumption_buffer.army_supply_need.resize(state.world.army_size(), economy::huge_commodity_amount_array(state.military_definitions.military_supply_goods.size()));
	consumption_buffer.army_reinforcement_need.resize(state.world.army_size(), economy::huge_commodity_amount_array(state.military_definitions.military_build_goods.size()));
	consumption_buffer.navy_supply_need.resize(state.world.navy_size(), economy::huge_commodity_amount_array(state.military_definitions.military_supply_goods.size()));
	consumption_buffer.navy_reinforcement_need.resize(state.world.navy_size(), economy::huge_commodity_amount_array(state.military_definitions.military_build_goods.size()));
	consumption_buffer.land_constructions_need.resize(state.world.province_land_construction_size());
	consumption_buffer.naval_constructions_need.resize(state.world.province_naval_construction_size());

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	// STEP 1: Compute the closest stockpile states to each military unit and construction, and accumulate all the goods required by each of them into buffers. Run them in parallel
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
		economy::get_closest_available_market_states(state, stockpiles_buffer.navy_closest_stockpiles[navy], nation, location);
		accumulate_military_requirements(state, navy, consumption_buffer.navy_supply_need[navy], consumption_buffer.navy_reinforcement_need[navy]);
	});

	concurrency::parallel_for(uint32_t(0), state.world.province_land_construction_size(), [&](uint32_t i) {
		dcon::province_land_construction_fat_id construction = fatten(state.world, dcon::province_land_construction_id{ dcon::province_land_construction_id::value_base_t(i) });
		if(!construction.is_valid()) {
			return;
		}
		auto nation = construction.get_nation();
		auto location = construction.get_pop().get_province_from_pop_location();
		stockpiles_buffer.land_construction_closest_stockpiles[construction].clear();
		economy::get_closest_available_market_states(state, stockpiles_buffer.land_construction_closest_stockpiles[construction], nation, location);
		accumulate_construction_requirements(state, construction.id, consumption_buffer.land_constructions_need[construction], stockpiles_buffer.land_construction_closest_stockpiles[construction]);

	});

	concurrency::parallel_for(uint32_t(0), state.world.province_naval_construction_size(), [&](uint32_t i) {
		dcon::province_naval_construction_fat_id construction = fatten(state.world, dcon::province_naval_construction_id{ dcon::province_naval_construction_id::value_base_t(i) });
		if(!construction.is_valid()) {
			return;
		}
		auto nation = construction.get_nation();
		auto location = construction.get_province();
		stockpiles_buffer.naval_construction_closest_stockpiles[construction].clear();
		economy::get_closest_available_market_states(state, stockpiles_buffer.naval_construction_closest_stockpiles[construction], nation, location);
		accumulate_construction_requirements(state, construction.id, consumption_buffer.naval_constructions_need[construction], stockpiles_buffer.naval_construction_closest_stockpiles[construction]);
	});

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	state.console_log(std::string("STEP 1 time: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())));



	auto reinforcement_prio_comparator = [&](unit unit_a, unit unit_b) {
		bool a_is_army = unit_a.is_army;
		bool b_is_army = unit_b.is_army;
		auto a_supply_prio = (a_is_army ? state.world.army_get_reinforcement_priority(unit_a.content.army) : state.world.navy_get_reinforcement_priority(unit_a.content.navy));
		auto b_supply_prio = (b_is_army ? state.world.army_get_reinforcement_priority(unit_b.content.army) : state.world.navy_get_reinforcement_priority(unit_b.content.navy));
		if(a_supply_prio != b_supply_prio) {
			return a_supply_prio > b_supply_prio;
		}
		if(a_is_army != b_is_army) {
			return a_is_army > b_is_army;
		}
		uint32_t a_index = (a_is_army ? unit_a.content.army.index() : unit_a.content.navy.index());
		uint32_t b_index = (b_is_army ? unit_b.content.army.index() : unit_b.content.navy.index());
		return a_index > b_index;
		};
	auto supply_prio_comparator = [&](unit unit_a, unit unit_b) {
		bool a_is_army = unit_a.is_army;
		bool b_is_army = unit_b.is_army;
		auto a_supply_prio = (a_is_army ? state.world.army_get_supply_priority(unit_a.content.army) : state.world.navy_get_supply_priority(unit_a.content.navy));
		auto b_supply_prio = (b_is_army ? state.world.army_get_supply_priority(unit_b.content.army) : state.world.navy_get_supply_priority(unit_b.content.navy));
		if(a_supply_prio != b_supply_prio) {
			return a_supply_prio > b_supply_prio;
		}
		if(a_is_army != b_is_army) {
			return a_is_army > b_is_army;
		}
		uint32_t a_index = (a_is_army ? unit_a.content.army.index() : unit_a.content.navy.index());
		uint32_t b_index = (b_is_army ? unit_b.content.army.index() : unit_b.content.navy.index());
		return a_index > b_index;
		};




	static tagged_vector<std::vector<unit>, dcon::nation_id> sorted_by_supply_prio;
	static tagged_vector<std::vector<unit>, dcon::nation_id> sorted_by_reinforcement_prio;
	sorted_by_supply_prio.resize(state.world.nation_size());
	sorted_by_reinforcement_prio.resize(state.world.nation_size());

	begin = std::chrono::steady_clock::now();

	// STEP 2: Sort military units per nation by their reinforcement and supply prio, to be used later.
	concurrency::parallel_for(uint32_t(0), state.world.nation_size(), [&](uint32_t i) {
		dcon::nation_fat_id nation = fatten(state.world, dcon::nation_id{ dcon::nation_id::value_base_t(i) });
		if(!nations::exists(state, nation)) {
			return;
		}
		sorted_by_supply_prio[nation].clear();
		sorted_by_reinforcement_prio[nation].clear();
		for(auto r : nation.get_army_control()) {
			sorted_by_supply_prio[nation].emplace_back(unit{ r.get_army().id });
			sorted_by_reinforcement_prio[nation].emplace_back(unit{ r.get_army().id });
		}
		for(auto r : nation.get_navy_control()) {
			sorted_by_supply_prio[nation].emplace_back(unit{ r.get_navy().id });
			sorted_by_reinforcement_prio[nation].emplace_back(unit{ r.get_navy().id });
		}
		std::sort(sorted_by_supply_prio[nation].begin(), sorted_by_supply_prio[nation].end(), supply_prio_comparator);
		std::sort(sorted_by_reinforcement_prio[nation].begin(), sorted_by_reinforcement_prio[nation].end(), reinforcement_prio_comparator);
	});
	end = std::chrono::steady_clock::now();
	state.console_log(std::string("STEP 2 time: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())));






	//STEP 3: Create supply routes for any potential route which lacks them for each military unit and construction. This must be done serially as the pathfinding takes other overlapping paths into account.
	// Don't create routes for units or constructions which can't consume goods anyhow (rebel army, construction is in an occupied province, or if there are none of the desired goods available in the stockpile)
	// Also delete routes which do not fit the criterias for existing
	begin = std::chrono::steady_clock::now();

	struct pending_deleted_routes {
		tagged_vector<fixed_bool_t, dcon::army_supply_route_id> army_routes;
		tagged_vector<fixed_bool_t, dcon::navy_supply_route_id> navy_routes;
		tagged_vector<fixed_bool_t, dcon::land_construction_supply_route_id> land_construction_routes;
		tagged_vector<fixed_bool_t, dcon::naval_construction_supply_route_id> naval_construction_routes;
	};
	struct pending_added_routes {
		tagged_vector<std::vector<dcon::market_id>, dcon::army_id> army_routes;
		tagged_vector<std::vector<dcon::market_id>, dcon::navy_id> navy_routes;
		tagged_vector<std::vector<dcon::market_id>, dcon::province_land_construction_id> land_construction_routes;
		tagged_vector<std::vector<dcon::market_id>, dcon::province_naval_construction_id> naval_construction_routes;
	};
	static pending_deleted_routes to_be_deleted;
	std::fill(to_be_deleted.army_routes.begin(), to_be_deleted.army_routes.end(), 0);
	std::fill(to_be_deleted.navy_routes.begin(), to_be_deleted.navy_routes.end(), 0);
	std::fill(to_be_deleted.land_construction_routes.begin(), to_be_deleted.land_construction_routes.end(), 0);
	std::fill(to_be_deleted.naval_construction_routes.begin(), to_be_deleted.naval_construction_routes.end(), 0);
	to_be_deleted.army_routes.resize(state.world.army_supply_route_size());
	to_be_deleted.navy_routes.resize(state.world.navy_supply_route_size());
	to_be_deleted.land_construction_routes.resize(state.world.land_construction_supply_route_size());
	to_be_deleted.naval_construction_routes.resize(state.world.naval_construction_supply_route_size());

	static pending_added_routes to_be_created;
	to_be_created.army_routes.resize(state.world.army_size());
	to_be_created.navy_routes.resize(state.world.navy_size());
	to_be_created.land_construction_routes.resize(state.world.province_land_construction_size());
	to_be_created.naval_construction_routes.resize(state.world.province_naval_construction_size());

	// Batch ones which need to be deleted
	concurrency::parallel_for(uint32_t(0), state.world.army_supply_route_size(), [&](uint32_t i) {
		dcon::army_supply_route_fat_id route = fatten(state.world, dcon::army_supply_route_id{ dcon::army_supply_route_id::value_base_t(i) });
		bool delete_route = should_supply_route_be_deleted(state, route.id, consumption_buffer.army_supply_need[route.get_army()], consumption_buffer.army_reinforcement_need[route.get_army()]);
		to_be_deleted.army_routes[route] = delete_route;
	});
	concurrency::parallel_for(uint32_t(0), state.world.navy_supply_route_size(), [&](uint32_t i) {
		dcon::navy_supply_route_fat_id route = fatten(state.world, dcon::navy_supply_route_id{ dcon::navy_supply_route_id::value_base_t(i) });
		bool delete_route = should_supply_route_be_deleted(state, route.id, consumption_buffer.navy_supply_need[route.get_navy()], consumption_buffer.navy_reinforcement_need[route.get_navy()]);
		to_be_deleted.navy_routes[route] = delete_route;

	});
	concurrency::parallel_for(uint32_t(0), state.world.land_construction_supply_route_size(), [&](uint32_t i) {
		dcon::land_construction_supply_route_fat_id route = fatten(state.world, dcon::land_construction_supply_route_id{ dcon::land_construction_supply_route_id::value_base_t(i) });
		bool delete_route = should_supply_route_be_deleted(state, route.id, consumption_buffer.land_constructions_need[route.get_construction()]);
		to_be_deleted.land_construction_routes[route] = delete_route;
	});
	concurrency::parallel_for(uint32_t(0), state.world.naval_construction_supply_route_size(), [&](uint32_t i) {
		dcon::naval_construction_supply_route_fat_id route = fatten(state.world, dcon::naval_construction_supply_route_id{ dcon::naval_construction_supply_route_id::value_base_t(i) });
		bool delete_route = should_supply_route_be_deleted(state, route.id, consumption_buffer.naval_constructions_need[route.get_construction()]);
		to_be_deleted.naval_construction_routes[route] = delete_route;
	});

	// Batch the ones which need to be created
	concurrency::parallel_for(uint32_t(0), state.world.army_size(), [&](uint32_t i) {
		dcon::army_fat_id army = fatten(state.world, dcon::army_id{ dcon::army_id::value_base_t(i) });
		if(!army.is_valid() || !army.get_controller_from_army_control()) {
			return;
		}
		to_be_created.army_routes[army].clear();
		for(auto stockpile_state : stockpiles_buffer.army_closest_stockpiles[army]) {
			auto market = state.world.state_instance_get_market_from_local_market(stockpile_state);
			auto route = state.world.get_army_supply_route_by_origin_army_pair(army, market);
			if(!route && should_supply_route_be_created(state, market, army.id, consumption_buffer.army_supply_need[army], consumption_buffer.army_reinforcement_need[army])) {
				to_be_created.army_routes[army].push_back(market);
			}
		}
	});
	concurrency::parallel_for(uint32_t(0), state.world.navy_size(), [&](uint32_t i) {
		dcon::navy_fat_id navy = fatten(state.world, dcon::navy_id{ dcon::navy_id::value_base_t(i) });
		if(!navy.is_valid()) {
			return;
		}
		to_be_created.navy_routes[navy].clear();
		for(auto stockpile_state : stockpiles_buffer.navy_closest_stockpiles[navy]) {
			auto market = state.world.state_instance_get_market_from_local_market(stockpile_state);
			auto route = state.world.get_navy_supply_route_by_origin_navy_pair(navy, market);
			if(!route && should_supply_route_be_created(state, market, navy.id, consumption_buffer.navy_supply_need[navy], consumption_buffer.navy_reinforcement_need[navy])) {
				to_be_created.navy_routes[navy].push_back(market);
			}
		}
	});
	concurrency::parallel_for(uint32_t(0), state.world.province_land_construction_size(), [&](uint32_t i) {
		dcon::province_land_construction_fat_id construction = fatten(state.world, dcon::province_land_construction_id{ dcon::province_land_construction_id::value_base_t(i) });
		if(!construction.is_valid()) {
			return;
		}
		to_be_created.land_construction_routes[construction].clear();
		for(auto stockpile_state : stockpiles_buffer.land_construction_closest_stockpiles[construction]) {
			auto market = state.world.state_instance_get_market_from_local_market(stockpile_state);
			auto route = state.world.get_land_construction_supply_route_by_origin_construction_pair(construction, market);
			if(!route && should_supply_route_be_created(state, market, construction.id, consumption_buffer.land_constructions_need[construction])) {
				to_be_created.land_construction_routes[construction].push_back(market);
			}
		}
	});

	concurrency::parallel_for(uint32_t(0), state.world.province_naval_construction_size(), [&](uint32_t i) {
		dcon::province_naval_construction_fat_id construction = fatten(state.world, dcon::province_naval_construction_id{ dcon::province_naval_construction_id::value_base_t(i) });
		if(!construction.is_valid()) {
			return;
		}
		to_be_created.naval_construction_routes[construction].clear();
		for(auto stockpile_state : stockpiles_buffer.naval_construction_closest_stockpiles[construction]) {
			auto market = state.world.state_instance_get_market_from_local_market(stockpile_state);
			auto route = state.world.get_naval_construction_supply_route_by_origin_construction_pair(construction, market);
			if(!route && should_supply_route_be_created(state, market, construction.id, consumption_buffer.naval_constructions_need[construction])) {
				to_be_created.naval_construction_routes[construction].push_back(market);
			}
		}
	});
	end = std::chrono::steady_clock::now();
	state.console_log(std::string("STEP 3 accumulate time: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())));

	begin = std::chrono::steady_clock::now();



	// Now, actually delete them. Iterate backwards through to compact as we go 
	for(uint32_t i = state.world.army_supply_route_size(); i-- > 0;) {
		dcon::army_supply_route_fat_id route = fatten(state.world, dcon::army_supply_route_id{ dcon::army_supply_route_id::value_base_t(i) });
		if(to_be_deleted.army_routes[route]) {
			state.world.delete_army_supply_route(route);
		}
	}
	for(uint32_t i = state.world.navy_supply_route_size(); i-- > 0;) {
		dcon::navy_supply_route_fat_id route = fatten(state.world, dcon::navy_supply_route_id{ dcon::navy_supply_route_id::value_base_t(i) });
		if(to_be_deleted.navy_routes[route]) {
			state.world.delete_navy_supply_route(route);
		}
	}
	for(uint32_t i = state.world.land_construction_supply_route_size(); i-- > 0;) {
		dcon::land_construction_supply_route_fat_id route = fatten(state.world, dcon::land_construction_supply_route_id{ dcon::land_construction_supply_route_id::value_base_t(i) });
		if(to_be_deleted.land_construction_routes[route]) {
			state.world.delete_land_construction_supply_route(route);
		}
	}
	for(uint32_t i = state.world.naval_construction_supply_route_size(); i-- > 0;) {
		dcon::naval_construction_supply_route_fat_id route = fatten(state.world, dcon::naval_construction_supply_route_id{ dcon::naval_construction_supply_route_id::value_base_t(i) });
		if(to_be_deleted.naval_construction_routes[route]) {
			state.world.delete_naval_construction_supply_route(route);
		}
	}


	// Now, create the queued routes serially

	state.world.for_each_army([&](dcon::army_id army) {
		const auto& market_vec = to_be_created.army_routes[army];
		for(auto market : market_vec) {
			create_supply_route(state, army, market);
		}
	});
	state.world.for_each_navy([&](dcon::navy_id navy) {
		const auto& market_vec = to_be_created.navy_routes[navy];
		for(auto market : market_vec) {
			create_supply_route(state, navy, market);
		}
	});
	state.world.for_each_province_land_construction([&](dcon::province_land_construction_id con) {
		const auto& market_vec = to_be_created.land_construction_routes[con];
		for(auto market : market_vec) {
			create_supply_route(state, con, market);
		}
	});
	state.world.for_each_province_naval_construction([&](dcon::province_naval_construction_id con) {
		const auto& market_vec = to_be_created.naval_construction_routes[con];
		for(auto market : market_vec) {
			create_supply_route(state, con, market);
		}
	});



	/*state.world.for_each_army([&](dcon::army_id army) {
		if(!state.world.army_get_controller_from_army_control(army)) {
			return;
		}
		for(auto stockpile_state : stockpiles_buffer.army_closest_stockpiles[army]) {
			auto market = state.world.state_instance_get_market_from_local_market(stockpile_state);
			auto route = state.world.get_army_supply_route_by_origin_army_pair(army, market);
			if(!route) {
				create_supply_route(state, army, market);
			}
		}
	});
	state.world.for_each_navy([&](dcon::navy_id navy) {
		for(auto stockpile_state : stockpiles_buffer.navy_closest_stockpiles[navy]) {
			auto market = state.world.state_instance_get_market_from_local_market(stockpile_state);
			auto route = state.world.get_navy_supply_route_by_origin_navy_pair(navy, market);
			if(!route) {
				create_supply_route(state, navy, market);
			}
		}
	});
	state.world.for_each_province_land_construction([&](dcon::province_land_construction_id construction) {
		if(economy::can_advance_construction(state, construction)) {
			for(auto stockpile_state : stockpiles_buffer.land_construction_closest_stockpiles[construction]) {
				auto market = state.world.state_instance_get_market_from_local_market(stockpile_state);
				auto route = state.world.get_land_construction_supply_route_by_origin_construction_pair(construction, market);
				if(!route) {
					create_supply_route(state, construction, market);
				}
			}
		}
	});
	state.world.for_each_province_naval_construction([&](dcon::province_naval_construction_id construction) {
		if(economy::can_advance_construction(state, construction)) {
			for(auto stockpile_state : stockpiles_buffer.naval_construction_closest_stockpiles[construction]) {
				auto market = state.world.state_instance_get_market_from_local_market(stockpile_state);
				auto route = state.world.get_naval_construction_supply_route_by_origin_construction_pair(construction, market);
				if(!route) {
					create_supply_route(state, construction, market);
				}
			}
		}
	});*/
	end = std::chrono::steady_clock::now();
	state.console_log(std::string("STEP 3 create/del routes time: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())));



	// STEP 4: update the paths of any supply routes which are deemed out-of-date or has no path. They are deemed out of date if anything significant happens to disrupt it and an update is scheduled. 
	// Weekly province updates are issued once per week at diffrent intervals, ie army supply routes are done at the 1st day of the week, navy supply routes on the 2nd day etc
	// Updates triggered by unit movement is done daily 

	begin = std::chrono::steady_clock::now();
	auto day_of_week = state.current_date.value % 7;
	concurrency::parallel_for(uint32_t(0), state.world.army_supply_route_size(), [&](uint32_t i) {
		dcon::army_supply_route_id route = dcon::army_supply_route_id(dcon::army_supply_route_id::value_base_t(i));
		for(auto prov : state.world.army_supply_route_get_path(route)) {
			if(state.world.province_get_supply_route_requires_weekly_update(prov)) {
				state.world.army_supply_route_set_path_out_of_date(route, true);
				return;
			}
		}
	});

	concurrency::parallel_for(uint32_t(0), state.world.navy_supply_route_size(), [&](uint32_t i) {
		dcon::navy_supply_route_id route = dcon::navy_supply_route_id(dcon::navy_supply_route_id::value_base_t(i));
		for(auto prov : state.world.navy_supply_route_get_path(route)) {
			if(state.world.province_get_supply_route_requires_weekly_update(prov)) {
				state.world.navy_supply_route_set_path_out_of_date(route, true);
				return;
			}
		}
	});

	concurrency::parallel_for(uint32_t(0), state.world.land_construction_supply_route_size(), [&](uint32_t i) {
		dcon::land_construction_supply_route_id route = dcon::land_construction_supply_route_id(dcon::land_construction_supply_route_id::value_base_t(i));
		for(auto prov : state.world.land_construction_supply_route_get_path(route)) {
			if(state.world.province_get_supply_route_requires_weekly_update(prov)) {
				state.world.land_construction_supply_route_set_path_out_of_date(route, true);
				return;
			}
		}
	});

	concurrency::parallel_for(uint32_t(0), state.world.naval_construction_supply_route_size(), [&](uint32_t i) {
		dcon::naval_construction_supply_route_id route = dcon::naval_construction_supply_route_id(dcon::naval_construction_supply_route_id::value_base_t(i));
		for(auto prov : state.world.naval_construction_supply_route_get_path(route)) {
			if(state.world.province_get_supply_route_requires_weekly_update(prov)) {
				state.world.naval_construction_supply_route_set_path_out_of_date(route, true);
				return;
			}
		}
	});
	end = std::chrono::steady_clock::now();
	state.console_log(std::string("STEP 3.5 time: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())));

	begin = std::chrono::steady_clock::now();
	counter = 0;
	// Update paths which have out of date paths, or have no throughput
	for(auto route : state.world.in_army_supply_route) {
		try_update_route_path(state, route.id);
	}
	for(auto route : state.world.in_navy_supply_route) {
		try_update_route_path(state, route.id);
	}
	for(auto route : state.world.in_land_construction_supply_route) {
		try_update_route_path(state, route.id);
	}
	for(auto route : state.world.in_naval_construction_supply_route) {
		try_update_route_path(state, route.id);
	}
	end = std::chrono::steady_clock::now();
	state.console_log(std::string("counter: " + std::to_string(counter)));
	state.console_log(std::string("STEP 4 time: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())));

	// STEP 5: Update route throughput and route attrition values.
	// Can be done in parallel
	begin = std::chrono::steady_clock::now();
	concurrency::parallel_for(uint32_t(0), state.world.army_supply_route_size(), [&](uint32_t i) {
		auto route = fatten(state.world, dcon::army_supply_route_id{ dcon::army_supply_route_id::value_base_t(i) });
		auto controller = route.get_army().get_controller_from_army_control();
		update_supply_route_throughput_attrition(state, route.id, controller);
	});
	concurrency::parallel_for(uint32_t(0), state.world.navy_supply_route_size(), [&](uint32_t i) {
		auto route = fatten(state.world, dcon::navy_supply_route_id{ dcon::navy_supply_route_id::value_base_t(i) });
		auto controller = route.get_navy().get_controller_from_navy_control();
		update_supply_route_throughput_attrition(state, route.id, controller);

	});
	concurrency::parallel_for(uint32_t(0), state.world.land_construction_supply_route_size(), [&](uint32_t i) {
		auto route = fatten(state.world, dcon::land_construction_supply_route_id{ dcon::land_construction_supply_route_id::value_base_t(i) });
		auto controller = route.get_construction().get_nation();
		update_supply_route_throughput_attrition(state, route.id, controller);
	});
	concurrency::parallel_for(uint32_t(0), state.world.naval_construction_supply_route_size(), [&](uint32_t i) {
		auto route = fatten(state.world, dcon::naval_construction_supply_route_id{ dcon::naval_construction_supply_route_id::value_base_t(i) });
		auto controller = route.get_construction().get_nation();
		update_supply_route_throughput_attrition(state, route.id, controller);
	});
	end = std::chrono::steady_clock::now();
	state.console_log(std::string("STEP 5 time: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())));







	// STEP 6: Update the actual goods satisfaction for all units and constructions, when taking into account throughput and attrition for each nation
	// Units must be done in order of supply/reinforcement priority
	// Can be done parallel over each nation as for now, route origin are only allowed to come from your own controlled states and nowhere else
	begin = std::chrono::steady_clock::now();
	concurrency::parallel_for(uint32_t(0), state.world.nation_size(), [&](uint32_t i) {
		dcon::nation_fat_id nation = fatten(state.world, dcon::nation_id{ dcon::nation_id::value_base_t(i) });
		if(!nations::exists(state, nation)) {
			return;
		}
		// Do supply first
		for(auto unit : sorted_by_supply_prio[nation]) {
			if(unit.is_army) {
				auto army = unit.content.army;
				update_military_unit_routes_satisfaction<military::unit_consumption_type::supply>(state, army, consumption_buffer.army_supply_need[army], stockpiles_buffer.army_closest_stockpiles[army]);
			} else {
				auto navy = unit.content.navy;
				update_military_unit_routes_satisfaction<military::unit_consumption_type::supply>(state, navy, consumption_buffer.navy_supply_need[navy], stockpiles_buffer.navy_closest_stockpiles[navy]);
			}
		}
		// Then reinforcement
		for(auto unit : sorted_by_reinforcement_prio[nation]) {
			if(unit.is_army) {
				auto army = unit.content.army;
				update_military_unit_routes_satisfaction<military::unit_consumption_type::reinforcement>(state, army, consumption_buffer.army_reinforcement_need[army], stockpiles_buffer.army_closest_stockpiles[army]);
			} else {
				auto navy = unit.content.navy;
				update_military_unit_routes_satisfaction<military::unit_consumption_type::reinforcement>(state, navy, consumption_buffer.navy_reinforcement_need[navy], stockpiles_buffer.navy_closest_stockpiles[navy]);
			}
		}
		// Then land constructions
		for(auto construction : nation.get_province_land_construction()) {
			update_construction_routes_satisfaction(state, construction.id, consumption_buffer.land_constructions_need[construction], stockpiles_buffer.land_construction_closest_stockpiles[construction]);

		}
		// Then land constructions
		for(auto construction : nation.get_province_naval_construction()) {
			update_construction_routes_satisfaction(state, construction.id, consumption_buffer.naval_constructions_need[construction], stockpiles_buffer.naval_construction_closest_stockpiles[construction]);
		}


	});
	end = std::chrono::steady_clock::now();
	state.console_log(std::string("STEP 6 time: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())));



	// STEP 7: subtract from stockpiles the actual buffered amount which each route has taken
	// Can be done in parallel over each nation
	begin = std::chrono::steady_clock::now();
	concurrency::parallel_for(uint32_t(0), state.world.nation_size(), [&](uint32_t i) {
		dcon::nation_id nation{ dcon::nation_id::value_base_t(i) };
		if(!nations::exists(state, nation)) {
			return;
		}
		state.world.nation_for_each_state_control(nation, [&](dcon::state_control_id sc) {
			auto state_inst = state.world.state_control_get_state(sc);
			auto market = state.world.state_instance_get_market_from_local_market(state_inst);
			state.world.for_each_commodity([&](dcon::commodity_id commodity_id) {
				auto to_apply = state.world.market_get_govt_stockpile_satisfaction_buffer(market, commodity_id);
				// Consume the goods. We know that this amount is exactly the amount left after consumption, and it should not be less than zero
				assert(to_apply >= 0.0f);
				economy::set_government_stockpile(state, nation, market, commodity_id, to_apply);
			});
		});
	});
	end = std::chrono::steady_clock::now();
	state.console_log(std::string("STEP 7 time: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())));

	// STEP 8: Update each army/navy supply & reinforcement satisfaction and advance constructions, by computing how much of their required commodities they were able to receive from all supply routes

	// Start processing each army/navy and applying reinforcement/supply satisfaction
	// Do armies
	begin = std::chrono::steady_clock::now();
	concurrency::parallel_for(uint32_t(0), state.world.army_size(), [&](uint32_t i) {
		dcon::army_id army{ dcon::army_id::value_base_t(i) };
		if(!state.world.army_is_valid(army)) {
			return;
		}
		update_unit_commodity_satisfaction<military::unit_consumption_type::supply>(state, army);
		update_unit_commodity_satisfaction<military::unit_consumption_type::reinforcement>(state, army);
	});



	// Then navies
	concurrency::parallel_for(uint32_t(0), state.world.navy_size(), [&](uint32_t i) {
		dcon::navy_id navy{ dcon::navy_id::value_base_t(i) };
		if(!state.world.navy_is_valid(navy)) {
			return;
		}
		update_unit_commodity_satisfaction<military::unit_consumption_type::supply>(state, navy);
		update_unit_commodity_satisfaction<military::unit_consumption_type::reinforcement>(state, navy);
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


	state.world.execute_serial_over_province([&](auto prov_ids) {
		state.world.province_set_supply_route_requires_daily_update(prov_ids, ve::vbitfield_type{ 0 });
		state.world.province_set_supply_route_requires_weekly_update(prov_ids, ve::vbitfield_type{ 0 });
	});
	end = std::chrono::steady_clock::now();
	state.console_log(std::string("STEP 8 time: " + std::to_string(std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count())));
}




}
