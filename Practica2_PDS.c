#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <float.h> 

#include "process.h"

int main()
{

    AllProcess();
    return 0;
}

void ReadStr(char *str , size_t size){
    fgets(str,size,stdin);
    str[strcspn(str, "\r\n")] ='\0';
}

float* ReadFile (const char *path,  int *length){
    FILE *arch;
    float *signal;
    int count=0;
    float raw;


    if ((arch=fopen(path,"r"))==NULL)
    {
        puts("\nERROR ABRIENDO EL ARCHIVO EN MODO LECTURA\n");
        exit(1);
    }

    while (fscanf(arch, "%f", &raw) == 1) {
        count++;
    }

   if ((signal = (float*) malloc ((count)*sizeof(float)))==NULL)
   {
    puts("\nERROR AL SOLICITAR MEMORIA\n");
    free(signal);
    fclose(arch);
    exit(1);
   }
     rewind(arch);

   for (int i = 0; i < count; i++) 
   {
        if (fscanf(arch, "%f", signal + i ) != 1) {
            fclose(arch);
            free(signal);
            puts("ERROR LEYENDO FLOATS (formato invalido)");
            exit(1);
        }
    }

   fclose(arch);
   *length=count;
   return signal; 
}

int WriteFile(const char *path ,float *data, int n){
FILE *arch;

 if ((arch=fopen(path,"w"))==NULL)
    {
        puts("ERROR ABRIENDO EL ARCHIVO EN MODO ESCRITURA\n");
        exit(1);
    }

    for (int i = 0; i < n; i++)
    {
        fprintf(arch, "%.2f\n", *(data+i));  
    }

    fclose(arch);
    //printf("Archivo escrito correctamente\n");
    return 0;
}

float* Averager(float *array , int len)
{
    float *aSig;
     if ((aSig = (float*) malloc ((len)*sizeof(float)))==NULL)
     {
        puts("Error solicitando memoria en Averager");
        return NULL;
     }

     // Filtro promediador de 3 puntos
    for (int i=0 ; i<len ; i++)
     {
        if(i==0) 
        {
            aSig[i] = array[i] / 3.0;
        }
         else if(i==1) 
        {
            aSig[i] = (array[i] + array[i-1]) / 3.0;
        }
        else 
        {
            aSig[i] = (array[i] + array[i-1] + array[i-2]) / 3.0;
        }
     }
    return aSig;
}

float* Filter(float *array , int len)
{
     float *aSig;
     if ((aSig = (float*) malloc ((len)*sizeof(float)))==NULL)
     {
        puts("Error solicitando memoria en Filter");
        return NULL;
     }

     // Ecuación
    for (int i=0 ; i<len ; i++)
     {
        if(i==0) 
        {
            aSig[i] = array[i];
        }
         else if(i==1   ) 
        {
            aSig[i] = (array[i] + 0.5*array[i-1]);
        }
        else 
        {
            aSig[i] = (array[i] + 0.5*array[i-1] - 0.25*array[i-2]);
        }
     }
    return aSig;
}

void wFile(float *array_original , int len , FilterFunction filter , char *Filename)
{
    float *New_signal;

    New_signal = filter(array_original ,len);
    WriteFile(Filename,New_signal,len);
    free(New_signal);
}


void AllProcess()
{
    float *signal;
    char str[256] , name[256];
    int len = 256;

    printf("Ingresar ruta del file\n");
    ReadStr(str,len);
    snprintf(name,256,"%s/Signal_011.txt",str);
    signal = ReadFile(name, &len);
    //escalo la señal
    for (int i = 0; i < len; i++) {
        // (valor - 1.0) * (3.3 / 4.0) señal escalada de 0 a 3.3
        signal[i] = (signal[i] - 1.0) * 0.825; 
    }
    snprintf(name,256,"%s/signal_filter_averager.txt",str);
    wFile(signal,len,Averager,name);
    snprintf(name,256,"%s/signal_filter.txt",str);
    wFile(signal,len,Filter,name);
    free(signal);
}