#include <algorithm>
#include <fstream>
#include <iostream>
#include <limits.h>
#include <stdio.h>
#include <sys/time.h>
#include <vector>

using namespace std;

#define THREADS_PER_BLOCK 64
#define GRAVITATIONAL_CONSTANT 6.7e-11
#define TIME_STEP 0.001f
#define PI 3.14152926f
#define RADIUS 10

struct Body {
  float3 pos;
  float3 vel;
  float3 acc;
  float m;
};

__device__ void updateBody(Body &body) {
  body.vel.x += body.acc.x * TIME_STEP;
  body.vel.y += body.acc.y * TIME_STEP;
  body.vel.z += body.acc.z * TIME_STEP;

  body.pos.x += body.vel.x * TIME_STEP;
  body.pos.y += body.vel.y * TIME_STEP;
  body.pos.z += body.vel.z * TIME_STEP;
}

__device__ void calculateBodyNewState(Body *bodies, int count, int id) {
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

__global__ void nbody(Body *bodies, int count) {
  int id = blockIdx.x * THREADS_PER_BLOCK + threadIdx.x;
  if (id >= count)
    return;

  calculateBodyNewState(bodies, count, id);
}

__global__ void updateBodies(Body *bodies, int count) {
  int id = blockIdx.x * THREADS_PER_BLOCK + threadIdx.x;
  if (id >= count)
    return;

  updateBody(bodies[id]);
}

void getNumberOfBlocksAndThreads(int elemsCount, int *blocks, int *threads) {
  *blocks = ((elemsCount - 1) / THREADS_PER_BLOCK) ?: 1;
  *threads = (elemsCount < THREADS_PER_BLOCK ? elemsCount : THREADS_PER_BLOCK);
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
  std::ofstream fout("result_parallel");
  if (!fout) {
    printf("fail to open result_parallel\n");
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
    printf("Usage: ./parallel <N> <time>\n");
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

  Body *d_bodies;
  cudaMalloc(&d_bodies, bodies.size() * sizeof(Body));
  cudaMemcpy(d_bodies, &bodies.front(), bodies.size() * sizeof(Body),
             cudaMemcpyHostToDevice);

  float curTime = 0;

  int blocks, threads;
  getNumberOfBlocksAndThreads(bodies.size(), &blocks, &threads);

  gettimeofday(&start, 0);
  while (curTime < time) {
    nbody<<<blocks, threads>>>(d_bodies, bodies.size());
    updateBodies<<<blocks, threads>>>(d_bodies, bodies.size());

    curTime += TIME_STEP;
  }
  cudaDeviceSynchronize();
  gettimeofday(&end, 0);

  cudaMemcpy(&bodies.front(), d_bodies, bodies.size() * sizeof(Body),
             cudaMemcpyDeviceToHost);
  cudaFree(d_bodies);

  double time_result =
      (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
  printf("Time: %lf\n", time_result);

  writeResultsToFile(bodies);
  return 0;
}
