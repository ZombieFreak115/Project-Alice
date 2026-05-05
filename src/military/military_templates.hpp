#pragma once
#include "system_state.hpp"
#include "military.hpp"
#include "economy_constants.hpp"

namespace military {

template<typename T>
auto province_is_blockaded(sys::state const& state, T ids) {
	return state.world.province_get_is_blockaded(ids);
}

template<typename T>
auto province_is_under_siege(sys::state const& state, T ids) {
	return state.world.province_get_siege_progress(ids) > 0.0f;
}

template<typename T>
auto battle_is_ongoing_in_province(sys::state const& state, T ids) {
	ve::apply(
			[&](dcon::province_id p) {
				auto battles = state.world.province_get_land_battle_location(p);
				return battles.begin() != battles.end();
			},
			ids);
	return false;
}

// Calculates whether province can support more regiments
// Considers existing regiments and construction as well
// Takes a filter function template to filter out which pops are eligible
template<typename F>
dcon::pop_id find_available_soldier(sys::state& state, dcon::province_id p, F&& filter) {

	float divisor = 0;

	if(state.world.province_get_is_colonial(p)) {
		divisor = state.defines.pop_size_per_regiment * state.defines.pop_min_size_for_regiment_colony_multiplier;
	}
	else if(!state.world.province_get_is_owner_core(p)) {
		divisor = state.defines.pop_size_per_regiment * state.defines.pop_min_size_for_regiment_noncore_multiplier;
	}
	else {
		divisor = state.defines.pop_size_per_regiment;
	}

	for(auto pop : state.world.province_get_pop_location(p)) {
		if(filter(state, pop.get_pop())) {
			if(can_pop_form_regiment(state, pop.get_pop(), divisor)) {
				return pop.get_pop().id;

			}
		}
	}
	return dcon::pop_id{};
	
}



// Finds a pop which can support more regiments anywhere in the target nation
// Takes a filter function template to filter out which pops are eligible
template<typename F>
dcon::pop_id find_available_soldier_anywhere(sys::state& state, dcon::nation_id nation, F&& filter) {
	if(!nation) {
		// can't find a pop for the invalid/rebel tag
		return dcon::pop_id{ };
	}

	for(auto p : state.world.nation_get_province_ownership(nation)) {
		auto prov = p.get_province();
		if(state.world.province_get_nation_from_province_control(prov) == nation) {
			auto pop = find_available_soldier(state, prov, filter);
			if(bool(pop)) {
				return pop;
			}
		}
	}
	return dcon::pop_id{};

}



// Calculates whether province can support more regiments when parsing OOBs
// Takes a filter function template to filter out which pops are eligible
template<typename F>
dcon::pop_id find_available_soldier_parsing(sys::state& state, dcon::province_id province_id, F&& filter) {
	float divisor = state.defines.pop_size_per_regiment;

	for(auto pop : state.world.province_get_pop_location(province_id)) {
		if(filter(state, pop.get_pop())) {
			if(can_pop_form_regiment(state, pop.get_pop(), divisor)) {
				return pop.get_pop().id;

			}
		}
	}
	return dcon::pop_id{ };
};




// Calculates reinforcement for a regiment while using the passed-in modifiers
// interval_type: Do we estimate the reinforcement per day, or per month?
// supply_type: Do we assume we have full supply, or do we scale it based on current satisfaction?
// potential_reinforcement: Do we cap the reinforcement at max strength, or not?
template<interval_estimation interval_type, supply_estimation supply_type, bool potential_reinforcement>
float calculate_regiment_reinforcement(sys::state& state, dcon::regiment_id regiment, float reinforcement_mods) {
	float reinf_fufillment;
	if constexpr(interval_type == interval_estimation::daily) {
		if constexpr(supply_type == supply_estimation::based_on_satisfaction) {
			reinf_fufillment = std::clamp(state.world.regiment_get_reinforcement_satisfaction_buffer(regiment) / economy::unit_reinforcement_demand_divisor, 0.f, 1.f);
		}
		// full supply always
		else {
			reinf_fufillment = std::clamp(1.0f / economy::unit_reinforcement_demand_divisor, 0.f, 1.f);
		}
	}
	// monthly
	else {
		if constexpr(supply_type == supply_estimation::based_on_satisfaction) {
			reinf_fufillment = state.world.regiment_get_reinforcement_satisfaction_buffer(regiment);
		}
		// full supply always
		else {
			reinf_fufillment = 1.0f;
		}
	}
	auto pop = state.world.regiment_get_pop_from_regiment_source(regiment);
	auto combined = reinf_fufillment * reinforcement_mods;
	float newstr;
	float curstr = state.world.regiment_get_strength(regiment);
	auto pop_size = state.world.pop_get_size(pop);
	if constexpr(!potential_reinforcement) {
		auto limit_fraction = std::max(state.defines.alice_full_reinforce, std::min(1.0f, pop_size / state.defines.pop_size_per_regiment));
		newstr = std::min(curstr + combined, limit_fraction);
		return newstr - curstr;
	} else {
		return combined;
	}

}
// Calculates reinforcement for a regiment, and computes the modifiers on its own
// US14 Calculates reinforcement for a particular regiment
template<interval_estimation interval_type, supply_estimation supply_type, bool potential_reinforcement>
float calculate_regiment_reinforcement(sys::state& state, dcon::regiment_id regiment) {
	auto mods = get_land_reinforcement_modifiers(state, regiment);
	return calculate_regiment_reinforcement<interval_type, supply_type, potential_reinforcement>(state, regiment, mods);

}

// Calculates combined reinforcement for an entire army
template<interval_estimation interval_type, supply_estimation supply_type, bool potential_reinforcement>
float calculate_army_reinforcement(sys::state& state, dcon::army_id army) {
	float total_reinforcement = 0.0f;
	for(auto r : state.world.army_get_army_membership(army)) {
		auto regiment = r.get_regiment();
		total_reinforcement += calculate_regiment_reinforcement<interval_type, supply_type, potential_reinforcement>(state, regiment);
	}
	return total_reinforcement;

}


// Calculates reinforcement for a ship while using the passed-in modifiers
// interval_type: Do we estimate the reinforcement per day, or per month?
// supply_type: Do we assume we have full supply, or do we scale it based on current satisfaction?
// potential_reinforcement: Do we cap the reinforcement at max strength, or not?
template<interval_estimation interval_type, supply_estimation supply_type, bool potential_reinforcement>
float calculate_ship_reinforcement(sys::state& state, dcon::ship_id ship, float reinforcement_mods) {
	float reinf_fufillment;
	if constexpr(interval_type == interval_estimation::daily) {
		if constexpr(supply_type == supply_estimation::based_on_satisfaction) {
			reinf_fufillment = std::clamp(state.world.ship_get_reinforcement_satisfaction_buffer(ship) / economy::unit_reinforcement_demand_divisor, 0.f, 1.f);
		}
		// full supply always
		else {
			reinf_fufillment = std::clamp(1.0f / economy::unit_reinforcement_demand_divisor, 0.f, 1.f);
		}
	}
	// monthly
	else {
		if constexpr(supply_type == supply_estimation::based_on_satisfaction) {
			reinf_fufillment = state.world.ship_get_reinforcement_satisfaction_buffer(ship);
		}
		// full supply always
		else {
			reinf_fufillment = 1.0f;
		}
	}
	auto combined = reinf_fufillment * reinforcement_mods;
	float newstr;
	float curstr = state.world.ship_get_strength(ship);
	if constexpr(!potential_reinforcement) {
		newstr = std::min(curstr + combined, 1.0f);
		return newstr - curstr;
	} else {
		return combined;
	}

}

// Calculates reinforcement for a regiment while computing the modifiers on its own
// interval_type: Do we estimate the reinforcement per day, or per month?
// supply_type: Do we assume we have full supply, or do we scale it based on current satisfaction?
// potential_reinforcement: Do we cap the reinforcement at max strength, or not?
template<interval_estimation interval_type, supply_estimation supply_type, bool potential_reinforcement>
float calculate_ship_reinforcement(sys::state& state, dcon::ship_id ship) {
	auto mods = get_naval_reinforcement_modifiers(state, ship);
	return calculate_ship_reinforcement<interval_type, supply_type, potential_reinforcement>(state, ship, mods);

}

// Calculates combined reinforcement for a whole navy
template<interval_estimation interval_type, supply_estimation supply_type, bool potential_reinforcement>
float calculate_navy_reinforcement(sys::state& state, dcon::navy_id navy) {
	float total_reinforcement = 0.0f;
	for(auto r : state.world.navy_get_navy_membership(navy)) {
		auto ship = r.get_ship();
		total_reinforcement += calculate_ship_reinforcement<interval_type, supply_type, potential_reinforcement>(state, ship);
	}
	return total_reinforcement;

}








} // namespace military
