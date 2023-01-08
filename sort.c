#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#define LEN 7000
#define MAX 255

typedef void sort_list(void *);
typedef void print_list(void *);
typedef void fill_list(void *,int);

typedef struct List
{
    int len;
    u_int8_t * data;
    sort_list *sort;
    print_list *print;
    fill_list* fill;

}List;

void printList(void *);
void fillList(void*,int);
void selectionSort(void*);
void countingSort(void *);

List * newList(int,sort_list*);
int64_t timespecDiff(struct timespec *, struct timespec *);
void benchmark(List*,int );

int main(int argc, char*argv[] ){
    assert(argc==3);
    int len = LEN;
    List * list;

    if( !strcmp(argv[1],"c")){
        printf("Testing with Counting Sort %d times\n",atoi(argv[2]));
        list = newList(len,countingSort);
    }else if(!strcmp(argv[1],"s")){
        printf("Testing with Selection Sort %d times\n",atoi(argv[2]));
        list = newList(len,selectionSort);
    }
    else{ 
        return 1;
    }

    benchmark(list,atoi(argv[2]));
    return 0;
}

void benchmark(List*list,int repeat){

    int count =0;
    uint64_t sum=0,max=0,min=INT64_MAX;
    uint64_t timeElapsed; 
    for(count=0;count<repeat;count++){

        int maxValue = MAX;
        struct timespec start, end;

        list->fill(list,maxValue);
        //printf("\nUnsorted:\n");
        //list->print(list);
        
        clock_gettime(CLOCK_MONOTONIC, &start);
        list->sort(list);
        clock_gettime(CLOCK_MONOTONIC, &end);
        
        //printf("\nSorted:\n");
        // list->print(list);
        timeElapsed = timespecDiff(&end, &start);    
    
        timeElapsed /= 1000L;

       //printf("Time measured: %ld microsseconds.\n", timeElapsed);
        sum+=timeElapsed;
        if(timeElapsed>max)
            max = timeElapsed;
        if(timeElapsed<min)
            min = timeElapsed;
    }

    printf("Total time: %ld microsseconds\n",sum);
    printf("Average time: %ld microsseconds\n",sum/repeat);
    printf("Max time: %ld microsseconds\n",max);
    printf("Min time: %ld microsseconds\n",min);
    return;
}

void selectionSort(void *list){
    int i,j,aux,i_min=0;
    List * l = (List*)list;
    for(i =0;i<l->len;i++){
        i_min=i;
        for(j=i+1;j<l->len;j++){
            if(*(l->data+j)<*(l->data+i_min))
                i_min = j;
        }

        aux = *(l->data+i);
        *(l->data+i)=*(l->data+i_min);
        *(l->data+i_min)=aux;
    }
}

void fillList(void * list, int max){
    srand(time(NULL));
    int i;
    List * l = (List*)list;
    for(i=0;i<l->len;i++){
        *(l->data+i) = rand()%max;
    }
}

void printList(void * list){
    List * l = (List *)list;
    int i=0;
    while(i<l->len){
        printf("%d\n",*(int *)(l->data+i++));
    }
}

void countingSort(void * list) {
    int output[LEN];
    List * l = (List*)list;
    // Find the largest element of the array
    int max = *(l->data);
    for (int i = 1; i < l->len; i++)
    {
        if (*(l->data + i) > max)
            max = *(l->data + i);
    }

    // The size of count must be at least (max+1) but
    // we cannot declare it as int count(max+1) in C as
    // it does not support dynamic memory allocation.
    // So, its size is provided statically.
    int count[LEN];

    // Initialize count array with all zeros.
    for (int i = 0; i <= max; ++i)
    {
        count[i] = 0;
    }

    // Store the count of each element
    for (int i = 0; i < l->len; i++)
    {
        count[*(l->data + i)]++;
    }

    // Store the cummulative count of each array
    for (int i = 1; i <= max; i++)
    {
        count[i] += count[i - 1];
    }

    // Find the index of each element of the original array in count array, and
    // place the elements in output array
    for (int i = l->len - 1; i >= 0; i--)
    {
        output[count[*(l->data + i)] - 1] = *(l->data + i);
        count[*(l->data + i)]--;
    }

    // Copy the sorted elements into original array
    for (int i = 0; i < l->len; i++)
    {
        *(l->data + i) = output[i];
    }
}

List * newList(int len,sort_list* sort){
    List * l = (List *) malloc(sizeof(List));
    assert(l);
   
    l->data=NULL;
    l->data=(u_int8_t*)malloc(sizeof(u_int8_t)*len);
    assert(l->data);
    
    l->len=len;
    l->fill=fillList;
    l->print=printList;
    l->sort=selectionSort;
    if(sort)
        l->sort=sort;
    
    return l;
}

int64_t timespecDiff(struct timespec *timeA_p, struct timespec *timeB_p)
{
  return ((timeA_p->tv_sec * 1000000000) + timeA_p->tv_nsec) -
           ((timeB_p->tv_sec * 1000000000) + timeB_p->tv_nsec);
}