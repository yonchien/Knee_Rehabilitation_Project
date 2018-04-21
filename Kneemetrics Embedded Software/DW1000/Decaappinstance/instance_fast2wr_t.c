// -------------------------------------------------------------------------------------------------------------------
//
//  File: instance_fast2wr_t.c - application level message exchange for ranging demo Tag instance
//
//  Copyright 2014 (c) DecaWave Ltd, Dublin, Ireland.
//
//  All rights reserved.
//
//  Author: Zoran Skrba, December 2014
//
// -------------------------------------------------------------------------------------------------------------------

#include "compiler.h"
#include "deca_port.h"
#include "deca_device_api.h"
#include "deca_spi.h"

#include "instance.h"

// -------------------------------------------------------------------------------------------------------------------


// -------------------------------------------------------------------------------------------------------------------
//
// the main instance state machine (for Tag instance mode only!)
//
// -------------------------------------------------------------------------------------------------------------------
//
int testapprun_tf(instance_data_t *inst, int message)
{

    switch (inst->testAppState)
    {
        case TA_INIT :
            // printf("TA_INIT") ;
            switch (inst->mode)
            {
                case TAG:
                {
                    dwt_enableframefilter(DWT_FF_DATA_EN | DWT_FF_ACK_EN); //allow data and ack frames;
                    inst->frameFilteringEnabled = 1 ;
                    dwt_setpanid(inst->panid);
                    dwt_seteui(inst->eui64);
            		inst->msg_f.panID[0] = (inst->panid) & 0xff;
                	inst->msg_f.panID[1] = inst->panid >> 8;

                    inst->mode = TAG_TDOA ;
                    inst->testAppState = TA_TXBLINK_WAIT_SEND;
                    memcpy(&inst->blinkmsg.tagID[0], &inst->eui64[0], BLINK_FRAME_SOURCE_ADDRESS);

                    //can use RX auto re-enable when not logging/plotting errored frames
                    inst->rxautoreenable = 1;

                    dwt_setautorxreenable(inst->rxautoreenable); //not necessary to auto RX re-enable as the receiver is on for a short time (Tag knows when the response is coming)

                    //disable double buffer for a Tag - not needed....
                    dwt_setdblrxbuffmode(0); //enable/disable double RX buffer

                    //NOTE - Auto ACK only works if frame filtering is enabled!
                    dwt_enableautoack(ACK_RESPONSE_TIME); //wait for ACK_RESPONSE_TIME symbols (e.g. 5) before replying with the ACK


                    //NOTE: on the EVK1000 the DEEPSLEEP is not actually putting the DW1000 into full DEEPSLEEP mode as XTAL is kept on
                    dwt_configuresleep(DWT_LOADUCODE|DWT_PRESRV_SLEEP|DWT_CONFIG|DWT_TANDV, DWT_WAKE_CS|DWT_SLP_EN); //configure the on wake parameters (upload the IC config settings)

                }
                break;
                default:
                break;
            }
            break; // end case TA_INIT

        case TA_SLEEP_DONE :
        {
        	event_data_t* dw_event = instance_getevent(20); //clear the event from the queue
			// waiting for timout from application to wakup IC
			if (dw_event->type != DWT_SIG_RX_TIMEOUT)
            {
                inst->done = INST_DONE_WAIT_FOR_NEXT_EVENT; //wait here for sleep timeout
                break;
            }

            inst->done = INST_NOT_DONE_YET;
            inst->instToSleep = 0;
            inst->testAppState = inst->nextState;
            inst->nextState = 0; //clear
            inst->canprintinfo = 0;
            //wake up from DEEP SLEEP
            {
                //wake up device from low power mode
                //NOTE - in the ARM  code just drop chip select for 200us
                port_SPIx_clear_chip_select();  //CS low
                Sleep(1);   //200 us to wake up then waits 5ms for DW1000 XTAL to stabilise
                port_SPIx_set_chip_select();  //CS high
                Sleep(5);

                //this is platform dependent - only program if DW EVK/EVB
                dwt_setleds(1);

                //MP bug - TX antenna delay needs reprogramming as it is not preserved
                dwt_settxantennadelay(inst->txantennaDelay) ;

                //set EUI as it will not be preserved unless the EUI is programmed and loaded from NVM
                /*if((inst->mode == TAG) || (inst->mode == TAG_TDOA))
                {
                    dwt_setpanid(inst->panid);
                    dwt_seteui(inst->eui64);
                }*/
                dwt_entersleepaftertx(0);
                dwt_setinterrupt(DWT_INT_TFRS, 1); //re-enable the TX/RX interrupts

            }
        }
            break;

        case TA_TXE_WAIT : //either go to sleep or proceed to TX a message
            // printf("TA_TXE_WAIT") ;
            //if we are scheduled to go to sleep before next sending then sleep first.
            if(((inst->nextState == TA_TXPOLL_WAIT_SEND)
                || (inst->nextState == TA_TXBLINK_WAIT_SEND))
                    && (inst->instToSleep)  //go to sleep before sending the next poll
                    )
            {
                //the app should put chip into low power state and wake up in tagSleepTime_ms time...
                //the app could go to *_IDLE state and wait for uP to wake it up...
                inst->done = INST_DONE_WAIT_FOR_NEXT_EVENT_TO; //don't sleep here but kick off the TagTimeoutTimer (instancetimer)
                inst->testAppState = TA_SLEEP_DONE;

                if(inst->nextState == TA_TXBLINK_WAIT_SEND)
                inst->canprintinfo = 1;

                //put device into low power mode
                dwt_entersleep(); //go to sleep

            }
            else //proceed to configuration and transmission of a frame
            {

				inst->testAppState = inst->nextState;
               	inst->nextState = 0; //clear
            }
            break ; // end case TA_TXE_WAIT

        case TA_TXBLINK_WAIT_SEND :
            {
                //blink frames with IEEE EUI-64 tag ID
                inst->blinkmsg.frameCtrl = 0xC5 ;
                inst->blinkmsg.seqNum = inst->frame_sn++;

                dwt_writetxdata((BLINK_FRAME_CRTL_AND_ADDRESS + FRAME_CRC), (uint8 *)  (&inst->blinkmsg), 0) ;  // write the frame data
                dwt_writetxfctrl((BLINK_FRAME_CRTL_AND_ADDRESS + FRAME_CRC), 0);

                //response will be sent after 500us (thus delay the receiver turn on by 290sym ~ 299us)
                //use delayed rx on (wait4resp timer) - this value is applied when the TX frame is done/sent, so this value can be written after TX is started
				dwt_setrxaftertxdelay(inst->rnginitW4Rdelay_sy);  //units are ~us - wait for wait4respTIM before RX on (delay RX)
                dwt_setrxtimeout(inst->fwtoTimeB_sy);  //units are us - wait for BLINKRX_FWTO_TIME after RX on before timing out

                dwt_starttx(DWT_START_TX_IMMEDIATE | DWT_RESPONSE_EXPECTED); //always using immediate TX

                inst->sentSN = inst->blinkmsg.seqNum;
                inst->wait4ack = DWT_RESPONSE_EXPECTED; //Poll is coming soon after...

                inst->instToSleep = 1;
                inst->testAppState = TA_TX_WAIT_CONF ; // wait confirmation
                inst->previousState = TA_TXBLINK_WAIT_SEND ;
                inst->done = INST_DONE_WAIT_FOR_NEXT_EVENT; //will use RX FWTO to time out (set below)

            }
            break ; // end case TA_TXBLINK_WAIT_SEND

        case TA_TXPOLL_WAIT_SEND :
            {

                //NOTE the anchor address is set after receiving the ranging initialisation message
                inst->instToSleep = 1; //we'll sleep after this ranging exchange (i.e. before sending the next poll)

                inst->msg_f.messageData[FCODE] = RTLS_DEMO_MSG_TAG_POLLF;

    			inst->psduLength = TAG_POLL_F_MSG_LEN + FRAME_CRTL_AND_ADDRESS_S + FRAME_CRC + EXTRA_LENGTH;

				//set frame type (0-2), SEC (3), Pending (4), ACK (5), PanIDcomp(6)
                inst->msg_f.frameCtrl[0] = 0x41 /*PID comp*/;

                //short address for both
                inst->msg_f.frameCtrl[1] = 0x8 /*dest short address (16bits)*/ | 0x80 /*src short address (16bits)*/;

                inst->msg_f.seqNum = inst->frame_sn++;


                inst->wait4ack = DWT_RESPONSE_EXPECTED; //Response is coming after 275 us...
                //500 -> 485, 800 -> 765
                dwt_writetxfctrl(inst->psduLength, 0);

                //if the response is expected there is a 1ms timeout to stop RX if no response (ACK or other frame) coming
                dwt_setrxtimeout(inst->fwtoTime_sy);  //units are us - wait for 215us after RX on

                //use delayed rx on (wait4resp timer)
                dwt_setrxaftertxdelay(inst->fixedReplyDelay_sy);  //units are ~us - wait for wait4respTIM before RX on (delay RX)

                dwt_writetxdata(inst->psduLength, (uint8 *)  &inst->msg_f, 0) ;   // write the poll frame data

                //start TX of frame
                dwt_starttx(DWT_START_TX_IMMEDIATE | inst->wait4ack);
                inst->sentSN = inst->msg_f.seqNum;

                //write the final function code
                inst->msg_f.messageData[FCODE] = RTLS_DEMO_MSG_TAG_FINALF;
                //increment the sequence number for the final message
                inst->msg_f.seqNum = inst->frame_sn;

                inst->testAppState = TA_TX_WAIT_CONF ;                                               // wait confirmation
                inst->previousState = TA_TXPOLL_WAIT_SEND ;
                inst->done = INST_DONE_WAIT_FOR_NEXT_EVENT; //will use RX FWTO to time out (set below)

                inst->responseRxNum = 0;

            }
            break;


        case TA_TX_WAIT_CONF :
           //printf("TA_TX_WAIT_CONF") ;
            {
            	//uint8 temp[5];
            	event_data_t* dw_event = instance_getevent(5); //get and clear this event

                //NOTE: Can get the ACK before the TX confirm event for the frame requesting the ACK
                //this happens because if polling the ISR the RX event will be processed 1st and then the TX event
                //thus the reception of the ACK will be processed before the TX confirmation of the frame that requested it.
                if(dw_event->type != DWT_SIG_TX_DONE) //wait for TX done confirmation
                {
                    if(dw_event->type == DWT_SIG_RX_TIMEOUT) //got RX timeout - i.e. did not get the response (e.g. ACK)
                    {
                    	//we need to wait for SIG_TX_DONE and then process the timeout and re-send the frame if needed
                    	inst->gotTO = 1;
                    }
                    if(dw_event->type == SIG_RX_ACK)
                    {
                        inst->wait4ack = 0 ; //clear the flag as the ACK has been received
                        inst_processackmsg(inst, dw_event->msgu.rxackmsg.seqNum);
                        //printf("RX ACK in TA_TX_WAIT_CONF... wait for TX confirm before changing state\n");
                    }

                    inst->done = INST_DONE_WAIT_FOR_NEXT_EVENT;
                    break;

                }

                inst->done = INST_NOT_DONE_YET;

                if(inst->previousState == TA_TXFINAL_WAIT_SEND) //tag will do immediate receive when waiting for report (as anchor sends it without delay)
                //anchor is not sending the report to tag
                {

                    inst->done = INST_DONE_WAIT_FOR_NEXT_EVENT_TO; //kick off the TagTimeoutTimer (instance timer) to initiate wakeup
                    inst->nextState = TA_TXPOLL_WAIT_SEND;
                    inst->testAppState = TA_TXE_WAIT; //we are going manually to sleep - change to TA_TXE_WAIT state

                    break;
                }
                else if (inst->gotTO) //timeout
                {
                	inst_processrxtimeout(inst);
                	inst->gotTO = 0;
                }
                else
                {

                    if(inst->previousState == TA_TXPOLL_WAIT_SEND)
					{

						// write the final's frame control and address tx data (add CRC as the function will write length - 2)
						dwt_writetxdata((FRAME_CRTL_AND_ADDRESS_S + 1 + FRAME_CRC), (uint8 *)  &inst->msg_f, FINAL_MSG_OFFSET) ;   // write the final frame data

						dwt_entersleepaftertx(1);
						dwt_setinterrupt(DWT_INT_TFRS, 0); //disable all the interrupts (wont be able to enter sleep if interrupts are pending)

						inst->tagPollTxTime32l = dw_event->timeStamp32l;
						inst->relpyAddress[0] = inst->msg_f.destAddr[0];
						inst->relpyAddress[1] = inst->msg_f.destAddr[1];
						inst->canprintinfo = 2;
					}

                    if(inst->previousState == TA_TXRANGINGINIT_WAIT_SEND) //set frame control for the response message
					{
						dwt_writetxfctrl((ANCH_RESPONSE_F_MSG_LEN + FRAME_CRTL_AND_ADDRESS_S + FRAME_CRC + EXTRA_LENGTH), RESPONSE_MSG_OFFSET);
					}

                    inst->testAppState = TA_RX_WAIT_DATA ;              // wait for next frame
                    //turn RX on
                    if(inst->wait4ack == 0)
                    	dwt_rxenable(0) ;               // turn receiver on, immediate = 0/delayed = 1

                    inst->wait4ack = 0 ;
                    //dwt_readfromdevice(0x19, 0, 5, temp);
                    //sprintf((char*)&usbdata[20], "T2R%d %02x%02x%02x%02x%02x ", count, temp[4], temp[3], temp[2], temp[1], temp[0]);
                    //send_usbmessage(&usbdata[20], 16);

                    //count=0;
                }

            }

            break ; // end case TA_TX_WAIT_CONF

        case TA_RXE_WAIT :
			//printf("TA_RXE_WAIT") ;
			{
				// - with "fast" ranging - we only get here after frame timeout...
				//turn RX on
				instancerxon(inst, 0, 0) ;   // turn RX on, with/without delay

				inst->testAppState = TA_RX_WAIT_DATA;   // let this state handle it

				// end case TA_RXE_WAIT, don't break, but fall through into the TA_RX_WAIT_DATA state to process it immediately.
				if(message == 0) break;
			}

        case TA_RX_WAIT_DATA :                                                                     // Wait RX data
           //printf("TA_RX_WAIT_DATA") ;

            switch (message)
            {

                case SIG_RX_BLINK :
                {
                	instance_getevent(6); //get and clear this event
                    //else //not initiating ranging - continue to receive
                    {
                        inst->testAppState = TA_RX_WAIT_DATA ;              // wait for next frame
                        //turn RX on
                        dwt_rxenable(0) ;               // turn receiver on, immediate = 0/delayed = 1
                        inst->done = INST_NOT_DONE_YET;
                    }

                }
                break;

                case SIG_RX_ACK :
                {
					event_data_t* dw_event = instance_getevent(7); //get and clear this event
					inst_processackmsg(inst, dw_event->msgu.rxackmsg.seqNum);
                    //else we did not expect this ACK turn the RX on again
                    //only enable receiver when not using double buffering
                    inst->testAppState = TA_RX_WAIT_DATA ;              // wait for next frame
                    //turn RX on
                    dwt_rxenable(0) ;               // turn receiver on, immediate = 0/delayed = 1
                    inst->done = INST_NOT_DONE_YET;
                }
                break;

                case DWT_SIG_RX_OKAY :
                {
					event_data_t* dw_event = instance_getevent(8); //get and clear this event
					uint8  srcAddr[8] = {0,0,0,0,0,0,0,0};
					int fcode = 0;
					int fn_code = 0;
					int srclen = 0;
					int fctrladdr_len;
					uint8 *messageData;

					inst->stoptimer = 0; //clear the flag, as we have received a message

					// 16 or 64 bit addresses
					switch(dw_event->msgu.frame[1])
					{
						case 0xCC: //
							memcpy(&srcAddr[0], &(dw_event->msgu.rxmsg_ll.sourceAddr[0]), ADDR_BYTE_SIZE_L);
							fn_code = dw_event->msgu.rxmsg_ll.messageData[FCODE];
							messageData = &dw_event->msgu.rxmsg_ll.messageData[0];
							srclen = ADDR_BYTE_SIZE_L;
							fctrladdr_len = FRAME_CRTL_AND_ADDRESS_L;
							break;
						case 0xC8: //
							memcpy(&srcAddr[0], &(dw_event->msgu.rxmsg_sl.sourceAddr[0]), ADDR_BYTE_SIZE_L);
							fn_code = dw_event->msgu.rxmsg_sl.messageData[FCODE];
							messageData = &dw_event->msgu.rxmsg_sl.messageData[0];
							srclen = ADDR_BYTE_SIZE_L;
							fctrladdr_len = FRAME_CRTL_AND_ADDRESS_LS;
							break;
						case 0x8C: //
							memcpy(&srcAddr[0], &(dw_event->msgu.rxmsg_ls.sourceAddr[0]), ADDR_BYTE_SIZE_S);
							fn_code = dw_event->msgu.rxmsg_ls.messageData[FCODE];
							messageData = &dw_event->msgu.rxmsg_ls.messageData[0];
							srclen = ADDR_BYTE_SIZE_S;
							fctrladdr_len = FRAME_CRTL_AND_ADDRESS_LS;
							break;
						case 0x88: //
							memcpy(&srcAddr[0], &(dw_event->msgu.rxmsg_ss.sourceAddr[0]), ADDR_BYTE_SIZE_S);
							fn_code = dw_event->msgu.rxmsg_ss.messageData[FCODE];
							messageData = &dw_event->msgu.rxmsg_ss.messageData[0];
							srclen = ADDR_BYTE_SIZE_S;
							fctrladdr_len = FRAME_CRTL_AND_ADDRESS_S;
							break;
					}

					if((inst->ackexpected) && (inst->ackTO)) //ACK frame was expected but we got a good frame - treat as ACK timeout
					{
						//printf("got good frame instead of ACK in DWT_SIG_RX_OKAY - pretend TO\n");
						inst_processrxtimeout(inst);
						message = 0; //clear the message as we have processed the event
					}
                    else
                    {

                    	inst->ackexpected = 0; //clear this as we got good frame (but as not using ACK TO) we prob missed the ACK - check if it has been addressed to us

                    	fcode = fn_code; //tag has address filtering so if it received a frame it must be addressed to it

                        switch(fcode)
                        {
							case RTLS_DEMO_MSG_RNG_INIT:
							{
								if(inst->mode == TAG_TDOA) //only start ranging with someone if not ranging already
								{
									//double delay = rxrngmsg->messageData[RES_T1] + (rxrngmsg->messageData[RES_T2] << 8); //in ms

									inst->testAppState = TA_TXPOLL_WAIT_SEND ; // send next poll
									//remember the anchor address
									inst->msg_f.destAddr[0] =  srcAddr[0];
									inst->msg_f.destAddr[1] =  srcAddr[1];
									inst->msg_f.sourceAddr[0] = messageData[RES_R1];
									inst->msg_f.sourceAddr[1] = messageData[RES_R1+1];

									inst->tagShortAdd = (uint16)messageData[RES_R1] + ((uint16)messageData[RES_R2] << 8) ;
									dwt_setaddress16(inst->tagShortAdd);

									//instancesetreplydelay(delay); //

									inst->mode = TAG ;
									inst->rxTimeouts = 0; //reset timeout count
									inst->instToSleep = 0; //don't go to sleep - start ranging instead and then sleep after 1 range is done
									inst->done = INST_NOT_DONE_YET;
								}
							}
							break; //RTLS_DEMO_MSG_RNG_INITF

                            case RTLS_DEMO_MSG_ANCH_RESPF:
                            {
#if (TWSYMRANGE == 1)
								//need to write the delayed time before starting transmission
								inst->delayedReplyTime32 = ((uint32)dw_event->timeStamp32h + (uint32)inst->fixedFastReplyDelay32h) ;
								dwt_setdelayedtrxtime(inst->delayedReplyTime32) ;

								dwt_writetxfctrl((TAG_FINAL_F_MSG_LEN + FRAME_CRTL_AND_ADDRESS_S + FRAME_CRC), FINAL_MSG_OFFSET);

								if(dwt_starttx(DWT_START_TX_DELAYED))
								{
									//error - TX FAILED
									// initiate the re-transmission
									inst->testAppState = TA_TXE_WAIT ;
									inst->nextState = TA_TXPOLL_WAIT_SEND;

									dwt_entersleepaftertx(0);

									inst->wait4ack = 0; //clear the flag as the TX has
									inst->lateTX++;
									break; //exit this switch case...
								}
								else
								{
									rtd_t rtd;
									//calculate the difference between response rx and final tx
									//here we just need to subtract the low 32 bits as the response delay is < 32bits (actually it is < 26 bits)
									rtd.diffRmP = (uint32)dw_event->timeStamp32l - (uint32)inst->tagPollTxTime32l ;
									//calculate difference between final tx and response rx
									rtd.diffFmR = (uint32)inst->txantennaDelay + ((uint32)inst->fixedFastReplyDelay32h << 8) - ((uint32)dw_event->timeStamp32l & 0x1FF);
									//write the rest of the message (the two response time differences (low 32 bits)
									dwt_writetxdata((TAG_FINAL_F_MSG_LEN - 1 + FRAME_CRC), (uint8 *)  &rtd, (FINAL_MSG_OFFSET+FRAME_CRTL_AND_ADDRESS_S+1)) ;   // write the frame data

									inst->sentSN = inst->msg_f.seqNum;
									inst->previousState = TA_TXFINAL_WAIT_SEND;
									//if Tag is not waiting for report - it will go to sleep automatically after the final is sent
									inst->done = INST_DONE_WAIT_FOR_NEXT_EVENT_TO; //kick off the TagTimeoutTimer (instancetimer) to initiate wakeup
									inst->testAppState = TA_SLEEP_DONE; //we are going automatically to sleep so no TX confirm interrupt (next state will be TA_SLEEP_DONE)
									inst->canprintinfo = 1;
									inst->txmsgcount ++;
									inst->frame_sn++ ; //increment as final is sent
								}

								inst->respPSC = (dwt_read16bitoffsetreg(0x10, 2) >> 4);
								inst->wait4ack = 0; //no response
								inst->ackexpected = !ACK_REQUESTED ; //used to ignore unexpected ACK frames
								//inst->rxu.anchorRespRxTime = inst->rxu.rxTimeStamp ; //Response's Rx time

								inst->nextState = TA_TXPOLL_WAIT_SEND;
#else
								if(inst->responseRxNum == 0) // this is first response
								{
									dwt_setrxtimeout(5000); //~5ms
									//turn RX on
									dwt_rxenable(0) ;               // turn receiver on, immediate = 0/delayed = 1
									inst->anchResp1RxTime32l = dw_event->timeStamp32l;
									inst->responseRxNum++;
								}
								else // we have two responses and can calculate ToF
								{
									//the first response will be sent time X after reception of the poll, but as the tx time is snapped to 8ns
									//we need to account for the low 9 bits of poll rx time in the RTD calculation
									uint32 pollrxlowbits = (uint32)messageData[1] + (uint32)(messageData[2] << 8);

									//RTD = (RxResp1 - TxPoll) - (RxResp2 - RxResp1)
									//ToF = RTD/2 = RxResp1 - 0.5 * (TxPoll + RxResp2)

									inst->tof32 = ((uint32)inst->anchResp1RxTime32l - (uint32)inst->tagPollTxTime32l + pollrxlowbits) - ((uint32)dw_event->timeStamp32l - (uint32)inst->anchResp1RxTime32l);

									inst->tof32 <<= 1; //to make it compatible with reportTOF() which expects ToF*4

									reportTOF_f(inst);
                                    inst->newrange = 1;
                                    inst->testAppState = TA_TXE_WAIT ;
                                    inst->nextState = TA_TXPOLL_WAIT_SEND;
								}
#endif


                            }
                            break; //RTLS_DEMO_MSG_ANCH_RESPF

                            case RTLS_DEMO_MSG_ANCH_TOFRF:
                            {
                                    inst->tof32 = messageData[TOFR];
                                    inst->tof32 += (uint32)messageData[TOFR+1] << 8;
                                    inst->tof32 += (uint32)messageData[TOFR+2] << 16;
                                    inst->tof32 += (uint32)messageData[TOFR+3] << 24;

                                    if(dw_event->msgu.rxmsg_ss.seqNum != inst->lastReportSN)
                                    {
                                    	reportTOF_f(inst);
                                        inst->newrange = 1;
                                        inst->lastReportSN = dw_event->msgu.rxmsg_ss.seqNum;
                                    }

                                    inst->testAppState = TA_TXE_WAIT;
                                    inst->nextState = TA_TXPOLL_WAIT_SEND ; // send next poll

                            }
                            break; //RTLS_DEMO_MSG_ANCH_TOFRF

                            default:
                            {
                                //only enable receiver when not using double buffering
                                inst->testAppState = TA_RX_WAIT_DATA ;              // wait for next frame
                                //turn RX on
                                dwt_rxenable(0) ;               // turn receiver on, immediate = 0/delayed = 1

                            }
                            break;
                        } //end switch (rxmsg->functionCode)

                        if(dw_event->msgu.frame[0] & 0x20)
    					{
    						//as we only pass the received frame with the ACK request bit set after the ACK has been sent
    						instance_getevent(9); //get and clear the ACK sent event
    					}

                    }
                }
                break ;

                case DWT_SIG_RX_TIMEOUT :
                    //printf("PD_DATA_TIMEOUT") ;
                	instance_getevent(26); //get and clear this event
                	inst_processrxtimeout(inst);
                    message = 0; //clear the message as we have processed the event
                break ;

                case DWT_SIG_TX_AA_DONE: //ignore this event - just process the rx frame that was received before the ACK response
                case 0: //no event - wait in receive...
                {
                    //stay in Rx (fall-through from above state)
                    //if(DWT_SIG_TX_AA_DONE == message) printf("Got SIG_TX_AA_DONE in RX wait - ignore\n");
                    if(inst->done == INST_NOT_DONE_YET) inst->done = INST_DONE_WAIT_FOR_NEXT_EVENT;
                }
                break;

                default :
                {
                    //printf("\nERROR - Unexpected message %d ??\n", message) ;
                    //assert(0) ;                                             // Unexpected Primitive what is going on ?
                }
                break ;

            }
            break ; // end case TA_RX_WAIT_DATA

            default:
                //printf("\nERROR - invalid state %d - what is going on??\n", inst->testAppState) ;
            break;
    } // end switch on testAppState

    return inst->done;
} // end testapprun()

// -------------------------------------------------------------------------------------------------------------------
#if NUM_INST != 1
#error These functions assume one instance only
#endif


/* ==========================================================

Notes:

Previously code handled multiple instances in a single console application

Now have changed it to do a single instance only. With minimal code changes...(i.e. kept [instance] index but it is always 0.

Windows application should call instance_init() once and then in the "main loop" call instance_run().

*/
