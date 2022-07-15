#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <sys/time.h>
#include <math.h>
#include "I2CWrapper.h"
#include "mpu6050.h"
#include  <math.h>
#include <stdlib.h>

#define LEN 100

GForceStruct   Data;
int ExitFlag=0;
int loop;
int i2c_handle;
float Gtotal;
float GSumSquare;
short GyroxOffset,GyroyOffset,GyrozOffset;
long  Xsum,Ysum,Zsum;
int CountSum;
const float  AccFactor=16.0 /32768.0;
const float  GyroFactor=250.0 / 32768.0;
const BUS = 1;
int I2C_Current_Slave_Adress=0x68;

void sig_handler(int signo)
{
  if (signo == SIGINT)
    printf("received SIGINT\n");
   ExitFlag=1;
}

int main(void)
{


    FILE *file_ptr;
    file_ptr = fopen("MPU_Data.txt", "w+a"); //создание файла и запись w (r-чтение, a-добавление текста в файл)
    if (file_ptr != NULL)
        {
        printf("Файл MPU_Data.txt успешно создан\n");
        }
        else
        {
        fprintf(stderr, "Не удалось создать либо открыть файл MPU_Data.txt\n");
        return 1;
        }
    fprintf(file_ptr, "MPU Data Read: \n");
    signal(SIGINT, sig_handler);

    i2c_handle = I2CWrapperOpen(BUS,I2C_Current_Slave_Adress);
	if(i2c_handle <0) return -1;
    if(! MPU6050_Test_I2C(i2c_handle))
       {
         printf("Unable to detect MPU6050\n");
       }
       else
      {
           Setup_MPU6050(i2c_handle);
           for(loop=0;loop<200;loop++)
           {
             while(!GotInt_MPU6050(i2c_handle));
             Get_Accel_Values(i2c_handle,&Data);
             Xsum += Data.Gyrox;
             Ysum += Data.Gyroy;
             Zsum += Data.Gyroz;
             CountSum++;
            }
           GyroxOffset = Xsum / CountSum;
           GyroyOffset = Ysum / CountSum;
           GyrozOffset = Zsum / CountSum;

           while(!ExitFlag)
           {
           //usleep(100000); // wait a little

           Get_Accel_Values(i2c_handle,&Data);
           GSumSquare = ((float) Data.Gx) * Data.Gx;
           GSumSquare += ((float) Data.Gy) * Data.Gy;
           GSumSquare += ((float) Data.Gz) * Data.Gz;
           Gtotal = sqrt(GSumSquare);

           printf("Accelerometer x=%+6.2f y=%+6.2f z=%+6.2f All=%5.2f Temp=%+4.1f Gyro x=%+7.2f y=%+7.2f z=%+7.2f\n",\

           AccFactor *  Data.Gx,
           AccFactor *  Data.Gy,
           AccFactor *  Data.Gz,
           AccFactor *  Gtotal,

           (float)  Data.Temperature / 340.0 + 36.53,
           GyroFactor * (Data.Gyrox - GyroxOffset),
           GyroFactor * (Data.Gyroy - GyroyOffset),
           GyroFactor * (Data.Gyroz - GyrozOffset));
                     fprintf(file_ptr, "Accelerometer x=%+6.2f y=%+6.2f z=%+6.2f All=%5.2f Temp=%+4.1f Gyro x=%+7.2f y=%+7.2f z=%+7.2f\n",\

           AccFactor *  Data.Gx,
           AccFactor *  Data.Gy,
           AccFactor *  Data.Gz,
           AccFactor *  Gtotal,

           (float)  Data.Temperature / 340.0 + 36.53,
           GyroFactor * (Data.Gyrox - GyroxOffset),
           GyroFactor * (Data.Gyroy - GyroyOffset),
           GyroFactor * (Data.Gyroz - GyrozOffset));
           fflush(stdout);
          }

      }

   close(i2c_handle);
return 0;
}
