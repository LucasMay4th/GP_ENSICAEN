
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "./../../structure_de_base/queue/queue.h" 
#include "./../../follow_line/follow_line.h" 

#define MAX_LINE_LENGTH 1024
#define MAX_BOOSTS 5
#define INF 1e12

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

/* Initialisation et libération de la mémoire */
Map* allocate_map(int width, int height);
void free_map(Map* map);


/* 1. Algorithme de plus court chemin (Pré-calcul) */
void compute_distance_map(Map* map);

/* 2. Gestion des adversaires */
void update_opponents(Map* map, CarState opp1, CarState opp2);
void clear_opponents(Map* map);

/* 3. Validation de trajectoire (utilise follow_line) */
int is_move_valid(Map* map, Position start, Position end);

/* 4. Algorithme DFS (Simulation & Choix d'action) */
int dfs_simulate(Map* map, CarState state, int depth, int is_first_turn, int* best_ax, int* best_ay);

/* 5. Heuristique de Boost */
int is_safe_to_boost(Map* map, CarState state, int ax, int ay);


/* ================= IMPLEMENTATIONS ================= */
Map* allocate_map(int width, int height){
    Map* map ;
    map = (Map*)malloc(sizeof(Map));
    map->width=width;
    map->height =height;
    map->grid=(MapCell**)malloc(height*sizeof(MapCell*));
    for(int i=0;i<height;i++){
        map->grid[i]=(MapCell*)malloc(width*sizeof(MapCell));
    }
    return map;

};
void free_map(Map* map){
    for(int i=0;i<map->height;i++){
        free(map->grid[i]);
    }
    free(map->grid);
    free(map);
};


/* 1. Algorithme de plus court chemin (Pré-calcul) */
void compute_distance_map(Map* map){
    int dx[8] = {-1,  0,  1, -1, 1, -1, 0, 1};
    int dy[8] = {-1, -1, -1,  0, 0,  1, 1, 1};
    int i,j, x, y, nx, ny,current, nouveau_cout;
    Queue q;
    init_queue(&q);
    // init les distance à INF (ou 0 pour les cases d'arrivées) et enqueue les cases d'arrivées
    for(i=0;i<map->height;i++){
        for(j=0;j<map->width;j++){
            if(map->grid[i][j].type=='='){
                map->grid[i][j].dist_to_goal=0;
                Cell cell;
                cell.value=i*map->width+j;
                cell.nextCell=NULL;
                enqueue(&q,cell.value);
            }else{
                map->grid[i][j].dist_to_goal = INF;
            }
        }
    }
    
    while(!isQueueEmpty(q)){
         current = dequeue(&q);
        x = current / map->width;
        y = current % map->width;
        for (i=0;i<8;i++){
            nx = x + dx[i];
            ny = y + dy[i];
            if (nx >= 0 && nx < map->height && ny >= 0 && ny < map->width) {
                if (map->grid[nx][ny].type!='.'){
                    nouveau_cout=map->grid[x][y].dist_to_goal+1;
                    
                }
                if (map->grid[nx][ny].type=='~'){
                        nouveau_cout+=5;
                }
                if (nouveau_cout < map->grid[nx][ny].dist_to_goal) {
                    map->grid[nx][ny].dist_to_goal = nouveau_cout;
                    Cell cell;
                    cell.value=nx*map->width+ny;
                    cell.nextCell=NULL;
                    enqueue(&q,cell.value);
                }
            }
        }
    }
    
};

/* 2. Gestion des adversaires */
void update_opponents(Map* map, CarState opp1, CarState opp2){
    map->grid[opp1.pos.x][opp1.pos.y].has_opponent=1;
    map->grid[opp2.pos.x][opp2.pos.y].has_opponent=1;
};
void clear_opponents(Map* map){
    for(int i=0;i<map->height;i++){
        for(int j=0;j<map->width;j++){
            map->grid[i][j].has_opponent=0;
        }
    }
};

/* 3. Validation de trajectoire (utilise follow_line) */
int is_move_valid(Map* map, Position start, Position end){
    InfoLine infoLine;
    initLine(start.x, start.y, end.x, end.y, &infoLine);
    while(nextPoint(&infoLine, &start, 1) > 0) {
        if(end.x >= 0 && end.x < map->height && end.y >= 0 && end.y < map->width){
            return 0; 
        }
        if (map->grid[end.x][end.y].type == '.'){
            return 0; 
        }
        if(map->grid[end.x][end.y].has_opponent){
            return 0; 
        }
    }
};

/* 4. Algorithme DFS (Simulation & Choix d'action) */
int dfs_simulate(Map* map, CarState state, int depth, int is_first_turn, int* best_ax, int* best_ay){
    int meilleur_score = -INF;
    int ax, ay, score, nouvelle_vx, nouvelle_vy;
    if(depth == 0 || state.gas <= 0) {
        return -map->grid[state.pos.x][state.pos.y].dist_to_goal;
    }
    for (ax = -1; ax <= 1; ax++) {
        for (ay = -1; ay <= 1; ay++) {
            //on simule lanouvelle cinematique
            nouvelle_vx = state.vx + ax;
            nouvelle_vy = state.vy + ay;
            // Vérifier la limite de vitesse stricte
            if(nouvelle_vx > 5) {
                continue;
            }
            Position new_pos;
            new_pos.x = state.pos.x + nouvelle_vx;
            new_pos.y = state.pos.y + nouvelle_vy;
            // 2. Vérifier la validité de la trajectoire
            if(is_move_valid(map, state.pos, new_pos)==0) {
                continue;
            }
            // 3. Cloner l'état et mettre à jour
            CarState new_state;
            new_state.pos = new_pos;
            new_state.vx = nouvelle_vx;
            new_state.vy = nouvelle_vy;
            new_state.gas =     state.gas - 1;
            new_state.boosts_left = state.boosts_left;
            // 4. Appel récursif
            score = dfs_simulate(map, new_state, depth - 1, 0,NULL, NULL);
            // 5. Sauvegarder le meilleur choix (uniquement au premier niveau de l'arbre)
            if(score > meilleur_score) {
                meilleur_score = score;
                if(is_first_turn==1) {
                    *best_ax = ax;
                    *best_ay = ay;
                }
            }
        }
    }

};

/* 5. Heuristique de Boost */
int is_safe_to_boost(Map* map, CarState state, int ax, int ay){
    if(state.boosts_left <= 0) {
        return 0; 
    }
   //simulation grande accélération
    int vitesse_projetée_x = state.vx + 2*ax;
    int vitesse_projetée_y = state.vy + 2*ay;
    // Formule : Distance d'arrêt = V * (V + 1) / 2 (appliquée à la norme de la vitesse)
    int vitesse_norme = sqrt(vitesse_projetée_x*vitesse_projetée_x + vitesse_projetée_y*vitesse_projetée_y);
    int distance_freinage = (vitesse_norme * (vitesse_norme + 1)) / 2;
    // Où sera-t-on si on met "X" tours à s'arrêter sans dévier ?
    Position point_impact_theorique;
    point_impact_theorique.x = state.pos.x + (vitesse_projetée_x / vitesse_norme) * distance_freinage;
    point_impact_theorique.y = state.pos.y + (vitesse_projetée_y / vitesse_norme) * distance_freinage;
    // Si ce lointain point d'impact est un mur ou du hors-piste, le boost est suicidaire
    if(map->grid[point_impact_theorique.y][point_impact_theorique.x].type == '.'){
        return 0; 
    }

};






/* --- Boucle principale --- */

int main(void) {
    /* En C ANSI, toutes les déclarations de variables se font au début de la fonction */
    int width, height, gasLevel;
    char line_buffer[MAX_LINE_LENGTH];
    int row, col;
    Map* map = NULL;
    int round = 0;
    
    CarState me, opp1, opp2;
    me.vx = 0; me.vy = 0; me.boosts_left = MAX_BOOSTS;
    
    /* Lecture des dimensions et du carburant */
    if (fgets(line_buffer, MAX_LINE_LENGTH, stdin) == NULL) return EXIT_FAILURE;
    sscanf(line_buffer, "%d %d %d", &width, &height, &gasLevel);
    me.gas = gasLevel;
    
    /* Initialisation dynamique de la carte */
    map = allocate_map(width, height);
    
    for (row = 0; row < height; ++row) {
        if (fgets(line_buffer, MAX_LINE_LENGTH, stdin) != NULL) {
            for (col = 0; col < width; ++col) {
                map->grid[row][col].type = line_buffer[col];
            }
        }
    }
    
    compute_distance_map(map); /* Pré-calcul des distances à l'arrivée pour l'heuristique */

    /* Boucle de jeu */
    while (!feof(stdin)) {
        int accX = 0, accY = 0;
        
        round++;
        if (fgets(line_buffer, MAX_LINE_LENGTH, stdin) == NULL) break;
        
        sscanf(line_buffer, "%d %d %d %d %d %d",
               &me.pos.x, &me.pos.y, 
               &opp1.pos.x, &opp1.pos.y, 
               &opp2.pos.x, &opp2.pos.y);
               
        /* * LOGIQUE DE L'IA (À DÉVELOPPER ENSEMBLE) :
         * 1. Vérifier la distance avec les adversaires (A* local si proches)
         * 2. Sinon, suivre la trajectoire globale (Dijkstra)
         * 3. Lancer le DFS restreint pour trouver la meilleure accélération (accX, accY)
         * 4. Appliquer is_safe_to_boost() pour doubler l'accélération si possible
         */
         
        /* --- PLACEHOLDER : Exemple d'action par défaut --- */
        dfs_fuel_and_kinematics(map, me, 3, &accX, &accY); 
        
        /* Envoi de l'action au Gestionnaire de Course */
        fprintf(stdout, "%d %d\n", accX, accY);
        fflush(stdout); /* OBLIGATOIRE pour que le GDC reçoive l'ordre immédiatement */
        
        /* Mise à jour locale (Approximative, le GDC a l'autorité finale) */
        me.vx += accX;
        me.vy += accY;
    }

    /* Nettoyage de la mémoire avant de quitter */
    if (map != NULL) {
        free_map(map);
    }
    
    return EXIT_SUCCESS;
}