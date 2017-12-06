APP  = $(notdir $(PWD))

default:
	arm-linux-gnueabihf-gcc -o $(APP) $(APP).c -L../mod_piezo_shared/ -lmod_piezo_shared
clean :	
	rm -rf $(APP)


