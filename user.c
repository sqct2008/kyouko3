#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

int main(){
    int fp;

    fp = open("/dev/kyouko3", O_RDWR);
    getchar();
    close(fp);
    return 0;

}
