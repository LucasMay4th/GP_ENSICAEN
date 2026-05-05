#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* --- CONSTANTES ET MACROS --- */
#define MAX_LINE_LENGTH 1024
#define MAX_X 500
#define MAX_Y 500
#define MAX_V 11
#define V_OFFSET 5

/* --- VARIABLES GLOBALES --- */
char map[MAX_X][MAX_Y];
int distance_map[MAX_X][MAX_Y];
char *closed_list = NULL;
int width, height, gasLevel;

/* --- STRUCTURES --- */
typedef struct {
    int x, y;
    int vx, vy;
    int g_score; /* Coût depuis le départ */
    int f_score; /* Score total (g + h) */
    
    /* ASTUCE : On mémorise la toute première accélération qui a mené à cet état.
       Cela évite de devoir remonter tout le chemin à la fin de l'A* ! */
    int first_acc_x; 
    int first_acc_y;
} AStarNode;

/* Ici, tu placeras ta structure MinHeap et ses fonctions (push, pop, sift_up...) */

/* --- PHASE 1 : PRÉ-CALCUL (BFS) --- */
void precompute_distances() {
    /* 1. Initialiser distance_map à -1
       2. Trouver toutes les cases '=' et les mettre à 0 dans distance_map et dans la file (queue)
       3. Faire le parcours en largeur (BFS) pour remplir le reste de distance_map
    */
    fprintf(stderr, "[INIT] Pré-calcul des distances terminé.\n");
}

/* --- OUTILS POUR LE A* --- */
int get_state_index(int x, int y, int vx, int vy) {
    return (x * height * MAX_V * MAX_V) + (y * MAX_V * MAX_V) + ((vx + V_OFFSET) * MAX_V) + (vy + V_OFFSET);
}

int heuristique_H(int x, int y) {
    if (distance_map[x][y] == -1) return 999999;
    return distance_map[x][y] / 5; /* Divisé par la vitesse théorique max */
}

/* --- PHASE 2 : L'ALGORITHME A* --- */
/* Cette fonction renvoie un nœud contenant la MEILLEURE première accélération à prendre */
AStarNode compute_best_move(int start_x, int start_y, int start_vx, int start_vy, int ax, int ay, int bx, int by) {
    AStarNode best_action;
    best_action.first_acc_x = 0; /* Par défaut, on freine/ne fait rien */
    best_action.first_acc_y = 0;

    /* 1. Reset de la Closed List */
    memset(closed_list, 0, width * height * MAX_V * MAX_V * sizeof(char));
    
    /* 2. Initialiser le Min-Heap et y mettre l'état de départ */
    /* ... */
    
    /* 3. Boucle principale du A* (Tant que le tas n'est pas vide) */
    /*
        AStarNode current = pop_node(&heap);
        
        if (map[current.x][current.y] == '=') {
            return current; // On a trouvé l'arrivée !
        }
        
        // Pour les 9 accélérations possibles (dvx de -1 à 1, dvy de -1 à 1) :
        //    Calculer new_vx, new_vy, new_x, new_y
        //    Vérifier les collisions (via follow_line) et la sortie de piste
        //    Calculer le nouveau G (G + 1 tour + malus si on frôle ax,ay ou bx,by)
        //    Calculer le nouveau F (G + heuristique_H(new_x, new_y))
        //    Si is_visited() == faux -> mark_visited() et push_node()
    */
    
    return best_action;
}

/* --- BOUCLE PRINCIPALE DU JEU --- */
int main() {
    int row;
    char line_buffer[MAX_LINE_LENGTH];

    /* Lecture des infos de départ */
    fgets(line_buffer, MAX_LINE_LENGTH, stdin);
    sscanf(line_buffer, "%d %d %d", &width, &height, &gasLevel);

    /* Allocation dynamique propre de la Closed List */
    closed_list = (char*)calloc(width * height * MAX_V * MAX_V, sizeof(char));

    /* Lecture de la carte */
    for (row = 0; row < height; ++row) {
        fgets(line_buffer, MAX_LINE_LENGTH, stdin);
        strncpy(map[row], line_buffer, width); // Sauvegarde dans notre tableau global
        fputs(line_buffer, stderr); // On l'envoie aussi dans les logs
    }
    fflush(stderr);

    /* Lancement de notre pré-calcul avant que la course ne commence */
    precompute_distances();

    int round = 0;
    int speedX = 0, speedY = 0;

    /* Boucle de course infinie jusqu'à la fin de la partie */
    while (!feof(stdin)) {
        int myX, myY, secondX, secondY, thirdX, thirdY;
        round++;
        
        /* Lecture des positions envoyées par le GDC */
        fgets(line_buffer, MAX_LINE_LENGTH, stdin);
        sscanf(line_buffer, "%d %d %d %d %d %d", &myX, &myY, &secondX, &secondY, &thirdX, &thirdY);

        /* --- C'EST ICI QUE LA MAGIE OPÈRE --- */
        /* On lance notre A* pour trouver le meilleur mouvement en fonction des adversaires */
        AStarNode best_move = compute_best_move(myX, myY, speedX, speedY, secondX, secondY, thirdX, thirdY);
        
        int accX = best_move.first_acc_x;
        int accY = best_move.first_acc_y;

        /* Mise à jour de notre vitesse théorique (pour le tour suivant) */
        speedX += accX;
        speedY += accY;

        /* Envoi de l'action au gestionnaire de course */
        char action[100];
        sprintf(action, "%d %d", accX, accY);
        fprintf(stdout, "%s\n", action);
        fflush(stdout); /* TRÈS IMPORTANT pour ne pas faire un Time-Out ! */

        fprintf(stderr, "    Action envoyée: %s\n", action);
        fflush(stderr);
    }

    /* Libération de la mémoire à la fin du programme */
    if (closed_list != NULL) free(closed_list);
    return EXIT_SUCCESS;
}