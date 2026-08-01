#pragma once
#include "system_state_forward.hpp"
namespace supply_routes {


dcon::province_id supply_route_get_destination(const sys::state& state, dcon::army_supply_route_id route);
dcon::province_id supply_route_get_destination(const sys::state& state, dcon::navy_supply_route_id route);
dcon::province_id supply_route_get_destination(const sys::state& state, dcon::land_construction_supply_route_id route);
dcon::province_id supply_route_get_destination(const sys::state& state, dcon::naval_construction_supply_route_id route);

dcon::province_id supply_route_get_origin(const sys::state& state, dcon::army_supply_route_id route);
dcon::province_id supply_route_get_origin(const sys::state& state, dcon::navy_supply_route_id route);
dcon::province_id supply_route_get_origin(const sys::state& state, dcon::land_construction_supply_route_id route);
dcon::province_id supply_route_get_origin(const sys::state& state, dcon::naval_construction_supply_route_id route);

template<concepts::unit_supply_or_build_commodity_type commodity_type, concepts::military_supply_route_type route_type>
float military_route_get_buffered_goods(const sys::state& state, route_type route, commodity_type commodity_id);


dcon::nation_id supply_route_get_owner(const sys::state& state, dcon::army_supply_route_id route);
dcon::nation_id supply_route_get_owner(const sys::state& state, dcon::navy_supply_route_id route);
dcon::nation_id supply_route_get_owner(const sys::state& state, dcon::land_construction_supply_route_id route);
dcon::nation_id supply_route_get_owner(const sys::state& state, dcon::naval_construction_supply_route_id route);

// Computes the efficiency of a construct with has consumed vs available. Eg supply throughput
float compute_efficiency(float consumed, float available);

float port_supply_capacity_mult_hostile_troops_modifier(const sys::state& state, dcon::province_id prov, dcon::nation_id nation_as);

float port_supply_capacity_in_province(const sys::state& state, dcon::province_id port_prov, dcon::nation_id nation_as);

float port_supply_capacity_mult_supply_access_modifier(const sys::state& state, dcon::province_id port_prov, dcon::nation_id nation_as);

float port_supply_capacity_mult_blockaded_modifier(const sys::state& state, dcon::province_id port_prov, dcon::nation_id nation_as);

float port_supply_capacity_efficiency(const sys::state& state, dcon::province_id port_prov, dcon::nation_id nation_as);



float supply_throughput_mult_access_modifier(const sys::state& state, dcon::province_id province, dcon::nation_id nation_as);
float supply_throughput_mult_hostile_troops_modifier(const sys::state& state, dcon::province_id prov, dcon::nation_id nation_as);

// Gets the max supply throughput modifier in goods volume for the specified nation in the specified province, with percentage-based modifiers taken into account
float supply_throughput_in_province(const sys::state& state, dcon::province_id prov, dcon::nation_id nation_as);

// Gets the current effective efficency percentage in moving supplies for a nation in the specified provincce
float supply_throughput_efficiency(const sys::state& state, dcon::province_id prov, dcon::nation_id nation_as);

float supply_loss_add_convoy_raiding(const sys::state& state, dcon::province_id province, dcon::nation_id nation_as);
float supply_loss_add_hostile_armies(const sys::state& state, dcon::province_id province, dcon::nation_id nation_as);

float supply_loss_in_province(const sys::state& state, dcon::province_id province, dcon::nation_id nation_as);
float adjacency_net_supply_loss(const sys::state& state, dcon::province_adjacency_id province_adj, dcon::nation_id nation_as);
float adjacency_avg_supply_loss(const sys::state& state, dcon::province_id prov_1, dcon::province_id prov_2, dcon::nation_id nation_as);
float adjacency_avg_supply_loss(const sys::state& state, dcon::province_adjacency_id province_adj, dcon::nation_id nation_as);
void update_supply_routes_daily(sys::state& state);


}
