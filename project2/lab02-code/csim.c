/**csim.c
 * Name: Shengyuan Hou    Student ID:518021910604
 * */
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include "cachelab.h"

#define SIZE 100

int s,E,b,m=64;
char *tracefile;
int verbosemode=0,help=0;                               //help indicates whether the usage should be displayed.
int hit_count=0,miss_count=0,eviction_count=0;
int clock=0;                                            //simulate a clock and every cache access will make it plus 1.

struct block                                            // struct block simulates a line in the cache
{
   int valid;                                           // valid bit
   long int tag;                                        // tag 
   int time;                                            // Since we care about the address rather than real data,
                                                        // we discard the storing data in cache. 
                                                        // We add an integer time here to record the recent access time for LRU.
};

struct block *cache;

void eviction(long int address,int size);        //LRU replacement
void Load(long int address,int size);            //L operation
void cache_access(long int address,int size);    //cache access of load or store operation
void Store(long int address,int size);           //S operation
void modify(long int address,int size);          //M operation

int main(int argc, char *argv[])
{
    int o;
    const char *optstring="hvs:E:b:t:";                 //optstring stores the name and type of every parameter
    while((o=getopt(argc,argv,optstring))!=-1)          //acquire the parameters by using getopt()
    {
        switch(o)
        {
         case 'h':help=1;break;
         case 'v':verbosemode=1;break;
         case 's':s=atoi(optarg);break;
         case 'E':E=atoi(optarg);break;
         case 'b':b=atoi(optarg);break;
         case 't':tracefile=optarg;break;
         case '?':
         printf("error optopt: %c\n",optopt);
         printf("error opterr: %c\n",opterr);
         break;
        }
    }
    cache=malloc(sizeof(struct block)*E*(1<<s));      //cache size=B*E*S=B*E*(2^s)
    memset(cache,0,sizeof(struct block)*E*(1<<s));    //initialize the cache
    FILE *in;
    char task[100],x[100],y[100];                    
    char *end;
    char op;                                         //operation type
    int size;                                        //request size
    long int address;                                // access address
    in=fopen(tracefile,"r");
    while(fgets(task,SIZE,in))                       //read in the trace file and get the resolution
    {
       if(task[0]=='I') continue;                    //If it is an I operation, ignore it.
       sscanf(task," %c %s",&op,x);                  //read in the operation
       sscanf(x,"%[^,]",y);
       int st=0;
       for(;x[st]!=',';st++);                        //read in the request size 
       sscanf(x+st+1,"%d",&size);
       address=strtol(y,&end,16);                    //read in the request address
       switch(op)                                    //select the operation
       {
         case 'M':modify(address,size);break;
         case 'L':Load(address,size);break;
         case 'S':Store(address,size);break;
       } 
    }
    if(help==1)                                            //if parameter h is 1 ,print out the usage information.
       printf("age: ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file>\nOptions:\n"
"  -h         Print this help message.\n  -v         Optional verbose flag.\n  -s <num>   Number of set index bits.\n"
"  -E <num>   Number of lines per set.\n  -b <num>   Number of block offset bits.\n"
"  -t <file>  Trace file.\n\nExamples:\n  linux>  ./csim-ref -s 4 -E 1 -b 4 -t traces/yi.trace\n"
"  linux>  ./csim-ref -v -s 8 -E 2 -b 4 -t traces/yi.trace\n");         
    printSummary(hit_count, miss_count, eviction_count);  //print out the hit count, miss count and eviction count  
    free(cache);                                          //free the memory space of cache
    return 0;
}
void eviction(long int address,int size)                  // This function realizes LRU cache replacement algorithm
{
   eviction_count++;
   if(verbosemode==1)
    printf(" eviction");
   int set=(address>>b)&((1<<s)-1),mintime=0xffffff,i,evict;  //evict records the least recently used line number
   for(i=0;i<E;i++)                                       //find the least recently used cache line
   {
      if(cache[set*E+i].time<mintime)
        mintime=cache[set*E+i].time,evict=i;   
   }
   cache[set*E+evict].valid=1;                           //replace the cache line
   cache[set*E+evict].tag=(address>>(b+s));
   cache[set*E+evict].time=clock++;    
}
void Load(long int address,int size)                     //This fuction simulates the load operation               
{
   unsigned long int tmp=address;
   if(verbosemode==1)                                    //If the verbosemode is 1, print the access information.
   printf("L %lx,%d",tmp,size);
   cache_access(address,size);                           //cache access of load
   printf("\n");
}
void cache_access(long int address,int size)             //This function simulates the cache access in both load and store operation
{                                                        //(actually they have the same cache access operation).
    int i,flag=0;                                        //flag indicates wether the data is found in the cache
    for(i=0;i<E;i++)                                     // this for loop checks whether the data is already in the cache
    {
       if(cache[((address>>b)&((1<<s)-1))*E+i].valid==1&&cache[((address>>b)&((1<<s)-1))*E+i].tag==(address>>(s+b))) //if validbit is set and tag matches, 
       {                                                                                                             //directly access it
          hit_count++;
          flag=1;
          cache[((address>>b)&((1<<s)-1))*E+i].time=clock++;   //record the access time
          if(verbosemode==1)
          printf(" hit");
          break;   
       }
    }
    if(!flag)                                                 // if data is not found in the cache
    {
       miss_count++;
       if(verbosemode==1)
       printf(" miss");
       int add=0;
       for(i=0;i<E;i++)                                      //calculate the corresponding set index and check whether it is empty
       { 
          if(cache[((address>>b)&((1<<s)-1))*E+i].valid==0)  //If it is empty, add it directly 
          {
           cache[((address>>b)&((1<<s)-1))*E+i].valid=1;
           cache[((address>>b)&((1<<s)-1))*E+i].tag=(address>>(b+s));
           cache[((address>>b)&((1<<s)-1))*E+i].time=clock++;
           add=1;break;      
          }  
       }
       if(!add) eviction(address,size);                      //If it is not empty, do the line replacement.
    }     
}
void Store(long int address,int size)                   //This function simulates store operation
{
  unsigned long int tmp=address;
  if(verbosemode==1)                                    //If verbosemode=1, print out the access information.
    printf("S %lx,%d",tmp,size);
  cache_access(address,size);                           //cache access of store
  printf("\n");
}

void modify(long int address,int size)                 //This function simulates the modify operation
{
    unsigned long int tmp=address;
    if(verbosemode==1)                                 //If verbosemode=1, print out the access information
    printf("M %lx,%d",tmp,size);                  
    cache_access(address,size);                        //cache access of load
    cache_access(address,size);                        //cache access of store
    printf("\n");     
} 
