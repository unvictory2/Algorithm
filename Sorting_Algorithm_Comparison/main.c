#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define SIZE 100000
const int repeat = 10; // 같은 정렬 몇 번 해서 평균낼 건지 (올릴 수록 소요 시간과 정확성 올라감)

#define SWAP(x, y, t) ((t) = (x), (x) = (y), (y) = (t)) // 두 변수 교체 
const char* names[] = { "삽입", "선택", "버블", "쉘", "합병", "퀵" }; // 출력할 때 씀
#define COUNT (sizeof(names) / sizeof(names[0])) // 정렬 함수 개수

int sorted[SIZE]; // 합병 정렬용 버퍼(임시저장)

// 1. 삽입 정렬 : 배열을 두 번째 요소부터 시작, 왼쪽의 정렬된 부분에 순서대로 넣는 정렬
void insertion_sort(int list[], int n)
{
    int i, j, key;
    for (i = 1; i < n; i++) {
        key = list[i]; // 값 가져오기
        for (j = i - 1; j >= 0 && list[j] > key; j--) // list[i-1] > list[i]로 정렬된 왼쪽이랑 비교
            list[j + 1] = list[j]; // 내(key=list[i]) 왼쪽(list[i-1])에 있는 애가 더 크면, 오른쪽으로 한 칸씩 이동시켜 자리 확보
        list[j + 1] = key; // 저장한 key를 i=j+1에 삽입
    }
}

// 2. 선택 정렬 : 매 반복마다 남은 배열에서 최소값을 찾아 현재 위치와 교환
void selection_sort(int list[], int n)
{
    int i, j, least, temp;
    for (i = 0; i < n - 1; i++) {
        least = i; // 현 위치를 최소값으로 초기화
        for (j = i + 1; j < n; j++) // 남은 애들 중 최솟값 탐색
            if (list[j] < list[least]) least = j; // 더 작은 애 있으면 갱신
        SWAP(list[i], list[least], temp); // 최소값 - 현재 위치i와 swap, 자리 확정
    }
}

// 3. 버블 정렬 : 인접한 두 요소를 비교해 크기가 크면 교환, 반복하여 가장 큰 값이 뒤로 이동
void bubble_sort(int list[], int n)
{
    int i, j, temp;
    for (i = n - 1; i > 0; i--) {
        for (j = 0; j < i; j++) // 인접한 두 원소를 비교 - 정렬되지 않은 부분을 순회
            if (list[j] > list[j + 1]) // 크면 위치 바꾸기
                SWAP(list[j], list[j + 1], temp); 
    }
}

// 4. 쉘 정렬 : 넓은 간격(gap)으로 떨어진 요소들을 부분적으로 삽입 정렬
// 단일 함수(나중에 shell sort가 얘 여러 번 불러서 정렬함)
void inc_insertion_sort(int list[], int first, int last, int gap) // 정렬의 범위는 first에서 last
{
    int i, j, key;
    for (i = first + gap; i <= last; i = i + gap) { // first~last, gap씩 건너뛰며
        key = list[i];
        for (j = i - gap; j >= first && key < list[j]; j = j - gap) // 현 key보다 큰 값 찾는 동안 왼쪽으로 gap만큼 건너뛰며
            list[j + gap] = list[j]; // 정렬
        list[j + gap] = key; // 위치 맞췄으니 삽입
    }
}
// 여러 gap 값으로 나누어진 부분 리스트들에 대해 inc_insertion_sort를 반복 호출하여 전체 정렬 수행
void shell_sort(int list[], int n) // n = size
{
    int i, gap;
    for (gap = n / 2; gap > 0; gap = gap / 2) { // gap은 초기에는 배열 길이의 절반에서 시작해 점차 감소
        if ((gap % 2) == 0) gap++; // 짝수 gap은 홀수로 변경 > 최적화 위해서
        for (i = 0; i < gap; i++) // gap 개수만큼 부분 리스트 나누어 각각 삽입 정렬 수행
            inc_insertion_sort(list, i, n - 1, gap);
    }
}

// 5. 합병 정렬 : 배열을 재귀를 이용해 반으로 나눠서 정렬하고 합병해서 전체 정렬 
// 두 정렬된 부분 리스트를 하나로 병합하는 merge. 얘를 밑에서 여러 번 불러서 정렬함
void merge(int list[], int left, int mid, int right)
{
    int i, j, k, l;
    i = left; j = mid + 1; k = left;
    // 분할 정렬된 list의 합병
    while (i <= mid && j <= right) { // 왼쪽 리스트와 오른쪽 리스트를 비교하며 작은 값부터 sorted 배열에 저장
        if (list[i] <= list[j]) sorted[k++] = list[i++]; 
        else sorted[k++] = list[j++];
    }
    if (i > mid) // 왼쪽 리스트가 모두 처리됐으면, 오른쪽 리스트의 나머지 원소 복사
        for (l = j; l <= right; l++)
            sorted[k++] = list[l];
    else // 오른쪽 리스트가 모두 처리됐으면, 왼쪽 리스트의 나머지 원소 복사
        for (l = i; l <= mid; l++)
            sorted[k++] = list[l];
    // 병합한 결과를 원래 배열에 다시 복사
    for (l = left; l <= right; l++)
        list[l] = sorted[l];
}
// 재귀적으로 배열을 반씩 나누고 병합하여 정렬
void merge_sort(int list[], int left, int right)
{
    int mid;
    if (left < right)
    {
        mid = (left + right) / 2; // 중간 지점 계산
        merge_sort(list, left, mid); // 왼쪽 부분 리스트 정렬 재귀 호출
        merge_sort(list, mid + 1, right); // 오른쪽 부분 리스트 정렬 재귀 호출
        merge(list, left, mid, right); // 두 정렬된 부분 리스트 병합
    }
}

// 6. 퀵 정렬 : 배열에서 피벗을 선택하고, 피벗보다 작은 값들은 왼쪽으로, 큰 값들은 오른쪽으로 분할한 후 
// 각 부분을 재귀적으로 정렬. 분할 정복 기반
// 배열을 pivot 기준으로 분할하는 함수 partition
int partition(int list[], int left, int right)
{
    int pivot, temp;
    int low, high;
    low = left;
    high = right + 1;
    pivot = list[left]; // 왼쪽 끝 요소를 pivot으로 선택
    do {
        do
            low++;
        while (low <= right && list[low] < pivot); // pivot보다 작은 값 건너뛰며, pivot 이상 찾기
        do
            high--;
        while (high >= left && list[high] > pivot); // pivot보다 큰 값 건너뛰며, pivot 이하 찾기
        if (low < high) SWAP(list[low], list[high], temp); // 조건 만족 시 값 교환
    } while (low < high);
    SWAP(list[left], list[high], temp); // pivot을 정확한 위치로 이동
    return high;
}

// 재귀적 퀵 정렬 구현, 분할 후 좌우 재정렬
void quick_sort(int list[], int left, int right)
{
    if (left < right) {
        int q = partition(list, left, right); // pivot 기준 분할
        quick_sort(list, left, q - 1); // 왼쪽 부분 정렬
        quick_sort(list, q + 1, right); // 오른쪽 부분 정렬
    }
}

// 정렬 검증 함수
// list랑 answer 배열 비교해서 원소 같은지 확인. 
// 정렬된 값 하나하나 출력해서 됐는지 보는 거 대신 내 코드 정렬값 vs 라이브러리 정렬값 비교해서 성공 여부 확인
int check_sorted(int list[], int answer[], int n) {
    for (int i = 0; i < n; i++)
        if (list[i] != answer[i]) return 0; // 불일치 발견 시 바로 리턴
    return 1;
}

// 비교할 정답 만들 때 쓸 라이브러리 qsort용
// a가 b보다 작으면 음수 반환, 같으면 0, 크면 양수 반환
int compare(const void* a, const void* b) {
    return (*(int*)a - *(int*)b);
}

// 하나의 정렬을 repeat번 실행 후 평균 시간 측정
// void (*sort_func)(int[], int): 함수 포인터 선언. 포인터가 가리키는 함수는 반환값이 없고(void), 두 개의 매개변수(int[], int)를 받는 함수임을 의미
// base: 원본 배열, n: 배열 크기
// answer: 정답 배열, temp: 임시 배열, name: 알고리즘 이름 출력용
double run_sort_average(void (*sort_func)(int[], int), int base[], int n, int answer[], int temp[], const char* name) {
    clock_t start, end;
    double total_time = 0.0;
    // repeat은 코드 맨 위에서 정의 
    for (int i = 0; i < repeat; i++) {
        memcpy(temp, base, sizeof(int) * n); // 원본 배열을 임시 배열로 복사
        start = clock();
        sort_func(temp, n); // 실제 정렬 함수 호출
        end = clock();
        total_time += (double)(end - start) / CLOCKS_PER_SEC; // 클락수를 현실 시간으로 계산
        if (!check_sorted(temp, answer, n)) { // 정렬 결과 검증. qsort()라이브러리로 돌린 결과물과 비교해서 검증한다.  
            printf("%s 정렬 : FAIL on iteration %d\n", name, i + 1);
            break;
        }
    }
    double avg = total_time / repeat; // 평균 시간 계산
    printf("%s 정렬 : %.4f sec (avg over %d runs)\n", name, avg, repeat); // 출력
    return avg;
}

// 세로 막대 그래프 그리기
void print_vertical_bar_graph(const char* names[], double times[], int count) {
    int i, j;
    // 높이 = 가장 오래 걸린 시간 정수로 구하기, 최소값1
    int max_height = (int)(times[0]);
    for (i = 1; i < count; i++) {
        int val = (int)(times[i]);
        if (val > max_height) max_height = val; // 비교하며 가장 오래 걸린 시간 찾음 max_height
    }
    if (max_height < 1) max_height = 1; // 최소값1 보장

    // 각 시간에 대응하는 막대 높이 계산 (0 ~ max_height 사이) 
    int* heights = malloc(sizeof(int) * count); // 각 알고리즘 높이 저장용. count만큼 할당하기 위해 동적할당 해야함. 
    memset(heights, 0, sizeof(int) * count); // 0으로 초기화
    if (heights == NULL) {
        printf("hieghts 할당 실패");
        return;
    }

    // 그래프 높이 결정
    for (i = 0; i < count; i++) {
        if (max_height == 0) heights[i] = 0;
        else { // 기본적으로 높이는 소요 시간을 int로 형변환한 값, 예외로 0이지만 시간이 있긴 한 경우 최소 1로 표현. (쉘, 합병, 퀵은 0.0x초 나오니까)
            heights[i] = ((int)(times[i]) == 0 && times[i] > 0) ? 1 : (int)times[i];
            if (heights[i] > max_height) heights[i] = max_height; // 최고 높이보다 높다면 갱신
        }
    }

    // 그래프 세로 방향 출력. (최대 높이부터 1까지) 여백 잘못 건드리면 그래프에 계단 현상 일어남...
    printf("\n      [ Sorting Time Vertical Bar Graph ]\n\n");
    for (j = max_height; j >= 1; j--) {
        printf("%4d │", j); // 축. %4d로 최소 4자리 공간 확보, 여백 주기 위해. 
        for (i = 0; i < count; i++) {
            if (heights[i] == j) // 막대 꼭대기 모서리
                printf("  ┌┐    "); 
            else if (heights[i] > j) // 막대 몸통
                printf("  ││    ");
            else // 빈 공간 
                printf("        ");
        }
        printf("\n");
    }

// 그래프의 x축. 마찬가지로 여백 주의
    printf("   0 └"); // 좌하단 모서리
    for (i = 0; i < count; i++) {
        if (heights[i]) printf("──┴┴────"); else printf("──────"); // x축, y=0 라인
    }
    printf("\n        ");
    for (i = 0; i < count; i++) printf("%-8s", names[i]); // 각 정렬명

    printf("\n        ");
    for (i = 0; i < count; i++) printf("%.2fs   ", times[i]); // 각 시간+s 붙여 두 칸 공백
    printf("\n(Each bar max height = %d)\n", max_height);

    free(heights); // heights 메모리 해제
}

// 래퍼 wrapper 함수들. 정렬 함수들마다 인자가 다른데, 전부 void func(int[], int)로 부르기 위해서 사용.
// 1) 삽입 정렬 래퍼
void insertion_wrapper(int list[], int n) {
    insertion_sort(list, n);
}

// 2) 선택 정렬 래퍼
void selection_wrapper(int list[], int n) {
    selection_sort(list, n);
}

// 3) 버블 정렬 래퍼
void bubble_wrapper(int list[], int n) {
    bubble_sort(list, n);
}

// 4) 쉘 정렬 래퍼
void shell_wrapper(int list[], int n) {
    shell_sort(list, n);
}

// 5) 합병 정렬 래퍼. 1~4는 사실 래퍼 안 써도 상관 없는데, 합병 정렬 같은 경우 인자 형태가 (int *list, int left, int right)기 때문에 래퍼 없으면 1~4와 동일하게 호출 불가함
void merge_wrapper(int list[], int n) {
    merge_sort(list, 0, n - 1);
}

// 6) 퀵 정렬 래퍼. 얘도 5와 마찬가지
void quick_wrapper(int list[], int n) {
    quick_sort(list, 0, n - 1);
}

int main(void) {
    int i, n = SIZE;
    int baseList[SIZE], tempList[SIZE], answer[SIZE];

    double times[COUNT];

    srand(1234);
    for (i = 0; i < n; i++) baseList[i] = rand(); // 랜덤값 생성
    memcpy(answer, baseList, sizeof(int) * n);
    qsort(answer, n, sizeof(int), compare); // answer는 라이브러리 qsort로 정렬 > 절대 틀릴리 없는 정답으로 사용. (내가 짠 정렬 코드 정상 동작 확인할 때 비교)

    printf("Number of input: %d\n", n);

    // 함수 포인터 배열 선언, 모든 래퍼 함수 여기 넣어서 관리
    void (*wrappers[])(int[], int) = {
        insertion_wrapper,
        selection_wrapper,
        bubble_wrapper,
        shell_wrapper,
        merge_wrapper,
        quick_wrapper
    };

    // 정렬 실행/시간 측정 일괄 수행
    for (i = 0; i < COUNT; i++) {
        times[i] = run_sort_average(wrappers[i], baseList, n, answer, tempList, names[i]);
    }

    // 그래프 출력
    print_vertical_bar_graph(names, times, COUNT);

    return 0;
}

