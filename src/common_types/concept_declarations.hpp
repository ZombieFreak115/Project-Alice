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

// Concept for the union of supply goods OR union of build goods for all military units. 
template<typename T>
concept commodity_amount_military_supply_or_build_union_array_type = std::is_same_v<T, economy::supply_cost_union_commodity_amount_array> ||
																	 std::is_same_v<T, economy::build_cost_union_commodity_amount_array>;

// Concept for union of supply goods OR union of build goods OR the union of both of these together.
template<typename T>
concept commodity_amount_military_union_array_type = commodity_amount_military_supply_or_build_union_array_type<T> ||
											std::is_same_v<T, economy::supply_and_build_cost_union_commodity_amount_array>;

// Concept for all commodity amount array types which hold commodity amounts unique to a unit type (eg. build goods for a single unit type)
template<typename T>
concept commodity_amount_military_unit_array_type = std::is_same_v<T, economy::unit_build_cost_commodity_amount_array> ||
										   std::is_same_v<T, economy::unit_supply_cost_commodity_amount_array>;
// Concept for all commodity amount array types which either hold commodity amounts unique to a unit type, or hold a union of commodity amounts
template<typename T>
concept commodity_amount_array_type = commodity_amount_military_union_array_type<T> || commodity_amount_military_unit_array_type<T>;




// Concept for the union of supply goods OR union of build goods for all military units. 
template<typename T>
concept commodity_id_military_supply_or_build_union_array_type = std::is_same_v<T, economy::supply_cost_union_commodity_id_array> ||
																std::is_same_v<T, economy::build_cost_union_commodity_id_array>;

// Concept for union of supply goods OR union of build goods OR the union of both of these together.
template<typename T>
concept commodity_id_military_union_array_type = commodity_id_military_supply_or_build_union_array_type<T> ||
												std::is_same_v<T, economy::supply_and_build_cost_union_commodity_id_array>;

// Concept for all commodity amount array types which either hold commodity amounts unique to a unit type, or hold a union of commodity amounts
template<typename T>
concept commodity_id_array_type = commodity_id_military_union_array_type<T>;

// Gets the corrosponding commodity id array type of the commodity amount array type
template<commodity_amount_military_union_array_type array_type>
struct military_commodity_amount_to_id_union{ };

template<>
struct military_commodity_amount_to_id_union<economy::supply_and_build_cost_union_commodity_amount_array> { using type = economy::supply_and_build_cost_union_commodity_id_array; };
template<>
struct military_commodity_amount_to_id_union<economy::build_cost_union_commodity_amount_array> { using type = economy::build_cost_union_commodity_id_array; };
template<>
struct military_commodity_amount_to_id_union<economy::supply_cost_union_commodity_amount_array> { using type = economy::supply_cost_union_commodity_id_array; };

// Gets the corrosponding amount array type of the commodity id array type
template<commodity_id_military_union_array_type array_type>
struct military_commodity_id_to_amount_union {};

template<>
struct military_commodity_id_to_amount_union<economy::supply_and_build_cost_union_commodity_id_array> {using type = economy::supply_and_build_cost_union_commodity_amount_array; };
template<>
struct military_commodity_id_to_amount_union<economy::build_cost_union_commodity_id_array> {using type = economy::build_cost_union_commodity_amount_array; };
template<>
struct military_commodity_id_to_amount_union<economy::supply_cost_union_commodity_id_array> { using type = economy::supply_cost_union_commodity_amount_array; };


// Gets the corrosponding amount array type of the unit consumption enum
template<military::unit_consumption_type consume_type>
struct unit_consumption_to_amount_union {
};

template<>
struct unit_consumption_to_amount_union<military::unit_consumption_type::supply> {
	using type = economy::supply_cost_union_commodity_amount_array;
};
template<>
struct unit_consumption_to_amount_union<military::unit_consumption_type::reinforcement> {
	using type = economy::build_cost_union_commodity_amount_array;
};

// Gets the corrosponding commodity id array type of the unit consumption enum
template<military::unit_consumption_type consume_type>
struct unit_consumption_to_id_union {
};

template<>
struct unit_consumption_to_id_union<military::unit_consumption_type::supply> {
	using type = economy::supply_cost_union_commodity_id_array;
};
template<>
struct unit_consumption_to_id_union<military::unit_consumption_type::reinforcement> {
	using type = economy::build_cost_union_commodity_id_array;
};


// Gets the corrosponding amount array type of the commodity consumption enum
template<military::commodity_consumption_type consume_type>
struct commodity_conumption_to_amount_union { };

template<>
struct commodity_conumption_to_amount_union<military::commodity_consumption_type::supply> {
	using type = economy::supply_cost_union_commodity_amount_array;
};
template<>
struct commodity_conumption_to_amount_union<military::commodity_consumption_type::reinforcement> {
	using type = economy::build_cost_union_commodity_amount_array;
};
template<>
struct commodity_conumption_to_amount_union<military::commodity_consumption_type::both> {
	using type = economy::supply_and_build_cost_union_commodity_amount_array;
};




// Gets the corrosponding commodity id array type of the commodity consumption enum
template<military::commodity_consumption_type consume_type>
struct commodity_conumption_to_id_union {
};

template<>
struct commodity_conumption_to_id_union<military::commodity_consumption_type::supply> {
	using type = economy::supply_cost_union_commodity_id_array;
};
template<>
struct commodity_conumption_to_id_union<military::commodity_consumption_type::reinforcement> {
	using type = economy::build_cost_union_commodity_id_array;
};
template<>
struct commodity_conumption_to_id_union<military::commodity_consumption_type::both> {
	using type = economy::supply_and_build_cost_union_commodity_id_array;
};

}
