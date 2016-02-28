#!/bin/bash

xdg-open ./images/out.ppm
WID=`xdotool search out | head -1`
echo $WID
sleep 0.75
#xdotool windowactivate --sync $WID
xdotool key ctrl+Super+Up

xdg-open ./images/in.ppm
WID=`xdotool search in | head -1`
echo $WID
sleep 0.75
#xdotool windowactivate --sync $WID
xdotool key ctrl+Super+Up
