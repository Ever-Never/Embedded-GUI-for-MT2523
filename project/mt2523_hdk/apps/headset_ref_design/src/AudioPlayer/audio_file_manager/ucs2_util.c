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

#include "FreeRTOS.h"
#include "ucs2_util.h"


int8_t *ucs2_strcpy(int8_t *strDestination, const int8_t *strSource)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    TCHAR count = 0;
    int8_t *temp = strDestination;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (strSource == NULL)
    {
        if (strDestination)
        {
            *(strDestination) = '\0';
        }
        return temp;

    }

    if (strDestination == NULL || strSource == NULL)
    {
        return NULL;
    }
    
    while (!(*(strSource + count) == 0))
    {
        *(strDestination + count) = *(strSource + count);
        count++;
    }

    *(strDestination + count) = '\0';

    return temp;
}


int32_t ucs2_strlen(const int8_t *arrOut)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    int32_t nCount = 0;
    int32_t nLength = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* Check for NULL character only at the odd no. of bytes 
       assuming forst byte start from zero */
    if (arrOut)
    {
        while (arrOut[nCount] != 0)
        {
            ++nLength;
            nCount++;
        }
    }
    return nLength; /* One is added to count 0th byte */
}


int32_t ucs2_wcslen(const TCHAR *string)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    const TCHAR *str_tmp = string;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* No need to restrict 2byte alignment */
    //configASSERT(!((uint32_t)string & 0x01));
    while(*str_tmp)
    {
        ++str_tmp;
    }
    
    return (int32_t) (str_tmp - string);
}


TCHAR* ucs2_wcscat(TCHAR *str_dst, const TCHAR *str_src)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    TCHAR *str_tmp = str_dst;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* No need to restrict 2byte alignment */
    //configASSERT(!((uint32_t)str_src & 0x01));
    //configASSERT(!((uint32_t)str_dst & 0x01));
    while(*str_tmp)
    {
        ++str_tmp;            /* find end of str_dst */
    }
    
    while(*str_src)
    {
        *str_tmp= *str_src;   /* Copy str_src to end of str_dst */
        ++str_tmp, ++str_src;
    }

    *str_tmp = '\0';

    return  str_dst;          /* return str_dst */
}


int32_t ucs2_wcsncmp(const TCHAR *str_src, 
                           const TCHAR *str_dst, 
                           uint32_t count)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //configASSERT(!((uint32_t)str_src & 0x01));
    //configASSERT(!((uint32_t)str_dst & 0x01));
    if (!count)
    {
        return 0;
    }    

    while ((--count) && (*str_src) && (*str_src == *str_dst))
    {
        ++str_src,++str_dst;        
    }   

    return (int32_t)(*str_src - *str_dst);
}


TCHAR* ucs2_wcscpy(TCHAR *str_dst, const TCHAR *str_src)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    TCHAR *str_tmp = str_dst;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /* No need to restrict 2byte alignment */
    //configASSERT(!((uint32_t)str_dst & 0x01));
    //configASSERT(!((uint32_t)str_src & 0x01));
    *str_tmp = *str_src;
    
    while(*str_src)
    {
        *(++str_tmp) = *(++str_src); /* Copy str_src over str_dst */
    }
    
    return str_dst;
}


uint32_t utf8_get_str_length(const char *str)
{
    uint32_t len = 0;
    while( *str != '\0' ){  
        if( *str & 1<<7 ){
            if( *str & (1<<6) ){
                if( *str & (1<<5) ){
                    if( *str & (1<<4)){ 
                        str += 4; /* 11110xxx */
                        len += 4;
                        continue;
                    }  
                    str += 3;/* 1110xxxx*/  
                    len += 3;
                    continue;  
                }  
                str += 2;/*110xxxxx*/  
                len += 2;
                continue;  
            }  
        }  
        str += 1;/*0xxxxxxx*/  
        len += 1;
        continue;  
  
    }
    return len;
}


uint32_t utf8_get_byte_num(const char *c)
{
    if( *c & 1<<7 ){
        if( *c & (1<<6) ){  
            if( *c & (1<<5) ){
                if( *c & (1<<4)){
                    return 4; /* 11110xxx */
                }  
                return 3; /* 1110xxxx */
            }  
            return 2; /* 110xxxxx */
        }  
    }  
    return 1; /* 0xxxxxxx */
}


static void reverse(TCHAR *start, int len)
{
    S32 i, j;
    TCHAR t; 

    for (i = 0, j = len-1; i < j; i++, j--)
    {
        t = start[i];
        start[i] = start[j];
        start[j] = t;
    }
}


static TCHAR* itow(TCHAR *out, U32 *max_len, S32 ival, char zflag, char sflag, int width, int x, char u)
{
    int i, rem, len; 
    unsigned int val, oldval;
    TCHAR *start;

    oldval = val = (unsigned int) ((sflag == 0) ? -ival : ival); 

    // calculate the result length in wchar
    for (len =(sflag == 0)? 2 : 1  ; (val /= x) > 0 ; len++)
      ;

    if (len > *max_len || width > *max_len)
    {
        *max_len = 0;
        return out;
    }

    if ((sflag == 0) && (0 != zflag)) 
    {
        *out++ = (TCHAR)'-'; 
    }
    if (len < width) // insert "space" or 0's before real value
    { 
        for (i=0; i <width-len ; i++)
        { 
            *out++ = zflag ? '0' : ' ';
        }
    }

    val = oldval;
    start = out;
    do 
    {
        rem = val % x;
        if (rem < 10)
        {
            *out = rem + '0';
        }
        else // hexadecimal
        {
            rem -= 10;
            if (u)
            {
                *out = rem + 'A';
            }
            else
            {
                *out = rem + 'a';
            }
        }
        out++;
    } while ((val /= x) > 0);

    if ((sflag == 0) && (0 == zflag)) 
    {
        *out++ = (TCHAR)'-';
    }

    reverse(start, (len-(((sflag == 0) && (0!=zflag)) ? 1 : 0))); 
    *max_len -= len;

    return out;
}


static void ucs2_wsprintf_int(TCHAR *str_buff, U32 buf_size, const TCHAR *fmt, va_list args)
{
    int dval, width = 0;
    unsigned int udval;
    char zflag = 0, sflag = 1;
    char *sval, cval;
    TCHAR *out = str_buff, *wval;
    const TCHAR *p; 

    buf_size /= sizeof(TCHAR);

    configASSERT(buf_size >= 1);

    /* Reserve string ending */
    buf_size--;

    //printf("ucs2_wsprintf_int enter\r\n");
    //printf("w char value is 0x%x\r\n", 'w');
    //printf("%% char value is 0x%x\r\n", '%');
    //printf("%% wide char value is 0x%x\r\n", L'%');
    for (p = fmt; *p && buf_size; p++, width = 0, zflag = 0, sflag = 1)
    {
        //printf("current char: 0x%.4x\r\n", *p);
        if (*p != '%')
        {
            *out++ = *p;
            buf_size--;
            continue;
        }

again:
        switch (*++p) {
        case 'c':
            cval= va_arg(args, int);
            *out++ = (TCHAR)cval;
            buf_size--;
            break;

        case 'd':
            dval= va_arg(args, int);
            sflag = ((dval<0) ? 0 : 1);
            out = itow(out, &buf_size, dval, zflag, sflag, width, 10, 0);
            break;

        case 'u':
            udval= va_arg(args, unsigned int);
            out = itow(out, &buf_size, udval, zflag, sflag, width, 10, 1);
            break;

        case 's':
            for (sval = va_arg(args, char *); *sval && buf_size; sval++ )
            {
                *out++ = (TCHAR)(*sval);
                buf_size--;
            }
            break;

        case 'w':
            //printf("wide enter");
            for (wval = va_arg(args, TCHAR *); *wval && buf_size; wval++ )
            {
                *out++ = *wval;
                buf_size--;
            }
            break;

        case 'x':
            dval= va_arg(args, int);
            out = itow(out, &buf_size, dval, zflag, sflag, width, 16, 0);
            break;

        case 'X':
            dval= va_arg(args, int);
            out = itow(out, &buf_size, dval, zflag, sflag, width, 16, 1);
            break;

        case '%':
            *out++ = '%';
            buf_size--;
            break;

        default:
             if (('0' == *p) && (width == 0) && (zflag == 0))
                zflag = 1;

             width = width * 10 + (*p - '0'); 

             goto again;  
        }
    }

    *out = 0;
}


TCHAR* ucs2_wsprintf_ex(TCHAR *str_buff, U32 buf_size, const TCHAR *fmt, ...)
{
    va_list args;

    va_start(args, fmt);

    //printf("ucs2_wsprintf_ex enter\r\n");
    ucs2_wsprintf_int(str_buff, buf_size, fmt, args);

    va_end(args);

    return str_buff;
}

