#include <gui/screen1_screen/Screen1View.hpp>


// Hi there!
// Remember to specify which letters you want loaded in TouchGFX.
/// I disabled the Fallback "?" unknown character, so you'll see nothing if you don't. | No you didnt


constexpr int16_t Screen1View::PROFILE_ROW_Y[5];
constexpr uint8_t Screen1View::BOX_SCROLL_ORDER[NUM_CUSTOM_BOXES];

// Fill colors used for dashboard boxes. The border fields (br/bg/bb) are kept for
// reference but are NOT used at runtime — borders are always white, except when the
// fill is white (palette index 1), in which case borders and text are set to black.
const Screen1View::ColorEntry Screen1View::COLOR_PALETTE[CR_NUM_PALETTE_COLORS] = {
    {  10,  10,  10,  255, 255, 255 },  // 0: Black
    { 255, 255, 255,  180, 180, 180 },  // 1: White   (triggers black border + black text)
    { 230,   0,   0,  161,   0,   0 },  // 2: Red
    {   0, 180,   0,    0, 126,   0 },  // 3: Green
    {   0,   0, 200,    0,   0, 140 },  // 4: Blue
    { 200, 200,   0,  140, 140,   0 },  // 5: Yellow
    { 220, 140,   0,  154,  98,   0 },  // 6: Orange
    { 140,   0, 200,   98,   0, 140 },  // 7: Purple
};

// Bounding rectangles for the widget selector around each customizer box.
// Derived from _1 box positions with ~15px margin.
const Screen1View::BoxGeometry Screen1View::CUSTOMIZER_BOX_POS[NUM_CUSTOM_BOXES] = {
    {  28,  59, 314, 106 },  // 0: bx_oilt_1
    {  28, 172, 314, 106 },  // 1: bx_oilp_1
    {  28, 280, 314, 106 },  // 2: bx_batt_1
    {  28, 387, 314, 106 },  // 3: bx_time_1
    { 355,  14, 314,  88 },  // 4: bx_rpm_1
    { 355, 101, 314, 278 },  // 5: bx_gear_1
    { 279, 508, 472,  68 },  // 6: dbx_fuel_1
    { 702,  -6, 266,  79 },  // 7: bx_change_bg
    { 682,  59, 314, 106 },  // 8: bx_speed_1
    { 682, 172, 314, 106 },  // 9: bx_DRS_1
    { 682, 280, 314, 106 },  // 10: bx_alag_1
    { 682, 387, 314, 106 },  // 11: bx_lch_1
    { 740, 503, 266,  79 },  // 12: bx_reset
};

Screen1View::Screen1View() {}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();

    // Read the stored fuel level from the model and paint the initial state.
    uint8_t stored = presenter->getFuelLevel();
    pending_fuel_tenths = stored;
    refresh_fuel_display(stored);

    // Apply active profile colors (boxes + background + text) on boot
    uint8_t activeIdx = presenter->getActiveProfileIndex();
    if(activeIdx < CR_MAX_PROFILES && presenter->isProfileFilled(activeIdx)){
        CR_profile_t profile;
        presenter->getProfile(activeIdx, &profile);
        apply_colors_to_dashboard(profile.box_colors, profile.bg_color);
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
			// STATUS column: select a filled profile and apply its colors + bg
			if(filled){
				CR_profile_t profile;
				presenter->getProfile(row, &profile);
				apply_colors_to_dashboard(profile.box_colors, profile.bg_color);
				presenter->setActiveProfile(row);
				presenter->saveAll();

				driver_profiles_menu_open = false;
				menu_driver_profiles_container.setVisible(false);
				menu_driver_profiles_container.invalidate();
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
			menu_driver_profiles_container.setVisible(true);
			menu_driver_profiles_container.invalidate();
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
		menu_driver_profiles_container.setVisible(true);
		menu_driver_profiles_container.invalidate();
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
		menu_driver_profiles_container.setVisible(false);
		menu_driver_profiles_container.invalidate();
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
			if(box == RESET_BOX_INDEX) { return; } // reset box has no color to cycle
			if(box == BG_BOX_INDEX){
				// BG box: cycle pending_bg_color (separate from pending_colors array)
				pending_bg_color++;
				if(pending_bg_color >= CR_NUM_PALETTE_COLORS)
					pending_bg_color = 0;
				apply_single_box_color_customizer(box, pending_bg_color);
			} else {
				// Normal box: map scroll index to pending_colors index.
				// Indices 0-6 map directly; 8-11 map to 7-10 (skip bg at 7).
				uint8_t ci = (box < 7) ? box : box - 1;
				pending_colors[ci]++;
				if(pending_colors[ci] >= CR_NUM_PALETTE_COLORS)
					pending_colors[ci] = 0;
				apply_single_box_color_customizer(box, pending_colors[ci]);
			}
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
			if(box == RESET_BOX_INDEX) { return; } // reset box has no color to cycle
			if(box == BG_BOX_INDEX){
				// BG box: cycle pending_bg_color backwards
				if(pending_bg_color == 0)
					pending_bg_color = CR_NUM_PALETTE_COLORS - 1;
				else
					pending_bg_color--;
				apply_single_box_color_customizer(box, pending_bg_color);
			} else {
				// Normal box: map scroll index to pending_colors index (skip bg at 7)
				uint8_t ci = (box < 7) ? box : box - 1;
				if(pending_colors[ci] == 0)
					pending_colors[ci] = CR_NUM_PALETTE_COLORS - 1;
				else
					pending_colors[ci]--;
				apply_single_box_color_customizer(box, pending_colors[ci]);
			}
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

// Encoder click handler - only active in the display customizer.
// Normal boxes: toggles color_edit_mode (red selector = editing, white = browsing).
// Reset box: immediately resets all colors to defaults instead of entering edit mode.
void Screen1View::encoder_click(){
	if(customize_profile_open){
		uint8_t box = BOX_SCROLL_ORDER[customizer_scroll_pos];

		// Reset box: trigger full color reset instead of entering color edit mode
		if(box == RESET_BOX_INDEX){
			reset_customizer_to_defaults();
			return;
		}

		// Toggle color edit mode for the currently selected box
		color_edit_mode = !color_edit_mode;

		// Visual feedback: red selector border = editing, white = browsing
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

		// A profile shows "Default" if it was never saved (empty slot) OR if it
		// was saved but all box_colors and bg_color are still 0 (all-black defaults).
		bool is_default_colors = false;
		if(filled){
			CR_profile_t p;
			presenter->getProfile(i, &p);
			is_default_colors = true;
			for(int c = 0; c < CR_NUM_BOX_COLORS; c++){
				if(p.box_colors[c] != 0){ is_default_colors = false; break; }
			}
			if(p.bg_color != 0) is_default_colors = false;
		}

		// Priority: "In Use" > "Filled" > "Default"
		if(filled && i == activeIdx){
			Unicode::strncpy(status_bufs[i], "In Use", 10);
		} else if(filled && !is_default_colors){
			Unicode::strncpy(status_bufs[i], "Filled", 10);
		} else {
			Unicode::strncpy(status_bufs[i], "Default", 10);
		}
		Unicode::strncpy(edit_bufs[i], filled ? "Edit" : "Add", 10);
		status_txts[i]->invalidate();
		edit_txts[i]->invalidate();
	}
}

// Apply all profile colors to the main dashboard view.
// Sets fill color from palette, border to white (or black if fill is white),
// and text to white (or black if fill is white) for each box's labels and values.
void Screen1View::apply_colors_to_dashboard(const uint8_t colors[CR_NUM_BOX_COLORS], uint8_t bg_color){
	// 10 BoxWithBorder widgets (all dashboard boxes except fuel bar)
	touchgfx::BoxWithBorder* boxes[10] = {
		&bx_oilt, &bx_oilp, &bx_batt, &bx_time,
		&bx_rpm, &bx_gear, /* skip 6 */ &bx_speed,
		&bx_DRS, &bx_alag, &bx_lch
	};
	// Maps the 10-element boxes[] array to indices in the 11-element colors[] array.
	// Skips index 6 (fuel bar) since it's a plain Box, not BoxWithBorder.
	static const uint8_t bwb_idx[10] = { 0, 1, 2, 3, 4, 5, 7, 8, 9, 10 };

	touchgfx::colortype white = touchgfx::Color::getColorFromRGB(255, 255, 255);
	touchgfx::colortype black = touchgfx::Color::getColorFromRGB(0, 0, 0);

	// Apply fill + border to each BoxWithBorder
	for(int i = 0; i < 10; i++){
		const ColorEntry& c = COLOR_PALETTE[colors[bwb_idx[i]]];
		// Border rule: white unless fill is white (palette index 1), then black
		touchgfx::colortype border = (colors[bwb_idx[i]] == 1) ? black : white;
		boxes[i]->setColor(touchgfx::Color::getColorFromRGB(c.r, c.g, c.b));
		boxes[i]->setBorderColor(border);
		boxes[i]->invalidate();
	}

	// colors[6]: dbx_fuel is a plain Box (fill only, no border or text)
	const ColorEntry& fc = COLOR_PALETTE[colors[6]];
	dbx_fuel.setColor(touchgfx::Color::getColorFromRGB(fc.r, fc.g, fc.b));
	dbx_fuel.invalidate();

	// Dashboard background (plain Box, fill only)
	const ColorEntry& bgc = COLOR_PALETTE[bg_color];
	bx_dashbackground.setColor(touchgfx::Color::getColorFromRGB(bgc.r, bgc.g, bgc.b));
	bx_dashbackground.invalidate();

	// Text colors per box: black text on white fill for readability, white otherwise.
	// Each box has 1-3 text widgets: label (txt_), unit (txt_), and dynamic value (dtxt_).
	touchgfx::colortype tc;

	tc = (colors[0] == 1) ? black : white; // oilt
	txt_oilt.setColor(tc); txt_f_oilt.setColor(tc); dtxt_oilt.setColor(tc);
	txt_oilt.invalidate(); txt_f_oilt.invalidate(); dtxt_oilt.invalidate();

	tc = (colors[1] == 1) ? black : white; // oilp
	txt_oilp.setColor(tc); txt_psi_oilp.setColor(tc); dtxt_oilp.setColor(tc);
	txt_oilp.invalidate(); txt_psi_oilp.invalidate(); dtxt_oilp.invalidate();

	tc = (colors[2] == 1) ? black : white; // batt
	txt_batt.setColor(tc); txt_v_batt.setColor(tc); dtxt_batt.setColor(tc);
	txt_batt.invalidate(); txt_v_batt.invalidate(); dtxt_batt.invalidate();

	tc = (colors[3] == 1) ? black : white; // time
	txt_time.setColor(tc); txt_min_time.setColor(tc); dtxt_time.setColor(tc);
	txt_time.invalidate(); txt_min_time.invalidate(); dtxt_time.invalidate();

	tc = (colors[4] == 1) ? black : white; // rpm (value only, no label)
	dtxt_rpm.setColor(tc); dtxt_rpm.invalidate();

	tc = (colors[5] == 1) ? black : white; // gear
	txt_gear.setColor(tc); dtxt_gear.setColor(tc);
	txt_gear.invalidate(); dtxt_gear.invalidate();

	// colors[6] = fuel bar — no text widgets

	tc = (colors[7] == 1) ? black : white; // speed
	txt_speed.setColor(tc); txt_mph_speed.setColor(tc); dtxt_speed.setColor(tc);
	txt_speed.invalidate(); txt_mph_speed.invalidate(); dtxt_speed.invalidate();

	tc = (colors[8] == 1) ? black : white; // DRS
	txt_drs.setColor(tc); txt_sw_drs.setColor(tc); dtxt_drs.setColor(tc);
	txt_drs.invalidate(); txt_sw_drs.invalidate(); dtxt_drs.invalidate();

	tc = (colors[9] == 1) ? black : white; // alag
	txt_alag.setColor(tc); txt_sw_alag.setColor(tc); dtxt_alag.setColor(tc);
	txt_alag.invalidate(); txt_sw_alag.invalidate(); dtxt_alag.invalidate();

	tc = (colors[10] == 1) ? black : white; // lch
	txt_lch.setColor(tc); txt_sw_lch.setColor(tc); dtxt_lch.setColor(tc);
	txt_lch.invalidate(); txt_sw_lch.invalidate(); dtxt_lch.invalidate();
}

// Apply all profile colors to the customizer preview (the _1 widgets).
// Same border/text rules as the dashboard version.
void Screen1View::apply_colors_to_customizer(const uint8_t colors[CR_NUM_BOX_COLORS], uint8_t bg_color){
	// 10 BoxWithBorder widgets in the customizer preview (same order as dashboard)
	touchgfx::BoxWithBorder* boxes[10] = {
		&bx_speed_1, &bx_oilp_1, &bx_batt_1, &bx_time_1,
		&bx_rpm_1, &bx_gear_1, /* skip 6 */ &vx_oilt_1,
		&bx_DRS_1, &bx_alag_1, &bx_lch_1
	};
	// Same mapping as dashboard: skip colors[6] (fuel bar)
	static const uint8_t bwb_idx[10] = { 0, 1, 2, 3, 4, 5, 7, 8, 9, 10 };

	touchgfx::colortype white = touchgfx::Color::getColorFromRGB(255, 255, 255);
	touchgfx::colortype black = touchgfx::Color::getColorFromRGB(0, 0, 0);

	// Apply fill + border to each BoxWithBorder
	for(int i = 0; i < 10; i++){
		const ColorEntry& c = COLOR_PALETTE[colors[bwb_idx[i]]];
		touchgfx::colortype border = (colors[bwb_idx[i]] == 1) ? black : white;
		boxes[i]->setColor(touchgfx::Color::getColorFromRGB(c.r, c.g, c.b));
		boxes[i]->setBorderColor(border);
		boxes[i]->invalidate();
	}

	// colors[6]: fuel bar preview (plain Box, fill only)
	const ColorEntry& fc = COLOR_PALETTE[colors[6]];
	dbx_fuel_1.setColor(touchgfx::Color::getColorFromRGB(fc.r, fc.g, fc.b));
	dbx_fuel_1.invalidate();

	// Background color applied to the customizer preview background
	const ColorEntry& bgc = COLOR_PALETTE[bg_color];
	bx_dashbackground_1.setColor(touchgfx::Color::getColorFromRGB(bgc.r, bgc.g, bgc.b));
	bx_dashbackground_1.invalidate();

	// bx_change_bg box mirrors the background color so user sees what they're picking
	touchgfx::colortype bg_border = (bg_color == 1) ? black : white;
	bx_change_bg.setColor(touchgfx::Color::getColorFromRGB(bgc.r, bgc.g, bgc.b));
	bx_change_bg.setBorderColor(bg_border);
	bx_change_bg.invalidate();

	// Text on the change_bg box and the customizer title follow bg color rule
	txt_change_background.setColor((bg_color == 1) ? black : white);
	txt_change_background.invalidate();
	txt_title.setColor((bg_color == 1) ? black : white);
	txt_title.invalidate();

	// Text colors per box: black on white fill, white otherwise.
	// Text widget names match the sensor, NOT the box widget name (see note above).
	touchgfx::colortype tc;

	tc = (colors[0] == 1) ? black : white; // oilt (sits on bx_speed_1)
	txt_oilt_1.setColor(tc); txt_f_oilt_1.setColor(tc); dtxt_oilt_1.setColor(tc);
	txt_oilt_1.invalidate(); txt_f_oilt_1.invalidate(); dtxt_oilt_1.invalidate();

	tc = (colors[1] == 1) ? black : white; // oilp
	txt_oilp_1.setColor(tc); txt_psi_oilp_1.setColor(tc); dtxt_oilp_1.setColor(tc);
	txt_oilp_1.invalidate(); txt_psi_oilp_1.invalidate(); dtxt_oilp_1.invalidate();

	tc = (colors[2] == 1) ? black : white; // batt
	txt_batt_1.setColor(tc); txt_v_batt_1.setColor(tc); dtxt_batt_1.setColor(tc);
	txt_batt_1.invalidate(); txt_v_batt_1.invalidate(); dtxt_batt_1.invalidate();

	tc = (colors[3] == 1) ? black : white; // time
	txt_time_1.setColor(tc); txt_min_time_1.setColor(tc); dtxt_time_1.setColor(tc);
	txt_time_1.invalidate(); txt_min_time_1.invalidate(); dtxt_time_1.invalidate();

	tc = (colors[4] == 1) ? black : white; // rpm (value only)
	dtxt_rpm_1.setColor(tc); dtxt_rpm_1.invalidate();

	tc = (colors[5] == 1) ? black : white; // gear
	txt_gear_1.setColor(tc); dtxt_gear_1.setColor(tc);
	txt_gear_1.invalidate(); dtxt_gear_1.invalidate();

	// colors[6] = fuel bar — no text widgets

	tc = (colors[7] == 1) ? black : white; // speed
	txt_speed_1.setColor(tc); txt_mph_speed_1.setColor(tc); dtxt_speed_1.setColor(tc);
	txt_speed_1.invalidate(); txt_mph_speed_1.invalidate(); dtxt_speed_1.invalidate();

	tc = (colors[8] == 1) ? black : white; // DRS
	txt_drs_1.setColor(tc); txt_sw_drs_1.setColor(tc); dtxt_drs_1.setColor(tc);
	txt_drs_1.invalidate(); txt_sw_drs_1.invalidate(); dtxt_drs_1.invalidate();

	tc = (colors[9] == 1) ? black : white; // alag
	txt_alag_1.setColor(tc); txt_sw_alag_1.setColor(tc); dtxt_alag_1.setColor(tc);
	txt_alag_1.invalidate(); txt_sw_alag_1.invalidate(); dtxt_alag_1.invalidate();

	tc = (colors[10] == 1) ? black : white; // lch
	txt_lch_1.setColor(tc); txt_sw_lch_1.setColor(tc); dtxt_lch_1.setColor(tc);
	txt_lch_1.invalidate(); txt_sw_lch_1.invalidate(); dtxt_lch_1.invalidate();
}

// Update a single customizer box during live color cycling (called by cursor_up/down).
// box_index is the scroll-order index (0-12), color_index is the palette index (0-7).
// Applies fill, border (white/black rule), and text color for the targeted box only.
void Screen1View::apply_single_box_color_customizer(uint8_t box_index, uint8_t color_index){
	const ColorEntry& c = COLOR_PALETTE[color_index];
	touchgfx::colortype white = touchgfx::Color::getColorFromRGB(255, 255, 255);
	touchgfx::colortype black = touchgfx::Color::getColorFromRGB(0, 0, 0);
	touchgfx::colortype border = (color_index == 1) ? black : white; // border rule
	touchgfx::colortype tc = border; // text color follows same white/black rule

	// Reset box (index 12) is click-only — no color to apply
	if(box_index == RESET_BOX_INDEX) return;

	// BG box (index 7): updates the preview background, change_bg box, and title text
	if(box_index == BG_BOX_INDEX){
		bx_dashbackground_1.setColor(touchgfx::Color::getColorFromRGB(c.r, c.g, c.b));
		bx_dashbackground_1.invalidate();
		bx_change_bg.setColor(touchgfx::Color::getColorFromRGB(c.r, c.g, c.b));
		bx_change_bg.setBorderColor(border);
		bx_change_bg.invalidate();
		txt_change_background.setColor(tc);
		txt_change_background.invalidate();
		txt_title.setColor(tc); // title sits on the background
		txt_title.invalidate();
		return;
	}

	// Fuel bar (index 6): plain Box, fill only, no border or text
	if(box_index == 6){
		dbx_fuel_1.setColor(touchgfx::Color::getColorFromRGB(c.r, c.g, c.b));
		dbx_fuel_1.invalidate();
		return;
	}

	// Standard BoxWithBorder: map scroll index to the 10-element boxes array.
	// Indices 0-5 map directly, 8-11 map to arr 6-9 (skipping fuel=6 and bg=7).
	touchgfx::BoxWithBorder* boxes[10] = {
		&bx_speed_1, &bx_oilp_1, &bx_batt_1, &bx_time_1,
		&bx_rpm_1, &bx_gear_1, /* skip 6,7 */ &vx_oilt_1,
		&bx_DRS_1, &bx_alag_1, &bx_lch_1
	};
	uint8_t arr_idx;
	if(box_index < 6)
		arr_idx = box_index;
	else // box_index 8-11 → arr_idx 6-9
		arr_idx = box_index - 2;

	boxes[arr_idx]->setColor(touchgfx::Color::getColorFromRGB(c.r, c.g, c.b));
	boxes[arr_idx]->setBorderColor(border);
	boxes[arr_idx]->invalidate();

	// Update text colors for the box's label, unit, and value widgets.
	// Text widget names match the sensor name (not the box widget name).
	switch(box_index){
		case 0: // oilt texts (positioned on bx_speed_1)
			txt_oilt_1.setColor(tc); txt_f_oilt_1.setColor(tc); dtxt_oilt_1.setColor(tc);
			txt_oilt_1.invalidate(); txt_f_oilt_1.invalidate(); dtxt_oilt_1.invalidate();
			break;
		case 1: // oilp
			txt_oilp_1.setColor(tc); txt_psi_oilp_1.setColor(tc); dtxt_oilp_1.setColor(tc);
			txt_oilp_1.invalidate(); txt_psi_oilp_1.invalidate(); dtxt_oilp_1.invalidate();
			break;
		case 2: // batt
			txt_batt_1.setColor(tc); txt_v_batt_1.setColor(tc); dtxt_batt_1.setColor(tc);
			txt_batt_1.invalidate(); txt_v_batt_1.invalidate(); dtxt_batt_1.invalidate();
			break;
		case 3: // time
			txt_time_1.setColor(tc); txt_min_time_1.setColor(tc); dtxt_time_1.setColor(tc);
			txt_time_1.invalidate(); txt_min_time_1.invalidate(); dtxt_time_1.invalidate();
			break;
		case 4: // rpm (value only)
			dtxt_rpm_1.setColor(tc); dtxt_rpm_1.invalidate();
			break;
		case 5: // gear
			txt_gear_1.setColor(tc); dtxt_gear_1.setColor(tc);
			txt_gear_1.invalidate(); dtxt_gear_1.invalidate();
			break;
		case 8: // speed texts (positioned on vx_oilt_1)
			txt_speed_1.setColor(tc); txt_mph_speed_1.setColor(tc); dtxt_speed_1.setColor(tc);
			txt_speed_1.invalidate(); txt_mph_speed_1.invalidate(); dtxt_speed_1.invalidate();
			break;
		case 9: // DRS
			txt_drs_1.setColor(tc); txt_sw_drs_1.setColor(tc); dtxt_drs_1.setColor(tc);
			txt_drs_1.invalidate(); txt_sw_drs_1.invalidate(); dtxt_drs_1.invalidate();
			break;
		case 10: // alag
			txt_alag_1.setColor(tc); txt_sw_alag_1.setColor(tc); dtxt_alag_1.setColor(tc);
			txt_alag_1.invalidate(); txt_sw_alag_1.invalidate(); dtxt_alag_1.invalidate();
			break;
		case 11: // lch
			txt_lch_1.setColor(tc); txt_sw_lch_1.setColor(tc); dtxt_lch_1.setColor(tc);
			txt_lch_1.invalidate(); txt_sw_lch_1.invalidate(); dtxt_lch_1.invalidate();
			break;
	}
}

void Screen1View::update_widget_selector(){
	dbx_widget_selector.invalidate();
	uint8_t box = BOX_SCROLL_ORDER[customizer_scroll_pos];
	const BoxGeometry& g = CUSTOMIZER_BOX_POS[box];
	dbx_widget_selector.setPosition(g.x, g.y, g.w, g.h);
	dbx_widget_selector.invalidate();
}

// Open the display customizer for a given profile slot.
// Loads existing colors (or defaults if empty) into pending state and shows the preview.
void Screen1View::open_customizer_for_profile(uint8_t profile_index){
	editing_profile_index = profile_index;
	customizer_scroll_pos = 0;
	color_edit_mode = false;

	// Load existing box colors + background color from profile, or default to all black
	if(presenter->isProfileFilled(profile_index)){
		CR_profile_t profile;
		presenter->getProfile(profile_index, &profile);
		for(int i = 0; i < CR_NUM_BOX_COLORS; i++)
			pending_colors[i] = profile.box_colors[i];
		pending_bg_color = profile.bg_color;
	} else {
		for(int i = 0; i < CR_NUM_BOX_COLORS; i++)
			pending_colors[i] = 0;
		pending_bg_color = 0;
	}

	// Render the customizer preview with loaded colors
	apply_colors_to_customizer(pending_colors, pending_bg_color);

	// Position widget selector at first box with white border
	update_widget_selector();
	dbx_widget_selector.setBorderColor(touchgfx::Color::getColorFromRGB(255, 255, 255));

	// Close driver profiles, open customizer
	driver_profiles_menu_open = false;
	menu_driver_profiles_container.setVisible(false);
	menu_driver_profiles_container.invalidate();

	customize_profile_open = true;
	display_customizer_container.setVisible(true);
	display_customizer_container.invalidate();
}

// Save pending colors + bg to flash, apply to dashboard, and close the customizer.
// Triggered by pressing the menu button while the customizer is open.
void Screen1View::save_and_apply_customizer(){
	// Build profile struct from pending state (11 box colors + 1 bg color)
	CR_profile_t profile;
	profile.magic = CR_PROFILE_MAGIC;
	for(int i = 0; i < CR_NUM_BOX_COLORS; i++)
		profile.box_colors[i] = pending_colors[i];
	profile.bg_color = pending_bg_color;
	profile.reserved[0] = 0;
	profile.reserved[1] = 0;
	profile.reserved[2] = 0;

	// Persist to flash and mark as active profile
	presenter->saveProfile(editing_profile_index, &profile);
	presenter->setActiveProfile(editing_profile_index);
	presenter->saveAll();

	// Apply saved colors to the main dashboard (boxes, borders, text, background)
	apply_colors_to_dashboard(pending_colors, pending_bg_color);

	// Close customizer and return to dashboard view
	customize_profile_open = false;
	color_edit_mode = false;
	display_customizer_container.setVisible(false);
	display_customizer_container.invalidate();
}

// Reset all customizer colors to defaults (black fill, palette index 0) without saving.
// Triggered by clicking the encoder on the reset box (scroll position 12).
// The user must still press menu to save, or back to discard.
void Screen1View::reset_customizer_to_defaults(){
	// Zero out all 11 box colors and background color
	for(int i = 0; i < CR_NUM_BOX_COLORS; i++)
		pending_colors[i] = 0;
	pending_bg_color = 0;

	// Refresh the customizer preview with default colors
	apply_colors_to_customizer(pending_colors, pending_bg_color);

	// Ensure we're in browse mode (not color edit mode) after reset
	color_edit_mode = false;
	dbx_widget_selector.setBorderColor(touchgfx::Color::getColorFromRGB(255, 255, 255));
	dbx_widget_selector.invalidate();
}
