/*
 * ui.h
 *
 * Created: 2026/01/13 12:29:32
 *  Author: Chuck
 */ 


#ifndef UI_H_
#define UI_H_

typedef struct key_t_tag {
	uint8_t cmd;
	uint8_t stat;
	uint8_t filt;
} key_t;

typedef struct {
	uint8_t key;
} key_event_t;

#define KEY_BUF_SIZE 1

/* prototypes */
void keyInit(void);
void keyScan(void);
bool keyGetEvent(key_event_t*);
uint8_t keyGetStat(void);

//uint8_t keyGetCmd(void);




#endif /* UI_H_ */