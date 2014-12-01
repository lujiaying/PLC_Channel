/************************************************************/
/* Copyright (c) 2014-2015                                  */
/* Wireless Signal Processing & Network Lab                 */
/* Beijing University of Posts and Telecommunications       */
/* All rights reserved                                      */
/************************************************************/

#ifndef _PLC_DATA
#define _PLC_DATA

extern int gvi_MAC_protocol;
extern double gvd_PHY_medium_busy_threshold_dBm;
extern double gvd_poweron_deadline;
extern int gvi_traffic[CONST_TRAFFIC_TYPE_NUMBER];
extern Objid gvoid_subnet;
//extern Objid gvoid_channel;
extern int gvi_HE_number;
extern int gvi_CPE_number;
extern HE_PROPERTY_T *gvo_HE_property;
extern CPE_PROPERTY_T *gvo_CPE_property;
extern double **gvo_pathloss_matrix_dB;
extern char *gvch_traffic_name[CONST_TRAFFIC_TYPE_NUMBER];

#endif
