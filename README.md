# RPI_PCD8544_Display
PCD8544 Display code for Raspberry Pi

### command for compile
Before compile, you need install wiringPi library first (https://github.com/WiringPi/WiringPi)
```
cc -o systemInfo systemInfo.c PCD8544.c -L/usr/local/lib -lwiringPi
```

PCD8544 Driver from https://github.com/Morlok8k/raspPi_PCD8544_LCD

### Output
IP address

Up Time

CPU Usage

RAM Usage

CPU Temperature

```
xxx.xxx.xxx.xxx
SYSTEM INFO
______________
UP   xxx MIN.
CPU  x%
RAM  xxx MB
TEMP xx.xx C
```
