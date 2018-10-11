/*
** $Id: //Department/DaVinci/TRUNK/MT6620_WiFi_Firmware/mcu/wifi/include/link.h#4 $
*/

/*! \file   link.h
    \brief  Definition for simple doubly linked list operations.

    In this file we define the simple doubly linked list data structure and its
    operation MACROs and INLINE functions.
*/

/*******************************************************************************
* Copyright (c) 2007 MediaTek Inc.
*
* All rights reserved. Copying, compilation, modification, distribution
* or any other use whatsoever of this material is strictly prohibited
* except in accordance with a Software License Agreement with
* MediaTek Inc.
********************************************************************************
*/

/*******************************************************************************
* LEGAL DISCLAIMER
*
* BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND
* AGREES THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK
* SOFTWARE") RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE
* PROVIDED TO BUYER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY
* DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT
* LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
* PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE
* ANY WARRANTY WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY
* WHICH MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK
* SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY
* WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE
* FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION OR TO
* CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
* BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
* LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL
* BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT
* ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY
* BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
* THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
* WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT
* OF LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING
* THEREOF AND RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN
* FRANCISCO, CA, UNDER THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE
* (ICC).
********************************************************************************
*/

/*
** $Log: link.h $
**
** 08 07 2013 cp.wu
** [BORA00002253] [MT6630 Wi-Fi][Driver][Firmware] Add NLO and timeout mechanism to SCN module
** enable NLO to be running in parallel with normal scan
** 
** 08 06 2012 yarco.yang
** [BORA00002091] [MT6630]Support 6630 complier flag
 *
 * 07 09 2012 george.huang
 * [BORA00001717] [MT6628][ROM patch] HAL power on sequence patch
 * update for 6582, decouple ROM to RAM include header file dependency
 *
 * 05 25 2012 george.huang
 * [WCXRP00000905] [MT6628 Wi-Fi][FW] Code refinement for ROM/ RAM module dependency
 * inital version for MT6628_WIFI_FIRMWARE_MP
 *
 * 08 02 2011 terry.wu
 * [WCXRP00000891] [MT5931][MCUSYS] move code to SYSRAM to enable ASSERT_DUMP
 * Replace ASSERT check of linking list and queue macro with LITE_ASSERT.
 *
 * 12 07 2010 cm.chang
 * [WCXRP00000239] MT6620 Wi-Fi][Driver][FW] Merge concurrent branch back to maintrunk
 * 1. BSS_INFO include RLM information
 * 2. New CNM concurrent mechanism
 *
 * 07 19 2010 cm.chang
 *
 * Update RLM parameter and enable CNM channel manager
 *
 * 07 12 2010 cp.wu
 *
 * commit Wi-Fi firmware part for MT6620
 *
 * May 4 2009 mtk01084
 * [BORA00000018] Integrate WIFI part into BORA for the 1st time
 * add WIFI to BORA source control
**  \main\maintrunk.MT5921\8 2008-10-16 15:57:11 GMT mtk01461
**  Update driver to fix lint warning
**  \main\maintrunk.MT5921\7 2008-08-10 18:47:53 GMT mtk01461
**  Update for Driver Review
**  \main\maintrunk.MT5921\6 2007-12-11 00:09:00 GMT mtk01461
**  Add macro for checking valid list
**  \main\maintrunk.MT5921\5 2007-11-13 14:27:01 GMT mtk01461
**  Add LINK_IS_INVALID macro
** Revision 1.1.1.1  2007/06/22 08:09:05  MTK01461
** no message
**
*/

#ifndef _LINK_ROM_H
#define _LINK_ROM_H



/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/

/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/



/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/
#define INVALID_LINK_POISON1    ((VOID *) 0x00100101) // May cause page fault & unalignment issue (data abort)
#define INVALID_LINK_POISON2    ((VOID *) 0x00100201) // Used to verify that nonbody uses non-initialized link entries.


/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/
/* Simple Doubly Linked List Structures - Entry Part */
typedef struct _LINK_ENTRY_T {
    struct _LINK_ENTRY_T *prNext, *prPrev;
} LINK_ENTRY_T, *P_LINK_ENTRY_T;

/* Simple Doubly Linked List Structures - List Part */
typedef struct _LINK_T {
    P_LINK_ENTRY_T  prNext;
    P_LINK_ENTRY_T  prPrev;
    UINT_32 u4NumElem;
} LINK_T, *P_LINK_T;

/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/

/*******************************************************************************
*                           P R I V A T E   D A T A
********************************************************************************
*/

/*******************************************************************************
*                                 M A C R O S
********************************************************************************
*/
#if 0 // No one use it, temporarily mark it for [Lint - Info 773]
#define LINK_ADDR(rLink)        { (P_LINK_ENTRY_T)(&(rLink)), (P_LINK_ENTRY_T)(&(rLink)), 0 }

#define LINK_DECLARATION(rLink) \
    struct _LINK_T rLink = LINK_ADDR(rLink)
#endif

#define LINK_INITIALIZE(_prLink) \
    do { \
        ((P_LINK_T)(_prLink))->prNext = (P_LINK_ENTRY_T)(_prLink); \
        ((P_LINK_T)(_prLink))->prPrev = (P_LINK_ENTRY_T)(_prLink); \
        ((P_LINK_T)(_prLink))->u4NumElem = 0; \
    } while (0)

#define LINK_ENTRY_INITIALIZE(_prEntry) \
    do { \
        ((P_LINK_ENTRY_T)(_prEntry))->prNext = (P_LINK_ENTRY_T)NULL; \
        ((P_LINK_ENTRY_T)(_prEntry))->prPrev = (P_LINK_ENTRY_T)NULL; \
    } while (0)

#define LINK_ENTRY_INVALID(_prEntry) \
    do { \
        ((P_LINK_ENTRY_T)(_prEntry))->prNext = (P_LINK_ENTRY_T)INVALID_LINK_POISON1; \
        ((P_LINK_ENTRY_T)(_prEntry))->prPrev = (P_LINK_ENTRY_T)INVALID_LINK_POISON2; \
    } while (0)

#define LINK_IS_EMPTY(_prLink)          (((P_LINK_T)(_prLink))->prNext == (P_LINK_ENTRY_T)(_prLink))

/* NOTE: We should do memory zero before any LINK been initiated, so we can check
 * if it is valid before parsing the LINK.
 */
#define LINK_IS_INVALID(_prLink)        (((P_LINK_T)(_prLink))->prNext == (P_LINK_ENTRY_T)NULL)

#define LINK_IS_VALID(_prLink)          (((P_LINK_T)(_prLink))->prNext != (P_LINK_ENTRY_T)NULL)


#define LINK_ENTRY(ptr, type, member)   ENTRY_OF(ptr, type, member)

/* Insert an entry into a link list's head */
#define LINK_INSERT_HEAD(_prLink, _prEntry) \
        { \
            linkAdd(_prEntry, _prLink); \
            ((_prLink)->u4NumElem)++; \
        }


/* Append an entry into a link list's tail */
#define LINK_INSERT_TAIL(_prLink, _prEntry) \
        { \
            linkAddTail(_prEntry, _prLink); \
            ((_prLink)->u4NumElem)++; \
        }

/* Peek head entry, but keep still in link list */
#define LINK_PEEK_HEAD(_prLink, _type, _member) \
        ( \
            LINK_IS_EMPTY(_prLink) ? \
            NULL : LINK_ENTRY((_prLink)->prNext, _type, _member) \
        )

/* Peek tail entry, but keep still in link list */
#define LINK_PEEK_TAIL(_prLink, _type, _member) \
        ( \
            LINK_IS_EMPTY(_prLink) ? \
            NULL : LINK_ENTRY((_prLink)->prPrev, _type, _member) \
        )


/* Get first entry from a link list */
/* NOTE: We assume the link entry located at the beginning of "prEntry Type",
 * so that we can cast the link entry to other data type without doubts.
 * And this macro also decrease the total entry count at the same time.
 */
#define LINK_REMOVE_HEAD(_prLink, _prEntry, _P_TYPE) \
        { \
            LITE_ASSERT(_prLink); \
            if (LINK_IS_EMPTY(_prLink)) { \
                _prEntry = (_P_TYPE)NULL; \
            } \
            else { \
                _prEntry = (_P_TYPE)(((P_LINK_T)(_prLink))->prNext); \
                linkDel((P_LINK_ENTRY_T)(_prEntry)); \
                ((_prLink)->u4NumElem)--; \
            } \
        }

/* Assume the link entry located at the beginning of prEntry Type.
 * And also decrease the total entry count.
 */
#define LINK_REMOVE_KNOWN_ENTRY(_prLink, _prEntry) \
        { \
            LITE_ASSERT(_prLink); \
            LITE_ASSERT(_prEntry); \
            linkDel((P_LINK_ENTRY_T)(_prEntry)); \
            ((_prLink)->u4NumElem)--; \
        }

/* Iterate over a link list */
#define LINK_FOR_EACH(_prEntry, _prLink) \
    for (_prEntry = (_prLink)->prNext; \
         _prEntry != (P_LINK_ENTRY_T)(_prLink); \
         _prEntry = ((P_LINK_ENTRY_T)(_prEntry))->prNext)

/* Iterate over a link list backwards */
#define LINK_FOR_EACH_PREV(_prEntry, _prLink) \
    for (_prEntry = (_prLink)->prPrev; \
         _prEntry != (P_LINK_ENTRY_T)(_prLink); \
         _prEntry = ((P_LINK_ENTRY_T)(_prEntry))->prPrev)

/* Iterate over a link list safe against removal of link entry */
#define LINK_FOR_EACH_SAFE(_prEntry, _prNextEntry, _prLink) \
    for (_prEntry = (_prLink)->prNext, _prNextEntry = (_prEntry)->prNext; \
         _prEntry != (P_LINK_ENTRY_T)(_prLink); \
         _prEntry = _prNextEntry, _prNextEntry = (_prEntry)->prNext)

/* Iterate over a link list of given type */
#define LINK_FOR_EACH_ENTRY(_prObj, _prLink, _rMember, _TYPE) \
    for (_prObj = LINK_ENTRY((_prLink)->prNext, _TYPE, _rMember); \
         &((_prObj)->_rMember) != (P_LINK_ENTRY_T)(_prLink); \
         _prObj = LINK_ENTRY((_prObj)->_rMember.prNext, _TYPE, _rMember))

/* Iterate backwards over a link list of given type */
#define LINK_FOR_EACH_ENTRY_PREV(_prObj, _prLink, _rMember, _TYPE) \
    for (_prObj = LINK_ENTRY((_prLink)->prPrev, _TYPE, _rMember);  \
         &((_prObj)->_rMember) != (P_LINK_ENTRY_T)(_prLink); \
         _prObj = LINK_ENTRY((_prObj)->_rMember.prPrev, _TYPE, _rMember))

/* Iterate over a link list of given type safe against removal of link entry */
#define LINK_FOR_EACH_ENTRY_SAFE(_prObj, _prNextObj, _prLink, _rMember, _TYPE) \
    for (_prObj = LINK_ENTRY((_prLink)->prNext, _TYPE, _rMember),  \
         _prNextObj = LINK_ENTRY((_prObj)->_rMember.prNext, _TYPE, _rMember); \
         &((_prObj)->_rMember) != (P_LINK_ENTRY_T)(_prLink); \
         _prObj = _prNextObj, \
         _prNextObj = LINK_ENTRY((_prNextObj)->_rMember.prNext,_TYPE, _rMember))

/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/

/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/
/*----------------------------------------------------------------------------*/
/*!
* \brief This function is only for internal link list manipulation.
*
* \param[in] prNew  Pointer of new link head
* \param[in] prPrev Pointer of previous link head
* \param[in] prNext Pointer of next link head
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
__KAL_INLINE__ VOID
__linkAdd (
    IN P_LINK_ENTRY_T prNew,
    IN P_LINK_ENTRY_T prPrev,
    IN P_LINK_ENTRY_T prNext
    )
{
    prNext->prPrev = prNew;
    prNew->prNext = prNext;
    prNew->prPrev = prPrev;
    prPrev->prNext = prNew;

    return;
} /* end of __linkAdd() */


/*----------------------------------------------------------------------------*/
/*!
* \brief This function will add a new entry after the specified link head.
*
* \param[in] prNew  New entry to be added
* \param[in] prHead Specified link head to add it after
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
__KAL_INLINE__ VOID
linkAdd (
    IN P_LINK_ENTRY_T prNew,
    IN P_LINK_T prLink
    )
{
    __linkAdd(prNew, (P_LINK_ENTRY_T)prLink, prLink->prNext);

    return;
} /* end of linkAdd() */


/*----------------------------------------------------------------------------*/
/*!
* \brief This function will add a new entry before the specified link head.
*
* \param[in] prNew  New entry to be added
* \param[in] prHead Specified link head to add it before
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
__KAL_INLINE__ VOID
linkAddTail (
    IN P_LINK_ENTRY_T prNew,
    IN P_LINK_T prLink
    )
{
    __linkAdd(prNew, prLink->prPrev, (P_LINK_ENTRY_T)prLink);

    return;
} /* end of linkAddTail() */


/*----------------------------------------------------------------------------*/
/*!
* \brief This function is only for internal link list manipulation.
*
* \param[in] prPrev Pointer of previous link head
* \param[in] prNext Pointer of next link head
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
__KAL_INLINE__ VOID
__linkDel (
    IN P_LINK_ENTRY_T prPrev,
    IN P_LINK_ENTRY_T prNext
    )
{
    prNext->prPrev = prPrev;
    prPrev->prNext = prNext;

    return;
} /* end of __linkDel() */


/*----------------------------------------------------------------------------*/
/*!
* \brief This function will delete a specified entry from link list.
*        NOTE: the entry is in an initial state.
*
* \param prEntry    Specified link head(entry)
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
__KAL_INLINE__ VOID
linkDel (
    IN P_LINK_ENTRY_T prEntry
    )
{
    __linkDel(prEntry->prPrev, prEntry->prNext);

    LINK_ENTRY_INITIALIZE(prEntry);

    return;
} /* end of linkDel() */


/*----------------------------------------------------------------------------*/
/*!
* \brief This function will delete a specified entry from link list and then add it
*        after the specified link head.
*
* \param[in] prEntry        Specified link head(entry)
* \param[in] prOtherHead    Another link head to add it after
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
__KAL_INLINE__ VOID
linkMove (
    IN P_LINK_ENTRY_T prEntry,
    IN P_LINK_T prLink
    )
{
    __linkDel(prEntry->prPrev, prEntry->prNext);
    linkAdd(prEntry, prLink);

    return;
} /* end of linkMove() */


/*----------------------------------------------------------------------------*/
/*!
* \brief This function will delete a specified entry from link list and then add it
*        before the specified link head.
*
* \param[in] prEntry        Specified link head(entry)
* \param[in] prOtherHead    Another link head to add it before
*
* \return (none)
*/
/*----------------------------------------------------------------------------*/
__KAL_INLINE__ VOID
linkMoveTail (
    IN P_LINK_ENTRY_T prEntry,
    IN P_LINK_T prLink
    )
{
    __linkDel(prEntry->prPrev, prEntry->prNext);
    linkAddTail(prEntry, prLink);

    return;
} /* end of linkMoveTail() */

#endif /* _LINK_ROM_H */

