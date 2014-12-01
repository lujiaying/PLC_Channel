#ifndef _PLC_CHANNEL
#define _PLC_CHANNEL

extern Objid gvoid_channel_id;
extern int gvi_HE_num, gvi_CPE_num, gvi_NOISE_num, gvi_X_num, gvi_total_num;

typedef enum INTRPT_CHANNEL_T
{
	INTRPT_CHANNEL_INITED = 31,
	INTRPT_CHANNEL_TIME_TO_UPDATE = 32,
	INTRPT_CHANNEL_PPDU_START = 33,
	INTRPT_CHANNEL_PPDU_END = 34,
} INTRPT_CHANNEL_T;

#endif
