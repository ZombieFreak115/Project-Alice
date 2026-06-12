#pragma once
#include "system_state.hpp"
namespace supply_routes {

float max_supply_throughput(const sys::state& state, dcon::province_id province, dcon::nation_id nation_as);
float province_supply_attrition(const sys::state& state, dcon::province_id province, dcon::nation_id nation_as);
float adjacency_supply_attrition(const sys::state& state, dcon::province_adjacency_id province_adj, dcon::nation_id nation_as);
void update_supply_routes_daily(sys::state& state);
void update_supply_routes_monthly(sys::state& state);


}
