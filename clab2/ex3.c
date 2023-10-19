#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MAX_TEMPERATURE 35.0
#define MIN_TEMPERATURE -10.0
#define FREQUENCY 5 //frequentie in seconden


int main() {
	//Random number generator initialiseren
	srand(time(NULL));
	//Random temperatuur genereren
	float temperature = (float)(rand()) / RAND_MAX * (MAX_TEMPERATURE - MIN_TEMPERATURE) + MIN_TEMPERATURE;
	float *current_temperature = &temperature;
	
	while (1) {
		//Nieuwe temperatuur genereren
		*current_temperature = (float)(rand()) / RAND_MAX * (MAX_TEMPERATURE - MIN_TEMPERATURE) + MIN_TEMPERATURE;
		
		//Datum en tijd
		time_t current_time;
		time(&current_time);
		struct tm *timeinfo = localtime(&current_time);
		char *datetime = asctime(timeinfo);
		
		//Temperatuur printen in de juiste format
		printf("Temperature = %1.2f @ %s", *current_temperature, datetime);
		
		//Flush voor direct display
		fflush(stdout);
		
		sleep(FREQUENCY);
	}
	return 0;
}	
