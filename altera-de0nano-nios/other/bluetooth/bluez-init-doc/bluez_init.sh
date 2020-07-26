#!/bin/sh
# bluez_init.sh

function bt_reset() {
  echo 0 > /sys/class/rfkill/rfkill0/state
  sleep 1
  echo 1 > /sys/class/rfkill/rfkill0/state
  sleep 1
}

function brcm_start() {
  brcm_patchram_plus1 --bd_addr_rand --enable_hci --no2bytes --use_baudrate_for_download --tosleep 200000 --baudrate 1500000 --patchram /system/etc/firmware/BCM4356A2.hcd /dev/ttyS4 > /dev/null 2>&1 &
  sleep 5 # at least 3 till it finishes
}

function btd_start() {
  /usr/libexec/bluetooth/bluetoothd -C -n -E > /dev/null 2>&1 &
  sleep 1
}

function clear_all() {
  killall bluetoothd
  sleep 1
  killall brcm_patchram_plus1
  sleep 1
}

function find_btd() {
  ps | grep bluetoothd | grep -v grep > /dev/null 2>&1
  rc=$?
  if [ $rc -eq 0 ]; then
    echo ok
  else
    echo fail
  fi
}

function find_hci() {
  hciconfig -a | grep hci0 > /dev/null 2>&1
  rc=$?
  if [ $rc -eq 0 ]; then
    echo ok
  else
    echo fail
  fi
}

logf="/userdata/logs/blued"
if [ ! -d "$logf" ]; then
  mkdir -pv $logf
fi
logf="$logf/log-loop"

trap cleanup 1 2 3 6 15

function cleanup() {
  echo "Caught Signal ... cleaning up." >> $logf
  echo "Caught Signal ... cleaning up."
  clear_all
  echo "Done cleanup ... quitting." >> $logf
  echo "Done cleanup ... quitting."
  date  >> $logf
  exit 1
}


cnt=0
while true; do
    cnt=$(( $cnt + 1 ))
    echo '----------------------------' >> $logf
    echo cnt $cnt >> $logf
    echo cnt $cnt
    date  >> $logf

    clear_all

    bt_reset
    brcm_start
    btd_start   

    hci_ok=0

    wcnt=0
    while true; do
        wcnt=$(( $wcnt + 1 ))
        hci_chk=`find_hci`
        if [ "$hci_chk" == "ok" ]; then
            hci_ok=1
            echo '   ' hci_ok first at wcnt $wcnt >> $logf
            echo '   ' hci_ok first at wcnt $wcnt
            break
        fi
        sleep 1
    done

    if [ $hci_ok -eq 1 ]; then 
        hciconfig hci0 up
        sleep 1
        hciconfig hci0 piscan
        sleep 1

      wcnt=0
      while true; do
        wcnt=$(( $wcnt + 1 ))
        hci_chk=`find_hci`
        if [ "$hci_chk" != "ok" ]; then
            echo '   ' hci_ok failed at wcnt $wcnt >> $logf
            echo '   ' hci_ok failed at wcnt $wcnt
            break
        fi
        btd_chk=`find_btd`
        if [ "$btd_chk" != "ok" ]; then
            echo '   ' btd_ok failed at wcnt $wcnt >> $logf
            echo '   ' btd_ok failed at wcnt $wcnt
            break
        fi
        sleep 6
      done
    fi

    echo '   ' loop bottom >> $logf
    echo '   ' loop bottom

    clear_all
    date  >> $logf

    echo sleep 20
    sleep 20
done

# add to the end of  /etc/init.d/rcS :
# (source /etc/profile; sleep 10; /userdata/blued/bluez_init.sh) &


