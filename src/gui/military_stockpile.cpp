// BEGIN prelude
// END

namespace alice_ui {
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wswitch"
#pragma clang diagnostic ignored "-Wimplicit-fallthrough"
#endif
struct military_stockpile_main_selected_commodity_icon_t;
struct military_stockpile_main_stockpile_target_input_t;
struct military_stockpile_main_confirm_target_button_t;
struct military_stockpile_main_stockpile_target_error_t;
struct military_stockpile_main_t;
struct military_stockpile_grid_item_commodity_icon_t;
struct military_stockpile_grid_item_current_stockpile_count_t;
struct military_stockpile_grid_item_stockpile_target_t;
struct military_stockpile_grid_item_daily_stockpile_change_t;
struct military_stockpile_grid_item_days_left_t;
struct military_stockpile_grid_item_t;
struct military_stockpile_spacer_t;
struct military_stockpile_main_selected_commodity_icon_t : public ui::element_base {
// BEGIN main::selected_commodity_icon::variables
// END
	std::string_view gfx_key;
	dcon::gfx_object_id background_gid;
	int32_t frame = 0;
	void on_create(sys::state& state) noexcept override;
	void render(sys::state & state, int32_t x, int32_t y) noexcept override;
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::variable_tooltip;
	}
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override {
		if(type == ui::mouse_probe_type::click) {
			return ui::message_result::unseen;
		} else if(type == ui::mouse_probe_type::tooltip) {
			return ui::message_result::consumed;
		} else if(type == ui::mouse_probe_type::scroll) {
			return ui::message_result::unseen;
		} else {
			return ui::message_result::unseen;
		}
	}
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct military_stockpile_main_stockpile_target_input_t : public ui::edit_box_element_base {
// BEGIN main::stockpile_target_input::variables
// END
	void on_update(sys::state& state) noexcept override;
	void on_create(sys::state& state) noexcept override;
};
struct military_stockpile_main_confirm_target_button_t : public alice_ui::template_mixed_button {
// BEGIN main::confirm_target_button::variables
// END
	bool button_action(sys::state& state) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct military_stockpile_main_stockpile_target_error_t : public ui::element_base {
// BEGIN main::stockpile_target_error::variables
// END
	text::layout internal_layout;
	text::text_color text_color = text::text_color::red;
	float text_scale = 1.000000f; 
	bool text_is_header = false; 
	text::alignment text_alignment = text::alignment::center;
	std::string cached_text;
	void set_text(sys::state & state, std::string const& new_text);
	void on_reset_text(sys::state & state) noexcept override;
	void on_create(sys::state& state) noexcept override;
	void render(sys::state & state, int32_t x, int32_t y) noexcept override;
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::no_tooltip;
	}
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override {
		if(type == ui::mouse_probe_type::click) {
			return ui::message_result::unseen;
		} else if(type == ui::mouse_probe_type::tooltip) {
			return ui::message_result::unseen;
		} else if(type == ui::mouse_probe_type::scroll) {
			return ui::message_result::unseen;
		} else {
			return ui::message_result::unseen;
		}
	}
	void on_update(sys::state& state) noexcept override;
};
struct military_stockpile_main_commodity_grid_g_t : public layout_generator {
// BEGIN main::commodity_grid_g::variables
// END
	struct grid_item_option { dcon::commodity_id value; };
	std::vector<std::unique_ptr<ui::element_base>> grid_item_pool;
	int32_t grid_item_pool_used = 0;
	void add_grid_item( dcon::commodity_id value);
	struct spacer_option { };
	std::vector<std::unique_ptr<ui::element_base>> spacer_pool;
	int32_t spacer_pool_used = 0;
	void add_spacer();
	std::vector<std::variant<std::monostate, grid_item_option, spacer_option>> values;
	void on_create(sys::state& state, layout_window_element* container);
	void update(sys::state& state, layout_window_element* container);
	measure_result place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) override;
	size_t item_count() override { return values.size(); };
	void reset_pools() override;
};
struct military_stockpile_grid_item_commodity_icon_t : public ui::element_base {
// BEGIN grid_item::commodity_icon::variables
// END
	std::string_view gfx_key;
	dcon::gfx_object_id background_gid;
	int32_t frame = 0;
	dcon::text_key tooltip_key;
	void on_create(sys::state& state) noexcept override;
	void render(sys::state & state, int32_t x, int32_t y) noexcept override;
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::tooltip;
	}
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override {
		if(type == ui::mouse_probe_type::click) {
			return ui::message_result::consumed;
		} else if(type == ui::mouse_probe_type::tooltip) {
			return ui::message_result::consumed;
		} else if(type == ui::mouse_probe_type::scroll) {
			return ui::message_result::unseen;
		} else {
			return ui::message_result::unseen;
		}
	}
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct military_stockpile_grid_item_current_stockpile_count_t : public alice_ui::template_label {
// BEGIN grid_item::current_stockpile_count::variables
// END
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct military_stockpile_grid_item_stockpile_target_t : public alice_ui::template_label {
// BEGIN grid_item::stockpile_target::variables
// END
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct military_stockpile_grid_item_daily_stockpile_change_t : public alice_ui::template_label {
// BEGIN grid_item::daily_stockpile_change::variables
// END
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct military_stockpile_grid_item_days_left_t : public alice_ui::template_label {
// BEGIN grid_item::days_left::variables
// END
	ui::tooltip_behavior has_tooltip(sys::state & state) noexcept override {
		return ui::tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override;
	void on_update(sys::state& state) noexcept override;
};
struct military_stockpile_main_t : public layout_window_element {
// BEGIN main::variables
// END
	dcon::commodity_id selected_commodity;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<template_label> header_text;
	std::unique_ptr<military_stockpile_main_selected_commodity_icon_t> selected_commodity_icon;
	std::unique_ptr<military_stockpile_main_stockpile_target_input_t> stockpile_target_input;
	std::unique_ptr<military_stockpile_main_confirm_target_button_t> confirm_target_button;
	std::unique_ptr<military_stockpile_main_stockpile_target_error_t> stockpile_target_error;
	std::unique_ptr<template_label> stockpile_target_editbox_label;
	std::unique_ptr<template_icon_graphic> info_stockpile_view;
	std::unique_ptr<template_icon_graphic> info_stockpile_target_view;
	military_stockpile_main_commodity_grid_g_t commodity_grid_g;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	void on_hide(sys::state& state) noexcept override;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override {
		return ui::message_result::consumed;
	}
	void on_drag(sys::state& state, int32_t oldx, int32_t oldy, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override {
		auto location_abs = get_absolute_location(state, *this);
		if(location_abs.x <= oldx && oldx < base_data.size.x + location_abs.x && location_abs.y <= oldy && oldy < base_data.size.y + location_abs.y) {
			ui::xy_pair new_abs_pos = location_abs;
			new_abs_pos.x += int16_t(x - oldx);
			new_abs_pos.y += int16_t(y - oldy);
			if(ui::ui_width(state) > base_data.size.x)
				new_abs_pos.x = int16_t(std::clamp(int32_t(new_abs_pos.x), 0, ui::ui_width(state) - base_data.size.x));
			if(ui::ui_height(state) > base_data.size.y)
				new_abs_pos.y = int16_t(std::clamp(int32_t(new_abs_pos.y), 0, ui::ui_height(state) - base_data.size.y));
			if(state_is_rtl(state)) {
				base_data.position.x -= int16_t(new_abs_pos.x - location_abs.x);
			} else {
				base_data.position.x += int16_t(new_abs_pos.x - location_abs.x);
			}
		base_data.position.y += int16_t(new_abs_pos.y - location_abs.y);
		}
	}
	void on_update(sys::state& state) noexcept override;
	void* get_by_name(sys::state& state, std::string_view name_parameter) noexcept override {
		if(name_parameter == "selected_commodity") {
			return (void*)(&selected_commodity);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_military_stockpile_main(sys::state& state);
struct military_stockpile_grid_item_t : public layout_window_element {
// BEGIN grid_item::variables
// END
	dcon::commodity_id value;
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::unique_ptr<military_stockpile_grid_item_commodity_icon_t> commodity_icon;
	std::unique_ptr<military_stockpile_grid_item_current_stockpile_count_t> current_stockpile_count;
	std::unique_ptr<military_stockpile_grid_item_stockpile_target_t> stockpile_target;
	std::unique_ptr<military_stockpile_grid_item_daily_stockpile_change_t> daily_stockpile_change;
	std::unique_ptr<template_label> value_divider;
	std::unique_ptr<military_stockpile_grid_item_days_left_t> days_left;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz);
	void on_create(sys::state& state) noexcept override;
	ui::message_result on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	ui::message_result on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept override;
	void on_update(sys::state& state) noexcept override;
	void* get_by_name(sys::state& state, std::string_view name_parameter) noexcept override {
		if(name_parameter == "value") {
			return (void*)(&value);
		}
		return nullptr;
	}
};
std::unique_ptr<ui::element_base> make_military_stockpile_grid_item(sys::state& state);
struct military_stockpile_spacer_t : public ui::non_owning_container_base {
// BEGIN spacer::variables
// END
	ankerl::unordered_dense::map<std::string, std::unique_ptr<ui::lua_scripted_element>> scripted_elements;
	std::vector<std::unique_ptr<ui::element_base>> gui_inserts;
	void on_create(sys::state& state) noexcept override;
	void render(sys::state & state, int32_t x, int32_t y) noexcept override;
	ui::message_result test_mouse(sys::state& state, int32_t x, int32_t y, ui::mouse_probe_type type) noexcept override {
		return ui::message_result::unseen;
	}
	void on_update(sys::state& state) noexcept override;
};
std::unique_ptr<ui::element_base> make_military_stockpile_spacer(sys::state& state);
void military_stockpile_main_commodity_grid_g_t::add_grid_item(dcon::commodity_id value) {
	values.emplace_back(grid_item_option{value});
}
void military_stockpile_main_commodity_grid_g_t::add_spacer() {
	values.emplace_back(spacer_option{});
}
void  military_stockpile_main_commodity_grid_g_t::on_create(sys::state& state, layout_window_element* parent) {
	military_stockpile_main_t& main = *((military_stockpile_main_t*)(parent)); 
// BEGIN main::commodity_grid_g::on_create
	state.world.for_each_unit_supply_and_build_commodity([&](dcon::unit_supply_and_build_commodity_id com_id) {
		dcon::commodity_id base_id = economy::unit_commodity_get_base_commodity(state, com_id);
		add_grid_item(base_id);
	});
// END
}
void  military_stockpile_main_commodity_grid_g_t::update(sys::state& state, layout_window_element* parent) {
	military_stockpile_main_t& main = *((military_stockpile_main_t*)(parent)); 
// BEGIN main::commodity_grid_g::update
// END
}
measure_result  military_stockpile_main_commodity_grid_g_t::place_item(sys::state& state, ui::non_owning_container_base* destination, size_t index, int32_t x, int32_t y, bool first_in_section, bool& alternate) {
	if(index >= values.size()) return measure_result{0,0,measure_result::special::none};
	if(std::holds_alternative<grid_item_option>(values[index])) {
		if(grid_item_pool.empty()) grid_item_pool.emplace_back(make_military_stockpile_grid_item(state));
		if(destination) {
			if(grid_item_pool.size() <= size_t(grid_item_pool_used)) grid_item_pool.emplace_back(make_military_stockpile_grid_item(state));
			grid_item_pool[grid_item_pool_used]->base_data.position.x = int16_t(x);
			grid_item_pool[grid_item_pool_used]->base_data.position.y = int16_t(y);
			grid_item_pool[grid_item_pool_used]->parent = destination;
			destination->children.push_back(grid_item_pool[grid_item_pool_used].get());
			((military_stockpile_grid_item_t*)(grid_item_pool[grid_item_pool_used].get()))->value = std::get<grid_item_option>(values[index]).value;
			grid_item_pool[grid_item_pool_used]->impl_on_update(state);
			grid_item_pool_used++;
		}
		alternate = true;
	 	 	bool stick_to_next = false;
		return measure_result{ grid_item_pool[0]->base_data.size.x, grid_item_pool[0]->base_data.size.y + 0, stick_to_next ? measure_result::special::no_break : measure_result::special::none};
	}
	if(std::holds_alternative<spacer_option>(values[index])) {
		if(spacer_pool.empty()) spacer_pool.emplace_back(make_military_stockpile_spacer(state));
		if(destination) {
			if(spacer_pool.size() <= size_t(spacer_pool_used)) spacer_pool.emplace_back(make_military_stockpile_spacer(state));
			spacer_pool[spacer_pool_used]->base_data.position.x = int16_t(x);
			spacer_pool[spacer_pool_used]->base_data.position.y = int16_t(y);
			spacer_pool[spacer_pool_used]->parent = destination;
			destination->children.push_back(spacer_pool[spacer_pool_used].get());
			spacer_pool[spacer_pool_used]->impl_on_update(state);
			spacer_pool_used++;
		}
		alternate = true;
	 	 	bool stick_to_next = false;
		return measure_result{ spacer_pool[0]->base_data.size.x, spacer_pool[0]->base_data.size.y + 0, stick_to_next ? measure_result::special::no_break : measure_result::special::end_page};
	}
	return measure_result{0,0,measure_result::special::none};
}
void  military_stockpile_main_commodity_grid_g_t::reset_pools() {
	grid_item_pool_used = 0;
	spacer_pool_used = 0;
}
ui::message_result military_stockpile_main_selected_commodity_icon_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
ui::message_result military_stockpile_main_selected_commodity_icon_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::unseen;
}
void military_stockpile_main_selected_commodity_icon_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	military_stockpile_main_t& main = *((military_stockpile_main_t*)(parent)); 
// BEGIN main::selected_commodity_icon::tooltip
// END
}
void military_stockpile_main_selected_commodity_icon_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(background_gid) {
		auto& gfx_def = state.ui_defs.gfx[background_gid];
		if(gfx_def.primary_texture_handle) {
			if(gfx_def.get_object_type() == ui::object_type::bordered_rect) {
				ogl::render_bordered_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), gfx_def.type_dependent, float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()), base_data.get_rotation(), gfx_def.is_vertically_flipped(), state_is_rtl(state)); 
			} else if(gfx_def.number_of_frames > 1) {
				ogl::render_subsprite(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), frame, gfx_def.number_of_frames, float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()), base_data.get_rotation(), gfx_def.is_vertically_flipped(), state_is_rtl(state)); 
			} else {
				ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, false), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()), base_data.get_rotation(), gfx_def.is_vertically_flipped(), state_is_rtl(state)); 
			}
		}
	}
}
void military_stockpile_main_selected_commodity_icon_t::on_update(sys::state& state) noexcept {
	military_stockpile_main_t& main = *((military_stockpile_main_t*)(parent)); 
// BEGIN main::selected_commodity_icon::update
	if(!main.selected_commodity) {
		set_visible(state, false);
	}
	else {
		set_visible(state, true);
		frame = state.world.commodity_get_icon(main.selected_commodity);
	}
// END
}
void military_stockpile_main_selected_commodity_icon_t::on_create(sys::state& state) noexcept {
	if(auto it = state.ui_state.gfx_by_name.find(state.lookup_key(gfx_key)); it != state.ui_state.gfx_by_name.end()) {
		background_gid = it->second;
	}
// BEGIN main::selected_commodity_icon::create
// END
}
void military_stockpile_main_stockpile_target_input_t::on_update(sys::state& state) noexcept {
	military_stockpile_main_t& main = *((military_stockpile_main_t*)(parent)); 
// BEGIN main::stockpile_target_input::update
	disabled = !main.selected_commodity;
// END
}
void military_stockpile_main_stockpile_target_input_t::on_create(sys::state& state) noexcept {
// BEGIN main::stockpile_target_input::create
// END
}
void military_stockpile_main_confirm_target_button_t::on_update(sys::state& state) noexcept {
	military_stockpile_main_t& main = *((military_stockpile_main_t*)(parent)); 
// BEGIN main::confirm_target_button::update
	disabled = !main.selected_commodity;
// END
}
bool military_stockpile_main_confirm_target_button_t::button_action(sys::state& state) noexcept {
	military_stockpile_main_t& main = *((military_stockpile_main_t*)(parent)); 
// BEGIN main::confirm_target_button::lbutton_action
	if(!main.selected_commodity) {
		return false;
	}
	auto raw_input = main.stockpile_target_input->get_text(state);
	auto utf8_str = simple_fs::utf16_to_utf8(raw_input);
	auto parsed_res = parsers::try_parse_float(utf8_str);
	if(parsed_res && command::can_change_stockpile_settings(state, state.local_player_nation, main.selected_commodity, parsed_res.value(), false)) {
		command::change_stockpile_settings(state, state.local_player_nation, main.selected_commodity,parsed_res.value(), false);
		main.stockpile_target_error->set_text(state, ""); // Clear any potential error if sucess
	}
	else {
		main.stockpile_target_error->set_text(state, text::produce_simple_string(state, "mlitary_stockpile_target_error"));
	}
	state.game_state_updated.store(true, std::memory_order::release);
// END
	return true;
}
void military_stockpile_main_stockpile_target_error_t::set_text(sys::state& state, std::string const& new_text) {
	if(new_text != cached_text) {
		cached_text = new_text;
		internal_layout.contents.clear();
		internal_layout.number_of_lines = 0;
		text::single_line_layout sl{ internal_layout, text::layout_parameters{ 0, 0, static_cast<int16_t>(base_data.size.x), static_cast<int16_t>(base_data.size.y), text::make_font_id(state, text_is_header, text_scale * 14), 0, text_alignment, text::text_color::black, true, true }, state_is_rtl(state) ? text::layout_base::rtl_status::rtl : text::layout_base::rtl_status::ltr };
		sl.add_text(state, cached_text);
	}
}
void military_stockpile_main_stockpile_target_error_t::on_reset_text(sys::state& state) noexcept {
}
void military_stockpile_main_stockpile_target_error_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(internal_layout.contents.empty()) return;
	auto fh = text::make_font_id(state, text_is_header, text_scale * 14);
	auto linesz = state.font_collection.line_height(state, fh); 
	if(linesz == 0.0f) return;
	auto ycentered = (base_data.size.y - linesz) / 2;
	auto cmod = ui::get_color_modification(this == state.ui_state.under_mouse, false, false); 
	for(auto& t : internal_layout.contents) {
		ui::render_text_chunk(state, t, float(x) + t.x, float(y + int32_t(ycentered)),  fh, ui::get_text_color(state, text_color), cmod);
	}
}
void military_stockpile_main_stockpile_target_error_t::on_update(sys::state& state) noexcept {
	military_stockpile_main_t& main = *((military_stockpile_main_t*)(parent)); 
// BEGIN main::stockpile_target_error::update
// END
}
void military_stockpile_main_stockpile_target_error_t::on_create(sys::state& state) noexcept {
// BEGIN main::stockpile_target_error::create
// END
}
ui::message_result military_stockpile_main_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	state.ui_state.drag_target = this;
	return ui::message_result::consumed;
}
ui::message_result military_stockpile_main_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void military_stockpile_main_t::on_hide(sys::state& state) noexcept {
// BEGIN main::on_hide
// END
}
void military_stockpile_main_t::on_update(sys::state& state) noexcept {
// BEGIN main::update
// END
	commodity_grid_g.update(state, this);
	remake_layout(state, true);
}
void military_stockpile_main_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
	serialization::in_buffer buffer(ldata, sz);
	buffer.read(lvl.size_x); 
	buffer.read(lvl.size_y); 
	buffer.read(lvl.margin_top); 
	buffer.read(lvl.margin_bottom); 
	buffer.read(lvl.margin_left); 
	buffer.read(lvl.margin_right); 
	buffer.read(lvl.line_alignment); 
	buffer.read(lvl.line_internal_alignment); 
	buffer.read(lvl.type); 
	buffer.read(lvl.page_animation); 
	buffer.read(lvl.interline_spacing); 
	buffer.read(lvl.paged); 
	if(lvl.paged) {
		lvl.page_controls = std::make_unique<page_buttons>();
		lvl.page_controls->for_layout = &lvl;
		lvl.page_controls->parent = this;
		lvl.page_controls->base_data.size.x = int16_t(grid_size * 10);
		lvl.page_controls->base_data.size.y = int16_t(grid_size * 2);
	}
	auto expansion_section = buffer.read_section();
	if(expansion_section)
		expansion_section.read(lvl.template_id);
	if(lvl.template_id == -1 && window_template != -1)
		lvl.template_id = int16_t(state.ui_templates.window_t[window_template].layout_region_definition);
	while(buffer) {
		layout_item_types t;
		buffer.read(t);
		switch(t) {
			case layout_item_types::texture_layer:
			{
				texture_layer temp;
				buffer.read(temp.texture_type);
				buffer.read(temp.texture);
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::control2:
			{
				layout_control temp;
				std::string_view cname = buffer.read<std::string_view>();
				buffer.read(temp.abs_x);
				buffer.read(temp.abs_y);
				buffer.read(temp.absolute_position);
				buffer.read(temp.fill_x);
				buffer.read(temp.fill_y);
				temp.ptr = nullptr;
				if(cname == "header_text") {
					temp.ptr = header_text.get();
				} else
				if(cname == "selected_commodity_icon") {
					temp.ptr = selected_commodity_icon.get();
				} else
				if(cname == "stockpile_target_input") {
					temp.ptr = stockpile_target_input.get();
				} else
				if(cname == "confirm_target_button") {
					temp.ptr = confirm_target_button.get();
				} else
				if(cname == "stockpile_target_error") {
					temp.ptr = stockpile_target_error.get();
				} else
				if(cname == "stockpile_target_editbox_label") {
					temp.ptr = stockpile_target_editbox_label.get();
				} else
				if(cname == "info_stockpile_view") {
					temp.ptr = info_stockpile_view.get();
				} else
				if(cname == "info_stockpile_target_view") {
					temp.ptr = info_stockpile_target_view.get();
				} else
				{
					std::string str_cname {cname};
					auto found = scripted_elements.find(str_cname);
					if (found != scripted_elements.end()) {
						temp.ptr = found->second.get();
					}
				}
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::window2:
			{
				layout_window temp;
				std::string_view cname = buffer.read<std::string_view>();
				buffer.read(temp.abs_x);
				buffer.read(temp.abs_y);
				buffer.read(temp.absolute_position);
				buffer.read(temp.fill_x);
				buffer.read(temp.fill_y);
				if(cname == "main") {
					temp.ptr = make_military_stockpile_main(state);
				}
				if(cname == "grid_item") {
					temp.ptr = make_military_stockpile_grid_item(state);
				}
				if(cname == "spacer") {
					temp.ptr = make_military_stockpile_spacer(state);
				}
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::glue:
			{
				layout_glue temp;
				buffer.read(temp.type);
				buffer.read(temp.amount);
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::generator2:
			{
				generator_instance temp;
				std::string_view cname = buffer.read<std::string_view>();
				auto gen_details = buffer.read_section(); // ignored
				if(cname == "commodity_grid_g") {
					temp.generator = &commodity_grid_g;
				}
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::layout:
			{
				sub_layout temp;
				temp.layout = std::make_unique<layout_level>();
				auto layout_section = buffer.read_section();
				create_layout_level(state, *temp.layout, layout_section.view_data() + layout_section.view_read_position(), layout_section.view_size() - layout_section.view_read_position());
				lvl.contents.emplace_back(std::move(temp));
			} break;
		}
	}
}
void military_stockpile_main_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("military_stockpile::main"));
	if(window_bytes == state.ui_state.new_ui_windows.end()) std::abort();
	std::vector<sys::aui_pending_bytes> pending_children;
	auto win_data = read_window_bytes(window_bytes->second.data, window_bytes->second.size, pending_children);
	base_data.position.x = win_data.x_pos;
	base_data.position.y = win_data.y_pos;
	base_data.size.x = win_data.x_size;
	base_data.size.y = win_data.y_size;
	base_data.flags = uint8_t(win_data.orientation);
	layout_window_element::initialize_template(state, win_data.template_id, win_data.grid_size, win_data.auto_close_button);
	while(!pending_children.empty()) {
		auto child_data = read_child_bytes(pending_children.back().data, pending_children.back().size);
		if(child_data.name == "header_text") {
			header_text = std::make_unique<template_label>();
			header_text->parent = this;
			auto cptr = header_text.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "selected_commodity_icon") {
			selected_commodity_icon = std::make_unique<military_stockpile_main_selected_commodity_icon_t>();
			selected_commodity_icon->parent = this;
			auto cptr = selected_commodity_icon.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->gfx_key = child_data.texture;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "stockpile_target_input") {
			stockpile_target_input = std::make_unique<military_stockpile_main_stockpile_target_input_t>();
			stockpile_target_input->parent = this;
			auto cptr = stockpile_target_input.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "confirm_target_button") {
			confirm_target_button = std::make_unique<military_stockpile_main_confirm_target_button_t>();
			confirm_target_button->parent = this;
			auto cptr = confirm_target_button.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->icon_id = child_data.icon_id;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "stockpile_target_error") {
			stockpile_target_error = std::make_unique<military_stockpile_main_stockpile_target_error_t>();
			stockpile_target_error->parent = this;
			auto cptr = stockpile_target_error.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->text_scale = child_data.text_scale;
			cptr->text_is_header = (child_data.text_type == aui_text_type::header);
			cptr->text_alignment = child_data.text_alignment;
			cptr->text_color = child_data.text_color;
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "stockpile_target_editbox_label") {
			stockpile_target_editbox_label = std::make_unique<template_label>();
			stockpile_target_editbox_label->parent = this;
			auto cptr = stockpile_target_editbox_label.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "info_stockpile_view") {
			info_stockpile_view = std::make_unique<template_icon_graphic>();
			info_stockpile_view->parent = this;
			auto cptr = info_stockpile_view.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->color = child_data.table_divider_color;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "info_stockpile_target_view") {
			info_stockpile_target_view = std::make_unique<template_icon_graphic>();
			info_stockpile_target_view->parent = this;
			auto cptr = info_stockpile_target_view.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			cptr->color = child_data.table_divider_color;
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if (child_data.is_lua) { 
			std::string str_name {child_data.name};
			scripted_elements[str_name] = std::make_unique<ui::lua_scripted_element>();
			auto cptr = scripted_elements[str_name].get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->texture_key = child_data.texture;
			cptr->text_scale = child_data.text_scale;
			cptr->text_is_header = (child_data.text_type == aui_text_type::header);
			cptr->text_alignment = child_data.text_alignment;
			cptr->text_color = child_data.text_color;
			cptr->on_update_lname = child_data.text_key;
			if(child_data.tooltip_text_key.length() > 0) {
				cptr->tooltip_key = state.lookup_key(child_data.tooltip_text_key);
			}
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		pending_children.pop_back();
	}
	commodity_grid_g.on_create(state, this);
	page_left_texture_key = win_data.page_left_texture;
	page_right_texture_key = win_data.page_right_texture;
	page_text_color = win_data.page_text_color;
	create_layout_level(state, layout, win_data.layout_data, win_data.layout_data_size);
// BEGIN main::create
// END
}
std::unique_ptr<ui::element_base> make_military_stockpile_main(sys::state& state) {
	auto ptr = std::make_unique<military_stockpile_main_t>();
	ptr->on_create(state);
	return ptr;
}
ui::message_result military_stockpile_grid_item_commodity_icon_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	military_stockpile_grid_item_t& grid_item = *((military_stockpile_grid_item_t*)(parent)); 
	military_stockpile_main_t& main = *((military_stockpile_main_t*)(parent->parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
// BEGIN grid_item::commodity_icon::lbutton_action
	main.stockpile_target_input->set_text(state, std::u16string{ }); // Reset the input field when switching between commodities
	main.selected_commodity = grid_item.value;
	state.game_state_updated.store(true, std::memory_order::release);
// END
	return ui::message_result::consumed;
}
ui::message_result military_stockpile_grid_item_commodity_icon_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	military_stockpile_grid_item_t& grid_item = *((military_stockpile_grid_item_t*)(parent)); 
	military_stockpile_main_t& main = *((military_stockpile_main_t*)(parent->parent)); 
	sound::play_interface_sound(state, sound::get_click_sound(state), state.user_settings.interface_volume* state.user_settings.master_volume);
// BEGIN grid_item::commodity_icon::rbutton_action
	main.stockpile_target_input->set_text(state, std::u16string{ }); // Reset the input field when switching between commodities
	main.selected_commodity = dcon::commodity_id{ };
	state.game_state_updated.store(true, std::memory_order::release);
// END
	return ui::message_result::consumed;
}
void military_stockpile_grid_item_commodity_icon_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	text::add_line(state, contents, tooltip_key);
}
void military_stockpile_grid_item_commodity_icon_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
	if(background_gid) {
		auto& gfx_def = state.ui_defs.gfx[background_gid];
		if(gfx_def.primary_texture_handle) {
			if(gfx_def.get_object_type() == ui::object_type::bordered_rect) {
				ogl::render_bordered_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, true), gfx_def.type_dependent, float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()), base_data.get_rotation(), gfx_def.is_vertically_flipped(), state_is_rtl(state)); 
			} else if(gfx_def.number_of_frames > 1) {
				ogl::render_subsprite(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, true), frame, gfx_def.number_of_frames, float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()), base_data.get_rotation(), gfx_def.is_vertically_flipped(), state_is_rtl(state)); 
			} else {
				ogl::render_textured_rect(state, ui::get_color_modification(this == state.ui_state.under_mouse, false, true), float(x), float(y), float(base_data.size.x), float(base_data.size.y), ogl::get_texture_handle(state, gfx_def.primary_texture_handle, gfx_def.is_partially_transparent()), base_data.get_rotation(), gfx_def.is_vertically_flipped(), state_is_rtl(state)); 
			}
		}
	}
}
void military_stockpile_grid_item_commodity_icon_t::on_update(sys::state& state) noexcept {
	military_stockpile_grid_item_t& grid_item = *((military_stockpile_grid_item_t*)(parent)); 
	military_stockpile_main_t& main = *((military_stockpile_main_t*)(parent->parent)); 
// BEGIN grid_item::commodity_icon::update
	if(!grid_item.value) {
		return;
	}
	frame = state.world.commodity_get_icon(grid_item.value);
// END
}
void military_stockpile_grid_item_commodity_icon_t::on_create(sys::state& state) noexcept {
	if(auto it = state.ui_state.gfx_by_name.find(state.lookup_key(gfx_key)); it != state.ui_state.gfx_by_name.end()) {
		background_gid = it->second;
	}
// BEGIN grid_item::commodity_icon::create
// END
}
void military_stockpile_grid_item_current_stockpile_count_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	military_stockpile_grid_item_t& grid_item = *((military_stockpile_grid_item_t*)(parent)); 
	military_stockpile_main_t& main = *((military_stockpile_main_t*)(parent->parent)); 
// BEGIN grid_item::current_stockpile_count::tooltip
	float current_amount = state.world.nation_get_total_stockpiles(state.local_player_nation, grid_item.value);
	text::add_line(state, contents, "current_stockpile_count_tooltip", text::variable_type::val, text::fp_one_place{ current_amount });
// END
}
void military_stockpile_grid_item_current_stockpile_count_t::on_update(sys::state& state) noexcept {
	military_stockpile_grid_item_t& grid_item = *((military_stockpile_grid_item_t*)(parent)); 
	military_stockpile_main_t& main = *((military_stockpile_main_t*)(parent->parent)); 
// BEGIN grid_item::current_stockpile_count::update
	if(!grid_item.value) {
		return;
	}
	float current_amount = state.world.nation_get_total_stockpiles(state.local_player_nation, grid_item.value);
	set_text(state, text::prettify_float(current_amount));

// END
}
void military_stockpile_grid_item_stockpile_target_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	military_stockpile_grid_item_t& grid_item = *((military_stockpile_grid_item_t*)(parent)); 
	military_stockpile_main_t& main = *((military_stockpile_main_t*)(parent->parent)); 
// BEGIN grid_item::stockpile_target::tooltip
	float current_amount = state.world.nation_get_stockpile_targets(state.local_player_nation, grid_item.value);
	text::add_line(state, contents, "stockpile_target_tooltip", text::variable_type::val, text::fp_one_place{ current_amount });
// END
}
void military_stockpile_grid_item_stockpile_target_t::on_update(sys::state& state) noexcept {
	military_stockpile_grid_item_t& grid_item = *((military_stockpile_grid_item_t*)(parent)); 
	military_stockpile_main_t& main = *((military_stockpile_main_t*)(parent->parent)); 
// BEGIN grid_item::stockpile_target::update
	if(!grid_item.value) {
		return;
	}
	float current_amount = state.world.nation_get_stockpile_targets(state.local_player_nation, grid_item.value);
	set_text(state, text::prettify_float(current_amount));
// END
}
void military_stockpile_grid_item_daily_stockpile_change_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	military_stockpile_grid_item_t& grid_item = *((military_stockpile_grid_item_t*)(parent)); 
	military_stockpile_main_t& main = *((military_stockpile_main_t*)(parent->parent)); 
// BEGIN grid_item::daily_stockpile_change::tooltip
	float current_stockpile = state.world.nation_get_total_stockpiles(state.local_player_nation, grid_item.value);
	float last_stockpile = state.world.nation_get_yesterday_total_stockpiles(state.local_player_nation, grid_item.value);
	text::add_line(state, contents, "stockpile_change_tooltip", text::variable_type::val, text::fp_two_places{ current_stockpile - last_stockpile });
// END
}
void military_stockpile_grid_item_daily_stockpile_change_t::on_update(sys::state& state) noexcept {
	military_stockpile_grid_item_t& grid_item = *((military_stockpile_grid_item_t*)(parent)); 
	military_stockpile_main_t& main = *((military_stockpile_main_t*)(parent->parent)); 
// BEGIN grid_item::daily_stockpile_change::update
	text::substitution_map sub{ };
	float current_stockpile = state.world.nation_get_total_stockpiles(state.local_player_nation, grid_item.value);
	float last_stockpile = state.world.nation_get_yesterday_total_stockpiles(state.local_player_nation, grid_item.value);
	text::add_to_substitution_map(sub, text::variable_type::val, text::fp_one_place{ current_stockpile - last_stockpile });
	set_text(state, text::resolve_string_substitution(state, "stockpile_change", sub));
// END
}
void military_stockpile_grid_item_days_left_t::update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept {
	military_stockpile_grid_item_t& grid_item = *((military_stockpile_grid_item_t*)(parent)); 
	military_stockpile_main_t& main = *((military_stockpile_main_t*)(parent->parent)); 
// BEGIN grid_item::days_left::tooltip
	float current_stockpile = state.world.nation_get_total_stockpiles(state.local_player_nation, grid_item.value);
	float last_stockpile = state.world.nation_get_yesterday_total_stockpiles(state.local_player_nation, grid_item.value);
	float stockpile_change = current_stockpile - last_stockpile;
	float days_supplies_left = (stockpile_change >= 0.0f ? INFINITY : current_stockpile / std::abs(stockpile_change));
	text::add_line(state, contents, "stockpile_days_left_tooltip", text::variable_type::days, text::fp_one_place{ days_supplies_left });
// END
}
void military_stockpile_grid_item_days_left_t::on_update(sys::state& state) noexcept {
	military_stockpile_grid_item_t& grid_item = *((military_stockpile_grid_item_t*)(parent)); 
	military_stockpile_main_t& main = *((military_stockpile_main_t*)(parent->parent)); 
// BEGIN grid_item::days_left::update
	text::substitution_map sub{ };
	float current_stockpile = state.world.nation_get_total_stockpiles(state.local_player_nation, grid_item.value);
	float last_stockpile = state.world.nation_get_yesterday_total_stockpiles(state.local_player_nation, grid_item.value);
	float stockpile_change = current_stockpile - last_stockpile;
	float days_supplies_left = (stockpile_change >= 0.0f ? INFINITY : current_stockpile / std::abs(stockpile_change));
	text::add_to_substitution_map(sub, text::variable_type::days, text::fp_one_place{ days_supplies_left });
	set_text(state, text::resolve_string_substitution(state, "stockpile_days_left", sub));
// END
}
ui::message_result military_stockpile_grid_item_t::on_lbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
ui::message_result military_stockpile_grid_item_t::on_rbutton_down(sys::state& state, int32_t x, int32_t y, sys::key_modifiers mods) noexcept {
	return ui::message_result::consumed;
}
void military_stockpile_grid_item_t::on_update(sys::state& state) noexcept {
	military_stockpile_main_t& main = *((military_stockpile_main_t*)(parent->parent)); 
// BEGIN grid_item::update
	if(!value) {
		flags |= element_base::is_invisible_mask;
	}
// END
	remake_layout(state, true);
}
void military_stockpile_grid_item_t::create_layout_level(sys::state& state, layout_level& lvl, char const* ldata, size_t sz) {
	serialization::in_buffer buffer(ldata, sz);
	buffer.read(lvl.size_x); 
	buffer.read(lvl.size_y); 
	buffer.read(lvl.margin_top); 
	buffer.read(lvl.margin_bottom); 
	buffer.read(lvl.margin_left); 
	buffer.read(lvl.margin_right); 
	buffer.read(lvl.line_alignment); 
	buffer.read(lvl.line_internal_alignment); 
	buffer.read(lvl.type); 
	buffer.read(lvl.page_animation); 
	buffer.read(lvl.interline_spacing); 
	buffer.read(lvl.paged); 
	if(lvl.paged) {
		lvl.page_controls = std::make_unique<page_buttons>();
		lvl.page_controls->for_layout = &lvl;
		lvl.page_controls->parent = this;
		lvl.page_controls->base_data.size.x = int16_t(grid_size * 10);
		lvl.page_controls->base_data.size.y = int16_t(grid_size * 2);
	}
	auto expansion_section = buffer.read_section();
	if(expansion_section)
		expansion_section.read(lvl.template_id);
	if(lvl.template_id == -1 && window_template != -1)
		lvl.template_id = int16_t(state.ui_templates.window_t[window_template].layout_region_definition);
	while(buffer) {
		layout_item_types t;
		buffer.read(t);
		switch(t) {
			case layout_item_types::texture_layer:
			{
				texture_layer temp;
				buffer.read(temp.texture_type);
				buffer.read(temp.texture);
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::control2:
			{
				layout_control temp;
				std::string_view cname = buffer.read<std::string_view>();
				buffer.read(temp.abs_x);
				buffer.read(temp.abs_y);
				buffer.read(temp.absolute_position);
				buffer.read(temp.fill_x);
				buffer.read(temp.fill_y);
				temp.ptr = nullptr;
				if(cname == "commodity_icon") {
					temp.ptr = commodity_icon.get();
				} else
				if(cname == "current_stockpile_count") {
					temp.ptr = current_stockpile_count.get();
				} else
				if(cname == "stockpile_target") {
					temp.ptr = stockpile_target.get();
				} else
				if(cname == "daily_stockpile_change") {
					temp.ptr = daily_stockpile_change.get();
				} else
				if(cname == "value_divider") {
					temp.ptr = value_divider.get();
				} else
				if(cname == "days_left") {
					temp.ptr = days_left.get();
				} else
				{
					std::string str_cname {cname};
					auto found = scripted_elements.find(str_cname);
					if (found != scripted_elements.end()) {
						temp.ptr = found->second.get();
					}
				}
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::window2:
			{
				layout_window temp;
				std::string_view cname = buffer.read<std::string_view>();
				buffer.read(temp.abs_x);
				buffer.read(temp.abs_y);
				buffer.read(temp.absolute_position);
				buffer.read(temp.fill_x);
				buffer.read(temp.fill_y);
				if(cname == "main") {
					temp.ptr = make_military_stockpile_main(state);
				}
				if(cname == "grid_item") {
					temp.ptr = make_military_stockpile_grid_item(state);
				}
				if(cname == "spacer") {
					temp.ptr = make_military_stockpile_spacer(state);
				}
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::glue:
			{
				layout_glue temp;
				buffer.read(temp.type);
				buffer.read(temp.amount);
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::generator2:
			{
				generator_instance temp;
				std::string_view cname = buffer.read<std::string_view>();
				auto gen_details = buffer.read_section(); // ignored
				lvl.contents.emplace_back(std::move(temp));
			} break;
			case layout_item_types::layout:
			{
				sub_layout temp;
				temp.layout = std::make_unique<layout_level>();
				auto layout_section = buffer.read_section();
				create_layout_level(state, *temp.layout, layout_section.view_data() + layout_section.view_read_position(), layout_section.view_size() - layout_section.view_read_position());
				lvl.contents.emplace_back(std::move(temp));
			} break;
		}
	}
}
void military_stockpile_grid_item_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("military_stockpile::grid_item"));
	if(window_bytes == state.ui_state.new_ui_windows.end()) std::abort();
	std::vector<sys::aui_pending_bytes> pending_children;
	auto win_data = read_window_bytes(window_bytes->second.data, window_bytes->second.size, pending_children);
	base_data.position.x = win_data.x_pos;
	base_data.position.y = win_data.y_pos;
	base_data.size.x = win_data.x_size;
	base_data.size.y = win_data.y_size;
	base_data.flags = uint8_t(win_data.orientation);
	layout_window_element::initialize_template(state, win_data.template_id, win_data.grid_size, win_data.auto_close_button);
	ui::element_base::flags |= ui::element_base::wants_update_when_hidden_mask;
	while(!pending_children.empty()) {
		auto child_data = read_child_bytes(pending_children.back().data, pending_children.back().size);
		if(child_data.name == "commodity_icon") {
			commodity_icon = std::make_unique<military_stockpile_grid_item_commodity_icon_t>();
			commodity_icon->parent = this;
			auto cptr = commodity_icon.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->gfx_key = child_data.texture;
			cptr->tooltip_key = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "current_stockpile_count") {
			current_stockpile_count = std::make_unique<military_stockpile_grid_item_current_stockpile_count_t>();
			current_stockpile_count->parent = this;
			auto cptr = current_stockpile_count.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "stockpile_target") {
			stockpile_target = std::make_unique<military_stockpile_grid_item_stockpile_target_t>();
			stockpile_target->parent = this;
			auto cptr = stockpile_target.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "daily_stockpile_change") {
			daily_stockpile_change = std::make_unique<military_stockpile_grid_item_daily_stockpile_change_t>();
			daily_stockpile_change->parent = this;
			auto cptr = daily_stockpile_change.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "value_divider") {
			value_divider = std::make_unique<template_label>();
			value_divider->parent = this;
			auto cptr = value_divider.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if(child_data.name == "days_left") {
			days_left = std::make_unique<military_stockpile_grid_item_days_left_t>();
			days_left->parent = this;
			auto cptr = days_left.get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->template_id = child_data.template_id;
			if(child_data.text_key.length() > 0)
				cptr->default_text = state.lookup_key(child_data.text_key);
			if(child_data.tooltip_text_key.length() > 0)
				cptr->default_tooltip = state.lookup_key(child_data.tooltip_text_key);
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		} else 
		if (child_data.is_lua) { 
			std::string str_name {child_data.name};
			scripted_elements[str_name] = std::make_unique<ui::lua_scripted_element>();
			auto cptr = scripted_elements[str_name].get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->texture_key = child_data.texture;
			cptr->text_scale = child_data.text_scale;
			cptr->text_is_header = (child_data.text_type == aui_text_type::header);
			cptr->text_alignment = child_data.text_alignment;
			cptr->text_color = child_data.text_color;
			cptr->on_update_lname = child_data.text_key;
			if(child_data.tooltip_text_key.length() > 0) {
				cptr->tooltip_key = state.lookup_key(child_data.tooltip_text_key);
			}
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		pending_children.pop_back();
	}
	page_left_texture_key = win_data.page_left_texture;
	page_right_texture_key = win_data.page_right_texture;
	page_text_color = win_data.page_text_color;
	create_layout_level(state, layout, win_data.layout_data, win_data.layout_data_size);
// BEGIN grid_item::create
// END
}
std::unique_ptr<ui::element_base> make_military_stockpile_grid_item(sys::state& state) {
	auto ptr = std::make_unique<military_stockpile_grid_item_t>();
	ptr->on_create(state);
	return ptr;
}
void military_stockpile_spacer_t::render(sys::state & state, int32_t x, int32_t y) noexcept {
}
void military_stockpile_spacer_t::on_update(sys::state& state) noexcept {
	military_stockpile_main_t& main = *((military_stockpile_main_t*)(parent->parent)); 
// BEGIN spacer::update
// END
}
void military_stockpile_spacer_t::on_create(sys::state& state) noexcept {
	auto window_bytes = state.ui_state.new_ui_windows.find(std::string("military_stockpile::spacer"));
	if(window_bytes == state.ui_state.new_ui_windows.end()) std::abort();
	std::vector<sys::aui_pending_bytes> pending_children;
	auto win_data = read_window_bytes(window_bytes->second.data, window_bytes->second.size, pending_children);
	base_data.position.x = win_data.x_pos;
	base_data.position.y = win_data.y_pos;
	base_data.size.x = win_data.x_size;
	base_data.size.y = win_data.y_size;
	base_data.flags = uint8_t(win_data.orientation);
	while(!pending_children.empty()) {
		auto child_data = read_child_bytes(pending_children.back().data, pending_children.back().size);
		if (child_data.is_lua) { 
			std::string str_name {child_data.name};
			scripted_elements[str_name] = std::make_unique<ui::lua_scripted_element>();
			auto cptr = scripted_elements[str_name].get();
			cptr->base_data.position.x = child_data.x_pos;
			cptr->base_data.position.y = child_data.y_pos;
			cptr->base_data.size.x = child_data.x_size;
			cptr->base_data.size.y = child_data.y_size;
			cptr->texture_key = child_data.texture;
			cptr->text_scale = child_data.text_scale;
			cptr->text_is_header = (child_data.text_type == aui_text_type::header);
			cptr->text_alignment = child_data.text_alignment;
			cptr->text_color = child_data.text_color;
			cptr->on_update_lname = child_data.text_key;
			if(child_data.tooltip_text_key.length() > 0) {
				cptr->tooltip_key = state.lookup_key(child_data.tooltip_text_key);
			}
			cptr->parent = this;
			cptr->on_create(state);
			children.push_back(cptr);
			pending_children.pop_back(); continue;
		}
		pending_children.pop_back();
	}
// BEGIN spacer::create
// END
}
std::unique_ptr<ui::element_base> make_military_stockpile_spacer(sys::state& state) {
	auto ptr = std::make_unique<military_stockpile_spacer_t>();
	ptr->on_create(state);
	return ptr;
}
#ifdef __clang__
#pragma clang diagnostic pop
#endif
// LOST-CODE
}
