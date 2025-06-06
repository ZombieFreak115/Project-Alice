#pragma once

#include "commands.hpp"
#include "culture.hpp"
#include "cyto_any.hpp"
#include "dcon_generated.hpp"
#include "gui_common_elements.hpp"
#include "gui_element_types.hpp"
#include "gui_graphics.hpp"
#include "gui_movements_window.hpp"
#include "gui_decision_window.hpp"
#include "gui_reforms_window.hpp"
#include "gui_release_nation_window.hpp"
#include "gui_unciv_reforms_window.hpp"
#include "nations.hpp"
#include "politics.hpp"
#include "system_state.hpp"
#include "text.hpp"
#include <cstdint>
#include <string_view>
#include <vector>

namespace ui {

class nation_national_value_icon : public standard_nation_icon {
public:
	int32_t get_icon_frame(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto nat_val = state.world.nation_get_national_value(nation_id);
		return nat_val.get_icon();
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto nation_id = retrieve<dcon::nation_id>(state, parent);
		auto fat_id = dcon::fatten(state.world, nation_id);
		if(auto name = fat_id.get_national_value().get_name(); state.key_is_localized(name)) {
			auto box = text::open_layout_box(contents, 0);
			text::localised_single_sub_box(state, contents, box, "politics_nationalvalue_tooltip", text::variable_type::nationalvalue, name);
			text::close_layout_box(contents, box);
		}
		auto mod_id = fat_id.get_national_value().id;
		if(bool(mod_id)) {
			modifier_description(state, contents, mod_id);
		}
	}
};


enum class politics_window_tab : uint8_t { reforms = 0x0, movements = 0x1, decisions = 0x2, releasables = 0x3 };

enum class politics_issue_sort_order : uint8_t { name, popular_support, voter_support };

class nation_plurality_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto plurality = state.world.nation_get_plurality(nation_id);
		return std::to_string(int32_t(plurality)) + '%';
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto box = text::open_layout_box(contents);
		text::localised_format_box(state, contents, box, "plurality");
		text::add_line_break_to_layout_box(state, contents, box);
		text::localised_format_box(state, contents, box, "plurality_explanation");
		text::add_line_break_to_layout_box(state, contents, box);
		text::localised_format_box(state, contents, box, "plurality_change");
		text::add_space_to_layout_box(state, contents, box);
		text::add_to_layout_box(state, contents, box, std::string_view{"+"}, text::text_color::green);
		text::add_to_layout_box(state, contents, box,
				text::fp_two_places{state.world.nation_get_demographics(state.local_player_nation, demographics::consciousness) * 0.0222f / std::max(state.world.nation_get_demographics(state.local_player_nation, demographics::total), 1.0f)},
				text::text_color::green);
		text::close_layout_box(contents, box);
		text::add_line(state, contents, "plurality_change_reason");
	}
};

class nation_revanchism_text : public standard_nation_text {
public:
	std::string get_text(sys::state& state, dcon::nation_id nation_id) noexcept override {
		auto revanchism = state.world.nation_get_revanchism(nation_id);
		return std::to_string(int32_t(revanchism * 100.0f)) + '%';
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		text::add_line(state, contents, "revanchism");
		text::add_line(state, contents, "revanchism_explanation");
		text::add_line(state, contents, "revanchism_reason");
	}
};

class politics_unciv_overlay : public standard_nation_icon {
public:
	int32_t get_icon_frame(sys::state& state, dcon::nation_id nation_id) noexcept override {
		set_visible(state, !state.world.nation_get_is_civilized(nation_id));
		return 0;
	}
};

class politics_upper_house_entry : public listbox_row_element_base<dcon::ideology_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "ideology_icon") {
			return make_element_by_type<ideology_plupp>(state, id);
		} else if(name == "ideology_name") {
			return make_element_by_type<generic_name_text<dcon::ideology_id>>(state, id);
		} else if(name == "ideology_perc") {
			return make_element_by_type<nation_ideology_percentage_text>(state, id);
		} else {
			return nullptr;
		}
	}
};

class politics_hold_election : public button_element_base {
public:
	void button_action(sys::state& state) noexcept override {
		command::start_election(state, state.local_player_nation);
	}

	void on_update(sys::state& state) noexcept override {
		disabled = !command::can_start_election(state, state.local_player_nation);
	}
};

class politics_upper_house_listbox : public listbox_element_base<politics_upper_house_entry, dcon::ideology_id> {
protected:
	std::string_view get_row_element_name() override {
		return "ideology_option_window";
	}

public:
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		state.world.for_each_ideology([&](dcon::ideology_id ideology_id) {
			if(state.world.nation_get_upper_house(state.local_player_nation, ideology_id) > 0.f)
				row_contents.push_back(ideology_id);
		});
		update(state);
	}
};

class issue_option_text : public simple_text_element_base {
	void on_update(sys::state& state) noexcept override {
		auto option = retrieve<dcon::issue_option_id>(state, parent);
		set_text(state, text::produce_simple_string(state, state.world.issue_option_get_name(option)));
	}
	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}
	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto option = retrieve<dcon::issue_option_id>(state, parent);
		reform_description(state, contents, option);
	}
};

class politics_party_issue_entry : public listbox_row_element_base<dcon::issue_option_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "issue_group") {
			return make_element_by_type<generic_name_text<dcon::issue_id>>(state, id);
		} else if(name == "issue_name") {
			return make_element_by_type<issue_option_text>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::issue_id>()) {
			payload.emplace<dcon::issue_id>(state.world.issue_option_get_parent_issue(content).id);
			return message_result::consumed;
		}
		return listbox_row_element_base<dcon::issue_option_id>::get(state, payload);
	}
};

class politics_party_issues_listbox : public listbox_element_base<politics_party_issue_entry, dcon::issue_option_id> {
protected:
	std::string_view get_row_element_name() override {
		return "party_issue_option_window";
	}

public:
	void on_update(sys::state& state) noexcept override {
		auto party = retrieve<dcon::political_party_id>(state, parent);
		row_contents.clear();
		for(auto& issue : state.culture_definitions.party_issues) {
			auto issue_option = state.world.political_party_get_party_issues(party, issue);
			if(issue_option) {
				row_contents.push_back(issue_option.id);
			}
		}
		update(state);
	}
};

class politics_choose_party_button : public button_element_base {
public:
	void on_update(sys::state& state) noexcept override {
		if(parent) {
			auto political_party_id = retrieve<dcon::political_party_id>(state, parent);
			set_button_text(state, text::produce_simple_string(state, state.world.political_party_get_name(political_party_id)));

			disabled = !command::can_appoint_ruling_party(state, state.local_player_nation, political_party_id);
		}
	}

	void button_action(sys::state& state) noexcept override {
		if(parent) {
			auto political_party_id = retrieve<dcon::political_party_id>(state, parent);

			command::appoint_ruling_party(state, state.local_player_nation, political_party_id);

			auto lb = parent->parent;
			if(!lb)
				return;
			auto window = lb->parent;
			if(!window)
				return;
			window->set_visible(state, false);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto party = retrieve<dcon::political_party_id>(state, parent);
		{
			auto box = text::open_layout_box(contents);
			text::add_to_layout_box(state, contents, box, state.world.political_party_get_name(party));
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, std::string_view{ "(" });
			text::add_to_layout_box(state, contents, box, state.world.ideology_get_name(state.world.political_party_get_ideology(party)));
			text::add_to_layout_box(state, contents, box, std::string_view{ ")" });
			text::close_layout_box(contents, box);
		}

		/*
		* We do not need to list party issues with full descriptions here since they are displayed literally below the party name with separate working tooltips.
		for(auto pi : state.culture_definitions.party_issues) {
			reform_description(state, contents, state.world.political_party_get_party_issues(party, pi));
			text::add_line_break_to_layout(state, contents);
		}
		*/

		auto cond_0 = (state.current_date >= state.world.political_party_get_start_date(party)) && (state.current_date <= state.world.political_party_get_end_date(party));
		if(state.world.political_party_get_trigger(party)) {
			text::add_line_with_condition(state, contents, "alice_political_party_trigger", cond_0, text::variable_type::date_long_0, state.world.political_party_get_start_date(party),
				text::variable_type::date_long_1, state.world.political_party_get_end_date(party));
			ui::trigger_description(state, contents, state.world.political_party_get_trigger(party), trigger::to_generic(state.local_player_nation), trigger::to_generic(state.local_player_nation), -1);
		} else {
			text::add_line_with_condition(state, contents, "alice_political_party_no_trigger", cond_0, text::variable_type::date_long_0, state.world.political_party_get_start_date(party),
				text::variable_type::date_long_1, state.world.political_party_get_end_date(party));
		}

		// Requirements for changing the party with vanilla texts
		auto cond_1 = politics::can_appoint_ruling_party(state, state.local_player_nation);
		text::add_line_with_condition(state, contents, "POLITICS_CANNOT_SET_RULING_PARTY_RULE", cond_1);

		auto gov = state.world.nation_get_government_type(state.local_player_nation);
		auto new_ideology = state.world.political_party_get_ideology(party);
		auto cond_2 = (state.world.government_type_get_ideologies_allowed(gov) & ::culture::to_bits(new_ideology)) != 0;
		text::add_line_with_condition(state, contents, "POLITICS_NOT_ACCEPTED", cond_2, text::variable_type::which, state.world.ideology_get_name(new_ideology), 0);

		auto cond_3 = politics::is_election_ongoing(state, state.local_player_nation);
		text::add_line_with_condition(state, contents, "POLITICS_CANNOT_SET_RULING_PARTY_IN_ELECTION", cond_3);

		auto last_change = state.world.nation_get_ruling_party_last_appointed(state.local_player_nation);
		auto cond_4 = (!last_change) || (state.current_date >= last_change + 365);
		text::add_line_with_condition(state, contents, "POLITICS_NOT_BEFORE", cond_4, text::variable_type::date, (last_change + 365), 0);
	}
};

class politics_all_party_entry : public listbox_row_element_base<dcon::political_party_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "party_name") {
			return make_element_by_type<politics_choose_party_button>(state, id);
		} else if(name == "party_icon") {
			return make_element_by_type<ideology_plupp>(state, id);
		} else if(name == "issue_listbox") {
			return make_element_by_type<politics_party_issues_listbox>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::ideology_id>()) {
			payload.emplace<dcon::ideology_id>(state.world.political_party_get_ideology(content));
			return message_result::consumed;
		}
		return listbox_row_element_base<dcon::political_party_id>::get(state, payload);
	}
};

class politics_all_party_listbox : public listbox_element_base<politics_all_party_entry, dcon::political_party_id> {
protected:
	std::string_view get_row_element_name() override {
		return "party_window";
	}

	void on_update(sys::state& state) noexcept override {
		row_contents.clear();
		nations::get_active_political_parties(state, state.local_player_nation, row_contents);
		update(state);
	}
};

class politics_all_party_window : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		base_data.position.y -= 66;
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "all_party_window_close") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "party_listbox") {
			return make_element_by_type<politics_all_party_listbox>(state, id);
		} else {
			return nullptr;
		}
	}
};

class politics_change_party_button : public standard_nation_button {
private:
	politics_all_party_window* all_party_window = nullptr;

public:
	void on_update(sys::state& state) noexcept override {
		auto party = state.world.nation_get_ruling_party(state.local_player_nation);
		set_button_text(state, text::produce_simple_string(state, state.world.political_party_get_name(party)));
	}

	void button_action(sys::state& state) noexcept override {
		if(all_party_window == nullptr) {
			auto ptr = make_element_by_type<politics_all_party_window>(state, "all_party_window");
			all_party_window = static_cast<politics_all_party_window*>(ptr.get());
			parent->parent->add_child_to_front(std::move(ptr));
		} else {
			all_party_window->set_visible(state, !all_party_window->is_visible());
		}
		if(all_party_window && all_party_window->is_visible()) {
			all_party_window->impl_on_update(state);
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		auto ruling_party = state.world.nation_get_ruling_party(state.local_player_nation);
		{
			auto box = text::open_layout_box(contents);
			text::add_to_layout_box(state, contents, box, state.world.political_party_get_name(ruling_party));
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, std::string_view{ "(" });
			text::add_to_layout_box(state, contents, box, state.world.ideology_get_name(state.world.political_party_get_ideology(ruling_party)));
			text::add_to_layout_box(state, contents, box, std::string_view{ ")" });
			text::close_layout_box(contents, box);
		}
		for(auto pi : state.culture_definitions.party_issues) {
			reform_description(state, contents, ruling_party.get_party_issues(pi));
			text::add_line_break_to_layout(state, contents);
		}
	}
};

class politics_ruling_party_window : public window_element_base {
public:
	void on_create(sys::state& state) noexcept override {
		window_element_base::on_create(state);
		base_data.position = state.ui_defs.gui[state.ui_state.defs_by_name.find(state.lookup_key("ruling_party_pos"))->second.definition].position;
	}

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "politics_selectparty_entrybg") {
			return make_element_by_type<invisible_element>(state, id);
		} else if(name == "party_name") {
			return make_element_by_type<politics_change_party_button>(state, id);
		} else if(name == "party_icon") {
			return make_element_by_type<nation_ruling_party_ideology_plupp>(state, id);
		} else if(name == "issue_listbox") {
			return make_element_by_type<politics_party_issues_listbox>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<dcon::political_party_id>()) {
			payload.emplace<dcon::political_party_id>(state.world.nation_get_ruling_party(state.local_player_nation));
			return message_result::consumed;
		}
		return window_element_base::get(state, payload);
	}
};

class nation_government_description_text : public generic_multiline_text<dcon::nation_id> {
public:
	void populate_layout(sys::state& state, text::endless_layout& contents, dcon::nation_id nation_id) noexcept override {
		if(politics::can_appoint_ruling_party(state, nation_id)) {
			auto box = text::open_layout_box(contents);
			text::localised_format_box(state, contents, box, std::string_view("can_appoint_ruling_party"));
			text::close_layout_box(contents, box);
		}
		if(!politics::has_elections(state, nation_id)) {
			auto box = text::open_layout_box(contents);
			text::localised_format_box(state, contents, box, std::string_view("term_for_life"));
			text::close_layout_box(contents, box);
		} else if(politics::is_election_ongoing(state, nation_id)) {
			auto box = text::open_layout_box(contents);
			text::localised_format_box(state, contents, box, std::string_view("election_info_in_gov"));
			text::close_layout_box(contents, box);
		} else {
			auto box = text::open_layout_box(contents);
			auto election_start_date = politics::next_election_date(state, nation_id);
			text::localised_format_box(state, contents, box, std::string_view("next_election"));
			text::add_to_layout_box(state, contents, box, std::string(":"), text::text_color::black);
			text::add_space_to_layout_box(state, contents, box);
			text::add_to_layout_box(state, contents, box, text::date_to_string(state, election_start_date), black_text ? text::text_color::black : text::text_color::white);
			text::close_layout_box(contents, box);
		}
	}
};

class standard_nation_issue_option_text : public simple_text_element_base {
public:
	virtual std::string get_text(sys::state& state, dcon::issue_option_id issue_option_id) noexcept {
		return "";
	}

	void on_update(sys::state& state) noexcept override {
		auto content = retrieve<dcon::issue_option_id>(state, parent);
		set_text(state, get_text(state, content));
	}
};

class issue_option_popular_support : public standard_nation_issue_option_text {
public:
	std::string get_text(sys::state& state, dcon::issue_option_id issue_option_id) noexcept override {
		return text::format_percentage(politics::get_popular_support(state, state.local_player_nation, issue_option_id), 1);
	}
};

class issue_option_voter_support : public standard_nation_issue_option_text {
public:
	std::string get_text(sys::state& state, dcon::issue_option_id issue_option_id) noexcept override {
		return text::format_percentage(politics::get_voter_support(state, state.local_player_nation, issue_option_id), 1);
	}
};

class politics_issue_support_item : public listbox_row_element_base<dcon::issue_option_id> {
public:
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "issue_name") {
			return make_element_by_type<generic_name_text<dcon::issue_option_id>>(state, id);
		} else if(name == "people_perc") {
			return make_element_by_type<issue_option_popular_support>(state, id);
		} else if(name == "voters_perc") {
			return make_element_by_type<issue_option_voter_support>(state, id);
		} else {
			return nullptr;
		}
	}
};

class politics_issue_support_listbox : public listbox_element_base<politics_issue_support_item, dcon::issue_option_id> {
protected:
	std::string_view get_row_element_name() override {
		return "issue_option_window";
	}
public:
	void on_create(sys::state& state) noexcept override {
		listbox_element_base<politics_issue_support_item, dcon::issue_option_id>::on_create(state);
		state.world.for_each_issue_option([&](dcon::issue_option_id io_id) { row_contents.push_back(io_id); });
		update(state);
	}
};

class politics_hold_election_button : public button_element_base {
	void on_update(sys::state& state) noexcept override {
		disabled = !command::can_start_election(state, state.local_player_nation);
	}

	void button_action(sys::state& state) noexcept override {
		command::start_election(state, state.local_player_nation);
	}
};


class national_modifier_icon : public image_element_base {
public:
	bool get_horizontal_flip(sys::state& state) noexcept override {
		return false; //never flip
	}

	void on_update(sys::state& state) noexcept override {
		sys::dated_modifier mod = retrieve< sys::dated_modifier>(state, parent);
		if(mod.mod_id) {
			frame = int8_t(state.world.modifier_get_icon(mod.mod_id)) + 1;
		}
	}

	tooltip_behavior has_tooltip(sys::state& state) noexcept override {
		return tooltip_behavior::variable_tooltip;
	}

	void update_tooltip(sys::state& state, int32_t x, int32_t y, text::columnar_layout& contents) noexcept override {
		sys::dated_modifier mod = retrieve< sys::dated_modifier>(state, parent);
		if(mod.mod_id) {
			text::add_line(state, contents, state.world.modifier_get_name(mod.mod_id));
			modifier_description(state, contents, mod.mod_id, 15);
		}
		if(mod.expiration) {
			text::add_line(state, contents, "expires_on", text::variable_type::date, mod.expiration);
		}
	}
};

class national_modifier_win : public window_element_base {
public:
	sys::dated_modifier mod;

	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "country_modifier_icon") {
			return make_element_by_type<national_modifier_icon>(state, id);
		} else {
			return nullptr;
		}
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<sys::dated_modifier>()) {
			payload.emplace<sys::dated_modifier>(mod);
			return message_result::consumed;
		}
		return message_result::unseen;
	}

};

class national_modifiers : public overlapping_listbox_element_base<national_modifier_win, sys::dated_modifier> {
public:
	std::string_view get_row_element_name() override {
		return "country_modifier_item";
	}
	void update_subwindow(sys::state& state, national_modifier_win& subwindow, sys::dated_modifier content) override {
		subwindow.mod = content;
	}
	void on_update(sys::state& state) noexcept override {
		row_contents.clear();

		for(auto mods : state.world.nation_get_current_modifiers(state.local_player_nation)) {
			row_contents.push_back(mods);
		}

		for(auto tm : state.national_definitions.triggered_modifiers) {
			if(tm.trigger_condition && tm.linked_modifier) {
				auto trigger_condition_satisfied =
					trigger::evaluate(state, tm.trigger_condition, trigger::to_generic(state.local_player_nation), trigger::to_generic(state.local_player_nation), 0);
				if(trigger_condition_satisfied)
					row_contents.push_back(sys::dated_modifier{ sys::date{}, tm.linked_modifier });
			}
		}
		
		update(state);
	}
};


class politics_issue_sort_button : public button_element_base {
public:
	politics_issue_sort_order order = politics_issue_sort_order::name;
	void button_action(sys::state& state) noexcept override {
		send(state, parent, order);
	}
};

class politics_window : public generic_tabbed_window<politics_window_tab> {
private:
	reforms_window* reforms_win = nullptr;
	unciv_reforms_window* unciv_reforms_win = nullptr;
	movements_window* movements_win = nullptr;
	decision_window* decision_win = nullptr;
	release_nation_window* release_nation_win = nullptr;
	politics_issue_support_listbox* issues_listbox = nullptr;
	element_base* release_win = nullptr;
	dcon::nation_id release_nation_id{};

public:
	void on_create(sys::state& state) noexcept override {
		generic_tabbed_window::on_create(state);
		{
			auto ptr = make_element_by_type<politics_ruling_party_window>(state, "party_window");
			add_child_to_front(std::move(ptr));
		}
		{
			auto ptr = make_element_by_type<politics_release_nation_window>(state, "releaseconfirm");
			release_win = ptr.get();
			add_child_to_front(std::move(ptr));
		}
		set_visible(state, false);
	}
	std::unique_ptr<element_base> make_child(sys::state& state, std::string_view name, dcon::gui_def_id id) noexcept override {
		if(name == "main_bg") {
			return make_element_by_type<image_element_base>(state, id);
		} else if(name == "bg_politics") {
			return make_element_by_type<opaque_element_base>(state, id);
		} else if(name == "close_button") {
			return make_element_by_type<generic_close_button>(state, id);
		} else if(name == "reforms_tab") {
			auto ptr = make_element_by_type<generic_tab_button<politics_window_tab>>(state, id);
			ptr->target = politics_window_tab::reforms;
			return ptr;
		} else if(name == "movements_tab") {
			auto ptr = make_element_by_type<generic_tab_button<politics_window_tab>>(state, id);
			ptr->target = politics_window_tab::movements;
			return ptr;
		} else if(name == "decisions_tab") {
			auto ptr = make_element_by_type<generic_tab_button<politics_window_tab>>(state, id);
			ptr->target = politics_window_tab::decisions;
			return ptr;
		} else if(name == "release_nations_tab") {
			auto ptr = make_element_by_type<generic_tab_button<politics_window_tab>>(state, id);
			ptr->target = politics_window_tab::releasables;
			return ptr;
		} else if(name == "reforms_window") {
			auto ptr = make_element_by_type<reforms_window>(state, id);
			reforms_win = ptr.get();
			ptr->set_visible(state, true);
			return ptr;
		} else if(name == "movements_window") {
			auto ptr = make_element_by_type<movements_window>(state, id);
			movements_win = ptr.get();
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "decision_window") {
			auto ptr = make_element_by_type<decision_window>(state, id);
			decision_win = ptr.get();
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "release_nation") {
			auto ptr = make_element_by_type<release_nation_window>(state, id);
			release_nation_win = ptr.get();
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "unciv_reforms_window") {
			auto ptr = make_element_by_type<unciv_reforms_window>(state, id);
			unciv_reforms_win = ptr.get();
			ptr->set_visible(state, false);
			return ptr;
		} else if(name == "country_modifier_overlappingbox") {
			return make_element_by_type<national_modifiers>(state, id);
		} else if(name == "government_name") {
			return make_element_by_type<nation_government_type_text>(state, id);
		} else if(name == "government_desc") {
			return make_element_by_type<nation_government_description_text>(state, id);
		} else if(name == "national_value") {
			return make_element_by_type<nation_national_value_icon>(state, id);
		} else if(name == "plurality_value") {
			return make_element_by_type<nation_plurality_text>(state, id);
		} else if(name == "revanchism_value") {
			return make_element_by_type<nation_revanchism_text>(state, id);
		} else if(name == "can_do_social_reforms") {
			return make_element_by_type<nation_can_do_social_reform_text>(state, id);
		} else if(name == "social_reforms_bock") {
			return make_element_by_type<nation_can_do_social_reform_icon>(state, id);
		} else if(name == "can_do_political_reforms") {
			return make_element_by_type<nation_can_do_political_reform_text>(state, id);
		} else if(name == "political_reforms_bock") {
			return make_element_by_type<nation_can_do_political_reform_icon>(state, id);
		} else if(name == "chart_upper_house") {
			return make_element_by_type<upper_house_piechart>(state, id);
		} else if(name == "chart_voters_ideologies") {
			return make_element_by_type<voter_ideology_piechart>(state, id);
		} else if(name == "chart_people_ideologies") {
			return make_element_by_type<ideology_piechart<dcon::nation_id>>(state, id);
		} else if(name == "upperhouse_ideology_listbox") {
			return make_element_by_type<politics_upper_house_listbox>(state, id);
		} else if(name == "unciv_overlay") {
			return make_element_by_type<politics_unciv_overlay>(state, id);
		} else if(name == "hold_election") {
			return make_element_by_type<politics_hold_election_button>(state, id);
		} else if(name == "issue_listbox") {
			auto ptr = make_element_by_type<politics_issue_support_listbox>(state, id);
			issues_listbox = ptr.get();
			return ptr;
		} else if(name == "sort_by_issue_name") {
			auto ptr = make_element_by_type<politics_issue_sort_button>(state, id);
			ptr->order = politics_issue_sort_order::name;
			return ptr;
		} else if(name == "sort_by_people") {
			auto ptr = make_element_by_type<politics_issue_sort_button>(state, id);
			ptr->order = politics_issue_sort_order::popular_support;
			return ptr;
		} else if(name == "sort_by_voters") {
			auto ptr = make_element_by_type<politics_issue_sort_button>(state, id);
			ptr->order = politics_issue_sort_order::voter_support;
			return ptr;
		} else {
			return nullptr;
		}
	}
	void on_update(sys::state& state) noexcept override {
		if(state.world.nation_get_is_civilized(state.local_player_nation) && unciv_reforms_win->is_visible()) {
			unciv_reforms_win->set_visible(state, false);
			reforms_win->set_visible(state, true);
		} else if(!state.world.nation_get_is_civilized(state.local_player_nation) && reforms_win->is_visible()) {
			reforms_win->set_visible(state, false);
			unciv_reforms_win->set_visible(state, true);
		}
	}

	void hide_sub_windows(sys::state& state) {
		reforms_win->set_visible(state, false);
		unciv_reforms_win->set_visible(state, false);
		decision_win->set_visible(state, false);
		movements_win->set_visible(state, false);
		release_nation_win->set_visible(state, false);
	}

	message_result get(sys::state& state, Cyto::Any& payload) noexcept override {
		if(payload.holds_type<politics_window_tab>()) {
			auto enum_val = any_cast<politics_window_tab>(payload);
			hide_sub_windows(state);
			switch(enum_val) {
			case politics_window_tab::reforms:
				if(state.world.nation_get_is_civilized(state.local_player_nation)) {
					reforms_win->set_visible(state, true);
				} else {
					unciv_reforms_win->set_visible(state, true);
				}
				break;
			case politics_window_tab::movements:
				movements_win->set_visible(state, true);
				break;
			case politics_window_tab::decisions:
				decision_win->set_visible(state, true);
				break;
			case politics_window_tab::releasables:
				release_nation_win->set_visible(state, true);
				break;
			default:
				break;
			}
			active_tab = enum_val;
			return message_result::consumed;
		} else if(payload.holds_type<dcon::nation_id>()) {
			payload.emplace<dcon::nation_id>(state.local_player_nation);
			return message_result::consumed;
		} else if(payload.holds_type<release_query_wrapper>()) {
			payload.emplace<dcon::nation_id>(release_nation_id);
			return message_result::consumed;
		} else if(payload.holds_type<release_emplace_wrapper>()) {
			release_nation_id = any_cast<release_emplace_wrapper>(payload).content;
			release_win->set_visible(state, true);
			move_child_to_front(release_win);
			return message_result::consumed;
		} else if(payload.holds_type<politics_issue_sort_order>()) {
			auto enum_val = any_cast<politics_issue_sort_order>(payload);
			switch(enum_val) {
			case politics_issue_sort_order::name:
				std::sort(issues_listbox->row_contents.begin(), issues_listbox->row_contents.end(),
						[&](dcon::issue_option_id a, dcon::issue_option_id b) {
							auto a_name = text::get_name_as_string(state, dcon::fatten(state.world, a));
							auto b_name = text::get_name_as_string(state, dcon::fatten(state.world, b));
							return a_name < b_name;
						});
				issues_listbox->update(state);
				break;
			case politics_issue_sort_order::popular_support:
				std::sort(issues_listbox->row_contents.begin(), issues_listbox->row_contents.end(),
						[&](dcon::issue_option_id a, dcon::issue_option_id b) {
							auto a_support = politics::get_popular_support(state, state.local_player_nation, a);
							auto b_support = politics::get_popular_support(state, state.local_player_nation, b);
							return a_support > b_support;
						});
				issues_listbox->update(state);
				break;
			case politics_issue_sort_order::voter_support:
				std::sort(issues_listbox->row_contents.begin(), issues_listbox->row_contents.end(),
						[&](dcon::issue_option_id a, dcon::issue_option_id b) {
							auto a_support = politics::get_voter_support(state, state.local_player_nation, a);
							auto b_support = politics::get_voter_support(state, state.local_player_nation, b);
							return a_support > b_support;
						});
				issues_listbox->update(state);
				break;
			default:
				break;
			}
			return message_result::consumed;
		}
		return message_result::unseen;
	}
};

} // namespace ui
