#include <stdio.h>
#include <limits.h>

#define TRUE 1
#define FALSE 0
#define MAX_VERTICES 10
#define INF 1000000

typedef struct GraphType {
    int n; // ���� ����
    int weight[MAX_VERTICES][MAX_VERTICES]; // ����ġ
} GraphType;

int distance[MAX_VERTICES]; // ���� �������� Ư�� �������� �Ÿ�, ���ͽ�Ʈ��
int found[MAX_VERTICES]; // �湮�� ���� ǥ��, ���ͽ�Ʈ��
int A[MAX_VERTICES][MAX_VERTICES]; // �ִ� �Ÿ� ���ſ� ������� ����, �÷��̵�

// ���ͽ�Ʈ�� �� �ܰ躰 ���� ��� �Լ� (distance, found, S ����)
void print_status(GraphType* g, int step) { // step�� �� �ܰ�
    int i;
    printf("STEP %d: distance:", step);
    // // �� ������ ���� ���� �ִ� �Ÿ� �Ǵ� ���� �Ұ� �� * ǥ�� ���
    for (i = 0; i < g->n; i++) {
        if (distance[i] >= INF) // ���� ���� �� �� ��� * 
            printf(" *");
        else
            printf(" %d", distance[i]);
    }
    printf("\nfound:   ");
    // �� ������ �湮(�ִܰŸ� Ȯ��)�Ǿ����� 0/1�� ���(0�� �̹湮, 1�� �湮)
    for (i = 0; i < g->n; i++) {
        printf(" %d", found[i]);
    }
    // found�� true�� �ֵ� = S���� ����ϱ�
    printf("\nS = {");
    int first = 1;
    for (i = 0; i < g->n; i++) {
        if (found[i]) {
            if (!first) printf(", ");
            printf("%d", i);
            first = 0;
        }
    }
    printf("}\n\n");  // �� �ܰ� �� �� �� ���� �߰�
}

// ���õ��� ���� ���� �� �ּ� distance ��ȯ
int choose(int distance[], int n, int found[]) {
    int i, min = INT_MAX, minpos = -1;
    for (i = 0; i < n; i++)
        if (!found[i] && distance[i] < min) { // �湮 �� �߰� + �ּ� �Ÿ�(����ġ)��
            min = distance[i]; // dist(�񱳿�)
            minpos = i; // �ε���(������ȣ)
        }
    return minpos;
}

// ���ͽ�Ʈ�� �� �Լ�. ���ڴ� ����ġ �׷��� g, �������� start
void shortest_path(GraphType* g, int start) {
    int i, u, w;
    int step = 1;
    // ��� ������ ���� �ʱ�ȭ �۾�
    for (i = 0; i < g->n; i++) {
        distance[i] = g->weight[start][i];  // �ʱⰪ�� ���� ����� weight �״�� ������. ���� ������ ���Ѵ�.
        found[i] = FALSE; // �湮�� �� ������ ��� FALSE
    }
    found[start] = TRUE; // ������ �ٷ� �ְ�
    print_status(g, step++);
    for (i = 0; i < g->n - 1; i++) { // �������� �̹� �־����� n-1�� �ݺ��ϸ� "S�� ��� ���� �� ������"��� ���ǰ� ������ ���̴�.
        u = choose(distance, g->n, found); // �Ÿ� ���� ª�� ���� u ����
        if (u == -1) break; // choose���� -1 ���ƿ��� break. choose�� found�� true�� -1 ����. �� �̹� �湮������.
        found[u] = TRUE;
        // �湮�� ���� �α� ������ ���� �� ª�� ��� �ֳ� �˾ƺ��� (u�� �����ϰ� S�� ���� ���� �� ������ ����)
        for (w = 0; w < g->n; w++) 
            if (!found[w] && distance[u] + g->weight[u][w] < distance[w]) // u�� ���� �߰��Ǹ鼭 ���� start->u->w�� start->w���� ���� ���
                distance[w] = distance[u] + g->weight[u][w]; // start->w�� �� ��η� ����
        print_status(g, step++);
    }
}

// �÷��̵� �迭 ��� �Լ� (INF�� *)
void printA(GraphType* g) {
    int i, j;
    printf("�÷��̵� ���� ����:\n");
    for (i = 0; i < g->n; i++) { // ��, ������
        for (j = 0; j < g->n; j++) { // ��, ������
            if (A[i][j] >= INF) // ���� �� �� ��� *�� ǥ��
                printf(" * ");
            else
                printf("%2d ", A[i][j]); // ��� ������ ���� �ִ� �Ÿ� ���
        }
        printf("\n");
    }
    printf("\n");  // �� �ܰ� �� �� �� ���� �߰�
}

// �÷��̵�
void floyd(GraphType* g) {
    int i, j, k;
    for (i = 0; i < g->n; i++)
        for (j = 0; j < g->n; j++)
            A[i][j] = g->weight[i][j]; // A(-1). ���ʷ� �ʱ�ȭ�� A.

    // �ε��� -1�� �ش��ϴ� �ʱ� ��� A0 ���
    printf("A(-1) (�ʱ� ���):\n");
    printA(g);

    for (k = 0; k < g->n; k++) {  // n-1���� �ݺ�. A(0), A(1), A(2), ... A(n-1)
        for (i = 0; i < g->n; i++) // ������ i 
            for (j = 0; j < g->n; j++) // ���� j
                if (A[i][k] + A[k][j] < A[i][j]) // �� ���� k�� �߰��Ǹ� i-k-j��ΰ� i-j���� ª���� �� ��θ� ä��
                    A[i][j] = A[i][k] + A[k][j];

        // ���
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
