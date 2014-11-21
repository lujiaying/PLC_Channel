/************************************************************/
/* Copyright (c) 2014                                       */
/* Wireless Signal Processing & Network Lab                 */
/* Beijing University of Posts and Telecommunications       */
/* All rights reserved                                      */
/************************************************************/

#include <opnet.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#ifndef _PLC_DEF
#define _PLC_DEF

/************************************************************/
/** Beginning of macro definitions                         **/

/* system parameters */
#define CONST_CARRIER_FREQUENCY						2.4		// in GHz
#define CONST_SYSTEM_BANDWIDTH							4e7     // in Hz (40 MHz)
#define CONST_TRAFFIC_TYPE_NUMBER						1
/* PHY parameters*/
#define CONST_OFDM_SYMBOL_DURATION					3.6e-6  // in second (400ns CP)
#define CONST_MAC_HEADER_SIZE             		40      // in Byte
#define CONST_SERVICE_FIELD_SIZE          		16      // in bit
#define CONST_PLCP_TAIL_SIZE							6       // in bit
#define CONST_PLCP_HEADER_DURATION  		   		40e-6   // in second
/* MAC parameters */
#define CONST_FRAME_LENGTH               			0.1
#define CONST_BEACON_LENGTH              			0.001
#define CONST_ASSOCIATION_REQUEST_LENGTH           	0.001
#define CONST_ASSOCIATION_RESPONSE_LENGTH         	0.001
#define CONST_PROBE_REQUEST_LENGTH              	0.001
#define CONST_PROBE_RESPONSE_LENGTH              	0.001
#define CONST_CP_LENGTH  	               			0.060
#define CONST_PROBE_REQUEST_LIMIT	              	3
#define CONST_MAX_CFP_LENGTH							0.040
#define CONST_CFP_SLOT_LENGTH							0.001
#define CONST_CW_SLOT		       						9e-6
#define CONST_SIFS            							16e-6
#define CONST_CW_MIN 										15
#define CONST_CW_MAX 										1023
#define CONST_MAX_RETRANSMISSION_LIMIT   			7
#define CONST_ACK_LENGTH									61.6e-6
/* special values */
#define CONST_NODE_INDEX_BROADCAST            	-1
/* power parameters */
#define CONST_HE_TRANSMIT_POWER_LINEAR			   0.5     // in Watt
#define CONST_CPE_TRANSMIT_POWER_LINEAR			   0.5     // in Watt
/* noise parameters */
#define CONST_BACKGROUND_NOISE_DBM_PER_HZ      -174    // in dBm
#define CONST_BACKGROUND_NOISE_DBM         		(CONST_BACKGROUND_NOISE_DBM_PER_HZ + 10.0 * log10(CONST_SYSTEM_BANDWIDTH))
#define CONST_HE_NOISE_FIGURE							5.0     // in dB
#define CONST_CPE_NOISE_FIGURE							9.0     // in dB
#define CONST_HE_THERMAL_NOISE_DBM					(CONST_BACKGROUND_NOISE_DBM + CONST_HE_NOISE_FIGURE)
#define CONST_CPE_THERMAL_NOISE_DBM					(CONST_BACKGROUND_NOISE_DBM + CONST_CPE_NOISE_FIGURE)
#define CONST_HE_THERMAL_NOISE_LINEAR				(pow(10.0, CONST_HE_THERMAL_NOISE_DBM / 10.0 - 3.0))     // in Watt
#define CONST_CPE_THERMAL_NOISE_LINEAR				(pow(10.0, CONST_CPE_THERMAL_NOISE_DBM / 10.0 - 3.0))    // in Watt

/** End of macro definitions                               **/
/************************************************************/


/************************************************************/
/** Beginning of enum data                                 **/

typedef enum INTRPT_T
{
	INTRPT_SYS_INIT = 0,                     
	INTRPT_POWER_ON = 1,
	INTRPT_MEDIUM_CHANGE = 2,
	INTRPT_PHY_TXSTART = 3,
	INTRPT_PHY_TXEND = 4,
	INTRPT_PHY_RXEND = 5,
	INTRPT_CFP_START = 6,								// interrupt code for CSMA/CA MAC
	INTRPT_CP_START = 7,								// interrupt code for CSMA/CA MAC
	INTRPT_PREPARE_TO_SEND = 8,							// interrupt code for CSMA/CA MAC
	INTRPT_DIFS_TIMEOUT = 9,							// interrupt code for CSMA/CA MAC
	INTRPT_SIFS_TIMEOUT = 10,							// interrupt code for CSMA/CA MAC
	INTRPT_CW_SLOT_TIMEOUT = 11,							// interrupt code for CSMA/CA MAC
	INTRPT_ACK_TIMEOUT = 12,								// interrupt code for CSMA/CA MAC
	INTRPT_MSDU_SENT = 13,								// interrupt code for CSMA/CA MAC
	INTRPT_ACK_SENT = 14,								// interrupt code for CSMA/CA MAC
	INTRPT_MAC_CONFIG_COMPLETE = 15,							// interrupt code for MAC control
	INTRPT_MAC_CONTROL_FRAME_RX_INDICATION = 16,				// interrupt code for MAC control
	INTRPT_BEACON_DETECTION_TIMEOUT = 17,					// interrupt code for MAC control
	INTRPT_SEND_BEACON = 18,   								// interrupt code for MAC control
	INTRPT_BEACON_SENT = 19,									// interrupt code for MAC control
	INTRPT_SEND_ASSOCIATION_REQUEST = 20,					// interrupt code for MAC control
	INTRPT_ASSOCIATION_REQUEST_SENT = 21,					// interrupt code for MAC control
	INTRPT_SEND_ASSOCIATION_RESPONSE = 22,					// interrupt code for MAC control
	INTRPT_ASSOCIATION_RESPONSE_SENT = 23,					// interrupt code for MAC control
	INTRPT_SEND_PROBE_REQUEST = 24,							// interrupt code for MAC control
	INTRPT_PROBE_REQUEST_SENT = 25,							// interrupt code for MAC control
	INTRPT_SEND_PROBE_RESPONSE = 26,							// interrupt code for MAC control
	INTRPT_PROBE_RESPONSE_SENT = 27,							// interrupt code for MAC control
	INTRPT_ASSOCIATION_RESPONSE_TIMEOUT = 28,				// interrupt code for MAC control
	INTRPT_PROBE_RESPONSE_TIMEOUT = 29,						// interrupt code for MAC control
	INTRPT_TRAFFIC_START = 30,							// interrupt code for traffic
	INTRPT_MSDU_ARRIVE = 31								// interrupt code for traffic
} INTRPT_T;	

typedef enum MAC_PROTOCOL_T
{
	MAC_PROTOCOL_CSMACA = 0,
	MAC_PROTOCOL_PRMA = 1
} MAC_PROTOCOL_T;

typedef enum MPDU_TYPE_T
{
	MPDU_TYPE_MSDU,
	MPDU_TYPE_ACK,
	MPDU_TYPE_RTS,
	MPDU_TYPE_CTS,
	MPDU_TYPE_CONTROL,
	MPDU_TYPE_INVALID
} MPDU_TYPE_T;	

typedef enum MPDU_SUBTYPE_T
{
	MPDU_SUBTYPE_BEACON,
	MPDU_SUBTYPE_ASSOCIATION_REQUEST,
	MPDU_SUBTYPE_ASSOCIATION_RESPONSE,
	MPDU_SUBTYPE_PROBE_REQUEST,
	MPDU_SUBTYPE_PROBE_RESPONSE
} MPDU_SUBTYPE_T;	

typedef enum TRAFFIC_T
{
	TRAFFIC_AUTO_METERING = 0
} TRAFFIC_T;

/** End of enum data                                       **/
/************************************************************/


/************************************************************/
/** Beginning of struct data                               **/

typedef struct COORDINATE_T
{
	double x;
	double y;
} COORDINATE_T;

typedef struct HE_PROPERTY_T
{
	COORDINATE_T coordinate;
	Objid node_id;
	Objid MAC_control_id;
	Prohandle MAC_prohandle;
	Boolean PHY_poweron_flag;
	Boolean PHY_busy_flag;
	Stathandle PHY_medium_stathandle;
	Stathandle PHY_SINR_stathandle;
	List *MAC_MSDU_buffer;
	Stathandle MAC_buffered_MSDU_number;	
} HE_PROPERTY_T;	
  
typedef struct CPE_PROPERTY_T
{
	COORDINATE_T coordinate;
	Objid node_id;
	Objid MAC_control_id;
	Objid MAC_id;
	Prohandle MAC_prohandle;
	Objid traffic_id;
	Prohandle traffic_prohandle[CONST_TRAFFIC_TYPE_NUMBER];
	Boolean PHY_poweron_flag;
	Boolean PHY_busy_flag;
	Stathandle PHY_medium_stathandle;
	Stathandle PHY_SINR_stathandle;
	List *MAC_MSDU_buffer;
	Stathandle MAC_buffered_MSDU_number;	
} CPE_PROPERTY_T;	

typedef struct BEACON_T
{
	MPDU_SUBTYPE_T subtype;
	int transmitter_node_index;  	// beacon broadcaster
	int hop_to_root;						// 0 denotes HE
	double CP_length;
	double quality;
} BEACON_T;	

typedef struct ASSOCIATION_REQUEST_T
{
	MPDU_SUBTYPE_T subtype;
	int transmitter_node_index; 
	int receiver_node_index; 
} ASSOCIATION_REQUEST_T;	

typedef struct ASSOCIATION_RESPONSE_T
{
	MPDU_SUBTYPE_T subtype;
	int transmitter_node_index;     	
	int receiver_node_index;     	
} ASSOCIATION_RESPONSE_T;	

typedef struct PROBE_REQUEST_T
{
	MPDU_SUBTYPE_T subtype;
	int transmitter_node_index;     	
} PROBE_REQUEST_T;	

typedef struct PROBE_RESPONSE_T
{
	MPDU_SUBTYPE_T subtype;
	int transmitter_node_index;     	
	int receiver_node_index;     	
	int hop_to_root;						// 0 denotes HE
	double CP_length;
	double quality;
} PROBE_RESPONSE_T;	

typedef struct MSDU_T
{
	int source_node_index;  
	int destination_node_index;
	int traffic_type;
	int MSDU_index;
	double generation_time;
	int size_bit;	      					 
} MSDU_T;	

typedef struct SEGMENT_SINR_T
{
   double segment_start_time;
	double segment_sinr_dB;
} SEGMENT_SINR_T;

typedef struct MPDU_SINR_T
{
	int actual_receiver_node_index;
	List *segment_sinr;
	double MPDU_sinr_dB;
} MPDU_SINR_T;
	
typedef struct MPDU_T
{
	Boolean broadcast_flag;
	MPDU_TYPE_T type;
	void *payload;
	double power_linear;
	int transmitter_node_index;
	int receiver_node_index;
	double start_time;
	double end_time;
	int actual_receiver_number;   
	MPDU_SINR_T *actual_receiver;
} MPDU_T;	

/** End of struct data                                     **/
/************************************************************/

#endif
