#include "pmsis.h"
#include "config.h"

#define STACK_SIZE      1024

void main_fn();


#ifndef FABRIC
static void cluster_entry(void *arg)
{
  pi_cl_team_fork(NUM_CORES, main_fn, (void *)0x0);
}
#endif


int main()
{
#ifdef FABRIC
  main_fn();
#else
  struct pi_device cluster_dev = {0};
  struct pi_cluster_conf conf;
  struct pi_cluster_task cluster_task = {0};

  // task parameters allocation
  pi_cluster_task(&cluster_task, cluster_entry, NULL);
  cluster_task.stack_size = STACK_SIZE;
  cluster_task.slave_stack_size = STACK_SIZE;

  // First open the cluster
  pi_cluster_conf_init(&conf);
  conf.id=0;
  pi_open_from_conf(&cluster_dev, &conf);
  if (pi_cluster_open(&cluster_dev))
    return -1;

  // Then offload an entry point, this will get executed on the cluster controller
  pi_cluster_send_task_to_cl(&cluster_dev, &cluster_task);

  // closing of the cluster
  pi_cluster_close(&cluster_dev);
#endif

  return 0;
 
}
