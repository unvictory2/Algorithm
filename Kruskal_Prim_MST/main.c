#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_NODES 10
#define MAX_EDGES 50

// ����. (���,���,����ġ)
typedef struct {
    int u, v, w;
} Edge;

// kruskal���� ���� union-find�� �θ� �迭. �̰ɷ� �� ���� �θ� ������ Ȯ�� > ���� �� cycle ���� ���� �˾Ƴ�
int parent[MAX_NODES];

// parent �ʱ�ȭ. -1�� ���� ��Ʈ��� ��
void makeSet(int n) {
    for (int i = 0; i < n; i++) {
        parent[i] = -1;  
    }
}

// ��������� �θ� ã�� ��Ʈ ��� ��ȯ
int findSet(int u) {
    if (parent[u] == -1) return u; // -1�̸� ���� ��Ʈ�ϱ� �ٷ� ����
    parent[u] = findSet(parent[u]); // �ƴϸ� �θ� ���� ������ ���
    return parent[u];
}

// �� ����� ������ ��ħ
// �׻� �ε����� ���� ��带 �θ�� ���� > ��� �������� ����ȭ ���� ���� ���� �ִµ� �׷� �� �ƴϴϱ� �׳� �ܼ��ϰ�
bool unionSet(int u, int v) {
    int rootU = findSet(u);
    int rootV = findSet(v);
    // �� �� ��Ʈ�� �ٸ� �� ��Ʈ�� �ڽ����� �־������ ��ġ��
    if (rootU != rootV) {
        if (rootU < rootV)
            parent[rootV] = rootU;
        else
            parent[rootU] = rootV;
        return true;
    }
    return false; // �̹� ���� ������ ���
}

// ���� ����ġ ���� �� �Լ� > qsort �Լ��� ��� ����
int compare(const void* a, const void* b) {
    Edge* ea = (Edge*)a;
    Edge* eb = (Edge*)b;
    return ea->w - eb->w;
}

// prim���� �� �켱���� ť(�ּ� ��) ��� ����ü. ���� ���, ����ġ, ��� ���
typedef struct {
    int v;      
    int weight; 
    int from;   
} PQNode;

PQNode priorityQueue[MAX_EDGES]; // �켱���� ť �迭. Ÿ���� ������ ������ ���. 
int pqSize = 0; // ���� ť�� �ִ� ��� ���� �����... c���� .length .size ���� �� �� ���� �� �������...

// �ڱ� ����: �켱���� ť(����)�� ������ �����ϴ� ����? > �켱���� int�� �������� ���� �����ؼ� �����ϰ� ���� / ��� �����ϱ� ����
// ���� ��� PQNode�� weight�� ����

// �� ����. �� ��� �ְ� �� �� ���ϰ��� ������ ������ �����̴� ���� 
void pqPush(int from, int v, int weight) {
    int i = pqSize++;
    // �θ� ���� ���ϸ� ��ġ ��ü
    while (i > 0) { // �߰��� �� �������ٸ� ���� ��尡 ��Ʈ �� ������ �ݺ�(��Ʈ �Ǹ� ��� 0�� �Ǵϱ�)
        int p = (i - 1) / 2; // = ����� �θ� ��� �ε���
        if (priorityQueue[p].weight <= weight) break; // �θ� ��� ����ġ <= �� ���� ����. �ּ����̴ϱ�. 
        priorityQueue[i] = priorityQueue[p];
        i = p;
    }
    priorityQueue[i].v = v;
    priorityQueue[i].weight = weight;
    priorityQueue[i].from = from;
}

// �켱���� ť���� �ּ� ����ġ ��� ���� (�� ����)
// ��Ʈ �� ����, ���� �ڿ� �ִ� ��带 ��Ʈ�� �ø� ���� ���� ��ġ ã�� �ڽİ� �����ϴ� ���� 
PQNode pqPop() {
    PQNode top = priorityQueue[0];     // ��Ʈ ��� (=�ּڰ�)
    PQNode last = priorityQueue[--pqSize]; // ������ ���
    int i = 0;
    // �ڽ� ���� ���� �������� ������
    while (i * 2 + 1 < pqSize) { // �ڽ� ������ ����
        int left = i * 2 + 1; // ���� �ڽ� �ε���. �迭�� 0���� �����ϴϱ� +1
        int right = i * 2 + 2;
        int smallest = left; // ���� �ڽ� ���� �ּҰ� �ĺ��� ����
        if (right < pqSize && priorityQueue[right].weight < priorityQueue[left].weight) // ������ �ڽĵ� �����ϰ�, ������ �ڽ� ����ġ�� ���� �ڽ� ����ġ���� ������
            smallest = right; // �ּҰ� �ĺ��� ������ �ڽ����� ����
        if (priorityQueue[smallest].weight >= last.weight) break; // ���� ��ġ�� �����Ϸ��� ���(last)�� ����ġ�� �ڽ� ���� �� �ּҰ� �̻��̸� �� �Ӽ� �����ϹǷ� �ݺ� ����. �� �� ���� ���� �θ𿩾� �Ǵ� Min heap Ư¡ ���������� �����
        // ���� ���� �� �߰� > �� �θ� �ڽĺ��� ũ�� ���� ���� �ڽ��̶� ��ġ �ٲٱ�
        priorityQueue[i] = priorityQueue[smallest];
        i = smallest; // ���� ���� �۾� �ݺ�
    }
    priorityQueue[i] = last;
    return top; // ó���� �� �ּҰ� ����
}

// ť�� ������� Ȯ��
bool pqEmpty() {
    return pqSize == 0;
}

// Kruskal MST �Լ� (��� ����, ���� �迭, ���� ����,  MST ���� ���� �迭, MST ���� ���� ������)
void kruskal(int n, Edge edges[], int m, Edge mst[], int* mstSize) {
    makeSet(n); // union find�� �迭 �ʱ�ȭ 
    qsort(edges, m, sizeof(Edge), compare); // ���� ����ġ �������� ����

    *mstSize = 0;

    for (int i = 0; i < m; i++) { // ��� ���� ���鼭
        int u = edges[i].u;
        int v = edges[i].v;
        int w = edges[i].w;
        // unionSet�� �� ����� �θ� �����ؼ� �ٸ��� cycle ���� �� �Ǵϱ� ��ġ�� true ����
        // �θ� ������ ������ �� cycle �����Ǵϱ� �� ��ġ�� false �����Ѵ�!
        if (unionSet(u, v)) { 
            mst[(*mstSize)++] = edges[i]; // MST ���� �߰� = ���� �� �߰�
        }
    }
}

// Prim MST �Լ� - ppt�� �ٸ��� �켱���� ť + ��������Ʈ ����� �ִ뼭 �õ�
// ���� (��尳��, �����迭, ��������, MST ���� ���� �迭, MST ���� ���� ������)
void prim(int n, Edge edges[], int m, Edge mst[], int* mstSize) {
    // ���� ����Ʈ ��� ����ü
    typedef struct AdjNode {
        int v;
        int w;
        struct AdjNode* next; // ���� ����Ʈ, ���� ��� ����Ŵ
    } AdjNode;

    AdjNode* graph[MAX_NODES] = { NULL }; // ���� ����Ʈ null�� �ʱ�ȭ

    // edges[] �����͸� ���� ����Ʈ �׷����� �ű��
    // �ϴ� ���� > ȿ�� + ���� ��� �� ���ϴٰ� ��
    for (int i = 0; i < m; i++) {
        // edges[i] ���� ������ ������� �ϳ��� ���� ����Ʈ ��带 ���� �Ҵ�
        AdjNode* node_u = (AdjNode*)malloc(sizeof(AdjNode));
        node_u->v = edges[i].v; // ���� ���� 
        node_u->w = edges[i].w; // ����ġ 
        node_u->next = graph[edges[i].u]; // ���� ����Ʈ�� �տ� ���� (���� ��� ������ ����)
        graph[edges[i].u] = node_u; // ���� u�� ���� ����Ʈ ����� �� ���� ����

        // �ݴ� ���⵵ �����ϰ� ó�� > ������ �׷����� �����
        AdjNode* node_v = (AdjNode*)malloc(sizeof(AdjNode));
        node_v->v = edges[i].u;
        node_v->w = edges[i].w;
        node_v->next = graph[edges[i].v];
        graph[edges[i].v] = node_v;
    }

    bool visited[MAX_NODES] = { false }; // �湮 �迭 �ʱ�ȭ

    // �ʱ�ȭ �۾� ��, ���� �۾� ����
    visited[0] = true;           // ���� ��� 0 �湮 ó��
    pqSize = 0;                 // �켱���� ť �ʱ�ȭ

    // ���� ��忡 ����� ��� ���� ť�� ����
    for (AdjNode* cur = graph[0]; cur != NULL; cur = cur->next) {
        pqPush(0, cur->v, cur->w);
    }

    *mstSize = 0;

    // �ٽ� �κ�
    while (!pqEmpty()) {
        PQNode top = pqPop(); // ����ġ ���� ���� �� ����
        if (visited[top.v]) continue; // �̹� �湮������ �ǳʶ� > �׳� ppt����ó�� ť�� ������ �ǳʶٴ� ��� ���� �͵� ��������?
        visited[top.v] = true; // �湮ó��
        mst[(*mstSize)++] = (Edge){ top.from, top.v, top.weight }; // MST ���� �߰�

        // ���� �湮�� ��忡�� �湮���� ���� ���� ���� ���� ť�� ����
        for (AdjNode* cur = graph[top.v]; cur != NULL; cur = cur->next) {
            if (!visited[cur->v]) pqPush(top.v, cur->v, cur->w);
        }
    }

    // ���� �Ҵ�� ���� ����Ʈ �޸� ����
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

    int n = 10; // ���
    int m = 13; // ����
    Edge edges[] = {
        {0,1,9}, {0,2,1}, {1,2,12}, {1,4,3},
        {2,3,10}, {3,4,7}, {2,5,6}, {3,5,13},
        {5,6,2}, {0,7,11}, {7,8,4}, {1,7,5},
        {4,9,8}
    };

    // �� �޾ƿ� �迭��
    Edge kruskalMST[MAX_NODES - 1];
    Edge primMST[MAX_NODES - 1];
    int kruskalSize = 0, primSize = 0;

    // �˰��� ���� ����
    kruskal(n, edges, m, kruskalMST, &kruskalSize);
    prim(n, edges, m, primMST, &primSize);

    // ��� ��� - Kruskal MST
    printf("==== Kruskal MST ���� ���õ� ���� ====\n");
    for (int i = 0; i < kruskalSize; i++) {
        printf("(%d, %d) ����ġ: %d\n", kruskalMST[i].u, kruskalMST[i].v, kruskalMST[i].w);
    }

    // ��� ��� - Prim MST
    printf("\n==== Prim MST ���� ���õ� ���� ====\n");
    for (int i = 0; i < primSize; i++) {
        printf("(%d, %d) ����ġ: %d\n", primMST[i].u, primMST[i].v, primMST[i].w);
    }

    return 0;
}
