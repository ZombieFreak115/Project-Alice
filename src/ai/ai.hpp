#pragma once
#include "dcon_generated.hpp"
#include "container_types.hpp"
#include "text.hpp"

namespace ai {

struct ai_path_length {
	uint32_t length;
	bool operator==(const ai_path_length& other) const = default;
	bool operator!=(const ai_path_length& other) const = default;

};

enum class army_state : uint8_t {
	free = 0, // dosen't do anything
	protect_battles = 1, //will reinforce and protect battles from being encircled
	attack_land = 2, //will attack enemies on land
	attack_naval_invasion = 3, // will attack from the sea via naval invasion
	recover_strength = 4, // will try to recover strength & org
	advance = 5, // try advance and siege enemy provinces
	guard_border = 6, // will guard enemey borders, will not advance past
	retreat = 7, // will retreat battle
};

struct army_state_data {
	dcon::army_id actor;
};

struct army_state_basic_check_data {
	bool check_passed;
	army_state new_state;
};

army_state get_next_army_ai_state(const sys::state& state, army_state current, const army_state_data& data) {
	auto basic_check = army_basic_check(state, data);
	if(!basic_check.check_passed) {
		return basic_check.new_state;
	}

}

// State functions

army_state army_state_free(const sys::state& state, const army_state_data& data);
army_state army_state_protect_battles(const sys::state& state, const army_state_data& data);
army_state army_state_attack_land(const sys::state& state, const army_state_data& data);
army_state army_state_attack_naval_invasion(const sys::state& state, const army_state_data& data);
army_state army_state_recover_strength(const sys::state& state, const army_state_data& data);
army_state army_state_advance(const sys::state& state, const army_state_data& data);
army_state army_state_guard_border(const sys::state& state, const army_state_data& data);
army_state army_state_retreat(const sys::state& state, const army_state_data& data);


army_state_basic_check_data army_basic_check(const sys::state& state, const army_state_data& data);


void take_ai_decisions(sys::state& state);
void update_ai_ruling_party(sys::state& state);
void update_ai_colonial_investment(sys::state& state);
void update_ai_colony_starting(sys::state& state);
void upgrade_colonies(sys::state& state);
void civilize(sys::state& state);
void take_reforms(sys::state& state);
void remove_ai_data(sys::state& state, dcon::nation_id n);
void update_ships(sys::state& state);
void build_ships(sys::state& state);
void refresh_home_ports(sys::state& state);
void daily_cleanup(sys::state& state);
void move_idle_guards(sys::state& state);
void update_land_constructions(sys::state& state);
void update_naval_transport(sys::state& state);
void move_gathered_attackers(sys::state& state);
void gather_to_battle(sys::state& state, dcon::nation_id n, dcon::province_id p);
void make_attacks(sys::state& state);
void make_defense(sys::state& state);
void general_ai_unit_tick(sys::state& state);

float estimate_army_offensive_strength(sys::state& state, dcon::army_id a);
float estimate_army_defensive_strength(sys::state& state, dcon::army_id a);
float estimate_rebel_strength(sys::state& state, dcon::province_id p);

void update_ai_embargoes(sys::state& state);

}
