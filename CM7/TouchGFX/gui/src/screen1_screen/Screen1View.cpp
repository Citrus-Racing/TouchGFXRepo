#include <gui/screen1_screen/Screen1View.hpp>


// Hi there!
// Remember to specify which letters you want loaded in TouchGFX.
/// I disabled the Fallback "?" unknown character, so you'll see nothing if you don't. | No you didnt


constexpr int16_t Screen1View::PROFILE_ROW_Y[5];
constexpr uint8_t Screen1View::BOX_SCROLL_ORDER[NUM_CUSTOM_BOXES];

const Screen1View::ColorEntry Screen1View::COLOR_PALETTE[CR_NUM_PALETTE_COLORS] = {
    {  10,  10,  10,  255, 255, 255 },  // 0: Black   - white border
    { 255, 255, 255,  180, 180, 180 },  // 1: White   - dark gray border
    { 200,   0,   0,  140,   0,   0 },  // 2: Red     - dark red border
    {   0, 180,   0,    0, 126,   0 },  // 3: Green   - dark green border
    {   0,   0, 200,    0,   0, 140 },  // 4: Blue    - dark blue border
    { 200, 200,   0,  140, 140,   0 },  // 5: Yellow  - dark yellow border
    { 220, 140,   0,  154,  98,   0 },  // 6: Orange  - dark orange border
    { 140,   0, 200,   98,   0, 140 },  // 7: Purple  - dark purple border
};

// Bounding rectangles for the widget selector around each customizer box.
// Derived from _1 box positions with ~15px margin.
const Screen1View::BoxGeometry Screen1View::CUSTOMIZER_BOX_POS[NUM_CUSTOM_BOXES] = {
    {  28,  59, 314, 106 },  // 0: bx_speed_1  (43,74,284,76)
    { 355,  14, 314,  88 },  // 1: bx_rpm_1    (370,29,284,58)
    { 355, 101, 314, 278 },  // 2: bx_gear_1   (370,116,284,248)
    { 279, 508, 472,  68 },  // 3: dbx_fuel_1  (294,523,442,38)
    { 682,  59, 314, 106 },  // 4: vx_oilt_1   (697,74,284,76)
    {  28, 172, 314, 106 },  // 5: bx_oilp_1   (43,187,284,76)
    {  28, 280, 314, 106 },  // 6: bx_batt_1   (43,295,284,76)
    {  28, 387, 314, 106 },  // 7: bx_time_1   (43,402,284,76)
    { 682, 172, 314, 106 },  // 8: bx_DRS_1    (697,187,284,76)
    { 682, 280, 314, 106 },  // 9: bx_alag_1   (697,295,284,76)
    { 682, 387, 314, 106 },  // 10: bx_lch_1   (697,402,284,76)
};

Screen1View::Screen1View() {}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();

    // Read the stored fuel level from the model and paint the initial state.
    uint8_t stored = presenter->getFuelLevel();
    pending_fuel_tenths = stored;
    refresh_fuel_display(stored);

    // Apply active profile colors on boot
    uint8_t activeIdx = presenter->getActiveProfileIndex();
    if(activeIdx < CR_MAX_PROFILES && presenter->isProfileFilled(activeIdx)){
        CR_profile_t profile;
        presenter->getProfile(activeIdx, &profile);
        apply_colors_to_dashboard(profile.box_colors);
    }
}

void Screen1View::tearDownScreen()
{
    Screen1ViewBase::tearDownScreen();
}

void Screen1View::update_CAN_info(CR_CAN_vals * CAN_data){
	CAN_data->CR_new_info_flag = 0; // acknowledge that new data is written
	Unicode::itoa(CAN_data->engine_speed, dtxt_rpmBuffer, 11, 10);
	Unicode::itoa(CAN_data->gear_output_shaft_speed, dtxt_speedBuffer, 11, 10);
	Unicode::itoa(CAN_data->gear, dtxt_gearBuffer, 11, 10);
	Unicode::itoa(((CAN_data->engine_oil_temperature-40)*1.8)+32, dtxt_oiltBuffer, 11, 10); // -40 celsius offset + America offset.
	Unicode::itoa((CAN_data->engine_oil_pressure)/68.94757, dtxt_oilpBuffer, 11, 10); // 0.1kpa to 1 PSI
	Unicode::itoa(CAN_data->ecu_battery_voltage*0.1, dtxt_battBuffer, 11, 10); // 0.1v scale
	Unicode::strncpy(dtxt_alagBuffer, (CAN_data->anti_lag_state ? "OFF" : "ON"), 11);
	Unicode::strncpy(dtxt_lchBuffer, (CAN_data->launch_state ? "OFF" : "ON"), 11);
	dtxt_lch.invalidate();
	dtxt_alag.invalidate();
	dtxt_batt.invalidate();
	dtxt_oilp.invalidate();
	dtxt_oilt.invalidate();
	dtxt_gear.invalidate();
	dtxt_speed.invalidate();
	dtxt_rpm.invalidate();
}

void Screen1View::open_menu(){
	// If the customizer is open, menu button = save + apply + close
	if(customize_profile_open){
		save_and_apply_customizer();
		return;
	}

	// If the fuel sub-menu is open, the menu button saves the pending fuel
	// level and closes the fuel sub-menu.
	if(fuel_menu_open){
		presenter->saveFuelLevel(pending_fuel_tenths);
		fuel_menu_open = false;
		set_fuel_container.setVisible(false);
		set_fuel_container.invalidate();
		return;
	}

	if(driver_profiles_menu_open){
		uint8_t row = profile_selector_index / 2;
		bool is_edit_column = (profile_selector_index % 2) != 0;
		bool filled = presenter->isProfileFilled(row);

		if(!is_edit_column){
			// STATUS column: select a filled profile
			if(filled){
				CR_profile_t profile;
				presenter->getProfile(row, &profile);
				apply_colors_to_dashboard(profile.box_colors);
				presenter->setActiveProfile(row);
				presenter->saveAll();

				driver_profiles_menu_open = false;
				driver_profiles_container.setVisible(false);
				driver_profiles_container.invalidate();
			}
		} else {
			// EDIT/ADD column: open customizer
			open_customizer_for_profile(row);
		}
		return;
	}

	// If the main menu is open, select the highlighted item
	if(main_menu_open){
		int16_t y_val = dbx_menu_selection.getY();
		if(y_val == 89){
			// "Set Fuel" selected - close main menu and open the fuel sub-menu
			pending_fuel_tenths = presenter->getFuelLevel();
			refresh_fuel_display(pending_fuel_tenths);
			main_menu_open = false;
			menu_scontainer.setVisible(false);
			menu_scontainer.invalidate();
			set_fuel_container.setVisible(true);
			set_fuel_container.invalidate();
			fuel_menu_open = true;
		} else if(y_val == 139){
			// "Driver Profiles" selected - close main menu and open profiles
			main_menu_open = false;
			menu_scontainer.setVisible(false);
			menu_scontainer.invalidate();
			profile_selector_index = 0;
			update_profile_selector();
			refresh_profile_status_texts();
			driver_profiles_container.setVisible(true);
			driver_profiles_container.invalidate();
			driver_profiles_menu_open = true;
		}

		return;
	}

	// Otherwise open the main menu
	main_menu_open = true;
	menu_scontainer.setVisible(true);
	dbx_menu_selection.setY(89);
	menu_scontainer.invalidate();
}

void Screen1View::close_menu(){
	// close_menu is triggered by the back button.
	// It always just closes whatever is currently open without saving.

	if(customize_profile_open){
		// Discard changes, go back to driver profiles menu
		customize_profile_open = false;
		color_edit_mode = false;
		display_customizer_container.setVisible(false);
		display_customizer_container.invalidate();

		profile_selector_index = 0;
		update_profile_selector();
		refresh_profile_status_texts();
		driver_profiles_container.setVisible(true);
		driver_profiles_container.invalidate();
		driver_profiles_menu_open = true;
		return;
	}

	if(fuel_menu_open){
		fuel_menu_open = false;
		set_fuel_container.setVisible(false);
		set_fuel_container.invalidate();

		// Discard pending changes - restore display to last saved value
		pending_fuel_tenths = presenter->getFuelLevel();
		refresh_fuel_display(pending_fuel_tenths);
		return;
	}

	if(driver_profiles_menu_open){
		driver_profiles_menu_open = false;
		driver_profiles_container.setVisible(false);
		driver_profiles_container.invalidate();
		return;
	}

	if(main_menu_open){
		main_menu_open = false;
		menu_scontainer.setVisible(false);
		menu_scontainer.invalidate();
	}
}

void Screen1View::cursor_up(){
	if(customize_profile_open){
		uint8_t box = BOX_SCROLL_ORDER[customizer_scroll_pos];
		if(color_edit_mode){
			// Cycle color up for the selected box
			pending_colors[box]++;
			if(pending_colors[box] >= CR_NUM_PALETTE_COLORS)
				pending_colors[box] = 0;
			apply_single_box_color_customizer(box, pending_colors[box]);
		} else {
			if(customizer_scroll_pos > 0){
				customizer_scroll_pos--;
				update_widget_selector();
			}
		}
		return;
	}

	// When the fuel sub-menu is open, encoder up increments by 1 tenth (0.1L)
	if(fuel_menu_open){
		if(pending_fuel_tenths < FUEL_MAX_TENTHS){
			pending_fuel_tenths++;
			refresh_fuel_display(pending_fuel_tenths);
		}
		return;
	}

	if(driver_profiles_menu_open){
		if(profile_selector_index > 0){
			profile_selector_index--;
			update_profile_selector();
		}
		return;
	}

	int16_t y_val = dbx_menu_selection.getY();
	if (y_val > 89){
		dbx_menu_selection.setY(y_val-50);
	}
	menu_scontainer.invalidate();
}

void Screen1View::cursor_down(){
	if(customize_profile_open){
		uint8_t box = BOX_SCROLL_ORDER[customizer_scroll_pos];
		if(color_edit_mode){
			// Cycle color down for the selected box
			if(pending_colors[box] == 0)
				pending_colors[box] = CR_NUM_PALETTE_COLORS - 1;
			else
				pending_colors[box]--;
			apply_single_box_color_customizer(box, pending_colors[box]);
		} else {
			if(customizer_scroll_pos < NUM_CUSTOM_BOXES - 1){
				customizer_scroll_pos++;
				update_widget_selector();
			}
		}
		return;
	}

	// When the fuel sub-menu is open, encoder down decrements by 1 tenth (0.1L)
	if(fuel_menu_open){
		if(pending_fuel_tenths > 0){
			pending_fuel_tenths--;
			refresh_fuel_display(pending_fuel_tenths);
		}
		return;
	}

	if(driver_profiles_menu_open){
		if(profile_selector_index < PROFILE_SELECTOR_MAX){
			profile_selector_index++;
			update_profile_selector();
		}
		return;
	}

	int16_t y_val = dbx_menu_selection.getY();
	if (y_val < 245){
		dbx_menu_selection.setY(y_val+50);
	}
	menu_scontainer.invalidate();
}

// Only used in the display customizer
void Screen1View::encoder_click(){
	if(customize_profile_open){
		// Toggle color edit mode for the currently selected box
		color_edit_mode = !color_edit_mode;

		// Red border when editing, white when browsing
		if(color_edit_mode){
			dbx_widget_selector.setBorderColor(touchgfx::Color::getColorFromRGB(255, 0, 0));
		} else {
			dbx_widget_selector.setBorderColor(touchgfx::Color::getColorFromRGB(255, 255, 255));
		}
		dbx_widget_selector.invalidate();
		return;
	}

}

void Screen1View::update_profile_selector(){
	uint8_t row = profile_selector_index / 2;
	bool is_edit = (profile_selector_index % 2) != 0;

	dbx_profile_selector.invalidate();
	dbx_profile_selector.setX(is_edit ? PROFILE_EDIT_X : PROFILE_STATUS_X);
	dbx_profile_selector.setY(PROFILE_ROW_Y[row]);
	dbx_profile_selector.setWidth(is_edit ? PROFILE_EDIT_W : PROFILE_STATUS_W);
	dbx_profile_selector.invalidate();
}

void Screen1View::refresh_fuel_display(uint8_t tenths){
	// Display as "X.X"
	uint8_t whole    = tenths / 10;
	uint8_t fraction = tenths % 10;
	Unicode::snprintf(dtxt_set_levelBuffer, DTXT_SET_LEVEL_SIZE, "%d.%d", whole, fraction);
	dtxt_set_level.invalidate();

	// Update the fuel bar. Invalidate the full original width first so that
	// shrinking the bar clears the pixels that are no longer covered.
	dbx_fuel.setWidth(FUEL_BAR_MAX_WIDTH);
	dbx_fuel.invalidate();
	uint16_t bar_width = (uint16_t)(((uint32_t)tenths * FUEL_BAR_MAX_WIDTH) / FUEL_MAX_TENTHS);
	dbx_fuel.setWidth(bar_width);
	dbx_fuel.invalidate();
}

void Screen1View::refresh_profile_status_texts(){
	touchgfx::Unicode::UnicodeChar* status_bufs[5] = {
		txt_profile_status_1Buffer, txt_profile_status_2Buffer,
		txt_profile_status_3Buffer, txt_profile_status_4Buffer,
		txt_profile_status_5Buffer
	};
	touchgfx::Unicode::UnicodeChar* edit_bufs[5] = {
		txt_profile_edit_1Buffer, txt_profile_edit_2Buffer,
		txt_profile_edit_3Buffer, txt_profile_edit_4Buffer,
		txt_profile_edit_5Buffer
	};
	touchgfx::TextAreaWithOneWildcard* status_txts[5] = {
		&txt_profile_status_1, &txt_profile_status_2,
		&txt_profile_status_3, &txt_profile_status_4,
		&txt_profile_status_5
	};
	touchgfx::TextAreaWithOneWildcard* edit_txts[5] = {
		&txt_profile_edit_1, &txt_profile_edit_2,
		&txt_profile_edit_3, &txt_profile_edit_4,
		&txt_profile_edit_5
	};

	uint8_t activeIdx = presenter->getActiveProfileIndex();
	for(int i = 0; i < 5; i++){
		bool filled = presenter->isProfileFilled(i);
		if(filled && i == activeIdx){
			Unicode::strncpy(status_bufs[i], "In Use", 10);
		} else if(filled){
			Unicode::strncpy(status_bufs[i], "Filled", 10);
		} else {
			Unicode::strncpy(status_bufs[i], "Empty", 10);
		}
		Unicode::strncpy(edit_bufs[i], filled ? "Edit" : "Add", 10);
		status_txts[i]->invalidate();
		edit_txts[i]->invalidate();
	}
}

void Screen1View::apply_colors_to_dashboard(const uint8_t colors[NUM_CUSTOM_BOXES]){
	touchgfx::BoxWithBorder* boxes[10] = {
		&bx_speed, &bx_rpm, &bx_gear, /* skip 3 */ &vx_oilt,
		&bx_oilp, &bx_batt, &bx_time, &bx_DRS, &bx_alag, &bx_lch
	};
	// Indices into colors[] for the 10 BoxWithBorder widgets
	static const uint8_t bwb_idx[10] = { 0, 1, 2, 4, 5, 6, 7, 8, 9, 10 };

	for(int i = 0; i < 10; i++){
		const ColorEntry& c = COLOR_PALETTE[colors[bwb_idx[i]]];
		boxes[i]->setColor(touchgfx::Color::getColorFromRGB(c.r, c.g, c.b));
		boxes[i]->setBorderColor(touchgfx::Color::getColorFromRGB(c.br, c.bg, c.bb));
		boxes[i]->invalidate();
	}

	// Index 3: dbx_fuel is a plain Box (fill only, no border)
	const ColorEntry& fc = COLOR_PALETTE[colors[3]];
	dbx_fuel.setColor(touchgfx::Color::getColorFromRGB(fc.r, fc.g, fc.b));
	dbx_fuel.invalidate();
}

void Screen1View::apply_colors_to_customizer(const uint8_t colors[NUM_CUSTOM_BOXES]){
	touchgfx::BoxWithBorder* boxes[10] = {
		&bx_speed_1, &bx_rpm_1, &bx_gear_1, /* skip 3 */ &vx_oilt_1,
		&bx_oilp_1, &bx_batt_1, &bx_time_1, &bx_DRS_1, &bx_alag_1, &bx_lch_1
	};
	static const uint8_t bwb_idx[10] = { 0, 1, 2, 4, 5, 6, 7, 8, 9, 10 };

	for(int i = 0; i < 10; i++){
		const ColorEntry& c = COLOR_PALETTE[colors[bwb_idx[i]]];
		boxes[i]->setColor(touchgfx::Color::getColorFromRGB(c.r, c.g, c.b));
		boxes[i]->setBorderColor(touchgfx::Color::getColorFromRGB(c.br, c.bg, c.bb));
		boxes[i]->invalidate();
	}

	const ColorEntry& fc = COLOR_PALETTE[colors[3]];
	dbx_fuel_1.setColor(touchgfx::Color::getColorFromRGB(fc.r, fc.g, fc.b));
	dbx_fuel_1.invalidate();
}

void Screen1View::apply_single_box_color_customizer(uint8_t box_index, uint8_t color_index){
	const ColorEntry& c = COLOR_PALETTE[color_index];

	// Index 3 is dbx_fuel_1 (plain Box, fill only)
	if(box_index == 3){
		dbx_fuel_1.setColor(touchgfx::Color::getColorFromRGB(c.r, c.g, c.b));
		dbx_fuel_1.invalidate();
		return;
	}

	// Map box_index to the BoxWithBorder array (skipping index 3)
	touchgfx::BoxWithBorder* boxes[10] = {
		&bx_speed_1, &bx_rpm_1, &bx_gear_1, /* skip 3 */ &vx_oilt_1,
		&bx_oilp_1, &bx_batt_1, &bx_time_1, &bx_DRS_1, &bx_alag_1, &bx_lch_1
	};
	uint8_t arr_idx = (box_index < 3) ? box_index : box_index - 1;

	boxes[arr_idx]->setColor(touchgfx::Color::getColorFromRGB(c.r, c.g, c.b));
	boxes[arr_idx]->setBorderColor(touchgfx::Color::getColorFromRGB(c.br, c.bg, c.bb));
	boxes[arr_idx]->invalidate();
}

void Screen1View::update_widget_selector(){
	dbx_widget_selector.invalidate();
	uint8_t box = BOX_SCROLL_ORDER[customizer_scroll_pos];
	const BoxGeometry& g = CUSTOMIZER_BOX_POS[box];
	dbx_widget_selector.setPosition(g.x, g.y, g.w, g.h);
	dbx_widget_selector.invalidate();
}

void Screen1View::open_customizer_for_profile(uint8_t profile_index){
	editing_profile_index = profile_index;
	customizer_scroll_pos = 0;
	color_edit_mode = false;

	// Load existing colors or defaults
	if(presenter->isProfileFilled(profile_index)){
		CR_profile_t profile;
		presenter->getProfile(profile_index, &profile);
		for(int i = 0; i < NUM_CUSTOM_BOXES; i++)
			pending_colors[i] = profile.box_colors[i];
	} else {
		// Default: all black (index 0)
		for(int i = 0; i < NUM_CUSTOM_BOXES; i++)
			pending_colors[i] = 0;
	}

	// Apply colors to customizer preview
	apply_colors_to_customizer(pending_colors);

	// Position widget selector at first box with white border
	update_widget_selector();
	dbx_widget_selector.setBorderColor(touchgfx::Color::getColorFromRGB(255, 255, 255));

	// Close driver profiles, open customizer
	driver_profiles_menu_open = false;
	driver_profiles_container.setVisible(false);
	driver_profiles_container.invalidate();

	customize_profile_open = true;
	display_customizer_container.setVisible(true);
	display_customizer_container.invalidate();
}

void Screen1View::save_and_apply_customizer(){
	// Build profile struct
	CR_profile_t profile;
	profile.magic = CR_PROFILE_MAGIC;
	for(int i = 0; i < NUM_CUSTOM_BOXES; i++)
		profile.box_colors[i] = pending_colors[i];
	profile.reserved[0] = 0;
	profile.reserved[1] = 0;
	profile.reserved[2] = 0;
	profile.reserved[3] = 0;

	// Save to model and persist to flash
	presenter->saveProfile(editing_profile_index, &profile);
	presenter->setActiveProfile(editing_profile_index);
	presenter->saveAll();

	// Apply to main dashboard
	apply_colors_to_dashboard(pending_colors);

	// Close customizer back to dashboard
	customize_profile_open = false;
	color_edit_mode = false;
	display_customizer_container.setVisible(false);
	display_customizer_container.invalidate();
}
