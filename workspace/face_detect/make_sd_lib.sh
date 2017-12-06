g++ -fpic -shared smartdoor.cpp -o libsmartdoor.so `pkg-config --cflags --libs opencv`
sudo cp libsmartdoor.so /usr/lib/
#arm-linux-gnueabihf-gcc test2.c -o test -lsmartdoor
arm-linux-gnueabihf-gcc test.c -o test -lsmartdoor
