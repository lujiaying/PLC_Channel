#include <opnet.h>

#ifndef _PLC_CHANNEL
#define _PLC_CHANNEL

/* enum */
typedef enum INTRPT_CHANNEL_T
{
	INTRPT_CHANNEL_INITED = 31,
	INTRPT_CHANNEL_TIME_TO_UPDATE = 32,
	INTRPT_CHANNEL_PPDU_START = 33,
	INTRPT_CHANNEL_PPDU_END = 34,
	INTRPT_NOISE_PULSE_START = 71,
	INTRPT_NOISE_POWER_UPDATE = 72,
} INTRPT_CHANNEL_T;

typedef enum PHASE_T
{
	PHASE_A = 0,
    PHASE_B = 1,
    PHASE_C = 2,
    PHASE_ABC = 3,
} PHASE_T;

typedef enum PPDU_TYPE_T
{
	PPDU_TYPE_DATA = 0,
	PPDU_TYPE_CONTROL = 1,
	PPDU_TYPE_NOISE = 3,
} PPDU_TYPE_T;


/* struct */
typedef struct PPDU_SEGMENT_SINR_T
{
	double segment_start_time;
	double segment_sinr_dB;
} PPDU_SEGMENT_SINR_T;


typedef struct PPDU_SINR_T
{
	int actual_receiver_node_index;
	Prg_List *segment_sinr;
	double PPDU_sinr_dB;
} PPDU_SINR_T;


typedef struct PPDU_T
{
	int transmitter_node_index;
	int receiver_node_index;
	int type;
	double power_linear;
	double start_time;
	double end_time;
	void *MPDU_ptr;
	PHASE_T send_phase;
	int PPDU_index;
	int actual_receiver_number;
	PPDU_SINR_T *actual_receiver_array;
} PPDU_T;

typedef struct NODE_OBJID_T
{
	Objid node_id;
	Objid phy_id;
} NODE_OBJID_T;

/* global var */
//extern Objid gvoid_channel, gvoid_noise_generater, gvoid_PHY;
extern Objid gvoid_channel;
extern int gvi_HE_num, gvi_CPE_num, gvi_NOISE_num, gvi_X_num, gvi_total_num;
extern NODE_OBJID_T *gvoid_node_oids;

#endif
