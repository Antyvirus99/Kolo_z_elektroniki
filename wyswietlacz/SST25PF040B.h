/*
 * SST25PF040B.h
 *
 * Created: 20.04.2017 19:07:08
 *  Author: Donat
 */ 


#ifndef SST25PF040B_H_
#define SST25PF040B_H_

#define SST25_PIN_CE PD6
#define SST25_PIN_WP PD5
#define SST25_PIN_HOLD PB7


extern void SST25PF040B_read_status (void);
extern void SST25PF040B_read_jedec (void);
extern void SST25_ustaw_piny (void);
extern void read_id (void);
extern void read_status_register (void);

#endif /* SST25PF040B_H_ */