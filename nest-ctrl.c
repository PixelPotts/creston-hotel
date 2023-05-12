#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <local.h>
#include <global.h>
#include <creston_bridge.h>

double cutoffFrequency = 10.0;
double samplingRate = 100.0;

double liveFeedData[] = {0.5, 1.2, 1.8, 1.3, 0.7, 0.3, 0.9, 1.6, 1.1, 0.6};
int liveFeedDataSize = sizeof(liveFeedData) / sizeof(liveFeedData[0]);

double filteredData[10];

pthread_mutex_t mutex;

void lowPassFilter(double* input, double* output, int size) {
    double omega = 2 * 3.14159265358979323846 * cutoffFrequency / samplingRate;
    double alpha = sin(omega) / (2.0 * cos(omega) + sin(omega));

    double x_n_minus_1 = 0.0;
    double y_n_minus_1 = 0.0;

    for (int i = 0; i < size; i++) {
        double x_n = input[i];
        double y_n = alpha * x_n + alpha * x_n_minus_1 + (1 - 2 * alpha) * y_n_minus_1;

        x_n_minus_1 = x_n;
        y_n_minus_1 = y_n;

        output[i] = y_n;
    }
}

void* processLiveFeed(void* arg) {
    double* liveData = (double*)arg;

    double filteredLiveFeed[10];
    lowPassFilter(liveData, filteredLiveFeed, liveFeedDataSize);

    pthread_mutex_lock(&mutex);

    for (int i = 0; i < liveFeedDataSize; i++) {
        filteredData[i] = filteredLiveFeed[i];
    }

    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main() {
    pthread_mutex_init(&mutex, NULL);

    pthread_t thread;
    pthread_create(&thread, NULL, processLiveFeed, (void*)liveFeedData);

    pthread_join(thread, NULL);

    pthread_mutex_lock(&mutex);

    printf("Filtered Data:\n");
    for (int i = 0; i < liveFeedDataSize; i++) {
        printf("%.2f\n", filteredData[i]);
    }

    pthread_mutex_unlock(&mutex);

    pthread_mutex_destroy(&mutex);

    return 0;
}
