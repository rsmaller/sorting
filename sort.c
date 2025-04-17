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

// Generics
void printArray(int *array, size_t numberOfElements) {
    printf("[");
    for (unsigned int i=0; i<numberOfElements-1; i++) {
        printf("%d, ", array[i]);
    }
    printf("%d]\n", array[numberOfElements-1]);
}

static inline void swap(int *x, int *y) {
    int swapper = *x;
    *x = *y;
    *y = swapper;
}

static inline int greaterThan(int x, int y) {
    return x > y;
}

static inline int lessThan(int x, int y) {
    return x < y;
}


// Merge Sort
void merge(int *array1, int *array2, size_t size1, size_t size2) { // shuffles array2 into array1. array1 should have everything
    int *internalArray = (int *)malloc((size1 + size2) * sizeof(int));
    size_t array1Index = 0; 
    size_t array2Index = 0;
    size_t totalSize = size1 + size2;
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

static inline size_t partition(size_t size) { // returns index to second array
    return size / 2UL;
}

void mergeSort(int *array1, size_t sizeTotal) {
    size_t partition2Index = partition(sizeTotal); // also size of first partition
    int *array2 = array1 + partition2Index;
    size_t size1 = partition2Index;
    size_t size2 = sizeTotal - partition2Index;
    if (sizeTotal <=1) return;
    mergeSort(array1, size1);
    mergeSort(array2, size2);
    merge(array1, array2, size1, size2);
    return;
}

// Bubble Sort
void bubbleSort(int *array, size_t numberOfElements) {
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
void selectionSort(int *array, size_t numberOfElements) {
    size_t currentLargestIndex;
    int currentLargestValue = INT_MIN;
    for (size_t numberOfSortedElements = 0; numberOfSortedElements < numberOfElements; numberOfSortedElements++) {
        for (size_t i=0; i<(numberOfElements - numberOfSortedElements); i++) {
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
void insertionSort(int *array, size_t numberOfElements) {
    size_t i;
    for (i=1; i<numberOfElements; i++) {
        int key = array[i];
        int j = i - 1; // must be signed for indexing in inner for loop to properly work.
        for (; (j>=0) && (array[j] > key); j--) {
            array[j+1] = array[j];
        }
        array[j + 1] = key;
    }
}

// Quick Sort
void quickSort(int *array, size_t numberOfElements, void (*smallSorterFunction)(int *, size_t)) {
    if (numberOfElements < 2) return;
    else if (numberOfElements <= 32) {
        smallSorterFunction(array, numberOfElements);
        return;
    }
    int pivotIndex = rand() % numberOfElements;
    int newPivotIndex;
    int i = -1;
    int j = 0;
    swap(array + pivotIndex, array + numberOfElements - 1); // move pivot to end
    pivotIndex = numberOfElements - 1;
    for (; j < numberOfElements - 1; j++) {
        if ((array[j] < array[pivotIndex])) {
            swap(array + j, array + (++i));
        }
    }
    newPivotIndex = i + 1;
    swap(array + pivotIndex, array + newPivotIndex); // move pivot to its correct spot
    int *leftArray = array;
    int *rightArray = array + newPivotIndex + 1;
    int leftArraySize = newPivotIndex;
    int rightArraySize = (numberOfElements - newPivotIndex) - 1;
    quickSort(leftArray, leftArraySize, smallSorterFunction); // for documentation: both original recursive calls.
    quickSort(rightArray, rightArraySize, smallSorterFunction);
    return;
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
    int *randomizedArray = (int *)malloc(sizeof(int) * arraySize);
    int *arrayToSort = (int *)malloc(sizeof(int) * arraySize);
    int randomValue;
    for (int i=0; i<arraySize; i++) {
        randomValue = rand() % 100000000; // for array readability
        randomizedArray[i] = randomValue;
        arrayToSort[i] = randomValue;
    }
    clock_t startingSortClock, endingSortClock;
    
    startingSortClock = clock();
    mergeSort(arrayToSort, arraySize);
    endingSortClock = clock();
    printf("%lf seconds to merge sort\n", (double)((endingSortClock - startingSortClock) / (double)CLOCKS_PER_SEC));
    memcpy(arrayToSort, randomizedArray, arraySize);

    startingSortClock = clock();
    quickSort(arrayToSort, arraySize, insertionSort);
    endingSortClock = clock();
    printf("%lf seconds to quick insertion sort\n", (double)((endingSortClock - startingSortClock) / (double)CLOCKS_PER_SEC));
    memcpy(arrayToSort, randomizedArray, arraySize);

    startingSortClock = clock();
    quickSort(arrayToSort, arraySize, selectionSort);
    endingSortClock = clock();
    printf("%lf seconds to quick selection sort\n", (double)((endingSortClock - startingSortClock) / (double)CLOCKS_PER_SEC));
    memcpy(arrayToSort, randomizedArray, arraySize);

    // startingSortClock = clock();
    // selectionSort(arrayToSort, arraySize);
    // endingSortClock = clock();
    // printf("%lf seconds to selection sort\n", (double)((endingSortClock - startingSortClock) / (double)CLOCKS_PER_SEC));
    // memcpy(arrayToSort, randomizedArray, arraySize);

    // startingSortClock = clock();
    // insertionSort(arrayToSort, arraySize);
    // endingSortClock = clock();
    // printf("%lf seconds to insertion sort\n", (double)((endingSortClock - startingSortClock) / (double)CLOCKS_PER_SEC));
    // memcpy(arrayToSort, randomizedArray, arraySize);

    // startingSortClock = clock();
    // bubbleSort(arrayToSort, arraySize);
    // endingSortClock = clock();
    // printf("%lf seconds to bubble sort\n", (double)((endingSortClock - startingSortClock) / (double)CLOCKS_PER_SEC));
    // memcpy(arrayToSort, randomizedArray, arraySize);

    free(randomizedArray);
    free(arrayToSort);
}