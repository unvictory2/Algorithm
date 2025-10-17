#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_NODES 10
#define MAX_EDGES 50

// 간선. (노드,노드,가중치)
typedef struct {
    int u, v, w;
} Edge;

// kruskal에서 쓰는 union-find용 부모 배열. 이걸로 두 정점 부모가 같은지 확인 > 연결 시 cycle 생성 여부 알아냄
int parent[MAX_NODES];

// parent 초기화. -1은 내가 루트라는 뜻
void makeSet(int n) {
    for (int i = 0; i < n; i++) {
        parent[i] = -1;  
    }
}

// 재귀적으로 부모를 찾아 루트 노드 반환
int findSet(int u) {
    if (parent[u] == -1) return u; // -1이면 내가 루트니까 바로 리턴
    parent[u] = findSet(parent[u]); // 아니면 부모가 나올 때까지 재귀
    return parent[u];
}

// 두 노드의 집합을 합침
// 항상 인덱스가 낮은 노드를 부모로 설정 > 노드 많아지면 최적화 문제 생길 수도 있는데 그런 거 아니니까 그냥 단순하게
bool unionSet(int u, int v) {
    int rootU = findSet(u);
    int rootV = findSet(v);
    // 한 쪽 루트를 다른 쪽 루트의 자식으로 넣어버려서 합치기
    if (rootU != rootV) {
        if (rootU < rootV)
            parent[rootV] = rootU;
        else
            parent[rootU] = rootV;
        return true;
    }
    return false; // 이미 같은 집합일 경우
}

// 간선 가중치 기준 비교 함수 > qsort 함수에 사용 예정
int compare(const void* a, const void* b) {
    Edge* ea = (Edge*)a;
    Edge* eb = (Edge*)b;
    return ea->w - eb->w;
}

// prim에서 쓸 우선순위 큐(최소 힙) 노드 구조체. 도착 노드, 가중치, 출발 노드
typedef struct {
    int v;      
    int weight; 
    int from;   
} PQNode;

PQNode priorityQueue[MAX_EDGES]; // 우선순위 큐 배열. 타입은 위에서 정의한 노드. 
int pqSize = 0; // 현재 큐에 있는 요소 개수 저장용... c언어에선 .length .size 같은 거 못 쓰는 거 기억하자...

// 자구 복습: 우선순위 큐(개념)을 힙으로 구현하는 이유? > 우선순위 int를 기준으로 힙을 정렬해서 간편하게 정렬 / 사용 가능하기 때문
// 예를 들어 PQNode의 weight로 정렬

// 힙 삽입. 새 노드 넣고 힙 내 상하관계 적당한 곳까지 움직이는 과정 
void pqPush(int from, int v, int weight) {
    int i = pqSize++;
    // 부모 노드와 비교하며 위치 교체
    while (i > 0) { // 중간에 안 멈춰진다면 현재 노드가 루트 될 때까지 반복(루트 되면 노드 0이 되니까)
        int p = (i - 1) / 2; // = 노드의 부모 노드 인덱스
        if (priorityQueue[p].weight <= weight) break; // 부모 노드 가중치 <= 새 노드면 정지. 최소힙이니까. 
        priorityQueue[i] = priorityQueue[p];
        i = p;
    }
    priorityQueue[i].v = v;
    priorityQueue[i].weight = weight;
    priorityQueue[i].from = from;
}

// 우선순위 큐에서 최소 가중치 노드 추출 (힙 삭제)
// 루트 값 빼고, 제일 뒤에 있던 노드를 루트로 올린 다음 적정 위치 찾아 자식과 스왑하는 과정 
PQNode pqPop() {
    PQNode top = priorityQueue[0];     // 루트 노드 (=최솟값)
    PQNode last = priorityQueue[--pqSize]; // 마지막 요소
    int i = 0;
    // 자식 노드와 비교해 내려가며 재정렬
    while (i * 2 + 1 < pqSize) { // 자식 없으면 멈춤
        int left = i * 2 + 1; // 왼쪽 자식 인덱스. 배열은 0부터 시작하니까 +1
        int right = i * 2 + 2;
        int smallest = left; // 왼쪽 자식 먼저 최소값 후보로 설정
        if (right < pqSize && priorityQueue[right].weight < priorityQueue[left].weight) // 오른쪽 자식도 존재하고, 오른쪽 자식 가중치가 왼쪽 자식 가중치보다 작으면
            smallest = right; // 최소값 후보를 오른쪽 자식으로 변경
        if (priorityQueue[smallest].weight >= last.weight) break; // 현재 위치에 삽입하려는 노드(last)의 가중치가 자식 노드들 중 최소값 이상이면 힙 속성 만족하므로 반복 종료. 즉 더 작은 값이 부모여야 되는 Min heap 특징 만족했으면 멈춰라
        // 조건 만족 못 했고 > 즉 부모가 자식보다 크면 제일 작은 자식이랑 위치 바꾸기
        priorityQueue[i] = priorityQueue[smallest];
        i = smallest; // 이후 같은 작업 반복
    }
    priorityQueue[i] = last;
    return top; // 처음에 뺀 최소값 리턴
}

// 큐가 비었는지 확인
bool pqEmpty() {
    return pqSize == 0;
}

// Kruskal MST 함수 (노드 개수, 간선 배열, 간선 개수,  MST 간선 저장 배열, MST 간선 개수 포인터)
void kruskal(int n, Edge edges[], int m, Edge mst[], int* mstSize) {
    makeSet(n); // union find용 배열 초기화 
    qsort(edges, m, sizeof(Edge), compare); // 간선 가중치 오름차순 정렬

    *mstSize = 0;

    for (int i = 0; i < m; i++) { // 모든 간선 돌면서
        int u = edges[i].u;
        int v = edges[i].v;
        int w = edges[i].w;
        // unionSet은 두 노드의 부모 추적해서 다르면 cycle 형성 안 되니까 합치고 true 리턴
        // 부모 같으면 합쳤을 때 cycle 형성되니까 안 합치고 false 리턴한다!
        if (unionSet(u, v)) { 
            mst[(*mstSize)++] = edges[i]; // MST 간선 추가 = 구한 답 추가
        }
    }
}

// Prim MST 함수 - ppt랑 다르게 우선순위 큐 + 인접리스트 방식이 있대서 시도
// 인자 (노드개수, 간선배열, 간선개수, MST 간선 저장 배열, MST 간선 개수 포인터)
void prim(int n, Edge edges[], int m, Edge mst[], int* mstSize) {
    // 인접 리스트 노드 구조체
    typedef struct AdjNode {
        int v;
        int w;
        struct AdjNode* next; // 연결 리스트, 다음 노드 가르킴
    } AdjNode;

    AdjNode* graph[MAX_NODES] = { NULL }; // 인접 리스트 null로 초기화

    // edges[] 데이터를 인접 리스트 그래프로 옮기기
    // 하는 이유 > 효율 + 추후 계산 시 편리하다고 함
    for (int i = 0; i < m; i++) {
        // edges[i] 간선 정보를 기반으로 하나의 인접 리스트 노드를 동적 할당
        AdjNode* node_u = (AdjNode*)malloc(sizeof(AdjNode));
        node_u->v = edges[i].v; // 도착 정점 
        node_u->w = edges[i].w; // 가중치 
        node_u->next = graph[edges[i].u]; // 기존 리스트의 앞에 삽입 (기존 헤드 포인터 연결)
        graph[edges[i].u] = node_u; // 정점 u의 인접 리스트 헤더를 새 노드로 갱신

        // 반대 방향도 동일하게 처리 > 무방향 그래프로 만들기
        AdjNode* node_v = (AdjNode*)malloc(sizeof(AdjNode));
        node_v->v = edges[i].u;
        node_v->w = edges[i].w;
        node_v->next = graph[edges[i].v];
        graph[edges[i].v] = node_v;
    }

    bool visited[MAX_NODES] = { false }; // 방문 배열 초기화

    // 초기화 작업 끝, 실제 작업 시작
    visited[0] = true;           // 시작 노드 0 방문 처리
    pqSize = 0;                 // 우선순위 큐 초기화

    // 시작 노드에 연결된 모든 간선 큐에 삽입
    for (AdjNode* cur = graph[0]; cur != NULL; cur = cur->next) {
        pqPush(0, cur->v, cur->w);
    }

    *mstSize = 0;

    // 핵심 부분
    while (!pqEmpty()) {
        PQNode top = pqPop(); // 가중치 제일 낮은 애 제거
        if (visited[top.v]) continue; // 이미 방문했으면 건너뜀 > 그냥 ppt에서처럼 큐에 없으면 건너뛰는 방법 쓰는 것도 괜찮을듯?
        visited[top.v] = true; // 방문처리
        mst[(*mstSize)++] = (Edge){ top.from, top.v, top.weight }; // MST 간선 추가

        // 새로 방문한 노드에서 방문하지 않은 노드로 가는 간선 큐에 삽입
        for (AdjNode* cur = graph[top.v]; cur != NULL; cur = cur->next) {
            if (!visited[cur->v]) pqPush(top.v, cur->v, cur->w);
        }
    }

    // 동적 할당된 인접 리스트 메모리 해제
    for (int i = 0; i < n; i++) {
        AdjNode* cur = graph[i];
        while (cur != NULL) {
            AdjNode* temp = cur;
            cur = cur->next;
            free(temp);
        }
    }
}

int main() {
    

    /*int n = 7;
    int m = 9;
    Edge edges[] = {
        {0,1,9}, {0,2,1}, {1,2,4}, {1,4,3},
        {2,3,5}, {3,4,7}, {2,5,6}, {3,5,8},
        {5,6,2}
    };*/

    int n = 10; // 노드
    int m = 13; // 간선
    Edge edges[] = {
        {0,1,9}, {0,2,1}, {1,2,12}, {1,4,3},
        {2,3,10}, {3,4,7}, {2,5,6}, {3,5,13},
        {5,6,2}, {0,7,11}, {7,8,4}, {1,7,5},
        {4,9,8}
    };

    // 답 받아올 배열들
    Edge kruskalMST[MAX_NODES - 1];
    Edge primMST[MAX_NODES - 1];
    int kruskalSize = 0, primSize = 0;

    // 알고리즘 실제 실행
    kruskal(n, edges, m, kruskalMST, &kruskalSize);
    prim(n, edges, m, primMST, &primSize);

    // 결과 출력 - Kruskal MST
    printf("==== Kruskal MST 최종 선택된 간선 ====\n");
    for (int i = 0; i < kruskalSize; i++) {
        printf("(%d, %d) 가중치: %d\n", kruskalMST[i].u, kruskalMST[i].v, kruskalMST[i].w);
    }

    // 결과 출력 - Prim MST
    printf("\n==== Prim MST 최종 선택된 간선 ====\n");
    for (int i = 0; i < primSize; i++) {
        printf("(%d, %d) 가중치: %d\n", primMST[i].u, primMST[i].v, primMST[i].w);
    }

    return 0;
}
