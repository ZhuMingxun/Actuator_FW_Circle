#ifndef _ACTUATOR_CONFIG_H
#define _ACTUATOR_CONFIG_H

#define WATCH_DOG
#define WATCH_DOG_RSTVAL 0x36



/*
*	0-Y-220-PT-DZ
*	1-Y-220-PT-JX
*	2-Y-220-PK-DZ
*	3-Y-220-PK-JX
*	4-Y-380-PT-DZ
*	5-Y-380-PT-JX
*	6-Y-380-PK-DZ
*	7-Y-380-PK-JX
*/

#define MODEL	7

#if (MODEL&(1<<2))
#define PHASE_SEQ //380
#endif

#if (MODEL&(1<<1))
#define KAIGUAN_MODE
#endif

#if (MODEL&(1<<0))
#define TRAVEL_PROTECT_MACHINE
#endif


#endif