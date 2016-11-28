/*
*** Configurando os pinos de GPIO como OUTPUT na RPI ***
*** Autor: Andre Leal de Freitas ***
*** Board: RPI 3 model B ***
*** Versao: 1
*/


#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <signal.h>

//---Macros---//

#define GPIO24 	24

#define HIGH    1
#define LOW 	0
#define INPUT   0
#define OUTPUT  1

//---Prototipos das funcoes---//

bool access_gpio(int pin);
bool export_gpio(int pin);
bool direction_gpio(int pin, int direction);
bool value_gpio(int pin, int value);
bool unexport_gpio(int pin);

void delay(float time);

void finalization(int nsingnal);

//----------------------------//

//---Variaveis Globais--------//
int arquive, pin=GPIO24;
float timeSleep=0.5;
char buffer[3];
char path[35];
//----------------------------//

int main (int argc, char * argv[])
{
	signal(SIGINT, finalization);
	// === Verifica se o pino ja foi exportado === //
	if(access_gpio(pin))
        {
		// === Export do Pino === //
		if (export_gpio(pin))
		{
		// === Configurando Direcao do Pino === //
			if(direction_gpio(pin, OUTPUT))
			{
				//=== Escrevendo no pino ===//
        			while(true)
        			{
					if(value_gpio(pin, HIGH))
					{
        	        			delay(timeSleep);
                			}
					else
					{
						printf("Ocorreu um problema para setar o pino em alto");
						return 3;
					}
					if (value_gpio(pin, LOW))
					{
						delay(timeSleep);
					}
					else
					{
						printf("Ocorreu um problema para setar o pino em baixo");
                                	        return 4;
					}
        			}
			}
			else
			{
				printf("Ocorreu um problema na configuracao do pino como I/O\n");
				return 2;
			}
			 // === Desvinculando o pino == //

	                if(unexport_gpio(pin))
        	        {
                	        return 0;
                	}
                	else
                	{
                        	printf("Ocorreu um problema para finalizar a utilizacao do pino\n");
                        	return 2;
                	}

		}
		else
		{
			printf("Ocorreu um problema no export do pino\n");
                        return 1;
		}
	}
	else
	{
		printf("Pino ja foi exportado! \n");
                delay(0.5);
                printf("Configurando as caracteristicas de utilizacao! \n");
                delay(0.5);

                if(direction_gpio(pin, OUTPUT))
                {
			printf("GPIO%d configurada como OUTPUT! \n", pin);
                        delay(0.5);
                        printf("Iniciando o processo de acionamento\n");
                        delay(0.5);
                        //=== Escrevendo no pino ===//
                        while(true)
			{
				if(value_gpio(pin, HIGH))
                        	{
                                	delay(timeSleep);
                                }
                                else
                                {
                                	printf("Ocorreu um problema para setar o pino em alto");
                                        return 3;
                               	}
                                if (value_gpio(pin, LOW))
                                {
                                	delay(timeSleep);
                                }
                                else
                                {
                                        printf("Ocorreu um problema para setar o pino em baixo");
                                        return 4;
                                }

 			}
                        if(unexport_gpio(pin))
			{
				return 0;
			}
                        else
			{
				printf("Ocorreu um problema para finalizar a utilizacao do pino\n");
				return 2;
			}
	  	}
                else
                {
                        printf("Ocorreu um problema na configuracao do pino como I/O\n");
                        return 2;
                }
	}
}

void finalization(int nsignal)
{
	// === Desvinculando o pino == //

        if(unexport_gpio(pin))
        {
		delay(0.5);
                printf("\nAplicando um unexport no pino\n");
		delay(0.5);
		printf("Programa finalizado...\n");
		exit(0);
        }
}

bool access_gpio(int pin)
{
        snprintf(path, 35, "/sys/class/gpio/gpio%d/direction", pin);
        if (access(path, 0) == -1) // Arquivo nao existe
        {
                return true;
        }
        else // Arquivo existe
        {
                return false;
        }
}

bool export_gpio(int pin)
{
        arquive = open ("/sys/class/gpio/export", O_WRONLY);
        if (arquive==-1)
        {
                printf("Arquivo abriu incorretamente\n");
                return false;
        }
        snprintf(buffer, 3, "%d", pin);
        if(write(arquive, buffer, 3) == -1)
        {
                close(arquive);
                return false;
        }

        close(arquive);

	return true;
}

bool direction_gpio(int pin, int direction)
{
	arquive=0;
        snprintf(path, 35, "/sys/class/gpio/gpio%d/direction", pin);
        arquive = open (path, O_WRONLY);
        if (arquive==-1)
        	return false;

        snprintf(buffer, 3, "%d", pin);
        if (write( arquive, ((direction == INPUT)?"in":"out"), 3 )==-1)
        {
                close(arquive);
                return false;
        }
        close(arquive);
	return true;
}

bool value_gpio(int pin, int value)
{
	arquive=0;
        snprintf(path, 35, "/sys/class/gpio/gpio%d/value", pin);
        arquive = open(path, O_WRONLY);
        if (arquive == -1)
        {
                return false;
        }
        if (write (arquive, ((value == HIGH)?"1":"0"), 1) == -1)
        {
                close(arquive);
                return false;
        }
	close(arquive);
	return true;
}

bool unexport_gpio(int pin)
{
	arquive = open ("/sys/class/gpio/unexport", O_WRONLY);
        if (arquive==-1)
        {
                printf("Arquivo abriu incorretamente\n");
                return false;
        }
        if(write(arquive, buffer, 3) == -1)
        {
                close(arquive);
                return false;
        }
        return true;
}

void delay(float time)
{
	struct timespec t;
	int seg;
	seg = time;
	t.tv_sec = seg;
	t.tv_nsec = (time-seg)*1e9;
	nanosleep(&t, NULL);
}
