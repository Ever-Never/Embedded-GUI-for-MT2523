/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */
 
/*
 * |key: |module_id|key_id|
 * |bytes|15  ~  10|9 ~ 0 |
 * modules: id
 *     hal: 0x1<<10
 *      BT: 0x2<<10
 * UI_KEYS: 0x3<<10
 * LED_STATE: 0x4<<10
 */
//#define CFW_ITEM(key, name, type, value)
//#define CFW_ARRAY_ITEM(key, name, type, size, value)
//CFW_ITEM(0x0801, bt_item_1, int8_t, -5)
//CFW_ITEM(0x0802, bt_item_2, uint8_t, 240)
//CFW_ITEM(0x0803, bt_item_3, int16_t, 15)
//CFW_ARRAY_ITEM(0x0804, bt_name, char, 10, {"bt_device"})
//CFW_ITEM(0x0401, hal_item_1, uint16_t, 15)
//CFW_ITEM(0x0402, hal_item_2, int32_t, -5)
//CFW_ITEM(0x0403, hal_item_3, uint32_t, 15)
//CFW_ARRAY_ITEM(0x0404, hal_item_4, uint8_t, 20, {1,2,3,4,5,6,7,8,1,2,3,4,5,6,7,8,1,2,3,4})
CFW_ITEM(0x0805, bt_device_role, uint8_t, 0x00)

CFW_ARRAY_ITEM(0x0C00, ui_action_key_00, uint8_t, 8, {0x01, 0x02, 0x10, 0x00, 0x00, 0x02, 0x00, 0x40}) /*Answer incoming*/
CFW_ARRAY_ITEM(0x0C01, ui_action_key_01, uint8_t, 8, {0x01, 0x09, 0x10, 0x00, 0x01, 0x02, 0x00, 0x40}) /*Reject incoming*/
CFW_ARRAY_ITEM(0x0C02, ui_action_key_02, uint8_t, 8, {0x01, 0x02, 0x60, 0x0A, 0x02, 0x02, 0x00, 0x40}) /*Hang up active&outgoing*/ 
CFW_ARRAY_ITEM(0x0C03, ui_action_key_03, uint8_t, 8, {0x01, 0x04, 0x40, 0x00, 0x0A, 0x02, 0x00, 0x40}) /*Held acitve*/
CFW_ARRAY_ITEM(0x0C04, ui_action_key_04, uint8_t, 8, {0x01, 0x04, 0x00, 0x04, 0x0A, 0x02, 0x00, 0x40}) /*Retrieve held*/
CFW_ARRAY_ITEM(0x0C05, ui_action_key_05, uint8_t, 8, {0x01, 0x04, 0x00, 0x02, 0x0A, 0x02, 0x00, 0x40}) /*Swap active and held*/
CFW_ARRAY_ITEM(0x0C06, ui_action_key_06, uint8_t, 8, {0x01, 0x02, 0x80, 0x00, 0x0A, 0x02, 0x00, 0x40}) /*Held and accept*/
CFW_ARRAY_ITEM(0x0C07, ui_action_key_07, uint8_t, 8, {0x01, 0x04, 0x80, 0x00, 0x07, 0x02, 0x00, 0x40}) /*Reject waiting*/
CFW_ARRAY_ITEM(0x0C08, ui_action_key_08, uint8_t, 8, {0x01, 0x06, 0x04, 0x00, 0x03, 0x02, 0x00, 0x40}) /*Redial the last dialed num*/
CFW_ARRAY_ITEM(0x0C09, ui_action_key_09, uint8_t, 8, {0x01, 0x09, 0x40, 0x02, 0x06, 0x02, 0x00, 0x40}) /*Switch audio path*/
CFW_ARRAY_ITEM(0x0C0A, ui_action_key_0A, uint8_t, 8, {0x02, 0x02, 0x40, 0x02, 0x00, 0x06, 0x00, 0x40}) /*Remote call control-volume up*/ 
CFW_ARRAY_ITEM(0x0C0B, ui_action_key_0B, uint8_t, 8, {0x03, 0x02, 0x40, 0x02, 0x01, 0x06, 0x00, 0x40}) /*Remote call control-volume down*/ 
CFW_ARRAY_ITEM(0x0C0C, ui_action_key_0C, uint8_t, 8, {0x03, 0x04, 0x40, 0x02, 0x03, 0x06, 0x00, 0x40}) /*Remote call control-volume min*/ 
CFW_ARRAY_ITEM(0x0C0D, ui_action_key_0D, uint8_t, 8, {0x02, 0x04, 0x40, 0x02, 0x02, 0x06, 0x00, 0x40}) /*Remote call control-volume max*/ 
CFW_ARRAY_ITEM(0x0C0E, ui_action_key_0E, uint8_t, 8, {0x02, 0x02, 0x04, 0x10, 0x00, 0x06, 0x00, 0x40}) /*Music control- volume up*/ 
CFW_ARRAY_ITEM(0x0C0F, ui_action_key_0F, uint8_t, 8, {0x03, 0x02, 0x04, 0x10, 0x01, 0x06, 0x00, 0x40}) /*Music control- volume down*/ 
//CFW_ARRAY_ITEM(0x0C10, ui_action_key_10, uint8_t, 8, {0x01, 0x0A, 0x05, 0x00, 0x04, 0x04, 0x00, 0x40}) /*Switch mode*/ 
CFW_ARRAY_ITEM(0x0C11, ui_action_key_11, uint8_t, 8, {0x01, 0x02, 0x00, 0x10, 0x01, 0x04, 0x00, 0x40}) /*Music control- pause*/
CFW_ARRAY_ITEM(0x0C12, ui_action_key_12, uint8_t, 8, {0x01, 0x02, 0x04, 0x00, 0x00, 0x04, 0x00, 0x40}) /*Music control- play*/
CFW_ARRAY_ITEM(0x0C13, ui_action_key_13, uint8_t, 8, {0x02, 0x04, 0x04, 0x10, 0x02, 0x04, 0x00, 0x40}) /*Music control- next*/
CFW_ARRAY_ITEM(0x0C14, ui_action_key_14, uint8_t, 8, {0x03, 0x04, 0x04, 0x10, 0x03, 0x04, 0x00, 0x40}) /*Music control- prev*/
//CFW_ARRAY_ITEM(0x0C15, ui_action_key_15, uint8_t, 8, {0x01, 0x09, 0x01, 0x00, 0x01, 0x01, 0x00, 0x40}) /*Connect the last connected device*/
CFW_ARRAY_ITEM(0x0C16, ui_action_key_16, uint8_t, 8, {0x01, 0x04, 0x01, 0x00, 0x00, 0x01, 0x00, 0x40}) /*Discoverable*/
CFW_ARRAY_ITEM(0x0C17, ui_action_key_17, uint8_t, 8, {0x01, 0x08, 0x01, 0x00, 0x04, 0x06, 0x00, 0x40}) /*Power off*/
CFW_ARRAY_ITEM(0x0C18, ui_action_key_18, uint8_t, 8, {0x01, 0x09, 0x01, 0x00, 0x04, 0x00, 0x00, 0x40}) /* Clean paired list*/
CFW_ARRAY_ITEM(0x0C19, ui_action_key_19, uint8_t, 8, {0x01, 0x02, 0x01, 0x00, 0x03, 0x01, 0x00, 0x40}) /*Inquiry*/
CFW_ARRAY_ITEM(0x0C1A, ui_action_key_1A, uint8_t, 8, {0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x00, 0x40}) /*The end line*/

CFW_ARRAY_ITEM(0x1001, led_state_01, uint8_t, 11, {0x01, 0x02, 0xF4, 0x01, 0xF4, 0x01, 0x03, 0x00, 0x00, 0x01, 0x01})
CFW_ARRAY_ITEM(0x1002, led_state_02, uint8_t, 11, {0x02, 0x01, 0xF4, 0x01, 0xF4, 0x01, 0x03, 0x00, 0x00, 0x01, 0x01})
CFW_ARRAY_ITEM(0x1003, led_state_03, uint8_t, 11, {0x03, 0x01, 0xF4, 0x01, 0xA0, 0x0F, 0xFF, 0x00, 0x00, 0x01, 0x01})
CFW_ARRAY_ITEM(0x1004, led_state_04, uint8_t, 11, {0x04, 0x02, 0xFF, 0xFF, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x01})
CFW_ARRAY_ITEM(0x1005, led_state_05, uint8_t, 11, {0x05, 0x02, 0x32, 0x00, 0x32, 0x00, 0x05, 0x00, 0x00, 0x01, 0x01})
CFW_ARRAY_ITEM(0x1006, led_state_06, uint8_t, 11, {0x06, 0x02, 0x32, 0x00, 0x32, 0x00, 0x03, 0xB8, 0x0B, 0xFF, 0x01})
CFW_ARRAY_ITEM(0x1007, led_state_07, uint8_t, 11, {0x07, 0x02, 0xF4, 0x01, 0xD0, 0x07, 0xFF, 0x00, 0x00, 0x01, 0x01})
CFW_ARRAY_ITEM(0x1008, led_state_08, uint8_t, 11, {0x08, 0x02, 0x32, 0x00, 0x32, 0x00, 0x02, 0xD0, 0x07, 0xFF, 0x01})
CFW_ARRAY_ITEM(0x1009, led_state_09, uint8_t, 11, {0x09, 0x02, 0x32, 0x00, 0x32, 0x00, 0x78, 0x00, 0x00, 0x01, 0x01})
CFW_ARRAY_ITEM(0x100A, led_state_0A, uint8_t, 11, {0x0A, 0x01, 0xFF, 0xFF, 0x00, 0x00, 0x01, 0x00, 0x00, 0x01, 0x01})
CFW_ARRAY_ITEM(0x100B, led_state_0B, uint8_t, 11, {0x0B, 0x01, 0x00, 0x00, 0xFF, 0xFF, 0x01, 0x00, 0x00, 0x01, 0x01})
CFW_ARRAY_ITEM(0x100C, led_state_0C, uint8_t, 11, {0x0C, 0x01, 0x32, 0x00, 0x32, 0x00, 0x03, 0x98, 0x3A, 0xFF, 0x01})
CFW_ARRAY_ITEM(0x100D, led_state_0D, uint8_t, 11, {0x0D, 0x02, 0x00, 0x00, 0xFF, 0xFF, 0x01, 0x00, 0x00, 0x01, 0x01})
CFW_ARRAY_ITEM(0x100E, led_state_0E, uint8_t, 11, {0x0E, 0x01, 0xF4, 0x01, 0xD0, 0x07, 0xFF, 0x00, 0x00, 0x01, 0x01})

#ifdef SCT_KEY_EVENT_ENABLE
CFW_ITEM(0x0410, cfw_sct_key_short_time, uint32_t, SCT_KEY_DEFAULT_SHORT_TIME)
CFW_ITEM(0x0411, cfw_sct_key_longpress_time_1, uint32_t, SCT_KEY_DEFAULT_LONGPRESS_TIME_1)
CFW_ITEM(0x0412, cfw_sct_key_longpress_time_2, uint32_t, SCT_KEY_DEFAULT_LONGPRESS_TIME_2)
CFW_ITEM(0x0413, cfw_sct_key_longpress_time_3, uint32_t, SCT_KEY_DEFAULT_LONGPRESS_TIME_3)
CFW_ITEM(0x0414, cfw_sct_key_repeat_time, uint32_t, SCT_KEY_DEFAULT_REPEAT_TIME)
CFW_ITEM(0x0415, cfw_sct_key_double_time, uint32_t, SCT_KEY_DEFAULT_DOUBLE_INTERVAL_TIME)
CFW_ITEM(0x0416, cfw_sct_key_triple_time, uint32_t, SCT_KEY_DEFAULT_TRIPLE_INTERVAL_TIME)
CFW_ITEM(0x0417, cfw_sct_key_debounce_time, uint32_t, SCT_KEY_DEFAULT_DEBOUNCE_TIME)
CFW_ITEM(0x0418, cfw_sct_key_driven_time, uint32_t, SCT_KEY_EVENT_DRIVEN_SIGNAL_TIME)
#endif
    

