/*
  Copyright (c) 2009 Dave Gamble

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/

#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"
#include "cJSON_test.h"
#include "syslog.h"

/* Example data type used in this example project. */
struct record {
    const char *precision;
    double lat, lon;
    const char *address, *city, *state, *zip, *country;
};

/* a bunch of json: */
const char text1[] = "{\n\"name\": \"Jack (\\\"Bee\\\") Nimble\", \n\"format\": {\"type\":       \"rect\", \n\"width\":      1920, \n\"height\":     1080, \n\"interlace\":  false,\"frame rate\": 24\n}\n}";
const char text2[] = "[\"Sunday\", \"Monday\", \"Tuesday\", \"Wednesday\", \"Thursday\", \"Friday\", \"Saturday\"]";
const char text3[] = "[\n    [0, -1, 0],\n    [1, 0, 0],\n    [0, 0, 1]\n   ]\n";
const char text4[] = "{\n     \"Image\": {\n          \"Width\":  800,\n          \"Height\": 600,\n          \"Title\":  \"View from 15th Floor\",\n         \"Thumbnail\": {\n              \"Url\":    \"http:/*www.example.com/image/481989943\",\n               \"Height\": 125,\n              \"Width\":  \"100\"\n           },\n            \"IDs\": [116, 943, 234, 38793]\n       }\n }";
const char text5[] = "[\n  {\n     \"precision\": \"zip\",\n   \"Latitude\":  37.7668,\n   \"Longitude\": -122.3959,\n     \"Address\":   \"\",\n  \"City\":      \"SAN FRANCISCO\",\n     \"State\":     \"CA\",\n    \"Zip\":       \"94107\",\n     \"Country\":   \"US\"\n     },\n    {\n     \"precision\": \"zip\",\n   \"Latitude\":  37.371991,\n     \"Longitude\": -122.026020,\n   \"Address\":   \"\",\n  \"City\":      \"SUNNYVALE\",\n     \"State\":     \"CA\",\n    \"Zip\":       \"94085\",\n     \"Country\":   \"US\"\n     }\n     ]";

/**
  * @brief     Parse text to JSON, then render back in text, and print
  * @param[in] char *text:input string
  * @return    None
  */
int doit(char *text)
{
    char *out;
    cJSON *json;

    json = cJSON_Parse(text);
    if (!json) {
        LOG_I(cjson_example, "Error before: [%s]\n", cJSON_GetErrorPtr());
        return 1;
    } else {
        out = cJSON_Print(json);
        cJSON_Delete(json);
        LOG_I(cjson_example, "%s\n", out);
        cJSON_free(out);
        return 0;
    }
}


/**
  * @brief     Create a number of JSON objects for demonstration purpose
  * @param     None
  * @return    None
  */
int create_objects()
{
    cJSON *root, *fmt, *img, *thm, *fld;
    char *out;
    int i;	/* The index number. */
    int ret = 0;

    /* Here we construct several JSON objects. */

    /* The "Video" data type: */
    root = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "name", cJSON_CreateString("Jack (\"Bee\") Nimble"));
    cJSON_AddItemToObject(root, "format", fmt = cJSON_CreateObject());
    cJSON_AddStringToObject(fmt, "type",		"rect");
    cJSON_AddNumberToObject(fmt, "width",		1920);
    cJSON_AddNumberToObject(fmt, "height",		1080);
    cJSON_AddFalseToObject (fmt, "interlace");
    cJSON_AddNumberToObject(fmt, "frame rate",	24);

    out = cJSON_Print(root); /* Print to text */
    cJSON_Delete(root);      /* Delete the cJSON object */
    LOG_I(cjson_example, "%s\n", out); /* Print out the text */
    cJSON_free(out);	     /* Release the string. */

    /* The "days of the week" array: */
    const char *strings[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
    root = cJSON_CreateStringArray(strings, 7);

    out = cJSON_Print(root);
    cJSON_Delete(root);
    LOG_I(cjson_example, "%s\n", out);
    cJSON_free(out);

    /* The matrix: */
    int numbers[3][3] = {{0, -1, 0}, {1, 0, 0}, {0, 0, 1}};
    root = cJSON_CreateArray();
    for (i = 0; i < 3; i++) {
        cJSON_AddItemToArray(root, cJSON_CreateIntArray(numbers[i], 3));
    }

    /*	cJSON_ReplaceItemInArray(root,1,cJSON_CreateString("Replacement")); */
    out = cJSON_Print(root);
    cJSON_Delete(root);
    LOG_I(cjson_example, "%s\n", out);
    cJSON_free(out);

    /* The "gallery" item: */
    int ids[4] = {116, 943, 234, 38793};
    root = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "Image", img = cJSON_CreateObject());
    cJSON_AddNumberToObject(img, "Width", 800);
    cJSON_AddNumberToObject(img, "Height", 600);
    cJSON_AddStringToObject(img, "Title", "View from 15th Floor");
    cJSON_AddItemToObject(img, "Thumbnail", thm = cJSON_CreateObject());
    cJSON_AddStringToObject(thm, "Url", "http:/*www.example.com/image/481989943");
    cJSON_AddNumberToObject(thm, "Height", 125);
    cJSON_AddStringToObject(thm, "Width", "100");
    cJSON_AddItemToObject(img, "IDs", cJSON_CreateIntArray(ids, 4));

    out = cJSON_Print(root);
    cJSON_Delete(root);
    LOG_I(cjson_example, "%s\n", out);
    cJSON_free(out);

    /* The array of "records": */
    struct record fields[2] = {
        {"zip", 37.7668, -1.223959e+2, "", "SAN FRANCISCO", "CA", "94107", "US"},
        {"zip", 37.371991, -1.22026e+2, "", "SUNNYVALE", "CA", "94085", "US"}
    };

    root = cJSON_CreateArray();
    for (i = 0; i < 2; i++) {
        cJSON_AddItemToArray(root, fld = cJSON_CreateObject());
        cJSON_AddStringToObject(fld, "precision", fields[i].precision);
        cJSON_AddNumberToObject(fld, "Latitude", fields[i].lat);
        cJSON_AddNumberToObject(fld, "Longitude", fields[i].lon);
        cJSON_AddStringToObject(fld, "Address", fields[i].address);
        cJSON_AddStringToObject(fld, "City", fields[i].city);
        cJSON_AddStringToObject(fld, "State", fields[i].state);
        cJSON_AddStringToObject(fld, "Zip", fields[i].zip);
        cJSON_AddStringToObject(fld, "Country", fields[i].country);
    }

    /*	cJSON_ReplaceItemInObject(cJSON_GetArrayItem(root,1),"City",cJSON_CreateIntArray(ids,4)); */
    out = cJSON_Print(root);
    cJSON_Delete(root);
    LOG_I(cjson_example, "%s\n", out);
    cJSON_free(out);
    return ret;
}


/**
  * @brief     Cjson test entry
  * @param     None
  * @return    0 -> Success
  */
int cjson_test(void)
{
    int ret = 0;
    /* Parse each JSON text block, then rebuild them back to JSON object and print out. */
    ret += doit((char *)text1);
    ret += doit((char *)text2);
    ret += doit((char *)text3);
    ret += doit((char *)text4);
    ret += doit((char *)text5);
    /* Execute the example code to build a number of JSON objects. */
    ret += create_objects();
    return ret;
}
