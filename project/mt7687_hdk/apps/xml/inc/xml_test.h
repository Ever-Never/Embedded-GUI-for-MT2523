/*
 * "$Id: testmxml.c 459 2014-10-19 17:21:48Z msweet $"
 *
 * Test program for Mini-XML, a small XML-like file parsing library.
 *
 * Usage:
 *
 *   ./testmxml input.xml [string-output.xml] >stdio-output.xml
 *   ./testmxml "<?xml ..." [string-output.xml] >stdio-output.xml
 *
 * Copyright 2003-2014 by Michael R Sweet.
 *
 * These coded instructions, statements, and computer programs are the
 * property of Michael R Sweet and are protected by Federal copyright
 * law.  Distribution and use rights are outlined in the file "COPYING"
 * which should have been included with this file.  If this file is
 * missing or damaged, see the license at:
 *
 *     http://www.msweet.org/projects.php/Mini-XML
 */


#ifndef __XML_TEST_H__
#define __XML_TEST_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "mxml.h"

extern int xml_test(void);


#ifdef __cplusplus
}
#endif

#endif
