# cpp face recognition library compilation
g++ -fpic -shared smartdoor.cpp -o libsmartdoor.so `pkg-config --cflags --libs opencv`
sudo mv libsmartdoor.so /usr/lib/

# compile the main file
arm-linux-gnueabihf-gcc test.c -o test -lsmartdoor 
