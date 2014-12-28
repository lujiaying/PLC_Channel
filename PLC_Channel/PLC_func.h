/************************************************************/
/* Copyright (c) 2014                                       */
/* Wireless Signaling & Network Lab                         */
/* Beijing University of Posts and Telecommunications       */
/* All rights reserved                                      */
/************************************************************/

#include "PLC_def.h"
#include "PLC_data.h"
#include "PLC_Channel.h"

#ifndef _PLC_FUNC
#define _PLC_FUNC

int _global_HE_index_find(Objid);
int _global_CPE_index_find(Objid);
double _global_pathloss_dB_calculate(COORDINATE_T, COORDINATE_T);
double _global_SINR_to_BLER(double, MPDU_TYPE_T);
Boolean _global_BLER_random_draw(double);
void _global_MPDU_destroy(MPDU_T *);
double _global_MPDU_duration_calculate(MPDU_T *);
int _global_MPDU_verify(MPDU_T *, int);

/* add by jiaying.lu */
int _global_self_index_find(Objid node_id);

#endif
