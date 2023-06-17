#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <syslog.h>

int main(int argc, char ** argv){

    if(argc != 3){     
        syslog(LOG_ERR, "writer: incorrect number of arguments");
        exit(1);
    }
    

    char * path = argv[1]; 
    char * str = argv[2];

    FILE * fp = fopen(path,"w");
    if( fp == NULL){
        syslog(LOG_ERR,"writer: Failed to open file %s",path);
        exit(1);
    }

    fprintf(fp,"%s",str);
    syslog(LOG_DEBUG,"Writing %s to %s", str, path);

}