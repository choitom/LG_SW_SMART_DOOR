# cpp face recognition library compilation
g++ -fpic -shared smartdoor.cpp -o libsmartdoor.so `pkg-config --cflags --libs opencv`
sudo mv libsmartdoor.so /usr/lib/

# compile the main file
arm-linux-gnueabihf-gcc test.c -o test -lsmartdoor -lwiringPi 

# mount drive
sudo insmod ../../PROJECT_HYUNA/mod_piezo/mod_piezo.ko
sudo insmod ../../PROJECT_HYUNA/mod_sw_drv/mod_sw_drv.ko
