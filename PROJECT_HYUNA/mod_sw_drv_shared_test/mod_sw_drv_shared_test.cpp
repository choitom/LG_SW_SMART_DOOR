#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#define SIG_CALLBACK 44
#define NODE_NAME "/dev/mod_sw_drv"
extern "C"{
int set_callback(void (*pfn_callback)(int));
int fd;
static void (*g_pfn_callback)(int);
void __attribute__((constructor)) init_callback(void)
{
	fd = open(NODE_NAME, O_WRONLY);
}
void __attribute__((destructor)) exit_callback(void)
{
	if(fd > 0){
		close(fd);
	}
}
static void receive_signal(int n, siginfo_t *info, void *unused)
{
	g_pfn_callback(info->si_int);
}
int set_callback(void (*pfn_callback)(int))
{

	int ret = 0, pid;
	struct sigaction sig;
	
	sig.sa_sigaction = receive_signal;
	sig.sa_flags = SA_SIGINFO;
	sigaction(SIG_CALLBACK, &sig, 0);
	g_pfn_callback = pfn_callback;
	pid = getpid();

	if(fd > 0){

		ret = write(fd, &pid, sizeof(pid));
	}
	return ret;
}
}

void sw_callback(int sw){

	printf("sw = %d\n", sw);
}

int main(int argc, char * argv[]) 
{
	
	printf("hello main\n");
	set_callback(sw_callback);

	while (1) 
{
}
//		pause();

	return 0;
}
