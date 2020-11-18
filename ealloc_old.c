#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include "alloc.h"
#define MAXPAGENUM 16 //4096/256 (PAGESIZE/MINALLOC)

typedef struct Bitmap{
	int free;
    int mmaped;
	char *region;//start addr
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
    /*int err;
    for(int i=0;i<MAXPAGENUM;i++){
         err=munmap(a[i].region, MINALLOC);
    }
    if(err!=0)
        return 1;*/
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
    else if(buf_size > PAGESIZE * 4)
        return NULL;
   
    int bufpage = buf_size / 256;

    for(i=0;i<MAXPAGENUM;i++){//check usable chunk                           
        if(a[i].free==1){//} && a[i].mmaped==1){
              
            flag=1;
            for( j=i; j< bufpage+i; j++){
                if(j >= MAXPAGENUM)
                    break;

                if(a[j].free!=1){
                    flag=0;
                }           
            }
            if(i+bufpage >= MAXPAGENUM){
                return NULL;
            }
            if(flag){//chunk exists
  
                for(j=i;j<bufpage+i; j++){
                    printf("1~~buf_size:%d bufpage:%d j:%d  i:%d\n",buf_size,bufpage,j,i);
                    if(j >= MAXPAGENUM)
                        break;
                    
                    a[j].free=0;
                    if(a[j].mmaped==0){
                        a[j].region=mmap(NULL, 256, PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,0,0);

                        if(a[j].region==MAP_FAILED)
                            return NULL;
                        a[j].mmaped=1;

                    }
                    
                }
                
                return a[i].region;//first fit
            }
        } 
        else {//used 
          
            /*if(a[i].mmaped==1){
                    for(j=i;j<bufpage+i; j++){
                      
                        a[j].free=0;
                    
                    }
            }
            else{*/
                for(j=i;j<bufpage+i; j++){
                    //printf("2flag:%d mmap:%d buf_size:%d bufpage:%d j:%d  i:%d\n",flag,a[j].mmaped,buf_size,bufpage,j,i);
                    a[j].region=mmap(NULL, 256, PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,0,0);

                    if(a[j].region==MAP_FAILED)
                        return NULL;
                    a[j].mmaped=1;
                    a[j].free=0;
                }
          //  }
          //  }
            //return a[i].region;

        }   
    }
 
    return NULL; //failed (no mem)
}

void dealloc(char *memchunk_ptr){

    for(int i=0;i<MAXPAGENUM; i++){
        if(memchunk_ptr == a[i].region){
            int j=i;
            while(a[j].free==0  && j<MAXPAGENUM){
                a[j].free=1;
                j++;
            }
        }
    }
}