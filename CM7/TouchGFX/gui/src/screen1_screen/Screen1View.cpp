#include <gui/screen1_screen/Screen1View.hpp>


// Hi there!
// Remember to specify which letters you want loaded in TouchGFX.
/// I disabled the Fallback "?" unknown character, so you'll see nothing if you don't.


Screen1View::Screen1View()
{

}

void Screen1View::setupScreen()
{
    Screen1ViewBase::setupScreen();
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
	if(!menu_scontainer.isVisible()){
		menu_scontainer.setVisible(true);
		dbx_menu_selection.setY(89);
		menu_scontainer.invalidate();
	}
}

void Screen1View::close_menu(){
	if(menu_scontainer.isVisible()){
		menu_scontainer.setVisible(false);
		menu_scontainer.invalidate();
	}
}

void Screen1View::cursor_up(){
	int16_t y_val = dbx_menu_selection.getY();
	if (y_val > 89){
		dbx_menu_selection.setY(y_val-50);
	}
	menu_scontainer.invalidate();
}

void Screen1View::cursor_down(){
	int16_t y_val = dbx_menu_selection.getY();
	if (y_val < 295){
		dbx_menu_selection.setY(y_val+50);
	}
	menu_scontainer.invalidate();
}
