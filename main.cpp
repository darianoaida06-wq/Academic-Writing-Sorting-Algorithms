#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

// --- Utility Functions ---
void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}
void print_first_n_elements(int arr[], int n, int limit) {
    int count = (n < limit) ? n : limit;

    printf("[ ");
    for (int i = 0; i < count; i++) {
        printf("%d ", arr[i]);
    }
    printf("... ]\n");
}
int compare_ints(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

// --- Sorting Algorithms ---

void bubble_sort(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        bool swapped = false;
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                swap(&arr[j], &arr[j + 1]);
                swapped = true;
            }
        }
        if (!swapped) break;
    }
}

void insertion_sort(int arr[], int n) {
    for (int i = 1; i < n; i++) {
        int key = arr[i];
        int j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
}

void selection_sort(int arr[], int n) {
    for (int i = 0; i < n - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < n; j++) {
            if (arr[j] < arr[min_idx]) {
                min_idx = j;
            }
        }
        swap(&arr[i], &arr[min_idx]);
    }
}

// Merge Sort Helpers
void merge(int arr[], int l, int m, int r) {
    int n1 = m - l + 1;
    int n2 = r - m;

    int *L = (int *)malloc(n1 * sizeof(int));
    int *R = (int *)malloc(n2 * sizeof(int));

    for (int i = 0; i < n1; i++) L[i] = arr[l + i];
    for (int j = 0; j < n2; j++) R[j] = arr[m + 1 + j];

    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) { arr[k] = L[i]; i++; k++; }
    while (j < n2) { arr[k] = R[j]; j++; k++; }

    free(L);
    free(R);
}

void merge_sort_recursive(int arr[], int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;
        merge_sort_recursive(arr, l, m);
        merge_sort_recursive(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}

void merge_sort(int arr[], int n) {
    merge_sort_recursive(arr, 0, n - 1);
}

// Quick Sort Helpers
int partition(int arr[], int low, int high) {
    int mid = low + (high - low) / 2;
    swap(&arr[mid], &arr[high]);

    int pivot = arr[high];
    int i = (low - 1);
    for (int j = low; j <= high - 1; j++) {
        if (arr[j] < pivot) {
            i++;
            swap(&arr[i], &arr[j]);
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return (i + 1);
}

void quick_sort_recursive(int arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quick_sort_recursive(arr, low, pi - 1);
        quick_sort_recursive(arr, pi + 1, high);
    }
}

void quick_sort(int arr[], int n) {
    quick_sort_recursive(arr, 0, n - 1);
}

void c_builtin_qsort(int arr[], int n) {
    qsort(arr, n, sizeof(int), compare_ints);
}

// --- Data Generation ---

void generate_data(int arr[], int size, const char* data_type) {
    if (strcmp(data_type, "random") == 0) {
        for (int i = 0; i < size; i++) arr[i] = rand() % 100000;
    } else if (strcmp(data_type, "sorted") == 0) {
        for (int i = 0; i < size; i++) arr[i] = i;
    } else if (strcmp(data_type, "reverse") == 0) {
        for (int i = 0; i < size; i++) arr[i] = size - i;
    } else if (strcmp(data_type, "almost_sorted") == 0) {
        for (int i = 0; i < size; i++) arr[i] = i;
        int swaps = size * 0.02; /
        if (swaps < 1) swaps = 1;
        for (int i = 0; i < swaps; i++) {
            int idx1 = rand() % size;
            int idx2 = rand() % size;
            swap(&arr[idx1], &arr[idx2]);
        }
    } else if (strcmp(data_type, "flat") == 0) {
        for (int i = 0; i < size; i++) arr[i] = (rand() % 5) + 1; 
    }
}


typedef void (*SortFunction)(int[], int);

void run_test(FILE *csv_file, const char* algo_name, SortFunction sort_func, int *base_data, int size, const char* dtype) {
    if (size > 20000 && (strcmp(algo_name, "Bubble") == 0 ||
                         strcmp(algo_name, "Insertion") == 0 ||
                         strcmp(algo_name, "Selection") == 0)) {
        return;
    }

    int *test_data = (int *)malloc(size * sizeof(int));
    memcpy(test_data, base_data, size * sizeof(int));

    if (size == 1000) {
        printf("\n--- %s on %s data ---\n", algo_name, dtype);
        printf("Before: ");
        print_first_n_elements(test_data, size, 10);
    }

    clock_t start = clock();
    sort_func(test_data, size);
    clock_t end = clock();

    if (size == 1000) {
        printf("After:  ");
        print_first_n_elements(test_data, size, 10);
        printf("\n");
    }

    double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    fprintf(csv_file, "%s,%d,%s,%f\n", algo_name, size, dtype, cpu_time_used);

    free(test_data);
}

int main() {
    srand(time(NULL));

    FILE *csv_file = fopen("benchmark_results.csv", "w");
    if (csv_file == NULL) {
        printf("Error: Could not open file for writing.\n");
        return 1;
    }
    fprintf(csv_file, "Algorithm,Size,DataType,Time_Seconds\n");

    int sizes[] = {1000, 5000, 20000, 50000};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);

    const char *data_types[] = {"random", "sorted", "reverse", "almost_sorted", "flat"};
    int num_types = sizeof(data_types) / sizeof(data_types[0]);

    printf("%-18s | %-8s | %-15s | %s\n", "Algorithm", "Size", "Data Type", "Time (s)");
    printf("------------------------------------------------------------------\n");

    for (int s = 0; s < num_sizes; s++) {
        int size = sizes[s];
        for (int t = 0; t < num_types; t++) {
            const char *dtype = data_types[t];

            int *base_data = (int *)malloc(size * sizeof(int));
            generate_data(base_data, size, dtype);

            run_test(csv_file, "Bubble", bubble_sort, base_data, size, dtype);
            run_test(csv_file, "Insertion", insertion_sort, base_data, size, dtype);
            run_test(csv_file, "Selection", selection_sort, base_data, size, dtype);
            run_test(csv_file, "Merge", merge_sort, base_data, size, dtype);
            run_test(csv_file, "Quick", quick_sort, base_data, size, dtype);
            run_test(csv_file, "Built-in qsort", c_builtin_qsort, base_data, size, dtype);

            free(base_data);
        }
        printf("------------------------------------------------------------------\n");
    }

    fclose(csv_file);
    printf("Results successfully exported to benchmark_results.csv\n");

    return 0;
}
