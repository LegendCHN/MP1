#include "userapp.h"

int main(int argc, char* argv[])
{
	int pid;
	pid = getpid();
    char cmd[100];
    sprintf(cmd, "echo %u > /proc/mp1/status", pid);
    system(command);
	return 0;
}
