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

/*
 * Include necessary headers...
 */
#include "mxml.h"
#include "xml/config.h"

#ifdef MXML_SUPPORT_FILE_SYSTEM
#ifndef WIN32
#  include <unistd.h>
#endif /* !WIN32 */
#endif
//#include <fcntl.h>
#ifndef O_BINARY
#  define O_BINARY 0
#endif /* !O_BINARY */
#include <xml_test.h>


/*
 * Globals...
 */

static int		event_counts[6];

const char test_xml[] = "<?xml version=\"1.0\" encoding=\"UTF-8\"?> \n <group> \n <option> \n <keyword type=\"opaque\">InputSlot</keyword> \n <default type=\"opaque\">Auto</default> \n <text>Media Source</text> \n <order type=\"real\">10.000000</order> \n <choice> \n <keyword type=\"opaque\">Auto</keyword> \n <text>Auto Tray Selection</text> \n <code type=\"opaque\" /> \n </choice> \n <choice> \n <keyword type=\"opaque\">Upper</keyword> \n <text>Tray 1</text> \n <code type=\"opaque\">&lt;&lt;/MediaPosition 0&gt;&gt;setpagedevice</code> \n </choice> \n <choice> \n <keyword type=\"opaque\">Lower</keyword> \n <text>Tray 2</text> \n <code type=\"opaque\">&lt;&lt;/MediaPosition 1&gt;&gt;setpagedevice</code> \n </choice> \n </option> \n <integer>123</integer> \n <string>Now is the time for all good men to come to the aid of their country.</string> \n <!-- this is a comment --> \n <![CDATA[this is CDATA 0123456789ABCDEF]]> \n </group>";

/*
 * Local functions...
 */

void		sax_cb(mxml_node_t *node, mxml_sax_event_t event, void *data);
mxml_type_t	type_cb(mxml_node_t *node);
#ifdef MXML_SUPPORT_FILE_SYSTEM
const char	*whitespace_cb(mxml_node_t *node, int where);
#endif /* MXML_SUPPORT_FILE_SYSTEM */

/*
 * 'main()' - Main entry for test program.
 */

int					/* O - Exit status */
xml_main(int  argc,				/* I - Number of command-line args */
         char *argv[])			/* I - Command-line args */
{
    int			i;		/* Looping var */
    //FILE			*fp;		/* File to read */
    //int			fd;		/* File descriptor */
    mxml_node_t		*tree,		/* XML tree */
                    *node;		/* Node which should be in test.xml */
#ifdef MXML_SUPPORT_INDEX
    mxml_index_t		*ind;		/* XML index */
#endif

#ifdef MXML_SUPPORT_FILE_SYSTEM
    char			buffer[16384];	/* Save string */
#endif
    static const char	*types[] =	/* Strings for node types */
    {
        "MXML_ELEMENT",
        "MXML_INTEGER",
        "MXML_OPAQUE",
        "MXML_REAL",
        "MXML_TEXT"
    };

#if 0
    /*
     * Check arguments...
     */

    if (argc != 2 && argc != 3) {
        fputs("Usage: testmxml filename.xml [string-output.xml]\n", stderr);
        return (1);
    }
#endif

    /*
     * Test the basic functionality...
     */
    LOG_I(xml_example, "[xml_test]Test the basic functionality...\n");

    tree = mxmlNewElement(MXML_NO_PARENT, "element");

    if (!tree) {
        fputs("ERROR: No parent node in basic test!\n", stderr);
        LOG_I(xml_example, "[xml_test]ERROR: No parent node in basic test!\n");
        return (1);
    }
    LOG_I(xml_example, "[xml_test]mxmlNewElement create tree, pass.\n");

    if (tree->type != MXML_ELEMENT) {
        fprintf(stderr, "ERROR: Parent has type %s (%d), expected MXML_ELEMENT!\n",
                tree->type < MXML_ELEMENT || tree->type > MXML_TEXT ?
                "UNKNOWN" : types[tree->type], tree->type);
        LOG_I(xml_example, "[xml_test]ERROR: Parent has type %s (%d), expected MXML_ELEMENT!\n",
               tree->type < MXML_ELEMENT || tree->type > MXML_TEXT ?
               "UNKNOWN" : types[tree->type], tree->type);
        mxmlDelete(tree);
        return (1);
    }
    LOG_I(xml_example, "[xml_test]Check the tree->type, pass.\n");

    if (strcmp(tree->value.element.name, "element")) {
        fprintf(stderr, "ERROR: Parent value is \"%s\", expected \"element\"!\n",
                tree->value.element.name);
        LOG_I(xml_example, "[xml_test]ERROR: Parent value is \"%s\", expected \"element\"!\n",
               tree->value.element.name);
        mxmlDelete(tree);
        return (1);
    }
    LOG_I(xml_example, "[xml_test]Check the tree->value.element.name, pass.\n");

    mxmlNewInteger(tree, 123);
    LOG_I(xml_example, "[xml_test]mxmlNewInteger: 123.\n");

    mxmlNewOpaque(tree, "opaque");
    LOG_I(xml_example, "[xml_test]mxmlNewOpaque: opaque.\n");

    mxmlNewReal(tree, 123.4f);
    LOG_I(xml_example, "[xml_test]mxmlNewReal: 123.4f.\n");

    mxmlNewText(tree, 1, "text");
    LOG_I(xml_example, "[xml_test]mxmlNewText: text.\n");

    mxmlLoadString(tree, "<group type='string'>string string string</group>",
                   MXML_NO_CALLBACK);
    LOG_I(xml_example, "[xml_test]mxmlLoadString: MXML_NO_CALLBACK.\n");

    mxmlLoadString(tree, "<group type='integer'>1 2 3</group>",
                   MXML_INTEGER_CALLBACK);
    LOG_I(xml_example, "[xml_test]mxmlLoadString: MXML_INTEGER_CALLBACK.\n");

    mxmlLoadString(tree, "<group type='real'>1.0 2.0 3.0</group>",
                   MXML_REAL_CALLBACK);
    LOG_I(xml_example, "[xml_test]mxmlLoadString: MXML_REAL_CALLBACK.\n");

    mxmlLoadString(tree, "<group>opaque opaque opaque</group>",
                   MXML_OPAQUE_CALLBACK);
    LOG_I(xml_example, "[xml_test]mxmlLoadString: MXML_OPAQUE_CALLBACK - 1.\n");

    mxmlLoadString(tree, "<foo><bar><one><two>value<two>value2</two></two></one>"
                   "</bar></foo>", MXML_OPAQUE_CALLBACK);
    LOG_I(xml_example, "[xml_test]mxmlLoadString: MXML_OPAQUE_CALLBACK - 2.\n");


    node = tree->child;

    if (!node) {
        fputs("ERROR: No first child node in basic test!\n", stderr);
        LOG_I(xml_example, "[xml_test]ERROR: No first child node in basic test!\n");
        mxmlDelete(tree);
        return (1);
    }
    LOG_I(xml_example, "[xml_test]Check the tree->child(first child), pass.\n");

    if (node->type != MXML_INTEGER) {
        fprintf(stderr, "ERROR: First child has type %s (%d), expected MXML_INTEGER!\n",
                node->type < MXML_ELEMENT || node->type > MXML_TEXT ?
                "UNKNOWN" : types[node->type], node->type);
        LOG_I(xml_example, "[xml_test]ERROR: First child has type %s (%d), expected MXML_INTEGER!\n",
               node->type < MXML_ELEMENT || node->type > MXML_TEXT ?
               "UNKNOWN" : types[node->type], node->type);
        mxmlDelete(tree);
        return (1);
    }
    LOG_I(xml_example, "[xml_test]Check the tree->child(first child)->type, pass.\n");

    if (node->value.integer != 123) {
        fprintf(stderr, "ERROR: First child value is %d, expected 123!\n",
                node->value.integer);
        LOG_I(xml_example, "[xml_test]ERROR: First child value is %d, expected 123!\n",
               node->value.integer);
        mxmlDelete(tree);
        return (1);
    }
    LOG_I(xml_example, "[xml_test]Check the tree->child(first child)->value.integer, pass.\n");

    node = node->next;

    if (!node) {
        fputs("ERROR: No second child node in basic test!\n", stderr);
        LOG_I(xml_example, "[xml_test]No second child node in basic test!\n");
        mxmlDelete(tree);
        return (1);
    }
    LOG_I(xml_example, "[xml_test]Check the tree->child->next(2nd child), pass.\n");

    if (node->type != MXML_OPAQUE) {
        fprintf(stderr, "ERROR: Second child has type %s (%d), expected MXML_OPAQUE!\n",
                node->type < MXML_ELEMENT || node->type > MXML_TEXT ?
                "UNKNOWN" : types[node->type], node->type);
        LOG_I(xml_example, "[xml_test]ERROR: Second child has type %s (%d), expected MXML_OPAQUE!\n",
               node->type < MXML_ELEMENT || node->type > MXML_TEXT ?
               "UNKNOWN" : types[node->type], node->type);
        mxmlDelete(tree);
        return (1);
    }
    LOG_I(xml_example, "[xml_test]Check the tree->child->next(2nd child)->type, pass.\n");

    if (!node->value.opaque || strcmp(node->value.opaque, "opaque")) {
        fprintf(stderr, "ERROR: Second child value is \"%s\", expected \"opaque\"!\n",
                node->value.opaque ? node->value.opaque : "(null)");
        LOG_I(xml_example, "[xml_test]ERROR: Second child value is \"%s\", expected \"opaque\"!\n",
               node->value.opaque ? node->value.opaque : "(null)");
        mxmlDelete(tree);
        return (1);
    }
    LOG_I(xml_example, "[xml_test]Check the tree->child->next(2nd child)->value.opaque, pass.\n");

    node = node->next;

    if (!node) {
        fputs("ERROR: No third child node in basic test!\n", stderr);
        LOG_I(xml_example, "[xml_test]ERROR: No third child node in basic test!\n");
        mxmlDelete(tree);
        return (1);
    }
    LOG_I(xml_example, "[xml_test]Check the tree->child->next->next->next(third child), pass.\n");

    if (node->type != MXML_REAL) {
        fprintf(stderr, "ERROR: Third child has type %s (%d), expected MXML_REAL!\n",
                node->type < MXML_ELEMENT || node->type > MXML_TEXT ?
                "UNKNOWN" : types[node->type], node->type);
        LOG_I(xml_example, "[xml_test]ERROR: Third child has type %s (%d), expected MXML_REAL!\n",
               node->type < MXML_ELEMENT || node->type > MXML_TEXT ?
               "UNKNOWN" : types[node->type], node->type);
        mxmlDelete(tree);
        return (1);
    }
    LOG_I(xml_example, "[xml_test]Check the (third child)->type, pass.\n");

    if (node->value.real != (double)123.4f) {
        fprintf(stderr, "ERROR: Third child value is %f, expected 123.4!\n",
                node->value.real);
        LOG_I(xml_example, "[xml_test]ERROR: Third child value is %f, expected 123.4!\n",
               node->value.real);
        mxmlDelete(tree);
        return (1);
    }
    LOG_I(xml_example, "[xml_test]Check the (third child)->value.real, pass.\n");

    node = node->next;

    if (!node) {
        fputs("ERROR: No fourth child node in basic test!\n", stderr);
        LOG_I(xml_example, "[xml_test]ERROR: No fourth child node in basic test!\n");
        mxmlDelete(tree);
        return (1);
    }
    LOG_I(xml_example, "[xml_test]Check the tree->child->next->next->next->next(fourth child), pass.\n");

    if (node->type != MXML_TEXT) {
        fprintf(stderr, "ERROR: Fourth child has type %s (%d), expected MXML_TEXT!\n",
                node->type < MXML_ELEMENT || node->type > MXML_TEXT ?
                "UNKNOWN" : types[node->type], node->type);
        LOG_I(xml_example, "[xml_test]ERROR: Fourth child has type %s (%d), expected MXML_TEXT!\n",
               node->type < MXML_ELEMENT || node->type > MXML_TEXT ?
               "UNKNOWN" : types[node->type], node->type);
        mxmlDelete(tree);
        return (1);
    }
    LOG_I(xml_example, "[xml_test]Check the (fourth child)->type, pass.\n");

    if (!node->value.text.whitespace ||
            !node->value.text.string || strcmp(node->value.text.string, "text")) {
        fprintf(stderr, "ERROR: Fourth child value is %d,\"%s\", expected 1,\"text\"!\n",
                node->value.text.whitespace,
                node->value.text.string ? node->value.text.string : "(null)");
        LOG_I(xml_example, "[xml_test]ERROR: Fourth child value is %d,\"%s\", expected 1,\"text\"!\n",
               node->value.text.whitespace,
               node->value.text.string ? node->value.text.string : "(null)");
        mxmlDelete(tree);
        return (1);
    }
    LOG_I(xml_example, "[xml_test]Check the (fourth child)->value.text.whitespace, pass.\n");

    for (i = 0; i < 4; i ++) {
        node = node->next;

        if (!node) {
            fprintf(stderr, "ERROR: No group #%d child node in basic test!\n", i + 1);
            LOG_I(xml_example, "[xml_test]ERROR: No group #%d child node in basic test!\n", i + 1);
            mxmlDelete(tree);
            return (1);
        }
        LOG_I(xml_example, "[xml_test]Check the group #%d, pass.\n", i + 1);


        if (node->type != MXML_ELEMENT) {
            fprintf(stderr, "ERROR: Group child #%d has type %s (%d), expected MXML_ELEMENT!\n",
                    i + 1, node->type < MXML_ELEMENT || node->type > MXML_TEXT ?
                    "UNKNOWN" : types[node->type], node->type);
            LOG_I(xml_example, "[xml_test]ERROR: Group child #%d has type %s (%d), expected MXML_ELEMENT!\n",
                   i + 1, node->type < MXML_ELEMENT || node->type > MXML_TEXT ?
                   "UNKNOWN" : types[node->type], node->type);
            mxmlDelete(tree);
            return (1);
        }
        LOG_I(xml_example, "[xml_test]Check the group #%d ->type, pass.\n", i + 1);
    }

#ifdef MXML_SUPPORT_SEARCH
    /*
     * Test mxmlFindPath...
     */
    LOG_I(xml_example, "[xml_test]Test mxmlFindPath...\n");

    node = mxmlFindPath(tree, "*/two");
    if (!node) {
        fputs("ERROR: Unable to find value for \"*/two\".\n", stderr);
        LOG_I(xml_example, "[xml_test]ERROR: Unable to find value for \"*/two\".\n");
        mxmlDelete(tree);
        return (1);
    }
    LOG_I(xml_example, "[xml_test]mxmlFindPath node = \"*/two\", pass.");

    if (node->type != MXML_OPAQUE || strcmp(node->value.opaque, "value")) {
        fputs("ERROR: Bad value for \"*/two\".\n", stderr);
        LOG_I(xml_example, "[xml_test]ERROR: Bad value for \"*/two\".\n");
        mxmlDelete(tree);
        return (1);
    }
    LOG_I(xml_example, "[xml_test]mxmlFindPath, check node value, pass.");

    node = mxmlFindPath(tree, "foo/*/two");
    if (!node) {
        fputs("ERROR: Unable to find value for \"foo/*/two\".\n", stderr);
        LOG_I(xml_example, "[xml_test]ERROR: Unable to find value for \"foo/*/two\".\n");
        mxmlDelete(tree);
        return (1);
    }
    LOG_I(xml_example, "[xml_test]mxmlFindPath node = \"foo/*/two\", pass.");

    if (node->type != MXML_OPAQUE || strcmp(node->value.opaque, "value")) {
        fputs("ERROR: Bad value for \"foo/*/two\".\n", stderr);
        LOG_I(xml_example, "[xml_test]ERROR: Bad value for \"foo/*/two\".\n");
        mxmlDelete(tree);
        return (1);
    }
    LOG_I(xml_example, "[xml_test]mxmlFindPath, check node value, pass.");

    node = mxmlFindPath(tree, "foo/bar/one/two");
    if (!node) {
        fputs("ERROR: Unable to find value for \"foo/bar/one/two\".\n", stderr);
        LOG_I(xml_example, "[xml_test]ERROR: Unable to find value for \"foo/bar/one/two\".\n");
        mxmlDelete(tree);
        return (1);
    }
    LOG_I(xml_example, "[xml_test]mxmlFindPath node = \"foo/bar/one/two\", pass.");

    if (node->type != MXML_OPAQUE || strcmp(node->value.opaque, "value")) {
        fputs("ERROR: Bad value for \"foo/bar/one/two\".\n", stderr);
        LOG_I(xml_example, "[xml_test]ERROR: Bad value for \"foo/bar/one/two\".\n");
        mxmlDelete(tree);
        return (1);
    }
    LOG_I(xml_example, "[xml_test]mxmlFindPath, check node value, pass.");

#endif /* MXML_SUPPORT_SEARCH */

#ifdef MXML_SUPPORT_INDEX
    /*
     * Test indices...
     */
    LOG_I(xml_example, "[xml_test]Test indices...\n");
    ind = mxmlIndexNew(tree, NULL, NULL);
    if (!ind) {
        fputs("ERROR: Unable to create index of all nodes!\n", stderr);
        LOG_I(xml_example, "[xml_test]ERROR: Unable to create index of all nodes!\n");
        mxmlDelete(tree);
        return (1);
    }
    LOG_I(xml_example, "[xml_test]create index of all nodes, pass\n");

    if (ind->num_nodes != 10) {
        fprintf(stderr, "ERROR: Index of all nodes contains %d "
                "nodes; expected 10!\n", ind->num_nodes);
        LOG_I(xml_example, "[xml_test]ERROR: Index of all nodes contains %d "
               "nodes; expected 10!\n", ind->num_nodes);
        mxmlIndexDelete(ind);
        mxmlDelete(tree);
        return (1);
    }
    LOG_I(xml_example, "[xml_test]check the number of all nodes, pass\n");

    mxmlIndexReset(ind);
    if (!mxmlIndexFind(ind, "group", NULL)) {
        fputs("ERROR: mxmlIndexFind for \"group\" failed!\n", stderr);
        LOG_I(xml_example, "[xml_test]ERROR: mxmlIndexFind for \"group\" failed!\n");
        mxmlIndexDelete(ind);
        mxmlDelete(tree);
        return (1);
    }
    LOG_I(xml_example, "[xml_test]find the index \"group\", pass\n");

    mxmlIndexDelete(ind);
    LOG_I(xml_example, "[xml_test]delete the index of all nodes, pass\n");

    ind = mxmlIndexNew(tree, "group", NULL);
    if (!ind) {
        fputs("ERROR: Unable to create index of groups!\n", stderr);
        LOG_I(xml_example, "[xml_test]ERROR: Unable to create index of groups!\n");
        mxmlDelete(tree);
        return (1);
    }
    LOG_I(xml_example, "[xml_test]create index for the \"group\" nodes, pass\n");

    if (ind->num_nodes != 4) {
        fprintf(stderr, "ERROR: Index of groups contains %d "
                "nodes; expected 4!\n", ind->num_nodes);
        LOG_I(xml_example, "[xml_test]ERROR: Index of groups contains %d "
               "nodes; expected 4!\n", ind->num_nodes);
        mxmlIndexDelete(ind);
        mxmlDelete(tree);
        return (1);
    }
    LOG_I(xml_example, "[xml_test]check the number of the \"group\" nodes, pass\n");

    mxmlIndexReset(ind);
    LOG_I(xml_example, "[xml_test]mxmlIndexReset, set the index to the first element , pass\n");

    if (!mxmlIndexEnum(ind)) {
        fputs("ERROR: mxmlIndexEnum failed!\n", stderr);
        LOG_I(xml_example, "[xml_test]ERROR: mxmlIndexEnum failed!\n");
        mxmlIndexDelete(ind);
        mxmlDelete(tree);
        return (1);
    }
    LOG_I(xml_example, "[xml_test]mxmlIndexEnum, check the next node in the sorted order, pass\n");

    mxmlIndexDelete(ind);
    LOG_I(xml_example, "[xml_test]delete the index of  \"group\" nodes, pass\n");

    ind = mxmlIndexNew(tree, NULL, "type");
    if (!ind) {
        fputs("ERROR: Unable to create index of type attributes!\n", stderr);
        LOG_I(xml_example, "[xml_test]ERROR: Unable to create index of type attributes!\n");
        mxmlDelete(tree);
        return (1);
    }
    LOG_I(xml_example, "[xml_test]create index of type attributes, pass\n");

    if (ind->num_nodes != 3) {
        fprintf(stderr, "ERROR: Index of type attributes contains %d "
                "nodes; expected 3!\n", ind->num_nodes);
        LOG_I(xml_example, "[xml_test]ERROR: Index of type attributes contains %d "
               "nodes; expected 3!\n", ind->num_nodes);
        mxmlIndexDelete(ind);
        mxmlDelete(tree);
        return (1);
    }
    LOG_I(xml_example, "[xml_test]check the number of node in this index, pass\n");

    mxmlIndexReset(ind);
    if (!mxmlIndexFind(ind, NULL, "string")) {
        fputs("ERROR: mxmlIndexFind for \"string\" failed!\n", stderr);
        LOG_I(xml_example, "[xml_test]ERROR: mxmlIndexFind for \"string\" failed!\n");
        mxmlIndexDelete(ind);
        mxmlDelete(tree);
        return (1);
    }
    LOG_I(xml_example, "[xml_test]find the \"string\" in this index, pass\n");

    mxmlIndexDelete(ind);
    LOG_I(xml_example, "[xml_test]delete in this index, pass\n");

    ind = mxmlIndexNew(tree, "group", "type");
    if (!ind) {
        fputs("ERROR: Unable to create index of elements and attributes!\n", stderr);
        LOG_I(xml_example, "[xml_test]ERROR: Unable to create index of elements and attributes!\n");
        mxmlDelete(tree);
        return (1);
    }
    LOG_I(xml_example, "[xml_test]create index of elements and attributes, pass\n");

    if (ind->num_nodes != 3) {
        fprintf(stderr, "ERROR: Index of elements and attributes contains %d "
                "nodes; expected 3!\n", ind->num_nodes);
        LOG_I(xml_example, "[xml_test]ERROR: Index of elements and attributes contains %d "
               "nodes; expected 3!\n", ind->num_nodes);
        mxmlIndexDelete(ind);
        mxmlDelete(tree);
        return (1);
    }
    LOG_I(xml_example, "[xml_test]check the number of node in this index, pass\n");

    mxmlIndexReset(ind);
    if (!mxmlIndexFind(ind, "group", "string")) {
        fputs("ERROR: mxmlIndexFind for \"group\" \"string\" failed!\n", stderr);
        LOG_I(xml_example, "[xml_test]ERROR: mxmlIndexFind for \"group\" \"string\" failed!\n");
        mxmlIndexDelete(ind);
        mxmlDelete(tree);
        return (1);
    }
    LOG_I(xml_example, "[xml_test]mxmlIndexFind for \"group\" \"string\", pass\n");

    mxmlIndexDelete(ind);
    LOG_I(xml_example, "[xml_test]delete in this index, pass\n");
#endif  /* MXML_SUPPORT_INDEX */

    /*
     * Check the mxmlDelete() works properly...
     */
    LOG_I(xml_example, "[xml_test]Check the mxmlDelete() works properly...\n");
    for (i = 0; i < 9; i ++) {
        if (tree->child) {
            mxmlDelete(tree->child);
            LOG_I(xml_example, "[xml_test]delete child #%d, pass\n",
                   i + 1);
        } else {
            fprintf(stderr, "ERROR: Child pointer prematurely NULL on child #%d\n",
                    i + 1);
            LOG_I(xml_example, "[xml_test]ERROR: Child pointer prematurely NULL on child #%d\n",
                   i + 1);
            mxmlDelete(tree);
            return (1);
        }
    }

    if (tree->child) {
        fputs("ERROR: Child pointer not NULL after deleting all children!\n", stderr);
        LOG_I(xml_example, "[xml_test]ERROR: Child pointer not NULL after deleting all children!\n");
        return (1);
    }

    if (tree->last_child) {
        fputs("ERROR: Last child pointer not NULL after deleting all children!\n", stderr);
        LOG_I(xml_example, "[xml_test]ERROR: Last child pointer not NULL after deleting all children!\n");
        return (1);
    }
    LOG_I(xml_example, "[xml_test]all children is deleted, pass\n");

    mxmlDelete(tree);

    LOG_I(xml_example, "[xml_test]tree is deleted, pass\n");

    /*
     * Open the file/string using the default (MXML_NO_CALLBACK) callback...
     */
    LOG_I(xml_example, "[xml_test]load the string using the default (MXML_NO_CALLBACK) callback...\n");

    if (argv[1][0] == '<') {
        tree = mxmlLoadString(NULL, argv[1], MXML_NO_CALLBACK);
    }
#ifdef MXML_SUPPORT_FILE_SYSTEM
    else if ((fp = fopen(argv[1], "rb")) == NULL) {
        perror(argv[1]);
        return (1);
    } else {
        /*
         * Read the file...
         */

        tree = mxmlLoadFile(NULL, fp, MXML_NO_CALLBACK);

        fclose(fp);
    }
#endif /* MXML_SUPPORT_FILE_SYSTEM */

    if (!tree) {
        fputs("Unable to read XML file with default callback!\n", stderr);
        LOG_I(xml_example, "[xml_test]ERROR: Unable to read XML file with default callback!\n");
        return (1);
    }
    LOG_I(xml_example, "[xml_test]create tree with loaded string, pass.\n");

#ifdef MXML_SUPPORT_SEARCH
    //if (!strcmp(argv[1], "test.xml"))
    {
        const char	*text;			/* Text value */

        /*
         * Verify that mxmlFindElement() and indirectly mxmlWalkNext() work
         * properly...
         */
        LOG_I(xml_example, "[xml_test]Verify that mxmlFindElement() and indirectly mxmlWalkNext() work properly...\n");
        LOG_I(xml_example, "[xml_test]this test is ONLY for \"test.xml\"...\n");

        if ((node = mxmlFindPath(tree, "group/option/keyword")) == NULL) {
            fputs("Unable to find group/option/keyword element in XML tree!\n", stderr);
            LOG_I(xml_example, "[xml_test][test.xml ONLY]Unable to find group/option/keyword element in XML tree!\n");
            mxmlDelete(tree);
            //return (1);
        }

        if (node->type != MXML_TEXT) {
            fputs("No child node of group/option/keyword!\n", stderr);
            LOG_I(xml_example, "[xml_test][test.xml ONLY]No child node of group/option/keyword!\n");
#ifdef MXML_SUPPORT_FILE_SYSTEM
            mxmlSaveFile(tree, stderr, MXML_NO_CALLBACK);
#endif /* MXML_SUPPORT_FILE_SYSTEM */
            mxmlDelete(tree);
            //return (1);
        }

#ifdef MXML_SUPPORT_GET_FUNCTIONS
        if ((text = mxmlGetText(node, NULL)) == NULL || strcmp(text, "InputSlot")) {
            fprintf(stderr, "Child node of group/option/value has value \"%s\" instead of \"InputSlot\"!\n", text ? text : "(null)");
            LOG_I(xml_example, "[xml_test][test.xml ONLY]Child node of group/option/value has value \"%s\" instead of \"InputSlot\"!\n", text ? text : "(null)");
            mxmlDelete(tree);
            //return (1);
        }
#endif /* MXML_SUPPORT_GET_FUNCTIONS */
    }
#endif /* MXML_SUPPORT_SEARCH */
    LOG_I(xml_example, "[xml_test][test.xml ONLY] test.xml is finished.\n");

    mxmlDelete(tree);
    LOG_I(xml_example, "[xml_test][test.xml ONLY] remove the tree of test.xml.\n");

    /*
     * Open the file...
     */
    LOG_I(xml_example, "[xml_test]load the string using the special callback, type_cb()...\n");

    if (argv[1][0] == '<') {
        tree = mxmlLoadString(NULL, argv[1], type_cb);
    }
#ifdef MXML_SUPPORT_FILE_SYSTEM
    else if ((fp = fopen(argv[1], "rb")) == NULL) {
        perror(argv[1]);
        return (1);
    } else {
        /*
         * Read the file...
         */

        tree = mxmlLoadFile(NULL, fp, type_cb);

        fclose(fp);
    }
#endif /* MXML_SUPPORT_FILE_SYSTEM */

    if (!tree) {
        fputs("Unable to read XML file!\n", stderr);
        LOG_I(xml_example, "[xml_test]ERROR: Unable to load XML string!\n");
        return (1);
    }
    LOG_I(xml_example, "[xml_test]create tree with the loaded string, pass\n");

#ifdef MXML_SUPPORT_SEARCH
    //if (!strcmp(argv[1], "test.xml"))
    {
        /*
         * Verify that mxmlFindElement() and indirectly mxmlWalkNext() work
         * properly...
         */
        LOG_I(xml_example, "[xml_test]Verify that mxmlFindElement() and indirectly mxmlWalkNext() work properly...(2)\n");
        LOG_I(xml_example, "[xml_test]this test is ONLY for \"test.xml\"...(2)\n");

        if ((node = mxmlFindElement(tree, tree, "choice", NULL, NULL,
                                    MXML_DESCEND)) == NULL) {
            fputs("Unable to find first <choice> element in XML tree!\n", stderr);
            LOG_I(xml_example, "[xml_test][test.xml ONLY] Unable to find first <choice> element in XML tree!(2)\n");
            mxmlDelete(tree);
            //return (1);
        }

        if (!mxmlFindElement(node, tree, "choice", NULL, NULL, MXML_NO_DESCEND)) {
            fputs("Unable to find second <choice> element in XML tree!\n", stderr);
            LOG_I(xml_example, "[xml_test][test.xml ONLY] Unable to find second <choice> element in XML tree!(2)\n");
            mxmlDelete(tree);
            //return (1);
        }
    }
    LOG_I(xml_example, "[xml_test][test.xml ONLY] test.xml is finished.(2)\n");
#endif /* MXML_SUPPORT_SEARCH */

#ifdef MXML_SUPPORT_FILE_SYSTEM
    /*
     * Print the XML tree...
     */

    mxmlSaveFile(tree, stdout, whitespace_cb);

    /*
     * Save the XML tree to a string and print it...
     */

    if (mxmlSaveString(tree, buffer, sizeof(buffer), whitespace_cb) > 0) {
        if (argc == 3) {
            fp = fopen(argv[2], "w");
            fputs(buffer, fp);
            fclose(fp);
        }
    }
#endif /* MXML_SUPPORT_FILE_SYSTEM */

    /*
     * Delete the tree...
     */

    mxmlDelete(tree);
    LOG_I(xml_example, "[xml_test][test.xml ONLY] remove the tree of test.xml.(2)\n");

#ifdef MXML_SUPPORT_FILE_SYSTEM
    /*
     * Read from/write to file descriptors...
     */

    if (argv[1][0] != '<') {
        /*
         * Open the file again...
         */

        if ((fd = open(argv[1], O_RDONLY | O_BINARY)) < 0) {
            perror(argv[1]);
            return (1);
        }

        /*
         * Read the file...
         */

        tree = mxmlLoadFd(NULL, fd, type_cb);

        close(fd);

        /*
         * Create filename.xmlfd...
         */

        snprintf(buffer, sizeof(buffer), "%sfd", argv[1]);

        if ((fd = open(buffer, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, 0666)) < 0) {
            perror(buffer);
            mxmlDelete(tree);
            return (1);
        }

        /*
         * Write the file...
         */

        mxmlSaveFd(tree, fd, whitespace_cb);

        close(fd);

        /*
         * Delete the tree...
         */

        mxmlDelete(tree);
    }
#endif /* MXML_SUPPORT_FILE_SYSTEM */

    /*
     * Test SAX methods...
     */
    LOG_I(xml_example, "[xml_test]Test SAX methods...\n");

    memset(event_counts, 0, sizeof(event_counts));

    if (argv[1][0] == '<') {
        mxmlSAXLoadString(NULL, argv[1], type_cb, sax_cb, NULL);
    }
#ifdef MXML_SUPPORT_FILE_SYSTEM
    else if ((fp = fopen(argv[1], "rb")) == NULL) {
        perror(argv[1]);
        return (1);
    } else {
        /*
         * Read the file...
         */

        mxmlSAXLoadFile(NULL, fp, type_cb, sax_cb, NULL);

        fclose(fp);
    }
#endif /* MXML_SUPPORT_FILE_SYSTEM */

    LOG_I(xml_example, "[xml_test]this test is ONLY for \"test.xml\"...\n");

    //if (!strcmp(argv[1], "test.xml"))
    {
        if (event_counts[MXML_SAX_CDATA] != 1) {
            fprintf(stderr, "MXML_SAX_CDATA seen %d times, expected 1 times!\n",
                    event_counts[MXML_SAX_CDATA]);
            LOG_I(xml_example, "[xml_test][test.xml ONLY]MXML_SAX_CDATA seen %d times, expected 1 times!\n",
                   event_counts[MXML_SAX_CDATA]);
            //return (1);
        }

        if (event_counts[MXML_SAX_COMMENT] != 1) {
            fprintf(stderr, "MXML_SAX_COMMENT seen %d times, expected 1 times!\n",
                    event_counts[MXML_SAX_COMMENT]);
            LOG_I(xml_example, "[xml_test][test.xml ONLY]MXML_SAX_COMMENT seen %d times, expected 1 times!\n",
                   event_counts[MXML_SAX_COMMENT]);
            //return (1);
        }

        if (event_counts[MXML_SAX_DATA] != 60) {
            fprintf(stderr, "MXML_SAX_DATA seen %d times, expected 60 times!\n",
                    event_counts[MXML_SAX_DATA]);
            LOG_I(xml_example, "[xml_test][test.xml ONLY]MXML_SAX_DATA seen %d times, expected 60 times!\n",
                   event_counts[MXML_SAX_DATA]);
            //return (1);
        }

        if (event_counts[MXML_SAX_DIRECTIVE] != 1) {
            fprintf(stderr, "MXML_SAX_DIRECTIVE seen %d times, expected 1 times!\n",
                    event_counts[MXML_SAX_DIRECTIVE]);
            LOG_I(xml_example, "[xml_test][test.xml ONLY]MXML_SAX_DIRECTIVE seen %d times, expected 1 times!\n",
                   event_counts[MXML_SAX_DIRECTIVE]);
            //return (1);
        }

        if (event_counts[MXML_SAX_ELEMENT_CLOSE] != 20) {
            fprintf(stderr, "MXML_SAX_ELEMENT_CLOSE seen %d times, expected 20 times!\n",
                    event_counts[MXML_SAX_ELEMENT_CLOSE]);
            LOG_I(xml_example, "[xml_test][test.xml ONLY]MXML_SAX_ELEMENT_CLOSE seen %d times, expected 20 times!\n",
                   event_counts[MXML_SAX_ELEMENT_CLOSE]);
            //return (1);
        }

        if (event_counts[MXML_SAX_ELEMENT_OPEN] != 20) {
            fprintf(stderr, "MXML_SAX_ELEMENT_OPEN seen %d times, expected 20 times!\n",
                    event_counts[MXML_SAX_ELEMENT_OPEN]);
            LOG_I(xml_example, "[xml_test][test.xml ONLY]MXML_SAX_ELEMENT_OPEN seen %d times, expected 20 times!\n",
                   event_counts[MXML_SAX_ELEMENT_OPEN]);
            //return (1);
        }

        LOG_I(xml_example, "[xml_test]check the event_counts, pass.\n");
    }

    /*
     * Return...
     */
    LOG_I(xml_example, "[xml_test]all test cases are finish.\n");
    return (0);
}


/*
 * 'sax_cb()' - Process nodes via SAX.
 */

void
sax_cb(mxml_node_t      *node,		/* I - Current node */
       mxml_sax_event_t event,		/* I - SAX event */
       void             *data)		/* I - SAX user data */
{
    /*
     * This SAX callback just counts the different events.
     */

    event_counts[event] ++;
    LOG_I(xml_example, "[xml_test]SAX callback sax_cb(), event = %d, event_counts = %d \n", event, event_counts[event]);
}


/*
 * 'type_cb()' - XML data type callback for mxmlLoadFile()...
 */

mxml_type_t				/* O - Data type */
type_cb(mxml_node_t *node)		/* I - Element node */
{
    const char	*type;			/* Type string */


    /*
     * You can lookup attributes and/or use the element name, hierarchy, etc...
     */

    if ((type = mxmlElementGetAttr(node, "type")) == NULL) {
        type = node->value.element.name;
    }

    LOG_I(xml_example, "[xml_test]XML data type callback type_cb(), type = %s \n", type);

    if (!strcmp(type, "integer")) {
        return (MXML_INTEGER);
    } else if (!strcmp(type, "opaque") || !strcmp(type, "pre")) {
        return (MXML_OPAQUE);
    } else if (!strcmp(type, "real")) {
        return (MXML_REAL);
    } else {
        return (MXML_TEXT);
    }
}


#ifdef MXML_SUPPORT_FILE_SYSTEM
/*
 * 'whitespace_cb()' - Let the mxmlSaveFile() function know when to insert
 *                     newlines and tabs...
 */

const char *				/* O - Whitespace string or NULL */
whitespace_cb(mxml_node_t *node,	/* I - Element node */
              int         where)	/* I - Open or close tag? */
{
    mxml_node_t	*parent;		/* Parent node */
    int		level;			/* Indentation level */
    const char	*name;			/* Name of element */
    static const char *tabs = "\t\t\t\t\t\t\t\t";
    /* Tabs for indentation */


    /*
     * We can conditionally break to a new line before or after any element.
     * These are just common HTML elements...
     */

    name = node->value.element.name;

    if (!strcmp(name, "html") || !strcmp(name, "head") || !strcmp(name, "body") ||
            !strcmp(name, "pre") || !strcmp(name, "p") ||
            !strcmp(name, "h1") || !strcmp(name, "h2") || !strcmp(name, "h3") ||
            !strcmp(name, "h4") || !strcmp(name, "h5") || !strcmp(name, "h6")) {
        /*
         * Newlines before open and after close...
         */

        if (where == MXML_WS_BEFORE_OPEN || where == MXML_WS_AFTER_CLOSE) {
            return ("\n");
        }
    } else if (!strcmp(name, "dl") || !strcmp(name, "ol") || !strcmp(name, "ul")) {
        /*
         * Put a newline before and after list elements...
         */

        return ("\n");
    } else if (!strcmp(name, "dd") || !strcmp(name, "dt") || !strcmp(name, "li")) {
        /*
         * Put a tab before <li>'s, <dd>'s, and <dt>'s, and a newline after them...
         */

        if (where == MXML_WS_BEFORE_OPEN) {
            return ("\t");
        } else if (where == MXML_WS_AFTER_CLOSE) {
            return ("\n");
        }
    } else if (!strncmp(name, "?xml", 4)) {
        if (where == MXML_WS_AFTER_OPEN) {
            return ("\n");
        } else {
            return (NULL);
        }
    } else if (where == MXML_WS_BEFORE_OPEN ||
               ((!strcmp(name, "choice") || !strcmp(name, "option")) &&
                where == MXML_WS_BEFORE_CLOSE)) {
        for (level = -1, parent = node->parent;
                parent;
                level ++, parent = parent->parent);

        if (level > 8) {
            level = 8;
        } else if (level < 0) {
            level = 0;
        }

        return (tabs + 8 - level);
    } else if (where == MXML_WS_AFTER_CLOSE ||
               ((!strcmp(name, "group") || !strcmp(name, "option") ||
                 !strcmp(name, "choice")) &&
                where == MXML_WS_AFTER_OPEN)) {
        return ("\n");
    } else if (where == MXML_WS_AFTER_OPEN && !node->child) {
        return ("\n");
    }

    /*
     * Return NULL for no added whitespace...
     */

    return (NULL);
}
#endif /* MXML_SUPPORT_FILE_SYSTEM */


int xml_test(void)
{
    char *test_data[2];
    test_data[0] = NULL;
    test_data[1] = (char *)test_xml;
    int ret = 0;
    //printf("[xml_test]test.xml: \n %s \n", test_xml);
    LOG_I(xml_example, "[xml_test]test.xml: \n %s \n", test_xml);
    ret = xml_main(2, test_data);
    return ret;
}

/*
 * End of "$Id: testmxml.c 459 2014-10-19 17:21:48Z msweet $".
 */
