#! /bin/sh

mode="664"
module="pwm"

rm -f /dev/${module}*

major=$1

mknod /dev/${module}1_enable c $major 0
mknod /dev/${module}1_freq c $major 1
mknod /dev/${module}1_duty c $major 2
mknod /dev/${module}2_enable c $major 3
mknod /dev/${module}2_freq c $major 4
mknod /dev/${module}2_duty c $major 5

chmod $mode /dev/${module}*