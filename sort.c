#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>

// Type Definitions
typedef enum order {
    ASCENDING = 1,
    DESCENDING = 0
} order;

#define swap(x, y) do {int swapper = *(x); *(x) = *(y); *(y) = swapper;} while (0)

// Generics
void printArray(int *array, int numberOfElements) {
    printf("[");
    for (unsigned int i=0; i<numberOfElements-1; i++) {
        printf("%d, ", array[i]);
    }
    printf("%d]\n", array[numberOfElements-1]);
}

static inline int greaterThan(int x, int y) {
    return x > y;
}

static inline int lessThan(int x, int y) {
    return x < y;
}


// Merge Sort
void merge(int *array1, int *array2, int size1, int size2) { // shuffles array2 into array1. array1 should have everything
    int *internalArray = (int *)malloc((size1 + size2) * sizeof(int));
    int array1Index = 0; 
    int array2Index = 0;
    int totalSize = size1 + size2;
    while ((array1Index < size1) && (array2Index < size2)) {
        if (array1[array1Index] < array2[array2Index]) {
            internalArray[array1Index + array2Index] = array1[array1Index];
            array1Index++;
        } else {
            internalArray[array1Index + array2Index] = array2[array2Index];
            array2Index++;
        }
    }
    while (array1Index < (size1)) {
        internalArray[array1Index + array2Index] = array1[array1Index];
        array1Index++;
    }
    while (array2Index < (size2)) {
        internalArray[array1Index + array2Index] = array2[array2Index];
        array2Index++;
    }
    memcpy(array1, internalArray, totalSize * sizeof(int));
    free(internalArray);
    return;
}

static inline int partition(int size) { // returns index to second array
    return size / 2UL;
}

void mergeSort(int *array1, int sizeTotal) {
    int partition2Index = partition(sizeTotal); // also size of first partition
    int *array2 = array1 + partition2Index;
    int size1 = partition2Index;
    int size2 = sizeTotal - partition2Index;
    if (sizeTotal <=1) return;
    mergeSort(array1, size1);
    mergeSort(array2, size2);
    merge(array1, array2, size1, size2);
    return;
}

// Bubble Sort
void bubbleSort(int *array, int numberOfElements) {
    int passthroughSort = 1;
    int swapper;
    while (passthroughSort) {
        passthroughSort = 0;
        for (unsigned int i=1; i<numberOfElements; i++) {
            if (array[i] < array[i-1]) {
                passthroughSort = 1;
                swapper = array[i];
                array[i] = array[i-1];
                array[i-1] = swapper;
            }
        }
    }
}

// Selection Sort
static inline void selectionSort(int * __restrict array, int numberOfElements) {
    int currentLargestIndex;
    int currentLargestValue = INT_MIN;
    for (int numberOfSortedElements = 0; numberOfSortedElements < numberOfElements; numberOfSortedElements++) {
        for (int i=0; i<(numberOfElements - numberOfSortedElements); i++) {
            if (array[i] > currentLargestValue) {
                currentLargestValue = array[i];
                currentLargestIndex = i;
            }
        }
        swap(array+currentLargestIndex, array+numberOfElements-numberOfSortedElements-1);
        currentLargestValue = INT_MIN;
    }
}

// Insertion Sort
static inline void insertionSort(int * __restrict array, int numberOfElements) {
    for (int i=1; i<numberOfElements; i++) {
        int key = array[i];
        int j = i - 1; // must be signed for indexing in inner for loop to properly work.
        while ((j >= 0) && (array[j] > key)) {
            array[j+1] = array[j];
            j--;
        }
        array[j + 1] = key;
    }
}

// Quick Sort
int quickSortPartition(int * __restrict array, int low, int high) {
    int mid = low + ((high - low) / 2);
    if (array[high] < array[low]) swap(array + low, array + high);
    swap(&high, &low);
    if (array[mid] < array[low]) swap(array + mid, array + low);
    swap(&mid, &low);
    if (array[high] < array[mid]) swap(array + high, array + mid);
    swap(&high, &mid);
    int pivotIndex = mid;
    
    int pivotValue = array[pivotIndex];
    swap(array + pivotIndex, array + high);
    pivotIndex = high;
    int i = low;
    for (int j = low; j < high; j++) {
        if (array[j] <= pivotValue) {
            swap(array+i, array+j);
            i++;
        }
    }
    swap(array+i, array+high); // i is now index that pivot belongs. put it there.
    return i; // return pivot index
}

void quickSortRecurse(int * __restrict array, int low, int high) {
    int pivotIndex;
    while ((high - low) > 32) {
        pivotIndex = quickSortPartition(array, low, high); // this does the reorganizing and returns the new pivot index to split at
        if (((pivotIndex - 1) - low) > (high - (pivotIndex + 1))) {
            quickSortRecurse(array, pivotIndex + 1, high);
            high = pivotIndex - 1;
        } else {
            quickSortRecurse(array, low, pivotIndex - 1);
            low = pivotIndex + 1;
        }
    }
    insertionSort(array + low, high - low + 1);
}

void quickSort(int * __restrict array, int numberOfElements) {
    quickSortRecurse(array, 0, numberOfElements - 1);
    return;
}

// Other
void setArray(int *array, int arraySize) {
    for (int i=0; i<arraySize; i++) {
        array[i] = rand() % 100000000; // for array readability
    }
}

int main(int argc, char **argv) {
    srand((unsigned int)(time(NULL)));
    int arraySize;
    if (argc > 1) {
        arraySize = atoi(argv[1]);
    } else {
        printf("Enter array size: ");
        scanf("%d", &arraySize);
    }
    if (arraySize < 2) {
        printf("Please enter a meaningful array size.\n");
        exit(1);
    }
    int *arrayToSort = (int *)malloc(sizeof(int) * arraySize);
    setArray(arrayToSort, arraySize);
    
    clock_t startingSortClock, endingSortClock;

    startingSortClock = clock();
    quickSort(arrayToSort, arraySize);
    endingSortClock = clock();
    printf("%lf seconds to quick sort\n", (double)((endingSortClock - startingSortClock) / (double)CLOCKS_PER_SEC));
    setArray(arrayToSort, arraySize);

    startingSortClock = clock();
    mergeSort(arrayToSort, arraySize);
    endingSortClock = clock();
    printf("%lf seconds to merge sort\n", (double)((endingSortClock - startingSortClock) / (double)CLOCKS_PER_SEC));
    setArray(arrayToSort, arraySize);

    startingSortClock = clock();
    selectionSort(arrayToSort, arraySize);
    endingSortClock = clock();
    printf("%lf seconds to selection sort\n", (double)((endingSortClock - startingSortClock) / (double)CLOCKS_PER_SEC));
    setArray(arrayToSort, arraySize);

    startingSortClock = clock();
    insertionSort(arrayToSort, arraySize);
    endingSortClock = clock();
    printf("%lf seconds to insertion sort\n", (double)((endingSortClock - startingSortClock) / (double)CLOCKS_PER_SEC));
    setArray(arrayToSort, arraySize);

    startingSortClock = clock();
    bubbleSort(arrayToSort, arraySize);
    endingSortClock = clock();
    printf("%lf seconds to bubble sort\n", (double)((endingSortClock - startingSortClock) / (double)CLOCKS_PER_SEC));
    setArray(arrayToSort, arraySize);

    free(arrayToSort);
}