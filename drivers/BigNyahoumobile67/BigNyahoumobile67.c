#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "./structure_de_base/queue/queue.h" 
#include "./../../follow_line/follow_line.h" 

#define MAX_LINE_LENGTH 1024
#define MAX_BOOSTS 5
#define INF 999999

typedef struct {
    int x;
    int y;
} Position;

typedef struct {
    Position pos;
    int vx;
    int vy;
    int gas;
    int boosts_left;
} CarState;

typedef struct {
    char type;         /* '#', '~', '.', '=' */
    int dist_to_goal;  /* Rempli par Dijkstra */
    int has_opponent;  /* Booléen (1 si adversaire présent ce tour) */
} MapCell;

typedef struct {
    MapCell** grid;
    int width;
    int height;
} Map;

/* ================= PROTOTYPES ================= */
Map* allocate_map(int width, int height);
void free_map(Map* map);
void compute_distance_map(Map* map);
void update_opponents(Map* map, CarState opp1, CarState opp2);
void clear_opponents(Map* map);
int is_move_valid(Map* map, Position start, Position end);
int dfs_simulate(Map* map, CarState state, int depth, int is_first_turn, int* best_ax, int* best_ay);
int is_safe_to_boost(Map* map, CarState state, int ax, int ay);
int gasConsumption(int accX, int accY, int speedX, int speedY, int inSand);

/* ================= IMPLEMENTATIONS ================= */

int gasConsumption(int accX, int accY, int speedX, int speedY, int inSand) {
    int gas = accX * accX + accY * accY;
    gas += (int)(sqrt(speedX * speedX + speedY * speedY) * 3.0 / 2.0);
    if (inSand) {
        gas += 1;
    }
    return gas; 
}

Map* allocate_map(int width, int height) {
    int i;
    Map* map = (Map*)malloc(sizeof(Map));
    map->width = width;
    map->height = height;
    map->grid = (MapCell**)malloc(height * sizeof(MapCell*));
    for(i = 0; i < height; i++){
        map->grid[i] = (MapCell*)malloc(width * sizeof(MapCell));
    }
    return map;
}

void free_map(Map* map) {
    int i;
    for(i = 0; i < map->height; i++){
        free(map->grid[i]);
    }
    free(map->grid);
    free(map);
}


void compute_distance_map(Map* map) {
    int dx[8] = {-1,  0,  1, -1, 1, -1, 0, 1};
    int dy[8] = {-1, -1, -1,  0, 0,  1, 1, 1};
    int i, j, x, y, nx, ny, current, nouveau_cout;
    Queue *q;
    Cell cell;
    
    q = createQueue();
    
    for(i = 0; i < map->height; i++) {
        for(j = 0; j < map->width; j++) {
            if(map->grid[i][j].type == '=') {
                map->grid[i][j].dist_to_goal = 0;
                cell.value = i * map->width + j; 
                cell.nextCell = NULL;
                enqueue(q, cell.value);
            } else {
                map->grid[i][j].dist_to_goal = INF;
            }
        }
    }
    
    
    while(!isQueueEmpty(*q)) {
        
        current = dequeue(q);
        y = current / map->width; /* Ligne */
        x = current % map->width; /* Colonne */
        
        for (i = 0; i < 8; i++) {
            nx = x + dx[i];
            ny = y + dy[i];
            
            if (nx >= 0 && nx < map->width && ny >= 0 && ny < map->height) {
                if (map->grid[ny][nx].type != '.') {
                    nouveau_cout = map->grid[y][x].dist_to_goal + 1;
                    if (map->grid[ny][nx].type == '~') {
                        nouveau_cout += 5;
                    }
                    if (nouveau_cout < map->grid[ny][nx].dist_to_goal) {
                        map->grid[ny][nx].dist_to_goal = nouveau_cout;
                        cell.value = ny * map->width + nx;
                        cell.nextCell = NULL;
                        enqueue(q, cell.value);
                    }
                }
            }
        }
    }
    
    free(q);
}

void update_opponents(Map* map, CarState opp1, CarState opp2) {
    /* Sécurité : on vérifie que l'adversaire est bien sur la carte */
    if (opp1.pos.x >= 0 && opp1.pos.x < map->width && opp1.pos.y >= 0 && opp1.pos.y < map->height) {
        map->grid[opp1.pos.y][opp1.pos.x].has_opponent = 1;
    }
    if (opp2.pos.x >= 0 && opp2.pos.x < map->width && opp2.pos.y >= 0 && opp2.pos.y < map->height) {
        map->grid[opp2.pos.y][opp2.pos.x].has_opponent = 1;
    }
}

void clear_opponents(Map* map) {
    int i, j;
    for(i = 0; i < map->height; i++) {
        for(j = 0; j < map->width; j++) {
            map->grid[i][j].has_opponent = 0;
        }
    }
}

int is_move_valid(Map* map, Position start, Position end) {
    InfoLine infoLine;
    Pos2Dint p; 
    
    initLine(start.x, start.y, end.x, end.y, &infoLine);
    
    while(nextPoint(&infoLine, &p, 1) > 0) {
        if(p.x == start.x && p.y == start.y) {
            continue;
        } 
        if(p.x < 0 || p.x >= map->width || p.y < 0 || p.y >= map->height) {
            return 0; 
        }
        if (map->grid[p.y][p.x].type == '.' || map->grid[p.y][p.x].has_opponent == 1) {
            return 0; 
        }
    }
    return 1; 
}

int dfs_simulate(Map* map, CarState state, int depth, int is_first_turn, int* best_ax, int* best_ay) {
    int meilleur_score = -INF;
    int ax, ay, score, nouvelle_vx, nouvelle_vy, norme_carre, in_sand;
    CarState new_state;
    Position new_pos;
    
    if(depth == 0 || state.gas <= 0) {
        return -map->grid[state.pos.y][state.pos.x].dist_to_goal;
    }
    
    for (ax = -1; ax <= 1; ax++) {
        for (ay = -1; ay <= 1; ay++) {
            nouvelle_vx = state.vx + ax;
            nouvelle_vy = state.vy + ay;
            
            norme_carre = (nouvelle_vx * nouvelle_vx) + (nouvelle_vy * nouvelle_vy);
            
            in_sand = (map->grid[state.pos.y][state.pos.x].type == '~') ? 1 : 0;
            if (in_sand && norme_carre > 1) continue;
            if (!in_sand && norme_carre > 25) continue;
            
            new_pos.x = state.pos.x + nouvelle_vx;
            new_pos.y = state.pos.y + nouvelle_vy;
            
            if(is_move_valid(map, state.pos, new_pos) == 0) {
                continue;
            }
            
            new_state.pos = new_pos;
            new_state.vx = nouvelle_vx;
            new_state.vy = nouvelle_vy;
            new_state.gas = state.gas - gasConsumption(ax, ay, state.vx, state.vy, in_sand);
            new_state.boosts_left = state.boosts_left;
            
            score = dfs_simulate(map, new_state, depth - 1, 0, NULL, NULL);
            
            if(score > meilleur_score) {
                meilleur_score = score;
                if(is_first_turn == 1 && best_ax != NULL && best_ay != NULL) {
                    *best_ax = ax;
                    *best_ay = ay;
                }
            }
        }
    }
    return meilleur_score;
}

int is_safe_to_boost(Map* map, CarState state, int ax, int ay) {
    int vitesse_projetee_x, vitesse_projetee_y, distance_freinage;
    double vitesse_norme;
    Position point_impact_theorique;
    
    if(state.boosts_left <= 0 || (ax == 0 && ay == 0)) {
        return 0; 
    }
    
    vitesse_projetee_x = state.vx + 2 * ax;
    vitesse_projetee_y = state.vy + 2 * ay;
    
    vitesse_norme = sqrt(vitesse_projetee_x * vitesse_projetee_x + vitesse_projetee_y * vitesse_projetee_y);
    if (vitesse_norme == 0) return 0;
    
    distance_freinage = (int)((vitesse_norme * (vitesse_norme + 1)) / 2);
    
    point_impact_theorique.x = state.pos.x + (int)((vitesse_projetee_x / vitesse_norme) * distance_freinage);
    point_impact_theorique.y = state.pos.y + (int)((vitesse_projetee_y / vitesse_norme) * distance_freinage);
    
    if(point_impact_theorique.x < 0 || point_impact_theorique.x >= map->width ||
       point_impact_theorique.y < 0 || point_impact_theorique.y >= map->height) {
        return 0;
    }
    
    if(map->grid[point_impact_theorique.y][point_impact_theorique.x].type == '.') {
        return 0; 
    }
    return 1;
}

/* --- Boucle principale --- */

int main(void) {
    int width, height, gasLevel;
    char line_buffer[MAX_LINE_LENGTH];
    int row, col;
    Map* map = NULL;
    int round = 0;
    
    CarState me, opp1, opp2;
    me.vx = 0; me.vy = 0; me.boosts_left = MAX_BOOSTS;
    
    if (fgets(line_buffer, MAX_LINE_LENGTH, stdin) == NULL) return EXIT_FAILURE;
    sscanf(line_buffer, "%d %d %d", &width, &height, &gasLevel);
    me.gas = gasLevel;
    
    map = allocate_map(width, height);
    
    for (row = 0; row < height; ++row) {
        if (fgets(line_buffer, MAX_LINE_LENGTH, stdin) != NULL) {
            for (col = 0; col < width; ++col) {
                map->grid[row][col].type = line_buffer[col];
                map->grid[row][col].has_opponent = 0;
            }
        }
    }
    
    compute_distance_map(map); 

    while (!feof(stdin)) {
        int accX = 0, accY = 0;
        
        round++;
        if (fgets(line_buffer, MAX_LINE_LENGTH, stdin) == NULL) break;
        
        sscanf(line_buffer, "%d %d %d %d %d %d",
               &me.pos.x, &me.pos.y, 
               &opp1.pos.x, &opp1.pos.y, 
               &opp2.pos.x, &opp2.pos.y);
         
        update_opponents(map, opp1, opp2);
        
        dfs_simulate(map, me, 4, 1, &accX, &accY);
        
        if (is_safe_to_boost(map, me, accX, accY)) {
            accX *= 2;
            accY *= 2;
            me.boosts_left--;
        }
        
        clear_opponents(map);
        
        fprintf(stdout, "%d %d\n", accX, accY);
        fflush(stdout); 
        
        me.vx += accX;
        me.vy += accY;
    }

    if (map != NULL) {
        free_map(map);
    }
    
    return EXIT_SUCCESS;
}