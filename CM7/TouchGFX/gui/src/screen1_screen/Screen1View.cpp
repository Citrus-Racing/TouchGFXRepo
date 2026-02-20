#include <gui/screen1_screen/Screen1View.hpp>


// Hi there!
// Remember to specify which letters you want loaded in TouchGFX.
/// I disabled the Fallback "?" unknown character, so you'll see nothing if you don't.


Screen1View::Screen1View()
    : fuel_menu_open(false), pending_fuel_tenths(55)
{

}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();

    // Read the stored fuel level from the model and paint the initial state.
    // TouchGFX View<T> provides a presenter pointer of the concrete presenter
    // type, so we can call methods on it directly.
    uint8_t stored = presenter->getFuelLevel();
    pending_fuel_tenths = stored;
    refresh_fuel_display(stored);
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
	// open_menu is triggered by the menu button.
	// If the fuel sub-menu is open, the menu button saves the pending fuel
	// level and closes the fuel sub-menu.
	if(fuel_menu_open){
		presenter->saveFuelLevel(pending_fuel_tenths);
		fuel_menu_open = false;
		set_fuel_container.setVisible(false);
		set_fuel_container.invalidate();
		return;
	}

	// If the main menu is open and "Set Fuel" is highlighted, open the fuel
	// sub-menu.
	if(menu_scontainer.isVisible()){
		int16_t y_val = dbx_menu_selection.getY();
		if(y_val == 89){
			// "Set Fuel" selected - close main menu and open the fuel sub-menu
			pending_fuel_tenths = presenter->getFuelLevel();
			refresh_fuel_display(pending_fuel_tenths);
			menu_scontainer.setVisible(false);
			menu_scontainer.invalidate();
			set_fuel_container.setVisible(true);
			set_fuel_container.invalidate();
			fuel_menu_open = true;
		}
		// Other menu items can be handled here in the future
		return;
	}

	// Otherwise open the main menu
	menu_scontainer.setVisible(true);
	dbx_menu_selection.setY(89);
	menu_scontainer.invalidate();
}

void Screen1View::close_menu(){
	// close_menu is triggered by the back button.
	// It always just closes whatever is currently open without saving.
	if(fuel_menu_open){
		fuel_menu_open = false;
		set_fuel_container.setVisible(false);
		set_fuel_container.invalidate();

		// Discard pending changes - restore display to last saved value
		pending_fuel_tenths = presenter->getFuelLevel();
		refresh_fuel_display(pending_fuel_tenths);
		return;
	}

	if(menu_scontainer.isVisible()){
		menu_scontainer.setVisible(false);
		menu_scontainer.invalidate();
	}
}

void Screen1View::cursor_up(){
	// When the fuel sub-menu is open, encoder up increments by 1 tenth (0.1L)
	if(fuel_menu_open){
		if(pending_fuel_tenths < FUEL_MAX_TENTHS){
			pending_fuel_tenths++;
			refresh_fuel_display(pending_fuel_tenths);
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
	// When the fuel sub-menu is open, encoder down decrements by 1 tenth (0.1L)
	if(fuel_menu_open){
		if(pending_fuel_tenths > 0){
			pending_fuel_tenths--;
			refresh_fuel_display(pending_fuel_tenths);
		}
		return;
	}

	int16_t y_val = dbx_menu_selection.getY();
	if (y_val < 245){
		dbx_menu_selection.setY(y_val+50);
	}
	menu_scontainer.invalidate();
}

void Screen1View::encoder_click(){
	// I was using this for a bit but clicking it is stupid when we have buttons
}

void Screen1View::refresh_fuel_display(uint8_t tenths){
	// Display as "X.X"
	// Write the integer part then decimal point then fractional part
	uint8_t whole    = tenths / 10;
	uint8_t fraction = tenths % 10;
	Unicode::snprintf(dtxt_set_levelBuffer, DTXT_SET_LEVEL_SIZE, "%d.%d", whole, fraction);
	dtxt_set_level.invalidate();

	// Update the fuel bar. Invalidate the full original width first so that
	// shrinking the bar clears the pixels that are no longer covered.
	// Bar is anchored left at x=294, max width 442px (at 5.5L).
	dbx_fuel.setWidth(FUEL_BAR_MAX_WIDTH);
	dbx_fuel.invalidate();
	uint16_t bar_width = (uint16_t)(((uint32_t)tenths * FUEL_BAR_MAX_WIDTH) / FUEL_MAX_TENTHS);
	dbx_fuel.setWidth(bar_width);
	dbx_fuel.invalidate();
}
