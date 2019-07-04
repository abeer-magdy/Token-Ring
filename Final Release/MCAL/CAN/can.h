/*
 * can.h
 *
 *  Created on: Jul 3, 2019
 *      Author: AVE-LAP-005
 */

#ifndef MCAL_CAN_CAN_H_
#define MCAL_CAN_CAN_H_


extern void CAN0_init ();
extern void CANsend(uint32_t mesgID, uint8_t *pui8MsgData, uint32_t Mask);
extern uint8_t CANrec (uint32_t MsgID, uint32_t msgIDMask);

#endif /* MCAL_CAN_CAN_H_ */
