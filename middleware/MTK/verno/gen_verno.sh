#!/bin/bash

# show usage message
usage () {
    echo "Usage: $0 <template_file> <output_file> <sw_verno> <hw_verno>"
    echo ""
    echo "Example:"
    echo "       $0 verno.template verno.c SDK_V3.3.2 mt2523_hdk"
}

# set exit immediately if a command exits with a non-zero status.
set -e

# read arguments
template_file=$1
dest_file=$2
sw_verno=$3
hw_verno=$4
date_time=$(date +"%Y/%m/%d %H:%M:%S GMT %:z")

# check arguments
if [ -z "$1" ]; then usage; exit 1; fi
if [ -z "$2" ]; then usage; exit 1; fi
if [ -z "$3" ]; then usage; exit 1; fi
if [ -z "$4" ]; then usage; exit 1; fi
if [ ! -z "$MTK_VERNO_DATE_TIME" ]; then date_time=$MTK_VERNO_DATE_TIME; fi

# replace string.
sed  "s|\$DATE|${date_time}|g; \
      s|\$HW_VERNO|${hw_verno}|g" $template_file > $dest_file

