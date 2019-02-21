# MP1

step1:
In folder MP1, run "make" to generate kernel module nwu10.ko and userapp

step2:
run "sudo insmod nwu10.ko" to insert module to the kernel

step3:
run "./userapp" to register it to the kernel module

step4:
run "./userapp" in another terminal to register

step5:
run "cat /proc/mp1/status" to see the process PID and cpu time