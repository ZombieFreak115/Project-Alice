#pragma once
#include "system_state.hpp"

namespace supply_routes {

template<typename F>
void for_each_supply_route(sys::state& state, F&& functor) {
	state.world.for_each_army_supply_route(functor);
	state.world.for_each_navy_supply_route(functor);
	state.world.for_each_land_construction_supply_route(functor);
	state.world.for_each_naval_construction_supply_route(functor);
}
template<typename F, typename army_container_type, typename navy_container_type, typename land_construction_container_type, typename naval_construction_container_type>
void for_each_supply_route_container(sys::state& state, F&& functor, army_container_type& army_container, navy_container_type& navy_container, land_construction_container_type& land_construction_container, naval_construction_container_type& naval_construction_container) {
	state.world.for_each_army_supply_route([&](auto route) {
		functor(route, army_container);
	});
	state.world.for_each_navy_supply_route([&](auto route) {
		functor(route, navy_container);
	});
	state.world.for_each_land_construction_supply_route([&](auto route) {
		functor(route, land_construction_container);
	});
	state.world.for_each_naval_construction_supply_route([&](auto route) {
		functor(route, naval_construction_container);
	});
}

template<typename F>
void parallel_for_each_supply_route(sys::state& state, F&& functor) {
	concurrency::parallel_for(uint32_t(0), state.world.army_supply_route_size(), [&](uint32_t i) {
		dcon::army_supply_route_id route = dcon::army_supply_route_id{ dcon::army_supply_route_id::value_base_t(i)};
		functor(route);
	});
	concurrency::parallel_for(uint32_t(0), state.world.navy_supply_route_size(), [&](uint32_t i) {
		dcon::navy_supply_route_id route = dcon::navy_supply_route_id{ dcon::navy_supply_route_id::value_base_t(i) };
		functor(route);
	});
	concurrency::parallel_for(uint32_t(0), state.world.land_construction_supply_route_size(), [&](uint32_t i) {
		dcon::land_construction_supply_route_id route = dcon::land_construction_supply_route_id{ dcon::land_construction_supply_route_id::value_base_t(i) };
		functor(route);
	});
	concurrency::parallel_for(uint32_t(0), state.world.naval_construction_supply_route_size(), [&](uint32_t i) {
		dcon::naval_construction_supply_route_id route = dcon::naval_construction_supply_route_id{ dcon::naval_construction_supply_route_id::value_base_t(i) };
		functor(route);
	});
}
template<typename F, typename army_container_type, typename navy_container_type, typename land_construction_container_type, typename naval_construction_container_type>
void parallel_for_each_supply_route_container(sys::state& state, F&& functor, army_container_type& army_container, navy_container_type& navy_container, land_construction_container_type& land_construction_container, naval_construction_container_type& naval_construction_container) {
	concurrency::parallel_for(uint32_t(0), state.world.army_supply_route_size(), [&](uint32_t i) {
		dcon::army_supply_route_id route = dcon::army_supply_route_id{ dcon::army_supply_route_id::value_base_t(i) };
		functor(route, army_container);
	});
	concurrency::parallel_for(uint32_t(0), state.world.navy_supply_route_size(), [&](uint32_t i) {
		dcon::navy_supply_route_id route = dcon::navy_supply_route_id{ dcon::navy_supply_route_id::value_base_t(i) };
		functor(route, navy_container);
	});
	concurrency::parallel_for(uint32_t(0), state.world.land_construction_supply_route_size(), [&](uint32_t i) {
		dcon::land_construction_supply_route_id route = dcon::land_construction_supply_route_id{ dcon::land_construction_supply_route_id::value_base_t(i) };
		functor(route, land_construction_container);
	});
	concurrency::parallel_for(uint32_t(0), state.world.naval_construction_supply_route_size(), [&](uint32_t i) {
		dcon::naval_construction_supply_route_id route = dcon::naval_construction_supply_route_id{ dcon::naval_construction_supply_route_id::value_base_t(i) };
		functor(route, naval_construction_container);
	});
}

}
