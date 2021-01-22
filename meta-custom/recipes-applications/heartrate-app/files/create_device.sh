#!/bin/bash
DEV_NAME="/dev/ppg"
NODE_ID=$(cat /proc/devices | grep ppg | awk '{ print $1 }')
mknod ${DEV_NAME} c ${NODE_ID} 0


