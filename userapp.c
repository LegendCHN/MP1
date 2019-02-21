#include "userapp.h"

int factorial(int n)
{
    int ret = 1;
    for(int i = 1; i <= n; i++)
        ret *= i;
    return ret;
}

int main(int argc, char* argv[])
{
	int pid;
	pid = getpid();
    char cmd[100];
    sprintf(cmd, "echo %d > /proc/mp1/status", pid);
    system(cmd);
    for(int i=0; i<100000; i++)
    	factorial(50000);
	return 0;
}
