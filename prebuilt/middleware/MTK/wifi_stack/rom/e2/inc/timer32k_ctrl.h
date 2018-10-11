/*
 ***************************************************************************
 * Ralink Tech Inc.
 * 4F, No. 2 Technology	5th	Rd.
 * Science-based Industrial	Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2004, Ralink Technology, Inc.
 *
 * All rights reserved.	Ralink's source	code is	an unpublished work	and	the
 * use of a	copyright notice does not imply	otherwise. This	source code
 * contains	confidential trade secret material of Ralink Tech. Any attemp
 * or participation	in deciphering,	decoding, reverse engineering or in	any
 * way altering	the	source code	is stricitly prohibited, unless	the	prior
 * written consent of Ralink Technology, Inc. is obtained.
 ***************************************************************************

	Module Name:
	timer32k_ctrl.h

	Abstract:

	Revision History:
	Who			When		  What
	--------	----------	  ----------------------------------------------
*/

#ifndef	__TIMER32K_CTRL_H__
#define	__TIMER32K_CTRL_H__
#ifdef BIG_ENDIAN
#undef BIG_ENDIAN
#endif
#ifdef TIMER32K_PS
/*
 ===========================================================
 *  struct definitions for interrup control 
 *	(section 3.6 in 3680 datasheet)
 *
 ===========================================================
 */

#define TMR_BIT_MASK_ON		1
#define TMR_BIT_MASK_OFF	0


#define TMP_CPU_GO_UP			1
#define TMP_CPU_GO_DOWN			2

/* IRQ0 and IRQ1 interrupt status register bit mask definitions */
#define SYSCTRL_MASK  			0x00000001
#define TIMER0_MASK				0x00000002
#define TIMER1_MASK				0x00000004
#define ILL_ACC_MASK			0x00000008
#define PIO_MASK				0x00000010
#define UARTLITE				0x00001000	
#define USB_HOST_MASK			0x00040000
#define USB_DEVICE_MASK			0x00080000
#define TIMER_32K_MASK			0x00100000

#define INTENA_GLOBAL_MASK		0x80000000


/* TMR_32K_INT and TMR_32K_INT_ENA */
#define WDTMR_ALM		0x00000010
#define TMR1_ALM1		0x00000008
#define TMR1_ALM0		0x00000004
#define TMR0_ALM1		0x00000002
#define TMR0_ALM0		0x00000001

/* define bit mask for TMR0_32K_CFG TMR1_32K_CFG */
#define TMR_SCALE_32KHZ		0
#define TMR_SCALE_16KHZ		1
#define TMR_SCALE_8KHZ		2
#define TMR_SCALE_4KHZ		3

#define TMR_MODE_FREE_RUN	0
#define TMR_MODE_PERIODIC	1
#define TMR_MODE_TIME_OUT	2

#define TMR_EN_ENABLE		1
#define TMR_EN_DISABLE		0

#define TMR_NALM_EN_ENABLE  1
#define TMR_NALM_EN_DISABLE 0

#define CPU_CLK_SEL_32KHZ		2
#define CPU_CLK_SEL_REF			1
#define CPU_CLK_SEL_160MHZ		0
#if 0
#define INTCTL_REG_BASE   (0x10000200) 

#define IRQ0STAT 	 		(INTCTL_REG_BASE + 0x0000)		/* Type 0 Status after Enable Mask */
#define IRQ1STAT 	 		(INTCTL_REG_BASE + 0x0004)		/* Type 1 Status after Enable Mask */
#define INTTYPE		 		(INTCTL_REG_BASE + 0x0020)		/* Interrupt Type */
#define INTRAW		 		(INTCTL_REG_BASE + 0x0030)		/* Raw Interrupt before Enable Mask */
#define INTENA		 		(INTCTL_REG_BASE + 0x0034)   	/* Interrup Enable */
#define INTDIS		 		(INTCTL_REG_BASE + 0x0038)   	/* Interrup Disable */ 

 
#define TIMER_REG_BASE    (0x10000100)

#define TMRSTAT				(TIMER_REG_BASE + 0x0000)		/* 40MHz Timer Status Register */

#define TMR0LOAD			(TIMER_REG_BASE + 0x0010)		/* 40MHz Timer 0 Load Value */
#define TMR0VAL				(TIMER_REG_BASE + 0x0014)		/* 40MHz Timer 0 Counter Value */
#define TMR0CTL				(TIMER_REG_BASE + 0x0018)		/* 40MHz Timer 0 Control */  

#define TMR1LOAD			(TIMER_REG_BASE + 0x0020)		/* 40MHz Timer 1 Load Value */
#define TMR1VAL				(TIMER_REG_BASE + 0x0024)		/* 40MHz Timer 1 Counter Value */
#define TMR1CTL				(TIMER_REG_BASE + 0x0028)		/* 40MHz Timer 1 Control */

#define TMR_32K_CTRL		(TIMER_REG_BASE + 0x0040)		/* 32KHz Timer Control */  
#define TMR_32K_INT			(TIMER_REG_BASE + 0x0044)		/* 32KHz Timer Interrupt Status */
#define TMR_32K_INT_ENA		(TIMER_REG_BASE + 0x0048)		/* 32KHz Timer Interrupt Enable */
 
#define WTMR_32K_LD			(TIMER_REG_BASE + 0x004c)	 	/* 32KHz Watch Dog Timer Load Value */
#define WTMR_32K_VALUE		(TIMER_REG_BASE + 0x0050) 		/* 32KHz Watch Dog Timer Current Value */
#define WTMR_32K_CFG 		(TIMER_REG_BASE + 0x0054) 		/* 32KHz Watch Dog Timer Configuration */

#define TMR0_32K_LD			(TIMER_REG_BASE + 0x0074)	 	/* 32KHz Timer0 Load Value */
#define TMR0_32K_VALUE		(TIMER_REG_BASE + 0x0078) 		/* 32KHz Timer0 Current Value */
#define TMR0_32K_CFG 		(TIMER_REG_BASE + 0x007c)  		/* 32KHz Timer0 Configuration */
#define TMR0_32K_ALM0 	 	(TIMER_REG_BASE + 0x0080)		/* 32KHz Timer0 Alarm0 Value */
#define TMR0_32K_OFFSET0 	(TIMER_REG_BASE + 0x0084)		/* 32KHz Timer0 Offset Value for Alarm0 */
#define TMR0_32K_ALM1 	 	(TIMER_REG_BASE + 0x0088)		/* 32KHz Timer0 Alarm1 Value */
#define TMR0_32K_OFFSET1 	(TIMER_REG_BASE + 0x008c)		/* 32KHz Timer0 Offset Value for Alarm1 */


#define TMR1_32K_LD			(TIMER_REG_BASE + 0x0090)	 	/* 32KHz Timer1 Load Value */
#define TMR1_32K_VALUE		(TIMER_REG_BASE + 0x0094) 		/* 32KHz Timer1 Current Value */
#define TMR1_32K_CFG 		(TIMER_REG_BASE + 0x0098)  		/* 32KHz Timer1 Configuration */
#define TMR1_32K_ALM0 	 	(TIMER_REG_BASE + 0x009c)		/* 32KHz Timer1 Alarm0 Value */
#define TMR1_32K_OFFSET0 	(TIMER_REG_BASE + 0x00a0)		/* 32KHz Timer1 Offset Value for Alarm0 */
#define TMR1_32K_ALM1 	 	(TIMER_REG_BASE + 0x00a4)		/* 32KHz Timer0 Alarm1 Value */
#define TMR1_32K_OFFSET1 	(TIMER_REG_BASE + 0x00a8)		/* 32KHz Timer1 Offset Value for Alarm1 */

#define SYSCTL_REG_BASE   (0x10000000)						
#define ROSC_CTL			(SYSCTL_REG_BASE + 0x00c0) 		/* Ring Oscillator control, suggested to assign value 0x7d03 */
#define CLKCFG0				(SYSCTL_REG_BASE + 0x002c)		/* Clock Configuration Register 0 */

#define PLL1_CTRL  			(SYSCTL_REG_BASE + 0x0078)		//0X78
			

#define PMU_REG_BASE   		(0x10118000)				
			
#define PMU_CTRL  			(PMU_REG_BASE + 0x0008)  

#define PCTL_TB0_L4B		(PMU_REG_BASE + 0x0010)  
#define PCTL_TB0_H4B		(PMU_REG_BASE + 0x0014)  

#define PCTL_TB1_L4B		(PMU_REG_BASE + 0x0018)  
#define PCTL_TB1_H4B		(PMU_REG_BASE + 0x001C)  

#define PCTL_TB2_L4B		(PMU_REG_BASE + 0x0020)  
#define PCTL_TB2_H4B		(PMU_REG_BASE + 0x0024)  

#define PCTL_TB3_L4B		(PMU_REG_BASE + 0x0028)  
#define PCTL_TB3_H4B		(PMU_REG_BASE + 0x002C)  

#define PCTL_TB4_L4B		(PMU_REG_BASE + 0x0030)  
#define PCTL_TB4_H4B		(PMU_REG_BASE + 0x0034)  

#define PCTL_TB5_L4B		(PMU_REG_BASE + 0x0038)  
#define PCTL_TB5_H4B		(PMU_REG_BASE + 0x003C)  

#define PCTL_TB6_L4B		(PMU_REG_BASE + 0x0040)  
#define PCTL_TB6_H4B		(PMU_REG_BASE + 0x0044)  

#define PCTL_TB7_L4B		(PMU_REG_BASE + 0x0048)  
#define PCTL_TB7_H4B		(PMU_REG_BASE + 0x004C)  
#endif


#define CLKCFG0_INT_CPU_CLK_160MHZ		0x0
#define CLKCFG0_INT_CPU_CLK_REF			0x1

#define CLK_INT0_MASK					0x1	
#define CLK_INT1_MASK					0x2
#define CLK_INT2_MASK					0x4
#define CLK_INT3_MASK					0x8
#define CLK_INT4_MASK					0x10
#define CLK_INT5_MASK					0x20
#define CLK_TIMER0_MASK					0x40

#define INT_FRC_CPU_CLK_MASK_ALL (CLK_INT0_MASK | CLK_INT1_MASK | CLK_INT2_MASK | CLK_INT3_MASK | CLK_INT4_MASK | CLK_INT5_MASK | CLK_TIMER0_MASK)

#ifdef BIG_ENDIAN
typedef	union	_CLKCFG0_STRUC {
	struct	{
		UINT32		rsv2:1;
		UINT32		int_frc_cpu_clk:7;	
		UINT32		rsv1:7;
		UINT32		int_cpu_clk:1;
		UINT32		cpu_clk_sel:2;
		UINT32		rsv0:14;
	}	field;
	UINT32			word;
} CLKCFG0_STRUC, *PCLKCFG0_STRUC;
#else 
typedef	union	_CLKCFG0_STRUC {
	struct	{
		UINT32		rsv0:14;
		UINT32		cpu_clk_sel:2;
		UINT32		int_cpu_clk:1;
		UINT32		rsv1:7;		
		UINT32		int_frc_cpu_clk:7;	
		UINT32		rsv2:1;
	}	field;
	UINT32			word;
} CLKCFG0_STRUC, *PCLKCFG0_STRUC;
#endif


#ifdef BIG_ENDIAN
typedef	union	_INT_BIT_MAP_TABLE_STRUC {
	struct	{
		UINT32		rsv3:11;
		UINT32		Timer_32k:1;
		UINT32		Usb_Device:1;
		UINT32		Usb_Host:1;
		UINT32		rsv2:5;		
		UINT32		UartLite:1;
		UINT32		rsv1:5;
		UINT32		Pio:1;
		UINT32		rsv0:2;
		UINT32		Ill_Acc:1;
		UINT32		Timer1:1;		
		UINT32		Timer0:1;
		UINT32		SysCtrl:1;
	}	field;
	UINT32			word;
} INT_BIT_MAP_TABLE_STRUC, *PINT_BIT_MAP_TABLE_STRUC;
#else 
typedef	union	_INT_BIT_MAP_TABLE_STRUC {
	struct	{
		UINT32		SysCtrl:1;
		UINT32		Timer0:1;
		UINT32		Timer1:1;		
		UINT32		Ill_Acc:1;
		UINT32		rsv0:2;
		UINT32		Pio:1;
		UINT32		rsv1:5;
		UINT32		UartLite:1;
		UINT32		rsv2:5;		
		UINT32		Usb_Host:1;
		UINT32		Usb_Device:1;
		UINT32		Timer_32k:1;
		UINT32		rsv3:11;
	}	field;
	UINT32			word;
} INT_BIT_MAP_TABLE_STRUC, *PINT_BIT_MAP_TABLE_STRUC;
#endif

#ifdef BIG_ENDIAN
typedef	union	_TMR_32K_CFG_STRUC {
	struct	{
		UINT32		rsv2:22;	
		UINT32		Tmr_Nalm_en1:1;	
		UINT32		Tmr_Nalm_en0:1;	
		UINT32		Tmr0_En:1;
		UINT32		rsv1:1;		
		UINT32		Tmr_Mode:2;		
		UINT32		rsv0:2;	
		UINT32		Tmr_Scale:2;
	}	field;
	UINT32			word;
} TMR_32K_CFG_STRUC, *PTMR_32K_CFG_STRUC;
#else 
typedef	union	_TMR_32K_CFG_STRUC {
	struct	{
		UINT32		Tmr_Scale:2;
		UINT32		rsv0:2;
		UINT32		Tmr_Mode:2;		
		UINT32		rsv1:1;		
		UINT32		Tmr0_En:1;		
		UINT32		Tmr_Nalm_en0:1;		
		UINT32		Tmr_Nalm_en1:1;				
		UINT32		rsv2:22;			
	}	field;
	UINT32			word;
} TMR_32K_CFG_STRUC, *PTMR_32K_CFG_STRUC;
#endif

#if 0

#define PMU_REG_BASE      			(0x10118000)
#define PMU_INT_STATUS         		(PMU_REG_BASE + 0x0058)
#define PMU_INT_ENABLE         		(PMU_REG_BASE + 0x005c)


#define LDO_CFG1					(PMU_REG_BASE + 0x0054)

typedef struct _INT_CTRL_SW_CFG_STRUC {
	BOOLEAN 	bHwCtrlClk;
	BOOLEAN 	bSlpClkCg;
	UINT32 		RfOscRdySrc;
	BOOLEAN		bMem2Pd;
} INT_CTRL_SW_CFG_STRUC, *PINT_CTRL_CFG_STRUC;


#ifdef USB_SUSPEND	

#define USB_CONTROL_BASE   	0x10120000
#define USBIRQ				(USB_CONTROL_BASE + 0x18c*4)
#define CLKGATE   			(USB_CONTROL_BASE + 0x1a7*4)
#define USBIEN				(USB_CONTROL_BASE + 0x198*4) 


#define USB_SUSPEND_ENABLE			1
#define USB_SUSPEND_DISABLE			0

#endif /* USB_SUSPEND */
#endif // if 0

#define MAC_STATUS_REG    0x1200

void init_IRQ0_Timer(UINT32 Tdim_Count);

void TIMER_AsicForceWakeup(void);

#endif /* TIMER32K_PS */

#endif /* __TIMER32K_CTRL_H__ */

