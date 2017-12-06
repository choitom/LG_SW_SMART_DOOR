#include <unistd.h>
#include <stdlib.h>

#include <fcntl.h>
#include <signal.h>

#include "mod_piezo_shared.h"

#define SIG_CALLBACK 44
#define NODE_NAME "/dev/mod_piezo"

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
	sigaction(SIG_CALLBACK, &sig, (void*)0);

	g_pfn_callback = pfn_callback;
	pid = getpid();

	if(fd > 0){
		ret = write(fd, &pid, sizeof(pid));
	}

	return ret;
}
