#include "userapp.h"

int main(int argc, char* argv[])
{
	int pid;
	pid = getpid();
    char cmd[100];
    sprintf(cmd, "echo %d > /proc/mp1/status", pid);
    system(command);
	return 0;
}
