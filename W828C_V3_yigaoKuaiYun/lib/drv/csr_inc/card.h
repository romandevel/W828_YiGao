/*
 ******************************************************************************
 * FILE : card.h
 *
 * PURPOSE : Defines abstract interface for hardware specific functions.
 *           Note, this is a different file from one of the same name in the
 *           Windows driver.
 *
 * Copyright (C) 2005-2009 by Cambridge Silicon Radio Ltd.
 *
 * Refer to LICENSE.txt included with this source code for details on
 * the license terms.
 *****************************************************************************
 */
#ifndef __CARD_H__
#define __CARD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "card_sdio.h"
#include "signals.h"


/*****************************************************************************
 * CardEnableInt -
 */
CsrInt32 CardEnableInt(card_t *card);

/*****************************************************************************
 * CardGenInt -
 */
CsrInt32 CardGenInt(card_t *card);

/*****************************************************************************
 * CardPendingInt -
 */
CsrInt32 CardPendingInt(card_t *card);

/*****************************************************************************
 * CardDisableInt -
 */
CsrInt32 CardDisableInt(card_t *card);

/*****************************************************************************
 * CardClearInt -
 */
CsrInt32 CardClearInt(card_t *card);

/*****************************************************************************
 * CardDisable -
 */
void CardDisable(card_t *card);

/*****************************************************************************
 * CardIntEnabled -
 */
CsrInt32 CardIntEnabled(card_t *card);

/*****************************************************************************
 * CardGetDataSlotSize
 */
CsrUint16 CardGetDataSlotSize(card_t *card);

/*****************************************************************************
 * CardWriteBulkData -
 */
CsrInt32 CardWriteBulkData(card_t *card, bulk_data_desc_t* bulkdata);

/*****************************************************************************
 * CardClearFromHostDataSlot -
 */
void CardClearFromHostDataSlot(card_t *card, const CsrInt16 aSlotNum);

/*****************************************************************************
 * CardGetFromHostDataSlot -
 *   returns -1 if no slots available
 */
CsrInt32 CardGetFromHostDataSlot(card_t *card, CsrUint32 size);

/*****************************************************************************
 * CardGetFreeFromHostDataSlots -
 */
CsrUint16 CardGetFreeFromHostDataSlots(card_t *card);


CsrInt32 card_stop_processor(card_t *card, CsrInt16 which);
CsrInt32 card_start_processor(card_t *card, CsrInt16 which);

CsrInt32 card_wait_for_firmware_to_start(card_t *card, CsrUint32 *paddr);

CsrInt32 unifi_dl_firmware(card_t *card, void *arg, CsrInt8 secondary);
CsrInt32 unifi_dl_patch(card_t *card, void *arg, CsrUint32 boot_ctrl);
CsrInt32 unifi_do_loader_op(card_t *card, CsrUint32 op_addr, CsrUint8 opcode);
void* unifi_dl_fw_read_start(card_t *card, CsrInt8 is_fw);

#ifdef __cplusplus
}
#endif

#endif /* __CARD_H__ */
