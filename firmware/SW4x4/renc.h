/*
 * renc.h
 *
 * Created: 2026/01/14 11:00:18
 *  Author: Chuck
 */ 


#ifndef RENC_H_
#define RENC_H_

typedef enum { cRE_NoTurn = ' ', cRE_RightTurn = 'R', cRE_LeftTurn = 'L' } re_t;
#define RE_BUF_SIZE 16

typedef struct {
	re_t rotation;
} re_event_t;

/* prototypes */

void reInit(void);
void reEnable(void);
void reDisable(void);
void reScan(void);
bool reGetEvent(re_event_t*);
//re_t reGetCmd(void);



#endif /* RENC_H_ */