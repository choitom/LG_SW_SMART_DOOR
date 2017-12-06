APP  = $(notdir $(PWD))

default:
	arm-linux-gnueabihf-gcc -shared -fPIC -o lib$(APP).so $(APP).c 
clean :	
	rm -rf *.so

