/************************************************************/
/* Copyright (c) 2014                                       */
/* Wireless Signal Processing & Network Lab                 */
/* Beijing University of Posts and Telecommunications       */
/* All rights reserved                                      */
/************************************************************/

#include "PLC_def.h"

int gvi_MAC_protocol;
double gvd_PHY_medium_busy_threshold_dBm;
double gvd_poweron_deadline;
Objid gvoid_subnet;
int gvi_HE_number;
int gvi_CPE_number;
HE_PROPERTY_T *gvo_HE_property;
CPE_PROPERTY_T *gvo_CPE_property;
Objid gvoid_channel;
List *gvlist_channel;
double **gvo_pathloss_matrix_dB;
int gvi_traffic[CONST_TRAFFIC_TYPE_NUMBER];
char *gvch_traffic_name[CONST_TRAFFIC_TYPE_NUMBER];

