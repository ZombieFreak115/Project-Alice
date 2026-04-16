#include "system_state.hpp"
#include "ai_influence.hpp"
#include "economy_stats.hpp"
#include "demographics.hpp"
#include "commands.hpp"

namespace ai {

void update_influence_priorities(sys::state& state) {
	struct weighted_nation {
		dcon::nation_id id;
		float weight = 0.0f;
	};
	if(!nations::can_change_influence_priority_global_checks<command::actor::ai>(state)) {
		return;
	}

	for(auto gprl : state.world.in_gp_relationship) {
		if(gprl.get_great_power().get_is_player_controlled()) {
			// nothing -- player GP
		} else {
			// If we are allowed to, set all influence priorities to 0, except for spherelings which is set to 1
			if(nations::can_change_influence_priority_source_checks<command::actor::ai>(state, gprl.get_great_power()) && nations::can_change_influence_priority_target_checks<command::actor::ai>(state, gprl.get_great_power(), gprl.get_influence_target())) {
				auto status = gprl.get_status();
				if((status & nations::influence::level_mask) == nations::influence::level_in_sphere) {
					nations::change_influence_priority<1>(state, gprl.get_great_power(), gprl.get_influence_target());
				}
				else {
					nations::change_influence_priority<0>(state, gprl.get_great_power(), gprl.get_influence_target());
				}
			}
		}
	}

	for(auto& n : state.great_nations) {
		if(state.world.nation_get_is_player_controlled(n.nation))
			continue;
		// Can this nation influence anyone?
		if(!nations::can_change_influence_priority_source_checks<command::actor::ai>(state, n.nation)) {
			continue;
		}
		static std::vector<weighted_nation> targets;
		targets.clear();
		for(auto t : state.world.in_nation) {
			// Can we influence this nation?
			if(!nations::can_change_influence_priority_target_checks<command::actor::ai>(state, n.nation, t)) {
				continue;
			}
			// Would we like to influence this nation?
			if(t.get_in_sphere_of() == n.nation)
				continue;
			if(t.get_demographics(demographics::total) > state.defines.large_population_limit)
				continue;

			auto natid = state.world.nation_get_identity_from_identity_holder(n.nation);
			auto holdscores = false;
			for(auto prov_owner : state.world.nation_get_province_ownership(t)) {
				auto prov = prov_owner.get_province();

				for(auto core : prov.get_core_as_province()) {
					if(core.get_identity() == natid) {
						holdscores = true;
						break;
					}
				}

				if(holdscores)
					break;
			}

			if(holdscores) {
				continue; // holds our cores
			}

			float weight = 0.0f;

			for(auto c : state.world.in_commodity) {
				if(auto d = economy::demand(state, n.nation, c); d > 0.001f) {
					auto cweight = std::min(
						1.0f,
						economy::supply(state, n.nation, c) / d)
						* (1.0f - economy::demand_satisfaction(state, n.nation, c));
					weight += cweight;
				}
			}

			//We probably don't want to fight a forever lasting sphere war, let's find some other uncontested nations
			if(t.get_in_sphere_of()) {
				weight /= 4.0f;
			}

			//Prioritize primary culture before culture groups; should ensure Prussia spheres all of the NGF first before trying to contest Austria
			if(t.get_primary_culture() == state.world.nation_get_primary_culture(n.nation)) {
				weight += 1.0f;
				weight *= 4000.0f;
			}

			else if(t.get_primary_culture().get_group_from_culture_group_membership() == state.world.nation_get_primary_culture(n.nation).get_group_from_culture_group_membership()) {
				weight *= 4.0f;
			}
			//Focus on gaining influence against nations we have active wargoals against so we can remove their protector, even if it's us
			if(military::can_use_cb_against(state, n.nation, t) && t.get_in_sphere_of()) {
				weight += 1.0f;
				weight *= 1000.0f;
			}
			//If it doesn't neighbor us or a friendly sphere and isn't coastal, please don't sphere it, we don't want sphere gore
			bool is_reachable = false;
			for(auto adj : state.world.nation_get_nation_adjacency(t)) {
				auto casted_adj = adj.get_connected_nations(0) != t ? adj.get_connected_nations(0) : adj.get_connected_nations(1);
				if(casted_adj == n.nation) {
					is_reachable = true;
					break;
				}
				if(casted_adj.get_in_sphere_of() == n.nation) {
					is_reachable = true;
					break;
				}
			};

			//Is coastal? Technically reachable
			if(state.world.nation_get_central_ports(t) > 0) {
				is_reachable = true;
			}

			//Prefer neighbors
			if(state.world.get_nation_adjacency_by_nation_adjacency_pair(n.nation, t.id)) {
				weight *= 10.0f;
				is_reachable = true;
			}

			if(!is_reachable) {
				weight *= 0.0f;
			}

			targets.push_back(weighted_nation{ t.id, weight });
		}

		std::sort(targets.begin(), targets.end(), [](weighted_nation const& a, weighted_nation const& b) {
			if(a.weight != b.weight)
				return a.weight > b.weight;
			else
				return a.id.index() < b.id.index();
		});

		uint32_t i = 0;
		for(; i < 2 && i < targets.size(); ++i) {
			nations::change_influence_priority<3>(state, n.nation, targets[i].id);
		}
		for(; i < 4 && i < targets.size(); ++i) {
			nations::change_influence_priority<2>(state, n.nation, targets[i].id);
		}
		for(; i < 6 && i < targets.size(); ++i) {
			nations::change_influence_priority<1>(state, n.nation, targets[i].id);
		}
	}
}

dcon::gp_relationship_id get_highest_gp_influence_except_us(sys::state& state, dcon::nation_id great_power, dcon::nation_id influence_target) {
	dcon::gp_relationship_id best{}; 
	float highest = -1.0f;// start negative incase no one has any influence
	for(auto gp_rel : state.world.nation_get_gp_relationship_as_influence_target(influence_target)) {
		if(gp_rel.get_influence() > highest) {
			highest = gp_rel.get_influence();
			best = gp_rel.id;
		}
	}
	return best;
}


void perform_influence_actions(sys::state& state) {
	for(auto gprl : state.world.in_gp_relationship) {
		if(gprl.get_great_power().get_is_player_controlled()) {
			// nothing -- player GP
		} else {
			// Are we allowed to take any influence action?
			if(!nations::can_do_influence_action<command::actor::ai>(state, gprl.get_great_power(), gprl.get_influence_target(), gprl)) {
				continue;
			}
			auto great_power = gprl.get_great_power();
			auto influence_target = gprl.get_influence_target();

			auto current_sphere = gprl.get_influence_target().get_in_sphere_of();
			// If the target is in no sphere
			if(!current_sphere) {
				if(nations::can_increase_opinion_specific_checks<command::actor::ai>(state, great_power, influence_target, gprl)) {
					assert(nations::can_increase_opinion<command::actor::player>(state, great_power, influence_target));
					nations::increase_opinion(state, gprl);
				}
				else if(nations::can_add_to_sphere_specific_checks<command::actor::ai>(state, great_power, influence_target, gprl)) {
					assert(nations::can_add_to_sphere<command::actor::player>(state, great_power, influence_target));
					nations::add_to_sphere_diplo_action(state, great_power, influence_target);
				}
			}
			// if it's our sphere
			else if(current_sphere == great_power) {
				auto highest_infl = get_highest_gp_influence_except_us(state, great_power, influence_target);
				auto other_gp = state.world.gp_relationship_get_great_power(highest_infl);
				auto other_infl_target = state.world.gp_relationship_get_influence_target(highest_infl);
				// If the highest competing influence in our sphere is greater than 40 and they are friendly, try to ban them to stop them from taking our sphere
				if(state.world.gp_relationship_get_influence(highest_infl) > 40.0f && nations::influence::get_level(state, other_gp, other_infl_target) == nations::influence::level_friendly) {
					if(nations::can_ban_embassy_specific_checks<command::actor::ai>(state, great_power, influence_target, other_gp, gprl)) {
						nations::ban_embassy(state, great_power, influence_target, other_gp);
					}
				}
				//De-sphere countries we have wargoals against, desphering countries need to check for going over infamy
				else if(military::can_use_cb_against(state, great_power, influence_target) && (state.world.nation_get_infamy(great_power) + state.defines.removefromsphere_infamy_cost) < state.defines.badboy_limit) {
					if(nations::can_remove_from_sphere_specific_checks<command::actor::ai>(state, great_power, influence_target, gprl)) {
						assert(nations::can_remove_from_sphere<command::actor::player>(state, great_power, influence_target));
						nations::remove_from_sphere_diplo_action(state, great_power, influence_target);
					}
				}
				
				
			}
			// If it's someone else's sphere
			else {
				if(nations::can_remove_from_sphere_specific_checks<command::actor::ai>(state, great_power, influence_target, gprl)) {
					assert(nations::can_remove_from_sphere<command::actor::player>(state, great_power, influence_target));
					// Remove sphere from another GP
					nations::remove_from_sphere_diplo_action(state, great_power, influence_target);
				}
				if(nations::can_increase_opinion_specific_checks<command::actor::ai>(state, great_power, influence_target, gprl)) {
					assert(nations::can_increase_opinion<command::actor::player>(state, great_power, influence_target));
					nations::increase_opinion(state, gprl);
				}
			}
		}
	}
}
}
