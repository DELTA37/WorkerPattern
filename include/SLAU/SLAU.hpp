#include <thread>
#include <iostream>
#include <deque>
#include <chrono>
#include <mutex>
#include <vector>
#include <bitset>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <time.h>
#include <thread>

#define THREAD_NUM 1
#define at(i, j, n) (i * n + j)

int readFromFile(const char* filename, double* (&A), double* (&b), unsigned int &n);
int readFromConsole(double* (&A), double* (&b), unsigned int &n); 
void print(double* res, unsigned int n); 
void print(double* A, double* b, unsigned int n);
int solve(double* A, double* b, unsigned int n, double* (&res)); 
double errorMax(double* A, double* b, unsigned int n, double* res); 

