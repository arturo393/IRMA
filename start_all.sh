#!/bin/bash
DIR=$HOME"/Dropbox/IRMAIII_oficial" 
sudo chown irma /dev/ttyACM0
sudo chown irma /dev/ttyUSB0
xterm -e "$DIR/src/init/init-cda" &
sleep 1
xterm -e "$DIR/src/processors/monitor/monitor" &
sleep 1
xterm -e "$DIR/src/processors/closeRangeNavigator/closeRangeNavigator" & 
sleep 1
#term -e "gdb $DIR/src/processors/longRangeNavigator/longRangeNavigator" &
sleep 1
xterm -e "$DIR/src/processors/executive/executive" "0" &
sleep 1
cd ./laser_output
$DIR/src/processors/laser/laser

