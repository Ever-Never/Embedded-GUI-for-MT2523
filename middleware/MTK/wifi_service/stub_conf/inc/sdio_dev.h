#ifndef __LTE_DEV_H__

#define __LTE_DEV_H__

#include <stdint.h>
#include <stdbool.h>

/*************************************************************
*
*				SDIO  DEVICE  PART
*
*************************************************************/

/**
 * SDIO Function 0 control registers
 */

#define SDIO_FN0_CCCR_CSRR       0x0000

#define SDIO_FN0_CCCR_SDSRR      0x0001

#define SDIO_FN0_CCCR_IOER       0x0002

#define SDIO_FN0_CCCR_IORR       0x0003

#define SDIO_FN0_CCCR_IER        0x0004

#define SDIO_FN0_CCCR_IPR        0x0005

#define SDIO_FN0_CCCR_IOAR       0x0006

#define SDIO_FN0_CCCR_BICR       0x0007

#define SDIO_FN0_CCCR_CCR        0x0008

#define SDIO_FN0_CCCR_CCPR       0x0009  // 0x0009 - 0x000B

#define SDIO_FN0_CCCR_BSR        0x000C	 //Suspend

#define SDIO_FN0_CCCR_FSR        0x000D	 //Resume

#define SDIO_FN0_CCCR_EFR        0x000E

#define SDIO_FN0_CCCR_RFR        0x000F

#define SDIO_FN0_CCCR_F0BSR      0x0010	// 0x0010 - 0x0011

#define SDIO_FN0_CCCR_PCR        0x0012

#define SDIO_FN0_CCCR_HSR        0x0013

#define SDIO_FN0_CCCR_UHSR       0x0014

#define SDIO_FN0_CCCR_DSR        0x0015

#define SDIO_FN0_CCCR_IEXTR      0x0016

#define SDIO_FN0_CCCR_IOBSF1R    0x0110

#define SDIO_FN0_CCCR_CIS0       0x1000

#define SDIO_FN0_CCCR_CIS1       0x2000

#define SDIO_CIS0_FW_ADDR        0xBF23C000

#define SDIO_CIS1_FW_ADDR        0xBF23C400

/* SDIO Function 1 control registers */

#define SDIO_FN1_FBR_CSAR        0x0100

#define SDIO_FN1_FBR_EXTSCR      0x0101

#define SDIO_FN1_FBR_PSR         0x0102

#define SDIO_FN1_FBR_CIS         0x0109 // 0x109 ~ 0x10B

#define SDIO_FN1_FBR_F1BSR       0x0110 // 0x110 ~ 0x111

/*************************************************************

*				SDIO  DEVICE  PART

*************************************************************/

typedef enum _SDIO_THREAD_STATE {
    SDIO_THREAD_IDLE = 0x0 ,
    SDIO_THREAD_RUNNING = 0x1 ,
} SDIO_THREAD_STATE ;


struct mtlte_dev
{
    /******************* SDIO/HAL related structure member ******************/
    struct sdio_func *sdio_func;	// the SDIO Device Function Core
    unsigned card_exist ;
    volatile int sdio_thread_kick;
    volatile SDIO_THREAD_STATE sdio_thread_state; //probe: running; remove: idle
};


/*************************************************************
*				TEST  DEVICE  PART
*************************************************************/

#define H2D_INT_H2DMB_init_req   (0x1<<31)
#define H2D_INT_H2DMB_data_sent  (0x1<<30)
#define H2D_INT_D2HMB_init_ack   (0x1<<29)
#define H2D_INT_D2HMB_data_ack   (0x1<<28)
#define D2H_INT_H2DMB_init_ack  (0x1<<31)
#define D2H_INT_H2DMB_data_ack  (0x1<<30)
#define D2H_INT_D2HMB_init_req  (0x1<<29)
#define D2H_INT_D2HMB_data_sent (0x1<<28)


typedef struct _at_msg_status
{
    bool D2H_INT_H2DMB_init_ack_st;	//a flag mtlte_dev_test_sent_atcmd will wait for it come ture (when Recieved the D2H_INT_H2DMB_init_ack will set ture)
    bool D2H_INT_H2DMB_data_ack_st;
    bool D2H_NEW_MSD_receiving_st;	// ture: a new msg is receiving;   false: receive over
    bool D2H_NEW_MSG_arrived;		// ture: a new msg is received and should be process
} at_msg_status;



typedef enum _athif_cmd_code
{
    /*!
     *  @brief set the hif at dataflow task reload RGPD mode
     */
    ATHIF_CMD_SET_RGPD_RL_MD = 0,

    /*!
     *  @brief : set the at dataflow task forward mode
     */
    ATHIF_CMD_SET_FWD_MD, // 1

    /*!
     *  @brief set the at dataflow task loopback mode type (direct / random / ..., etc.)
     */
    ATHIF_CMD_SET_LB_TYPE, // 2

    /*!
     *  @brief get the received or sent packet count from the data flow driver
     */
    ATHIF_CMD_GET_PKT_CNT, // 3

    /*!
     *  @brief Test the DL only performance and the performance calculated on host
     */
    ATHIF_CMD_DL_PERF,	    /*4*/ // 4

    /*!
     *  @brief host send this cmd with pattern in param buffer and device send back in the CMD_ACK
     */

//20150326	ATHIF_CMD_CTRL_CH_TST,

    /*!

     *  @brief device would send the specific GPD format packet to host on specific queue

     */

    ATHIF_CMD_DL_SEND_N,	// 5

    /*!

     *  @brief device would send N random GPD format packet to host on specific queue

     */

    ATHIF_CMD_DL_SEND_RAND_N, // 6

    /*!

     *  @brief device would send random GPD format packet stress for specific loop

     */

    ATHIF_CMD_DL_SEND_RAND_STRESS,   /*8*/ // 7

    /*!

     *  @brief device would shut the HIF port and then re-attach

     */

//20150326	ATHIF_CMD_HIF_RECONNECT,

    /*!

     *  @brief pause or resume dataflow RGPD reload handle

     */

    ATHIF_CMD_PAUSE_RGPD_RL, // 8

    /*!

     *  @brief prepare RGPD with specific format and enqueue for unit test

     */

    ATHIF_CMD_PREPARE_RGPD, // 9

    /*!

     *  @brief configure the RGPD format for reload buffer thread

     */

//20150326	ATHIF_CMD_SET_RGPD_FROMAT,       /*12*/

    /*!

     *  @brief configure the TGPD format for loopback

     */

//20150326	ATHIF_CMD_SET_TGPD_FROMAT,

    /*!

     *  @brief write specific memory

     */

    ATHIF_CMD_WRITE_MEM, // 10

    /*!

     *  @brief read specific memory

     */

    ATHIF_CMD_READ_MEM,	// 11

    /*!

     *  @brief get current qmu interrupt handle recorded info

     */

//20150326	ATHIF_CMD_GET_QMU_INTR_INFO,          /*16 */

    /*!

     *  @brief clear current qmu interrupt handle recorded info

     */

//20150326	ATHIF_CMD_CLR_QMU_INTR_INFO,

    /*!

     *  @brief prepare n RGPD and specifiy which RGPD IOC assert

     */

//20150326	ATHIF_CMD_PREPARE_IOC_RGPD,

    /*!

     *  @brief prepare n TGPD and specify which TGPD IOC assert

     */

//20150326	ATHIF_CMD_PREPARE_IOC_TGPD,

    /*!

     *  @brief configure new checksum length

     */

//20150326	ATHIF_CMD_SET_CS_LEN,                  /*20*/

    /*!

     *  @brief pause or resume the tx and rx dataflow reload queue dequeue flow

     */

    ATHIF_CMD_PAUSE_RESUME_DATAFLOW, // 12

    /*!

     *  @brief prepare RGPD and specifiy which RGPD CS setting

     */

    ATHIF_CMD_PREPARE_CS_TST_RGPD, // 13

    /*!

     *  @brief compare and free the local RGPD, and send the compare/free result

     */

    ATHIF_CMD_GET_LOCAL_RGPD_RSLT, // 14

    /*!

     *  @brief prepare TGPD and specifiy which TGPD CS setting

     */

    ATHIF_CMD_PREPARE_CS_TST_TGPD,         /*24*/ // 15

    /*!

     *  @brief check and free the local TGPD

     */

    ATHIF_CMD_GET_LOCAL_TGPD_RSLT, // 16

    /*!

     *  @brief set GPD buffer offset

     */

    ATHIF_CMD_SET_BUF_OFFSET, // 17

    /*!

     *  @brief set configure USB EP0 DMA or PIO mode

     */

//20150326	ATHIF_CMD_USB_SET_EP0_DMA,

    /*!

     *  @brief set timing to stop queue

     */

    ATHIF_CMD_SET_STOPQ_TIME,              /*28*/ // 18

    /*!

     *  @brief configure the dataflow reload specific rgpd type

     */

    ATHIF_CMD_CFG_SPECIFIC_RGPD_RL, // 19

    /*!

     *  @brief host request to prepare MSD RGPD with specific MSD xfer length

     */

//20150326	ATHIF_CMD_PREPARE_MSD_RGPD,

    /*!

     *  @brief host request to change the loopback queue mapping

     */

//20150326	ATHIF_CMD_SET_LB_QUE_MAP_TBL,

    /*!

     *  @brief get current test HW IP information for the difference of each IP

     */

//20150326  ATHIF_CMD_GET_USB_IP_INFO,               /*32*/

    /*!

     *  @brief set the USB IP max transfer speed

     */

//20150326   ATHIF_CMD_SET_USB_SPEED,

    /*

     * 	@brief	reconfigure the USB endpoint FIFO,TYPE...

    */

//20150326	ATHIF_CMD_RECONFIG_USB_EP,

    /*

     * 	@brief	emulate the exception handle to test hif exception handle driver

    */

//20150326	ATHIF_CMD_EXCEPTION_DRV_TST,

    /*

     * 	@brief	emulate the exception handle swithc USB channel

    */

//20150326    ATHIF_CMD_EXCEPTION_SWITCH_USB_CH_TST,             /*36*/

    /*

     * 	@brief	get the USB event counter information from usb_at.c

    */

//20150326    ATHIF_CMD_GET_USB_EVT_CNT,

    /*

     * 	@brief	Waiting expected delay time to issue remote wakeup after suspend event

     *			This is a non-ack command

    */

//20150326    ATHIF_CMD_USB_REMOTE_WK_TST,



    /*

     * 	@brief	Used to test the USB3.0 device notify TP

    */

//20150326   ATHIF_CMD_U3_DEV_NOTIFY,





    /* **************************** */

    /*         Below is SDIO part               */

    /* **************************** */

    /*!

     *  @brief  start the auto test cmd loopback test of sdio

     */

    SDIO_AT_CMD_LOOPBACK,                  /*40*/ // 20



    /*!

     *  @brief  start the mailbox loopback test of sdio

     */

    SDIO_AT_MB_LOOPBACK, // 21



    SDIO_AT_TEST_MODE, // 22



    /*!

     *  @brief  start the auto test sw interrupt test of sdio

     */

    SDIO_AT_SW_INT, // 23



    /*!

      *  @brief  start the auto test of D2H normal operation releated interrupt

      */

    SDIO_AT_D2H_NORMAL_OP,  // 24             /*44*/



    /*!

      *  @brief  start the auto test of device self normal opearation interrupt

      */

    SDIO_AT_SELF_NORMAL_INT, // 25



    /*!

      *  @brief  start the auto test of device self normal opearation interrupt

      */

    SDIO_AT_NORMAL_DLQ_OP, // 26



    /*!

      *  @brief  send spaciel DL GPD for test

      */

    SDIO_AT_DL_SEND_SP, // 27



    /*!

     *  @brief  start, end big size packet test

     */

    SDIO_AT_BIG_SIZE,  // 28              /*48*/



    /*!

     *  @brief  UL big size packet (up to 4K) test

     */

    SDIO_AT_UL_BIG_SIZE, // 29



    /*!

      *  @brief  UL big size packet (up to 64K) test

      */

    SDIO_AT_DL_BIG_SIZE,  // 30



    /*!

      *  @brief  set whether ISR is work at normal mode or test mode.

      */

    SDIO_AT_DL_INT_TEST_SWITCH, // 31

    /*!

     *  @brief  Reset & flush the DL queue for error handle test.

     */

    SDIO_AT_RESET_DL_QUEUE,  // 32               /*52*/



    /*!

     *  @brief  Reset & flush the UL queue for error handle test.

     */

    SDIO_AT_RESET_UL_QUEUE, // 33



    /*!

    *  @brief  read the SDIO interrupt status.

    */

    SDIO_AT_READ_INT_STATUS, // 34



    /*!

    *  @brief  set the SDIO interrupt mask value.

    */

    SDIO_AT_SET_INT_MASK, // 35



    /*!

     *  @brief  read the SDIO interrupt mask value.

     */

    SDIO_AT_READ_INT_MASK,  // 36                /*56*/



    /*!

     *  @brief  read the SDIO Tx data compare result.

     */

    SDIO_AT_READ_TX_COMP_RET, // 37



    SDIO_AT_PREPARE_RGPD, // 38



    ////////////////////////////////////////////////////////////////////////

    ////

    ////    For SDIO PIO-Based Function Test

    ////

    ///////////////////////////////////////////////////////////////////////

    SDIO_PIO_CMD_RST_FIFO_ETC,          // 39

    SDIO_PIO_CMD_SET_DAT_PATTERN,       // 40  /*60*/

    SDIO_PIO_CMD_SET_FWD_MOD,

    SDIO_PIO_CMD_SET_INT_CLR_CTL,

    SDIO_PIO_CMD_SET_AUTO_RCV_TX_DAT,

    SDIO_PIO_CMD_PRPARE_RX_PKT,         /* 64*/

    SDIO_PIO_CMD_FIFO_TX_EMPTY_INT_TST,

    SDIO_PIO_CMD_FIFO_TX_UNDERTHLD_INT_TST,

    SDIO_PIO_CMD_FIFO_RX_RDY_INT_TST,

    SDIO_PIO_CMD_SET_TX_THOLD,

    SDIO_PIO_CMD_SET_TX_TIMEOUT_CNT,

} athif_cmd_code_e;





typedef struct _athif_init_cmd

{

    char signature[2];		// the signature



    short length;			// the command set length include the header and payload



} athif_init_cmd_t;





typedef struct _athif_cmd

{

    char 	signature[2];	// the signature



    short 	len;			// the command set length include the header and payload



    short 	cmd;			// the command code



    char 	buf[1024];		// the command parameter

} athif_cmd_t;





typedef struct _athif_status

{

    char 	signature[2];	// the signature



    short 	len;			// the status length include the header and payload



    short 	status;			// the status code



    char 	buf[1024];		// the status detail statement

} athif_status_t;



#endif


