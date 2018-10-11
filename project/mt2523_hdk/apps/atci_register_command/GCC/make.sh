#!/bin/bash

###############################################################################
#Variables
#BL max size is 64K
BOOTLOADERMAXSIZE=65536
FREERTOSBIN=./Build/atci_register_command.bin
FLASHBIN=./Build/flash.bin
BOOTLOADERBIN=./Build/mt2523_bootloader.bin
BOOTLOADERBIN_TMP=./Build/bootloader_tmp.bin



###############################################################################
#Functions
function show_usage {
	echo "Usage: $0 [make | clean | cat]"
	echo "$0 make : build load"
	echo "$0 clean : clean all."
	echo "$0 cat : cat binary and bootloader to one binary."
	echo "******************************************"
	
}


function make_and_combine_binary {
	make

	if [ -e "$FLASHBIN" ]; then
		echo "delete $FLASHBIN"
			rm $FLASHBIN
	fi

	if [ -e "$BOOTLOADERBIN" ]; then
		echo "cat binary begin"
		cp $BOOTLOADERBIN $BOOTLOADERBIN_TMP
		BOOTLOADERBINSIZE=$(stat -c "%s" $BOOTLOADERBIN)
		PADDINGCOUNT=$(($BOOTLOADERMAXSIZE-$BOOTLOADERBINSIZE))
		dd if=/dev/zero ibs=1 count="$PADDINGCOUNT" | tr "\000" "\377" >> $BOOTLOADERBIN_TMP
		cat $BOOTLOADERBIN_TMP $FREERTOSBIN > $FLASHBIN
		echo "cat binary done"
		rm $BOOTLOADERBIN_TMP
	else
		echo "error: no mt2523_bootloader.bin exist"
		exit -1
	fi

}

function cat_binary {
	
	if [ -e "$FLASHBIN" ]; then
		echo "delete $FLASHBIN"
			rm $FLASHBIN
	fi

	if [ -e "$BOOTLOADERBIN" ]; then
		echo "cat binary begin"
		cp $BOOTLOADERBIN $BOOTLOADERBIN_TMP
		BOOTLOADERBINSIZE=$(stat -c "%s" $BOOTLOADERBIN)
		PADDINGCOUNT=$(($BOOTLOADERMAXSIZE-$BOOTLOADERBINSIZE))
		dd if=/dev/zero ibs=1 count="$PADDINGCOUNT" | tr "\000" "\377" >> $BOOTLOADERBIN_TMP
		cat $BOOTLOADERBIN_TMP $FREERTOSBIN > $FLASHBIN
		echo "cat binary done"
		rm $BOOTLOADERBIN_TMP
	else
		echo "error: no mt2523_bootloader.bin exist"
		exit -1
	fi
}

###############################################################################
#Begin here
if [ "$#" == "0" ]; then
	show_usage
	exit
elif [ "$1" == "make" ]; then
	make_and_combine_binary
	exit
elif [ "$1" == "clean" ]; then
	make clean
	exit
elif [ "$1" == "cat" ]; then
	cat_binary
	exit	
fi

###############################################################################
#Perl script start




	

