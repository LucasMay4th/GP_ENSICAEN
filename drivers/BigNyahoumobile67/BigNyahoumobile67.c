#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "./structure_de_base/queue/queue.h" 
#include "./../../follow_line/follow_line.h" 

#define MAX_LINE_LENGTH 1024
#define MAX_BOOSTS 5
#define INF 1000000000

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
    char type;         
    int dist_to_goal;  
    int has_opponent;  
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
int is_move_valid(Map* map, Position start, Position end, int* crossed_finish, int* crossed_sand, int check_opponents);
int dfs_simulate(Map* map, CarState state, int depth, int is_first_turn, int* best_ax, int* best_ay);
int gasConsumption(int accX, int accY, int speedX, int speedY, int inSand);

/* ================= IMPLEMENTATIONS ================= */

int gasConsumption(int accX, int accY, int speedX, int speedY, int inSand) {
    int gas = accX * accX + accY * accY;
    gas += (int)floor(sqrt(1.5 * (speedX * speedX + speedY * speedY)));
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
    int dx[8] = {-1,  1,  0,  0, -1, -1,  1,  1};
    int dy[8] = { 0,  0, -1,  1, -1,  1, -1,  1};
    int cost[8] = {10, 10, 10, 10, 14, 14, 14, 14}; 
    int i, j, x, y, nx, ny, current, nouveau_cout;
    Queue *q = createQueue();
    Cell cell;
    char t;
    
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
        y = current / map->width; 
        x = current % map->width; 
        
        for (i = 0; i < 8; i++) {
            nx = x + dx[i];
            ny = y + dy[i];
            
            if (nx >= 0 && nx < map->width && ny >= 0 && ny < map->height) {
                t = map->grid[ny][nx].type;
                if (t == '#' || t == '~' || t == '=' || (t >= '1' && t <= '3')) {
                    nouveau_cout = map->grid[y][x].dist_to_goal + cost[i];
                    if (t == '~') nouveau_cout += 50; 
                    
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
        for(j = 0; j < map->width; j++) map->grid[i][j].has_opponent = 0;
    }
}

int is_move_valid(Map* map, Position start, Position end, int* crossed_finish, int* crossed_sand, int check_opponents) {
    InfoLine infoLine;
    Pos2Dint p; 
    char t;
    *crossed_finish = 0; 
    *crossed_sand = 0;
    
    initLine(start.x, start.y, end.x, end.y, &infoLine);
    
    while(nextPoint(&infoLine, &p, 1) > 0) {
        if(p.x == start.x && p.y == start.y) continue;
         
        if(p.x < 0 || p.x >= map->width || p.y < 0 || p.y >= map->height) return 0; 
        
        t = map->grid[p.y][p.x].type;
        if (t != '#' && t != '~' && t != '=' && !(t >= '1' && t <= '3')) return 0; 
        if (check_opponents && map->grid[p.y][p.x].has_opponent == 1) return 0; 
        
        if (t == '=') *crossed_finish = 1;
        if (t == '~') *crossed_sand = 1;
    }
    return 1; 
}

int dfs_simulate(Map* map, CarState state, int depth, int is_first_turn, int* best_ax, int* best_ay) {
    int meilleur_score = -INF;
    int ax, ay, score, nouvelle_vx, nouvelle_vy, norme_carre, crossed_finish, crossed_sand, in_sand_start;
    int eval_score, v_carre, dist;
    CarState new_state;
    Position new_pos;
    
    if(state.gas <= 0) {
        dist = map->grid[state.pos.y][state.pos.x].dist_to_goal;
        if (dist >= INF) return -INF + 1000;
        return -(dist * 100) - 50000;
    }
    
    if(depth == 0) {
        eval_score = 0;
        v_carre = (state.vx * state.vx) + (state.vy * state.vy);
        dist = map->grid[state.pos.y][state.pos.x].dist_to_goal;
        
        if (dist >= INF) return -INF + 1000;
        
        eval_score -= (dist * 100);   
        eval_score += (state.gas * 600);  
        eval_score -= (v_carre * 150);  
        
        return eval_score;
    }
    
    in_sand_start = (map->grid[state.pos.y][state.pos.x].type == '~') ? 1 : 0;
    
    for (ax = -1; ax <= 1; ax++) {
        for (ay = -1; ay <= 1; ay++) {
            nouvelle_vx = state.vx + ax;
            nouvelle_vy = state.vy + ay;
            norme_carre = (nouvelle_vx * nouvelle_vx) + (nouvelle_vy * nouvelle_vy);
            
            if (norme_carre > 25) continue;
            if (in_sand_start && norme_carre > 1) continue;
            
            new_pos.x = state.pos.x + nouvelle_vx;
            new_pos.y = state.pos.y + nouvelle_vy;
            
            if (new_pos.x < 0 || new_pos.x >= map->width || new_pos.y < 0 || new_pos.y >= map->height) {
                continue;
            }
            
            crossed_finish = 0;
            crossed_sand = 0;
            
            if(is_move_valid(map, state.pos, new_pos, &crossed_finish, &crossed_sand, is_first_turn) == 0) continue;
            if (crossed_sand == 1 && norme_carre > 1) continue;
            
            if (crossed_finish == 1) {
                score = 1000000 + (state.gas * 10) + (depth * 10000); 
            } else {
                new_state.pos = new_pos;
                new_state.vx = nouvelle_vx;
                new_state.vy = nouvelle_vy;
                
                if (crossed_sand) {
                    new_state.vx = (new_state.vx > 0) ? 1 : ((new_state.vx < 0) ? -1 : 0);
                    new_state.vy = (new_state.vy > 0) ? 1 : ((new_state.vy < 0) ? -1 : 0);
                }
                
                new_state.gas = state.gas - gasConsumption(ax, ay, state.vx, state.vy, crossed_sand);
                new_state.boosts_left = state.boosts_left;
                
                score = dfs_simulate(map, new_state, depth - 1, 0, NULL, NULL);
            }
            
            if(meilleur_score == -INF || score > meilleur_score) {
                meilleur_score = score;
                if(is_first_turn == 1 && best_ax != NULL && best_ay != NULL) {
                    *best_ax = ax;
                    *best_ay = ay;
                }
            }
        }
    }
    
    if (meilleur_score == -INF) {
        return -1000000 - (depth * 1000);
    }
    
    return meilleur_score;
}

/* --- Boucle principale --- */

int main(void) {
    int width, height, gasLevel;
    char line_buffer[MAX_LINE_LENGTH];
    int row, col;
    Map* map = NULL;
    int round = 0;
    Position previous_pos;
    int is_first_round = 1;
    int expected_vx = 0, expected_vy = 0;
    /* CORRECTION C89 : Déclaration du compteur d'immobilité au sommet du bloc */
    int consecutive_no_move = 0;
    
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
        int expected_x, expected_y;
        int score;
        
        round++;
        if (fgets(line_buffer, MAX_LINE_LENGTH, stdin) == NULL) break;
        
        sscanf(line_buffer, "%d %d %d %d %d %d",
               &me.pos.x, &me.pos.y, 
               &opp1.pos.x, &opp1.pos.y, 
               &opp2.pos.x, &opp2.pos.y);
        
        if (!is_first_round) {
            expected_x = previous_pos.x + expected_vx;
            expected_y = previous_pos.y + expected_vy;

            if (me.pos.x == expected_x && me.pos.y == expected_y) {
                me.vx = expected_vx;
                me.vy = expected_vy;
            } else {
                me.vx = 0;
                me.vy = 0;
            }

            /* CORRECTION : On vérifie si la voiture a bougé par rapport au tour précédent */
            if (me.pos.x == previous_pos.x && me.pos.y == previous_pos.y) {
                consecutive_no_move++;
            } else {
                consecutive_no_move = 0;
            }
        } else {
            me.vx = 0;
            me.vy = 0;
            is_first_round = 0;
        }
        
        /* CORRECTION : Condition de crash forcé après 20 boucles sans bouger */
        if (consecutive_no_move >= 20) {
            fprintf(stderr, "=== CRASH : Voiture immobile depuis 20 tours. Fin du pilote. ===\n");
            break;
        }
        
        update_opponents(map, opp1, opp2);
        
        accX = 0; 
        accY = 0;
        score = dfs_simulate(map, me, 5, 1, &accX, &accY);
        (void)score; 
        
        clear_opponents(map);
        
        fprintf(stdout, "%d %d\n", accX, accY);
        fflush(stdout); 
        
        previous_pos = me.pos;
        expected_vx = me.vx + accX;
        expected_vy = me.vy + accY;
    }
    
    if (map != NULL) free_map(map);
    return EXIT_SUCCESS;
}