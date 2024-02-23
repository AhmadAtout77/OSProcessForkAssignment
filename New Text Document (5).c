#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>


int main() {

  int N = 118228 ;     //my student ID = 11822841 -> /100 = 118228.
  int size = N * sizeof(double); // size = 472912 bytes.
  double result[N];
  char *name1 = "SHA1";
  char *name2 = "SHA2";
  int i;
  int j;
  int pid;
  int f = 0;
  clock_t start1,start2,end1,end2;
  double part1_cpu_time;
  double part2_cpu_time;
  

  int fd1;
  int fd2;
  double *sharedArray1;
  double *sharedArray2;

  fd1 = shm_open(name1, O_CREAT | O_RDWR, 0666);
  fd2 = shm_open(name2, O_CREAT | O_RDWR, 0666);

  ftruncate(fd1, size);
  ftruncate(fd2, size);

  sharedArray1 = (double *) mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd1, 0);
  sharedArray2 = (double *) mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd2, 0); 

  for(i = 0; i < N; i ++) {
    sharedArray1[i] = (double)rand()/(double)(RAND_MAX/N);
    sharedArray2[i] = (double)rand()/(double)(RAND_MAX/N);
  }
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  start1 = clock();
  for(i = 0; i < N; i++) {
    result[i] = sharedArray1[i] + sharedArray2[i];
                                                                  //PART 1: addition is done in the parent process and stored in an
                                                                    // array called result[N] in the parent process  
  }
  end1 = clock();
  part1_cpu_time = (double) (end1 - start1)/ CLOCKS_PER_SEC;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//PART 2: Parallel addition and storing the result in a shared array
// since N = 118228 which is equal to (118226 + 2)
// M = last digit of my student ID + 5 -----> 1+5 = 6 --> M = 6
// 118224/6 = 19704 ---> each child process will perform (19704 addition operation)
// last child process will perform (19704 + 4) --> (19708 addition operation)

  char *name3 = "result";
  int fd3;
  double *sharedResultArray;
  fd3 = shm_open(name3, O_CREAT | O_RDWR, 0666);
  ftruncate(fd3, size);
  sharedResultArray = (double *) mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd3, 0);


  start2 = clock();
  for( i = 0; i < N; i++){
        pid = fork();
        if(pid == 0) { 
             for( j = i*N; j < (i+1)*N; j++) {
                 sharedResultArray[j] = sharedArray1[j]+sharedArray2[j];
             }
         exit(0);
        }   

  }  
  while (wait(NULL) != -1) {
     
}
 end2 =clock();
 part2_cpu_time= (double)(end2 - start2) / CLOCKS_PER_SEC;
  
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  for( i = 0; i < N; i++) {
      if(result[i] != sharedResultArray[i]) {
     
      f =1;
                                                                //This code checks if part1 and part2 results match or not
      }
  }
 
  if(f==0){
  printf("part1 and part2 results are matching\n");
  } else {

  printf("part1 and part2 results do not match\n");
  }
  printf("PART1 performence time : %f sec\n",part1_cpu_time);
  printf("PART2 performence time : %f sec\n",part2_cpu_time);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  shm_unlink(name1);
  shm_unlink(name2);
  shm_unlink(name3);


  return 0;

}