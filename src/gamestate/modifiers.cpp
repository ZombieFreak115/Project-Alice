#include "modifiers.hpp"
#include "system_state.hpp"
#include "demographics.hpp"
#include "military.hpp"
#include "military_templates.hpp"
#include "province.hpp"
#include "province_templates.hpp"
#include "triggers.hpp"
#include "ve_scalar_extensions.hpp"
#include "economy.hpp"
#include "supply_route.hpp"

namespace sys {

void apply_hardcoded_modifier_values_to_nation(sys::state& state, dcon::nation_id target_nation) {
	auto fat_nation = fatten(state.world, target_nation);
	float land_supply_speed = supply_routes::land_supply_speed(state, target_nation);
	float cur_land_throughput = fat_nation.get_modifier_values(sys::national_mod_offsets::national_land_supply_throughput_add);
	fat_nation.set_modifier_values(sys::national_mod_offsets::national_land_supply_throughput_add, cur_land_throughput + land_supply_speed * supply_routes::supply_throughput_per_km_land_supply_speed);

	float naval_supply_speed = supply_routes::naval_supply_speed(state, target_nation);
	float cur_naval_throughput = fat_nation.get_modifier_values(sys::national_mod_offsets::national_naval_supply_throughput_add);
	fat_nation.set_modifier_values(sys::national_mod_offsets::national_naval_supply_throughput_add, cur_naval_throughput + naval_supply_speed * supply_routes::supply_throughput_per_km_naval_supply_speed);
}
template<concepts::dcon_id_ve_type<dcon::nation_id> nation_ids>
void ve_apply_hardcoded_modifier_values_to_nation(sys::state& state, nation_ids target_nations, ve::mask_vector apply_mask) {
	auto land_supply_speed = supply_routes::land_supply_speed(state, target_nations);
	auto cur_land_throughput = state.world.nation_get_modifier_values(target_nations, sys::national_mod_offsets::national_land_supply_throughput_add);
	state.world.nation_set_modifier_values(target_nations, sys::national_mod_offsets::national_land_supply_throughput_add, ve::select(apply_mask, cur_land_throughput + land_supply_speed * supply_routes::supply_throughput_per_km_land_supply_speed, cur_land_throughput));

	auto naval_supply_speed = supply_routes::naval_supply_speed(state, target_nations);
	auto cur_naval_throughput = state.world.nation_get_modifier_values(target_nations, sys::national_mod_offsets::national_naval_supply_throughput_add);
	state.world.nation_set_modifier_values(target_nations, sys::national_mod_offsets::national_naval_supply_throughput_add, ve::select(apply_mask, cur_naval_throughput + naval_supply_speed * supply_routes::supply_throughput_per_km_naval_supply_speed, cur_naval_throughput));
}

void apply_hardcoded_modifier_values_to_province(sys::state& state, dcon::province_id prov) {
	auto fat_prov = fatten(state.world, prov);
	// Apply supply throughput modifiers from movement cost
	auto movement_cost = province::movement_cost(state, prov);
	float current = fat_prov.get_modifier_values(sys::provincial_mod_offsets::supply_throughput_percent);
	fat_prov.set_modifier_values(sys::provincial_mod_offsets::supply_throughput_percent, current + ((1.0f / movement_cost) - 1.0f));
}
template<concepts::dcon_id_ve_type<dcon::province_id> province_ids>
void ve_apply_hardcoded_modifier_values_to_province(sys::state& state, province_ids provs, ve::mask_vector apply_mask) {
	// Apply supply throughput modifiers from movement cost
	auto movement_cost = province::movement_cost(state, provs);
	float current = state.world.province_get_modifier_values(provs, sys::provincial_mod_offsets::supply_throughput_percent);
	state.world.province_set_modifier_values(provs, sys::provincial_mod_offsets::supply_throughput_percent, ve::select(apply_mask, current + ((1.0f / movement_cost) - 1.0f), current));
}


// NOTE: these functions do not add or remove a modifier from the list of modifiers for an entity
void apply_modifier_values_to_nation(sys::state& state, dcon::nation_id target_nation, dcon::modifier_id mod_id) {
	auto& nat_values = state.world.modifier_get_national_values(mod_id);
	for(uint32_t i = 0; i < sys::national_modifier_definition::modifier_definition_size; ++i) {
		if(!(nat_values.offsets[i]))
			break; // no more modifier values

		auto fixed_offset = nat_values.offsets[i];
		auto modifier_amount = nat_values.values[i];
		const auto& metadata = sys::national_modifier_metadata[fixed_offset.index()];
		auto current_val = state.world.nation_get_modifier_values(target_nation, fixed_offset);
		float new_val = (metadata.op == sys::modifier_operation::add ? current_val + modifier_amount : current_val * modifier_amount);
		state.world.nation_set_modifier_values(target_nation, fixed_offset, new_val);
	}
}

template<concepts::dcon_id_ve_type<dcon::nation_id> nation_ids>
void ve_apply_modifier_values_to_nation(sys::state& state, nation_ids target_nations, dcon::modifier_id mod_id) {
	auto& nat_values = state.world.modifier_get_national_values(mod_id);
	for(uint32_t i = 0; i < sys::national_modifier_definition::modifier_definition_size; ++i) {
		if(!(nat_values.offsets[i]))
			break; // no more modifier values

		auto fixed_offset = nat_values.offsets[i];
		auto modifier_amount = nat_values.values[i];
		const auto& metadata = sys::national_modifier_metadata[fixed_offset.index()];
		auto current_val = state.world.nation_get_modifier_values(target_nations, fixed_offset);
		auto new_val = (metadata.op == sys::modifier_operation::add ? current_val + modifier_amount : current_val * modifier_amount);
		state.world.nation_set_modifier_values(target_nations, fixed_offset, new_val);
	}
}
template<typename F, concepts::dcon_id_ve_type<dcon::nation_id> nation_ids>
void ve_apply_masked_modifier_values_to_nation(sys::state& state, nation_ids target_nations, dcon::modifier_id mod_id, F&& mask_func) {
	auto& nat_values = state.world.modifier_get_national_values(mod_id);
	ve::mask_vector mask = mask_func(target_nations);
	for(uint32_t i = 0; i < sys::national_modifier_definition::modifier_definition_size; ++i) {
		if(!(nat_values.offsets[i]))
			break; // no more modifier values

		auto fixed_offset = nat_values.offsets[i];
		auto modifier_amount = nat_values.values[i];
		const auto& metadata = sys::national_modifier_metadata[fixed_offset.index()];
		auto current_val = state.world.nation_get_modifier_values(target_nations, fixed_offset);
		state.world.nation_set_modifier_values(target_nations, fixed_offset, ve::select(mask, (metadata.op == sys::modifier_operation::add ? current_val + modifier_amount : current_val * modifier_amount),  current_val));
	}
}


void apply_scaled_modifier_values_to_nation(sys::state& state, dcon::nation_id target_nation, dcon::modifier_id mod_id,
		float scale) {
	auto& nat_values = state.world.modifier_get_national_values(mod_id);
	for(uint32_t i = 0; i < sys::national_modifier_definition::modifier_definition_size; ++i) {
		if(!(nat_values.offsets[i]))
			break; // no more modifier values

		auto fixed_offset = nat_values.offsets[i];
		auto modifier_amount = nat_values.values[i];
		const auto& metadata = sys::national_modifier_metadata[fixed_offset.index()];
		auto current_val = state.world.nation_get_modifier_values(target_nation, fixed_offset);
		float new_val = (metadata.op == sys::modifier_operation::add ? current_val + modifier_amount * scale : current_val * modifier_amount * scale);
		state.world.nation_set_modifier_values(target_nation, fixed_offset, new_val);
	}
}
template<typename F, concepts::dcon_id_ve_type<dcon::nation_id> nation_ids>
void ve_apply_scaled_modifier_values_to_nation(sys::state& state, nation_ids target_nations, dcon::modifier_id mod_id, ve::mask_vector apply_mask, F&& scale_func) {
	auto& nat_values = state.world.modifier_get_national_values(mod_id);
	auto scaling_factor = scale_func(target_nations);
	for(uint32_t i = 0; i < sys::national_modifier_definition::modifier_definition_size; ++i) {
		if(!(nat_values.offsets[i]))
			break; // no more modifier values

		auto fixed_offset = nat_values.offsets[i];
		auto modifier_amount = nat_values.values[i];
		const auto& metadata = sys::national_modifier_metadata[fixed_offset.index()];
		auto current_val = state.world.nation_get_modifier_values(target_nations, fixed_offset);
		state.world.nation_set_modifier_values(target_nations, fixed_offset, ve::select(apply_mask, (metadata.op == sys::modifier_operation::add ? current_val + modifier_amount * scaling_factor : current_val * modifier_amount * scaling_factor), current_val));
	}
}


void apply_modifier_values_to_province(sys::state& state, dcon::province_id target_prov, dcon::modifier_id mod_id) {
	auto& prov_values = state.world.modifier_get_province_values(mod_id);
	auto owner = state.world.province_get_nation_from_province_ownership(target_prov);
	for(uint32_t i = 0; i < sys::provincial_modifier_definition::modifier_definition_size; ++i) {
		if(!(prov_values.offsets[i]))
			break; // no more modifier values

		auto fixed_offset = prov_values.offsets[i];
		auto modifier_amount = prov_values.values[i];
		const auto& metadata = sys::province_modifier_metadata[fixed_offset.index()];
		auto current_val = state.world.province_get_modifier_values(target_prov, fixed_offset);
		float new_val = (metadata.op == sys::modifier_operation::add ? current_val + modifier_amount : current_val * modifier_amount);
		state.world.province_set_modifier_values(target_prov, fixed_offset, new_val);
	}
	//if(owner) {
	//	auto& nat_values = state.world.modifier_get_national_values(mod_id);
	//	for(uint32_t i = 0; i < sys::national_modifier_definition::modifier_definition_size; ++i) {
	//		if(!(nat_values.offsets[i]))
	//			break; // no more modifier values

	//		auto fixed_offset = nat_values.offsets[i];
	//		auto modifier_amount = nat_values.values[i];
	//		const auto& metadata = sys::national_modifier_metadata[fixed_offset.index()];
	//		auto current_val = state.world.nation_get_modifier_values(owner, fixed_offset);

	//		float new_val = (metadata.op == sys::modifier_operation::add ? current_val + modifier_amount : current_val * modifier_amount);
	//		state.world.nation_set_modifier_values(owner, fixed_offset, new_val);
	//	}
	//}
}

template<concepts::dcon_id_ve_type<dcon::province_id> prov_ids>
void ve_apply_modifier_values_to_province(sys::state& state, prov_ids target_provs, dcon::modifier_id mod_id) {
	auto& prov_values = state.world.modifier_get_province_values(mod_id);
	for(uint32_t i = 0; i < sys::provincial_modifier_definition::modifier_definition_size; ++i) {
		if(!(prov_values.offsets[i]))
			break; // no more modifier values

		auto fixed_offset = prov_values.offsets[i];
		auto modifier_amount = prov_values.values[i];
		const auto& metadata = sys::province_modifier_metadata[fixed_offset.index()];
		auto current_val = state.world.province_get_modifier_values(target_provs, fixed_offset);
		float new_val = (metadata.op == sys::modifier_operation::add ? current_val + modifier_amount : current_val * modifier_amount);
		state.world.province_set_modifier_values(target_provs, fixed_offset, new_val);
	}
}
template<typename F, concepts::dcon_id_ve_type<dcon::province_id> prov_ids>
void ve_apply_masked_modifier_values_to_province(sys::state& state, prov_ids target_provs, dcon::modifier_id mod_id, ve::mask_vector apply_mask) {
	auto& prov_values = state.world.modifier_get_province_values(mod_id);
	for(uint32_t i = 0; i < sys::provincial_modifier_definition::modifier_definition_size; ++i) {
		if(!(prov_values.offsets[i]))
			break; // no more modifier values

		auto fixed_offset = prov_values.offsets[i];
		auto modifier_amount = prov_values.values[i];
		const auto& metadata = sys::province_modifier_metadata[fixed_offset.index()];
		auto current_val = state.world.province_get_modifier_values(target_provs, fixed_offset);
		float new_val = (metadata.op == sys::modifier_operation::add ? current_val + modifier_amount : current_val * modifier_amount);
		state.world.province_set_modifier_values(target_provs, fixed_offset, ve::select(apply_mask, (metadata.op == sys::modifier_operation::add ? current_val + modifier_amount : current_val * modifier_amount), current_val));
	}
}
template<typename F, concepts::dcon_id_ve_type<dcon::province_id> prov_ids>
void ve_apply_scaled_modifier_values_to_province(sys::state& state, prov_ids target_provs, dcon::modifier_id mod_id, ve::mask_vector apply_mask, F&& scaling_func) {
	auto& prov_values = state.world.modifier_get_province_values(mod_id);
	auto scaling_factor = mask_func(target_provs);
	for(uint32_t i = 0; i < sys::provincial_modifier_definition::modifier_definition_size; ++i) {
		if(!(prov_values.offsets[i]))
			break; // no more modifier values

		auto fixed_offset = prov_values.offsets[i];
		auto modifier_amount = prov_values.values[i];
		const auto& metadata = sys::province_modifier_metadata[fixed_offset.index()];
		auto current_val = state.world.province_get_modifier_values(target_provs, fixed_offset);
		float new_val = (metadata.op == sys::modifier_operation::add ? current_val + modifier_amount : current_val * modifier_amount);
		state.world.province_set_modifier_values(target_provs, fixed_offset, ve::select(apply_mask, (metadata.op == sys::modifier_operation::add ? current_val + modifier_amount * scaling_factor : current_val * modifier_amount * scaling_factor), current_val));
	}
}


void apply_scaled_modifier_values_to_province(sys::state& state, dcon::province_id target_prov, dcon::modifier_id mod_id, float scale) {
	auto& prov_values = state.world.modifier_get_province_values(mod_id);
	for(uint32_t i = 0; i < sys::provincial_modifier_definition::modifier_definition_size; ++i) {
		if(!(prov_values.offsets[i]))
			break; // no more modifier values

		auto fixed_offset = prov_values.offsets[i];
		auto modifier_amount = prov_values.values[i];
		const auto& metadata = sys::province_modifier_metadata[fixed_offset.index()];
		auto current_val = state.world.province_get_modifier_values(target_prov, fixed_offset);
		float new_val = (metadata.op == sys::modifier_operation::add ? current_val + modifier_amount * scale : current_val * modifier_amount * scale);
		state.world.province_set_modifier_values(target_prov, fixed_offset, new_val);
	}
}


void add_modifier_to_nation(sys::state& state, dcon::nation_id target_nation, dcon::modifier_id mod_id, sys::date expiration) {
	assert(state.world.nation_is_valid(target_nation) && "Invalid write incoming!");
	auto lst = state.world.nation_get_current_modifiers(target_nation);
	for(auto& m : lst) {
		if(m.mod_id == mod_id) {
			if(!expiration || (m.expiration && m.expiration < expiration)) {
				m.expiration = expiration;
			}
			return;
		}
	}
	lst.push_back(sys::dated_modifier{expiration, mod_id});
}
void add_modifier_to_province(sys::state& state, dcon::province_id target_prov, dcon::modifier_id mod_id, sys::date expiration) {
	assert(state.world.province_is_valid(target_prov) && "Invalid write incoming!");
	auto lst = state.world.province_get_current_modifiers(target_prov);
	for(auto& m : lst) {
		if(m.mod_id == mod_id) {
			if(!expiration || (m.expiration && m.expiration < expiration)) {
				m.expiration = expiration;
			}
			return;
		}
	}
	lst.push_back(sys::dated_modifier{expiration, mod_id});
}
void remove_modifier_from_nation(sys::state& state, dcon::nation_id target_nation, dcon::modifier_id mod_id) {
	auto modifiers_range = state.world.nation_get_current_modifiers(target_nation);
	auto count = modifiers_range.size();
	for(uint32_t i = count; i-- > 0;) {
		if(modifiers_range.at(i).mod_id == mod_id) {
			modifiers_range.remove_at(i);
			return;
		}
	}
}

void remove_modifier_from_province(sys::state& state, dcon::province_id target_prov, dcon::modifier_id mod_id) {
	auto modifiers_range = state.world.province_get_current_modifiers(target_prov);
	auto count = modifiers_range.size();
	for(uint32_t i = count; i-- > 0;) {
		if(modifiers_range.at(i).mod_id == mod_id) {
			modifiers_range.remove_at(i);
			return;
		}
	}
}

void toggle_modifier_from_province(sys::state& state, dcon::province_id target_prov, dcon::modifier_id mod_id, sys::date expiration) {
	assert(state.world.province_is_valid(target_prov) && "Invalid write incoming!");
	auto lst = state.world.province_get_current_modifiers(target_prov);
	auto modifiers_range = state.world.province_get_current_modifiers(target_prov);
	auto count = modifiers_range.size();
	for(uint32_t i = count; i-- > 0;) {
		if(modifiers_range.at(i).mod_id == mod_id) {
			modifiers_range.remove_at(i);
			return;
		}
	}
	lst.push_back(sys::dated_modifier{ expiration, mod_id });
}

template<typename F>
void bulk_apply_masked_modifier_to_nations(sys::state& state, dcon::modifier_id m, F const& mask_functor) {
	auto& nat_values = state.world.modifier_get_national_values(m);
	for(uint32_t i = 0; i < sys::national_modifier_definition::modifier_definition_size; ++i) {
		if(!(nat_values.offsets[i]))
			break; // no more modifier values attached

		auto fixed_offset = nat_values.offsets[i];
		auto modifier_amount = nat_values.values[i];
		const auto& metadata = sys::national_modifier_metadata[fixed_offset.index()];
		state.world.execute_serial_over_nation(
				[&](auto nation_indices) {
					auto has_mod_mask = mask_functor(nation_indices);
					auto old_mod_value = state.world.nation_get_modifier_values(nation_indices, fixed_offset);
					auto new_mod_value = (metadata.op == sys::modifier_operation::add ? old_mod_value + modifier_amount : old_mod_value * modifier_amount);
					state.world.nation_set_modifier_values(nation_indices, fixed_offset,
							ve::select(has_mod_mask, new_mod_value, old_mod_value));
				});
	}
}

template<typename F>
void bulk_apply_scaled_modifier_to_nations(sys::state& state, dcon::modifier_id m, F const& scale_functor) {
	auto& nat_values = state.world.modifier_get_national_values(m);
	for(uint32_t i = 0; i < sys::national_modifier_definition::modifier_definition_size; ++i) {
		if(!(nat_values.offsets[i]))
			break; // no more modifier values attached


		auto fixed_offset = nat_values.offsets[i];
		auto modifier_amount = nat_values.values[i];
		const auto& metadata = sys::national_modifier_metadata[fixed_offset.index()];
		state.world.execute_serial_over_nation(
				[&](auto nation_indices) {
					auto scaling_factor = scale_functor(nation_indices);
					auto old_mod_value = state.world.nation_get_modifier_values(nation_indices, fixed_offset);
					auto new_mod_values = (metadata.op == sys::modifier_operation::add ? old_mod_value + modifier_amount * scaling_factor : old_mod_value * modifier_amount * scaling_factor);
					state.world.nation_set_modifier_values(nation_indices, fixed_offset, new_mod_values);
				});
	}
}

template<typename F>
void bulk_apply_masked_modifier_to_provinces(sys::state& state, dcon::modifier_id mod_id, F const& mask_functor) {
	auto& prov_values = state.world.modifier_get_province_values(mod_id);

	for(uint32_t i = 0; i < sys::provincial_modifier_definition::modifier_definition_size; ++i) {
		if(!(prov_values.offsets[i]))
			break; // no more modifier values

		auto fixed_offset = prov_values.offsets[i];
		auto modifier_amount = prov_values.values[i];
		const auto& metadata = sys::province_modifier_metadata[fixed_offset.index()];
		state.world.execute_serial_over_province([&](auto ids) {
			auto has_mod_mask = mask_functor(ids);
			auto old_value = state.world.province_get_modifier_values(ids, fixed_offset);
			auto new_val = (metadata.op == sys::modifier_operation::add ? old_value + modifier_amount : old_value * modifier_amount);
			state.world.province_set_modifier_values(ids, fixed_offset,
					ve::select(has_mod_mask, new_val, old_value));
		});
	}

	auto& nat_values = state.world.modifier_get_national_values(mod_id);

	for(uint32_t i = 0; i < sys::national_modifier_definition::modifier_definition_size; ++i) {
		if(!(nat_values.offsets[i]))
			break; // no more modifier values

		auto fixed_offset = nat_values.offsets[i];
		auto modifier_amount = nat_values.values[i];
		const auto& metadata = sys::national_modifier_metadata[fixed_offset.index()];

		province::ve_for_each_land_province(state, [&](auto ids) {
			auto owners = state.world.province_get_nation_from_province_ownership(ids);
			auto has_mod_mask = mask_functor(ids) && (owners != dcon::nation_id{});

			auto old_value = state.world.nation_get_modifier_values(owners, fixed_offset);
			auto new_val = (metadata.op == sys::modifier_operation::add ? old_value + modifier_amount : old_value * modifier_amount);
			state.world.nation_set_modifier_values(owners, fixed_offset,
					ve::select(has_mod_mask, new_val, old_value));
		});
	}
}

void bulk_apply_modifier_to_provinces(sys::state& state, dcon::modifier_id mod_id) {
	auto& prov_values = state.world.modifier_get_province_values(mod_id);

	for(uint32_t i = 0; i < sys::provincial_modifier_definition::modifier_definition_size; ++i) {
		if(!(prov_values.offsets[i]))
			break; // no more modifier values

		auto fixed_offset = prov_values.offsets[i];
		auto modifier_amount = prov_values.values[i];

		const auto& metadata = sys::province_modifier_metadata[fixed_offset.index()];

		state.world.execute_serial_over_province([&](auto ids) {
			auto old_value = state.world.province_get_modifier_values(ids, fixed_offset);
			auto new_val = (metadata.op == sys::modifier_operation::add ? old_value + modifier_amount : old_value * modifier_amount);
			state.world.province_set_modifier_values(ids, fixed_offset, new_val);
		});
	}
}

template<typename F>
void bulk_apply_scaled_modifier_to_provinces(sys::state& state, dcon::modifier_id mod_id, F const& scale_functor) {
	auto& prov_values = state.world.modifier_get_province_values(mod_id);

	for(uint32_t i = 0; i < sys::provincial_modifier_definition::modifier_definition_size; ++i) {
		if(!(prov_values.offsets[i]))
			break; // no more modifier values

		auto fixed_offset = prov_values.offsets[i];
		auto modifier_amount = prov_values.values[i];

		const auto& metadata = sys::province_modifier_metadata[fixed_offset.index()];

		state.world.execute_serial_over_province([&](auto ids) {
			auto scale = scale_functor(ids);
			auto old_value = state.world.province_get_modifier_values(ids, fixed_offset);
			auto new_val = (metadata.op == sys::modifier_operation::add ? old_value + modifier_amount * scale : old_value * modifier_amount * scale);
			state.world.province_set_modifier_values(ids, fixed_offset, new_val);
		});
	}

	auto& nat_values = state.world.modifier_get_national_values(mod_id);

	for(uint32_t i = 0; i < sys::national_modifier_definition::modifier_definition_size; ++i) {
		if(!(nat_values.offsets[i]))
			break; // no more modifier values

		auto fixed_offset = nat_values.offsets[i];
		auto modifier_amount = nat_values.values[i];

		const auto& metadata = sys::national_modifier_metadata[fixed_offset.index()];

		province::ve_for_each_land_province(state, [&](auto ids) {
			auto owners = state.world.province_get_nation_from_province_ownership(ids);
			auto scale = ve::select(owners != dcon::nation_id{}, scale_functor(ids), 0.0f);

			auto old_value = state.world.nation_get_modifier_values(owners, fixed_offset);
			auto new_val = (metadata.op == sys::modifier_operation::add ? old_value + modifier_amount * scale : old_value * modifier_amount * scale);
			state.world.nation_set_modifier_values(owners, fixed_offset, new_val);
		});
	}
}

void recreate_national_modifiers(sys::state& state) {

	ve::execute_parallel<dcon::nation_id>(state.world.nation_size(), [&](auto nations) {

		ve::mask_vector nations_valid = ve::apply([&](dcon::nation_id nation) {
			return state.world.nation_is_valid(nation);
		}, nations);

		for(uint32_t i = 0; i < sys::national_mod_offsets::count; i++) {
			dcon::national_modifier_value mid{ dcon::national_modifier_value::value_base_t(i) };
			float prev_val = state.world.nation_get_modifier_values(nations, mid);
			float start_val = sys::national_modifier_metadata[mid.index()].start_value;
			state.world.nation_set_modifier_values(nations, mid, ve::select(nations_valid, start_val, prev_val));
		}


		ve::apply([&](dcon::nation_id nation, bool is_valid) {
			if(is_valid) {
				for(auto mpr : state.world.nation_get_current_modifiers(nation)) {
					apply_modifier_values_to_nation(state, nation, mpr.mod_id);
				}
				if(auto ts = state.world.nation_get_tech_school(nation); ts) {
					apply_modifier_values_to_nation(state, nation, ts);
				}
				if(auto nv = state.world.nation_get_national_value(nation); nv) {
					apply_modifier_values_to_nation(state, nation, nv);
				}
				if(auto rgmd = state.world.religion_get_nation_modifier(state.world.nation_get_religion(nation)); rgmd) {
					// Apply only when state religion is majority religion. The function is called once per month
					if(state.world.nation_get_dominant_religion(nation) == state.world.nation_get_religion(nation)) {
						apply_modifier_values_to_nation(state, nation, rgmd);
					}
				}
				state.world.for_each_issue([&](dcon::issue_id i) {
					auto iopt = state.world.nation_get_issues(nation, i);
					auto imod = state.world.issue_option_get_modifier(iopt);
					if(imod && (state.world.nation_get_is_civilized(nation) || state.world.issue_get_issue_type(i) == uint8_t(culture::issue_type::party))) {
						apply_modifier_values_to_nation(state, nation, imod);
					}
				});
				state.world.for_each_reform([&](dcon::reform_id i) {
					auto iopt = state.world.nation_get_reforms(nation, i);
					auto imod = state.world.reform_option_get_modifier(iopt);
					if(imod && !state.world.nation_get_is_civilized(nation)) {
						apply_modifier_values_to_nation(state, nation, imod);
					}
				});

				if(state.national_definitions.fastest_land_unit_speed) {
					auto fastest_land_unit = state.world.nation_get_fastest_unlocked_land_unit(nation);
					auto spd = state.world.nation_get_unit_stats(nation, fastest_land_unit).maximum_speed;
					apply_scaled_modifier_values_to_nation(state, nation, state.national_definitions.fastest_land_unit_speed, spd);
				}
				if(state.national_definitions.fastest_transport_unit_speed) {
					auto fastest_transport_unit = state.world.nation_get_fastest_unlocked_transport_unit(nation);
					auto spd = state.world.nation_get_unit_stats(nation, fastest_transport_unit).maximum_speed;
					apply_scaled_modifier_values_to_nation(state, nation, state.national_definitions.fastest_transport_unit_speed, spd);
				}
				if(state.national_definitions.disarming) {
					if(bool(state.world.nation_get_disarmed_until(nation)) && state.world.nation_get_disarmed_until(nation) > state.current_date) {
						apply_modifier_values_to_nation(state, nation, state.national_definitions.disarming);
					}
				}

			}

		}, nations, nations_valid);

		state.world.for_each_technology([&](dcon::technology_id tech) {
			auto tmod = state.world.technology_get_modifier(tech);
			if(tmod) {
				ve_apply_masked_modifier_values_to_nation(state, nations, tmod, [&](auto ids) {  return nations_valid && state.world.nation_get_active_technologies(ids, tech); });
			}
		});

		state.world.for_each_invention([&](dcon::invention_id invention) {
			auto imod = state.world.invention_get_modifier(invention);
			if(imod) {
				ve_apply_masked_modifier_values_to_nation(state, nations, imod, [&](auto ids) {  return nations_valid && state.world.nation_get_active_inventions(ids, invention); });
			}
		});

		if(state.national_definitions.war) {
			ve_apply_masked_modifier_values_to_nation(state, nations, state.national_definitions.war, [&](auto ids) { return nations_valid && state.world.nation_get_is_at_war(ids); });
		}

		if(state.national_definitions.peace) {
			ve_apply_masked_modifier_values_to_nation(state, nations, state.national_definitions.peace, [&](auto ids) { return nations_valid && !state.world.nation_get_is_at_war(ids); });
		}

		if(state.national_definitions.nation_base) {
			ve_apply_masked_modifier_values_to_nation(state, nations, state.national_definitions.nation_base, [&](auto ids) { return nations_valid; });
		}
		if(state.national_definitions.badboy) {
			ve_apply_scaled_modifier_values_to_nation(state, nations, state.national_definitions.badboy, nations_valid , [&](auto ids) { return state.world.nation_get_infamy(ids); });
		}
		if(state.national_definitions.plurality) {
			ve_apply_scaled_modifier_values_to_nation(state, nations, state.national_definitions.plurality, nations_valid,[&](auto ids) { return state.world.nation_get_plurality(ids); });
		}
		if(state.national_definitions.war_exhaustion) {
			ve_apply_scaled_modifier_values_to_nation(state, nations, state.national_definitions.war_exhaustion, nations_valid,[&](auto ids) { return state.world.nation_get_war_exhaustion(ids); });
		}
		if(state.national_definitions.average_literacy) {
			ve_apply_scaled_modifier_values_to_nation(state, nations, state.national_definitions.average_literacy, nations_valid, [&](auto ids) {
				auto total = state.world.nation_get_demographics(ids, demographics::non_colonial_total);
				return ve::select(total > 0, state.world.nation_get_demographics(ids, demographics::non_colonial_literacy) / total, 0.0f);
			});
		}
		if(state.national_definitions.total_blockaded) {
			ve_apply_scaled_modifier_values_to_nation(state, nations, state.national_definitions.total_blockaded, nations_valid, [&](auto ids) {
				auto bc = ve::to_float(state.world.nation_get_central_blockaded(ids));
				auto c = ve::to_float(state.world.nation_get_central_ports(ids));
				return ve::select(c > 0.0f, bc / c, 0.0f);
			});
		}

		if(state.national_definitions.total_occupation) {
			ve_apply_scaled_modifier_values_to_nation(state, nations, state.national_definitions.total_occupation, nations_valid, [&](auto ids) {
				return ve::apply(
						[&](dcon::nation_id nid) {
								auto n = fatten(state.world, nid);
								auto cap_continent = n.get_capital().get_continent();
								float total = 0.0f;
								float occupied = 0.0f;
								for(auto owned : n.get_province_ownership()) {
									if(owned.get_province().get_continent() == cap_continent) {
										total += 1.0f;
										if(auto c = owned.get_province().get_nation_from_province_control().id; c && c != n.id) {
											occupied += 1.0f;
										}
									}
								}
								if(total > 0.0f)
									return 100.0f * occupied / total;
								else
									return 0.0f;
						},
						ids);
			});
		}

		if(state.national_definitions.unciv_nation) {
			ve_apply_masked_modifier_values_to_nation(state, nations, state.national_definitions.unciv_nation, [&](auto ids) { return nations_valid && !state.world.nation_get_is_civilized(ids); });
		}
		if(state.national_definitions.great_power) {
			ve_apply_masked_modifier_values_to_nation(state, nations, state.national_definitions.great_power, [&](auto ids) { return nations_valid && state.world.nation_get_is_great_power(ids); });
		}
		if(state.national_definitions.second_power) {
			ve_apply_masked_modifier_values_to_nation(state, nations, state.national_definitions.second_power, [&](auto ids) { return nations_valid && state.world.nation_get_rank(ids) <= uint16_t(state.defines.colonial_rank) && !state.world.nation_get_is_great_power(ids); });
		}
		if(state.national_definitions.civ_nation) {
			ve_apply_masked_modifier_values_to_nation(state, nations, state.national_definitions.civ_nation, [&](auto ids) {
				return nations_valid && state.world.nation_get_rank(ids) > uint16_t(state.defines.colonial_rank) && state.world.nation_get_is_civilized(ids);
			});
		}
		if(state.national_definitions.in_bankrupcy) {
			ve_apply_masked_modifier_values_to_nation(state, nations, state.national_definitions.in_bankrupcy,
					[&](auto ids) { return nations_valid && state.world.nation_get_is_bankrupt(ids); });
		}

		for(auto tm : state.national_definitions.triggered_modifiers) {
			if(tm.trigger_condition && tm.linked_modifier) {

				auto trigger_condition_satisfied =
					nations_valid && trigger::evaluate(state, tm.trigger_condition, trigger::to_generic(nations), trigger::to_generic(nations), 0);
				auto compressed_res = ve::compress_mask(trigger_condition_satisfied);
				if(compressed_res.v == ve::vbitfield_type::storage(0)) {
					return;
				} else {
					ve_apply_masked_modifier_values_to_nation(state, nations, tm.linked_modifier, [&](auto ids) { return nations_valid && trigger_condition_satisfied; });
				}
			}
		}
		ve_apply_hardcoded_modifier_values_to_nation(state, nations, nations_valid);
	});

}

void update_single_nation_modifiers(sys::state& state, dcon::nation_id n) {

	for(uint32_t i = uint32_t(0); i < sys::national_mod_offsets::count; ++i) {
		dcon::national_modifier_value mid{dcon::national_modifier_value::value_base_t(i)};
		float start_val = sys::national_modifier_metadata[mid.index()].start_value;
		state.world.nation_set_modifier_values(n, mid, start_val);
	}

	if(auto ts = state.world.nation_get_tech_school(n); ts)
		apply_modifier_values_to_nation(state, n, ts);

	if(auto nv = state.world.nation_get_national_value(n); nv)
		apply_modifier_values_to_nation(state, n, nv);

	for(auto mpr : state.world.nation_get_current_modifiers(n)) {
		apply_modifier_values_to_nation(state, n, mpr.mod_id);
	}

	state.world.for_each_technology([&](dcon::technology_id t) {
		auto tmod = state.world.technology_get_modifier(t);
		if(tmod && state.world.nation_get_active_technologies(n, t)) {
			apply_modifier_values_to_nation(state, n, tmod);
		}
	});
	state.world.for_each_invention([&](dcon::invention_id i) {
		auto tmod = state.world.invention_get_modifier(i);
		if(tmod && state.world.nation_get_active_inventions(n, i)) {
			apply_modifier_values_to_nation(state, n, tmod);
		}
	});
	state.world.for_each_issue([&](dcon::issue_id i) {
		auto iopt = state.world.nation_get_issues(n, i);
		auto imod = state.world.issue_option_get_modifier(iopt);
		if(imod &&
				(state.world.nation_get_is_civilized(n) || state.world.issue_get_issue_type(i) == uint8_t(culture::issue_type::party))) {
			apply_modifier_values_to_nation(state, n, imod);
		}
	});
	if(!state.world.nation_get_is_civilized(n)) {
		state.world.for_each_reform([&](dcon::reform_id i) {
			auto iopt = state.world.nation_get_reforms(n, i);
			auto imod = state.world.reform_option_get_modifier(iopt);
			if(imod) {
				apply_modifier_values_to_nation(state, n, imod);
			}
		});
	}

	if(state.national_definitions.nation_base) {
		apply_modifier_values_to_nation(state, n, state.national_definitions.nation_base);
	}

	auto in_wars = state.world.nation_get_war_participant(n);
	if(in_wars.begin() != in_wars.end()) {
		if(state.national_definitions.war)
			apply_modifier_values_to_nation(state, n, state.national_definitions.war);
	} else {
		if(state.national_definitions.peace)
			apply_modifier_values_to_nation(state, n, state.national_definitions.peace);
	}

	if(state.national_definitions.badboy) {
		apply_scaled_modifier_values_to_nation(state, n, state.national_definitions.badboy, state.world.nation_get_infamy(n));
	}
	if(state.national_definitions.plurality) {
		apply_scaled_modifier_values_to_nation(state, n, state.national_definitions.plurality, state.world.nation_get_plurality(n));
	}
	if(state.national_definitions.war_exhaustion) {
		apply_scaled_modifier_values_to_nation(state, n, state.national_definitions.war_exhaustion,
				state.world.nation_get_war_exhaustion(n));
	}
	if(state.national_definitions.average_literacy) {
		auto literacy = nations::get_avg_non_colonial_literacy(state, n);
		apply_scaled_modifier_values_to_nation(state, n, state.national_definitions.average_literacy, literacy);
	}
	if(state.national_definitions.total_blockaded) {
		auto bc = ve::to_float(state.world.nation_get_central_blockaded(n));
		auto c = ve::to_float(state.world.nation_get_central_ports(n));

		apply_scaled_modifier_values_to_nation(state, n, state.national_definitions.total_blockaded, c > 0.0f ? bc / c : 0.0f);
	}
	if(state.national_definitions.total_occupation) {
		auto nid = fatten(state.world, n);
		auto cap_continent = nid.get_capital().get_continent();
		float total = 0.0f;
		float occupied = 0.0f;
		for(auto owned : nid.get_province_ownership()) {
			if(owned.get_province().get_continent() == cap_continent) {
				total += 1.0f;
				if(auto c = owned.get_province().get_nation_from_province_control().id; c && c != n) {
					occupied += 1.0f;
				}
			}
		}

		apply_scaled_modifier_values_to_nation(state, n, state.national_definitions.total_occupation,
				total > 0.0f ? occupied / total : 0.0f);
	}

	if(state.world.nation_get_is_civilized(n) == false) {
		if(state.national_definitions.unciv_nation)
			apply_modifier_values_to_nation(state, n, state.national_definitions.unciv_nation);
	} else if(nations::is_great_power(state, n)) {
		if(state.national_definitions.great_power)
			apply_modifier_values_to_nation(state, n, state.national_definitions.great_power);
	} else if(state.world.nation_get_rank(n) <= uint16_t(state.defines.colonial_rank)) {
		if(state.national_definitions.second_power)
			apply_modifier_values_to_nation(state, n, state.national_definitions.second_power);
	} else {
		if(state.national_definitions.civ_nation)
			apply_modifier_values_to_nation(state, n, state.national_definitions.civ_nation);
	}

	if(state.national_definitions.disarming) {
		if(bool(state.world.nation_get_disarmed_until(n)) && state.world.nation_get_disarmed_until(n) > state.current_date) {
			apply_modifier_values_to_nation(state, n, state.national_definitions.disarming);
		}
	}
	if(state.national_definitions.in_bankrupcy) {
		if(bool(state.world.nation_get_is_bankrupt(n))) {
			apply_modifier_values_to_nation(state, n, state.national_definitions.in_bankrupcy);
		}
	}
	// TODO: debt

	for(auto tm : state.national_definitions.triggered_modifiers) {
		if(tm.trigger_condition && tm.linked_modifier) {

			auto trigger_condition_satisfied =
					trigger::evaluate(state, tm.trigger_condition, trigger::to_generic(n), trigger::to_generic(n), 0);

			if(trigger_condition_satisfied) {
				apply_modifier_values_to_nation(state, n, tm.linked_modifier);
			}
		}
	}

	if(state.national_definitions.fastest_land_unit_speed) {
		auto fastest_land_unit = state.world.nation_get_fastest_unlocked_land_unit(n);
		auto spd = state.world.nation_get_unit_stats(n, fastest_land_unit).maximum_speed;
		apply_scaled_modifier_values_to_nation(state, n, state.national_definitions.fastest_land_unit_speed, spd);
	}
	if(state.national_definitions.fastest_transport_unit_speed) {
		auto fastest_transport_unit = state.world.nation_get_fastest_unlocked_transport_unit(n);
		auto spd = state.world.nation_get_unit_stats(n, fastest_transport_unit).maximum_speed;
		apply_scaled_modifier_values_to_nation(state, n, state.national_definitions.fastest_transport_unit_speed, spd);
	}

	apply_hardcoded_modifier_values_to_nation(state, n);
}
template<typename T>
void recreate_single_shared_province_modifiers(sys::state& state, dcon::province_id province) {

	auto owner = state.world.province_get_nation_from_province_ownership(province);

	if constexpr(std::is_same_v<T, dcon::national_modifier_value>) {
		if(!owner) {
			return;
		}
	}

	auto add_modifier = [&](dcon::modifier_id modifier) {
		if constexpr(std::is_same_v<T, dcon::provincial_modifier_value>) {
			apply_modifier_values_to_province(state, province, modifier);
		}
		else if constexpr(std::is_same_v<T, dcon::national_modifier_value>) {
			apply_modifier_values_to_nation(state, owner, modifier);
		}
	};


	for(auto mpr : state.world.province_get_current_modifiers(province)) {
		add_modifier(mpr.mod_id);
	}
	if(auto m = state.world.province_get_terrain(province); m) {
		add_modifier(m);
	}
	if(auto m = state.world.province_get_climate(province); m) {
		add_modifier(m);
	}
	if(auto m = state.world.province_get_continent(province); m) {
		add_modifier(m);
	}
	

	if(state.national_definitions.province_base) {
		add_modifier(state.national_definitions.province_base);
	}
	apply_hardcoded_modifier_values_to_province(state, provinces, provinces_valid);
}

template<typename T>
void recreate_single_land_province_modifiers(sys::state& state, dcon::province_id province) {

	auto owner = state.world.province_get_nation_from_province_ownership(province);

	auto add_modifier = [&](dcon::modifier_id modifier) {
		if constexpr(std::is_same_v<T, dcon::provincial_modifier_value>) {
			apply_modifier_values_to_province(state, province, modifier);
		} else if constexpr(std::is_same_v<T, dcon::national_modifier_value>) {
			apply_modifier_values_to_nation(state, owner, modifier);
		}
	};

	if(state.national_definitions.land_province) {
		add_modifier(state.national_definitions.land_province);
	}
	for(auto t = economy::province_building_type::railroad; t != economy::province_building_type::last; t = economy::province_building_type(uint8_t(t) + 1)) {
		if(state.economy_definitions.building_definitions[int32_t(t)].province_modifier) {
			apply_scaled_modifier_values_to_province(state, province, state.economy_definitions.building_definitions[int32_t(t)].province_modifier, state.world.province_get_building_level(ids, uint8_t(t)));
			apply_scaled_modifier_values_to_nation()
		}
	}
	if(state.national_definitions.infrastructure) {
		ve_apply_scaled_modifier_values_to_province(state, provinces, state.national_definitions.infrastructure, [&](auto ids) {
			return ve::to_float(state.world.province_get_building_level(ids, uint8_t(economy::province_building_type::railroad))) *
				state.economy_definitions.building_definitions[int32_t(economy::province_building_type::railroad)].infrastructure;
		});
	}

	if(state.national_definitions.nationalism) {
		ve_apply_scaled_modifier_values_to_province(state, provinces, state.national_definitions.nationalism, provinces_valid, [&](auto ids) {
			return ve::select(state.world.province_get_is_owner_core(ids), 0.0f, state.world.province_get_nationalism(ids));
		});
	}
	if(state.national_definitions.non_coastal) {
		ve_apply_masked_modifier_values_to_province(state, provinces, state.national_definitions.non_coastal, provinces_valid && !state.world.province_get_is_coast(provinces));
	}
	if(state.national_definitions.coastal) {
		ve_apply_masked_modifier_values_to_province(state, provinces, state.national_definitions.coastal, provinces_valid && state.world.province_get_is_coast(provinces));
	}
	if(state.national_definitions.overseas) {
		ve_apply_masked_modifier_values_to_province(state, provinces, state.national_definitions.overseas, provinces_valid && province::is_overseas(state, ids));
	}
	if(state.national_definitions.core) {
		ve_apply_masked_modifier_values_to_province(state, provinces, state.national_definitions.core, provinces_valid && state.world.province_get_is_owner_core(ids));
	}
	if(state.national_definitions.has_siege) {
		ve_apply_masked_modifier_values_to_province(state, provinces, state.national_definitions.has_siege, provinces_valid && military::province_is_under_siege(state, ids));
	}
	if(state.national_definitions.blockaded) {
		ve_apply_masked_modifier_values_to_province(state, provinces, state.national_definitions.blockaded, provinces_valid && military::province_is_blockaded(state, ids));
	}
	if(state.national_definitions.province_militancy) {
		ve_apply_scaled_modifier_values_to_province(state, provinces, state.national_definitions.province_militancy, provinces_valid, [&](auto ids) {
			auto total_militancy = state.world.province_get_demographics(ids, demographics::militancy);
			auto total_pop = state.world.province_get_demographics(ids, demographics::total);
			return ve::select(total_pop == 0.0f, 0.0f, (total_militancy / total_pop) / 10.0f);

		});
	}
	if(state.national_definitions.province_control) {
		ve_apply_scaled_modifier_values_to_province(state, provinces, state.national_definitions.province_control, provinces_valid, [&](auto ids) {
			auto control_level = state.world.province_get_control_ratio(ids);
			return control_level / 1.0f;
		});
	}

	ve::apply([&](dcon::province_id prov, bool is_valid) {
		if(is_valid) {
			if(auto m = state.world.province_get_state_membership(prov).get_owner_focus(); m) {
				apply_modifier_values_to_province(state, p, m.get_modifier());
			}
			if(auto c = state.world.province_get_crime(prov); c) {
				if(auto m = state.culture_definitions.crimes[c].modifier; m) {
					apply_modifier_values_to_province(state, prov, m);
				}
			}
		}
	});

}

void recreate_province_modifiers(sys::state& state) {
	
	concurrency::parallel_for(uint32_t(0), sys::provincial_mod_offsets::count, [&](uint32_t i) {
		dcon::provincial_modifier_value mid{dcon::provincial_modifier_value::value_base_t(i)};
		state.world.execute_serial_over_province([&](auto ids) {
			float start_val = sys::province_modifier_metadata[mid.index()].start_value;
			state.world.province_set_modifier_values(ids, mid, start_val);
		});
	});

	// First, do modifier types which affect both sea and land provs
	state.world.execute_parallel_over_province([&](auto provinces) {
		ve::mask_vector provinces_valid = ve::apply([&](dcon::province_id prov) {
			return state.world.province_is_valid(prov);
		}, provinces);

		ve::apply([&](dcon::province_id prov, bool is_valid) {
			if(is_valid) {
				for(auto mpr : state.world.province_get_current_modifiers(prov)) {
					apply_modifier_values_to_province(state, prov, mpr.mod_id);
				}
				if(auto m = state.world.province_get_terrain(prov); m) {
					apply_modifier_values_to_province(state, prov, m);
				}
				if(auto m = state.world.province_get_climate(prov); m) {
					apply_modifier_values_to_province(state, prov, m);
				}
				if(auto m = state.world.province_get_continent(prov); m) {
					apply_modifier_values_to_province(state, prov, m);
				}		

			}
		}, provinces, provinces_valid);

		if(state.national_definitions.province_base) {
			ve_apply_masked_modifier_values_to_province(state, provinces, state.national_definitions.province_base, provinces_valid);
		}
		ve_apply_hardcoded_modifier_values_to_province(state, provinces, provinces_valid);

	});
	// Then modifiers for land-only provinces
	province::ve_parallel_for_each_land_province(state, [&](auto provinces) {
		ve::mask_vector provinces_valid = ve::apply([&](dcon::province_id prov) {
			return state.world.province_is_valid(prov);
		}, provinces);
		if(state.national_definitions.land_province) {
			ve_apply_masked_modifier_values_to_province(state, provinces, state.national_definitions.land_province, provinces_valid);
		}
		for(auto t = economy::province_building_type::railroad; t != economy::province_building_type::last; t = economy::province_building_type(uint8_t(t) + 1)) {
			if(state.economy_definitions.building_definitions[int32_t(t)].province_modifier) {
				ve_apply_scaled_modifier_values_to_province(state, provinces, state.economy_definitions.building_definitions[int32_t(t)].province_modifier, provinces_valid, [&](auto ids) { return ve::to_float(state.world.province_get_building_level(ids, uint8_t(t))); });
			}
		}
		if(state.national_definitions.infrastructure) {
			ve_apply_scaled_modifier_values_to_province(state, provinces, state.national_definitions.infrastructure, [&](auto ids) {
				return ve::to_float(state.world.province_get_building_level(ids, uint8_t(economy::province_building_type::railroad))) *
					state.economy_definitions.building_definitions[int32_t(economy::province_building_type::railroad)].infrastructure;
			});
		}

		if(state.national_definitions.nationalism) {
			ve_apply_scaled_modifier_values_to_province(state, provinces, state.national_definitions.nationalism, provinces_valid, [&](auto ids) {
				return ve::select(state.world.province_get_is_owner_core(ids), 0.0f, state.world.province_get_nationalism(ids));
			});
		}
		if(state.national_definitions.non_coastal) {
			ve_apply_masked_modifier_values_to_province(state, provinces, state.national_definitions.non_coastal, provinces_valid && !state.world.province_get_is_coast(provinces));
		}
		if(state.national_definitions.coastal) {
			ve_apply_masked_modifier_values_to_province(state, provinces, state.national_definitions.coastal, provinces_valid && state.world.province_get_is_coast(provinces));
		}
		if(state.national_definitions.overseas) {
			ve_apply_masked_modifier_values_to_province(state, provinces, state.national_definitions.overseas, provinces_valid && province::is_overseas(state, ids));
		}
		if(state.national_definitions.core) {
			ve_apply_masked_modifier_values_to_province(state, provinces, state.national_definitions.core, provinces_valid && state.world.province_get_is_owner_core(ids));
		}
		if(state.national_definitions.has_siege) {
			ve_apply_masked_modifier_values_to_province(state, provinces, state.national_definitions.has_siege, provinces_valid && military::province_is_under_siege(state, ids));
		}
		if(state.national_definitions.blockaded) {
			ve_apply_masked_modifier_values_to_province(state, provinces, state.national_definitions.blockaded, provinces_valid && military::province_is_blockaded(state, ids));
		}
		if(state.national_definitions.province_militancy) {
			ve_apply_scaled_modifier_values_to_province(state, provinces, state.national_definitions.province_militancy, provinces_valid, [&](auto ids) {
				auto total_militancy = state.world.province_get_demographics(ids, demographics::militancy);
				auto total_pop = state.world.province_get_demographics(ids, demographics::total);
				return ve::select(total_pop == 0.0f, 0.0f, (total_militancy / total_pop) / 10.0f);

			});
		}
		if(state.national_definitions.province_control) {
			ve_apply_scaled_modifier_values_to_province(state, provinces, state.national_definitions.province_control, provinces_valid, [&](auto ids) {
				auto control_level = state.world.province_get_control_ratio(ids);
				return control_level / 1.0f;
			});
		}

		ve::apply([&](dcon::province_id prov, bool is_valid) {
			if(is_valid) {
				if(auto m = state.world.province_get_state_membership(prov).get_owner_focus(); m) {
					apply_modifier_values_to_province(state, p, m.get_modifier());
				}
				if(auto c = state.world.province_get_crime(prov); c) {
					if(auto m = state.culture_definitions.crimes[c].modifier; m) {
						apply_modifier_values_to_province(state, prov, m);
					}
				}
			}
		});
	});



	/*if(state.national_definitions.province_base) {
		bulk_apply_modifier_to_provinces(state, state.national_definitions.province_base);
	}
	if(state.national_definitions.land_province) {
		province::for_each_land_province(state, [&](dcon::province_id prov) {
			apply_modifier_values_to_province(state, prov, state.national_definitions.land_province);
		});
	}
	if(state.national_definitions.sea_zone) {
		province::for_each_sea_province(state, [&](dcon::province_id prov) {
			apply_modifier_values_to_province(state, prov, state.national_definitions.sea_zone);
		});
	}

	state.world.for_each_province([&](dcon::province_id p) {
		for(auto mpr : state.world.province_get_current_modifiers(p)) {
			apply_modifier_values_to_province(state, p, mpr.mod_id);
		}
	});

	state.world.for_each_province([&](dcon::province_id p) {
		if(auto m = state.world.province_get_terrain(p); m)
			apply_modifier_values_to_province(state, p, m);
	});
	state.world.for_each_province([&](dcon::province_id p) {
		if(auto m = state.world.province_get_climate(p); m)
			apply_modifier_values_to_province(state, p, m);
	});
	state.world.for_each_province([&](dcon::province_id p) {
		if(auto m = state.world.province_get_continent(p); m)
			apply_modifier_values_to_province(state, p, m);
	});
	province::for_each_land_province(state, [&](dcon::province_id p) {
		if(auto m = state.world.province_get_state_membership(p).get_owner_focus(); m)
			apply_modifier_values_to_province(state, p, m.get_modifier());
	});
	province::for_each_land_province(state, [&](dcon::province_id p) {
		if(auto c = state.world.province_get_crime(p); c) {
			if(auto m = state.culture_definitions.crimes[c].modifier; m)
				apply_modifier_values_to_province(state, p, m);
		}
	});

	for(auto t = economy::province_building_type::railroad; t != economy::province_building_type::last; t = economy::province_building_type(uint8_t(t) + 1)) {
		if(state.economy_definitions.building_definitions[int32_t(t)].province_modifier) {
			bulk_apply_scaled_modifier_to_provinces(state, state.economy_definitions.building_definitions[int32_t(t)].province_modifier,
					[&](auto ids) { return ve::to_float(state.world.province_get_building_level(ids, uint8_t(t))); });
		}
	}
	if(state.national_definitions.infrastructure) {
		bulk_apply_scaled_modifier_to_provinces(state, state.national_definitions.infrastructure, [&](auto ids) {
			return ve::to_float(state.world.province_get_building_level(ids, uint8_t(economy::province_building_type::railroad))) *
						 state.economy_definitions.building_definitions[int32_t(economy::province_building_type::railroad)].infrastructure;
		});
	}
	if(state.national_definitions.nationalism) {
		bulk_apply_scaled_modifier_to_provinces(state, state.national_definitions.nationalism, [&](auto ids) {
			return ve::select(state.world.province_get_is_owner_core(ids), 0.0f, state.world.province_get_nationalism(ids));
		});
	}
	if(state.national_definitions.non_coastal) {
		bulk_apply_masked_modifier_to_provinces(state, state.national_definitions.non_coastal,
				[&](auto ids) { return !state.world.province_get_is_coast(ids); });
	}
	if(state.national_definitions.coastal) {
		bulk_apply_masked_modifier_to_provinces(state, state.national_definitions.coastal,
				[&](auto ids) { return state.world.province_get_is_coast(ids); });
	}
	if(state.national_definitions.overseas) {
		bulk_apply_masked_modifier_to_provinces(state, state.national_definitions.overseas,
				[&](auto ids) { return province::is_overseas(state, ids); });
	}
	if(state.national_definitions.core) {
		bulk_apply_masked_modifier_to_provinces(state, state.national_definitions.core,
				[&](auto ids) { return state.world.province_get_is_owner_core(ids); });
	}
	if(state.national_definitions.has_siege) {
		bulk_apply_masked_modifier_to_provinces(state, state.national_definitions.has_siege,
				[&](auto ids) { return military::province_is_under_siege(state, ids); });
	}
	if(state.national_definitions.blockaded) {
		bulk_apply_masked_modifier_to_provinces(state, state.national_definitions.blockaded,
				[&](auto ids) { return military::province_is_blockaded(state, ids); });
	}
	if(state.national_definitions.province_militancy) {
		bulk_apply_scaled_modifier_to_provinces(state, state.national_definitions.province_militancy, [&](auto ids) {
			auto total_militancy = state.world.province_get_demographics(ids, demographics::militancy);
			auto total_pop = state.world.province_get_demographics(ids, demographics::total);
			return ve::select(total_pop == 0.0f, 0.0f, (total_militancy / total_pop) / 10.0f);

		});
	}
	if(state.national_definitions.province_control) {
		bulk_apply_scaled_modifier_to_provinces(state, state.national_definitions.province_control, [&](auto ids) {
			auto control_level = state.world.province_get_control_ratio(ids);
			return control_level / 1.0f;
		});
	}
	state.world.for_each_province([&](dcon::province_id p) {
		apply_hardcoded_modifier_values_to_province(state, p);
	});*/
}

// removes province modifiers which has expired, should be used on daily update
void purge_expired_province_modifiers(sys::state& state) {
	// purge expired triggered modifiers
	province::for_each_land_province(state, [&](dcon::province_id p) {
		auto timed_modifiers = state.world.province_get_current_modifiers(p);
		for(uint32_t i = timed_modifiers.size(); i-- > 0;) {
			if(bool(timed_modifiers[i].expiration) && timed_modifiers[i].expiration < state.current_date) {
				timed_modifiers.remove_at(i);
			}
		}
	});
}

void purge_expired_national_province_modifiers(sys::state& state) {
	// purge expired triggered modifiers
	for(auto n : state.world.in_nation) {
		assert(n);
		auto timed_modifiers = n.get_current_modifiers();
		for(uint32_t i = timed_modifiers.size(); i-- > 0;) {
			if(bool(timed_modifiers[i].expiration) && timed_modifiers[i].expiration < state.current_date) {
				timed_modifiers.remove_at(i);
			}
		}
	}
}

// restores values after loading a save
void repopulate_modifier_effects(sys::state& state) {
	recreate_national_modifiers(state);
	recreate_province_modifiers(state);
}
// this is ran on update
void update_modifier_effects(sys::state& state) {
	purge_expired_national_province_modifiers(state);
	recreate_national_modifiers(state);
	purge_expired_province_modifiers(state);
	recreate_province_modifiers(state);
	for(auto n : state.world.in_nation) {
		economy::bound_budget_settings(state, n);
	}
}

} // namespace sys
