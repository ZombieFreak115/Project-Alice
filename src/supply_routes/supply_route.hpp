#pragma once
#include "system_state.hpp"
namespace supply_routes {


constexpr float sea_base_supply_thoughput = 1000.0f;
constexpr float land_base_supply_thoughput = 0.5f;
constexpr float supply_throughput_per_km_land_speed = 10.0f; // Supply throughput in land provinces per 1 km/h land supply speed. Eg if set to 100 and a nation has a speed of 4 km/h, then the base is 400
constexpr float supply_throughput_per_km_naval_speed = 10.0f; // Supply throughput in naval provinces per 1 km/h naval supply speed. Eg if set to 100 and a nation has a speed of 4 km/h, then the base is 400
constexpr float supply_throughput_infrastructure = 6.0f; // Extra supply throughput per 1% of infrastructure
constexpr float army_supply_throughput_blockade_threshold = 0.9f; // This amount of POP_SIZE_PER_REGIMENT army strength is required to fully block enemy throughput in a land province. Throughput scales linearly with the amount of strength present. Also includes strength in battles. Set to zero or negative number to disable
constexpr float navy_supply_throughput_coastal_sea_blockade_threshold = 30.0f; // This amount of blockade power (unit stat) per 1000 km2 province size is required to to fully block enemy throughput in a sea province. Throughput scales linearly with the amount of strength present. Also includes strength in battles. Set to zero or negative number to disable
constexpr float base_land_supply_attrition = 0.0000001f;
constexpr float base_sea_supply_attrition = 0.0f;
constexpr float control_level_supply_attrition = 0.0001f; // the supply loss % per km of travel if province control is 0%. Scales back to 0 at 100% control.
constexpr float militancy_supply_attrition = 0.000005f; // the supply loss % per km of travel per average militancy in the province.
constexpr float hostile_army_supply_attrition = 0.0f; // the supply loss % per km of travel per POP_SIZE_PER_REGIMENT enemy strength present in the land province
constexpr float hostile_navy_supply_attrition = 0.0f; // the supply loss % per km of travel per 100% enemy ship strength present in the sea province



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

float supply_throughput_percentage_access_modifier(const sys::state& state, dcon::province_id province, dcon::nation_id nation_as);
float supply_throughput_percentage_blockade_modifier(const sys::state& state, dcon::province_id prov, dcon::nation_id nation_as);

// Gets the percentage supply throughput modifier for the specified nation in the specified province
float supply_throughput_percentage_modifier(const sys::state& state, dcon::province_id prov, dcon::nation_id nation_as);

float supply_throughput_infrastructure_modifier(const sys::state& state, dcon::province_id province, dcon::nation_id nation_as);
float supply_throughput_speed_modifier(const sys::state& state, dcon::province_id province, dcon::nation_id nation_as);

// Gets the raw max supply throughput modifier in goods volume for the specified nation in the specified province
float supply_throughput_modifier(const sys::state& state, dcon::province_id province, dcon::nation_id nation_as);

// Gets the max supply throughput modifier in goods volume for the specified nation in the specified province, with percentage-based modifiers taken into account
float combined_supply_throughput_modifier(const sys::state& state, dcon::province_id prov, dcon::nation_id nation_as);

// Gets the current effective efficency percentage in moving supplies for a nation in the specified provincce (0.0-1.0)
float supply_throughput_efficiency(const sys::state& state, dcon::province_id prov, dcon::nation_id nation_as);

float province_supply_attrition_modifier(const sys::state& state, dcon::province_id province, dcon::nation_id nation_as);
float adjacency_supply_attrition_modifier(const sys::state& state, dcon::province_adjacency_id province_adj, dcon::nation_id nation_as);
float avg_adjacency_supply_attrition_modifier(const sys::state& state, dcon::province_id prov_1, dcon::province_id prov_2, dcon::nation_id nation_as);
float avg_adjacency_supply_attrition_modifier(const sys::state& state, dcon::province_adjacency_id province_adj, dcon::nation_id nation_as);
void update_supply_routes_daily(sys::state& state);
void update_supply_routes_monthly(sys::state& state);


}
