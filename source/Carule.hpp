/*******************************************************************************
	FILE:				carule.hpp
	PROJECT:			CAMCOS CAPOW!
	ENVIRONMENT:		MS Visual C++ 5.0/MS Windows 95/NT


	FILE DESCRIPTION:	This file contains defintions and data to control
						carule creation.

	UPDATE LOG:			

*******************************************************************************/
//====================INCLUDES===============


#ifndef CARULE_HPP
#define CARULE_HPP

//#include "random.cpp"
//#include "tweak.cpp"
//#include "userca.cpp"
//-----------1D Wave variable names-------------------
#define PAST_C_I0 owner->wave_past_row[c].variable[0]
#define L_I0 owner->wave_source_row[l].variable[0]
#define C_I0 owner->wave_source_row[c].variable[0]
#define R_I0 owner->wave_source_row[r].variable[0]
#define NEW_C_I0	owner->wave_target_row[c].variable[0]

#define PAST_C_I1 owner->wave_past_row[c].variable[1]
#define L_I1 owner->wave_source_row[l].variable[1]
#define C_I1 owner->wave_source_row[c].variable[1]
#define R_I1 owner->wave_source_row[r].variable[1]
#define NEW_C_I1	owner->wave_target_row[c].variable[1]

#define PAST_C_I2 owner->wave_past_row[c].variable[2]
#define L_I2 owner->wave_source_row[l].variable[2]
#define C_I2 owner->wave_source_row[c].variable[2]
#define R_I2 owner->wave_source_row[r].variable[2]
#define NEW_C_I2	owner->wave_target_row[c].variable[2]

#define PAST_C_I3 owner->wave_past_row[c].variable[3]
#define L_I3 owner->wave_source_row[l].variable[3]
#define C_I3 owner->wave_source_row[c].variable[3]
#define R_I3 owner->wave_source_row[r].variable[3]
#define NEW_C_I3	owner->wave_target_row[c].variable[3]

#define PAST_C_V owner->wave_past_row[c].velocity
#define L_V owner->wave_source_row[l].velocity
#define C_V owner->wave_source_row[c].velocity
#define R_V owner->wave_source_row[r].velocity
#define NEW_C_V owner->wave_target_row[c].velocity

//-----------Parameter names-------------------------------------
#define HEAT_INC owner->_heat_inc.Val()
#define MAX_INTENSITY owner->_max_intensity.Val()
#define MAX_VELOCITY owner->_max_velocity.Val()
#define DT owner->_dt.Val()
#define DX owner->_dx.Val()
#define LAMBDA owner->_wavespeed_2_times_dt_2_over_dx_2
#define VEL_LAMBDA owner->_dt_over_dx_2 //For wave rules this is 1/(2*dt), quite large
#define HEAT_LAMBDA VEL_LAMBDA
#define ACCEL_LAMBDA owner->_dt_2_over_mass
//--------------OSCILLATOR parameter names-------------
#define MASS owner->_mass.Val()
#define FRICTION owner->_friction_multiplier.Val()
#define SPRING owner->_spring_multiplier.Val()
#define AMPLITUDE owner->_driver_multiplier.Val()
#define FREQUENCY owner->frequency_factor
#define PHASE owner->_phase
#define TIME owner->time
//-------------NONLINEARITY parameter names------------
#define NONLINEARITY owner->_nonlinearity1.Val() 
//---------------Diverse Cell Parameters--------------------------
/* Here are a couple of individual cell parameters that are different in each
cell, but which are always roughly around 1.0.  You can change them with the
variance button in the Digital Dialog box.  You have to have set your
owner->usercastyle to CA_DIVERSE_OSCILLATOR_WAVE to be able to use these,
also these are only implemented in 1D rules */
#define CELL_PARAM_0 owner->wave_source_row[c]._cell_param[0]
#define CELL_PARAM_1 owner->wave_source_row[c]._cell_param[1]

//----------------2D variable names Intenstity -------------
#define PLANE_C_I0 owner->wave_source_plane[c].variable[0]
#define PLANE_E_I0 owner->wave_source_plane[e].variable[0]
#define PLANE_NE_I0 owner->wave_source_plane[ne].variable[0]
#define PLANE_N_I0 owner->wave_source_plane[n].variable[0]
#define PLANE_NW_I0 owner->wave_source_plane[nw].variable[0]
#define PLANE_W_I0 owner->wave_source_plane[w].variable[0]
#define PLANE_SW_I0 owner->wave_source_plane[sw].variable[0]
#define PLANE_S_I0 owner->wave_source_plane[s].variable[0]
#define PLANE_SE_I0 owner->wave_source_plane[se].variable[0]
	//---new---
#define PLANE_NEW_C_I0 owner->wave_target_plane[c].variable[0]
#define PLANE_NEW_E_I0 owner->wave_target_plane[e].variable[0]
#define PLANE_NEW_NE_I0 owner->wave_target_plane[ne].variable[0]
#define PLANE_NEW_N_I0 owner->wave_target_plane[n].variable[0]
#define PLANE_NEW_NW_I0 owner->wave_target_plane[nw].variable[0]
#define PLANE_NEW_W_I0 owner->wave_target_plane[w].variable[0]
#define PLANE_NEW_SW_I0 owner->wave_target_plane[sw].variable[0]
#define PLANE_NEW_S_I0 owner->wave_target_plane[s].variable[0]
#define PLANE_NEW_SE_I0 owner->wave_target_plane[se].variable[0]
	//---past---
#define PLANE_PAST_C_I0 owner->wave_past_plane[c].variable[0]
#define PLANE_PAST_E_I0 owner->wave_past_plane[e].variable[0]
#define PLANE_PAST_NE_I0 owner->wave_past_plane[ne].variable[0]
#define PLANE_PAST_N_I0 owner->wave_past_plane[n].variable[0]
#define PLANE_PAST_NW_I0 owner->wave_past_plane[nw].variable[0]
#define PLANE_PAST_W_I0 owner->wave_past_plane[w].variable[0]
#define PLANE_PAST_SW_I0 owner->wave_past_plane[sw].variable[0]
#define PLANE_PAST_S_I0 owner->wave_past_plane[s].variable[0]
#define PLANE_PAST_SE_I0 owner->wave_past_plane[se].variable[0]
	//combo
/*Initially I left off the parenthesis around the definition of PLANE_FOUR_SUM,
which had the effect that 0.25*PLANE_FOUR_SUM was not the right thing. Macros are
dangerous. --RR, 2/17/97.*/
#define PLANE_FOUR_SUM_I0 (PLANE_E_I0+PLANE_N_I0+PLANE_W_I0+PLANE_S_I0)
#define PLANE_FIVE_SUM_I0 (PLANE_E_I0+PLANE_N_I0+PLANE_W_I0+PLANE_S_I0+PLANE_C_I0)
#define PLANE_CORNER_SUM_I0 (PLANE_NE_I0+PLANE_NW_I0+PLANE_SW_I0+PLANE_SE_I0)
#define PLANE_EIGHT_SUM_I0 (PLANE_FOUR_SUM_I0+PLANE_CORNER_SUM_I0)
#define PLANE_NINE_SUM_I0 (PLANE_EIGHT_SUM_I0+PLANE_C_I0)
//----------------2D variable names Intensity 1---------------------------
#define PLANE_C_I1 owner->wave_source_plane[c].variable[1]
#define PLANE_E_I1 owner->wave_source_plane[e].variable[1]
#define PLANE_NE_I1 owner->wave_source_plane[ne].variable[1]
#define PLANE_N_I1 owner->wave_source_plane[n].variable[1]
#define PLANE_NW_I1 owner->wave_source_plane[nw].variable[1]
#define PLANE_W_I1 owner->wave_source_plane[w].variable[1]
#define PLANE_SW_I1 owner->wave_source_plane[sw].variable[1]
#define PLANE_S_I1 owner->wave_source_plane[s].variable[1]
#define PLANE_SE_I1 owner->wave_source_plane[se].variable[1]
#define PLANE_NEW_C_I1 owner->wave_target_plane[c].variable[1]
#define PLANE_NEW_E_I1 owner->wave_target_plane[e].variable[1]
#define PLANE_NEW_NE_I1 owner->wave_target_plane[ne].variable[1]
#define PLANE_NEW_N_I1 owner->wave_target_plane[n].variable[1]
#define PLANE_NEW_NW_I1 owner->wave_target_plane[nw].variable[1]
#define PLANE_NEW_W_I1 owner->wave_target_plane[w].variable[1]
#define PLANE_NEW_SW_I1 owner->wave_target_plane[sw].variable[1]
#define PLANE_NEW_S_I1 owner->wave_target_plane[s].variable[1]
#define PLANE_NEW_SE_I1 owner->wave_target_plane[se].variable[1]
#define PLANE_PAST_C_I1 owner->wave_past_plane[c].variable[1]
#define PLANE_PAST_E_I1 owner->wave_past_plane[e].variable[1]
#define PLANE_PAST_NE_I1 owner->wave_past_plane[ne].variable[1]
#define PLANE_PAST_N_I1 owner->wave_past_plane[n].variable[1]
#define PLANE_PAST_NW_I1 owner->wave_past_plane[nw].variable[1]
#define PLANE_PAST_W_I1 owner->wave_past_plane[w].variable[1]
#define PLANE_PAST_SW_I1 owner->wave_past_plane[sw].variable[1]
#define PLANE_PAST_S_I1 owner->wave_past_plane[s].variable[1]
#define PLANE_PAST_SE_I1 owner->wave_past_plane[se].variable[1]
	//------combinations
/*Initially I left off the parenthesis around the definition of PLANE_FOUR_SUM,
which had the effect that 0.25*PLANE_FOUR_SUM was not the right thing. Macros are
dangerous. --RR, 2/17/97.*/
#define PLANE_FOUR_SUM_I1 (PLANE_E_I1+PLANE_N_I1+PLANE_W_I1+PLANE_S_I1)
#define PLANE_FIVE_SUM_I1 (PLANE_E_I1+PLANE_N_I1+PLANE_W_I1+PLANE_S_I1+PLANE_C_I1)
#define PLANE_CORNER_SUM_I1 (PLANE_NE_I1+PLANE_NW_I1+PLANE_SW_I1+PLANE_SE_I1)
#define PLANE_EIGHT_SUM_I1 (PLANE_FOUR_SUM_I1+PLANE_CORNER_SUM_I1)
#define PLANE_NINE_SUM_I1 (PLANE_EIGHT_SUM_I1+PLANE_C_I1)
/*We don't currently support more than 2 plane variables, if you want them
you need to increase PLANE_VARIABLE_COUNT in ca.hpp and rebuild CAPOW.EXE */

//begin 32 bit DLL entry
int WINAPI DllMain(HINSTANCE hInst, DWORD reason, LPVOID)
{
	return 1;
}
//end 32 bit DLL entry

//End CARULE.HPP

#endif //CARULE_HPP