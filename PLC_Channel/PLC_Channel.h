#include <opnet.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#ifndef _PLC_CHANNEL
#define _PLC_CHANNEL

/* enum */
typedef enum NODE_TYPE_T
{
    TYPE_HE = 0,
    TYPE_CPE = 1,
    TYPE_NOISE = 2,
    TYPE_X = 3,
} NODE_TYPE_T;

typedef enum PHASE_T
{
    PHASE_ABC = 0,
    PHASE_A = 1,
    PHASE_B = 2,
    PHASE_C = 3,
} PHASE_T;
/* end enum */


/* struct */
typedef struct NODE_T
{
    int node_id;
    NODE_TYPE_T type;
    int parent_id;
    double distance;
    PHASE_T phase;
} NODE_T;


typedef struct DISTANCE_PHASE_T
{
    double dis_x;
    double dix_Y;
    double dis_Z;
    PHASE_T pha_X;
    PHASE_T pha_Y;
    PHASE_T pha_Z;
} DISTANCE_PHASE_T;
/* end struct */

#endif
