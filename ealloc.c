#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include "alloc.h"
#define MAXPAGENUM 64 //4096/8


typedef struct Bitmap{
	char free;
	char *region;//start addr
    int mmaped;
}Bitmap;


struct Bitmap a[MAXPAGENUM];

int init_alloc(){

	for(int i=0;i<MAXPAGENUM;i++){
	
		a[i].free=1;
        a[i].mmaped=0;

       
	}
    
    return 0;
}

int cleanup(){
    for(int i=0;i<MAXPAGENUM;i++){
	
		a[i].free=1;
        a[i].mmaped=0;

       
	}
    
    return 0;
}
char *alloc(int buf_size){
    int flag;
    int i, j;
    if(buf_size % 8 != 0)
        return NULL;
    else if(buf_size > PAGESIZE * 4)
        return NULL;

    int bufpage = buf_size / 256 ;
    i = 0;
    while (i<MAXPAGENUM){
       
        if(a[i].free==1){
 
            flag=1;
            for( j=i; j< bufpage+i; j++){
                   if(j >= MAXPAGENUM)
                break;
 
                if(a[j].free!=1){
                    flag=0;
                }
             
            }
            if(flag){//allocate
                       
                for( j=i;j<bufpage+i; j++){
                      if(j >= MAXPAGENUM)
                         break;
                                   
                    //   printf("debug j:%d bufpage:%d buf_size%d\n",j,bufpage,buf_size);
                    a[j].free=0;

                    if(j== 0 && a[j].mmaped==0){
                        for (int k=0; k< 16; k++) {
                            a[k].region=mmap(NULL, 256, PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,0,0);
                             if(a[k].region==MAP_FAILED)
                            return NULL;
                            a[k].mmaped=1;
                        }
                    }
                    else if(j== 16 && a[j].mmaped==0){
                        for (int k=16; k< 32; k++) {
                            a[k].region=mmap(NULL, 256, PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,0,0);
                             if(a[k].region==MAP_FAILED)
                            return NULL;
                            a[k].mmaped=1;
                        }
                    }   
                    else if(j== 32 && a[j].mmaped==0){
                        for (int k=32; k< 48; k++) {
                            a[k].region=mmap(NULL, 256, PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,0,0);
                             if(a[k].region==MAP_FAILED)
                            return NULL;
                            a[k].mmaped=1;
                        }
                    }  
                    else if(j== 48 && a[j].mmaped==0){
                        for (int k=48; k< 64; k++) {
                            a[k].region=mmap(NULL, 256, PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,0,0);
                             if(a[k].region==MAP_FAILED)
                            return NULL;
                            a[k].mmaped=1;
                        }
                    }         
                     
 //printf("2flag:%d mmap:%d free%d bufpage:%d j:%d  i:%d\n",flag,a[j].mmaped,a[j].free,bufpage,j,i);
                  
                   
                }
              

                return a[i].region;//first fit
            }
        }
        else{
            i++;
          //  printf("2flag:%d mmap:%d free%d bufpage:%d j:%d  i:%d\n",flag,a[i].mmaped,a[i].free,bufpage,j,i);
        }
       
    }
    return NULL; //failed (no mem)
}

void dealloc(char *memchunk_ptr){

    for(int i=0;i<MAXPAGENUM; i++){
        if(memchunk_ptr == a[i].region){
            int j=i;
            while(a[j].free==0 && j<MAXPAGENUM){
                a[j].free=1;
                j++;
            }
        }
    }
}