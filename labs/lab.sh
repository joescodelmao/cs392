#!/bin/bash
# 

flag_h = 0
flag_l = 0
flag_p = 0

while getopts ":hlp" op; do

    case ${op} in
    h) flag_h = 1
        ;;
    l) flag_l = 1
        ;;
    p) flag_p = 1
        ;;
    *)
        echo Illegal option! >&2
        exit 1
    esac

done

msg=""
if [[ $flag_h -eq 1 ]]; then
    msg=${msg}"h"
fi

if [[ $flag_l -eq 1 ]]; then
    msg=${msg}"l"
fi

if [[ $flag_p -eq 1 ]]; then
    msg=${msg}"p"
fi

if[[ $(($flag_h + $flag_l + $flag_p )) eq -1 ]]; then
    msg="Flag "${msg}" is used"
else
    msg="Flags "${msg}" are used"
fi

echo $msg
exit 0






