/* 
   Project    : Tema1 IOCLA - Sistemul lui Biju
   Programmer : Alexandru Olteanu 
   Group      : 322CA
   Email      : alexandruolteanu2001@gmail.com
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT_LINE_SIZE 300

struct Dir;
struct File;

typedef struct Dir{
	char *name;
	struct Dir* parent;
	struct File* head_children_files;
	struct Dir* head_children_dirs;
	struct Dir* next;
} Dir;

typedef struct File {
	char *name;
	struct Dir* parent;
	struct File* next;
} File;

// Eliberarea memoriei unei variabile de tip Fisier
void FreeFile(File *aux) { 
	free(aux->name);	// Am eliberat mai intai sirul de caractere 
	free(aux);	        // responsabil pentru nume si apoi fisierul in sine
	return;
}

// Eliberarea memoriei unei variabile de tip Director
void FreeDir(Dir *aux) { 
	
	free(aux->name); // Am eliberat numele acestuia
	if (aux->head_children_dirs != NULL) {
		Dir *aux_dir = aux->head_children_dirs;
		while (aux_dir != NULL) {
			Dir *aux2 = aux_dir->next;  // Eliberez iterativ si recursiv in acelasi timp
			FreeDir(aux_dir);           // intreaga lista de subdirectoare prezente
			aux_dir = aux2;
		}
	}

	if (aux->head_children_files != NULL) {
		File *aux_file = aux->head_children_files;
		while (aux_file != NULL) {
			File *aux2 = aux_file->next; // Eliberez iterativ lista de fisiere prezenta
			FreeFile(aux_file);
			aux_file = aux2;
		}
	}
    free(aux); // Eliberez in cele din urma variabila Director
	return;
}

// Aloc memorie pentru un sir de caractere
char* createString() {
	char *aux = (char *)malloc(305 * sizeof(char));
	return aux;
}

// Aloc memorie pentru un Fisier avand numele "nume" si ii adaug ca parinte
// directorul "parent" 
File* createFile(Dir *parent, char *nume) {
	File *aux = (File *)malloc(sizeof(File));
	aux->name = createString();
	strcpy(aux->name, nume);
	aux->parent = parent;
	aux->next = NULL;
	return aux;
}

// Aloc memorie pentru Director avand numele "nume", ii adaug parintele si 
// initializez restul membrilor cu NULL (memoria lor va fi alocata doar daca este nevoie)
Dir* createDir(Dir *parent, char *nume) {
	Dir* aux = (Dir *)malloc(sizeof(Dir));
	aux->parent = parent;
    aux->name = createString();
    strcpy(aux->name, nume);
	aux->head_children_files = NULL;
	aux->head_children_dirs = NULL;
	aux->next = NULL;
    return aux;
}

// Functie ce adauga un fisier in directorul curent
void touch (Dir* parent, char* name) {
	int found = 0;
	Dir* aux_dir = parent->head_children_dirs;
	File* aux_file = parent->head_children_files;
	while (aux_dir != NULL && !found) {
		if (!strcmp(name, aux_dir->name)) {
			found = 1;
		}
		aux_dir = aux_dir->next;	// In aceste doua while-uri verific daca numele fisierului
	}					// ce urmeaza a fi creat este deja existent in lista de fisiere
	while (aux_file != NULL && !found) { // sau directoare
		if (!strcmp(name, aux_file->name)) {
			found = 1;
		}
		aux_file = aux_file->next;
	}
	if (found) {		// Daca este prezent nu putem crea fisierul asa ca afisam mesajul de eroare
		printf("File already exists\n");
		return;
	}
	aux_file = parent->head_children_files;
	if (aux_file == NULL) {
		aux_file = createFile(parent, name);    // In cazul in care nu avem fisiere il adaugam ca fiind
		parent->head_children_files = aux_file; // capul listei de fisiere
	}
	else {
		while (aux_file->next != NULL) {
			aux_file = aux_file->next;  // Daca nu, ne deplasam cu un pointer prin lista pana ajungem 
		}				    // la ultimul si il adaugam in urma acestuia
		aux_file->next = createFile(parent, name);
	}

	return;
}

// Functie ce creeaza un Director in Directorul curent
void mkdir (Dir* parent, char* name) {
	int found = 0;
	Dir* aux_dir = parent->head_children_dirs;
	File* aux_file = parent->head_children_files;
	while (aux_dir != NULL && !found) {
		if (!strcmp(name, aux_dir->name)) {
			found = 1;
		}
		aux_dir = aux_dir->next;
	}
	while (aux_file != NULL && !found) {
		if (!strcmp(name, aux_file->name)) {
			found = 1;
		}
		aux_file = aux_file->next;		// Am folosit exact acceasi strategie
	}						// ca la functia touch de mai sus doar
	if (found) {					// ca am lucrat cu directoare
		printf("Directory already exists\n");
		return;
	}
	aux_dir = parent->head_children_dirs;
	if (aux_dir == NULL) {
		aux_dir = createDir(parent, name);
		parent->head_children_dirs = aux_dir;
	}
	else {
		while (aux_dir->next != NULL) {
			aux_dir = aux_dir->next;
		}
		aux_dir->next = createDir(parent, name);
	}

	return;
}

// Afiseaza directoarele si apoi fisierele in ordinea in care au fost create 
// din directorul curent
void ls (Dir* parent) {
	Dir *aux_dir = parent->head_children_dirs;
	File *aux_file = parent->head_children_files;
	while (aux_dir != NULL) {
		printf("%s\n", aux_dir->name);  // Am parcurs lista de directoare si am afisat
		aux_dir = aux_dir->next;	// numele acestora
	}
	while (aux_file != NULL) {			
		printf("%s\n", aux_file->name); //Analog pentru fisiere
		aux_file = aux_file->next; 
	}
	return;
}

// Functie ce sterge un fisier daca acesta este prezent in directorul curent
void rm (Dir* parent, char* name) {
	int found = 0;
	File *aux_file = parent->head_children_files;
	if (aux_file == NULL){
		printf("Could not find the file\n"); // Cazul in care lista de fisiere este goala,
		return;				     // asa ca afisam mesajul de eroare
	}
	if (!strcmp(aux_file->name, name)) {
		if (aux_file->next != NULL) {
			parent->head_children_files = aux_file->next;
		}							// Cazul in care primul fisier din lista
		else {							// este cel ce trebuie sters, caz in care
			parent->head_children_files = NULL;		// trebuie schimbat si capul listei
		}
        FreeFile(aux_file);	// Eliberam memoria fisierului corespunzator dupa stergere
		return;
	}
	while (aux_file->next != NULL && !found) {              // Cat timp nu am ajuns la finalul listei 
		if (!strcmp(aux_file->next->name, name)) {	// si nu am gasit fisierul ce trebuie sters
			File *aux = aux_file->next;
            aux_file->next = aux->next;       // Facem legatura corespunzatoare dupa eliminare
			FreeFile(aux);        // Eliberam memoria fisierului sters
			found = 1; // Am gasit fisierul
		}
        else {
		    aux_file = aux_file->next; // Altfel trecem la urmatorul
        }
	}
	if (!found){
		printf("Could not find the file\n"); // Daca am ajuns la finalul listei si nu 
		return;				     // am gasit fisierul, afisam mesajul de eroare
	}
	
	return;
}

// Functie ce sterge un director din directorul curent
void rmdir (Dir* parent, char* name) {
	int found = 0;
	Dir *aux_dir = parent->head_children_dirs;
	if (aux_dir == NULL) {
		printf("Could not find the dir\n");
		return;
	}
	if (!strcmp(aux_dir->name, name)) {
		if(aux_dir->next != NULL){
			parent->head_children_dirs = aux_dir->next;
		}
		else {
			parent->head_children_dirs = NULL;
		}
        FreeDir(aux_dir);
		return;
	}						  // Am aplicat acceasi metoda ca la functia
	while (aux_dir->next != NULL && !found) {	  // rm, desigur ca aici a contat faptul ca functia
		if (!strcmp(aux_dir->next->name, name)) { // FreeDir este recursiva si sterge intreaga ierarhie
			Dir *aux = aux_dir->next;	  // pe care o contine acel director
            if (aux->next != NULL) {
			    aux_dir->next = aux->next;
            }
            else {
                aux_dir->next = NULL;
            }
			FreeDir(aux);
			found = 1;
		}
        else {
		    aux_dir = aux_dir->next;
        }
	}
	if (!found) {
		printf("Could not find the dir\n");
		return;
	}
	
	return;
}

// Functie ce schimba directorul curent in directorul cu numele name din cel curent
// (sau ne intoarcem inapoi la parinte)
void cd(Dir** target, char *name) {

	int found = 0;
	if (!strcmp(name, "..")) {
		if (((Dir *)*target)->parent != NULL) {
			*target = ((Dir *)*target)->parent; // Cazul in care ne intoarcem inapoi, daca
		}					   // parintele este NULL ramanem aici, daca nu, ne
		return;					  // intoarcem
	}
	Dir *aux_dir = ((Dir *)*target)->head_children_dirs;
	while (aux_dir != NULL && !found) {
		if (!strcmp(aux_dir->name, name)) {
			*target = aux_dir;			 // Verificam daca avem directorul cu numele dorit
			found = 1;				// Daca da, pointam *target la aceasta memorie
		}						// Pointer-ul fiind dublu si parsarea noastra prin
		aux_dir = aux_dir->next;		        // adresa ne permite sa schimbam si in programul 
	}							// principal adresa de memorie la care pointeaza
	if (!found) {						// directorul curent
		printf("No directories found!\n");
	}			// Mesajul de eraore afisat daca nu gasim directorul
	return;
}

 char *pwd (Dir* target) {
     if (target->parent != NULL) {
		 pwd(target->parent);	// Afisam recursiv intregul path apeland mai intai
	 }				// pwd pentru parinte si apoi afisand numele curent
	 printf("/%s", target->name);
}

void stop (Dir* target) {
	FreeDir(target);	// Elibereaza intreaga memorie alocata, constituind astfel inheierea programului
}				// Va primi ca parametru root-ul intregului tree

// Functie ce afiseaza intregul arbore de directoare si fisiere recursiv
void tree (Dir* target, int level) {
	for (int i = 1; i <= 4 * level; ++i){
		printf(" ");		// Afisam numarul de spatii pentru directorul curent
	}
	if (level >= 0) {
		printf("%s\n", target->name); 		// Daca nu suntem in directorul initial de 
	}						// la care s-a pornit functia tree (cel de pe nivelul -1)
							// afisam numele directorului
	Dir *aux_dir = target->head_children_dirs;
	while (aux_dir != NULL) {
		tree(aux_dir, level + 1); // Afisam mai intai iterativ dar recursiv tree-ul corespunzator
		aux_dir = aux_dir->next;  // pentru directoarele prezente in cel curent, incrementand nivelul
	}

	File *aux_file = target->head_children_files;
	while (aux_file != NULL) {
		for(int i = 1; i <= 4 * (level + 1); ++i){
			printf(" ");		// Afisam lista de fisiere cu numarul corespunzator de spatii
		}
		printf("%s\n", aux_file->name);
		aux_file = aux_file->next;
	}
}

// Functie pentru redenumirea unui fisier doar daca acesta exista si noua sa 
// redenumire nu este deja prezenta
void mv(Dir* parent, char *oldname, char *newname) {

	int found_old = 0, found_new = 0;
	File *aux_file = parent->head_children_files;
	Dir *aux_dir = parent->head_children_dirs;

	while (aux_file != NULL) {
		if (!strcmp(aux_file->name, oldname)) {
			found_old = 1;
		}					// Verificam daca lista de fisiere contine fisiere cu 
		if (!strcmp(aux_file->name, newname)) { // unul din numele nou sau vechi
			found_new = 1;
		}
		aux_file = aux_file->next;
	}
	while (aux_dir != NULL) {
		if(!strcmp(aux_dir->name, oldname)){
			found_old = 1;			// Analog pentru directoare
		}
		if(!strcmp(aux_dir->name, newname)){
			found_new = 1;
		}
		aux_dir = aux_dir->next;
	}

	if (!found_old) {
		printf("File/Director not found\n"); // Daca nu am gasit fisierul ce trebuie redenumit
		return;				     // Afisam mesajul corespunzator de eroare
	}

	if (found_new) {
		printf("File/Director already exists\n"); // Daca noul nume este deja utilizat
		return;					  // afisam mesajul de eroare corespunzator
	}

	aux_file = parent->head_children_files;
	aux_dir  = parent->head_children_dirs;

	int ready = 0;

	if (aux_file != NULL) {
		if (!strcmp(aux_file->name, oldname)) {
			strcpy(aux_file->name, newname);
			if (aux_file->next != NULL) {
				parent->head_children_files = aux_file->next; // Daca trebuie redenumit fisierul
				File *aux2 = aux_file;			      // din capul listei trebuie schimbat capul
				while (aux_file->next != NULL) {	      // acesteia si dupa redenumire dus la finalul listei
					aux_file = aux_file->next;
				}
				aux2->next = NULL;
				aux_file->next = aux2;
			}
			ready = 1; // Am gasit fisierul cautat
		}
	}

	if (aux_dir != NULL) {
		if (!strcmp(aux_dir->name, oldname)) {
			strcpy(aux_dir->name, newname);
			if (aux_dir->next != NULL) {
				parent->head_children_dirs = aux_dir->next;
				Dir *aux2 = aux_dir;
				while (aux_dir->next != NULL) {  // Analog pentru capul listei de directoare
					aux_dir = aux_dir->next;
				}
				aux2->next = NULL;
				aux_dir->next = aux2;
			}
			ready = 1;
		}
	}

	if (aux_file != NULL) {
		while (aux_file->next != NULL && !ready) {
			if (!strcmp(aux_file->next->name, oldname)) {
				strcpy(aux_file->next->name, newname);
				File *aux2 = aux_file->next;
				aux_file->next = aux2->next;	  // Cautam in lista de fisiere si daca il gasim refacem legaturile
				while (aux_file->next != NULL) {  // si parcurgem lista pana la final pentru a-l adauga acolo
					aux_file = aux_file->next;
				}
				aux2->next = NULL;
				aux_file->next = aux2;
				ready = 1;
			}
			aux_file = aux_file->next;
		}
	}

	if (aux_dir != NULL) {
		while (aux_dir->next != NULL && !ready) {
			if (!strcmp(aux_dir->next->name, oldname)) {
				strcpy(aux_dir->next->name, newname);
				Dir *aux2 = aux_dir->next;
				aux_dir->next = aux2->next;
				while (aux_dir->next != NULL) { // Analog procedam pentru directoare
					aux_dir = aux_dir->next;
				}
				aux2->next = NULL;
				aux_dir->next = aux2;
				ready = 1;
			}
			aux_dir = aux_dir->next;
		}
	}

	return;

}

int main () {

	char *command = createString();
	char *name = createString();
	char *oldname = createString(); // Creez sirurile ce vor fi folosite de functiile declarate
	char *newname = createString();

    strcpy(name, "home");
	Dir *root = createDir(NULL, name); // Pastrez un pointer root la directorul home pe care il
	Dir *currentDir = root;	           // creez initial deoarece currentDir se poate schimba si
					   // nu vom mai putea dealoca memoria corespunzator
	int ok = 1;
	do
	{
	  ok = 1;  // comanda nu este stop
	  scanf("%s", command);
	  if (!strcmp(command, "touch")) {
		  scanf("%s", name);
		  touch(currentDir, name);
	  }
	  if (!strcmp(command, "mkdir")) {
		  scanf("%s", name);
		  mkdir(currentDir, name);
	  }
	  if (!strcmp(command, "ls")) { // Intreaga apelare de comenzi in functie de comanda
		  ls(currentDir);
	  }
	  if (!strcmp(command, "rm")) {
		  scanf("%s", name);
		  rm(currentDir, name);
	  }
	  if (!strcmp(command, "rmdir")) {
		  scanf("%s", name);
		  rmdir(currentDir, name);
	  }
	  if (!strcmp(command, "cd")) {
		  scanf("%s", name);
		  cd(&currentDir, name); // Observam ca am trimis adresa ca parametru a directorului
	  }				 // curent
	  if (!strcmp(command, "tree")) {
		  tree(currentDir, -1); // Cum am mentionat, pornim de la nivelul -1
	  }
	  if (!strcmp(command, "pwd")) {
		  pwd(currentDir);
	  }
	  if (!strcmp(command, "stop")) {
		  stop(root);
		  ok = 0; // Am gasit comanda stop
	  }
	  // Bonus
	  if (!strcmp(command, "mv")) {
		  scanf("%s", oldname);
		  scanf("%s", newname);
		  mv(currentDir, oldname, newname);
	  }
	
		
	} while (ok);

	free(command); free(name);   // Realizam ultimele free-uri pentru sirulurile de caractere ce au
	free(oldname); free(newname); // ajutat la comenzi!

	return 0;
}
