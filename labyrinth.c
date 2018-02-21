/*********************************************************
	Projet Labyrinth - Complexité & Algorithmique
		Université de Franche-Comté
	 Simon Gigant & Loïc Grandperrin - TP2A	
*********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#define BUFSIZE 256

/*********************************************************
			STRUCTURES
*********************************************************/

struct path{
	struct path_node *first;
};

struct path_node{
	struct path_node *next;
	char dir;//Direction prise entre le noeud présent et precedent.
	int north;//Valuation de la direction nord...
	int south;
	int west;
	int east;
};

/*********************************************************
		 DECLARATION DE FONCTIONS
*********************************************************/

/*Initialisation d'un tableau*/
int *initialize_table(int h, int w, int xt, int yt, int x, int y);

/*Initialisation d'un tableau de valuations*/
int *initialize_valuation_table(int h,int w,int xt,int yt,int x,int y,int *tab);

/*Destruction d'une table*/
void destroy_table(int *tab);

/*Decouverte des cases autour du joueur*/
void discover_around(int *tab, int x, int y, int w, int h, int xt, int yt, const char *cells, int *val);

/*Verifie si la case testée est dans les bordures du labyrinth et si il ne s'agit pas d'un mur*/
int check_wall(const int *tab, int x, int y, int w, int h);

/*Recupere la valuation en (x,y) et renvoie dans *dir*/
void get_valuation(int *dir, int *tab, int x, int y, int xt, int yt, int w, int h);

/*Recupere la meilleur direction*/
char get_best_direction(struct path_node *self, bool go_back);

/*Place un mur au point (x,y)*/
void block_direction(struct path_node *self, int *tab, int *val, int x, int y, int w, int xdeb, int ydeb);

/*Déplacer le joueur, modification de la position (x,y)*/
void move(struct path_node *self, int *x, int *y, bool go_back);

/*Evalue les 4 directions dans la pile*/
void valuate_path(const int *tab, int *val, int w, int h, int x, int y, int xt, int yt, struct path_node *p);

/*Prends les valeurs du tableau *val et les place dans le noeud de pile*/
void get_valuation_node(int *dir,int *val,int x,int y,int w);

/*Decremente la valuation de la case actuelle du joueur*/
void fill_val(int *tab,int x,int y,int xt,int yt,int w,int h,int *val);

/*Création de path_node*/
struct path_node *create_path_node(const int *tab, int *val, int w, int h, int x, int y, int xt, int yt);

/*Création d'une pile*/
void stack_create(struct path *self);

/*Verification si la pile est vide*/
bool stack_is_empty(const struct path *self);

/*Ajout d'une donnée dans la pile*/
void stack_push(struct path *self, struct path_node *element);

/*Enleve une donnée dans la pile*/
void stack_pop(struct path *self,int *tab, int *val, int x, int y, int w, int xdeb, int ydeb);

/*Destruction de la pile*/
void stack_destroy(struct path *self);

/*Affichage d'un tableau*/
void print_grid(int *tab, int w, int h, int x, int y);

/*Verifie si le joueur se trouve dans un cul-de-sac*/
bool if_go_back(struct path *self,int *tab, int *val, int x, int y, int w);

/*Affichage d'une pile*/
void print_stack(struct path_node *self);

/*Prend une direction aléatoire (en cas extrême)*/
char go_random_dir(struct path_node *self);

/*Mise à jour de la pile par rapport au tableau *val */
void update_stack(struct path_node *self, int *tab, int *val, int x, int y, int w, int h);

/*********************************************************
			   MAIN
*********************************************************/

int main(int argc, char *argv[]) {
	setbuf(stdout, NULL);
	char buf[BUFSIZE];
	char cells[10];

	// get the width.
	fgets(buf, BUFSIZE, stdin);
	int width = atoi(buf);
	//fprintf(stderr, "Width: %d\n", width);

	// get the height.
	fgets(buf, BUFSIZE, stdin);
	int height = atoi(buf);
	//fprintf(stderr, "height: %d\n", height);

	// get the initial x
	fgets(buf, BUFSIZE, stdin);
	int x = atoi(buf);
	//fprintf(stderr, "x: %d\n", x);

	// get the initial y
	fgets(buf, BUFSIZE, stdin);
	int y = atoi(buf);
	//fprintf(stderr, "y: %d\n", y);

	// get the treasure x.
	fgets(buf, BUFSIZE, stdin);
	int xt = atoi(buf);
	//fprintf(stderr, "xt: %d\n", xt);

	// get the treasure y.
	fgets(buf, BUFSIZE, stdin);
	int yt = atoi(buf);
	//fprintf(stderr, "yt: %d\n", yt);
	
	//Tableau de la topologie du labyrinth (murs, tresor, couloir, cases non explorées).
	int *tab = initialize_table(height,width,xt,yt,x,y);
	
	//Tableau des valuations.
	int *val = initialize_valuation_table(height,width,xt,yt,x,y,tab);

	//Coordonnée initiale du joueur en x.
	int xdeb = x;
	//Coordonnée initiale du joueur en y.
	int ydeb = y;
	
	//Pile
	struct path stack_path;
	stack_create(&stack_path);
	bool go_back = false;
	bool treasure_get = false;

	// initialize the state of the game
	for(;;){
		// get the adjacent cells.
		fgets(buf, BUFSIZE, stdin);
		strcpy(cells,buf);
		// update the state of the game.
		if(tab[x+width*y] == 3){
			treasure_get = true;
		}
		// Met à jour le tableau si le personnage découvre des nouvelles cases.
		discover_around(tab,x,y,width,height,xt,yt,cells,val);
		fprintf(stderr, "x: %d\n",x);
		fprintf(stderr, "y: %d\n",y);
		print_grid(tab,width,height,x,y);
		fprintf(stderr,"\n\n");
		print_grid(val,width,height,x,y);
		fill_val(tab,x,y,xt,yt,width,height,val);
		if(!go_back){
			//Si on ne revient pas en arrière, on crée un nouveau noeud dans la pile.
			stack_push(&stack_path,create_path_node(tab,val,width,height,x,y,xt,yt)); 
		}
		update_stack(stack_path.first, tab, val, x, y, width, height);
		if(!treasure_get){
			go_back = if_go_back(&stack_path,tab,val,x,y,width);
			if(go_back){
				//Si on est dans une impasse, on pop la pile pour retourner en arrière.
				stack_pop(&stack_path,tab,val,x,y,width,xdeb,ydeb);
			}
		//print_stack(stack_path.first);
		// send the new direction.
		move(stack_path.first,&x,&y,go_back);
		}else{
			//Si on a trouver le trésor, on fait que de pop la pile pour retourner à la case de départ.
			go_back = true;
			stack_pop(&stack_path,tab,val,x,y,width,xdeb,ydeb);
			move(stack_path.first,&x,&y,true);
		}
		
		// get the result.
		fgets(buf, BUFSIZE, stdin);
		if (strcmp(buf, "END\n") == 0) {
			break;
		}
	}
	stack_destroy(&stack_path);
	destroy_table(tab);
	destroy_table(val);
	return 0;
}

/*********************************************************
			FONCTIONS
*********************************************************/

// Tableau des murs :
// 0 : non exploré
// 1 : couloir
// 2 : mur
// 3 : trésor

// Tableau des valuations :
// 0 : pas valué
// < 0 : mur réel (-2) ou virtuel (-1)
// > 0 : passage (plus la note est haute, plus la case semble rapprocher du trésor)

/*Crée un tableau de h*w entiers
Place un 3 à la position du trésor, un 1 à la position du joueur et des 0 partout
On modifiera les 0 au fur et à mesure de l'avancée du personnage*/

int *initialize_table(int h, int w, int xt, int yt, int x, int y){
	int *tab = calloc(h*w, sizeof(int));
	for(size_t i = 0; i < h*w; ++i){
		if(i == xt + yt*w){
			tab[i] = 3;
		}else if(i == x + y*w){
			//Le personnage est forcément placé sur un couloir.
			tab[i] = 1;
		}else{
			tab[i] = 0;
		}
	}
	return tab;
}

void print_grid(int *tab, int w, int h, int x, int y){
	for(size_t i = 0; i < w*h; ++i){
		if(i == x+y*w){
			//Place un '°' à la position du joueur dans l'affichage.
			fprintf(stderr,"|°|");
		}else if(tab[i] == 0){
			fprintf(stderr,"| |");
		}else{
			fprintf(stderr,"|%d|",tab[i]);
		}
		if(i%w == w-1){
			fprintf(stderr,"\n");
		}
	}
}

void print_stack(struct path_node *self){
	if(self!=NULL){
		fprintf(stderr,"\nnorth : %d",self->north);
		fprintf(stderr,"\nsouth : %d",self->south);
		fprintf(stderr,"\neast : %d",self->east);
		fprintf(stderr,"\nwest : %d",self->west);
		fprintf(stderr,"\ndir : %c\n",self->dir);
		print_stack(self->next);
	}
}

int *initialize_valuation_table(int h,int w,int xt,int yt,int x,int y,int *tab){
	int *val = calloc(h*w, sizeof(int));
	for(size_t i = 0; i < w*h; ++i){
		val[i] = 0; //0 signifie que le case n'est pas encore valuée.
	}
	val[xt+w*yt] = 10000;
	return val;
}

void fill_val(int *tab,int x,int y,int xt,int yt,int w,int h,int *val){
	//Pour limiter les boucles, on décrémente de 2000 la valuation de toute case visitée.
	//Si on la visite plusieurs fois, elle continue à être décrémentée.
	if(val[x+y*w] > 0){
		val[x+y*w] = val[x+y*w] > 2000 ? val[x+y*w]-2000 : -1;
	}
}

void destroy_table(int *tab){
	free(tab);
}

void discover_around(int *tab, int x, int y, int w, int h, int xt, int yt, const char *cells, int *val){
	for(size_t i = 0; i < 8; ++i){
		int xc;
		int yc;
    		//récupère la position de la case testée :
		if(i < 3){
			xc = x + i - 1;
			yc = y - 1;
		}else if(i < 5){
			xc = i==3 ? x-1 : x+1;
			yc = y;
		}else{
			xc = x + i - 6;
			yc = y + 1;
		}
		//Si la case est dans les limites du labyrinthe et qu'elle n'est pas découverte, alors on indique sa nature (mur ou couloir) dans le tableau.
		if(xc >= 0 && xc < w && yc >= 0 && yc < h){
			int pos = xc + yc*w;
			if(tab[pos] == 0){
				switch(cells[i]){
					case('_'):{
						tab[pos] = 1;
						get_valuation(val+pos,tab,xc,yc,xt,yt,w,h);
						break;
					}
					case('W'):{
						tab[pos] = 2;
						val[pos] = -2;
						break;
					}
				}
			}
		}
	}
}

int check_wall(const int *tab, int x, int y, int w, int h){
	//S'il existe un mur, on retourne -2.
	if(!(x >= 0 && x < w && y >= 0 && y < h) || tab[x+w*y] == 2){
		return -2;
	}
	return 0;
}

void update_stack(struct path_node *self, int *tab, int *val, int x, int y, int w, int h){
	if(check_wall(tab,x,y-1,w,h) != -2){
		self->north = val[x+(y-1)*w];
	}
	if(check_wall(tab,x,y+1,w,h) != -2){
		self->south = val[x+(y+1)*w];
	}
	if(check_wall(tab,x+1,y,w,h) != -2){
		self->east = val[x+1+y*w];
	}
	if(check_wall(tab,x-1,y,w,h) != -2){
		self->west = val[x-1+y*w];
	}
}

void get_valuation(int *dir, int *tab, int x, int y, int xt, int yt, int w, int h){
	if(*dir > -1){
		*dir = 10000-(x-xt)*(x-xt)-(y-yt)*(y-yt);
		int count = 0;
    		//On compte la présence de murs adjacents :
	   	if(check_wall(tab,x,y-1,w,h) == -2){
	      		++count;
	   	}
	    	if(check_wall(tab,x,y+1,w,h) == -2){
	      		++count;
	    	}
	    	if(check_wall(tab,x-1,y,w,h) == -2){
	      		++count;
	    	}
	    	if(check_wall(tab,x+1,y,w,h) == -2){
	      		++count;
	    	}
	    	if(count >= 3){
			//Si il y a trois murs adjacents, on detecte une impasse.
	      		if(*dir >-1){
				*dir = -1;
	      		}
	    	}
  	}
}

char go_random_dir(struct path_node *self){
	switch(rand()%4){
    		case(0):{
     			if(self->north == -2){
        			return go_random_dir(self);
      			}else{
        			return 'n';
      			}
    		}
    		case(1):{
      			if(self->south == -2){
        			return go_random_dir(self);
      			}else{
        			return 's';
      			}
    		}
    		case(2):{
      			if(self->east == -2){
        			return go_random_dir(self);
      			}else{
        			return 'e';
      			}
    		}
    		case(3):{
      			if(self->west == -2){
        			return go_random_dir(self);
      			}else{
        			return 'w';
      			}
    		}
  	}
  	return '\0';
}

char get_best_direction(struct path_node *self, bool go_back){
	char res = 'n';
  	int max = self->north;
  	if(self->south > max){
		//On prend la meilleur direction valuée dans la pile.
    		max = self->south;
    		res = 's';
  	}
  	if(self->east > max){
    		max = self->east;
    		res = 'e';
  	}
  	if(self->west > max){
    		max = self->west;
    		res = 'w';
  	}
  	if(!go_back){
    		fprintf(stderr,"mr : %d\n",max);
    	if(max <= 1){
		//Si la meilleure valuation est un mur, on va dans une direction aléatoire.
      		res = go_random_dir(self);
    	}
  	}else{
    		//Si on revient en arrière, on inverse le déplacement précédent (qui provient de self->first->dir).
    		res = self->dir;
    		fprintf(stderr,"backdir : %c\n",res);
    		if(res == 'n'){
      			return 's';
    		}
    		if(res == 's'){
      			return 'n';
    		}
    		if(res == 'e'){
      			return 'w';
    		}
    		return 'e';
  	}
 	self->dir = res;
  	return res;
}

void move(struct path_node *self, int *x, int *y, bool go_back){
  	switch(get_best_direction(self, go_back)){
    		case('n'):{
      			fprintf(stderr,"NORTH\n");
      			puts("NORTH");
      			--*y;
      			break;
    		}
    		case('s'):{
      			fprintf(stderr,"SOUTH\n");
      			puts("SOUTH");
      			++*y;
      			break;
    		}
    		case('e'):{
      			fprintf(stderr,"EAST\n");
      			puts("EAST");
      			++*x;
      			break;
    		}
    		case('w'):{
      			fprintf(stderr,"WEST\n");
      			puts("WEST");
      			--*x;
      			break;
    		}
  	}
}

bool if_go_back(struct path *self,int *tab, int *val, int x, int y, int w){
  	int count = 0;
	//On revient en arrière si on est entouré de trois murs.
  	if(self->first->north == -1 || self->first->north == -2){
   		++count;
  	}
  	if(self->first->south == -1 || self->first->south == -2){
    		++count;
  	}
  	if(self->first->east == -1 || self->first->east == -2){
    		++count;
  	}
  	if(self->first->west== -1 || self->first->west == -2){
    		++count;
  	}
  	if(count >= 3 && !stack_is_empty(self) && self->first->next != NULL){
    		fprintf(stderr,"prochain : BACK\n");
    		return true;
  	}else{
    		return false;
  	}
}

void block_direction(struct path_node *self, int *tab, int *val, int x, int y, int w, int xdeb, int ydeb){
  	char dir = self->dir;
 	switch(dir){//Ajoute un mur si uniquement ce n'est pas dans la case de départ (par exemple si le départ est une impasse).
	    	case('n'):{
	      		if(x == xdeb && y-1 == ydeb){
				return;
	      		}
	      		self->north = -1;
	      		val[x+(y)*w] = -1;
	      		break;
	    	}
	    	case('s'):{
	      		if(x == xdeb && y+1 == ydeb){
				return;
	      		}
	      		self->south = -1;
	      		val[x+(y)*w] = -1;
	      		break;
	    	}
	    	case('e'):{
	      		if(x+1 == xdeb && y == ydeb){
				return;
	      		}
	      		self->east = -1;
	      		val[x+y*w] = -1;
	      		break;
	    	}
	    	case('w'):{
	      		if(x-1 == xdeb && y == ydeb){
				return;
	      		}
	      		self->west = -1;
	      		val[x+y*w] = -1;
	      		break;
	    	}
  	}
}

void get_valuation_node(int *dir,int *val,int x,int y,int w){
  	if(*dir >-1){
    		*dir = val[x+y*w];
  	}
}

void valuate_path(const int *tab, int *val, int w, int h, int x, int y, int xt, int yt, struct path_node *p){
  	p->north = check_wall(tab,x,y-1,w,h);
  	p->south = check_wall(tab,x,y+1,w,h);
  	p->east = check_wall(tab,x+1,y,w,h);
  	p->west = check_wall(tab,x-1,y,w,h);
  	get_valuation_node(&p->north,val,x,y-1,w);
  	get_valuation_node(&p->south,val,x,y+1,w);
  	get_valuation_node(&p->east,val,x+1,y,w);
  	get_valuation_node(&p->west,val,x-1,y,w);
}

struct path_node *create_path_node(const int *tab, int *val, int w, int h, int x, int y, int xt, int yt){
  	struct path_node *element;
  	element = malloc(sizeof(struct path_node));
  	if(element == NULL){
    		printf("Un problème est survenu dans l'allocation dynamique"); 
    		exit(1);
  	}
	//Pour dire qu'il n'y a pas de direction pour le moment.
  	element->dir = '0';
  	element->next = NULL;
  	valuate_path(tab,val,w,h,x,y,xt,yt,element);
  	return element;
}

void stack_create(struct path *self){
	self->first = NULL;
}

bool stack_is_empty(const struct path *self){
	return (self -> first == NULL);
}

void stack_push(struct path *self, struct path_node *element){
	element->next = self->first;
	self->first = element;
}

void stack_pop(struct path *self,int *tab, int *val, int x, int y, int w, int xdeb, int ydeb){
	if(!stack_is_empty(self)){
		struct path_node *element = self->first;
		self->first = element->next;
		free(element);
		//Si on pop, cela signifie que l'on est revenu sur nos pas et donc que l'endroit est une impasse.
		if(!stack_is_empty(self)){
		  	block_direction(self->first,tab,val,x,y,w,xdeb,ydeb);
		}
	}
}

void path_node_destroy(struct path_node *self){
  	if(self != NULL){
    		path_node_destroy(self->next);
    		free(self);
  	}
}

void stack_destroy(struct path *self){
	path_node_destroy(self->first);
}
