#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <assert.h>
#include <thread>
#include <future>
#include <chrono>
#include <algorithm>
using namespace std;

// Type Definitions
typedef enum order {
    ASCENDING = 1,
    DESCENDING = 0
} order;

typedef struct stack {
    int *stackArray;
    size_t size;
    int nextEmptySlot;
} stack;

stack *constructStack() {
    stack *returnValue = (stack *)malloc(sizeof(stack));
    returnValue -> stackArray = (int *)malloc(sizeof(int) * 4);
    returnValue -> size = 4;
    returnValue -> nextEmptySlot = 0;
    return returnValue;
}

#define swap(type, x, y) do {type swapper = *(x); *(x) = *(y); *(y) = swapper;} while(0)
#define THREAD_DEPTH_LIMIT 4
#define QUICKSORT_SIZE_LIMIT 16

// Generics
void printArray(int *array, int numberOfElements) {
    printf("[");
    for (unsigned int i=0; i<numberOfElements-1; i++) {
        printf("%d, ", array[i]);
    }
    printf("%d]\n", array[numberOfElements-1]);
}

static inline int qsort_compare(const void *a, const void *b) {
    int compare1 = *(int *)a;
    int compare2 = *(int *)b;
    if (compare1 < compare2) {
        return -1;
    } else if  (compare1 == compare2) {
        return 0;
    } else {
        return 1;
    }
}


// Merge Sort
static inline void merge(int *array1, int *array2, int size1, int size2) { // shuffles array2 into array1. array1 should have everything
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

void mergeSort(int *array1, int sizeTotal, int depth = 0) {
    int partition2Index = sizeTotal / 2; // also size of first partition
    int *array2 = array1 + partition2Index;
    int size1 = partition2Index;
    int size2 = sizeTotal - partition2Index;
    future<void> future1;
    future<void> future2;
    if (sizeTotal <=1) return;
    if (depth < THREAD_DEPTH_LIMIT) {
        if (size1 < size2) {
            future1 = async(launch::async, [&](){mergeSort(array1, size1, depth+1);});
            mergeSort(array2, size2, depth);
        } else {
            future2 = async(launch::async, [&](){mergeSort(array2, size2, depth+1);});
            mergeSort(array1, size1, depth);
        }
        if (future1.valid()) future1.get();
        if (future2.valid()) future2.get();
    } else {
        mergeSort(array1, size1, depth);
        mergeSort(array2, size2, depth);
    }
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
        swap(int, array+currentLargestIndex, array+numberOfElements-numberOfSortedElements-1);
        currentLargestValue = INT_MIN;
    }
}

// Insertion Sort
static inline void insertionSort(int * __restrict array, int numberOfElements) {
    for (int i=1; i<numberOfElements; i++) {
        const int key = array[i];
        int j = i - 1; // must be signed for indexing in inner for loop to properly work.
        while ((j >= 0) && (array[j] > key)) {
            array[j+1] = array[j];
            j--;
        }
        if (j + 1 != i) { // don't overrwrite key for no reason
            array[j + 1] = key;
        }
    }
}

// Quick Sort
static inline int quickSortPartition(int * __restrict array, int low, int high) {
    int mid = low + ((high - low) / 2);
    if (array[high] < array[low]) swap(int, array + low, array + high);
    if (array[mid] < array[low]) swap(int, array + mid, array + low);
    if (array[high] < array[mid]) swap(int, array + high, array + mid);
    int pivotIndex = mid;
    
    int pivotValue = array[pivotIndex];
    swap(int, array + pivotIndex, array + high);
    pivotIndex = high;
    int i = low;
    for (int j = low; j < high; j++) {
        if (array[j] <= pivotValue) {
            swap(int, array+i, array+j);
            i++;
        }
    }
    swap(int, array+i, array+high); // i is now index that pivot belongs. put it there.
    return i; // return pivot index
}

void push(stack *stackArg, int value) {
    if (stackArg -> nextEmptySlot >= stackArg -> size - 1) {
        stackArg -> size *= 2;
        stackArg -> stackArray = (int *)realloc(stackArg -> stackArray, stackArg -> size * sizeof(int));
    }
    stackArg -> stackArray[stackArg -> nextEmptySlot++] = value;
}

int pop(stack *stackArg) {
    return stackArg -> stackArray[--stackArg -> nextEmptySlot];
}

void quickSort(int * __restrict array, int numberOfElements) {
    int high = numberOfElements - 1;
    int low = 0;
    int pivotIndex;
    stack *sortStack = constructStack();
    push(sortStack, low); // push low
    push(sortStack, high); // push high
    while (sortStack -> nextEmptySlot > 0) {
        high = pop(sortStack); // pop high
        low = pop(sortStack); // pop low
        if (low >= high - 1) continue;
        pivotIndex = quickSortPartition(array, low, high);
        if (pivotIndex - low > 1) {
            push(sortStack, low);               // push low 1
            push(sortStack, pivotIndex - 1);    // push high 1
        }
        if (high - pivotIndex >= 1) {
            push(sortStack, pivotIndex + 1);    // push low 2
            push(sortStack, high);              // push high 2
        }
    }
    free(sortStack -> stackArray);
    free(sortStack);
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

    auto startingClock = std::chrono::steady_clock::now();
    mergeSort(arrayToSort, arraySize);
    auto endingClock = std::chrono::steady_clock::now();
    printf("%.7lf seconds to merge sort\n", chrono::duration<double>(endingClock-startingClock).count());
    setArray(arrayToSort, arraySize);

    startingClock = std::chrono::steady_clock::now();
    std::sort(arrayToSort, arrayToSort + arraySize - 1);
    endingClock = std::chrono::steady_clock::now();
    printf("%.7lf seconds to C++ sort\n", chrono::duration<double>(endingClock-startingClock).count());
    setArray(arrayToSort, arraySize);

    startingClock = std::chrono::steady_clock::now();
    qsort(arrayToSort, arraySize, sizeof(int), qsort_compare);
    endingClock = std::chrono::steady_clock::now();
    printf("%.7lf seconds to qsort\n", chrono::duration<double>(endingClock-startingClock).count());
    setArray(arrayToSort, arraySize);

    startingClock = std::chrono::steady_clock::now();
    quickSort(arrayToSort, arraySize);
    endingClock = std::chrono::steady_clock::now();
    printf("%.7lf seconds to quick sort\n", chrono::duration<double>(endingClock-startingClock).count());
    setArray(arrayToSort, arraySize);

    startingClock = std::chrono::steady_clock::now();
    insertionSort(arrayToSort, arraySize);
    endingClock = std::chrono::steady_clock::now();
    printf("%.7lf seconds to insertion sort\n", chrono::duration<double>(endingClock-startingClock).count());
    setArray(arrayToSort, arraySize);

    startingClock = std::chrono::steady_clock::now();
    selectionSort(arrayToSort, arraySize);
    endingClock = std::chrono::steady_clock::now();
    printf("%.7lf seconds to selection sort\n", chrono::duration<double>(endingClock-startingClock).count());
    setArray(arrayToSort, arraySize);

    startingClock = std::chrono::steady_clock::now();
    bubbleSort(arrayToSort, arraySize);
    endingClock = std::chrono::steady_clock::now();
    printf("%.7lf seconds to bubble sort\n", chrono::duration<double>(endingClock-startingClock).count());
    setArray(arrayToSort, arraySize);

    free(arrayToSort);
}