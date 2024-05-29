#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COMP_COUNT 3
#define iterate_comps for (comp_index = 0; comp_index < COMP_COUNT; comp_index++)

enum state { IDLE, WORKING, QUEUE_FILLED };

struct computer {
  enum state state;
  int change_timestamp, change_timestamp2;
  int work_time, tasks_handled;
};

int main(int argc, char *argv[]) {
  if (argc > 1)
    srand(atoi(argv[1]));

  struct computer pool[COMP_COUNT];
  int dropped = 0, comp_index;
  int time_in_queue = 0, tasks_in_queue = 0;
  int time_in_system = 0, tasks_in_system = 0;

  int comps_order[10] = {2, 0, 2, 1, 2, 0, 2, 1, 2, 0};
  int comp_order_index = 0;
  memset(pool, 0, sizeof(struct computer) * COMP_COUNT);

  int time = 1 + rand() % COMP_COUNT;
  for (int i = 0; i < 1000 * 100; i++) {
    int handle_delay = 1 + rand() % 7;

    iterate_comps {
      switch (pool[comp_index].state) {
      case QUEUE_FILLED:
        if (pool[comp_index].change_timestamp <= time) {
          pool[comp_index].state = WORKING;
          pool[comp_index].change_timestamp =
              pool[comp_index].change_timestamp2;
          pool[comp_index].tasks_handled++;
        }
      case WORKING:
        if (pool[comp_index].change_timestamp <= time) {
          pool[comp_index].state = IDLE;
          pool[comp_index].tasks_handled++;
        }
      case IDLE:
        break;
      }
    }
    comp_index = comps_order[comp_order_index++ % 10];

    switch (pool[comp_index].state) {
    case QUEUE_FILLED:
      dropped++;
      break;
    case WORKING:
      pool[comp_index].state = QUEUE_FILLED;
      pool[comp_index].change_timestamp2 =
          pool[comp_index].change_timestamp + handle_delay;
      pool[comp_index].work_time += handle_delay;

      tasks_in_queue++;
      time_in_queue += handle_delay;

      tasks_in_system++;
      time_in_system += handle_delay + pool[comp_index].change_timestamp - time;
      break;
    case IDLE:
      pool[comp_index].state = WORKING;
      pool[comp_index].change_timestamp = time + handle_delay;
      pool[comp_index].work_time += handle_delay;
      break;
    }
    time += 1 + rand() % COMP_COUNT;
  }

  int work_done = 0;

  for (int i = 0; i < COMP_COUNT - 1; i++) {
    iterate_comps {
      if (pool[comp_index].change_timestamp > time)
        time = pool[comp_index].change_timestamp;

      switch (pool[comp_index].state) {
      case QUEUE_FILLED:
        if (pool[comp_index].change_timestamp <= time) {
          pool[comp_index].state = WORKING;
          pool[comp_index].change_timestamp =
              pool[comp_index].change_timestamp2;
          pool[comp_index].tasks_handled++;
        }
      case WORKING:
        if (pool[comp_index].change_timestamp <= time) {
          pool[comp_index].state = IDLE;
          pool[comp_index].tasks_handled++;
        }
      case IDLE:
        break;
      }
    }
  }

  iterate_comps {
    printf("Worker %d IDLE percent: %2.0f%%\n", comp_index,
           (time - pool[comp_index].work_time) * 100. / time);
  }
  printf("\nAverage queue time: %f\n", time_in_queue / (double)tasks_in_queue);
  printf("\nAverage time in system: %f\n",
         time_in_system / (double)tasks_in_system);
  return 0;
}
