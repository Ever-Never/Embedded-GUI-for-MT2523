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

#ifndef __XML_DEF_H__
#define __XML_DEF_H__

#include "stdint.h"

/********************************************************************/
/* declaration */
/********************************************************************/

/* DOM-NOT_FOR_SDK-BEGIN */
#define XML_TAG_PROLOG_START        "<?xml"
#define XML_TAG_PROLOG_END          "?>"
#define XML_TAG_DOCTYPE_START       "<!DOCTYPE"
#define XML_TAG_ELEMENTTYPE_START   "<!ELEMENT"
#define XML_TAG_ATTLISTTYPE_START   "<!ATTLIST"
#define XML_TAG_ENTITYTYPE_START    "<!ENTITY"
#define XML_TAG_NOTATIONTYPE_START  "<!NOTATION"
#define XML_TAG_COMMENT_START       "<!--"
#define XML_TAG_COMMENT_END         "-->"
#define XML_TAG_CDATA_START         "<![CDATA["
#define XML_TAG_CDATA_END           "]]>"
#define XML_TAG_PI_START            "<?"
#define XML_TAG_ENCODING            "encoding"
#define XML_TAG_ENCODING_UTF8       "utf-8"
#define XML_TAG_ENCODING_UTF16      "utf-16"
#define XML_TAG_ENCODING_UCS2       "iso-10646-ucs-2"
#define XML_TAG_ENCODING_UCS4       "iso-10646-ucs-4"
#define XML_TAG_ENCODING_ASCII      "iso-8859-1"

#define XML_TAG_ELEMENT_START       "<"
#define XML_TAG_ELEMENT_END         "</"
#define XML_TAG_SINGLE_END          "/>"
#define XML_TAG_GENERAL_END         ">"
#define XML_SPACE                   " "
#define XML_QUOTE                   "'"
#define XML_DOUBLE_QUOTE            "\""
#define XML_SQUARE_LEFT_BRAKET      "["
#define XML_SQUARE_RIGHT_BRAKET     "]"


/* Leo add for DLT */
/*****************************************
Note: DLT needs more buffer length.
If anyone needs to use these two macro,
   please include "MMI_features.h" before
   xml_def.h.
*****************************************/
#if defined (__MMI_DOWNLOADABLE_THEMES_SUPPORT__) || defined (SVG_SUPPORT)
#define XML_BUFFER_LENGTH        (512)
#define XML_MAX_ATTR_COUNT       (100)
#elif defined (__MMI_BT_MAP_CLIENT__)
#define XML_BUFFER_LENGTH        (128)
#define XML_MAX_ATTR_COUNT       (40)
#else /* defined (__MMI_DOWNLOADABLE_THEMES_SUPPORT__) || defined (__SVG_SUPPORT__) */
#define XML_BUFFER_LENGTH        (128)
#define XML_MAX_ATTR_COUNT       (30)
#endif /* defined (__MMI_DOWNLOADABLE_THEMES_SUPPORT__) || defined (__SVG_SUPPORT__) */

#define MAX_CTRL_BUFFER_SIZE    (2048)
#define XML_MAX_ENTITY_COUNT    (10)
#define XML_MAX_ELEM_DEPTH      (20)
#define XML_CHAR_ENTITY_LENGTH  (15)

#define MAX_FILEBUF_NUM     (2)
#define FILEBUF_SIZE        (5*1024)
#define FILEBUF_BACKSIZE    (2*1024)
#define XML_VIRFILE_MIN     (4*1024)
/* DOM-NOT_FOR_SDK-END */


/* FS definition start*/
typedef int32_t             FS_HANDLE;

#define FS_Write(A, B, C, D)        empty_func()
#define FS_Close(A)                 empty_func()
#define FS_Open(A, B)               empty_func()
#define FS_CreateDir(A)             empty_func()
#define DRM_open_file(A, B, C)      ((FS_HANDLE)empty_func())
#define DRM_close_file(A)           empty_func()
#define DRM_file_pos(A, B)          empty_func()
#define DRM_seek_file(A, B, C)      empty_func()
#define DRM_read_file(A, B, C, D)   empty_func()

/*---------------------------------------------------------------------------
 * Error Codes
 *---------------------------------------------------------------------------*/
typedef enum {
    FS_FILE_BEGIN,
    FS_FILE_CURRENT,
    FS_FILE_END
} FS_SEEK_POS_ENUM;

typedef enum {
    FS_NO_ERROR                    =    0,    /* No error */
    FS_ERROR_RESERVED              =   -1,    /* Reserved error, not used currently */
    FS_PARAM_ERROR                 =   -2,    /* User */
    FS_INVALID_FILENAME            =   -3,    /* User */
    FS_DRIVE_NOT_FOUND             =   -4,    /* User or Fatal */
    FS_TOO_MANY_FILES              =   -5,    /* User or Normal: use over max file handle number or more than 256 files in sort */
    FS_NO_MORE_FILES               =   -6,    /* Normal */
    FS_WRONG_MEDIA                 =   -7,    /* Fatal */
    FS_INVALID_FILE_SYSTEM         =   -8,    /* Fatal */
    FS_FILE_NOT_FOUND              =   -9,    /* User or Normal */
    FS_INVALID_FILE_HANDLE         =  -10,    /* User or Normal */
    FS_UNSUPPORTED_DEVICE          =  -11,    /* User */
    FS_UNSUPPORTED_DRIVER_FUNCTION =  -12,    /* User or Fatal */
    FS_CORRUPTED_PARTITION_TABLE   =  -13,    /* fatal */
    FS_TOO_MANY_DRIVES             =  -14,    /* not use so far */
    FS_INVALID_FILE_POS            =  -15,    /* User */
    FS_ACCESS_DENIED               =  -16,    /* User or Normal */
    FS_STRING_BUFFER_TOO_SMALL     =  -17,    /* User */
    FS_GENERAL_FAILURE             =  -18,    /* Normal */
    FS_PATH_NOT_FOUND              =  -19,    /* User */
    FS_FAT_ALLOC_ERROR             =  -20,    /* Fatal: disk crash */
    FS_ROOT_DIR_FULL               =  -21,    /* Normal */
    FS_DISK_FULL                   =  -22,    /* Normal */
    FS_TIMEOUT                     =  -23,    /* Normal: FS_CloseMSDC with nonblock */
    FS_BAD_SECTOR                  =  -24,    /* Normal: NAND flash bad block */
    FS_DATA_ERROR                  =  -25,    /* Normal: NAND flash bad block */
    FS_MEDIA_CHANGED               =  -26,    /* Normal */
    FS_SECTOR_NOT_FOUND            =  -27,    /* Fatal */
    FS_ADDRESS_MARK_NOT_FOUND      =  -28,    /* not use so far */
    FS_DRIVE_NOT_READY             =  -29,    /* Normal */
    FS_WRITE_PROTECTION            =  -30,    /* Normal: only for MSDC */
    FS_DMA_OVERRUN                 =  -31,    /* not use so far */
    FS_CRC_ERROR                   =  -32,    /* not use so far */
    FS_DEVICE_RESOURCE_ERROR       =  -33,    /* Fatal: Device crash */
    FS_INVALID_SECTOR_SIZE         =  -34,    /* Fatal */
    FS_OUT_OF_BUFFERS              =  -35,    /* Fatal */
    FS_FILE_EXISTS                 =  -36,    /* User or Normal */
    FS_LONG_FILE_POS               =  -37,    /* User : FS_Seek new pos over sizeof int */
    FS_FILE_TOO_LARGE              =  -38,    /* User: filesize + pos over sizeof int */
    FS_BAD_DIR_ENTRY               =  -39,    /* Fatal */
    FS_ATTR_CONFLICT               =  -40,    /* User: Can't specify FS_PROTECTION_MODE and FS_NONBLOCK_MOD */
    FS_CHECKDISK_RETRY             =  -41,    /* System: don't care */
    FS_LACK_OF_PROTECTION_SPACE    =  -42,    /* Fatal: Device crash */
    FS_SYSTEM_CRASH                =  -43,    /* Normal */
    FS_FAIL_GET_MEM                =  -44,    /* Normal */
    FS_READ_ONLY_ERROR             =  -45,    /* User or Normal */
    FS_DEVICE_BUSY                 =  -46,    /* Normal */
    FS_ABORTED_ERROR               =  -47,    /* Normal */
    FS_QUOTA_OVER_DISK_SPACE       =  -48,    /* Normal: Configuration Mistake */
    FS_PATH_OVER_LEN_ERROR         =  -49,    /* Normal */
    FS_APP_QUOTA_FULL              =  -50,    /* Normal */
    FS_VF_MAP_ERROR                =  -51,    /* User or Normal */
    FS_DEVICE_EXPORTED_ERROR       =  -52,    /* User or Normal */
    FS_DISK_FRAGMENT               =  -53,    /* Normal */
    FS_DIRCACHE_EXPIRED            =  -54,    /* Normal */
    FS_QUOTA_USAGE_WARNING         =  -55,    /* Normal or Fatal: System Drive Free Space Not Enought */
    FS_ERR_DIRDATA_LOCKED          =  -56,    /* Normal */
    FS_INVALID_OPERATION           =  -57,    /* Normal */
    FS_ERR_VF_PARENT_CLOSED        =  -58,    /* Virtual file's parent is closed */
    FS_ERR_UNSUPPORTED_SERVICE     =  -59,    /* The specified FS service is closed in this project. */

    FS_ERR_INVALID_JOB_ID          =  -81,
    FS_ERR_ASYNC_JOB_NOT_FOUND     =  -82,

    FS_MSDC_MOUNT_ERROR            = -100,    /* Normal */
    FS_MSDC_READ_SECTOR_ERROR      = -101,    /* Normal */
    FS_MSDC_WRITE_SECTOR_ERROR     = -102,    /* Normal */
    FS_MSDC_DISCARD_SECTOR_ERROR   = -103,    /* Normal */
    FS_MSDC_PRESNET_NOT_READY      = -104,    /* System */
    FS_MSDC_NOT_PRESENT            = -105,    /* Normal */

    FS_EXTERNAL_DEVICE_NOT_PRESENT = -106,    /* Normal */
    FS_HIGH_LEVEL_FORMAT_ERROR     = -107,    /* Normal */

    FS_CARD_BATCHCOUNT_NOT_PRESENT = -110,    /* Normal */

    FS_FLASH_MOUNT_ERROR           = -120,    /* Normal */
    FS_FLASH_ERASE_BUSY            = -121,    /* Normal: only for nonblock mode */
    FS_NAND_DEVICE_NOT_SUPPORTED   = -122,    /* Normal: Configuration Mistake */
    FS_FLASH_OTP_UNKNOWERR         = -123,    /* User or Normal */
    FS_FLASH_OTP_OVERSCOPE         = -124,    /* User or Normal */
    FS_FLASH_OTP_WRITEFAIL         = -125,    /* User or Normal */
    FS_FDM_VERSION_MISMATCH        = -126,    /* System */
    FS_FLASH_OTP_LOCK_ALREADY      = -127,    /* User or Normal */
    FS_FDM_FORMAT_ERROR            = -128,    /* The format of the disk content is not correct */

    FS_FDM_USER_DRIVE_BROKEN       = -129,    /*User drive unrecoverable broken*/
    FS_FDM_SYS_DRIVE_BROKEN        = -130,    /*System drive unrecoverable broken*/
    FS_FDM_MULTIPLE_BROKEN         = -131,    /*multiple unrecoverable broken*/

    FS_LOCK_MUTEX_FAIL             = -141,    /* System: don't care */
    FS_NO_NONBLOCKMODE             = -142,    /* User: try to call nonblock mode other than NOR flash */
    FS_NO_PROTECTIONMODE           = -143,    /* User: try to call protection mode other than NOR flash */

    /*
     * If disk size exceeds FS_MAX_DISK_SIZE (unit is KB, defined in makefile),
     * FS_TestMSDC(), FS_GetDevStatus(FS_MOUNT_STATE_ENUM) and all access behaviors will
     * get this error code.
     */
    FS_DISK_SIZE_TOO_LARGE         = (FS_MSDC_MOUNT_ERROR),     /*Normal*/

    FS_MINIMUM_ERROR_CODE          = -65536 /* 0xFFFF0000 */
} fs_error_enum;
/* FS definition end*/


/*****************************************************************************
 * <GROUP  CallbackFunctions>
 *
 * FUNCTION
 *  XML_start_elem_hdlr
 * DESCRIPTION
 *  This function is element start handler.
 * PARAMETERS
 *  data:        [IN]        reserved
 *  el:          [IN]        Element name
 *  attr:        [IN]        Attribution list
 *  error:       [IN]        error code
 * RETURNS
 *  void
 *****************************************************************************/
typedef void (*XML_start_elem_hdlr) (
    void *data,
    const char *el,
    const char **attr,
    int32_t error);


/*****************************************************************************
 * <GROUP  CallbackFunctions>
 *
 * FUNCTION
 *  XML_end_elem_hdlr
 * DESCRIPTION
 *  This function is element end handler.
 * PARAMETERS
 *  data:        [IN]        reserved
 *  el:          [IN]        Element name
 *  error:       [IN]        error code
 * RETURNS
 *  void
 *****************************************************************************/
typedef void (*XML_end_elem_hdlr) (void *data, const char *el, int32_t error);


/*****************************************************************************
 * <GROUP  CallbackFunctions>
 *
 * FUNCTION
 *  XML_start_doctype_hdlr
 * DESCRIPTION
 *  This function is document type start handler.
 * PARAMETERS
 *  data:            [IN]        reserved
 *  doctypeName:     [IN]        document type name
 *  sysid:           [IN]        system id
 *  pubid:           [IN]        public id
 *  internal_subset: [IN]        internal subset
 *  error:           [IN]        error code
 * RETURNS
 *  void
 *****************************************************************************/
typedef void (*XML_start_doctype_hdlr) (
    void *data,
    const char *doctypeName,
    const char **sysid,
    const char **pubid,
    int32_t internal_subset,
    int32_t error);


/*****************************************************************************
 * <GROUP  CallbackFunctions>
 *
 * FUNCTION
 *  XML_end_doctype_hdlr
 * DESCRIPTION
 *  This function is used to handle document type.
 * PARAMETERS
 *  data:            [IN]        reserved
 *  error:           [IN]        error code
 * RETURNS
 *  void
 *****************************************************************************/
typedef void (*XML_end_doctype_hdlr) (void *data, int32_t error);


/*****************************************************************************
 * <GROUP  CallbackFunctions>
 *
 * FUNCTION
 *  XML_data_hdlr
 * DESCRIPTION
 *  This function is used to handle data.
 * PARAMETERS
 *  resv:            [IN]        reserved
 *  el:              [IN]        element name
 *  data:            [IN]        data
 *  len:             [IN]        length
 *  error:           [IN]        error code
 * RETURNS
 *  void
 *****************************************************************************/
typedef void (*XML_data_hdlr) (
    void *resv,
    const char *el,
    const char *data,
    int32_t len,
    int32_t error);


/*****************************************************************************
 * <GROUP  CallbackFunctions>
 *
 * FUNCTION
 *  XML_malloc_buffer_ptr
 * DESCRIPTION
 *  This function is used to malloc buffer.
 * PARAMETERS
 *  len:           [IN]        length
 * RETURNS
 *  void
 *****************************************************************************/
typedef void *(*XML_malloc_buffer_ptr) (int32_t len);


/*****************************************************************************
 * <GROUP  CallbackFunctions>
 *
 * FUNCTION
 *  XML_free_buffer_ptr
 * DESCRIPTION
 *  This function is used to free buffer.
 * PARAMETERS
 *  ptr:           [IN]        buffer pointer
 * RETURNS
 *  void
 *****************************************************************************/
typedef void (*XML_free_buffer_ptr) (void *ptr);


/* define the error code */
typedef enum {
    XML_RESULT_IS_STOPPED = -5, /* the parser is stopped. */
    XML_RESULT_FILE_NOT_FOUND = -4, /* the file isn't found */
    XML_RESULT_OUT_OF_MEMORY = -3,  /* the memory isn't enough*/
    XML_RESULT_ENCODING_ERROR = -2, /* the char encoding error */
    XML_RESULT_FAIL = -1,           /* fail to parse xml */
    XML_RESULT_OK = 0               /* parsing xml succed */
} XML_RESULT_ENUM;

/* define the error code */
typedef enum {
    XML_NO_ERROR,                               /* no error */
    XML_ERROR_MISMATCH_TAG,                     /* error string: end tag name */
    XML_ERROR_MISMATCH_DOCTYPENAME_ROOTNAME,    /* error string : root element name */
    XML_ERROR_INVALID_ATTRIBUTE,                /* error string : attribute name */
    XML_ERROR_DUPLICATE_ATTRIBUTE,              /* error string : attribute name */
    XML_ERROR_ENTITY_DEFINITION,                /* error string: entity name */
    XML_ERROR_UNDEFINED_ENTITY                  /* error string : entity name */
} xml_error_enum;

/* define the pending type */
typedef enum {
    XML_PENDING_NONE,           /* no pending content */
    XML_PENDING_PROLOG,         /* the prolog is pending */
    XML_PENDING_DOCTYPE,        /* the document type decare is pending */
    XML_PENDING_DOCTYPE_MARKUP, /* the doctype markup is pending */
    XML_PENDING_COMMENT,        /* the comment is pending */
    XML_PENDING_PI,             /* the pi is pending */
    XML_PENDING_CDATA,          /* the cdata is pending */
    XML_PENDING_START_ELEMENT,  /* the start element is pending */
    XML_PENDING_END_ELEMENT     /* the end element is pending */
} XML_PENDING_ENTITY_TYPE_ENUM;

/* define the charset encoding */
typedef enum {
    XML_ENCODING_UTF8,      /* the char encoding is utf8 */
    XML_ENCODING_UTF16,     /* the char encoding is utf16 */
    XML_ENCODING_UCS2,      /* the char encoding is ucs2 */
    XML_ENCODING_UCS4,      /* the char encoding is ucs4 */
    XML_ENCODING_UNKNOWN    /* the char encoding is unknown */
} XML_ENCODING_TYPE_ENUM;

/* this struct is used to parse char entity */
typedef struct {
    const char *named_string;   /* replace text of char entity */
    const char named_ch;        /* Predefined char entity */
} named_char_entry_struct;

/* this struct is used when using file buffer */
typedef struct {
    char  *bufptr;              /* point to the buffer provided by app */
    int32_t virfilelen;           /* the length of data in buffer */
    int32_t virfileseek;          /* the position of file buffer */
    int32_t bufsize;              /* the size of file buffer */
    FS_HANDLE fp;                   /* file handle */
    int32_t realfilebeginpos;     /* file begin position from where we read data */
    int32_t realfileseekpos;      /* seek pointer of real file */
    char  valid;                /* whether the file buffer is valid */
    char  isfull;               /* whether the buffer is full */
} virtual_file_buf_struct;

/* this struct is used to parse xml */
typedef struct {
    XML_malloc_buffer_ptr get_buffer_ptr;       /* function pointer provided by app */
    XML_free_buffer_ptr free_buffer_ptr;        /* function pointer provided by app */
    XML_start_doctype_hdlr start_doctype_hdlr;  /* function pointer provided by app */
    XML_end_doctype_hdlr end_doctype_hdlr;      /* function pointer provided by app */
    XML_start_elem_hdlr start_elem_hdlr;        /* function pointer provided by app */
    XML_end_elem_hdlr end_elem_hdlr;            /* function pointer provided by app */
    XML_data_hdlr data_hdlr;                    /* function pointer provided by app */
    FS_HANDLE source;                           /* file handle */
    virtual_file_buf_struct virfileptr;         /* information of file buffer  */
    const char *xml_text;                   /* point to buffer provided by app */
    char *elem[XML_MAX_ELEM_DEPTH];         /* stack for managing element nesting */
    char *ge_data[XML_MAX_ENTITY_COUNT << 1]; /* manage general entity data */
    char *pe_data[XML_MAX_ENTITY_COUNT << 1]; /* manage parameter entity data */
    char *doct_name;        /* this name is saved to match with root element name */
    char *token_ptr;        /* token pointer */
    char *ap_temp;          /* temp buffer to parse xml */
    char *err_str;          /* save the error string */
    void *root;                 /* indicate the root element */
    int32_t xml_len;          /* the length of xml_text */
    int32_t curr_depth;       /* stack pointer */
    int32_t error;            /* error code */
    int32_t prolog_depth;     /* the depth of prolog */
    int32_t encoding;         /* character encoding */
    int32_t prolog_done;      /* whether the prolog has been parsed */
    int32_t pos;              /* file position that indicates the length of parsed content */
    int32_t pending_start_pos; /* indicate the file position of the pending data */
    int32_t cdata_start_pos;  /* indicate the file position of the cdata */
    int32_t cdata_len;        /* the length of cdata */
    int32_t data_start_pos;   /* indicate the file position of the data */
    int32_t need_end_tag;     /* whether need the end tag */
    int32_t pending_type;     /* please refer to XML_PENDING_ENTITY_TYPE_ENUM */
    int32_t stop;             /* whether stop the xml parser */
    int32_t pause;            /* whether pause the xml parser */
    uint8_t pe_count;         /* the count of parameter entity */
    uint8_t ge_count;         /* the count of general entity */
    int32_t offset;
    uint8_t isext;                            /* whether use file buffer */
} XML_PARSER_STRUCT;

/* DOM-NOT_FOR_SDK-BEGIN */
/* this function is used as default element start handler */
extern void xml_elem_start_hdlr(
    void *data,
    const char *el,
    const char **attr,
    int32_t error);

/* this function is used as default element end handler */
extern void xml_elem_end_hdlr(void *data, const char *el, int32_t error);

/* this function is used as default doctype start handler */
extern void xml_doctype_start_hdlr(
    void *data,
    const char *doctypeName,
    const char **sysid,
    const char **pubid,
    int32_t internal_subset,
    int32_t error);

/* this function is used as default doctype end handler */
extern void xml_doctype_end_hdlr(void *data, int32_t error);

/* this function is used as default general data handler */
extern void xml_general_data_hdlr(
    void *resv,
    const char *el,
    const char *data,
    int32_t len,
    int32_t error);
/* DOM-NOT_FOR_SDK-END */

/*****************************************************************************
 * FUNCTION
 *  xml_new_parser
 * DESCRIPTION
 *  This function is used to new parser
 * PARAMETERS
 *  parser:      [IN]        Global parser struct
 * RETURNS
 *  result of parser
 *****************************************************************************/
extern int32_t xml_new_parser(XML_PARSER_STRUCT *parser);


/*****************************************************************************
 * FUNCTION
 *  xml_close_parser
 * DESCRIPTION
 *  This function is used to close parser
 * PARAMETERS
 *  parser:      [IN]        Global parser struct
 * RETURNS
 *  void
 *****************************************************************************/
extern void xml_close_parser(XML_PARSER_STRUCT *parser);


/*****************************************************************************
 * FUNCTION
 *  xml_register_doctype_handler
 * DESCRIPTION
 *  This functin is used to register document handler
 * PARAMETERS
 *  parser:          [OUT]       Global parser struct
 *  start_hdlr:      [IN]        Function handler to handle start doc data
 *  end_hdlr:        [IN]        Function handler to handle end doc data
 * RETURNS
 *  void
 *****************************************************************************/
extern void xml_register_doctype_handler(
    XML_PARSER_STRUCT *parser,
    XML_start_doctype_hdlr start_hdlr,
    XML_end_doctype_hdlr end_hdlr);


/*****************************************************************************
 * FUNCTION
 *  xml_register_element_handler
 * DESCRIPTION
 *  This function is used to register element handler
 * PARAMETERS
 *  parser:          [OUT]       Global parser struct
 *  start_hdlr:      [IN]        Function handler to handle start element data
 *  end_hdlr:        [IN]        Function handler to handle end element data
 * RETURNS
 *  void
 *****************************************************************************/
extern void xml_register_element_handler(
    XML_PARSER_STRUCT *parser,
    XML_start_elem_hdlr start_hdlr,
    XML_end_elem_hdlr end_hdlr);


/*****************************************************************************
 * FUNCTION
 *  xml_register_data_handler
 * DESCRIPTION
 *  This function is used to register data handler
 * PARAMETERS
 *  parser:          [OUT]       Global parser struct
 *  data_hdlr:       [IN]        Function handler to handle data
 * RETURNS
 *  void
 *****************************************************************************/
extern void xml_register_data_handler(XML_PARSER_STRUCT *parser, XML_data_hdlr data_hdlr);


/*****************************************************************************
 * FUNCTION
 *  xml_parse_buffer
 * DESCRIPTION
 *  This function is called by application to start parsing if application
 *  pass xml text with buffer
 * PARAMETERS
 *  parser:          [IN/OUT]        Global parser struct
 *  xmltext:         [IN]            Xml text buffer
 *  length:          [IN]            the length of buffer
 * RETURNS
 *  return XML_RESULT_OK if successful
 *****************************************************************************/
extern int32_t xml_parse_buffer(
    XML_PARSER_STRUCT *parser,
    const char *xmltext,
    int32_t length);


/*****************************************************************************
 * FUNCTION
 *  xml_resume_parse
 * DESCRIPTION
 *  This function is called to resume parsing
 * PARAMETERS
 *  parser:      [IN/OUT]        Global parser struct
 * RETURNS
 *  void
 *****************************************************************************/
extern int32_t xml_resume_parse(XML_PARSER_STRUCT *parser);


/*****************************************************************************
 * FUNCTION
 *  xml_pause_parse
 * DESCRIPTION
 *  This function is called to pause parsing
 * PARAMETERS
 *  parser:      [IN/OUT]        Global parser struct
 * RETURNS
 *  void
 *****************************************************************************/
extern void xml_pause_parse(XML_PARSER_STRUCT *parser);


/*****************************************************************************
 * FUNCTION
 *  xml_stop_parse
 * DESCRIPTION
 *  This function is called to stop parsing
 * PARAMETERS
 *  parser:      [IN/OUT]        Global parser struct
 * RETURNS
 *  void
 *****************************************************************************/
extern void xml_stop_parse(XML_PARSER_STRUCT *parser);


/*****************************************************************************
 * FUNCTION
 *  xml_free_parse
 * DESCRIPTION
 *  This function is called to free parser
 * PARAMETERS
 *  parser:      [IN/OUT]        Global parser struct
 * RETURNS
 *  void
 *****************************************************************************/
extern void xml_free_parse(XML_PARSER_STRUCT *parser);


/*****************************************************************************
 * FUNCTION
 *  xml_get_err_string
 * DESCRIPTION
 *  This function is used to return error string to application
 * PARAMETERS
 *  parser:      [IN]        Global parser struct
 * RETURNS
 *  return error string to application
 *****************************************************************************/
extern char *xml_get_err_string(XML_PARSER_STRUCT *parser);


/*****************************************************************************
 * FUNCTION
 *  xml_configure_memory
 * DESCRIPTION
 *  This function is used to configure memory
 * PARAMETERS
 *  parser:          [IN]        Global parser struct
 *  get_ptr:         [IN]        The pointer to malloc function
 *  free_ptr:        [IN]
 * RETURNS
 *  void
 *****************************************************************************/
extern void xml_configure_memory(
    XML_PARSER_STRUCT *parser,
    XML_malloc_buffer_ptr get_ptr,
    XML_free_buffer_ptr free_ptr);

#endif /* __XML_DEF_H__ */


