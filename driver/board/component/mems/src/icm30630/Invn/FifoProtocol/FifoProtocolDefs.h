/*
* ________________________________________________________________________________________________________
* Copyright (c) 2014-2015 InvenSense Inc. Portions Copyright (c) 2014-2015 Movea. All rights reserved.
*
* This software, related documentation and any modifications thereto (collectively "Software") is subject
* to InvenSense and its licensors' intellectual property rights under U.S. and international copyright and
* other intellectual property rights laws.
*
* InvenSense and its licensors retain all intellectual property and proprietary rights in and to the Software
* and any use, reproduction, disclosure or distribution of the Software without an express license
* agreement from InvenSense is strictly prohibited.
*
* EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, THE SOFTWARE IS
* PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
* TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
* EXCEPT AS OTHERWISE PROVIDED IN A LICENSE AGREEMENT BETWEEN THE PARTIES, IN NO EVENT SHALL
* INVENSENSE BE LIABLE FOR ANY DIRECT, SPECIAL, INDIRECT, INCIDENTAL, OR CONSEQUENTIAL DAMAGES, OR ANY
* DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
* NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
* OF THE SOFTWARE.
* ________________________________________________________________________________________________________
*/

/** @defgroup 	FifoProtocolDefs	 FifoProtocolDefs
 	@brief 		FIFO Protocol definitions
    @ingroup 	FifoProtocolAdapter
    @{
*/

#ifndef _INV_FIFO_PROTOCOL_DEFS_H_
#define _INV_FIFO_PROTOCOL_DEFS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/** @brief Enumeration discribing the fifo type */
typedef enum FifoProtocolType {
	FIFOPROTOCOL_TYPE_COMMAND       = 0,    /**< fifo is the command fifo */
	FIFOPROTOCOL_TYPE_NORMAL        = 1,    /**< fifo is the data fifo for normal sensor */
	FIFOPROTOCOL_TYPE_WAKEUP        = 2,    /**< fifo is the data fifo for wake up sensor */
	FIFOPROTOCOL_TYPE_ANSWER        = 3,    /**< fifo is the answer fifo */
	FIFOPROTOCOL_TYPE_MAX                   /**< sentinel value for fifo type */
} FifoProtocolType;

/** @brief Error code enumeration */
typedef enum FifoProtocolError {
    FIFOPROTOCOL_ERROR_NO           = 0,    /**< success, no error */
    FIFOPROTOCOL_ERROR              = -1,   /**< unspecified error */
    FIFOPROTOCOL_ERROR_NIMPL        = -2,   /**< method is not implemented for provided argument */
    FIFOPROTOCOL_ERROR_FIFOEMPTY    = -3,   /**< fifo is empty */
    FIFOPROTOCOL_ERROR_FIFOFULL     = -4,   /**< fifo is full */
    FIFOPROTOCOL_ERROR_INCOMPLETE   = -5,   /**< not enough data to decode */
    FIFOPROTOCOL_ERROR_SIZE         = -6,   /**< size does not match what is expected or buffer is not big enough */
    FIFOPROTOCOL_ERROR_TYPE         = -7,   /**< unpected fifo type to perform required action */
    FIFOPROTOCOL_ERROR_PARSE        = -8,   /**< cannot parse a valid frame */
} FifoProtocolError;

/** @brief Option when popping/pushing from/to a fifo */
typedef enum FifoProtocolOpt {
	FIFOPROTOCOL_OPT_NONE               = 0,    /**< reserved value to indicate no particular option */
	FIFOPROTOCOL_OPT_DROP_IF_FULL       = 1,    /**< item should be drop if fifo is full */
	FIFOPROTOCOL_OPT_RESET_IF_FULL      = 2,    /**< fifo should be reset if fifo is full */
	FIFOPROTOCOL_OPT_OVERWRITE_IF_FULL  = 4,    /**< overwrite oldest item if possible */
} FifoProtocolOpt;

/** @brief outcomming frame status definition  */
typedef enum FifoProtocolStatus {
	FIFOPROTOCOL_STATUS_DATAUPDATED		= 0,
	FIFOPROTOCOL_STATUS_STATECHANGED	= 1,
	FIFOPROTOCOL_STATUS_FLUSH			= 2,
	FIFOPROTOCOL_STATUS_POLL		    = 3
} FifoProtocolStatus;

/** @brief accuracy flag definition */
typedef enum FifoProtocolAccuracy {
	FIFOPROTOCOL_ACCURACY_NO_CONTACT    = -1,
	FIFOPROTOCOL_ACCURACY_UNRELIABLE	= 0,
	FIFOPROTOCOL_ACCURACY_LOW			= 1,
	FIFOPROTOCOL_ACCURACY_MEDIUM		= 2,
	FIFOPROTOCOL_ACCURACY_HIGH			= 3
} FifoProtocolAccuracy;

/** @brief AP power mode definition */
typedef enum FifoProtocolApPowerMode {
	FIFOPROTOCOL_APPOWERMODE_SUSPEND      = 0,
	FIFOPROTOCOL_APPOWERMODE_ON           = 1,
	FIFOPROTOCOL_APPOWERMODE_IDLE         = 2,
} FifoProtocolApPowerMode;

/** @brief command id definition */
typedef enum FifoProtocolCmd {
	FIFOPROTOCOL_CMD_SENSOROFF					= 0,
	FIFOPROTOCOL_CMD_SENSORON					= 1,
	FIFOPROTOCOL_CMD_SETPOWER					= 2,
	FIFOPROTOCOL_CMD_BATCHON					= 3,
	FIFOPROTOCOL_CMD_FLUSH						= 4,
	FIFOPROTOCOL_CMD_SETDELAY					= 5,
	FIFOPROTOCOL_CMD_SETCALIBRATIONGAINS		= 6,
	FIFOPROTOCOL_CMD_GETCALIBRATIONGAINS		= 7,
	FIFOPROTOCOL_CMD_SETCALIBRATIONOFFSETS		= 8,
	FIFOPROTOCOL_CMD_GETCALIBRATIONOFFSETS		= 9,
	FIFOPROTOCOL_CMD_SETREFERENCEFRAME			= 10,
	FIFOPROTOCOL_CMD_GETFIRMWAREINFO            = 11,
	FIFOPROTOCOL_CMD_GETDATA		            = 12,
	FIFOPROTOCOL_CMD_GETCLOCKRATE	            = 13,
	FIFOPROTOCOL_CMD_PING                       = 14,
	FIFOPROTOCOL_CMD_RESET			            = 15,
	FIFOPROTOCOL_CMD_LOAD			            = 0x10,
	FIFOPROTOCOL_CMD_SETCONFIG      			= 0x11,
	FIFOPROTOCOL_CMD_GETCONFIG     				= 0x12,
	FIFOPROTOCOL_CMD_SETDATAINJECTION           = 0x13,
	FIFOPROTOCOL_CMD_AUDIO						= 0x14,
	FIFOPROTOCOL_CMD_MAX,
} FifoProtocolCmd;

/** @brief WHO identifier for LOAD command */
typedef enum FifoProtocolLoadWho {
	FIFOPROTOCOL_LOAD_WHO_RESERVED					= 0,
	FIFOPROTOCOL_LOAD_WHO_DMP3_FW					= 1,
	FIFOPROTOCOL_LOAD_WHO_DMP4_FW					= 2,
	FIFOPROTOCOL_LOAD_WHO_EXTENDED_BINARY_FW		= 3,
	FIFOPROTOCOL_LOAD_WHO_MAX,
} FifoProtocolLoadWho;

/** @brief WHAT identifier for LOAD command */
typedef enum FifoProtocolLoadWhat {
	FIFOPROTOCOL_LOAD_WHAT_RESERVED					= 0,
	FIFOPROTOCOL_LOAD_WHAT_MALLOC					= 1,
	FIFOPROTOCOL_LOAD_WHAT_FREE						= 2,
	FIFOPROTOCOL_LOAD_WHAT_CHECK					= 3,
	FIFOPROTOCOL_LOAD_WHAT_MAX,
} FifoProtocolLoadWhat;

/** @brief  packet size */
#define FIFOPROTOCOL_PACKET_SIZE    14

/** @brief  header value preceeding any packet, to check packet validity */
#define FIFOPROTOCOL_PROTECTED_HEADER    0xEF
/** @brief  footer value succeeding any packet, to check packet validity */
#define FIFOPROTOCOL_PROTECTED_FOOTER    0xBE

/** @brief  header size */
#define FIFOPROTOCOL_HEADER_SIZE    3
/** @brief  footer size */
#define FIFOPROTOCOL_FOOTER_SIZE    1

/** @brief  timestamp size */
#define FIFOPROTOCOL_TIMESTAMP_SIZE 4

/** @brief  answer mask */
#define FIFOPROTOCOL_ANSWER_MASK    0x40

/** @brief  status mask */
#define FIFOPROTOCOL_STATUS_MASK    0x60
/** @brief  status shift */
#define FIFOPROTOCOL_STATUS_SHIFT   5

/** @brief  accuracy mask */
#define FIFOPROTOCOL_ACCURACY_MASK    0x18
/** @brief  accuracy shift */
#define FIFOPROTOCOL_ACCURACY_SHIFT   3

/** @brief  number mask */
#define FIFOPROTOCOL_NUMBER_MASK    0x07
/** @brief  number shift */
#define FIFOPROTOCOL_NUMBER_SHIFT   0
/** @brief  max value for number */
#define FIFOPROTOCOL_NUMBER_MAX     7

/** @brief  max payload size according to packet size and header size */
#define FIFOPROTOCOL_PAYLOAD_SIZE_MAX \
    ( (FIFOPROTOCOL_NUMBER_MAX - 1)*(FIFOPROTOCOL_PACKET_SIZE - FIFOPROTOCOL_HEADER_SIZE - FIFOPROTOCOL_FOOTER_SIZE) \
     + (FIFOPROTOCOL_PACKET_SIZE - FIFOPROTOCOL_HEADER_SIZE - FIFOPROTOCOL_TIMESTAMP_SIZE - FIFOPROTOCOL_FOOTER_SIZE) )

#define FIFOPROTOCOL_FRAME_MAX_SIZE     FIFOPROTOCOL_PAYLOAD_SIZE_MAX

#ifdef __cplusplus
}
#endif

#endif /* _INV_FIFO_PROTOCOL_DEFS_H_ */

/** @} */

