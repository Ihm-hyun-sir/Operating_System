#include "types.h"
#include "user.h"
#include "param.h"
#include "fcntl.h"
#include "stddef.h"

#define MEMORY_SIZE 4 * 1024

int main(int argc, char**argv){
   printf(1,"test\n");
   int num1;
   int num2;
   for (int i=0;i<700;i++){
	 swapstat(&num1,&num2);
         printf(1,"read write %d %d\n",num1,num2);
        char *memory_start = (char *)sbrk(MEMORY_SIZE);
	memory_start = "hello my friends";
        if (memory_start == (char *)-1) {
            printf(1, "Failed to allocate memory\n");
            exit();
        }
      if (i % 10 == 0){
	     // swapstat(num1,num2);
	     // printf("read : %d write :%d \n",num1,num2);
      }
   }
   printf(1,"test end\n");
   exit();
}
/*
int main () {
    // infFork(50);
    int a, b;
    int* testlist[620];
    for(int i=0; i<620; ++i){
        testlist[i] = (int*)malloc(sizeof(int)*1024);
        // printf(1, "i:%d, alloc addr%p\n", i, testlist[i]);
        testlist[i][0] = (i+77);
        // printf(1, "alloc: %d\n", *testlist[i]);
        // swapstat(&a, &b);
        // if(a+b>0) printf(1, "swapout %d %d\n", a, b);
    }
    printf(1, "answer: %d\n", testlist[6][0]);
    // printf(1, "should page fault\n");
    swapstat(&a, &b);
    // testlist[0][0] = 0;
    printf(1, "swap %d %d\n", a, b);
    // printf(1, "end\n");
    exit();
}*/
