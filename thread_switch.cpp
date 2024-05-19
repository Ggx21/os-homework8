#include <cstdlib>
#include <iostream>
#include <pthread.h>
#include <sys/time.h>

void*
thread_func(void* arg)
{
  pthread_exit(nullptr);
}

void
measure_thread_switch(int num_threads)
{
  pthread_t* threads = new pthread_t[num_threads];
  struct timeval start, end;

  gettimeofday(&start, nullptr);
  for (int i = 0; i < num_threads; ++i) {
    pthread_create(&threads[i], nullptr, thread_func, nullptr);
  }
  for (int i = 0; i < num_threads; ++i) {
    pthread_join(threads[i], nullptr);
  }
  gettimeofday(&end, nullptr);

  long seconds = end.tv_sec - start.tv_sec;
  long microseconds = end.tv_usec - start.tv_usec;
  double elapsed = seconds + microseconds * 1e-6;

  std::cout << elapsed / num_threads << std::endl;
  delete[] threads;
}

int
main(int argc, char* argv[])
{
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <num_threads>" << std::endl;
    return 1;
  }
  int num_threads = std::atoi(argv[1]);
  measure_thread_switch(num_threads);
  return 0;
}
