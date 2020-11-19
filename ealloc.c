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
    if(buf_size % 256 != 0)
        return NULL;
    else if(buf_size > PAGESIZE)
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
                    j=i;
                /*for( j=i;j<bufpage+i; j++){
                      if(j >= MAXPAGENUM)
                         break;
                  */                 

                    if(j== 0 && a[j].mmaped==0){
                         
                        for (int k=0; k< 1; k++) {
                            a[k].region=mmap(NULL, 16, PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,0,0);
                             if(a[k].region==MAP_FAILED)
                            return NULL;
                            a[k].mmaped=1;
                             a[k].free=0;
                        }
                    }
                    else if(j== 1 && a[j].mmaped==0){
                        for (int k=1; k< 2; k++) {
                            // printf("free:%d mmaped:%d j:%d bufpage:%d buf_size%d\n",a[k].free,a[k].mmaped,j,bufpage,buf_size);
                            a[k].region=mmap(NULL, 16, PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,0,0);
                             if(a[k].region==MAP_FAILED)
                            return NULL;
                            a[k].mmaped=1;
                             a[k].free=0;
                        }
                    }   
                    else if(j== 2 && a[j].mmaped==0){
                        for (int k=2; k< 3; k++) {
                            a[k].region=mmap(NULL, 256, PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,0,0);
                             if(a[k].region==MAP_FAILED)
                            return NULL;
                            a[k].mmaped=1;
                             a[k].free=0;
                        }
                    }  
                    else if(j== 3 && a[j].mmaped==0){
                        for (int k=3; k<4; k++) {
                            a[k].region=mmap(NULL, 256, PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,0,0);
                             if(a[k].region==MAP_FAILED)
                            return NULL;
                            a[k].mmaped=1;
                             a[k].free=0;
                        }
                    }         
                     
 //printf("2flag:%d mmap:%d free%d bufpage:%d j:%d  i:%d\n",flag,a[j].mmaped,a[j].free,bufpage,j,i);
                  
                   
             // }
              

                return a[i].region;//first fit
            }
        }
        else{
            i++;
           // printf("2flag:%d mmap:%d free%d bufpage:%d j:%d  i:%d\n",flag,a[i].mmaped,a[i].free,bufpage,j,i);
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