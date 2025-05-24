#!/bin/bash

#ADC Init config
devmem 0x43c00304 32 0x4f0f # Sequence mode simulatenous
devmem 0x43c00308 32 0x800  # Configure divider to 8 and disable PD of both ADC
devmem 0x43c00320 32 0x00   # Disable internal channels
devmem 0x43c00324 32 0x01   # Enable auxiliary channels 0 and 8
devmem 0x43c00330 32 0x00   # Disable config for internal channels
devmem 0x43c00334 32 0x09   # Enable bipolar for channel 0 and 8

/home/root/IPC_Daemon &
sleep 2
/home/root/servertcp 4998 &
sleep 2
/home/root/radar_control_app &
