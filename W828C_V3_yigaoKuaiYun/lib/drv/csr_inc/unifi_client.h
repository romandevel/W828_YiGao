/*
 *****************************************************************************
 *
 * FILE : unifi_client.h
 * 
 * PURPOSE : 
 *
 * Copyright (C) 2008 by Cambridge Silicon Radio Ltd.
 ***************************************************************************** 
 */

#ifndef __UNIFI_CLIENT_H__
#define __UNIFI_CLIENT_H__


/*
 * Structure describing a bulk data slot.
 * The length field is used to indicate empty/occupied state.
 */
typedef struct _bulk_data
{
    unsigned char ptr[2000];
    unsigned int length;
} bulk_data_t;

typedef struct unifi_client 
{
    // value to use in signal->SenderProcessId.
    CsrUint32	sender_id;

    // a 0x00 - 0x0F mask to apply in signal->SenderProcessId.
    // every time we do a blocking mlme request we increase this value.
    CsrUint32	seq_no;

    CSR_SIGNAL* reply_signal;
//    bulk_data_t* reply_bulkdata[UNIFI_MAX_DATA_REFERENCES];
    
    /* Flag to wake up blocking clients waiting on udi_wq. */
    int wake_up_wq_id;

    /*
     * A 0x00 - 0x0F counter, containing the sequence number of
     * the signal that this client has last received.
     * Only the MLME blocking functions update this field.
     */
    unsigned char wake_seq_no;
    
    int power_mode;
    /* Whether to wake for broadcast packets (using DTIM interval) */
    int wakeup_for_dtims;
} unifi_client_t;


#endif /* __UNIFI_CLIENT_H__ */
