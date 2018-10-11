#!/bin/bash

set -e

BASEDIR=$(dirname "$0")
MERGE_BIN_FILE_NAME=$1
OUTPUT=$2


writehex  ()
{
    while [ $1 ]; do
        for ((i=0; i<${#1}; i+=2))
        do
            printf "\x${1:i:2}";
        done;
        shift;
    done
}

addBinInfo () 
{
    echo "add address and size info of bin file:" $2

    #get file name (without extension)
    filename=`echo "$2" | cut -d'.' -f1`

    awk \
    '{ \
        if ($0 ~ /Name=.+/) \
        {
            if ($0 ~ /Name='$filename'/) \
            {
                find=1 \
            }
            else \
            { \
                find=0 \
            } \
        } \
    \
        if (find==1 && match($0,/TargetAddress=(\w*)/,m)) \
        { \
            print 'm[1]' \
        } \
    }' $1 > $OUTPUT/merge_bin_temp/$2.temp

    # write address
    address=`cat $OUTPUT/merge_bin_temp/$2.temp | cut -d 'x' -f2`
    echo "address =" $address 
    writehex $address > $OUTPUT/merge_bin_temp/$2.temp

    # write size
    size=`wc --bytes $BASEDIR/$2 | cut -d ' ' -f1`
    echo "size =" $size
    size_hex=$(printf "%.8x" $size)
    writehex $size_hex >> $OUTPUT/merge_bin_temp/$2.temp

    # write bin data
    #echo "the output file is:" $OUTPUT/merge_bin_temp/$2.temp
    cat $BASEDIR/$2 >> $OUTPUT/merge_bin_temp/$2.temp
}

# create temp folder to store the temp bin file
mkdir -p $OUTPUT/merge_bin_temp
addBinInfo $BASEDIR/external_dsp.ini iram0.bin
addBinInfo $BASEDIR/external_dsp.ini dram0.bin
addBinInfo $BASEDIR/external_dsp.ini dram1.bin

# merge three bin file
cat $OUTPUT/merge_bin_temp/iram0.bin.temp $OUTPUT/merge_bin_temp/dram0.bin.temp $OUTPUT/merge_bin_temp/dram1.bin.temp > $OUTPUT/$MERGE_BIN_FILE_NAME
echo "merge bin done, output path: $OUTPUT/$MERGE_BIN_FILE_NAME"

# remove temp file
rm -rf $OUTPUT/merge_bin_temp

