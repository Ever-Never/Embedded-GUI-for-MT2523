#ifndef __DEFINE_H__
#define __DEFINE_H__

#define USENORFLASH 1
//#define _2870_USB_MODE_	1
//#define _2860_PCI_MODE_	1
//#define AUTORUN	1
//#define AT25HP512				//TODO: Final review these definitions

//typedef unsigned char	BYTE;
//typedef unsigned short	WORD;
typedef unsigned long	DWORD;
//typedef bit				BOOL;
//typedef bit				bool;

#define TRUE	1
#define	OK		1
#define FALSE	0
#define	FAIL	0
#define	NOTYET	0

#define CARRIER_DETECT_TASK_NUM	6
#define	RADAR_DETECT_TASK_NUM	7

#define GetBit(x, y)			(	((x) & (1 << (y)))		)
#define GetBitX(x, y)			(	((x) & (1 << (y)))>>y		)
#define SetBit(x, y)			(	(x) = (x) | (1 << (y))	)
#define ClearBit(x, y)			{	(x) &= (~(1 << (y)));	}
#define DSetBit(x, y)			(	(x) = (1 << (y))		)
#define MakeWord(HI, LO)		(	((WORD)(LO)) | (((WORD)(HI))<<8) )	// Note! LO must be read first
#define HiByte(W)				(	(BYTE)(((WORD)W) >> 8)	)
#define LoByte(W)				(	(BYTE)(W)				)
#define	FireInterruptToHost()	{	IntToHost = 0xff;		}
WORD minW(WORD x, WORD y);
BYTE minB(BYTE x, BYTE y);

union W_B				// A word allowed easy byte access
{
	BYTE B[2];
	WORD W;
};


union DW_B				// A word allowed easy byte access
{
	BYTE B[4];
	DWORD DW;
};


union uLedParameter
{
	BYTE	LedParamArray[7];
	struct
	{
		BYTE	LedMode;
		BYTE	LedCfgAGLo;
		BYTE	LedCfgAGHi;
		BYTE	LedCfgActLo;
		BYTE	LedCfgActHi;
		BYTE	LedPolarityLo;
		BYTE	LedPolarityHi;
	} Idv;
};

#endif
