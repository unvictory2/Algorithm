#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SIZE 100000
#define SWAP(x, y, t) ((t) = (x), (x) = (y), (y) = (t))

int sorted[SIZE]; // 합병 정렬용 버퍼
const int repeat = 10; // 같은 정렬 몇 번 해서 평균낼 건지 (올릴 수록 소요 시간과 정확성 올라감)

const char* names[] = { "삽입", "선택", "버블", "쉘", "합병", "퀵" };

// 1. 삽입 정렬 
void insertion_sort(int list[], int n)
{
    int i, j, key;
    for (i = 1; i < n; i++) {
        key = list[i];
        for (j = i - 1; j >= 0 && list[j] > key; j--)
            list[j + 1] = list[j]; // 레코드의 오른쪽 이동
        list[j + 1] = key;
    }
}

// 2. 선택 정렬
void selection_sort(int list[], int n)
{
    int i, j, least, temp;
    for (i = 0; i < n - 1; i++) {
        least = i;
        for (j = i + 1; j < n; j++) // 최솟값 탐색
            if (list[j] < list[least]) least = j;
        SWAP(list[i], list[least], temp);
    }
}

// 3. 버블 정렬
void bubble_sort(int list[], int n)
{
    int i, j, temp;
    for (i = n - 1; i > 0; i--) {
        for (j = 0; j < i; j++) // 앞뒤의 레코드를 비교한 후 교체
            if (list[j] > list[j + 1])
                SWAP(list[j], list[j + 1], temp);
    }
}

// 4. 쉘 정렬 
// gap 만큼 떨어진 요소들을 삽입 정렬
// 정렬의 범위는 first에서 last
void inc_insertion_sort(int list[], int first, int last, int gap)
{
    int i, j, key;
    for (i = first + gap; i <= last; i = i + gap) {
        key = list[i];
        for (j = i - gap; j >= first && key < list[j]; j = j - gap)
            list[j + gap] = list[j];
        list[j + gap] = key;
    }
}
void shell_sort(int list[], int n) // n = size
{
    int i, gap;
    for (gap = n / 2; gap > 0; gap = gap / 2) {
        if ((gap % 2) == 0) gap++;
        for (i = 0; i < gap; i++) // 부분 리스트의 개수는 gap
            inc_insertion_sort(list, i, n - 1, gap);
    }
}

// 5. 합병 정렬 
void merge(int list[], int left, int mid, int right)
{
    int i, j, k, l;
    i = left; j = mid + 1; k = left;
    // 분할 정렬된 list의 합병
    while (i <= mid && j <= right) {
        if (list[i] <= list[j]) sorted[k++] = list[i++];
        else sorted[k++] = list[j++];
    }
    if (i > mid) // 남아 있는 레코드의 일괄 복사
        for (l = j; l <= right; l++)
            sorted[k++] = list[l];
    else // 남아 있는 레코드의 일괄 복사
        for (l = i; l <= mid; l++)
            sorted[k++] = list[l];
    // 배열 sorted[]의 리스트를 배열 list[]로 복사
    for (l = left; l <= right; l++)
        list[l] = sorted[l];
}

void merge_sort(int list[], int left, int right)
{
    int mid;
    if (left < right)
    {
        mid = (left + right) / 2; // 리스트의 균등분할
        merge_sort(list, left, mid); // 부분리스트 정렬
        merge_sort(list, mid + 1, right);//부분리스트 정렬
        merge(list, left, mid, right); // 합병
    }
}

// 6. 퀵 정렬 
int partition(int list[], int left, int right)
{
    int pivot, temp;
    int low, high;
    low = left;
    high = right + 1;
    pivot = list[left];
    do {
        do
            low++;
        while (low <= right && list[low] < pivot);
        do
            high--;
        while (high >= left && list[high] > pivot);
        if (low < high) SWAP(list[low], list[high], temp);
    } while (low < high);
    SWAP(list[left], list[high], temp);
    return high;
}

void quick_sort(int list[], int left, int right)
{
    if (left < right) {
        int q = partition(list, left, right);
        quick_sort(list, left, q - 1);
        quick_sort(list, q + 1, right);
    }
}

// 정렬 검증 함수
int check_sorted(int list[], int answer[], int n) {
    for (int i = 0; i < n; i++)
        if (list[i] != answer[i]) return 0;
    return 1;
}

// 정답 비교용 함수
int compare(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}

// 하나의 정렬 함수에 대해 10회 실행 후 평균 시간 측정
double run_sort_average(void (*sort_func)(int[], int), int base[], int n, int answer[], int temp[], const char* name) {
    clock_t start, end;
    double total_time = 0.0;
    for (int i = 0; i < repeat; i++) {
        memcpy(temp, base, sizeof(int) * n);
        start = clock();
        sort_func(temp, n);
        end = clock();
        total_time += (double)(end - start) / CLOCKS_PER_SEC;
        if (!check_sorted(temp, answer, n)) {
            printf("%s sort: FAIL on iteration %d\n", name, i + 1);
            break;
        }
    }
    double avg = total_time / repeat;
    printf("%s sort: %.4f sec (avg over %d runs)\n", name, avg, repeat);
    return avg;
}

// 합병 정렬용 10회 평균 측정 별도 함수
double run_merge_sort_average(int base[], int n, int answer[], int temp[], const char* name) {
    clock_t start, end;
    double total_time = 0.0;
    for (int i = 0; i < repeat; i++) {
        memcpy(temp, base, sizeof(int) * n);
        start = clock();
        merge_sort(temp, 0, n - 1);
        end = clock();
        total_time += (double)(end - start) / CLOCKS_PER_SEC;
        if (!check_sorted(temp, answer, n)) {
            printf("%s sort: FAIL on iteration %d\n", name, i + 1);
            break;
        }
    }
    double avg = total_time / repeat;
    printf("%s sort: %.4f sec (avg over %d runs)\n", name, avg, repeat);
    return avg;
}

// 퀵 정렬용 10회 평균 측정 별도 함수
double run_quick_sort_average(int base[], int n, int answer[], int temp[], const char* name) {
    clock_t start, end;
    double total_time = 0.0;
    for (int i = 0; i < repeat; i++) {
        memcpy(temp, base, sizeof(int) * n);
        start = clock();
        quick_sort(temp, 0, n - 1);
        end = clock();
        total_time += (double)(end - start) / CLOCKS_PER_SEC;
        if (!check_sorted(temp, answer, n)) {
            printf("%s 정렬: FAIL on iteration %d\n", name, i + 1);
            break;
        }
    }
    double avg = total_time / repeat;
    printf("%s 정렬: %.4f sec (avg over %d runs)\n", name, avg, repeat);
    return avg;
}

// 세로 막대 그래프, height 동적 지정, 좌우 여백 확장
void print_vertical_bar_graph(const char* names[], double times[], int count) {
    int i, j;
    // 높이 = 가장 오래 걸린 알고리즘의 시간(소수 첫째자리 내림, 최소 1)
    int max_height = (int)(times[0]);
    for (i = 1; i < count; i++) {
        int val = (int)(times[i]);
        if (val > max_height) max_height = val;
    }
    if (max_height < 1) max_height = 1;

    // 막대 높이 계산 (0 ~ max_height)
    int heights[100] = { 0 };
    for (i = 0; i < count; i++) {
        if (max_height == 0) heights[i] = 0;
        else {
            int h = (int)(times[i] / max_height * max_height + 0.5);
            heights[i] = (h == 0 && times[i] > 0) ? 1 : h;
            if (heights[i] > max_height) heights[i] = max_height;
        }
    }

    printf("\n      [ Sorting Time Vertical Bar Graph ]\n\n");
    for (j = max_height; j >= 1; j--) {
        printf("%4d │", j);
        for (i = 0; i < count; i++) {
            if (heights[i] == j)
                printf("  ┌┐    ");
            else if (heights[i] > j)
                printf("  ││    ");
            else
                printf("        ");
        }
        printf("\n");
    }

// 그래프의 x축
    printf("   0 └");
    for (i = 0; i < count; i++) {
        if (heights[i]) printf("──┴┴────"); else printf("──────");
    }
    printf("\n        ");
    for (i = 0; i < count; i++) printf("%-8s", names[i]);

    printf("\n        ");
    for (i = 0; i < count; i++) printf("%.2fs   ", times[i]); // 각 시간+s 붙여 두 칸 공백
    printf("\n(Each bar max height = %d)\n", max_height);
}

int main(void) {
    int i, n = SIZE;
    int baseList[SIZE], tempList[SIZE], answer[SIZE];
    double times[6];

    srand(1234);
    for (i = 0; i < n; i++) baseList[i] = rand();
    memcpy(answer, baseList, sizeof(int) * n);
    qsort(answer, n, sizeof(int), compare);

    printf("Number of input: %d\n", n);

    times[0] = run_sort_average(insertion_sort, baseList, n, answer, tempList, names[0]);
    times[1] = run_sort_average(selection_sort, baseList, n, answer, tempList, names[1]);
    times[2] = run_sort_average(bubble_sort, baseList, n, answer, tempList, names[2]);
    times[3] = run_sort_average(shell_sort, baseList, n, answer, tempList, names[3]);
    times[4] = run_merge_sort_average(baseList, n, answer, tempList, names[4]);
    times[5] = run_quick_sort_average(baseList, n, answer, tempList, names[5]);

    print_vertical_bar_graph(names, times, 6);

    return 0;
}
