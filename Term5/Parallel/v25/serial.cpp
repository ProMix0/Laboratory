#include <fstream>
#include <iostream>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <vector>

using namespace std;

#define THREADS_PER_BLOCK 64
#define GRAVITATIONAL_CONSTANT 6.7e-11
#define TIME_STEP 0.001f
#define PI 3.14152926f

typedef struct {
  float x, y, z;
} float3;

struct Body {
  float3 pos;
  float3 vel;
  float3 acc;
  float m;
};

void updateBody(Body &body) {
  body.vel.x += body.acc.x * TIME_STEP;
  body.vel.y += body.acc.y * TIME_STEP;
  body.vel.z += body.acc.z * TIME_STEP;

  body.pos.x += body.vel.x * TIME_STEP;
  body.pos.y += body.vel.y * TIME_STEP;
  body.pos.z += body.vel.z * TIME_STEP;
}

void calculateBodyNewState(Body *bodies, int count, int id) {

  bodies[id].acc.x = 0;
  bodies[id].acc.y = 0;
  bodies[id].acc.z = 0;

  for (int i = 0; i < count; i++) {
    if (i == id)
      continue;

    float3 distance;
    distance.x = bodies[i].pos.x - bodies[id].pos.x;
    distance.y = bodies[i].pos.y - bodies[id].pos.y;
    distance.z = bodies[i].pos.z - bodies[id].pos.z;

    float dist_sqr = distance.x * distance.x + distance.y * distance.y +
                     distance.z * distance.z;

    bodies[id].acc.x += bodies[i].m / dist_sqr;
    bodies[id].acc.y += bodies[i].m / dist_sqr;
    bodies[id].acc.z += bodies[i].m / dist_sqr;
  }

  bodies[id].acc.x *= GRAVITATIONAL_CONSTANT;
  bodies[id].acc.y *= GRAVITATIONAL_CONSTANT;
  bodies[id].acc.z *= GRAVITATIONAL_CONSTANT;
}

void solve_serial(vector<Body> &bodies, float time) {
  float curTime = 0;

  while (curTime < time) {
    for (int i = 0; i < bodies.size(); i++)
      calculateBodyNewState(&bodies.front(), bodies.size(), i);

    for (int i = 0; i < bodies.size(); i++)
      updateBody(bodies[i]);

    curTime += TIME_STEP;
  }
}

void createBodies(vector<Body> &bodies, int count) {
  bodies = vector<Body>(count);
  if (getenv("SEED"))
    srand(atoi(getenv("SEED")));
  else
    srand((0));

  for (int i = 0; i < bodies.size(); i++) {
    bodies[i].pos.x = rand();
    bodies[i].pos.y = rand();
    bodies[i].pos.z = rand();

    bodies[i].m = rand() / ((float)RAND_MAX / 100000);

    bodies[i].vel.x = 0;
    bodies[i].vel.y = 0;
    bodies[i].vel.z = 0;

    bodies[i].acc.x = 0;
    bodies[i].acc.y = 0;
    bodies[i].acc.z = 0;
  }
}

void writeResultsToFile(vector<Body> &bodies) {
  std::ofstream fout("result_serial");
  if (!fout) {
    printf("fail to open result_serial\n");
    return;
  }

  fout << "Bodies: \n";

  for (int i = 0; i < bodies.size(); i++) {
    fout << "(x:" << bodies[i].pos.x << "| y:" << bodies[i].pos.y
         << "| z:" << bodies[i].pos.z << ")\n";
  }
}

int main(int argc, char **argv) {
  if (argc < 3) {
    printf("Usage: ./serial <N> <time>\n");
    return -1;
  }

  int count = atoi(argv[1]);
  if (count < 2) {
    printf("Must be at least two bodies\n");
    return -1;
  }

  int time = atoi(argv[2]);
  if (time < TIME_STEP) {
    printf("Too small time\n");
    return -1;
  }

  vector<Body> bodies;
  createBodies(bodies, count);

  struct timeval start, end;
  gettimeofday(&start, 0);

  solve_serial(bodies, time);

  gettimeofday(&end, 0);
  double time_result =
      (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
  printf("Time: %lf\n", time_result);

  writeResultsToFile(bodies);
  return 0;
}
