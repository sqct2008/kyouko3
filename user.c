#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include <sys/mman.h>

struct u_kyouko_device {
    unsigned int *u_control_base;

}kyouko3;

#define KYOUKO3_CONTROL_SIZE 65536
#define Device_RAM 0x0020

unsigned int U_READ_REG(unsigned int rgister)
{
    return (*(kyouko3.u_control_base + (rgister>>2)));
}

int main(){
    int fp;
    int result;

    fp = open("/dev/kyouko3", O_RDWR);
    getchar();
    kyouko3.u_control_base = mmap(0,KYOUKO3_CONTROL_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fp, 0);
    result = U_READ_REG(Device_RAM);
    printf("Ram size in MB is : %d\n", result);
    close(fp);
    return 0;

}
