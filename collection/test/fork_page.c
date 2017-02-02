#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

/*  
 * gcc fork_page.c -ggdb -lpthread
 *
 */

static void fill(char*,size_t, char);

static void *run(void*);
static const size_t length=64*1024*2;

int main(){
  char *ptr = mmap(0,length,PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
  if(ptr == MAP_FAILED){
    perror("error mmap");
    return(1);
  }
  pthread_t th;
  fill(ptr,length, 'a');
  pthread_create(&th, 0, run,ptr);
  usleep(100);

  int pid = fork();
  switch(pid){
    case 0: //child
      /* usleep(1000*1000); */
      /* ptr[0] = 's'; */
      /* usleep(2000*1000); */
      printf("child: %s\n",ptr);
      break;
      case -1:
        perror("fork");
      break;
    default: // parent
      /* fill(ptr,length, 'b'); */
      usleep(1000*1000);
      /* printf("parent: %s\n",ptr); */
      /* usleep(2000*1000); */
      pthread_join(th,0);
      break;
  }

  munmap(ptr, length);

}


void fill(char*arr,size_t length, char c){
   for(size_t i=0; i < length;++i){
      arr[i] = c;
    if(i % 100 == 0){
      arr[i] = '\n';
    }
   }
   arr[length-1] = 0;
}

void *run(void* args){
  char*ptr = args;
  for(size_t it=0;it<1000;++it){
    fill(ptr,length, (it % 9) + 48);
  }
}
