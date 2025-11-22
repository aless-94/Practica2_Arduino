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
        fprintf(arch, "%.6f\n", *(data+i));  
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

void wFWav(float *array , int len, char *Filename)
{
    float *New_wav , *final_wav , retraso_seg = 0.3 , atte = 0.4;
    int retraso_muestras = (int)(retraso_seg * S);

      if ((New_wav = (float*) malloc ((len)*sizeof(float)))==NULL)
     {
        puts("Error solicitando memoria en archivo Wav");
        exit(1);
     }

     // Aplicar Lógica de ECO
    // y[n] = x[n] + atte * x[n - D]
    for (int i = 0; i < len; i++) 
    {
        if (i >= retraso_muestras) {
            New_wav[i] = array[i] + (atte * array[i - retraso_muestras]);
        } else {
            New_wav[i] = array[i]; // Antes del retraso, la señal es igual
        }
    }

    final_wav = Averager(New_wav,len);
    WriteFile(Filename, final_wav, len);
    free(New_wav);
    free(final_wav);

}


void AllProcess()
{
    float *signal , *signalWav;
    char str[256] , name[256] , nameWav[256];
    int len = 256, lenWav = 0 , pathR = 0;
    FILE *check;

    while (pathR == 0)
    {
    
        printf("Ingresar ruta del folder donde se encuentra las signals a procesar:\n");
        ReadStr(str,len);
        snprintf(name,256,"%s/Signal_011.txt",str);
        check = fopen(name, "r");
        if(check != NULL)
        {
                fclose(check); // existe
                pathR = 1;   
                printf("Ruta valida. Se encontro 'Signal_011.txt'.\n");
        } 
        else 
        {
                printf("ERROR: No se encuentra 'Signal_011.txt' en esa carpeta.\n");
                printf("Revisa la ruta e intenta de nuevo.\n");
        }
    }
       
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

    snprintf(nameWav,256,"%s/file_example_W.txt",str);

    check = fopen(nameWav,"r");

    if(check!= NULL)
    {
        fclose(check);

        signalWav= ReadFile(nameWav, &lenWav);
        snprintf(nameWav,256,"%s/file_wav_eco.txt",str);
        wFWav(signalWav,lenWav,nameWav);

        free(signalWav);

    } 
    else 
    {
        printf("ADVERTENCIA: No se encontro 'file_example_W.txt' en la misma carpeta.\n");
        printf("Asegurate de haber ejecutado el script de Matlab primero.\n");
    }
    
    puts("\n--- TODO EL PROCESO TERMINADO ---");
    puts("Presiona Enter para salir...");
    getchar();
}