g++ -fpic -shared record.cpp -o librecord.so `pkg-config --cflags --libs opencv`
sudo cp librecord.so /usr/lib/
arm-linux-gnueabihf-gcc test.c -o test -lrecord
