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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "xml_def.h"
#include "stdbool.h"
#include <assert.h>
#include "syslog.h"
log_create_module(XML, PRINT_LEVEL_INFO);

#ifndef WIN32
//#include "portable.h"
#include "FreeRTOS.h"
#endif


#ifdef MMI_ON_WIN32
#include "OslMemory.h"
#define free_buffer(P,X)   OslMfree(X)
#define get_buffer(P,X)    OslMalloc(X)
#else /* MMI_ON_WIN32 */
#define free_buffer(P,X)   xml_free_buffer(P,X)
#define get_buffer(P,X)    xml_get_buffer(P,X)
#endif /* MMI_ON_WIN32 */

#define XML_TRACE_INFO TRACE_INFO

/********************************************************************/
/* code begin */
/********************************************************************/

#define MAX_NAME_CHAR_TABLE_SIZE 5

const named_char_entry_struct xml_named_char_table[MAX_NAME_CHAR_TABLE_SIZE] = {
    {"&lt;", '<'},
    {"&gt;", '>'},
    {"&amp;", '&'},
    {"&apos;", '\''},
    {"&quot;", '\"'}
};

/* internal functions */
static void xml_reset_parser(XML_PARSER_STRUCT *parser);
static int32_t xml_parse_prolog(XML_PARSER_STRUCT *parser);
static int32_t xml_parse_element(XML_PARSER_STRUCT *parser);
static bool xml_init_folder(void);
static int32_t xml_process_prolog(XML_PARSER_STRUCT *parser, int32_t start, int32_t length);
static int32_t xml_process_doctype(XML_PARSER_STRUCT *parser, int32_t start, int32_t length);
static int32_t xml_process_data(XML_PARSER_STRUCT *parser, int32_t length);
static int32_t xml_process_cdata(XML_PARSER_STRUCT *parser, int32_t length);
static int32_t xml_process_entity(XML_PARSER_STRUCT *parser, char *start, int32_t length);
static int32_t xml_process_start_tag_internal(XML_PARSER_STRUCT *parser,
        int32_t start,
        int32_t length,
        char **p_name,
        int32_t *empty);
static int32_t xml_process_start_tag(XML_PARSER_STRUCT *parser, int32_t start, int32_t length);
static int32_t xml_process_end_tag(XML_PARSER_STRUCT *parser, int32_t start, int32_t length);
static char *xml_find_min_ptr(char *a, char *b, char *c);
static int32_t xml_scan_named_char(XML_PARSER_STRUCT *parser, FS_HANDLE *handle,
                                   char **dest, char *original, int32_t length);
static char *xml_replace_entity(XML_PARSER_STRUCT *parser, char *start, int32_t length);
static char *xml_find_min_entity_mark(char *string, char *end);
static char *xml_find_valid_entity_start(char *start, char *end);
static char *xml_is_pre_entity(char *start, char *end);
static void xml_get_entity_value(XML_PARSER_STRUCT *parser,
                                 char *data,
                                 char *start,
                                 char *end);
static void xml_save_content_data(XML_PARSER_STRUCT *parser, FS_HANDLE *handle,
                                  char **dest, const char *data, int32_t len_data);
static int32_t xml_reset_end_position(char *data, int32_t length);
static int32_t xml_filter_comment_pi(char *data, const char *filter_start, const char *filter_end);
static int32_t xml_check_attr_duplicate(char *attr, char **attrlist, int32_t index);
static int32_t xml_attr_handle_entity(XML_PARSER_STRUCT *parser,
                                      char *dest,
                                      char *original,
                                      int32_t length);
static int32_t xml_push_start_tag(XML_PARSER_STRUCT *parser, char *tagname);
static void xml_pop_start_tag(XML_PARSER_STRUCT *parser);
static void xml_free_tag_list(XML_PARSER_STRUCT *parser);
static void xml_set_error (XML_PARSER_STRUCT *parser, int32_t error, char *err_str);
static int32_t xml_read_xmltext(XML_PARSER_STRUCT *parser,
                                char *buffer,
                                int32_t from,
                                int32_t size,
                                int32_t *length);
static int32_t xml_seek_xmltext(XML_PARSER_STRUCT *parser, int32_t offset, int32_t ref);
static int32_t xml_get_curr_position(XML_PARSER_STRUCT *parser);
static void xml_to_lower(uint8_t *str);
static char *xml_extract_token(XML_PARSER_STRUCT *parser, char *string, char *sep);


int32_t empty_func(void)
{
    return 0;
}

/*****************************************************************************
 * FUNCTION
 * vir_seek_file
 * DESCRIPTION
 *  This function is used to seek vir file
 * PARAMETERS
 *  parser      [IN]        Global parser struct
 *  offset        [IN]        Pointer of buf
 * RETURNS
 *  int32_t
 *****************************************************************************/
static int32_t vir_seek_file(XML_PARSER_STRUCT *parser, int32_t offset, int32_t ref)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    virtual_file_buf_struct *fileptr;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    fileptr = &(parser->virfileptr);
    fileptr->realfileseekpos = offset;
    return 0;
}


/*****************************************************************************
 * FUNCTION
 *  vir_read_file
 * DESCRIPTION
 *  This function is used to read vir file
 * PARAMETERS
 *  parser      [IN]        Global parser struct
 *  buf         [IN]        Pointer of buf
 *  size        [IN]        The size of data read
 *  len         [IN]        The real length of data read
 * RETURNS
 *  void
 *****************************************************************************/
static int32_t vir_read_file(XML_PARSER_STRUCT *parser, void *buf, uint32_t size, uint32_t *len)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    virtual_file_buf_struct *fileptr;
    int32_t  ret, reserdatalen, readlen = 0;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    fileptr = &(parser->virfileptr);
    reserdatalen = fileptr->virfilelen - (fileptr->realfileseekpos - fileptr->realfilebeginpos);

    if (reserdatalen >= size) {
        fileptr->virfileseek = fileptr->realfileseekpos - fileptr->realfilebeginpos;
        memcpy(buf, fileptr->bufptr + fileptr->virfileseek, size);
        *len = size;
        fileptr->virfileseek = fileptr->realfileseekpos - fileptr->realfilebeginpos + size;
        return 1;
    } else if (fileptr->isfull != 1) {
        if ((fileptr->virfilelen == 0) || (fileptr->realfileseekpos == 0)) {
            DRM_seek_file(fileptr->fp, fileptr->realfileseekpos, FS_FILE_BEGIN);
            ret = DRM_read_file(fileptr->fp, fileptr->bufptr, fileptr->bufsize, (uint32_t *)&readlen);
            if (ret < 0) {
                return -1;
            }

            fileptr->virfilelen = readlen;

            if (readlen != fileptr->bufsize) {
                fileptr->isfull = 0;
            } else {
                fileptr->isfull = 1;
            }

            *len = (size > readlen ? readlen : size);
            memcpy(buf, fileptr->bufptr, *len);

            return 1;
        } else {
            fileptr->virfileseek = fileptr->realfileseekpos - fileptr->realfilebeginpos;
            memcpy(buf, fileptr->bufptr + fileptr->virfileseek, reserdatalen);
            *len = reserdatalen;
            return 1;
        }
    } else {
        memcpy(fileptr->bufptr, fileptr->bufptr + fileptr->bufsize - FILEBUF_BACKSIZE,  FILEBUF_BACKSIZE);
        DRM_seek_file(fileptr->fp, fileptr->realfileseekpos + reserdatalen, FS_FILE_BEGIN);
        ret = DRM_read_file(fileptr->fp, fileptr->bufptr + FILEBUF_BACKSIZE,
                            fileptr->bufsize - FILEBUF_BACKSIZE, (uint32_t *)&readlen);
        if (readlen == (fileptr->bufsize - FILEBUF_BACKSIZE)) {
            fileptr->isfull = 1;
            fileptr->realfilebeginpos += fileptr->bufsize - FILEBUF_BACKSIZE;
            fileptr->virfilelen = fileptr->bufsize;
            fileptr->virfileseek = fileptr->realfileseekpos - fileptr->realfilebeginpos;
            memcpy(buf, fileptr->bufptr + fileptr->virfileseek, size);
            *len = size;
            return 1;
        } else {
            fileptr->isfull = 0;
            fileptr->realfilebeginpos += fileptr->bufsize - FILEBUF_BACKSIZE;
            fileptr->virfilelen = FILEBUF_BACKSIZE + readlen;
            fileptr->virfileseek = fileptr->realfileseekpos - fileptr->realfilebeginpos;
            reserdatalen = fileptr->virfilelen - fileptr->virfileseek;

            if (reserdatalen >= size) {
                memcpy(buf, fileptr->bufptr + fileptr->virfileseek, size);
                *len = size;
                return 1;
            } else {
                memcpy(buf, fileptr->bufptr + fileptr->virfileseek, reserdatalen);
                *len = reserdatalen;
                return 1;
            }
        }

    }
    //return -1;
}


/*****************************************************************************
 * FUNCTION
 *  xml_free_buffer
 * DESCRIPTION
 *  This function is used to free buffer
 * PARAMETERS
 *  parser      [IN]        Global parser struct
 *  p           [IN]        Pointer to be freed
 * RETURNS
 *  void
 *****************************************************************************/
static void xml_free_buffer(XML_PARSER_STRUCT *parser, void *p)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (parser->free_buffer_ptr) {
        parser->free_buffer_ptr(p);
    } else {
#ifdef MMI_ON_WIN32
        OslMfree(p);
#else

#ifndef WIN32
        vPortFree(p);
#else
        free_ctrl_buffer(p);
#endif

#endif
    }
}


/*****************************************************************************
 * FUNCTION
 *  xml_get_buffer
 * DESCRIPTION
 *  This function is used to get buffer
 * PARAMETERS
 *  parser      [IN]        Global parser struct
 *  size        [IN]        Size of buffer
 * RETURNS
 *  void
 *****************************************************************************/
static void *xml_get_buffer(XML_PARSER_STRUCT *parser, int32_t size)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (parser->get_buffer_ptr) {
        return parser->get_buffer_ptr(size);
    } else {
#ifdef MMI_ON_WIN32
        return OslMalloc(size);
#else /* MMI_ON_WIN32 */
        if (size <= MAX_CTRL_BUFFER_SIZE) {

#ifndef WIN32
            return pvPortMalloc(size);
#else
            return get_ctrl_buffer(size);
#endif

        } else {
            return NULL;
        }
#endif /* MMI_ON_WIN32 */
    }
}


/*****************************************************************************
 * FUNCTION
 *  xml_init_folder
 * DESCRIPTION
 *  This function is used to init folder
 * PARAMETERS
 *  void
 * RETURNS
 *  bool
 *****************************************************************************/
bool xml_init_folder(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    FS_HANDLE fh;
    int32_t ret;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    fh = FS_Open((kal_uint16 *)L"Z:\\@Xml\\", FS_READ_ONLY | FS_OPEN_DIR);

    if (fh >= FS_NO_ERROR) {
        FS_Close(fh);
    } else {
        ret = FS_CreateDir((kal_uint16 *)L"Z:\\@Xml\\");

        if (ret < 0) {
            return false;
        }
    }

    return true;
}


/*****************************************************************************
 * FUNCTION
 *  xml_new_parser
 * DESCRIPTION
 *  This function is used to new parser
 * PARAMETERS
 *  parser      [IN]        Global parser struct
 * RETURNS
 *  result of parser
 *****************************************************************************/
int32_t xml_new_parser(XML_PARSER_STRUCT *parser)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    if (parser != NULL) {
        memset(parser, 0, sizeof(XML_PARSER_STRUCT));
        parser->pending_start_pos = -1;
        parser->start_doctype_hdlr = xml_doctype_start_hdlr;
        parser->end_doctype_hdlr = xml_doctype_end_hdlr;
        parser->start_elem_hdlr = xml_elem_start_hdlr;
        parser->end_elem_hdlr = xml_elem_end_hdlr;
        parser->data_hdlr = xml_general_data_hdlr;
        parser->isext = 0;
        if (xml_init_folder() == false) {
            return XML_RESULT_FAIL;
        }
        return XML_RESULT_OK;
    } else {
        return XML_RESULT_FAIL;
    }
}


/*****************************************************************************
 * FUNCTION
 *  xml_configure_memory
 * DESCRIPTION
 *  This function is used to configure memory
 * PARAMETERS
 *  parser          [IN]        Global parser struct
 *  get_ptr         [IN]        The pointer to malloc function
 *  free_ptr        [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void xml_configure_memory(XML_PARSER_STRUCT *parser, XML_malloc_buffer_ptr get_ptr, XML_free_buffer_ptr free_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    parser->get_buffer_ptr = get_ptr;
    parser->free_buffer_ptr = free_ptr;
}


/*****************************************************************************
 * FUNCTION
 *  xml_close_parser
 * DESCRIPTION
 *  this function is used to close parser
 * PARAMETERS
 *  parser      [IN]        Global parser struct
 * RETURNS
 *  void
 *****************************************************************************/
void xml_close_parser(XML_PARSER_STRUCT *parser)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (parser != NULL) {
        int32_t i;

        if (parser->source > FS_NO_ERROR) {
            DRM_close_file(parser->source);
            parser->source = FS_NO_ERROR;
        }
        if (parser->xml_text != NULL) {
            parser->xml_text = NULL;
        }
        if (parser->doct_name != NULL) {
            free_buffer(parser, parser->doct_name);
            parser->doct_name = NULL;
        }
        if (parser->err_str != NULL) {
            free_buffer(parser, parser->err_str);
            parser->err_str = NULL;
        }
        for (i = 0; i < parser->ge_count; i++) {
            if (parser->ge_data[i << 1]) {
                free_buffer(parser, parser->ge_data[i << 1]);
            }

            if (parser->ge_data[(i << 1) + 1]) {
                free_buffer(parser, parser->ge_data[(i << 1) + 1]);
            }

            parser->ge_data[i << 1] = NULL;
            parser->ge_data[(i << 1) + 1] = NULL;
        }
        for (i = 0; i < parser->pe_count; i++) {
            if (parser->pe_data[i << 1]) {
                free_buffer(parser, parser->pe_data[i << 1]);
            }
            if (parser->pe_data[(i << 1) + 1]) {
                free_buffer(parser, parser->pe_data[(i << 1) + 1]);
            }

            parser->pe_data[i << 1] = NULL;
            parser->pe_data[(i << 1) + 1] = NULL;
        }
        xml_free_tag_list(parser);
        xml_reset_parser(parser);
    }
}


/*****************************************************************************
 * FUNCTION
 *  xml_reset_parser
 * DESCRIPTION
 *  This function is used to reset parser
 * PARAMETERS
 *  parser      [IN]        Global parser struct
 * RETURNS
 *  void
 *****************************************************************************/
static void xml_reset_parser(XML_PARSER_STRUCT *parser)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    parser->prolog_depth = 0;
    parser->curr_depth = 0;
    parser->prolog_done = 0;
    parser->pos = 0;
    parser->pending_start_pos = -1;
    parser->data_start_pos = 0;
    parser->pending_type = 0;
    parser->stop = 0;
    parser->pause = 0;
    parser->encoding = 0;
    parser->error = 0;
    parser->need_end_tag = 0;
    parser->ge_count = 0;
    parser->pe_count = 0;
}


/*****************************************************************************
 * FUNCTION
 *  xml_register_doctype_handler
 * DESCRIPTION
 *  This functin is used to register document handler
 * PARAMETERS
 *  parser          [OUT]       Global parser struct
 *  start_hdlr      [IN]        Function handler to handle start doc data
 *  end_hdlr        [IN]        Function handler to handle end doc data
 * RETURNS
 *  void
 *****************************************************************************/
void xml_register_doctype_handler(
    XML_PARSER_STRUCT *parser,
    XML_start_doctype_hdlr start_hdlr,
    XML_end_doctype_hdlr end_hdlr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (parser != NULL) {
        parser->start_doctype_hdlr = (start_hdlr == NULL) ? xml_doctype_start_hdlr : start_hdlr;
        parser->end_doctype_hdlr = (end_hdlr == NULL) ? xml_doctype_end_hdlr : end_hdlr;
    }
}


/*****************************************************************************
 * FUNCTION
 *  xml_register_element_handler
 * DESCRIPTION
 *  This function is used to register element handler
 * PARAMETERS
 *  parser          [OUT]       Global parser struct
 *  start_hdlr      [IN]        Function handler to handle start element data
 *  end_hdlr        [IN]        Function handler to handle end element data
 * RETURNS
 *  void
 *****************************************************************************/
void xml_register_element_handler(
    XML_PARSER_STRUCT *parser,
    XML_start_elem_hdlr start_hdlr,
    XML_end_elem_hdlr end_hdlr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (parser != NULL) {
        parser->start_elem_hdlr = (start_hdlr == NULL) ? xml_elem_start_hdlr : start_hdlr;
        parser->end_elem_hdlr = (end_hdlr == NULL) ? xml_elem_end_hdlr : end_hdlr;
    }
}


/*****************************************************************************
 * FUNCTION
 *  xml_register_data_handler
 * DESCRIPTION
 *  This function is used to register data handler
 * PARAMETERS
 *  parser          [OUT]       Global parser struct
 *  data_hdlr       [IN]        Function handler to handle data
 * RETURNS
 *  void
 *****************************************************************************/
void xml_register_data_handler(XML_PARSER_STRUCT *parser, XML_data_hdlr data_hdlr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (parser != NULL) {
        parser->data_hdlr = (data_hdlr == NULL) ? xml_general_data_hdlr : data_hdlr;
    }
}


/*****************************************************************************
 * FUNCTION
 *  xml_elem_start_hdlr
 * DESCRIPTION
 *  This function is used as default element start handler.
 * PARAMETERS
 *  data        [IN]        Data to be handled
 *  el          [IN]        Element name
 *  attr        [IN]        Attribution list
 *  error       [IN]        Error code
 * RETURNS
 *  void
 *****************************************************************************/
void xml_elem_start_hdlr(void *data, const char *el, const char **attr, int32_t error)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int32_t i = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    while (1) {
        if (attr[i] == NULL) {
            break;
        } else {
            i += 2;
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  xml_elem_end_hdlr
 * DESCRIPTION
 *  This function is used as default element_end_handler
 * PARAMETERS
 *  data        [IN]        Data to be handled
 *  el          [IN]        Element name
 *  error       [IN]        Error code
 * RETURNS
 *  void
 *****************************************************************************/
void xml_elem_end_hdlr(void *data, const char *el, int32_t error)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

}


/*****************************************************************************
 * FUNCTION
 *  xml_doctype_start_hdlr
 * DESCRIPTION
 *  This function is used as default doctype_start_handler
 * PARAMETERS
 *  data                [IN]        Data to be handled
 *  doctypeName         [IN]        Doctype name
 *  sysid               [IN]        Sysid
 *  pubid               [IN]        Pubid
 *  internal_subset     [IN]        Internal_subset
 *  error               [IN]        Error code
 * RETURNS
 *  void
 *****************************************************************************/
void xml_doctype_start_hdlr(
    void *data,
    const char *doctypeName,
    const char **sysid,
    const char **pubid,
    int32_t internal_subset,
    int32_t error)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int32_t i = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (sysid != NULL)
        while (sysid[i]) {
            i++;
        }
    i = 0;
    if (pubid != NULL)
        while (pubid[i]) {
            i++;
        }
}


/*****************************************************************************
 * FUNCTION
 *  xml_doctype_end_hdlr
 * DESCRIPTION
 *  This function is use as default doctype_end_hdlr
 * PARAMETERS
 *  data        [IN]        Data to be handled
 *  error       [IN]        Error code
 * RETURNS
 *  void
 *****************************************************************************/
void xml_doctype_end_hdlr(void *data, int32_t error)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
}


/*****************************************************************************
 * FUNCTION
 *  xml_general_data_hdlr
 * DESCRIPTION
 *  This function is used as general data handler
 * PARAMETERS
 *  resv        [?]         [IN         no use
 *  el          [IN]        Element name
 *  data        [IN]        Data to be handled
 *  len         [IN]        Length of data
 *  error       [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void xml_general_data_hdlr(void *resv, const char *el, const char *data, int32_t len, int32_t error)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
}


/*****************************************************************************
 * FUNCTION
 *  xml_parse_buffer
 * DESCRIPTION
 *  This function is called by application to start parsing if application
 *  pass xml text with buffer
 * PARAMETERS
 *  parser          [IN/OUT]        Global parser struct
 *  xmltext         [IN]            Xml text buffer
 * RETURNS
 *  return XML_RESULT_OK if successful
 *****************************************************************************/
int32_t xml_parse_buffer(XML_PARSER_STRUCT *parser, const char *xmltext, int32_t length)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int32_t result = XML_RESULT_OK;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (xmltext == NULL) {
        return XML_RESULT_FAIL;
    }

    parser->xml_text = xmltext;
    parser->xml_len = length;
    parser->source = 0;
    if (parser->prolog_done == 0) {
        /* 1. parser prolog first */
        //LOG_I(XML, "[XML]xml_parse_buffer: Parse Prolog\n");
        result = xml_parse_prolog(parser);
        if (parser->encoding != XML_ENCODING_UTF8) {
            result = XML_RESULT_ENCODING_ERROR;
        }
    }
    /* 2. parse element */
    if (result == XML_RESULT_OK) {
        //LOG_I(XML, "[XML]xml_parse_buffer: Parse Element\n");
        result = xml_parse_element(parser);
    }

    if (parser->pause == 1) {
        return XML_RESULT_OK;
    }
    if (parser->curr_depth != 0 || parser->pending_type != XML_PENDING_NONE) {
        result = XML_RESULT_FAIL;
    }

    if (parser->stop == 1) {
        result = XML_RESULT_IS_STOPPED;
    }

    parser->xml_text = NULL;
    return result;
}


/*****************************************************************************
 * FUNCTION
 *  xml_stop_parse
 * DESCRIPTION
 *  This function is called to stop parsing
 * PARAMETERS
 *  parser      [IN/OUT]        Global parser struct
 * RETURNS
 *  void
 *****************************************************************************/
void xml_stop_parse(XML_PARSER_STRUCT *parser)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    uint8_t i;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    parser->stop = 1;
    if (parser->doct_name != NULL) {
        free_buffer(parser, parser->doct_name);
        parser->doct_name = NULL;
    }
    if (parser->err_str != NULL) {
        free_buffer(parser, parser->err_str);
        parser->err_str = NULL;
    }
    if (parser->ap_temp != NULL) {
        free_buffer(parser, parser->ap_temp);
        parser->ap_temp = NULL;
    }
    if (parser->xml_text != NULL) {
        parser->xml_text = NULL;
    }
    for (i = 0; i < parser->ge_count; i++) {
        free_buffer(parser, parser->ge_data[i << 1]);
        free_buffer(parser, parser->ge_data[(i << 1) + 1]);

        parser->ge_data[i << 1] = NULL;
        parser->ge_data[(i << 1) + 1] = NULL;
    }
    for (i = 0; i < parser->pe_count; i++) {
        free_buffer(parser, parser->pe_data[i << 1]);
        free_buffer(parser, parser->pe_data[(i << 1) + 1]);

        parser->pe_data[i << 1] = NULL;
        parser->pe_data[(i << 1) + 1] = NULL;
    }
    xml_free_tag_list(parser);
}


/*****************************************************************************
 * FUNCTION
 *  xml_pause_parse
 * DESCRIPTION
 *  This function is called to pause parsing
 * PARAMETERS
 *  parser      [IN/OUT]        Global parser struct
 * RETURNS
 *  void
 *****************************************************************************/
void xml_pause_parse(XML_PARSER_STRUCT *parser)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    parser->pause = 1;
}


/*****************************************************************************
 * FUNCTION
 *  xml_resume_parse
 * DESCRIPTION
 *  This function is called to resume parsing
 * PARAMETERS
 *  parser      [IN/OUT]        Global parser struct
 * RETURNS
 *  void
 *****************************************************************************/
int32_t xml_resume_parse(XML_PARSER_STRUCT *parser)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int32_t result = XML_RESULT_OK;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    parser->pause = 0;

    if (parser->prolog_done == 0) {
        /* 1. parser prolog first */
        result = xml_parse_prolog(parser);
        if (parser->encoding != XML_ENCODING_UTF8) {
            result = XML_RESULT_ENCODING_ERROR;
        }
    }
    /* 2. parse element */
    if (result == XML_RESULT_OK) {
        result = xml_parse_element(parser);
    }

    if (parser->pause == 1) {
        return XML_RESULT_OK;
    }
    if (parser->stop == 1 || parser->curr_depth != 0 || parser->pending_type != XML_PENDING_NONE) {
        result = XML_RESULT_FAIL;
    }

    DRM_close_file(parser->source);
    parser->source = FS_NO_ERROR;
    return result;
}


/*****************************************************************************
 * FUNCTION
 *  xml_free_parse
 * DESCRIPTION
 *  This function is called to free parser
 * PARAMETERS
 *  parser      [IN/OUT]        Global parser struct
 * RETURNS
 *  void
 *****************************************************************************/
void xml_free_parse(XML_PARSER_STRUCT *parser)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (parser != NULL) {
        parser->stop = 1;
        if (parser->doct_name != NULL) {
            free_buffer(parser, parser->doct_name);
            parser->doct_name = NULL;
        }
        if (parser->err_str != NULL) {
            free_buffer(parser, parser->err_str);
            parser->err_str = NULL;
        }
        if (parser->ap_temp != NULL) {
            free_buffer(parser, parser->ap_temp);
            parser->ap_temp = NULL;
        }
        xml_free_tag_list(parser);
    }
}


/*****************************************************************************
 * FUNCTION
 *  xml_get_err_string
 * DESCRIPTION
 *  This function is used to return error string to application
 * PARAMETERS
 *  parser      [IN]        Global parser struct
 * RETURNS
 *  return error string to application
 *****************************************************************************/
extern char *xml_get_err_string(XML_PARSER_STRUCT *parser)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (parser != NULL) {
        return parser->err_str;
    } else {
        return NULL;
    }
}


/*****************************************************************************
 * FUNCTION
 *  xml_parse_prolog
 * DESCRIPTION
 * This function is used to parse prolog
 * PARAMETERS
 *  parser      [IN/OUT]        Global parser struct
 * RETURNS
 *  void
 *****************************************************************************/
static int32_t xml_parse_prolog(XML_PARSER_STRUCT *parser)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    char *buffer;   /* [XML_BUFFER_LENGTH]; */
    char *ptr_prolog, *ptr_start, *ptr_end, *ptr_doc, *ptr_comment, *ptr_pi;
    char *ptr, *temp;
    int32_t len, ret;
    int32_t back_len = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    buffer = xml_get_buffer(parser, XML_BUFFER_LENGTH);
    if (buffer == NULL) {
        return XML_RESULT_OUT_OF_MEMORY;
    }
    //LOG_I(XML, "[XML]we have get buffer for parser!\r\n");
    while ((parser->stop == 0)  &&
            (parser->pause == 0) &&
            (xml_read_xmltext(parser, buffer, parser->pos, XML_BUFFER_LENGTH - 1, &len) >= FS_NO_ERROR)) {
        if (len == 0 || len == back_len) {
            break;    /* go to the end of xml file */
        }
        //LOG_I(XML, "[XML]the length of data read is %d\n", len);

        ptr = buffer;
        buffer[len] = 0;

        if (parser->pending_start_pos != -1) {
            /* prolog started but not finished */
            /* choose end tag to be searched */
            if (parser->pending_type == XML_PENDING_PROLOG || parser->pending_type == XML_PENDING_PI) {
                ptr_end = strstr(buffer, XML_TAG_PROLOG_END);
            } else if (parser->pending_type == XML_PENDING_COMMENT) {
                ptr_end = strstr(buffer, XML_TAG_COMMENT_END);
            } else if (parser->pending_type == XML_PENDING_DOCTYPE) {
                temp = strstr(buffer, XML_SQUARE_LEFT_BRAKET);

                if (temp != NULL) {
                    /* left braket exist, find right bracket */
                    temp = strstr(temp, XML_SQUARE_RIGHT_BRAKET);
                    /* right braket exist, find end tag */
                    if (temp != NULL) {
                        ptr_end = strstr(temp, XML_TAG_GENERAL_END);
                    } else {
                        /* DOCTYPE end tag not present, keep scanning in file */
                        parser->pending_type = XML_PENDING_DOCTYPE_MARKUP;
                        parser->pos += len;
                        back_len = 0;
                        continue;
                    }
                } else {
                    ptr_end = strstr(buffer, XML_TAG_GENERAL_END);
                }
            } else if (parser->pending_type == XML_PENDING_DOCTYPE_MARKUP) {
                temp = strstr(buffer, XML_SQUARE_RIGHT_BRAKET);
                if (temp != NULL) {
                    /* right bracket exist, find end tag */
                    ptr_end = strstr(temp, XML_TAG_GENERAL_END);
                    parser->pending_type = XML_PENDING_DOCTYPE;
                } else {
                    /* DOCTYPE end tag not present, keep scanning in file */
                    parser->pos += len;
                    back_len = 0;
                    continue;
                }
            } else {
                xml_free_buffer(parser, buffer);
                return XML_RESULT_FAIL;
            }

            if (ptr_end != NULL) {
                /* end tag present */
                if (parser->pending_type == XML_PENDING_DOCTYPE) {
                    /* DOCTYPE end tag present, prolog finished */
                    int32_t length1;

                    length1 = parser->pos - parser->pending_start_pos;
                    xml_process_doctype(parser, parser->pending_start_pos, length1 + ptr_end - ptr + 1);
                    parser->pos += ptr_end - ptr + 1;
                    parser->prolog_done = 1;
                } else if (parser->pending_type == XML_PENDING_COMMENT) {
                    parser->pos += ptr_end - ptr + 3;
                } else if (parser->pending_type == XML_PENDING_PI) {
                    parser->pos += ptr_end - ptr + 2;
                } else {
                    /* prolog end */
                    int32_t length1;

                    length1 = parser->pos - parser->pending_start_pos;
                    ret = xml_process_prolog(parser, parser->pending_start_pos, length1 + ptr_end - ptr + 1);
                    if (ret < 0) {
                        xml_free_buffer(parser, buffer);
                        return XML_RESULT_FAIL;
                    }
                    parser->pos += ptr_end - ptr + 2;
                }

                parser->pending_type = XML_PENDING_NONE;
                parser->pending_start_pos = -1;
                back_len = 0;

                if (parser->prolog_done == 1) {
                    break;
                } else {
                    continue;
                }
            } else {
                /* end tag not present, keep scanning in file */
                parser->pos += len - 2;
                back_len = 2;
                continue;
            }
        }

        /* prolog begins */
        ptr_prolog = strstr(ptr, XML_TAG_PROLOG_START);
        ptr_doc = strstr(ptr, XML_TAG_DOCTYPE_START);
        ptr_pi = strstr(ptr, XML_TAG_PI_START);
        ptr_comment = strstr(ptr, XML_TAG_COMMENT_START);
        ptr_start = strstr(ptr, XML_TAG_ELEMENT_START);

        temp = xml_find_min_ptr(ptr_prolog, ptr_doc, ptr_comment);
        if ((ptr_pi != NULL) && (ptr_pi < temp)) {
            temp = ptr_pi;
        }

        if ((temp != NULL) && (!(ptr_start != NULL && ptr_start < temp))) {
            if (temp == ptr_prolog) {
                /* prolog start tag present, scan for end tag */
                ptr_end = strstr(ptr_prolog, XML_TAG_PROLOG_END);
                if (ptr_end != NULL) {
                    /* prolog end tag present, keep scan for DOCTYPE */
                    ret = xml_process_prolog(parser, parser->pos + ptr_prolog - ptr, ptr_end - ptr_prolog + 2);
                    if (ret < 0) {
                        xml_free_buffer(parser, buffer);
                        return XML_RESULT_FAIL;
                    }
                    parser->pos += ptr_end - ptr + 2;
                    ptr = ptr_end + 2;
                    back_len = 0;
                    continue;
                } else {
                    /* prolog end tag not present, keep scanning in file */
                    parser->pending_type = XML_PENDING_PROLOG;
                    parser->pending_start_pos = parser->pos + ptr_prolog - ptr;
                    parser->pos += len - 1;
                    back_len = 1;
                    continue;
                }
            }
            if (temp == ptr_doc) {
                /* DOCTYPE start tag present, scan for end tag */
                temp = strstr(ptr_doc, XML_SQUARE_LEFT_BRAKET);
                ptr_end = strstr(ptr_doc, XML_TAG_GENERAL_END);

                if (temp != NULL) {
                    if (!(ptr_end != NULL && ptr_end < temp)) {
                        /* left braket comes before DOCTYPE end */
                        temp = strstr(ptr_doc, XML_SQUARE_RIGHT_BRAKET);
                        if (temp != NULL) {
                            ptr_end = strstr(temp, XML_TAG_GENERAL_END);
                        } else {
                            /* DOCTYPE end tag not present, keep scanning in file */
                            parser->pending_type = XML_PENDING_DOCTYPE_MARKUP;
                            parser->pending_start_pos = parser->pos + ptr_doc - ptr;
                            parser->pos += len;
                            back_len = 0;
                            continue;
                        }
                    }
                } else {
                    ptr_end = strstr(ptr_doc, XML_TAG_GENERAL_END);
                }
                if (ptr_end != NULL) {
                    /* DOCTYPE end tag present, prolog finished */
                    xml_process_doctype(parser, parser->pos + ptr_doc - ptr, ptr_end - ptr_doc + 1);
                    parser->pos += ptr_end - ptr + 1;
                    ptr = ptr_end + 1;
                    ret = xml_seek_xmltext(parser, parser->pos, FS_FILE_BEGIN);
                    if (ret < 0) {
                        xml_free_buffer(parser, buffer);
                        return XML_RESULT_FAIL;
                    }
                    parser->prolog_done = 1;
                    break;
                } else {
                    /* DOCTYPE end tag not present, keep scanning in file */
                    parser->pending_type = XML_PENDING_DOCTYPE;
                    parser->pending_start_pos = parser->pos + ptr_doc - ptr;
                    parser->pos += len;
                    back_len = 0;
                    continue;
                }
            } else if (temp == ptr_comment) {
                ptr_end = strstr(ptr_comment, XML_TAG_COMMENT_END);
                if (ptr_end != NULL) {
                    /* comment end tag present */
                    parser->pos += ptr_end - ptr + 3;
                    ptr = ptr_end + 3;
                    back_len = 0;
                    continue;
                } else {
                    /* comment end tag not present, keep scanning in file */
                    parser->pending_type = XML_PENDING_COMMENT;
                    parser->pending_start_pos = parser->pos + ptr_comment - ptr;
                    parser->pos += len - 2;
                    back_len = 2;
                    continue;
                }
            } else if (temp == ptr_pi) {
                ptr_end = strstr(ptr_pi, XML_TAG_PROLOG_END);
                if (ptr_end != NULL) {
                    /* pi end tag present */
                    parser->pos += ptr_end - ptr + 2;
                    ptr = ptr_end + 2;
                    back_len = 0;
                    continue;
                } else {
                    /* comment end tag not present, keep scanning in file */
                    parser->pending_type = XML_PENDING_PI;
                    parser->pending_start_pos = parser->pos + ptr_pi - ptr;
                    parser->pos += len - 1;
                    back_len = 1;
                    continue;
                }
            }
        } else {
            if (temp == NULL && ptr_start != NULL &&
                    (ptr_start[1] == '\0' || ptr_start[1] == '!' || ptr_start[1] == '?')) {
                parser->pos += ptr_start - ptr;
                back_len = len - (ptr_start - buffer);
                continue;
            } else {
                /* no DOCTYPE, prolog finished */
                if (parser->pos == 0 &&
                        ((ptr[0] == (char)0xff && ptr[1] == (char)0xfe) ||
                         (ptr[0] == (char)0xfe && ptr[1] == (char)0xff))) {
                    parser->encoding = XML_ENCODING_UTF16;
                }
                parser->prolog_done = 1;
                ret = xml_seek_xmltext(parser, parser->pos, FS_FILE_BEGIN);
                if (ret < 0) {
                    xml_free_buffer(parser, buffer);
                    return XML_RESULT_FAIL;
                }
                break;
            }
        }
    }   /* while */
    xml_free_buffer(parser, buffer);
    return XML_RESULT_OK;
}


/*****************************************************************************
 * FUNCTION
 *  xml_parse_element
 * DESCRIPTION
 *  This function is used to parser element if application pass xml text with
 *  file
 * PARAMETERS
 *  parser      [IN/OUT]        Global parser struct
 * RETURNS
 *  void
 *****************************************************************************/
static int32_t xml_parse_element(XML_PARSER_STRUCT *parser)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    char *buffer;   /* buffer[XML_BUFFER_LENGTH]; */
    char *ptr_start, *ptr_end, *ptr_comment, *ptr_pi, *ptr_cdata;
    char *ptr, *temp;
    int32_t len;
    int32_t back_len = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    buffer = xml_get_buffer(parser, XML_BUFFER_LENGTH);
    if (buffer == NULL) {
        return XML_RESULT_OUT_OF_MEMORY;
    }

    while ((parser->stop == 0) &&
            (parser->pause == 0) &&
            (xml_read_xmltext(parser, buffer, parser->pos, XML_BUFFER_LENGTH - 1, &len) >= FS_NO_ERROR)) {
        if (len == 0 || len == back_len) {
            break;    /* when len == 0/back_len, it means the end of xml file */
        }

        ptr = buffer;
        buffer[len] = 0;

        /* end of file */
        if (len == 0) {
            xml_free_buffer(parser, buffer);
            return XML_RESULT_OK;
        }

        if (parser->pending_start_pos != -1) {
            if (parser->pending_type == XML_PENDING_COMMENT) {
                ptr_end = strstr(buffer, XML_TAG_COMMENT_END);
            } else if (parser->pending_type == XML_PENDING_PI) {
                ptr_end = strstr(buffer, XML_TAG_PROLOG_END);
            } else if (parser->pending_type == XML_PENDING_CDATA) {
                ptr_end = strstr(buffer, XML_TAG_CDATA_END);
            } else if (parser->pending_type == XML_PENDING_START_ELEMENT ||
                       parser->pending_type == XML_PENDING_END_ELEMENT) {
                ptr_end = strstr(buffer, XML_TAG_GENERAL_END);
            } else {
                xml_free_buffer(parser, buffer);
                return XML_RESULT_FAIL;
            }

            if (ptr_end != NULL) {
                if (parser->pending_type == XML_PENDING_START_ELEMENT) {
                    /* call back to start hdlr */
                    int32_t length1, result;

                    length1 = parser->pos - parser->pending_start_pos;
                    result = xml_process_start_tag(parser, parser->pending_start_pos, length1 + ptr_end - ptr + 1);
                    if (result != XML_RESULT_OK) {
                        xml_free_buffer(parser, buffer);
                        return result;
                    }
                    parser->data_start_pos = parser->pos + (ptr_end - ptr + 1);
                    parser->pos += ptr_end - ptr + 1;
                } else if (parser->pending_type == XML_PENDING_END_ELEMENT) {
                    int32_t length1, result;

                    length1 = parser->pos - parser->pending_start_pos;
                    result = xml_process_end_tag(parser, parser->pending_start_pos, length1 + ptr_end - ptr + 1);
                    if (result != XML_RESULT_OK) {
                        xml_free_buffer(parser, buffer);
                        return result;
                    }
                    parser->pos += ptr_end - ptr + 1;
                    parser->data_start_pos = parser->pos;   /* reset data_start_pos after xml_process_end_tag */
                } else if (parser->pending_type == XML_PENDING_PI) {
                    parser->pos += ptr_end - ptr + 2;
                } else if (parser->pending_type == XML_PENDING_COMMENT ) {
                    parser->pos += ptr_end - ptr + 3;
                } else if (parser->pending_type == XML_PENDING_CDATA) {
                    parser->pos += ptr_end - ptr + 3;
                    parser->cdata_start_pos = parser->pending_start_pos;
                    parser->cdata_len = parser->pos - parser->pending_start_pos;
                }

                parser->pending_type = XML_PENDING_NONE;
                parser->pending_start_pos = -1;
                back_len = 0;
                continue;
            } else {
                parser->pos += len - 2;
                back_len = 2;
                continue;
            }
        }

        ptr_comment = strstr(buffer, XML_TAG_COMMENT_START);
        ptr_pi = strstr(buffer, XML_TAG_PI_START);
        ptr_cdata = strstr(buffer, XML_TAG_CDATA_START);
        ptr_end = strstr(buffer, XML_TAG_ELEMENT_END);
        ptr_start = strstr(buffer, XML_TAG_ELEMENT_START);

        if ((ptr_start == ptr_comment) || (ptr_start == ptr_end) || (ptr_start == ptr_pi) || (ptr_start == ptr_cdata)) {
            ptr_start = NULL;
        }

        if (ptr_start != NULL &&
                (ptr_start[1] == '\0' || ptr_start[1] == '!' || ptr_start[1] == '?')) {
            parser->pos += ptr_start - ptr;
            back_len = len - (ptr_start - buffer);
            continue;
        }

        temp = xml_find_min_ptr(ptr_comment, ptr_start, ptr_end);
        temp = xml_find_min_ptr(temp, ptr_pi, ptr_cdata);

        if (temp != NULL) {
            if (temp[1] == 0) {
                /* end of file, no need to loop again */
                if (temp == ptr) {
                    xml_free_buffer(parser, buffer);
                    return XML_RESULT_OK;
                }
                parser->pos += temp - ptr;
                back_len = len - (temp - buffer);
                continue;
            }
            if (temp == ptr_start) {
                /* found start tag first, process start tag */
                ptr_end = strstr(ptr_start, XML_TAG_GENERAL_END);

                if (ptr_end != NULL) {
                    int32_t result, length;

                    /*
                                 * first we should handle data before start tag:
                                 * * case 1: data between parent start tag and child start tag
                                 * * case 2: data between end tag and start tag of next element
                                 */
                    if (parser->need_end_tag == 1) {
                        length = parser->pos - parser->data_start_pos + ptr_start - ptr;
                        if (length > 0) {
                            result = xml_process_data(parser, length);
                            if (result < 0) {
                                xml_free_buffer(parser, buffer);
                                return result;
                            }
                        }
                    }
                    result = xml_process_start_tag(parser, parser->pos + ptr_start - ptr, ptr_end - ptr_start + 1);
                    if (result != XML_RESULT_OK) {
                        xml_free_buffer(parser, buffer);
                        return result;
                    }
                    parser->pos += ptr_end - ptr + 1;
                    parser->data_start_pos = parser->pos;
                    back_len = 0;
                    continue;
                } else {
                    /* need to read more till start end */
                    parser->pending_type = XML_PENDING_START_ELEMENT;
                    parser->pending_start_pos = (parser->pos + ptr_start - ptr);
                    parser->pos += len;
                    back_len = 0;
                    continue;
                }
            } else if (temp == ptr_end) {
                int32_t result;

                /* found end tag first, process end tag */
                ptr_start = ptr_end;
                /* data callback first */
                if (parser->data_start_pos > 0 && (parser->pos + (ptr_start - ptr)) > parser->data_start_pos) {
                    result = xml_process_data(parser, (parser->pos + ptr_start - ptr) - parser->data_start_pos);
                    if (result != XML_RESULT_OK) {
                        xml_free_buffer(parser, buffer);
                        return result;
                    }
                }
                parser->data_start_pos = -1;
                ptr_end = strstr(ptr_start, XML_TAG_GENERAL_END);
                if (ptr_end != NULL) {
                    /* complete end tag */
                    parser->offset = parser->pos + ptr_end - ptr + 1;
                    result = xml_process_end_tag(parser, parser->pos + ptr_start - ptr, ptr_end - ptr_start + 1);
                    if (result != XML_RESULT_OK) {
                        xml_free_buffer(parser, buffer);
                        return result;
                    }
                    parser->pos += ptr_end - ptr + 1;
                    /*
                     * reset data_start_pos after xml_process_end_tag fot such case:
                     * * 1) data between end tag and start tag;
                     * * 2) data between end tag and end tag;
                     */
                    parser->data_start_pos = parser->pos;
                    back_len = 0;
                    continue;
                } else {
                    /* need to read more till end end */
                    parser->pending_type = XML_PENDING_END_ELEMENT;
                    parser->pending_start_pos = (parser->pos + ptr_start - ptr);
                    parser->pos += len;
                    back_len = 0;
                    continue;
                }
            } else if (temp == ptr_pi) {
                /* found pi tag first, skip till pi end */
                ptr_end = strstr(ptr_pi, XML_TAG_PROLOG_END);
                if (ptr_end != NULL) {
                    /* complete pi */
                    parser->pos += ptr_end - ptr + 2;
                    back_len = 0;
                    continue;
                } else {
                    /* need to read more till pi end */
                    parser->pending_type = XML_PENDING_PI;
                    parser->pending_start_pos = (parser->pos + ptr_pi - ptr);
                    parser->pos += len - 1;
                    back_len = 1;
                    continue;
                }
            } else if (temp == ptr_cdata) {
                /* found cdata tag first, skip till cdata end */
                ptr_end = strstr(ptr_cdata, XML_TAG_CDATA_END);
                if (ptr_end != NULL) {
                    /* complete cdata */
                    parser->cdata_start_pos = (parser->pos + ptr_cdata - ptr);
                    parser->cdata_len = ptr_end - ptr_cdata + 3;
                    parser->pos += ptr_end - ptr + 3;
                    back_len = 0;
                    continue;
                } else {
                    /* need to read more till cdata end */
                    parser->pending_type = XML_PENDING_CDATA;
                    parser->pending_start_pos = (parser->pos + ptr_cdata - ptr);
                    parser->pos += len - 2; /* it is for end position is between "]]>" */
                    back_len = 2;
                    continue;
                }
            } else {
                if (ptr_comment != NULL) {
                    /* found comment tag first, skip till comment end */
                    ptr_end = strstr(ptr_comment, XML_TAG_COMMENT_END);
                    if (ptr_end != NULL) {
                        /* complete comment */
                        parser->pos += ptr_end - ptr + 3;
                        back_len = 0;
                        continue;
                    } else {
                        /* need to read more till comment end */
                        parser->pending_type = XML_PENDING_COMMENT;
                        parser->pending_start_pos = (parser->pos + ptr_comment - ptr);
                        parser->pos += len - 2; /* it is for end position is between "-->" */
                        back_len = 2;
                        continue;
                    }
                }
            }
        } else {
            /* all data, no tag */
            parser->pos += len;
            back_len = 0;
            continue;
        }
    }   /* while */

    xml_free_buffer(parser, buffer);
    return XML_RESULT_OK;
}


/*****************************************************************************
 * FUNCTION
 *  xml_process_prolog
 * DESCRIPTION
 *  This function is used to process prolog
 * PARAMETERS
 *  parser      [IN]        Global parser struct
 *  start        [IN]        Start position of prolog in xml file
 *  length      [IN]        Length of prolog
 * RETURNS
 *  return XML_RESULT_OK if OK
 *****************************************************************************/
static int32_t xml_process_prolog(XML_PARSER_STRUCT *parser, int32_t start, int32_t length)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    char *temp, *ptr;
    uint32_t fp;
    int32_t len, ret;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    temp = get_buffer(parser, length + 1);
    if (temp == NULL) {
        return XML_RESULT_OUT_OF_MEMORY;
    }

    fp = xml_get_curr_position(parser);

    ret = xml_read_xmltext(parser, temp, start, length, &len);
    if (ret < 0) {
        free_buffer(parser, temp);
        return XML_RESULT_FAIL;
    }
    temp[len] = 0;

    ptr = strstr(temp, XML_TAG_ENCODING);

    if (ptr != NULL) {
        while ((ptr < temp + length) && (*ptr != '\'') && (*ptr != '\"')) {
            ptr++;
        }
        ptr++;
        xml_to_lower((uint8_t *) ptr);
        if ((strncmp(ptr, XML_TAG_ENCODING_UTF8, strlen(XML_TAG_ENCODING_UTF8)) == 0) ||
                (strncmp(ptr, XML_TAG_ENCODING_ASCII, strlen(XML_TAG_ENCODING_ASCII)) == 0)) {
            parser->encoding = XML_ENCODING_UTF8;
        } else if (strncmp(ptr, XML_TAG_ENCODING_UTF16, strlen(XML_TAG_ENCODING_UTF16)) == 0) {
            parser->encoding = XML_ENCODING_UTF16;
        } else if (strncmp(ptr, XML_TAG_ENCODING_UCS2, strlen(XML_TAG_ENCODING_UCS2)) == 0) {
            parser->encoding = XML_ENCODING_UCS2;
        } else if (strncmp(ptr, XML_TAG_ENCODING_UCS4, strlen(XML_TAG_ENCODING_UCS4)) == 0) {
            parser->encoding = XML_ENCODING_UCS4;
        } else {
            parser->encoding = XML_ENCODING_UNKNOWN;
        }
    } else {
        parser->encoding = XML_ENCODING_UTF8;
    }

    free_buffer(parser, temp);
    xml_seek_xmltext(parser, fp, FS_FILE_BEGIN);

    return XML_RESULT_OK;
}


/*****************************************************************************
 * FUNCTION
 *  xml_process_doctype
 * DESCRIPTION
 *  This function is used to process doctype
 * PARAMETERS
 *  parser      [IN]        Global parser struct
 *  start       [IN]        Start position of doctype in xml file
 *  length      [IN]        Length of doctype
 * RETURNS
 *  return XML_RESULT_OK if OK
 *****************************************************************************/
static int32_t xml_process_doctype(XML_PARSER_STRUCT *parser, int32_t start, int32_t length)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    char *name, *ptr, *id[10];
    uint32_t fp, len, i = 0;
    int32_t ret;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    parser->ap_temp = get_buffer(parser, length + 1);
    if (parser->ap_temp == NULL) {
        return XML_RESULT_OUT_OF_MEMORY;
    }
    parser->error = XML_NO_ERROR;

    fp = xml_get_curr_position(parser);

    ret = xml_read_xmltext(parser, parser->ap_temp, start, length, (int32_t *)&len);
    if (ret < 0) {
        free_buffer(parser, parser->ap_temp);
        parser->ap_temp = NULL;
        return XML_RESULT_FAIL;
    }
    parser->ap_temp[len] = 0;

    ptr = strstr(parser->ap_temp, "[");

    if (ptr != NULL) {
        char *ptr_entity, *ptr_end, *temp;

        /* scan for entity */
        temp = ptr + 1;
        while ((temp < parser->ap_temp + length) && ((ptr_entity = strstr(temp, XML_TAG_ENTITYTYPE_START)) != NULL)) {
            ptr_end = strstr(ptr_entity, XML_TAG_GENERAL_END);
            if (ptr_end != NULL) {
                xml_process_entity(parser, ptr_entity, ptr_end - ptr_entity + 1);
                temp = ptr_end + 1;
            } else {
                break;
            }
        }

        ptr[0] = 0;
    }

    memset(id, 0, sizeof(id));
    name = xml_extract_token(parser, parser->ap_temp + strlen(XML_TAG_DOCTYPE_START) + 1, " \r\n\t");

    if (name != NULL) {
        /* first save doct_name */
        parser->doct_name = get_buffer(parser, strlen(name) + 1);
        if (parser->doct_name == NULL) {
            return XML_RESULT_OUT_OF_MEMORY;
        }
        strcpy(parser->doct_name, name);

        while (1) {
            id[i] = xml_extract_token(parser, NULL, " \r\n\t");
            if (id[i] == NULL) {
                break;
            }
            ptr = strstr(id[i], XML_TAG_GENERAL_END);
            if (ptr != NULL) {
                ptr[0] = 0;
            }
            i++;
        }
        if (i > 0) {
            if (strcmp(id[0], "SYSTEM") == 0) {
                parser->start_doctype_hdlr(parser, (const char *)name, (const char **)id + 1, NULL, 0, parser->error);
            } else {
                if (strcmp(id[0], "PUBLIC") != 0) {
                    xml_set_error(parser, XML_ERROR_MISMATCH_TAG, id[0]);
                }
                parser->start_doctype_hdlr(parser, (const char *)name, NULL, (const char **)id + 1, 0, parser->error);
            }
        } else {
            parser->start_doctype_hdlr(parser, name, NULL, NULL, 0, parser->error);
        }
        if (parser->stop == 0) {
            parser->end_doctype_hdlr(parser, parser->error);
        }
    }

    if (parser->stop == 0) {
        free_buffer(parser, parser->ap_temp);
        parser->ap_temp = NULL;
        xml_seek_xmltext(parser, fp, FS_FILE_BEGIN);
    }
    return XML_RESULT_OK;
}


/*****************************************************************************
 * FUNCTION
 *  xml_process_data
 * DESCRIPTION
 *  This function is used to process data
 * PARAMETERS
 *  parser      [IN]        Global parser struct
 *  length      [IN]        Length of data
 * RETURNS
 *  if successful, return XML_RESULT_OK
 *****************************************************************************/
static int32_t xml_process_data(XML_PARSER_STRUCT *parser, int32_t length)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    char *data, *dest = NULL;
    uint32_t fp, position;
    int32_t ret, i = 0;
    FS_HANDLE handle = 0;
    int32_t result = XML_RESULT_OK;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //LOG_I(XML, "[XML]xml_process_data: length = %d\n", length);

    /* if data is one cdata section, call xml_process_cdata */
    if ((parser->data_start_pos == parser->cdata_start_pos) && (parser->cdata_len == length)) {
        ret = xml_process_cdata(parser, parser->cdata_len);
        parser->cdata_len = 0;
        parser->cdata_start_pos = -1;
        return ret;
    }

    fp = xml_get_curr_position(parser);

    parser->error = XML_NO_ERROR;
    parser->ap_temp = get_buffer(parser, MAX_CTRL_BUFFER_SIZE);
    if (parser->ap_temp == NULL) {
        return XML_RESULT_OUT_OF_MEMORY;
    }

    /* dest is used to save result after filter_comment/pi, entity replace, and cdata */
    dest = get_buffer(parser, MAX_CTRL_BUFFER_SIZE);
    if (dest == NULL) {
        free_buffer(parser, parser->ap_temp);
        return XML_RESULT_OUT_OF_MEMORY;
    }
    dest[0] = 0;

    i = length;
    position = parser->data_start_pos;
    while (i > 0) {   /* i is used as the rest length of content data */
        int32_t len_data, len_read;

        len_read = 0;

        /* for each loop, if i > 2047, we read 2047, else we read i */
        if (i >= (MAX_CTRL_BUFFER_SIZE - 1)) {
            ret = xml_read_xmltext(parser, parser->ap_temp, position, (MAX_CTRL_BUFFER_SIZE - 1), &len_read);
        } else {
            ret = xml_read_xmltext(parser, parser->ap_temp, position, i, &len_read);
        }
        if (ret < 0) {
            free_buffer(parser, parser->ap_temp);
            parser->ap_temp = NULL;
            free_buffer(parser, dest);
            return XML_RESULT_FAIL;
        }
        position += len_read;
        parser->ap_temp[len_read] = 0;

        /*
             * if PI/Comment/Entity/CDATA is overlap one parser->ap_temp, we should put the end of
             * parser->ap_temp to the (start-1) of PI/Comment/Entity/CDATA
             */
        if (len_read == (MAX_CTRL_BUFFER_SIZE - 1)) {
            len_data = xml_reset_end_position(parser->ap_temp, len_read);
            position -= len_read - len_data;
        } else {
            len_data = len_read;
        }
        i -= len_data;

        data = parser->ap_temp;

        /* skip comment */
        len_data = xml_filter_comment_pi(data, XML_TAG_COMMENT_START, XML_TAG_COMMENT_END);
        /* skip PI */
        len_data = xml_filter_comment_pi(data, XML_TAG_PI_START, XML_TAG_PROLOG_END);

        /* handle entity and CDATA */
        if (len_data > 0) {
            char *str1, *str2, *str3, *string;

            string = data;

            /* eg: string "abc&e1;<![CDATA[&e2;]]>xyz<![CDATA[XXX]]>mnl" */
            while ((str1 = strstr(string, XML_TAG_CDATA_START)) != NULL) {        /* str1: "<![CDATA[&e2;]]>xyz<![CDATA[XXX]]>mnl" */
                str2 = strstr(string, XML_TAG_CDATA_END);   /* str2: " ]]>xyz<![CDATA[XXX]]>mnl" */
                if (str2 != NULL) {
                    /* entity handle before CDATA section */
                    xml_scan_named_char(parser, &handle, &dest, string, (str1 - string));
                    /*
                     * handle data in CDATA section: entity in CDATA section is no need to be replaced,
                     * so we can save them directly.
                     */
                    str3 = str1 + 9;    /* 9 means : "<![CDATA[" */
                    xml_save_content_data(parser, &handle, &dest, str3, (uint16_t) (str2 - str3));
                    string = str2 + 3;  /* 3 means: "]]>"; string: "xyz<![CDATA[XXX]]>mnl" */
                } else {
                    break;
                }
            }
            /*
                     * it will come here in following cases:
                     * 1) cannot find XML_TAG_CDATA_START in string, eg: when rest string is "mnl"
                     * 2) find XML_TAG_CDATA_START, but don't find XML_TAG_CDATA_END, treat it as common data
                     * 3) no CDATA section in whole original content data
                     */
            xml_scan_named_char(parser, &handle, &dest, string, strlen(string));
        } else {
            continue;
        }
    }

    /* callback data handler */
    if (dest != NULL) {
        char *ret_data = dest;

        i = strlen(dest);
        /* filter space in begin and end */
        while (ret_data[0] == '\n' || ret_data[0] == '\r' || ret_data[0] == ' ') {
            ret_data++;
            i--;
        }
        while (i > 0 && (ret_data[i - 1] == ' ' || ret_data[i - 1] == '\n' || ret_data[i - 1] == '\r')) {
            i--;
        }
        if (i > 0) {
            ret_data[i] = '\0';
        }
        if (i > 0) {
            if (parser->curr_depth > 0) {
                parser->data_hdlr(parser, parser->elem[parser->curr_depth - 1], ret_data, i, parser->error);
                //LOG_I(XML, "[XML]Data_hdlr: %s\n", ret_data);
            } else {
                xml_set_error(parser, XML_ERROR_MISMATCH_TAG, "error");
                parser->data_hdlr(parser, NULL, ret_data, i, parser->error);
                result = XML_RESULT_FAIL;
            }
        }
        free_buffer(parser, dest);
    } else {
        FS_Close(handle);
        parser->data_hdlr(parser, parser->elem[parser->curr_depth - 1], (char *)L"Z:\\@Xml\\xml_temp", 0, parser->error);
    }

    /* free buffer */
    if (parser->stop == 0) {
        free_buffer(parser, parser->ap_temp);
        parser->ap_temp = NULL;
        xml_seek_xmltext(parser, fp, FS_FILE_BEGIN);
    }
    parser->need_end_tag = 0;

    return result;

}


/*****************************************************************************
 * FUNCTION
 *  xml_process_cdata
 * DESCRIPTION
 *  This function is used to process lage cdata over 2k, it is specially for sycml
 * PARAMETERS
 *  parser      [IN]        Global parser struct
 *  length      [IN]        Length of data
 * RETURNS
 *  if successful, return XML_RESULT_OK
 *****************************************************************************/
static int32_t xml_process_cdata(XML_PARSER_STRUCT *parser, int32_t length)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    uint32_t fp, position;
    int32_t ret, i = 0;
    FS_HANDLE handle = -1;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //LOG_I(XML, "[XML]xml_process_cdata: %d\n", length);
    fp = xml_get_curr_position(parser);
    parser->error = XML_NO_ERROR;

    parser->ap_temp = get_buffer(parser, MAX_CTRL_BUFFER_SIZE);
    if (parser->ap_temp == NULL) {
        return XML_RESULT_OUT_OF_MEMORY;
    }

    /* throw off XML_TAG_CDATA_START and XML_TAG_CDATA_END */
    position = parser->data_start_pos + strlen(XML_TAG_CDATA_START);
    i = length - strlen(XML_TAG_CDATA_START) - strlen(XML_TAG_CDATA_END);

    if (i >=  (MAX_CTRL_BUFFER_SIZE - 1)) {
        handle = FS_Open((const WCHAR *)L"Z:\\@Xml\\xml_temp", FS_READ_WRITE | FS_CREATE_ALWAYS);
        if (handle < 0) {
            free_buffer(parser, parser->ap_temp);
            parser->ap_temp = NULL;
            return XML_RESULT_FAIL;
        }
    }

    while (i > 0) {   /* i is used as the rest length of content data */
        int32_t len_read = 0;

        /* for each loop, if i > 2047, we read 2047, else we read i */
        if (i >= (MAX_CTRL_BUFFER_SIZE - 1)) {
            ret = xml_read_xmltext(parser, parser->ap_temp, position, (MAX_CTRL_BUFFER_SIZE - 1), &len_read);
        } else {
            ret = xml_read_xmltext(parser, parser->ap_temp, position, i, &len_read);
        }
        if (ret < 0) {
            free_buffer(parser, parser->ap_temp);
            parser->ap_temp = NULL;
            return XML_RESULT_FAIL;
        }

        parser->ap_temp[len_read] = 0;

        if (handle == -1) {
            break;
        }

        ret = FS_Write(handle, parser->ap_temp, len_read, NULL);
        if (ret < 0) {
            free_buffer(parser, parser->ap_temp);
            parser->ap_temp = NULL;
            return XML_RESULT_FAIL;
        }

        i -= len_read;
        position += len_read;
    }

    if (handle == -1) {
        parser->data_hdlr(parser, parser->elem[parser->curr_depth - 1], parser->ap_temp, i, parser->error);
        //LOG_I(XML, "[XML]Data_hdle: %s\n", parser->ap_temp);
    } else {
        FS_Close(handle);
        parser->data_hdlr(parser, parser->elem[parser->curr_depth - 1], (char *)L"Z:\\@Xml\\xml_temp", 0, parser->error);
    }

    /* free buffer */
    if (parser->stop == 0) {
        free_buffer(parser, parser->ap_temp);
        parser->ap_temp = NULL;
        xml_seek_xmltext(parser, fp, FS_FILE_BEGIN);
    }
    parser->need_end_tag = 0;

    return XML_RESULT_OK;

}


/*****************************************************************************
 * FUNCTION
 *  xml_process_entity
 * DESCRIPTION
 *  This function is used to process entity
 * PARAMETERS
 *  parser      [IN]        Global parser struct
 *  start       [IN]        The start pointer of entity definition
 *  length      [IN]        The length of entity definition
 * RETURNS
 *  if successful, return XML_RESULT_OK
 *****************************************************************************/
static int32_t xml_process_entity(XML_PARSER_STRUCT *parser, char *start, int32_t length)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    char *name, *pe_mark, *value, *tmp, *ptr;
    int32_t i;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    parser->error = XML_NO_ERROR;
    ptr = strstr(start, XML_SPACE);
    if (ptr == NULL) {  /* no entity declare */
        return XML_RESULT_OK;
    }

    /* first, find entity name */
    if (((pe_mark = strstr(ptr, "%")) != NULL) && (pe_mark < start + length)) {
        /* parameter entity */
        if (parser->ge_count > XML_MAX_ENTITY_COUNT) {
            return XML_RESULT_OK;
        }
        name = xml_extract_token(parser, pe_mark + 1, " \r\n\t");
        i = parser->pe_count << 1;
    } else {
        /* general entity */
        if (parser->pe_count > XML_MAX_ENTITY_COUNT) {
            return XML_RESULT_OK;
        }
        name = xml_extract_token(parser, ptr + 1, " \r\n\t");
        i = parser->ge_count << 1;
    }
    if (name != NULL) {
        char *ptr1, *ptr2;

        /* extract entity name according " " */
        ptr1 = strstr(name + strlen(name) + 1, "\"");   /* NULL is set in the end of name after strtok is called */
        if (ptr1 > start + length) {
            ptr1 = NULL;    /* value is between start and (start + length) */
        }
        if (ptr1 == NULL) {
            /* error on entity definition */
            xml_set_error(parser, XML_ERROR_ENTITY_DEFINITION, name);
            return XML_RESULT_FAIL;
        }
        ptr1++;
        ptr2 = strstr(ptr1, "\"");
        if (ptr2 > start + length) {
            ptr2 = NULL;    /* end is between start and (start + length) */
        }
        if (ptr2 == NULL) {
            /* error on entity definition */
            xml_set_error(parser, XML_ERROR_ENTITY_DEFINITION, name);
            return XML_RESULT_FAIL;
        }

        /* value is used to save entity value */
        value = get_buffer(parser, XML_BUFFER_LENGTH);
        if (value == NULL) {
            return XML_RESULT_OUT_OF_MEMORY;
        }
        memcpy(value, ptr1, ptr2 - ptr1);
        value[ptr2 - ptr1] = 0;

        /*
             * handle entity reference in entity value, for example: value: "xyz&ge;sdf&pe;"
             * * first loop: ptr1: "&ge;sdf&pe;"    ptr2:";sdf&pe;"  copy ptr2 to rest
             * * "&ge;" is defined as "general entity"
             * * result after first loop : value : "xyzgeneral entitysdf&pe;"
             */

        ptr1 = value;
        tmp = get_buffer(parser, XML_BUFFER_LENGTH);
        if (tmp == NULL) {
            return XML_RESULT_OUT_OF_MEMORY;
        }
        tmp[0] = 0;/* temp is used to save entity value of entity reference in entity declare */

        while ((ptr1 = xml_find_min_entity_mark(ptr1, value + strlen(value))) != NULL) {
            char *rest;
            int32_t len;

            ptr2 = strstr(ptr1, ";");
            if (ptr2 == NULL) {
                break;
            }
            ptr1 = xml_find_valid_entity_start(ptr1, ptr2);
            xml_get_entity_value(parser, tmp, ptr1, ptr2);

            ptr1[0] = 0;
            ptr2++;

            /* in general, the length of entity value wouldn't exceed XML_BUFFER_LENGTH */
            len = strlen(value) + strlen(tmp);
            assert((len + strlen(ptr2)) < XML_BUFFER_LENGTH);

            rest = get_buffer(parser, strlen(ptr2) + 1);
            if (rest == NULL) {
                return XML_RESULT_OUT_OF_MEMORY;
            }
            strcpy(rest, ptr2);

            strcat(value, tmp);
            strcat(value, rest);
            free_buffer(parser, rest);
            tmp[0] = 0;

            ptr1 = value + len; /* for next loop, set ptr1 the end of temp */
        }
        free_buffer(parser, tmp);

        /* store entity value */
        if (value != NULL) {
            char *entity_name, *entity_value;

            entity_name = get_buffer(parser, strlen(name) + 1);
            if (entity_name == NULL) {
                return XML_RESULT_OUT_OF_MEMORY;
            }
            entity_value = get_buffer(parser, strlen(value) + 1);
            if (entity_value == NULL) {
                return XML_RESULT_OUT_OF_MEMORY;
            }
            strcpy(entity_name, name);
            strcpy(entity_value, value);

            if ((pe_mark != NULL) && (pe_mark < start + length)) {       /* parameter entity */
                parser->pe_data[i] = entity_name;
                parser->pe_data[i + 1] = entity_value;
                parser->pe_count++;
            } else {
                /* general entity */
                parser->ge_data[i] = entity_name;
                parser->ge_data[i + 1] = entity_value;
                parser->ge_count++;
            }
        }
        free_buffer(parser, value);
    }
    return XML_RESULT_OK;
}


/*****************************************************************************
 * FUNCTION
 *  xml_process_start_tag
 * DESCRIPTION
 *  This function is used to process start tag
 * PARAMETERS
 *  parser      [IN]        Global parser struct
 *  start       [IN]        The start position of start tag in xml file
 *  length      [IN]        The length of start tag
 * RETURNS
 *  if successful, return XML_RESULT_OK
 *****************************************************************************/
static int32_t xml_process_start_tag(XML_PARSER_STRUCT *parser, int32_t start, int32_t length)
{
    int32_t empty = 0;
    uint32_t result;
    char *name = NULL;

    //LOG_I(XML, "[XML]xml_process_start_tag: length = %d\n", length);

    result = xml_process_start_tag_internal(parser, start, length, &name, &empty);


    if (result == XML_RESULT_OK && empty == 1 && parser->stop == 0) {
        xml_pop_start_tag(parser);
        parser->end_elem_hdlr(parser, name, parser->error);

        if (name != NULL) {
            free_buffer(parser, name);
        }
    }

    return result;
}


/*****************************************************************************
 * FUNCTION
 *  xml_process_start_tag
 * DESCRIPTION
 *  This function is used to process start tag
 * PARAMETERS
 *  parser      [IN]        Global parser struct
 *  start       [IN]        The start position of start tag in xml file
 *  length      [IN]        The length of start tag
 *  empty       [OUT]       Is an empty tag
 * RETURNS
 *  if successful, return XML_RESULT_OK
 *****************************************************************************/
static int32_t xml_process_start_tag_internal(XML_PARSER_STRUCT *parser,
        int32_t start,
        int32_t length,
        char **p_name,
        int32_t *empty)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    char *attr[XML_MAX_ATTR_COUNT], *ptr;
    char *dest[XML_MAX_ATTR_COUNT / 2];
    uint32_t fp, len, i = 0;
    int32_t ret;
    char *name;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    parser->ap_temp = get_buffer(parser, length + 1);
    if (parser->ap_temp == NULL) {
        return XML_RESULT_OUT_OF_MEMORY;
    }

    fp = xml_get_curr_position(parser);

    ret = xml_read_xmltext(parser, parser->ap_temp, start, length, (int32_t *)&len);
    if (ret < 0) {
        free_buffer(parser, parser->ap_temp);
        parser->ap_temp = NULL;
        return XML_RESULT_FAIL;
    }
    parser->ap_temp[len] = 0;

    memset(attr, 0, sizeof(attr));
    memset(dest, 0, sizeof(dest));
    parser->error = XML_NO_ERROR;

    ptr = strstr(parser->ap_temp, XML_TAG_SINGLE_END);
    if (ptr != NULL) {
        /* this is an emptyelemtag */
        *empty = 1;
        ptr[0] = 0;
    } else {
        parser->need_end_tag = 1;
    }

    name = xml_extract_token(parser, parser->ap_temp + 1, " \r\n\t");
    *p_name = name;

    if (name != NULL) {
        ptr = strstr(name, XML_TAG_GENERAL_END);
        if (ptr != NULL) {
            /* no attribute for such case: <a>; or <b/>, set ptr[0] = 0, it is for extract name */
            ptr[0] = 0;

        } else {
            /* with attribute */
            ptr = parser->ap_temp + 1 + strlen(name) + 1;
            while (1) {
                if (i >= XML_MAX_ATTR_COUNT - 1) {
                    break;
                }
                attr[i] = xml_extract_token(parser, ptr, " \r\n\t=");
                if ((attr[i] == NULL) || (strcmp(attr[i], XML_TAG_GENERAL_END) == 0)) {
                    attr[i] = NULL;
                    break;
                } else {
                    char *value = NULL;
                    char *attr_start;
                    char ch[2];

                    /* find the first ' or " */
                    attr_start = attr[i] + strlen(attr[i]) + 1;
                    while (*attr_start != 0 && attr_start[0] != '\'' && attr_start[0] != '\"') {
                        attr_start++;
                    }

                    ch[0] = attr_start[0];
                    ch[1] = 0;
                    if (ch[0] == '\'' || ch[0] == '\"') {
                        /* ch must be ''' or '"' */
                        attr[i + 1] = attr_start + 1;   /* attr[i] + strlen(attr[i]) + 2; */
                        value = strstr(attr[i + 1], ch);
                        if (value != NULL) {
                            if (xml_check_attr_duplicate(attr[i], attr, i) == 1) {
                                /* dupliated attribute */
                                xml_set_error(parser, XML_ERROR_DUPLICATE_ATTRIBUTE, attr[i]);
                                attr[i] = NULL;
                                attr[i + 1] = NULL;
                                ptr = value + 1;
                            } else {
                                /* correct attribute */
                                value[0] = 0;
                                ptr = value + 1;
                                i += 2;
                            }
                        } else {
                            /* single '\'' || '\"', ignore such attr, report error. */
                            xml_set_error(parser, XML_ERROR_INVALID_ATTRIBUTE, attr[i]);
                            break;
                        }
                    } else {
                        /* no find '\'' || '\"', ignore such attr, report error. */
                        xml_set_error(parser, XML_ERROR_INVALID_ATTRIBUTE, attr[i]);
                        break;
                    }

                }

            }
        }
        i = 0;
        /* handle entity reference in attribute, entity reference only exists in attr value */
        while (attr[i + 1] != NULL) {
            char *ptr_ge, *ptr_pe;

            ptr_ge = strstr(attr[i + 1], "&");
            ptr_pe = strstr(attr[i + 1], "%");

            if (((ptr_ge != NULL) && (strstr(ptr_ge, ";") != NULL) ) ||
                    ((ptr_pe != NULL) && (strstr(ptr_pe, ";") != NULL) && parser->pe_count > 0)) {
                dest[i >> 1] = get_buffer(parser, MAX_CTRL_BUFFER_SIZE);
                if (dest[i >> 1] == NULL) {
                    return XML_RESULT_OUT_OF_MEMORY;
                }
                *(dest[i >> 1]) = 0;

                xml_attr_handle_entity(parser, dest[i >> 1], attr[i + 1], strlen(attr[i + 1]));
                attr[i + 1] = dest[i >> 1];
            }

            i += 2;
        }
        /* check if root name match with doctype name */
        if ((parser->elem[0] == NULL) && (parser->doct_name != NULL)) {
            /* only do checking when processing root element */

            if ((strcmp(name, parser->doct_name)) != 0) {
                xml_set_error(parser, XML_ERROR_MISMATCH_DOCTYPENAME_ROOTNAME, parser->doct_name);
            }
            free_buffer(parser, parser->doct_name); /* free doct_name after checking */
            parser->doct_name = NULL;
        }
        /* push start tag to tag list */
        if ((xml_push_start_tag(parser, name)) == XML_RESULT_FAIL) {
            return XML_RESULT_FAIL;
        }

        parser->start_elem_hdlr(parser, name, (const char **)attr, parser->error);
        //LOG_I(XML, "[XML]Start_elem_hdlr: %s\n", name);
    }

    for (i = 0; i < XML_MAX_ATTR_COUNT / 2; i++) {
        if (dest[i]) {
            free_buffer(parser, dest[i]);
        }
    }

    if (parser->stop == 0) {
        if (name != NULL && *empty == 1) {
            *p_name = get_buffer(parser, strlen(name) + 1);
            if (*p_name == NULL) {
                free_buffer(parser, parser->ap_temp);
                parser->ap_temp = NULL;
                return XML_RESULT_OUT_OF_MEMORY;
            }
            strcpy(*p_name, name);
        }

        free_buffer(parser, parser->ap_temp);
        parser->ap_temp = NULL;
        xml_seek_xmltext(parser, fp, FS_FILE_BEGIN);

    }

    return XML_RESULT_OK;
}


/*****************************************************************************
 * FUNCTION
 *  xml_find_min_ptr
 * DESCRIPTION
 *  this function is used to the minimal pointr
 * PARAMETERS
 *  a       [IN]        Char* pointer to be input
 *  b       [IN]        Char* pointer to be input
 *  c       [IN]        Char* pointer to be input
 * RETURNS
 *  return the minimal pointer
 *****************************************************************************/
static char *xml_find_min_ptr(char *a, char *b, char *c)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (a != NULL) {
        if (a < b) {
            if (c != NULL && c < a) {
                return c;
            } else {
                return a;
            }
        } else if (b != NULL) {
            if (c != NULL && c < a && c < b) {
                return c;
            } else {
                return b;
            }
        } else {
            if (c != NULL && c < a) {
                return c;
            } else {
                return a;
            }
        }
    } else {    /* ptr_comment != NULL */
        /* no comment */
        if (b != NULL) {
            if (c != NULL && c < b) {
                return c;
            } else {
                return b;
            }
        } else if (c != NULL) {
            return c;
            /* found end tag first, process end tag */
        } else {
            return NULL;
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  xml_find_min_entity_mark
 * DESCRIPTION
 *  This function is used to find the first "&" or "%" in string
 * PARAMETERS
 *  string      [IN]        Original string
 *  end         [IN]        End pointer
 * RETURNS
 *  if find, return start pointer of "&" or "%", or else return NULL
 *****************************************************************************/
static char *xml_find_min_entity_mark(char *string, char *end)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    char *ptr_ge, *ptr_pe, *start;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* start -> & or % ;  end -> ; */
    ptr_ge = strstr(string, "&");
    ptr_pe = strstr(string, "%");

    if (ptr_ge > end) {
        ptr_ge = NULL;
    }

    if (ptr_pe > end) {
        ptr_pe = NULL;
    }

    if ((ptr_ge != NULL) && (ptr_pe != NULL)) {
        start = (ptr_ge < ptr_pe) ? ptr_ge : ptr_pe;
    } else {
        start = (ptr_ge == NULL) ? ptr_pe : ptr_ge;
    }

    return start;

}


/*****************************************************************************
 * FUNCTION
 *  xml_check_attr_duplicate
 * DESCRIPTION
 *
 * PARAMETERS
 *  attr            [IN]        Attribute to be checked
 *  attrlist        [IN]        Attribute list
 *  index           [IN]        index in list
 * RETURNS
 *  if attribute is duplicated, return 1, else return 0
 *****************************************************************************/
static int32_t xml_check_attr_duplicate(char *attr, char **attrlist, int32_t index)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int32_t i = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    while ((i < index) && (attrlist[i] != NULL)) {
        if (strcmp(attr, attrlist[i]) == 0) {
            return 1;
        }
        i += 2;
    }
    return 0;
}


/*****************************************************************************
 * FUNCTION
 *  xml_process_end_tag
 * DESCRIPTION
 *  This function is used to process end tag
 * PARAMETERS
 *  parser      [IN]        Global parser struct
 *  start       [IN]        Start position of end tag in xml file
 *  length      [IN]        Length of end tag
 * RETURNS
 *  return XML_RESULT_OK if OK
 *****************************************************************************/
static int32_t xml_process_end_tag(XML_PARSER_STRUCT *parser, int32_t start, int32_t length)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    char *name, *ptr = NULL;
    int32_t fp, len, ret;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //LOG_I(XML, "[XML]xml_process_end_tag: length = %d\n", length);
    parser->error = XML_NO_ERROR;
    parser->ap_temp = get_buffer(parser, length + 1);
    if (parser->ap_temp == NULL) {
        return XML_RESULT_OUT_OF_MEMORY;
    }

    fp = xml_get_curr_position(parser);

    ret = xml_read_xmltext(parser, parser->ap_temp, start, length, &len);
    if (ret < 0) {
        free_buffer(parser, parser->ap_temp);
        parser->ap_temp = NULL;
        return XML_RESULT_FAIL;
    }
    parser->ap_temp[len] = 0;


    name = xml_extract_token(parser, parser->ap_temp + 2, " \r\n\t");
    if (name != NULL) {
        ptr = strstr(name, XML_TAG_GENERAL_END);
    }
    if (ptr != NULL) {
        ptr[0] = 0;
    }

    if ((name != NULL) && (parser->curr_depth == 0)) {
        xml_set_error(parser, XML_ERROR_MISMATCH_TAG, name);
    }

    if ((name != NULL) && (parser->curr_depth > 0)) {
        if ((strcmp(name, parser->elem[parser->curr_depth - 1])) != 0) {
            xml_set_error(parser, XML_ERROR_MISMATCH_TAG, name); /* check if end tag name matches start tag name */
        }
        xml_pop_start_tag(parser);
        parser->end_elem_hdlr(parser, name, parser->error);
        //LOG_I(XML, "[XML]End_elem_hdlr: %s\n", name);
    }
    /*
     * reset need_end_tag to 1, for those case:
     * * 1)data between end tag and start tag
     * * 2)data between end tag and end tag
     */
    parser->need_end_tag = 1;

    if (parser->stop == 0) {
        free_buffer(parser, parser->ap_temp);
        parser->ap_temp = NULL;
        xml_seek_xmltext(parser, fp, FS_FILE_BEGIN);

    }
    return XML_RESULT_OK;
}


/*****************************************************************************
 * FUNCTION
 *  xml_filter_comment_pi
 * DESCRIPTION
 *  This function is used to filter comment or pi in data string. after calling
 *  this function, comment or pi will be eliminated. eg:
 *  data (before) : xyz<--! comment here -->sdf
 *  data (after)  : xyzsdf
 * PARAMETERS
 *  data                [IN/OUT]        Buffer to save original and result
 *  filter_start        [IN]            Start string to be filtered
 *  filter_end          [IN]            End string to be filtered
 * RETURNS
 *  the length of result data.
 *****************************************************************************/
static int32_t xml_filter_comment_pi(char *data, const char *filter_start, const char *filter_end)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    char *ptr1, *ptr2;
    int32_t len, temp;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    len = strlen(data);
    temp = strlen(filter_end);

    while (len > 0) {
        ptr1 = strstr(data, filter_start);
        if (ptr1 != NULL) {
            ptr2 = strstr(ptr1, filter_end);
            if (ptr2 != NULL) {
                strcpy(ptr1, ptr2 + temp);
                len = len - (ptr2 - ptr1 + temp);

            } else {
                break;
            }
        } else {
            break;
        }
    }
    data[len] = 0;
    return len;

}


/*****************************************************************************
 * FUNCTION
 *  xml_reset_end_position
 * DESCRIPTION
 *  This function is used to reset end position of data when processing data:
 *  if PI/Comment/Entity/CDATA is overlap one parser->ap_temp, we should put the end of
 *  parser->ap_temp to the (start-1) of PI/Comment/Entity/CDATA
 * PARAMETERS
 *  data        [IN]        Data to be reseted
 *  length      [IN]        length of data
 * RETURNS
 *  The length of data result
 *****************************************************************************/
static int32_t xml_reset_end_position(char *data, int32_t length)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    char *start_pi, *start_comment, *start_ge, *start_pe, *start_cdata, *ptr;
    char *start = NULL, *end = NULL;
    int32_t len;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ptr = data;

    start = start_comment = NULL;
    start_pi = start_cdata = NULL;
    start_ge = start_pe = NULL;

    /* find tag's start mark nearest to the end of data */
    while ((start_comment = strstr(ptr, XML_TAG_COMMENT_START)) != NULL) {
        start = start_comment;
        end = strstr(start, XML_TAG_COMMENT_END);
        if (end != NULL) {
            ptr = end + 3;  /* set ptr to the end of XML_TAG_COMMENT_END "-->"  */
        } else {
            ptr = start;
            goto EXIT_HANDLE;
        }
    }

    /* ptr for PI is after comment  */
    while ((start_pi = strstr(ptr, XML_TAG_PI_START)) != NULL) {
        start = start_pi;
        end = strstr(start, XML_TAG_PROLOG_END);
        if (end != NULL) {
            ptr = end + 2;  /* set ptr to the end of XML_TAG_PROLOG_END "?>"  */
        } else {
            ptr = start;
            goto EXIT_HANDLE;
        }
    }

    while ((start_cdata = strstr(ptr, XML_TAG_CDATA_START)) != NULL) {
        start = start_cdata;
        end = strstr(start, XML_TAG_CDATA_END);
        if (end != NULL) {
            ptr = end + 3;  /* set ptr to the end of XML_TAG_CDATA_END "]]>"  */
        } else {
            ptr = start;
            goto EXIT_HANDLE;
        }
    }

    while ((start_ge = strstr(ptr, "&")) != NULL) {
        start = start_ge;
        end = strstr(start, ";");
        if (end != NULL) {
            ptr = end + 1;
        } else {
            ptr = start;
            goto EXIT_HANDLE;
        }
    }

    while ((start_pe = strstr(ptr, "%")) != NULL) {
        start = start_pe;
        end = strstr(start, ";");
        if (end != NULL) {
            ptr = end + 1;
        } else {
            ptr = start;
            goto EXIT_HANDLE;
        }
    }

    if (start == NULL) { /* no comment/pi/cdata/entity in data */
        return length;
    }

EXIT_HANDLE:
    if (end == NULL) {
        /*
         * (end == NULL) means that a full tag is over 1 data buffer,
         * we should reset end to start-1, the real len is (start - data)
         */
        len = start - data;
        /* if (len == 0), it means that one full tag cannont be included in one buffer
         * for such case, we ingnore this tag, return length
         */
        if (len == 0) {
            return length;
        } else {
            *start = 0;
            return len;
        }

    }

    return length;
}


/*****************************************************************************
 * FUNCTION
 *  xml_find_valid_entity_start
 * DESCRIPTION
 *  This function is used to handle such case:
 *  &e1&e2;
 *  here only &e2 is entity reference, we should modify sart pointer from "&e1.."
 *  to "&e2;"
 * PARAMETERS
 *  start       [IN]        Start pointer
 *  end         [IN]        End pointer
 * RETURNS
 *  void
 *****************************************************************************/
static char *xml_find_valid_entity_start(char *start, char *end)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    char *newstart, *temp1, *temp2, *temp = start + 1;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    newstart = start;

    while (temp < end) {
        temp1 = strstr(temp, "&");
        if (temp1 > end) {
            temp1 = NULL;
        }
        temp2 = strstr(temp, "%");
        if (temp2 > end) {
            temp2 = NULL;
        }

        if ((temp1 == NULL) && (temp2 == NULL)) {
            break;
        } else if ((temp1 != NULL) && (temp2 != NULL)) {
            newstart = (temp1 < temp2) ? temp2 : temp1;
            temp = newstart + 1;
        } else {
            newstart = (temp1 != NULL) ? temp1 : temp2;
            temp = newstart + 1;
        }
    }
    return newstart;
}


/*****************************************************************************
 * FUNCTION
 *  xml_convert_entity_value
 * DESCRIPTION
 *  This function converts entity value into buffer.
 * PARAMETERS
 *  value       [IN]        entity value.
 *  data        [IN]        data buffer.
 * RETURNS
 *  On success, return byte number, else return 0.
 *****************************************************************************/
static int32_t xml_convert_entity_value(uint32_t value, char *data, int32_t length)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int32_t i = 0, len = 0;
    char temp_buf[XML_CHAR_ENTITY_LENGTH];

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    while (value) {
        temp_buf[i++] = (char)(value & 0xFF);
        value = value >> 8;

        if (i >= length) {
            break;
        }
    }

    len = strlen(data);

    if (i > 0) {
        memcpy(&data[len], temp_buf, i);
    } else {
        memcpy(&data[len], (char *)(&value), 1);
    }

    data[len + i] = 0;

    return i;
}


/*****************************************************************************
 * FUNCTION
 *  xml_is_pre_entity
 * DESCRIPTION
 *  This function is used to find value of pre_entity in xml_named_char_table.
 *  pre_entity is like this: &gt;&lt;&amp;&quot;
 * PARAMETERS
 *  start       [IN]        Start pointer of pre_entity
 *  end         [IN]        End pointer of pre_entity
 * RETURNS
 *  if value is found, return it, else return NULL.
 *****************************************************************************/
static char *xml_is_pre_entity(char *start, char *end)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int32_t i;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    for (i = 0; i < MAX_NAME_CHAR_TABLE_SIZE; i++) {
        if (strncmp(xml_named_char_table[i].named_string, start, end - start + 1) == 0) {
            return (char *) & (xml_named_char_table[i].named_ch);
        }
    }
    return NULL;

}


/*****************************************************************************
 * FUNCTION
 *  xml_get_entity_value
 * DESCRIPTION
 *  This function is used to get entity value of entityreference, including character entity("&#10;")
 *  pre_entity ("&lg;"), general entity, and parameter entity
 * PARAMETERS
 *  parser          [IN]            Global parser struct
 *  data            [IN/OUT]        Destination buffer to save entity value
 *  start           [IN]            start pointer of entity reference
 *  end             [IN]            end pointer of entity reference
 * RETURNS
 *  void
 *****************************************************************************/
static void xml_get_entity_value(
    XML_PARSER_STRUCT *parser,
    char *data,
    char *start,
    char *end)
{
    char *temp;

    /* get char entity: &#10;&#x11; */
    if (start[1] == '#') {
        char value_str[XML_CHAR_ENTITY_LENGTH];

        memset(value_str, 0, XML_CHAR_ENTITY_LENGTH);
        if (end - start < XML_CHAR_ENTITY_LENGTH) {
            uint32_t value = 0;

            if (start[2] == 'x') {
                /* hexdecimal, 3~i */
                memcpy(value_str, &start[3], end - start - 3);
                sscanf(value_str, "%x", (unsigned int *)&value);

                xml_convert_entity_value(value, data, XML_CHAR_ENTITY_LENGTH);
            } else {
                /* decimal, 2~i */
                memcpy(value_str, &start[2], end - start - 2);
                value = atoi(value_str);
                strncat(data, (char *)(&value), 1);
            }
        } else {
            /* if length > XML_CHAR_ENTITY_LENGTH, ignore it */
            strncat(data, start, end - start + 1);
        }
    } else if ((temp = xml_is_pre_entity(start, end)) != NULL) {
        /* get pre_defined entity: &gt; &lt; */
        strcat(data, temp);
    } else {
        /* handle entity replace */
        char *entity_value = NULL;

        if ((entity_value = xml_replace_entity(parser, start, end - start + 1)) != NULL) {
            strcat(data, entity_value);
        } else {
            /* if cannot find entity_value, copy itself to data */
            strncat(data, start, end - start + 1);
        }
    }
}


/*****************************************************************************
 * FUNCTION
 *  xml_scan_named_char
 * DESCRIPTION
 *  This function is used to handle entity reference, including character entity("&#10;")
 *  pre_entity ("&lg;"), general entity, and parameter entity
 * PARAMETERS
 *  parser          [IN]            Global parser struct
 *  handle          [IN/OUT]        FS HANDLE to save data result
 *  dest            [IN/OUT]        Destination buffer to save data result
 *  original        [IN]            Original string to be scanned
 *  length          [IN]            The length of original string
 * RETURNS
 *  void
 *****************************************************************************/
static int32_t xml_scan_named_char(
    XML_PARSER_STRUCT *parser,
    FS_HANDLE *handle,
    char **dest,
    char *original,
    int32_t length)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    char *string, *start, *end,  *data = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    start = original;
    string = original;
    while (string < (original + length)) {

        /* start -> & or % ;  end -> ; */
        start = xml_find_min_entity_mark(string, original + length);
        if (start == NULL) {
            break;    /* no found "&" or "%", no entity  */
        }

        end = strstr(&start[1], ";");
        if ((end == NULL) || (end > string + length))
            /* ensure end is between length and avoid this case:"&xy<![CDATA[&ge;]]" */
        {
            break;    /* no found ";", no entity  */
        }

        start = xml_find_valid_entity_start(start, end);
        if ((end - start + 1) < XML_BUFFER_LENGTH) {
            if (data == NULL) {
                data = get_buffer(parser, XML_BUFFER_LENGTH);
                if (data == NULL) {
                    return XML_RESULT_OUT_OF_MEMORY;
                }
                data[0] = 0;    /* data is used to save result of entity reference in each loop */
            }
            /* store data before each "&" */
            xml_save_content_data(parser, handle, dest, string, start - string);

            /* get entity value to data, and store it to dest or handle */
            xml_get_entity_value(parser, data, start, end);
            xml_save_content_data(parser, handle, dest, data, strlen(data));
            data[0] = 0;
            string = end + 1;
        } else {
            break; /* if entity value > XML_BUFFER_LENGTH, ignore it*/
        }
    }
    /* save the rest data to dest or file */
    xml_save_content_data(parser, handle, dest, string, (original + length - string));
    if (data != NULL) {
        free_buffer(parser, data);
    }
    return XML_RESULT_OK;
}


/*****************************************************************************
 * FUNCTION
 *  xml_attr_handle_entity
 * DESCRIPTION
 *  This function is used to handle entity reference in attribute, including character entity("&#10;")
 *  pre_entity ("&lg;"), general entity, and parameter entity
 * PARAMETERS
 *  parser          [IN]            Global parser struct
 *  dest            [IN/OUT]        Destination buffer to save data result
 *  original        [IN]            Original string to be scanned
 *  length          [IN]            The length of original string
 * RETURNS
 *  void
 *****************************************************************************/
static int32_t xml_attr_handle_entity(
    XML_PARSER_STRUCT *parser,
    char *dest,
    char *original,
    int32_t length)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    char *start, *end,  *tmp = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    /*
     * handle entity reference in attribute, for example: value: "xyz&ge;sdf&pe;"
     * first loop: ptr1: "&ge;sdf&pe;"    ptr2:";sdf&pe;"  copy ptr2 to rest
     * "&ge;" is defined as "general entity"
     * result after first loop : value : "xyzgeneral entitysdf&pe;"
     */

    assert (length < MAX_CTRL_BUFFER_SIZE);

    strcat(dest, original);
    tmp = get_buffer(parser, MAX_CTRL_BUFFER_SIZE);
    if (tmp == NULL) {
        return XML_RESULT_OUT_OF_MEMORY;
    }
    tmp[0] = 0;/* tmp is used to save entity value of entity reference in entity declare */

    start = dest;
    while ((start = xml_find_min_entity_mark(start, dest + strlen(dest))) != NULL) {
        char *rest;
        int32_t len1, len2;

        end = strstr(start, ";");
        if (end == NULL) {
            break;
        }
        start = xml_find_valid_entity_start(start, end);
        xml_get_entity_value(parser, tmp, start, end);

        start[0] = 0;
        end++;

        /* in general, the length of last data wouldn't exceed MAX_CTRL_BUFFER_SIZE */
        len1 = strlen(dest) + strlen(tmp);
        len2 = strlen(end);
        assert ((len1 + len2) < MAX_CTRL_BUFFER_SIZE);

        rest = get_buffer(parser, strlen(end) + 1);
        if (rest == NULL) {
            return XML_RESULT_OUT_OF_MEMORY;
        }
        strcpy(rest, end);

        strcat(dest, tmp);
        strcat(dest, rest);
        free_buffer(parser, rest);
        tmp[0] = 0;

        start = dest + len1; /* for next loop, set start the end of temp */
    }
    free_buffer(parser, tmp);
    return XML_RESULT_OK;
}


/*****************************************************************************
 * FUNCTION
 *  xml_save_content_data
 * DESCRIPTION
 *  This function is used to save data to buffer or file: if data is over 2k,
 *  file will be used, else buffer will be used
 * PARAMETERS
 *  parser          [IN]            Global parser struct
 *  handle          [IN/OUT]        FILE HANDLE to save data
 *  dest            [IN/OUT]        Buffer to save data
 *  data            [IN]            Data to be saved
 *  len_data        [IN]            Length of data
 * RETURNS
 *  void
 *****************************************************************************/
static void xml_save_content_data(
    XML_PARSER_STRUCT *parser,
    FS_HANDLE *handle,
    char **dest,
    const char *data,
    int32_t len_data)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    uint32_t len_dest;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if ((*dest == NULL) && (*handle != 0)) {
        FS_Write(*handle, (void *)data, len_data, NULL);
    } else {
        if (*dest == NULL) {
            return;
        }

        len_dest = strlen(*dest);

        if (len_dest + len_data >= MAX_CTRL_BUFFER_SIZE) {
            *handle = FS_Open(L"Z:\\@Xml\\xml_temp", FS_READ_WRITE | FS_CREATE_ALWAYS);
            FS_Write(*handle, (void *)*dest, len_dest, NULL);
            FS_Write(*handle, (void *)data, len_data, NULL);
            xml_free_buffer(parser, *dest);
            *dest = NULL;
        } else {
            strncat(*dest, data, len_data);
        }
    }
    return;
}


/*****************************************************************************
 * FUNCTION
 *  xml_set_error
 * DESCRIPTION
 *  This function is used to set error code and error string
 * PARAMETERS
 *  parser      [OUT]       Global parser struct
 *  error       [IN]        Error code
 *  err_str     [IN]        Error string
 * RETURNS
 *  void
 *****************************************************************************/
static void xml_set_error(XML_PARSER_STRUCT *parser, int32_t error, char *err_str)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (parser->err_str != NULL) {
        free_buffer(parser, parser->err_str);
        parser->err_str = NULL;
    }

    if ((parser->err_str = get_buffer(parser, strlen(err_str) + 1)) != NULL) {
        strcpy(parser->err_str, err_str);
    }
    parser->error = error;

    return;
}


/*****************************************************************************
 * FUNCTION
 *  xml_read_xmltext
 * DESCRIPTION
 *  This function is used to read xml text from file or buffer
 * PARAMETERS
 *  parser      [OUT]       Global parser struct
 *  buffer      [IN]        buffer which is used to read xml text to
 *  from        [IN]        start position to read from
 *  size        [IN]        bytes to read
 *  length      [IN]        real bytes read
 * RETURNS
 *  return 0 if successed, <0 if failed.
 *****************************************************************************/
static int32_t xml_read_xmltext(
    XML_PARSER_STRUCT *parser,
    char *buffer,
    int32_t from,
    int32_t size,
    int32_t *length)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int32_t ret;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (parser->source != 0) {
        if (parser->isext == 0) {
            ret = DRM_seek_file(parser->source, from, FS_FILE_BEGIN);
            if (ret < 0) {
                return XML_RESULT_FILE_NOT_FOUND;
            }
            ret = DRM_read_file(parser->source, buffer, size, (uint32_t *)length);

            if (ret == FS_MSDC_READ_SECTOR_ERROR) {
                return XML_RESULT_FILE_NOT_FOUND;
            } else {
                return ret;
            }
        } else {
            ret = vir_seek_file(parser, from, FS_FILE_BEGIN);
            if (ret < 0) {
                return -1;
            }
            return vir_read_file(parser, buffer, size, (uint32_t *)length);
        }
    } else {
        if (from >= parser->xml_len) {
            *length = 0;
            return -1;
        }
        if ((from + size) > parser->xml_len) {
            size = parser->xml_len - from;
        }
        memcpy(buffer, parser->xml_text + from, size);
        *length = size;
        return 0;
    }
}


/*****************************************************************************
 * FUNCTION
 *  xml_seek_xmltext
 * DESCRIPTION
 *  This function is used to seek position in xml file or buffer
 * PARAMETERS
 *  parser      [OUT]       Global parser struct
 *  offset      [IN]        offset to seek
 *  ref         [IN]        reference position to seek
 * RETURNS
 *  return 0 if successfull
 *****************************************************************************/
static int32_t xml_seek_xmltext(XML_PARSER_STRUCT *parser, int32_t offset, int32_t ref)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (parser->source != 0) {
        if (parser->isext != 0) {
            return vir_seek_file(parser, offset, (uint8_t)ref);
        } else {
            return DRM_seek_file(parser->source, offset, (uint8_t)ref);
        }
    } else {
        return 0;
    }
}


/*****************************************************************************
 * FUNCTION
 *  xml_get_curr_position
 * DESCRIPTION
 *  This function is used to get current file position if xml text is passed by
 *  file
 * PARAMETERS
 *  parser      [OUT]       Global parser struct
 * RETURNS
 *  return file fp if xml text is passed by file
 *****************************************************************************/
static int32_t xml_get_curr_position(XML_PARSER_STRUCT *parser)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    uint32_t pos = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (parser->source != 0) {
        DRM_file_pos(parser->source, &pos);

        return pos;
    } else {
        return 0;
    }
}


/*****************************************************************************
 * FUNCTION
 *  xml_replace_entity
 * DESCRIPTION
 *  This function is used to get entity value of general entity reference or
 *  parameter entity
 * PARAMETERS
 *  parser      [IN]        Global parser struct
 *  start       [IN]        Start pointer of entity reference
 *  length      [IN]        The length of entity reference
 * RETURNS
 *  the entity value
 *****************************************************************************/
static char *xml_replace_entity(XML_PARSER_STRUCT *parser, char *start, int32_t length)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    char *target;
    int32_t i;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* get name of entity reference */
    target = get_buffer(parser, length - 1);
    if (target != NULL) {
        /* skip & ; */
        strncpy(target, &start[1], length - 2);
        target[length - 2] = 0;
    } else {
        return NULL;
    }

    /* match general entity */
    if (*start == '&') {
        for (i = 0; i < parser->ge_count; i++)
            if (strcmp(parser->ge_data[i << 1], target) == 0) {
                free_buffer(parser, target);
                return parser->ge_data[(i << 1) + 1];
            }
    }

    /* match parameter entity */
    if (*start == '%') {
        for (i = 0; i < parser->pe_count; i++)
            if (strcmp(parser->pe_data[i << 1], target) == 0) {
                free_buffer(parser, target);
                return parser->pe_data[(i << 1) + 1];
            }
    }

    /* cannot find match, return NULL */
    free_buffer(parser, target);
    return NULL;
}


/*****************************************************************************
 * FUNCTION
 *  xml_push_start_tag
 * DESCRIPTION
 *  This function is used to push start_tag to tag list
 * PARAMETERS
 *  parser      [IN]        Global parser struct
 *  tagname     [IN]        Start tag name to be pused
 * RETURNS
 *  if successful, return XML_RESULT_OK
 *****************************************************************************/
static int32_t xml_push_start_tag(XML_PARSER_STRUCT *parser, char *tagname)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    char *namebuffer = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    /* if parser->curr_depth overs XML_MAX_ELEM_DEPTH, it maybe cause out of memory */
    if (parser->curr_depth >= XML_MAX_ELEM_DEPTH) {
        return XML_RESULT_FAIL;
    }
    namebuffer = get_buffer(parser, strlen(tagname) + 1);
    if (namebuffer == NULL) {
        return XML_RESULT_OUT_OF_MEMORY;
    }
    strcpy(namebuffer, tagname);

    parser->elem[parser->curr_depth] = namebuffer;
    parser->curr_depth++;

    return XML_RESULT_OK;
}


/*****************************************************************************
 * FUNCTION
 *  xml_pop_start_tag
 * DESCRIPTION
 *  This function is used to pop last start_tag
 * PARAMETERS
 *  parser      [IN]        Global parser struct
 * RETURNS
 *  void
 *****************************************************************************/
static void xml_pop_start_tag(XML_PARSER_STRUCT *parser)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    parser->curr_depth--;
    if ((parser->curr_depth >= 0) && (parser->elem[parser->curr_depth] != NULL)) {
        free_buffer(parser, parser->elem[parser->curr_depth]);
    }
    return;
}


/*****************************************************************************
 * FUNCTION
 *  xml_free_tag_list
 * DESCRIPTION
 *  This function is used to free whole start_tag list
 * PARAMETERS
 *  parser      [IN]        Global parser struct
 * RETURNS
 *  void
 *****************************************************************************/
static void xml_free_tag_list(XML_PARSER_STRUCT *parser)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int32_t i = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    for (i = 0; i < parser->curr_depth; i++) {
        free_buffer(parser, parser->elem[i]);
        parser->elem[i] = NULL;
    }

    parser->curr_depth = 0;
    return;
}


/*****************************************************************************
 * FUNCTION
 *  xml_extract_token
 * DESCRIPTION
 *  this function is used to extract token
 * PARAMETERS
 *  parser      [IN]        Global parser struct
 *  string      [IN]        String to be extracted
 *  sep         [IN]        Mark char used to separated
 * RETURNS
 *  return extracted token
 *****************************************************************************/
static char *xml_extract_token(XML_PARSER_STRUCT *parser, char *string, char *sep)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    char *ptr;
    char *last_ptr = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
#ifdef MMI_ON_HARDWARE_P
    ptr = kal_strtok_r(string, sep, &parser->token_ptr);
#else
    ptr = strtok_r(string, sep, &last_ptr);
#endif
    return ptr;
}


/*****************************************************************************
 * FUNCTION
 *  xml_to_lower
 * DESCRIPTION
 *  this function is used to transfer str to lower
 * PARAMETERS
 *  str     [IN]        Pointer to be transfered
 * RETURNS
 *  void
 *****************************************************************************/
static void xml_to_lower(uint8_t *str)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    uint8_t *ptr;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    ptr = str;
    while (*ptr != 0) {
        if ((*ptr >= 'A') && (*ptr <= 'Z')) {
            *ptr += 'a' - 'A';
        }
        ptr++;
    }
}

//#ifdef __TEST_XML__
#if 0
static XML_PARSER_STRUCT parser;
static FS_HANDLE  result;


void test_xml_elem_start_hdlr(void *data, const char *el, const char **attr, int32_t error)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int32_t i = 0, length ;
    char *err_str;
    char *el_info;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    el_info = get_ctrl_buffer(1024);
    memset(el_info, 0, 1024);

    if (error != 0) {
        err_str = xml_get_err_string(&parser);
        strcat(el_info, "error string in element ");
        strcat(el_info, el);
        strcat(el_info, ":");
        strcat(el_info, err_str);
        strcat(el_info, "\r\n");
    }

    strcat(el_info, "<");
    strcat(el_info, el);
    strcat(el_info, " ");


    while ((attr[i] != NULL) && (attr[i + 1] != NULL)) {
        strcat(el_info, attr[i]);
        strcat(el_info, " = ");
        strcat(el_info, attr[i + 1]);
        strcat(el_info, "; ");
        i += 2;
    }

    strcat(el_info, ">");
    //strcat(el_info, "\r\n");

    //LOG_I(XML, el_info);
    FS_Write(result, el_info, strlen(el_info), (uint32_t *)&length);
    free_ctrl_buffer(el_info);
    el_info = NULL;

}


void test_xml_elem_end_hdlr(void *data, const char *el, int32_t error)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    char endtag[100];
    int32_t length;


    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    memset(endtag, 0, 100);
    strcat(endtag, "</");
    strcat(endtag, el);
    strcat(endtag, ">");
    //strcat(endtag, "\r\n");

    //LOG_I(XML, endtag);
    FS_Write(result, endtag, strlen(endtag), (uint32_t *)&length);
}

void test_xml_general_data_hdlr(void *resv, const char *el, const char *data, int32_t len, int32_t error)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    char *err_str;
    int32_t length;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (error != 0) {
        char err_info[100];

        memset(err_info, 0, 100);
        err_str = xml_get_err_string(&parser);
        //LOG_I(XML, err_info);
        FS_Write(result, err_info, strlen(err_info), (uint32_t *)&length);
    }
    //LOG_I(XML, data);
    //OS_Report("\r\n");
    FS_Write(result, (void *)data, strlen(data), (uint32_t *)&length);
    FS_Write(result, "\r\n", 2, (uint32_t *)&length);
}

extern void test_xml(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    uint16_t resultname[128];
    uint16_t testname[128];
    int32_t driver;
    /*uint8_t xmltext[] = {"<?xml version=\"1.0\" encoding=\"UTF-8\"?><adobehelp type=\"product\"><title>Adobe Reader 8</title><files type=\"nav\">\
<file type=\"contents\">nav_toc.xml</file><file type=\"search\">final_search_entries.xml</file></files></adobehelp>"};*/
#if 1
    uint8_t xmltext[] = {" <?xml version=\'1.0\' encoding=\'UTF-8\' standalone=\'yes\' ?>\
<event_report>\
<header>\
<category>notification</category>\
<subType>text</subType>\
<msgId>36868</msgId>\
<action>add</action>\
</header><body>\
<sender>Mediatek SmartDevice</sender>\
<appId>8</appId>\
<icon>\
<![CDATA[/9j/4AAQSkZJRgABAQAAAQABAAD/2wBDABsSFBcUERsXFhceHBsgKEIrKCUlKFE6PTBCYFVlZF9V\
XVtqeJmBanGQc1tdhbWGkJ6jq62rZ4C8ybqmx5moq6T/2wBDARweHigjKE4rK06kbl1upKSkpKSk\
pKSkpKSkpKSkpKSkpKSkpKSkpKSkpKSkpKSkpKSkpKSkpKSkpKSkpKSkpKT/wAARCAAoACgDASIA\
AhEBAxEB/8QAHwAAAQUBAQEBAQEAAAAAAAAAAAECAwQFBgcICQoL/8QAtRAAAgEDAwIEAwUFBAQA\
AAF9AQIDAAQRBRIhMUEGE1FhByJxFDKBkaEII0KxwRVS0fAkM2JyggkKFhcYGRolJicoKSo0NTY3\
ODk6Q0RFRkdISUpTVFVWV1hZWmNkZWZnaGlqc3R1dnd4eXqDhIWGh4iJipKTlJWWl5iZmqKjpKWm\
p6ipqrKztLW2t7i5usLDxMXGx8jJytLT1NXW19jZ2uHi4+Tl5ufo6erx8vP09fb3+Pn6/8QAHwEA\
AwEBAQEBAQEBAQAAAAAAAAECAwQFBgcICQoL/8QAtREAAgECBAQDBAcFBAQAAQJ3AAECAxEEBSEx\
BhJBUQdhcRMiMoEIFEKRobHBCSMzUvAVYnLRChYkNOEl8RcYGRomJygpKjU2Nzg5OkNERUZHSElK\
U1RVVldYWVpjZGVmZ2hpanN0dXZ3eHl6goOEhYaHiImKkpOUlZaXmJmaoqOkpaanqKmqsrO0tba3\
uLm6wsPExcbHyMnK0tPU1dbX2Nna4uPk5ebn6Onq8vP09fb3+Pn6/9oADAMBAAIRAxEAPwDpGYKu\
T0qnfXc6Rr9jjWRyed3AA/MVZuf9Q1cR16CgDoft+sf8+sH5/wD2VXLC8uJFf7bEsTA/KVOQR+Zr\
keKcylQM4Gecd6AO5Vgy5U5FFMtv9QtFACXf/Hu34fzrh1co25Tgiu3vP+PZvw/nXE/Zrj/nhJ/3\
waAH74h84X5v7h6A+v8A9aoi5ZiSck9Sad9muP8AnhJ/3waPs1x/zwk/74NO4krHb2n/AB7r+P8A\
Oiks/wDj2X8f50UhkrorqVYZBrP1HTGniX7LIYZAeu44IoooAz/7E1L/AJ/V/wC+2/wq/pumPAj/\
AGuUzOx4+Y4AoooA0URUUKowBRRRQB//2Q==\
]]></icon>\
<page_num>1</page_num>\
<page index=\"0\"><title><![CDATA[Title]]></title><content><![CDATA[Content: Hello!36868]]>\
</content></page><timestamp>1442542325</timestamp>\
<action_num>1</action_num><action_id>0</action_id>\
<action_name>´ò¿ª</action_name></body></event_report>"
                        };
#endif
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    if (xml_new_parser(&parser) < 0) {
        return;
    }
    xml_register_element_handler(&parser, test_xml_elem_start_hdlr, test_xml_elem_end_hdlr);
    xml_register_data_handler(&parser, test_xml_general_data_hdlr);

    xml_parse_buffer(&parser, xmltext, strlen(xmltext));
    xml_close_parser(&parser);
}
#endif /* __TEST_XML__*/


