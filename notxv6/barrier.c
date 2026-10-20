#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>

static int nthread = 1;
static int round = 0;

struct barrier {
  pthread_mutex_t barrier_mutex;
  pthread_cond_t barrier_cond;
  int nthread;      // Number of threads that have reached this round of the barrier
  int round;     // Barrier round
  int narrived;   // Number of threads that have arrived at the barrier
} bstate;

static void
barrier_init(void)
{
  assert(pthread_mutex_init(&bstate.barrier_mutex, NULL) == 0);
  assert(pthread_cond_init(&bstate.barrier_cond, NULL) == 0);
  bstate.nthread = nthread;
  bstate.narrived = 0;
}

static void 
barrier()
{
  // YOUR CODE HERE
  //
  // Block until all threads have called barrier() and
  // then increment bstate.round.
  //
  pthread_mutex_lock(&bstate.barrier_mutex); //加锁
  bstate.narrived++; //到达的线程数加1
  if(bstate.narrived == nthread){ //如果到达的线程数等于总线程数
    bstate.round++; //轮数加1
    bstate.narrived = 0; //到达的线程数清零
    pthread_cond_broadcast(&bstate.barrier_cond); //唤醒所有等待的线程
  }else{
    //等待其他线程到达屏障
    int t_round = bstate.round; //记录当前轮数
    while(t_round == bstate.round){ //如果当前轮数等于记录的轮数，说明还有线程没有到达屏障，继续等待
      pthread_cond_wait(&bstate.barrier_cond, &bstate.barrier_mutex); //等待条件变量
    }
  }
  pthread_mutex_unlock(&bstate.barrier_mutex); //解锁
}

static void *
thread(void *xa)
{
  long n = (long) xa;
  long delay;
  int i;

  for (i = 0; i < 20000; i++) {
    int t = bstate.round;
    assert (i == t);
    barrier();
    usleep(random() % 100);
  }

  return 0;
}

int
main(int argc, char *argv[])
{
  pthread_t *tha;
  void *value;
  long i;
  double t1, t0;

  if (argc < 2) {
    fprintf(stderr, "%s: %s nthread\n", argv[0], argv[0]);
    exit(-1);
  }
  nthread = atoi(argv[1]);
  tha = malloc(sizeof(pthread_t) * nthread);
  srandom(0);

  for(i = 0; i < nthread; i++) {
    assert(pthread_create(&tha[i], NULL, thread, (void *) i) == 0);
  }
  for(i = 0; i < nthread; i++) {
    assert(pthread_join(tha[i], &value) == 0);
  }
  printf("OK; passed\n");
}
