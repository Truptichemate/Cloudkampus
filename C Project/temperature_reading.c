#include <stdio.h>
#include <stdlib.h>

#define MAX_TEMPERATURE_THRESHOLD 95.0 // Threshold for extreme temperatures
#define MAX_READINGS 1000

void readDataFromCSV(const char *filename, float temperatures[], int *numReadings);
void calculateMetrics(float temperatures[], int numReadings, float *average, float *maxTemp, float *minTemp);
void saveExtremeTemperaturesToCSV(const char *filename, float temperatures[], int numReadings);

int main() {
    float temperatures[MAX_READINGS];
    int numReadings = 0;

    readDataFromCSV("sensor_data.csv", temperatures, &numReadings);

    float average, maxTemp, minTemp;
    calculateMetrics(temperatures, numReadings, &average, &maxTemp, &minTemp);

    printf("Temperature Metrics:\n");
    printf("====================\n");
    printf("Average Temperature: %.2f\n", average);
    printf("Maximum Temperature: %.2f\n", maxTemp);
    printf("Minimum Temperature: %.2f\n", minTemp);

    saveExtremeTemperaturesToCSV("extreme_temperatures.csv", temperatures, numReadings);

    return 0;
}

void readDataFromCSV(const char *filename, float temperatures[], int *numReadings) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char line[256];
    int index = 0;

    while (fgets(line, sizeof(line), file)) {
        if (index >= MAX_READINGS) {
            fprintf(stderr, "Exceeded maximum storage capacity\n");
            break;
        }

        float temperature;
        if (sscanf(line, "%f", &temperature) == 1) {
            temperatures[index++] = temperature;
        }
    }

    *numReadings = index;
    fclose(file);
}

void calculateMetrics(float temperatures[], int numReadings, float *average, float *maxTemp, float *minTemp) {
    if (numReadings == 0) {
        *average = 0;
        *maxTemp = 0;
        *minTemp = 0;
        return;
    }

    float sum = 0.0;
    *maxTemp = temperatures[0];
    *minTemp = temperatures[0];

    for (int i = 0; i < numReadings; i++) {
        float temp = temperatures[i];
        sum += temp;
        if (temp > *maxTemp) *maxTemp = temp;
        if (temp < *minTemp) *minTemp = temp;
    }

    *average = sum / numReadings;
}

void saveExtremeTemperaturesToCSV(const char *filename, float temperatures[], int numReadings) {
    FILE *file = fopen(filename, "w");
    if (!file) {
        perror("Error creating file");
        exit(EXIT_FAILURE);
    }

    fprintf(file, "Temperature\n");
    for (int i = 0; i < numReadings; i++) {
        if (temperatures[i] > MAX_TEMPERATURE_THRESHOLD) {
            fprintf(file, "%.2f\n", temperatures[i]);
        }
    }

    fclose(file);
}
