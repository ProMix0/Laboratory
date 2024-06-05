#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STUD_NUM_ORIG 756
#define STUD_NUM STUD_NUM_ORIG | 1

#define TASKS_PRESETS 10
#define TASKS 10
#define Q 5
#define MEMORY 16
#define DISK 12

typedef enum { WAIT, READY, LOADING, EXECUTING, DONE } state_t;

typedef struct {
  int v, h, time;
  int ingress_interval;
  int ingress_time, load_start_time, exec_start_time, egress_time;
  int time_left;
  state_t state;
} task_t;

task_t tasks[TASKS], *tasks_sorted[TASKS],
    task_presets[TASKS_PRESETS] = {
        {.v = 6, .h = 2, .time = 60}, {.v = 3, .h = 4, .time = 90},
        {.v = 2, .h = 3, .time = 20}, {.v = 4, .h = 1, .time = 10},
        {.v = 3, .h = 2, .time = 60}, {.v = 5, .h = 0, .time = 30},
        {.v = 7, .h = 4, .time = 70}, {.v = 9, .h = 1, .time = 30},
        {.v = 4, .h = 6, .time = 40}, {.v = 1, .h = 3, .time = 20}};

void make_tasks(int size);

int tick = 0, next_tick;
int memory = MEMORY, disk = DISK;

int tasks_done = 0, tasks_running = 0, t_proc = 0;

void swap(task_t *arr[], int i, int j) {
  task_t *temp = arr[i];
  arr[i] = arr[j];
  arr[j] = temp;
}

void make_sorted() {
  for (int i = 0; i < TASKS; i++)
    tasks_sorted[i] = tasks + i;

  for (int i = 0; i < TASKS - 1; i++)

    for (int j = 0; j < TASKS - i - 1; j++)
#ifdef LIFO
      if (tasks_sorted[j]->ingress_time < tasks_sorted[j + 1]->ingress_time)
#else
#ifdef SJF
      if (tasks_sorted[j]->time > tasks_sorted[j + 1]->time)
#else
#error No dispatch strategy defined!
#endif
#endif
        swap(tasks_sorted, j, j + 1);
}

int main(int argc, char **argv) {
  make_tasks(TASKS);

  make_sorted();

  printf("\nTime table:\n\t");
  for (int i = 0; i < TASKS; i++)
    printf(" %d", i);
  printf("\tmemory\tdisk\tin run\tT proc\n");

  while (1) {

    for (int i = 0; i < TASKS; i++) {
      switch (tasks[i].state) {
      case WAIT:
        if (tasks[i].ingress_time <= tick) {
          tasks[i].state = READY;
        }
        continue;
      case EXECUTING:
        if (tasks[i].time_left <= 0) {
          tasks[i].state = DONE;
          tasks[i].egress_time = tick;
          tasks_running--;
          tasks_done++;
          memory += tasks[i].v;
          disk += tasks[i].h;
        }
        break;
      default:
        break;
      }
    }
    for (int i = 0; i < TASKS; i++) {
      switch (tasks_sorted[i]->state) {
      case READY:
        if (memory >= tasks_sorted[i]->v && disk >= tasks_sorted[i]->h) {
          tasks_sorted[i]->state = LOADING;
          tasks_sorted[i]->load_start_time = tick;
          tasks_sorted[i]->exec_start_time = tick + tasks_sorted[i]->h * Q;
          memory -= tasks_sorted[i]->v;
          disk -= tasks_sorted[i]->h;
        }
        continue;
      case LOADING:
        if (tasks_sorted[i]->exec_start_time <= tick) {
          tasks_sorted[i]->state = EXECUTING;
          tasks_running++;
        }
        break;
      default:
        break;
      }
    }

    printf("%d:\t", tick);
    for (int i = 0; i < TASKS; i++) {

      switch (tasks[i].state) {
      case READY:
        printf(" W");
        break;

      case LOADING:
        printf(" D");
        break;

      case EXECUTING:
        printf(" R");
        break;

      case WAIT:
      case DONE:
        printf("  ");
        break;
      }
    }
    printf("\t%d\t%d\t%d\t%d\n", memory, disk, tasks_running, t_proc);

    if (tasks_done >= TASKS)
      break;

    next_tick = INT_MAX;
    for (int i = 0; i < TASKS; i++) {

      switch (tasks[i].state) {
      case WAIT:
        if (tasks[i].ingress_time < next_tick)
          next_tick = tasks[i].ingress_time;
        break;

      case READY:
        break;

      case LOADING:
        if (tasks[i].exec_start_time < next_tick)
          next_tick = tasks[i].exec_start_time;
        break;

      case EXECUTING:
        if (tick + tasks[i].time_left * tasks_running < next_tick)
          next_tick = tick + tasks[i].time_left * tasks_running;
        break;

      case DONE:
        break;
      }
    }
    if (next_tick == INT_MAX) {
      printf("next_tick not changed!!!\n");
      return -1;
    }
    if (tasks_running) {
      for (int index = 0; tick < next_tick;) {
        if (tasks[index].state == EXECUTING) {
          tick++;
          tasks[index].time_left--;
          t_proc++;
        }
        index++;
        index = index % TASKS;
      }
    } else {
      tick = next_tick;
    }
  }

  printf("\ni\tingr\tload\tstart\tend\tdt\tW\n");
  for (int i = 0; i < TASKS; i++)
    printf("%d\t%d\t%d\t%d\t%d\t%d\t%f\n", i, tasks[i].ingress_time,
           tasks[i].load_start_time, tasks[i].exec_start_time,
           tasks[i].egress_time,
           tasks[i].egress_time - tasks[i].exec_start_time,
           (tasks[i].egress_time - tasks[i].ingress_time) /
               (tasks[i].egress_time - tasks[i].exec_start_time +
                tasks[i].h * (double)Q));
}

void make_tasks(int size) {
  int x = STUD_NUM, ingress_time = 0;
  printf("t_i\tx_i\tk_i\tv_i\th_i\tingr\tT_i\n");
  for (int i = 0; i < size; i++) {
    x = (7 * x + 417) % 1000;
    int k = x / 7 % TASKS_PRESETS;
    tasks[i] = task_presets[k];
    ingress_time += k;
    tasks[i].ingress_time = ingress_time;
    tasks[i].exec_start_time = INT_MAX;
    tasks[i].egress_time = INT_MAX;
    tasks[i].time_left = tasks[i].time;
    tasks[i].state = WAIT;
    printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\n", k, x, k, tasks[i].v, tasks[i].h,
           ingress_time, tasks[i].time /*+ Q * tasks[i].h*/);
  }
}
