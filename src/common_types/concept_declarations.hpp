#pragma once
#include <type_traits>
#include "dcon_generated_ids.hpp"
namespace concepts {


template<typename T>
concept military_unit = std::is_same_v<T, dcon::army_id> || std::is_same_v<T, dcon::navy_id>;

template<typename T>
concept military_subunit = std::is_same_v<T, dcon::regiment_id> || std::is_same_v<T, dcon::ship_id>;


template<typename T>
concept military_supply_route_type = std::is_same_v<T, dcon::army_supply_route_id> ||
									std::is_same_v<T, dcon::navy_supply_route_id>;

template<typename T>
concept construction_supply_route_type = std::is_same_v<T, dcon::land_construction_supply_route_id> ||
										 std::is_same_v<T, dcon::naval_construction_supply_route_id>;

template<typename T>
concept supply_route_type = military_supply_route_type<T> || construction_supply_route_type<T>;


template<typename T>
concept military_construction_type = std::is_same_v<T, dcon::province_land_construction_id> ||
									 std::is_same_v<T, dcon::province_naval_construction_id>;

template<typename T>
concept economy_construction_type = std::is_same_v<T, dcon::factory_construction_id> ||
									std::is_same_v<T, dcon::province_building_construction_id>;

template<typename T>
concept construction_type = economy_construction_type<T> || military_construction_type<T>;

template<typename T>
concept cvref_integral = std::integral<std::remove_cvref_t<T>>;

template <typename T>
concept dcon_id_type =
	requires (T t) { { t.index() }-> cvref_integral; }
&&
	requires (T t) { { t.value }-> cvref_integral; };


template<typename T, typename ID>
concept dcon_id_ve_type = dcon_id_type<ID> &&
						  (std::is_same_v<T, ve::contiguous_tags<ID>> ||
							  std::is_same_v<T, ve::tagged_vector<ID>>||
							  std::is_same_v<T, ve::value_to_vector_type<ID>> ||
						  std::is_same_v <T, ve::partial_contiguous_tags<ID>> ||
						  std::is_same_v <T, ve::unaligned_contiguous_tags<ID>>);


// This is really ugly and unintuitive. If anyone knows a nicer way of doing this (have a concept that accepts all specializations of a specific template) feel free to chang eit
template<typename T>
struct is_commodity_set : std::false_type { };

template<uint32_t N>
struct is_commodity_set<economy::commodity_set_base<N>> : std::true_type { };

template<typename T>
concept commodity_set_type = is_commodity_set<T>::value;




template<typename T>
struct is_commodity_amount_array : std::false_type {
};

template<uint32_t N>
struct is_commodity_amount_array<sys::fixed_size_vector<float, N>> : std::true_type {
};

template<typename T>
concept commodity_amount_array_type = is_commodity_amount_array<T>::value;







template<typename T>
struct is_commodity_id_array : std::false_type {
};

template<uint32_t N>
struct is_commodity_id_array<sys::fixed_size_vector<dcon::commodity_id, N>> : std::true_type {
};

template<typename T>
concept commodity_id_array_type = is_commodity_id_array<T>::value;



}
