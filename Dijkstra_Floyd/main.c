#include <stdio.h>
#include <limits.h>

#define TRUE 1
#define FALSE 0
#define MAX_VERTICES 10
#define INF 1000000

typedef struct GraphType {
    int n; // 정점 개수
    int weight[MAX_VERTICES][MAX_VERTICES]; // 가중치
} GraphType;

int distance[MAX_VERTICES]; // 시작 정점에서 특정 정점까지 거리, 다익스트라
int found[MAX_VERTICES]; // 방문한 정점 표시, 다익스트라
int A[MAX_VERTICES][MAX_VERTICES]; // 최단 거리 갱신용 인접행렬 저장, 플로이드

// 다익스트라 각 단계별 상태 출력 함수 (distance, found, S 집합)
void print_status(GraphType* g, int step) { // step은 현 단계
    int i;
    printf("STEP %d: distance:", step);
    // // 각 정점에 대해 현재 최단 거리 또는 도달 불가 시 * 표시 출력
    for (i = 0; i < g->n; i++) {
        if (distance[i] >= INF) // 아직 도달 못 할 경우 * 
            printf(" *");
        else
            printf(" %d", distance[i]);
    }
    printf("\nfound:   ");
    // 각 정점이 방문(최단거리 확정)되었는지 0/1로 출력(0은 미방문, 1은 방문)
    for (i = 0; i < g->n; i++) {
        printf(" %d", found[i]);
    }
    // found가 true인 애들 = S집합 출력하기
    printf("\nS = {");
    int first = 1;
    for (i = 0; i < g->n; i++) {
        if (found[i]) {
            if (!first) printf(", ");
            printf("%d", i);
            first = 0;
        }
    }
    printf("}\n\n");  // 각 단계 뒤 한 줄 공백 추가
}

// 선택되지 않은 정점 중 최소 distance 반환
int choose(int distance[], int n, int found[]) {
    int i, min = INT_MAX, minpos = -1;
    for (i = 0; i < n; i++)
        if (!found[i] && distance[i] < min) { // 방문 안 했고 + 최소 거리(가중치)면
            min = distance[i]; // dist(비교용)
            minpos = i; // 인덱스(정점번호)
        }
    return minpos;
}

// 다익스트라 본 함수. 인자는 가중치 그래프 g, 시작정점 start
void shortest_path(GraphType* g, int start) {
    int i, u, w;
    int step = 1;
    // 모든 정점에 대해 초기화 작업
    for (i = 0; i < g->n; i++) {
        distance[i] = g->weight[start][i];  // 초기값은 인접 행렬의 weight 그대로 가져옴. 간선 없으면 무한대.
        found[i] = FALSE; // 방문한 점 없으니 모두 FALSE
    }
    found[start] = TRUE; // 시작점 바로 넣고
    print_status(g, step++);
    for (i = 0; i < g->n - 1; i++) { // 시작점은 이미 넣었으니 n-1번 반복하면 "S에 모든 정점 들어갈 때까지"라는 조건과 동일한 뜻이다.
        u = choose(distance, g->n, found); // 거리 제일 짧은 정점 u 선택
        if (u == -1) break; // choose에서 -1 돌아오면 break. choose는 found가 true면 -1 리턴. 즉 이미 방문했으면.
        found[u] = TRUE;
        // 방문한 정점 인근 정점에 대해 더 짧은 경로 있나 알아보기 (u에 인접하고 S에 있지 않은 각 정점에 대해)
        for (w = 0; w < g->n; w++) 
            if (!found[w] && distance[u] + g->weight[u][w] < distance[w]) // u가 새로 추가되면서 생긴 start->u->w가 start->w보다 나을 경우
                distance[w] = distance[u] + g->weight[u][w]; // start->w를 그 경로로 수정
        print_status(g, step++);
    }
}

// 플로이드 배열 출력 함수 (INF는 *)
void printA(GraphType* g) {
    int i, j;
    printf("플로이드 현재 상태:\n");
    for (i = 0; i < g->n; i++) { // 행, 시작점
        for (j = 0; j < g->n; j++) { // 렬, 도착점
            if (A[i][j] >= INF) // 도달 못 할 경우 *로 표시
                printf(" * ");
            else
                printf("%2d ", A[i][j]); // 모든 정점에 대해 최단 거리 출력
        }
        printf("\n");
    }
    printf("\n");  // 각 단계 후 한 줄 공백 추가
}

// 플로이드
void floyd(GraphType* g) {
    int i, j, k;
    for (i = 0; i < g->n; i++)
        for (j = 0; j < g->n; j++)
            A[i][j] = g->weight[i][j]; // A(-1). 최초로 초기화된 A.

    // 인덱스 -1에 해당하는 초기 행렬 A0 출력
    printf("A(-1) (초기 행렬):\n");
    printA(g);

    for (k = 0; k < g->n; k++) {  // n-1까지 반복. A(0), A(1), A(2), ... A(n-1)
        for (i = 0; i < g->n; i++) // 시작점 i 
            for (j = 0; j < g->n; j++) // 끝점 j
                if (A[i][k] + A[k][j] < A[i][j]) // 새 정점 k가 추가되며 i-k-j경로가 i-j보다 짧으면 그 경로를 채택
                    A[i][j] = A[i][k] + A[k][j];

        // 출력
        printf("A(%d):\n", k);
        printA(g);
    }
}

int main(void) {
    GraphType g = {
        10,
        {
          {0,5,INF,8,INF,1,INF,INF,9,2},
          {5,0,10,INF,2,INF,6,12,19,3},
          {INF,10,0,7,13,INF,INF,29,5,13},
          {8,INF,7,0,17,13,5,2,INF,1},
          {INF,2,13,17,0,8,16,2,INF,INF},
          {1,INF,INF,13,8,0,INF,23,9,11},
          {INF,6,INF,5,16,INF,0,14,INF,2},
          {INF,12,29,2,2,23,14,0,3,9},
          {9,19,5,INF,INF,9,INF,3,0,4},
          {2,3,13,1,INF,11,2,9,4,0}
        }

    };

    shortest_path(&g, 0);
    printf("\n");
    floyd(&g);

    return 0;
}
