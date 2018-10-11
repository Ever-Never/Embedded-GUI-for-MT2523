/*****************************************************************************
 *============================================================================
 *                               HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * $Log$
 *
 * 06 13 2015 raghav.monga
 * [BORA00004254] [MT7637E2][IOT][FW]
 * Add repeater Support. Synced to JEDI revision: 127949
 * 1) DHCP working
 * 2) ARP working
 * 3) Ping working.
 *
 * 05 20 2013 ryan.hou
 * [BORA00002024] [MT7662] [BT] Update the latest patch
 * Merge 1st version with MT6630 PLC/mSBC.
 *
 * Apr 29 2009 mtk01270
 * [BORA00000006] Check in first verion BORA firmware source code
 * add to source control recursely
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/

#ifndef _SYS_HAL_TYPES_EXT_H
#define _SYS_HAL_TYPES_EXT_H

/******************************************************************************
 * MODULE NAME:    sys_hal_types.h
 * PROJECT CODE:   BlueStream
 * DESCRIPTION:    Basic platform specific type definitions
 * AUTHOR:         John Sheehy    
 * DATE:           08 March 2001
 *
 * SOURCE CONTROL: $Id: sys_hal_types.h,v 1.3 2004/07/07 14:30:50 namarad Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2001-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *    08 March 2001   - Initially added by js
 *                                
 *
 * NOTES TO USERS:
 ******************************************************************************/

#define __user 	

typedef char	__s8;
typedef short	__s16;
typedef int		__s32;
typedef unsigned char	__u8;
typedef unsigned short	__u16;
typedef unsigned int	__u32;

typedef char	INT8;
typedef short	INT16;
typedef int		INT32;
typedef unsigned char	UINT8;
typedef unsigned short	UINT16;
typedef unsigned int	UINT32;

typedef char			s8;
typedef unsigned char	u8;
typedef unsigned short	u16;
typedef unsigned int	u32;

typedef unsigned long 	*PULONG;
typedef unsigned short 	*PUSHORT;
typedef unsigned int 	*PUINT;
typedef char	*PSTRING;

#endif /* _SYS_HAL_TYPES_EXT_H */


