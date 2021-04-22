# Crazyflie UART test app

This is an app for the crazyflie which simply prints any character it receives over UART2.  You can specify the location of your crazyflie firmware by editing the Makefile.

See App layer API guide [here](https://www.bitcraze.io/documentation/repository/crazyflie-firmware/master/userguides/app_layer/)

## Build

Make sure that you are in the app_hello_world folder (not the main folder of the crazyflie firmware). Then type the following to build and flash it while the crazyflie is put into bootloader mode:

```
make clean
make 
make cload
```

If you want to compile the application elsewhere in your machine, make sure to update ```CRAZYFLIE_BASE``` in the **Makefile**.