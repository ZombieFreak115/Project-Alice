#pragma once
#include "system_state.hpp"
namespace supply_routes {





dcon::province_id supply_route_get_destination(const sys::state& state, dcon::army_supply_route_id route);
dcon::province_id supply_route_get_destination(const sys::state& state, dcon::navy_supply_route_id route);
dcon::province_id supply_route_get_destination(const sys::state& state, dcon::land_construction_supply_route_id route);
dcon::province_id supply_route_get_destination(const sys::state& state, dcon::naval_construction_supply_route_id route);

dcon::province_id supply_route_get_origin(const sys::state& state, dcon::army_supply_route_id route);
dcon::province_id supply_route_get_origin(const sys::state& state, dcon::navy_supply_route_id route);
dcon::province_id supply_route_get_origin(const sys::state& state, dcon::land_construction_supply_route_id route);
dcon::province_id supply_route_get_origin(const sys::state& state, dcon::naval_construction_supply_route_id route);

template<military::unit_consumption_type consumption_type, concepts::military_supply_route_type route_type>
const economy::huge_commodity_amount_array& military_route_get_buffered_goods(const sys::state& state, route_type route);

template<military::unit_consumption_type consumption_type, concepts::military_supply_route_type route_type>
economy::huge_commodity_amount_array& military_route_get_buffered_goods(sys::state& state, route_type route);

dcon::nation_id supply_route_get_owner(const sys::state& state, dcon::army_supply_route_id route);
dcon::nation_id supply_route_get_owner(const sys::state& state, dcon::navy_supply_route_id route);
dcon::nation_id supply_route_get_owner(const sys::state& state, dcon::land_construction_supply_route_id route);
dcon::nation_id supply_route_get_owner(const sys::state& state, dcon::naval_construction_supply_route_id route);

float max_supply_throughput(const sys::state& state, dcon::province_id province, dcon::nation_id nation_as);
float province_supply_attrition(const sys::state& state, dcon::province_id province, dcon::nation_id nation_as);
float adjacency_supply_attrition(const sys::state& state, dcon::province_adjacency_id province_adj, dcon::nation_id nation_as);
float avg_adjacency_supply_attrition(const sys::state& state, dcon::province_id prov_1, dcon::province_id prov_2, dcon::nation_id nation_as);
float avg_adjacency_supply_attrition(const sys::state& state, dcon::province_adjacency_id province_adj, dcon::nation_id nation_as);
void update_supply_routes_daily(sys::state& state);
void update_supply_routes_monthly(sys::state& state);


}
