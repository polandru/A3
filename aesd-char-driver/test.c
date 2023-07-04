#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include "aesd_ioctl.h"

int main(void){
	struct aesd_seekto seekto;
	seekto.write_cmd =1;
	seekto.write_cmd_offset=2;

	FILE* f = fopen("/dev/aesdchar", "w+");
	char str[1000];
	fread(str,1,5,f);
	printf("%s",str);
	ioctl(fileno(f),AESDCHAR_IOCSEEKTO,&seekto);
	char str2[10000];
	int r2 = fread(str2,1,1000,f);
	printf("%s %d",str2,r2);
	
}
