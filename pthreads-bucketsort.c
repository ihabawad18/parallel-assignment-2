#define HAVE_STRUCT_TIMESPEC
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <pthread.h>
#define ARRAY_SIZE 800000
#define BUCKET_SIZE 10



typedef struct {
    int* bucket;
    int count;
} BucketSortArgs;

// Function to perform insertion sort on a bucket
void insertionSort(int arr[], int n) {
    int i, key, j;
    for (i = 1; i < n; i++) {
        key = arr[i];
        j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
}

// The thread function to sort buckets
void* threadSort(void* arg) {
    BucketSortArgs* args = (BucketSortArgs*)arg;
    insertionSort(args->bucket, args->count);

    pthread_t tid = pthread_self();

    // Portable way to print the thread identifier
    printf("Sorting with thread pseudo-ID: %p\n", (void*)(&tid));

    pthread_exit(NULL);
    return NULL;
}

// Function to perform bucket sort
int buckets[BUCKET_SIZE][ARRAY_SIZE] = { 0 };
void bucketSort(int arr[], int n) {
    int bucketCount[BUCKET_SIZE] = { 0 };

    // Distributing elements to the buckets
    for (int i = 0; i < n; i++) {
        int index = arr[i] / BUCKET_SIZE;
        buckets[index][bucketCount[index]++] = arr[i];
    }

    // Creating threads to sort each bucket
    pthread_t threads[BUCKET_SIZE];
    BucketSortArgs threadData[BUCKET_SIZE];

    for (int i = 0; i < BUCKET_SIZE; i++) {
        threadData[i].bucket = buckets[i];
        threadData[i].count = bucketCount[i];
        pthread_create(&threads[i], NULL, threadSort, (void*)&threadData[i]);
    }

    for (int i = 0; i < BUCKET_SIZE; i++) {
        pthread_join(threads[i], NULL);
    }

    // merging results
    int index = 0;
    for (int i = 0; i < BUCKET_SIZE; i++) {
        for (int j = 0; j < bucketCount[i]; j++) {
            arr[index++] = buckets[i][j];
        }
    }
}

void printArray(int arr[], int n) {
    for (int i = 0; i < n; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int arr[ARRAY_SIZE] = { 0 };
int main() {
    srand(time(NULL));
    clock_t start, end;
    double cpu_time_used;


    for (int i = 0; i < ARRAY_SIZE; i++) {
        // Generate a random number between 1 and 99
        arr[i] = rand() % 99 + 1;
    }

    //printf("Original array: \n");
    //printArray(arr, ARRAY_SIZE);

    start = clock();

    bucketSort(arr, ARRAY_SIZE);

    end = clock();

    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;

    //printf("Sorted array: \n");
    //printArray(arr, ARRAY_SIZE);

    printf("Time took = %f\n", cpu_time_used);

    return 0;
}
