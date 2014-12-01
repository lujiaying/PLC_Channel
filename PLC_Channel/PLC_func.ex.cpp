/************************************************************/
/* Copyright (c) 2014                                       */
/* Wireless Signaling & Network Lab                         */
/* Beijing University of Posts and Telecommunications       */
/* All rights reserved                                      */
/************************************************************/

#include "PLC_def.h"
#include "PLC_data.h"
#include "PLC_func.h"

/************************************************************/
/* Author: Li Yong                                      	*/
/* Last Update: 2014.07.21                                  */
/* Remarks:                                             	*/
/************************************************************/
int
_global_HE_index(Objid lvoid_HE_id)
{
	int lvi_HE_index;
	
	FIN(_global_HE_index(lvoid_HE_id));
	
	for (lvi_HE_index = 0; lvi_HE_index < gvi_HE_number; lvi_HE_index++)
	{
		if (gvo_HE_property[lvi_HE_index].node_id == lvoid_HE_id)
		{
			FRET(lvi_HE_index);
		}
	}
	
	op_sim_end("Error: HE index can't be found!", "Error source module: PLC_func", "Error source function: _global_HE_index()", "");

	FRET(-1);
}


/************************************************************/
/* Author: Li Yong                                      	*/
/* Last Update: 2014.07.21                                  */
/* Remarks:                                             	*/
/************************************************************/
int
_global_CPE_index(Objid lvoid_CPE_id)
{
	int lvi_CPE_index;
	
	FIN(_global_CPE_index(lvoid_CPE_id));
	
	for (lvi_CPE_index = 0; lvi_CPE_index < gvi_CPE_number; lvi_CPE_index++)
	{
		if (gvo_CPE_property[lvi_CPE_index].node_id == lvoid_CPE_id)
		{
			FRET(lvi_CPE_index);
		}
	}
	
	op_sim_end("Error: CPE index can't be found!", "Error source module: PLC_func", "Error source function: _global_CPE_index()", "");

	FRET(-1);
}


/************************************************************/
/* Author: Li Yong                                      	*/
/* Last Update: 2014.07.21                                  */
/* Remarks:                                             	*/
/************************************************************/
double
_global_get_pathloss_dB(COORDINATE_T lvo_node_1, COORDINATE_T lvo_node_2)
{
	double lvd_x_diff, lvd_y_diff;
	double lvd_distance;
	double lvd_pathloss;
	const double a0 = -2.03e-3, a1 = 3.75e-7, f = 2e7, k = 0.7;
		
	FIN(_global_get_pathloss_dB());	
	
	lvd_x_diff = lvo_node_1.x - lvo_node_2.x; 
	lvd_y_diff = lvo_node_1.y - lvo_node_2.y; 
	lvd_distance = sqrt(lvd_x_diff * lvd_x_diff + lvd_y_diff * lvd_y_diff);
	
	/* the two nodes are the same */
	if ((lvd_distance >= 0.0) && (lvd_distance <= 0.0))
	{
	    lvd_pathloss = 0.0;
	}	
	/* otherwise*/
	else
	{	
		lvd_pathloss = -20.0 * log10(exp(-(a0+a1*pow(f,k))*lvd_distance));  // M.Zimmermann channel model
		// reference: M.Zimmermann and K.Dostert, "A multipath model for the powerline channel," IEEE Trans. Commun., vol.50, no.4, pp.553-559, Apr.2002.
	}	

	FRET(lvd_pathloss);
	
}


/************************************************************/
/* Author: Li Yong	                                     	*/
/* Last Update: 2011.11.11                                  */
/* Remarks:                                             	*/
/************************************************************/
double 
_global_SINR_to_BLER(double lvd_SINR)
{
	double lvd_BLER;
	
	FIN(_global_SINR_to_BLER());

	// the following codes are only for debug
	// use your modulation curve instead here
	if (lvd_SINR >= 50)
	{
	    lvd_BLER = 0.0;
	}	
	else if (lvd_SINR >= 40 && lvd_SINR < 50)
	{
	    lvd_BLER = 0.0;
	}	
	else if (lvd_SINR >= 30 && lvd_SINR < 40)
	{
	    lvd_BLER = 0.2;
	}	
	else if (lvd_SINR >= 20 && lvd_SINR < 30)
	{
	    lvd_BLER = 0.5;
	}	
	else if (lvd_SINR >= 10 && lvd_SINR < 20)
	{
	    lvd_BLER = 1.0;
	}	
	else if (lvd_SINR >= 0 && lvd_SINR < 10)
	{
	    lvd_BLER = 1.0;
	}
	else
	{
	    lvd_BLER = 1.0;
	}	

	FRET(lvd_BLER);
}	


/************************************************************/
/* Author: Li Yong	                                     	*/
/* Last Update: 2011.11.11                                  */
/* Remarks:                                             	*/
/************************************************************/
Boolean 
_global_BLER_draw(double lvd_BLER)
{
	FIN(_global_BLER_draw());

	if (op_dist_uniform(1.0) < lvd_BLER)
	{	
		FRET(OPC_FALSE);
	}	
	else
	{
		FRET(OPC_TRUE); 
	}	
}	
