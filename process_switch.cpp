#include <cstdlib>
#include <iostream>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>

void
measure_process_switch(int num_processes)
{
  struct timeval start, end;

  gettimeofday(&start, nullptr);
  for (int i = 0; i < num_processes; ++i) {
    if (fork() == 0) {
      exit(0);
    }
    wait(nullptr);
  }
  gettimeofday(&end, nullptr);

  long seconds = end.tv_sec - start.tv_sec;
  long microseconds = end.tv_usec - start.tv_usec;
  double elapsed = seconds + microseconds * 1e-6;

  std::cout << elapsed / num_processes << std::endl;
}

int
main(int argc, char* argv[])
{
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <num_processes>" << std::endl;
    return 1;
  }
  int num_processes = std::atoi(argv[1]);
  measure_process_switch(num_processes);
  return 0;
}
