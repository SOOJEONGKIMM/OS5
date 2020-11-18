#include <stdio.h>
#include <string.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include "alloc.h"
#define MAXPAGENUM 512 //4096/8

int maxchunknum;
typedef struct Bitmap{
	int free;
	char *region;//start addr
    //size_t *chunk_start; 
	//size_t *chunk_end;
}Bitmap;
typedef struct Chunk{
    int chunksize;
    int startpagenum;
    int endpagenum;
    char *region;
    int free;  // free 1 used 0
}Chunk;

struct Bitmap a[MAXPAGENUM];
struct Chunk free_c[MAXPAGENUM];
struct Chunk used_c[MAXPAGENUM];

int init_alloc(){

	for(int i=0;i<MAXPAGENUM;i++){//allocate 4kb page with mmap()
		a[i].region=mmap(NULL, MINALLOC, PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,0,0);
	
		a[i].free=1;
       // free_c[i].free=1;

		if(a[i].region==MAP_FAILED)
			return 1;
       
	}

    /*free_c[0].chunksize=PAGESIZE;
    free_c[0].startpagenum=0;
    free_c[0].endpagenum=512-1;
    
    free_c[0].region = a[0].region;
    maxchunknum=1;*/
    
    return 0;
}
int cleanup(){
    int err;
    for(int i=0;i<MAXPAGENUM;i++){
         err=munmap(a[i].region, MINALLOC);
    }
    if(err!=0)
        return 1;
    return 0;
}
char *alloc(int buf_size){
    int flag;
    int i, j;
    if(buf_size % 8 != 0)
        return NULL;
   
    int bufpage = buf_size / MINALLOC ;
    for(i=0;i<MAXPAGENUM;i++){
       
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
                                   
                      // printf("debug j:%d bufpage:%d buf_size%d\n",j,bufpage,buf_size);
                    a[j].free=0;
                   
                }
                return a[i].region;//first fit
            }
        }
       
    }
    return NULL; //failed (no mem)
}
char *allocB(int buf_size){

    if(buf_size % 8 != 0)
        return NULL;

    for(int i=0;i<maxchunknum;i++){
       // if (free_c[i].free == 0)
         //   continue;
        if( (free_c[i].chunksize >= buf_size)){
            int startpgnum1 = free_c[i].startpagenum;
            int leftover = free_c[i].chunksize-buf_size;
            int endpgnum1 = free_c[i].endpagenum - leftover / 8 ;
            int startpgnum2 = endpgnum1 + 1;
            int endpgnum2 = free_c[i].endpagenum;

            // for remaining chunk
            free_c[i].startpagenum = startpgnum2;
            free_c[i].endpagenum = endpgnum2;  
            free_c[i].region = a[startpgnum2].region;
            free_c[i].chunksize = leftover;
            free_c[i].free = 1;

            // allocated chunk 
            free_c[maxchunknum].startpagenum = startpgnum1;
            free_c[maxchunknum].endpagenum = endpgnum1;
            free_c[maxchunknum].chunksize = buf_size;
            free_c[maxchunknum].free = 0;
            free_c[maxchunknum].region = a[startpgnum1].region;
            for(int j=startpgnum1;j<endpgnum1;j++){
                a[j].free =0;
            }
            
            maxchunknum++;
           
            return a[startpgnum1].region;
        }
    }


   

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