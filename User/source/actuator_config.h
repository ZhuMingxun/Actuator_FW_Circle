#ifndef _ACTUATOR_CONFIG_H
#define _ACTUATOR_CONFIG_H

//#define WATCH_DOG
#define WATCH_DOG_RSTVAL 0x35

//#define PHASE_SEQ //380
//#define KAIGUNA_MODE //ฟชนุ
//#define TRAVEL_PROTECT_MACHINE  //ป๚ะต



/*
*	0-H-220-PT-DZ
*	1-H-220-PT-JX
*	2-H-220-PK-DZ
*	3-H-220-PK-JX
*	4-H-380-PT-DZ
*	5-H-380-PT-JX
*	6-H-380-PK-DZ
*	7-H-380-PK-JX
*/

#define MODEL	4

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