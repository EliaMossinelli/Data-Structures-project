#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

/* Route element: stores source/destination pair, linked list node */
typedef struct route {
    int x_dest;
    int y_dest;
    int x_src;
    int y_src;
    struct route *next;
} route;

/* Single hexagon: traversal cost and number of air routes departing from it */
typedef struct {
    unsigned short cost;
    unsigned short num_routes;
} hexagon;

typedef struct {
    unsigned short visited;
    int dist;
    int r;
    int c;
} heap_node;

typedef struct {
    heap_node **array;
    int heap_size;
} MinHeap;

typedef struct {
    unsigned int x;
    unsigned int y;
} coords;

typedef struct {
    int xp;
    int yp;
    int xd;
    int yd;
    int min_distance;
    unsigned short occupied;
} hash_entry;

/* Global hash table */
hash_entry table[997];

route *route_list_head = NULL;

/* Global map and dimensions */
hexagon **map = NULL;
int R;
int C;

/* Dijkstra state and min-heap, allocated once in init */
heap_node *state = NULL;
MinHeap   *Q     = NULL;

/* Function prototypes */
void       init(int col, int rows);
void       change_cost(int x, int y, int v, unsigned int radius);
void       toggle_air_route(int x1, int y1, int x2, int y2);
void       travel_cost(int xp, int yp, int xd, int yd);
int        hex_distance(int X, int Y, int X1, int Y1);
int        approximate(int v, unsigned int radius, int distance);
MinHeap   *create_heap();
void       push(MinHeap *Q, heap_node *node);
heap_node *pop(MinHeap *h);
void       min_heapify(MinHeap *h, int n);
void       swap_nodes(MinHeap *h, int i, int j);
void       convert_coords(int *x, int *y);
void       fill_offsets_even(int *y);
void       fill_offsets_odd(int *y);
void       table_insert(int xP, int yP, int xD, int yD, int distance);
int        hash_fn(int xP, int yP, int xD, int yD);
int        table_search(int xP, int yP, int xD, int yD);
void       table_clear();
void       route_insert(int x1, int y1, int x2, int y2);
int        route_search(int x1, int y1, int x2, int y2);
void       route_delete(int x1, int y1, int x2, int y2);
coords     route_find_dest(int x1, int y1, unsigned short route_index);
void       route_list_clear();

int main() {
    char line[100];

    while (fgets(line, sizeof(line), stdin)) {
        char *cmd = strtok(line, " ");

        if (strcmp(cmd, "init") == 0) {
            char *arg1 = strtok(NULL, " ");
            char *arg2 = strtok(NULL, " ");
            int col  = atoi(arg1);
            int rows = atoi(arg2);
            init(col, rows);
        } else if (strcmp(cmd, "change_cost") == 0) {
            char *arg1 = strtok(NULL, " ");
            char *arg2 = strtok(NULL, " ");
            char *arg3 = strtok(NULL, " ");
            char *arg4 = strtok(NULL, " ");
            int x = atoi(arg1);
            int y = atoi(arg2);
            int v = atoi(arg3);
            unsigned int radius = atoi(arg4);
            change_cost(x, y, v, radius);
        } else if (strcmp(cmd, "toggle_air_route") == 0) {
            char *arg1 = strtok(NULL, " ");
            char *arg2 = strtok(NULL, " ");
            char *arg3 = strtok(NULL, " ");
            char *arg4 = strtok(NULL, " ");
            int x1 = atoi(arg1);
            int y1 = atoi(arg2);
            int x2 = atoi(arg3);
            int y2 = atoi(arg4);
            toggle_air_route(x1, y1, x2, y2);
        } else if (strcmp(cmd, "travel_cost") == 0) {
            char *arg1 = strtok(NULL, " ");
            char *arg2 = strtok(NULL, " ");
            char *arg3 = strtok(NULL, " ");
            char *arg4 = strtok(NULL, " ");
            int xp = atoi(arg1);
            int yp = atoi(arg2);
            int xd = atoi(arg3);
            int yd = atoi(arg4);
            travel_cost(xp, yp, xd, yd);
        }
    }

    for (int i = 0; i < R; i++)
        free(map[i]);
    free(map);
    map = NULL;
    route_list_clear();
    free(state);
    free(Q->array);
    free(Q);

    return 0;
}

/* Convert from spec coordinates to internal (cartesian) representation */
void convert_coords(int *x, int *y) {
    int temp = *x;
    *x = R - 1 - *y;
    *y = temp;
}

/* INIT */
void init(int col, int rows) {
    unsigned int i, j;

    if (map != NULL) {
        for (i = 0; i < R; i++)
            free(map[i]);
        free(map);
        map = NULL;
    }

    table_clear();
    route_list_clear();

    R = rows;
    C = col;

    if (state != NULL)
        free(state);
    state = malloc(R * C * sizeof(heap_node));

    if (Q != NULL) {
        free(Q->array);
        free(Q);
    }
    Q = create_heap();

    map = malloc(R * sizeof(hexagon *));
    for (i = 0; i < R; i++)
        map[i] = malloc(C * sizeof(hexagon));

    for (i = 0; i < R; i++) {
        for (j = 0; j < C; j++) {
            map[i][j].num_routes = 0;
            map[i][j].cost       = 1;
        }
    }

    printf("OK\n");
}

/* CHANGE_COST */
void change_cost(int x, int y, int v, unsigned int radius) {
    if (map == NULL) {
        printf("KO\n");
        return;
    }

    convert_coords(&x, &y);

    if (radius == 0 || y >= C || x >= R || x < 0 || y < 0 || v < -10 || v > 10) {
        printf("KO\n");
        return;
    }

    table_clear();

    int dist;
    int row_min = (x - (int)(radius + 1) < 0) ? 0 : x - (radius + 1);
    int row_max = (x + (int)(radius + 1) > R) ? R : x + (radius + 1);
    int col_min = (y - (int)(radius + 1) < 0) ? 0 : y - (radius + 1);
    int col_max = (y + (int)(radius + 1) > C) ? C : y + (radius + 1);

    for (int h = row_min; h < row_max; h++) {
        for (int k = col_min; k < col_max; k++) {
            dist = hex_distance(x, y, h, k);
            if (dist < (int)radius) {
                int delta = approximate(v, radius, dist);
                if (delta < 0 && map[h][k].cost < (unsigned short)abs(delta))
                    map[h][k].cost = 0;
                else {
                    map[h][k].cost += delta;
                    if (map[h][k].cost > 100)
                        map[h][k].cost = 100;
                }
            }
        }
    }

    printf("OK\n");
}

int approximate(int v, unsigned int radius, int distance) {
    float val = v * fmaxf(0.0f, (float)(radius - distance) / radius);
    return (int)floorf(val);
}

/* Hex distance using cube coordinates */
int hex_distance(int X, int Y, int X1, int Y1) {
    int temp;

    temp = Y;
    Y    = R - 1 - X;
    X    = temp;

    temp = Y1;
    Y1   = R - 1 - X1;
    X1   = temp;

    int a1 = X  - (int)floorf(Y  / 2.0f), b1 = Y,  c1 = -a1 - b1;
    int a2 = X1 - (int)floorf(Y1 / 2.0f), b2 = Y1, c2 = -a2 - b2;

    return (int)fmaxf(fmaxf(abs(a1 - a2), abs(b1 - b2)), abs(c1 - c2));
}

/* TOGGLE_AIR_ROUTE */
void toggle_air_route(int x1, int y1, int x2, int y2) {
    if (map == NULL) {
        printf("KO\n");
        return;
    }

    convert_coords(&x1, &y1);
    convert_coords(&x2, &y2);

    if (x1 < 0 || y1 < 0 || x2 < 0 || y2 < 0 ||
        x1 >= R || x2 >= R || y1 >= C || y2 >= C) {
        printf("KO\n");
        return;
    }

    int found = route_search(x1, y1, x2, y2);

    if (found != 110) {
        /* Route exists: remove it */
        route_delete(x1, y1, x2, y2);
        map[x1][y1].num_routes--;
    } else {
        /* Route does not exist: add it */
        if (map[x1][y1].num_routes == 5) {
            printf("KO\n");
            return;
        }
        route_insert(x1, y1, x2, y2);
        map[x1][y1].num_routes++;
    }

    printf("OK\n");
    table_clear();
}

/* TRAVEL_COST â€” Dijkstra with lazy deletion */
void travel_cost(int xp, int yp, int xd, int yd) {
    if (map == NULL) {
        printf("-1\n");
        return;
    }
    if (xp < 0 || xp >= C || yp < 0 || yp >= R ||
        xd < 0 || xd >= C || yd < 0 || yd >= R) {
        printf("-1\n");
        return;
    }
    if (xp == xd && yp == yd) {
        printf("0\n");
        return;
    }

    convert_coords(&xp, &yp);
    convert_coords(&xd, &yd);

    int new_dist = table_search(xp, yp, xd, yd);
    if (new_dist != -2) {
        printf("%d\n", new_dist);
        return;
    }

    int row_offsets[6] = {-1, 0, +1, +1, 0, -1};
    int col_offsets[6];

    /* Initialise state array */
    for (int i = 0; i < R; i++) {
        for (int j = 0; j < C; j++) {
            heap_node *cur = &state[i * C + j];
            cur->dist    = INT_MAX;
            cur->visited = 0;
            cur->r       = i;
            cur->c       = j;
        }
    }

    state[xp * C + yp].dist = 0;
    push(Q, &state[xp * C + yp]);

    heap_node *u;
    while (Q->heap_size != 0) {
        u = pop(Q);

        if (u->r == xd && u->c == yd) {
            printf("%d\n", state[u->r * C + u->c].dist);
            Q->heap_size = 0;
            table_insert(xp, yp, xd, yd, state[u->r * C + u->c].dist);
            return;
        }

        if (state[u->r * C + u->c].visited == 1)
            continue;
        if (map[u->r][u->c].cost == 0) {
            state[u->r * C + u->c].visited = 1;
            continue;
        }

        state[u->r * C + u->c].visited = 1;

        /* Relax air routes */
        for (int k = 0; k < map[u->r][u->c].num_routes; k++) {
            coords     dst_c = route_find_dest(u->r, u->c, k + 1);
            heap_node *dst   = &state[dst_c.x * C + dst_c.y];
            new_dist = u->dist + map[dst_c.x][dst_c.y].cost;
            if (new_dist < dst->dist) {
                dst->dist = new_dist;
                push(Q, dst);
            }
        }

        /* Choose correct neighbour offsets depending on row parity */
        if (R % 2 == 0)
            (u->r % 2 == 0) ? fill_offsets_even(col_offsets) : fill_offsets_odd(col_offsets);
        else
            (u->r % 2 == 0) ? fill_offsets_odd(col_offsets) : fill_offsets_even(col_offsets);

        /* Relax ground neighbours */
        for (int i = 0; i < 6; i++) {
            int nr = u->r + row_offsets[i];
            int nc = u->c + col_offsets[i];
            if (nr < 0 || nr >= R || nc < 0 || nc >= C)
                continue;
            new_dist = u->dist + map[u->r][u->c].cost;
            heap_node *dst = &state[nr * C + nc];
            if (new_dist < dst->dist) {
                dst->dist = new_dist;
                push(Q, dst);
            }
        }
    }

    printf("-1\n");
    table_insert(xp, yp, xd, yd, -1);
    Q->heap_size = 0;
}

/* MIN-HEAP */
MinHeap *create_heap() {
    MinHeap *h   = malloc(sizeof(MinHeap));
    h->heap_size = 0;
    h->array     = malloc(R * C * sizeof(heap_node *));
    return h;
}

void push(MinHeap *Q, heap_node *node) {
    Q->array[Q->heap_size] = node;
    int i = Q->heap_size;
    while (i > 0 && Q->array[(i - 1) / 2]->dist > Q->array[i]->dist) {
        swap_nodes(Q, (i - 1) / 2, i);
        i = (i - 1) / 2;
    }
    Q->heap_size++;
}

heap_node *pop(MinHeap *h) {
    heap_node *removed = h->array[0];
    h->array[0]        = h->array[h->heap_size - 1];
    h->heap_size--;
    min_heapify(h, 0);
    return removed;
}

void min_heapify(MinHeap *h, int n) {
    int left   = n * 2 + 1;
    int right  = n * 2 + 2;
    int posmin = n;

    if (left  < h->heap_size && h->array[left]->dist  < h->array[posmin]->dist) posmin = left;
    if (right < h->heap_size && h->array[right]->dist < h->array[posmin]->dist) posmin = right;

    if (posmin != n) {
        swap_nodes(h, n, posmin);
        min_heapify(h, posmin);
    }
}

void swap_nodes(MinHeap *h, int i, int j) {
    heap_node *temp = h->array[i];
    h->array[i]     = h->array[j];
    h->array[j]     = temp;
}

/* Neighbour column offsets for offset hex grids */
void fill_offsets_even(int *y) {
    int v[6] = {+1, +1, +1, 0, -1, 0};
    for (int i = 0; i < 6; i++) y[i] = v[i];
}

void fill_offsets_odd(int *y) {
    int v[6] = {0, +1, 0, -1, -1, -1};
    for (int i = 0; i < 6; i++) y[i] = v[i];
}

/* HASH TABLE */
void table_insert(int xP, int yP, int xD, int yD, int distance) {
    unsigned int h = hash_fn(xP, yP, xD, yD);
    while (table[h].occupied == 1)
        h = (h + 1) % 997;

    table[h].xp           = xP;
    table[h].yp           = yP;
    table[h].xd           = xD;
    table[h].yd           = yD;
    table[h].min_distance = distance;
    table[h].occupied     = 1;
}

int hash_fn(int xP, int yP, int xD, int yD) {
    unsigned int h = xP * 73856093u + yP * 19349663u +
                     xD * 83492791u + yD * 2654435761u;
    return (int)(h % 997);
}

int table_search(int xP, int yP, int xD, int yD) {
    unsigned int h = hash_fn(xP, yP, xD, yD);
    while (table[h].occupied == 1) {
        if (table[h].xp == xP && table[h].yp == yP &&
            table[h].xd == xD && table[h].yd == yD)
            return table[h].min_distance;
        h = (h + 1) % 997;
    }
    return -2;
}

void table_clear() {
    for (int i = 0; i < 997; i++)
        table[i].occupied = 0;
}

/* LINKED LIST OF AIR ROUTES */
void route_insert(int x1, int y1, int x2, int y2) {
    route *new_node  = malloc(sizeof(route));
    new_node->next   = route_list_head;
    new_node->x_src  = x1;
    new_node->y_src  = y1;
    new_node->x_dest = x2;
    new_node->y_dest = y2;
    route_list_head  = new_node;
}

int route_search(int x1, int y1, int x2, int y2) {
    route *cur = route_list_head;
    while (cur != NULL) {
        if (cur->x_src == x1 && cur->y_src == y1 &&
            cur->x_dest == x2 && cur->y_dest == y2)
            return map[x1][y1].cost;
        cur = cur->next;
    }
    return 110; /* sentinel: route not found */
}

void route_delete(int x1, int y1, int x2, int y2) {
    route *cur  = route_list_head;
    route *prev = NULL;

    while (cur != NULL) {
        if (cur->x_src == x1 && cur->y_src == y1 &&
            cur->x_dest == x2 && cur->y_dest == y2) {
            if (prev == NULL)
                route_list_head = cur->next;
            else
                prev->next = cur->next;
            free(cur);
            return;
        }
        prev = cur;
        cur  = cur->next;
    }
}

coords route_find_dest(int x1, int y1, unsigned short route_index) {
    route         *cur = route_list_head;
    unsigned short j   = 0;
    coords         dst;

    while (j < route_index) {
        if (cur->x_src == x1 && cur->y_src == y1) {
            j++;
            if (j == route_index)
                break;
        }
        cur = cur->next;
    }

    dst.x = cur->x_dest;
    dst.y = cur->y_dest;
    return dst;
}

void route_list_clear() {
    route *cur = route_list_head;
    while (cur != NULL) {
        route *next = cur->next;
        free(cur);
        cur = next;
    }
    route_list_head = NULL;
}