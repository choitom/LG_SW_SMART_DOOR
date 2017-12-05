#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include "../mod_sw_drv_shared/mod_sw_drv_shared.h"

void sw_callback(int sw){
	printf("sw = %d\n", sw);
}

int main(int argc, char * argv[]) 
{
	set_callback(sw_callback);

	while (1) 
		pause();

	return 0;
}
