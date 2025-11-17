#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h> 
void ReadStr(char *str , size_t size);
float* ReadFile(const char *path , int *length);
int WriteFile(const char *path, float *data, int n);
float* Averager(float *array, int len);
float* Filter(float *array, int len);
typedef float* (*FilterFunction)(float *array, int len);
void wFile (float *array_original , int len , FilterFunction , char *Filename);
void AllProcess();