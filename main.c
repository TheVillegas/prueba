#include "hashmap.h"
#include "list.h"
#include "stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#define MAX 31

typedef struct
{
  char jugador[MAX];
  int puntosHabilidad;
  int numeroItems;
  HashMap *items; // Mapa de ítems

  Stack *registro; // pila de última función accedida
  Stack *registroSum; // pila de últimos puntos de habilidad registrados
  Stack *registroItem; // pila de último ítem registrado

} Jugador; // Definimos nuestra estructura a trabajar.


// Subfunciones

void validarOpcion(int *opcion) // Valida las opciones del menú.
{
  char opcionAux[MAX];
  
  while (true) 
  {
    scanf("%30s", opcionAux);
    printf("\n");
      
    if (isdigit(*opcionAux)) 
    {
      *opcion = atoi(opcionAux);
        
      if (*opcion >= 1 && *opcion <= 10) break;
      else printf("Debe ingresar un número válido entre 1 y 10.\n");
    }
    else printf("Debe ingresar un número válido entre 1 y 10.\n");
    
    getchar();
  }
}

void menu() // Opciones del menú.
{
  printf("\nEliga una opción a continuación.\n\n");

  printf("1. Crear perfil de jugador(a).\n");
  printf("2. Mostrar perfil de jugador(a).\n");
  printf("3. Agregar ítem a jugador(a).\n");
  printf("4. Eliminar ítem a jugador(a).\n");
  printf("5. Agregar puntos de habilidad a jugador(a).\n");
  printf("6. Mostrar jugadores con un ítem específico.\n");
  printf("7. Deshacer última acción de jugador(a).\n");
  printf("8. Cargar datos de jugadores.\n");
  printf("9. Exportar datos de jugadores.\n");
  printf("10. Salir del programa.\n\n");
}

void validar(int *user_continue) // Validamos que el usuario desee seguir con la ejecución del programa.
{
  char salida[2];
  
  printf("Desea realizar otra operación? (s/n)\n");
  scanf("%1s", salida);
  
  if(strcmp(salida, "n") == 0)
  {
    *user_continue = 0;
    printf("\nGracias por usar el programa, adiós!");
  }
  else while (strcmp(salida, "s") != 0)
  {
    printf("Ingrese una opción válida\n\n");
    printf("Desea realizar otra operación? (s/n)\n");
    scanf("%1s", salida);
  }
}

char *gets_csv_field(char *tmp, int k) //
{
  int open_mark = 0;
  char *ret = (char*) malloc(100 * sizeof(char));
  int ini_i = 0, i = 0;
  int j = 0;
  int last_field = 0;
  
  while (tmp[i + 1] != '\0')
  {  
    if(tmp[i] == '\"')
    {
      open_mark = 1 - open_mark;
      if(open_mark) ini_i = i + 1;
      i++;
      continue;
    }
    
    if(open_mark || tmp[i] != ',')
    {
      if(k == j) ret[i - ini_i] = tmp[i];
      i++;
      continue;
    }

    if(tmp[i] == ',')
    {
      if(k == j) 
      {
        ret[i - ini_i] = 0;
        return ret;
      }
      j++; 
      ini_i = i + 1;
    }
    i++;
  }

  if(k == j) 
  {
    last_field = 1;
    ret[i - ini_i] = 0;
    return ret;
  }

  if (last_field && k == j)
  {
    strcpy(ret + strlen(ret), tmp + ini_i);
    return ret;
  }
  
  return NULL;
}

void preguntarNombreUsuario(char *nombreJugador) // Solicita que el usuario ingrese el nombre de usuario del jugador
{
  printf("Ingrese el nombre de usuario del jugador:\n");
  getchar();
  scanf("%30[^\n]s", nombreJugador);
}

// Funciones 

// 1.
void crearPerfil(HashMap *mapa, int *registrado)
{
  Jugador *jugadorAux = malloc(sizeof(Jugador));
  jugadorAux -> items = createMap(1000);
  jugadorAux -> registro = stack_create();
  jugadorAux -> registroItem = stack_create();
  jugadorAux -> registroSum = stack_create();
  
  stack_push(jugadorAux -> registro, "0");
  
  char username[MAX], item[MAX];
  int puntosHab, numItems, contItems = 1;

  preguntarNombreUsuario(username);
  strcpy(jugadorAux -> jugador, username);
    
  insertMap(mapa, jugadorAux -> jugador, jugadorAux);
  
  (*registrado)++;

  printf("\nJugador registrado!\n\n");
}

// 2.
void mostrarPerfil(HashMap *mapa)
{
  char username[MAX];
  int cont = 0;
  
  preguntarNombreUsuario(username);
  
  Pair *par = searchMap(mapa, username);
  
  if (par != NULL)
  {
    puts("\n~~~~~~~~~~~~~~~~~ JUGADOR ENCONTRADO ~~~~~~~~~~~~~~~~~");
    Jugador *jugadorAux = par -> value;
    printf("\nJugador: %s\n", jugadorAux -> jugador);
    printf("\nPuntos de habilidad: %d\n", jugadorAux -> puntosHabilidad);
    printf("\nNúmero de ítems: %d\n", jugadorAux -> numeroItems);
  
    if(jugadorAux -> numeroItems == 0) 
    {
      puts("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~o~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
      return;
    }
    
    printf("\nÍtems: \n\n");
    
    Pair *auxItems = firstMap(jugadorAux->items);

    while(auxItems != NULL)
    {
      printf(" - %s\n", auxItems->key);
      auxItems = nextMap(jugadorAux -> items);
      cont++;
    }
    
    puts("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~o~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  }
  else printf("\nEl jugador ' %s ' no se encuentra en el mapa.\n\n", username);
}

// 3. 
void agregarItem(HashMap *mapa)
{  
  //
  char username[MAX];
  char *item = (char*) malloc(sizeof(char)); 
  preguntarNombreUsuario(username);
  
  Pair *par = searchMap(mapa, username);
  
  if (par != NULL)
  {
    Jugador *jugadorAux = par -> value;
    
    printf("\nIngrese el ítem que desea guardar:\n");
    getchar();
    scanf("%30s", item);
    
    insertMap(jugadorAux -> items, item, NULL);
    stack_push(jugadorAux-> registroItem, item);
    stack_push(jugadorAux -> registro, "3");
    
    printf("\nNuevo ítem guardado!\n\n");
    
    jugadorAux -> numeroItems++;
  }
  else printf("\nEl jugador ' %s ' no se encuentra en el mapa.\n\n", username);
}

// 4.
void eliminarItem(HashMap *mapa)
{
  char username[MAX];
  char item[MAX];
   
  preguntarNombreUsuario(username);
  Pair *par = searchMap(mapa, username);
  
  if (par != NULL)
  {
    Jugador *jugadorAux = par -> value;
    
    if(jugadorAux -> numeroItems == 0)
    {
      printf("\nEl jugador no tiene ítems.\n\n");
      return;
    }
    
    printf("\nIngrese el ítem que desee eliminar:\n");
    getchar();
    scanf("%30[^\n]s", item);
    
    Pair *parAux = searchMap(jugadorAux -> items, item);
    
    if(parAux != NULL)
    {
      stack_push(jugadorAux-> registroItem, strdup(item));
      eraseMap(jugadorAux -> items, item);
  
      stack_push(jugadorAux -> registro, "4");
  
      printf("\nÍtem eliminado!\n\n");

      jugadorAux -> numeroItems--;

      return;
    }
    
    printf("\nEl jugador no tiene el ítem ' %s '.\n\n", item);
  }
  else printf("\nEl jugador ' %s ' no se encuentra en el sistema.\n\n", username);
  
}

// 5.
void agregarPuntosHabilidad(HashMap *mapa)
{
  char username[MAX];
  preguntarNombreUsuario(username);
  Pair *par = searchMap(mapa, username);
  
  if (par != NULL)
  {
    Jugador *jugadorAux = par -> value;
    char *numHabilidad = (char*) malloc(sizeof(char));
      
    while(true) 
    {
      printf("\nCantidad de puntos que desea agregar:\n");
      scanf("%30s", numHabilidad);
      printf("\n");
        
      if (isdigit(*numHabilidad)) 
      {
        int puntos = atoi(numHabilidad);
        
        if(puntos > 0)
        {
          jugadorAux -> puntosHabilidad = jugadorAux -> puntosHabilidad + puntos;
          printf("Los puntos fueron agregados!\n\n");
          stack_push(jugadorAux -> registroSum, numHabilidad);
          stack_push(jugadorAux -> registro, "5");
          return;
        }
        else printf("Debe ingresar un número entero positivo\n");
      }
      else printf("Debe ingresar un número\n");
      
      getchar();
    }
  }
  else printf("\nEl jugador ' %s ' no se encuentra en el sistema.\n\n", username);
}

// 6.
void mostrarJugadoresConItemEsp(HashMap *mapa)
{
  char item[MAX];
  int cont = 0; 
  printf("Ingrese el ítem que desea buscar:\n");
  getchar();
  scanf("%30[^\n]s", item);
  
  Pair *aux = firstMap(mapa);

  while(aux != NULL)
  {
    Jugador *jugadorAux = aux -> value;
    Pair *auxI = firstMap(jugadorAux->items);
    
    if(auxI == NULL) aux = nextMap(mapa);
    else while (auxI != NULL)
    {
      if(strcmp(item, auxI -> key) == 0)
      {
        if(cont == 0)
        {
          printf("\n~~~~~~~~~~~~~~~~~ JUGADORES CON EL ÍTEM ' %s ' ENCONTRADOS ~~~~~~~~~~~~~~~~~\n\n", item);
        }
        
        printf("- %s\n", jugadorAux -> jugador);
        cont++;
        break;
      }
      auxI = nextMap(jugadorAux -> items);
    }
    aux = nextMap(mapa);
  }
  if (cont == 0) 
  {
    printf("\nNo se han encontrado jugadores con el ítem ' %s '.\n", item);
  }
  puts("\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~o~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  
}

// 7.
void deshacerAccion(HashMap *mapa)
{
  char username[MAX];
  
  preguntarNombreUsuario(username);

  Pair *par = searchMap(mapa, username);

  if(par != NULL)
  {
    Jugador *jugadorAux = par -> value;
    char *datoRegistro = stack_pop(jugadorAux -> registro);
    
    if(strcmp(datoRegistro, "0") == 0)
    {
      printf("\nNo se pueden deshacer más acciones. \n\n");
      stack_push(jugadorAux -> registro, "0");
    }
      
    else if(strcmp(datoRegistro, "3") == 0)
    {
      char *item = stack_pop(jugadorAux -> registroItem);
      eraseMap(jugadorAux -> items, item);
      stack_push(jugadorAux -> registroItem, item);
  
      printf("\nLa última acción fue deshecha y el ítem ' %s ' fue eliminado!\n\n", item);
      jugadorAux -> numeroItems--;
    }
    else if(strcmp(datoRegistro, "4") == 0)
    {
      char *item = stack_pop(jugadorAux -> registroItem);
      insertMap(jugadorAux -> items, item, NULL);
      
      printf("\nLa última acción fue deshecha y el ítem ' %s ' fue agregado!\n\n", item);
      jugadorAux->numeroItems++;
    }
    else if(strcmp(datoRegistro, "5") == 0)
    {
      //usar resgistroSum para restarle lo previamente sumado
      char *auxSum = stack_pop(jugadorAux -> registroSum);
      int numSum = atoi(auxSum);
      jugadorAux -> puntosHabilidad = jugadorAux -> puntosHabilidad - numSum;
      printf("\nLa última acción fue deshecha y al jugador se le disminuyeron ' %i ' puntos!\n\n", numSum);
    }
  }
  else printf("\nEl jugador ' %s ' no se encuentra en el sistema.\n\n", username);
}

// 8.
void importarJugadoresCSV(HashMap *mapa, int *registrado)
{
  char nombreArchivo[MAX];
  
  printf("Ingrese el nombre del archivo a importar:\n");
  scanf("%30s", nombreArchivo);

  FILE *fp = fopen(nombreArchivo, "r");

  if (fp == NULL)
  {
    printf("\nNo hay ningún archivo con ese nombre.\n\n");
    return;
  }
  
  char linea[1024];
  int i, k = 0;

  fgets(linea, 1023, fp);

  while(fgets(linea, 1023, fp) != NULL)
  {
    Jugador *auxJugador = malloc(sizeof(Jugador));
    
    auxJugador -> registro = stack_create();
    auxJugador -> registroSum = stack_create();
    auxJugador -> registroItem = stack_create();
    
    stack_push(auxJugador -> registro, "0");
    
    for(i = 0; i < 4; i++)
    {
      char *aux = gets_csv_field(linea, i);

      if(i == 0)
      {
        strcpy(auxJugador -> jugador, aux);
      }
      if(i == 1)
      {
        int puntosHab = atol(aux);
        auxJugador -> puntosHabilidad = puntosHab;
      }
      if(i == 2)
      {
        int numItems = atol(aux);
        auxJugador -> numeroItems = numItems;
      }
      if(i == 3)
      {
        auxJugador -> items = createMap(auxJugador -> numeroItems);
        for(int j = 0 ; j < auxJugador -> numeroItems ; j++)
        {
          char *item = gets_csv_field(linea, j+3);
          insertMap(auxJugador -> items, strdup(item), strdup(item));
        }
      }
    }
    insertMap(mapa, auxJugador -> jugador, auxJugador);
    k++; 
  }
  
  (*registrado)++;
  
  fclose(fp);
  
  printf("\n%i jugadores han sido importados al sistema!\n\n", k);
}

// 9.
void exportarJugadoresCSV(HashMap *mapa)
{
  char nombreArchivo[MAX];
  
  printf("Ingrese un nombre para el archivo exportado:\n");
  scanf("%30s",nombreArchivo);
  strcat(nombreArchivo,".csv");
 
  FILE *fp = fopen(nombreArchivo, "w");
  Pair *aux = firstMap(mapa);

  fprintf(fp, "Nombre,Puntos de habilidad,#items,Item 1,Item 2,Item 3,Item 4,Item 5,Item 6,Item 7,Item 8\n");
  while(aux != NULL)
  {
    Jugador *jugadorAux = aux -> value;
    Pair *auxI = firstMap(jugadorAux->items);

    fprintf(fp, "%s,%d,", jugadorAux -> jugador, jugadorAux -> puntosHabilidad);
    fprintf(fp, "%d,", jugadorAux -> numeroItems);
    
    if(auxI == NULL) fprintf(fp, "\n");
    else while (auxI != NULL)
    {
     
      fprintf(fp,"%s", auxI -> key);
      auxI = nextMap(jugadorAux -> items);
      if(auxI != NULL) fprintf(fp,",");
    }
    fprintf(fp,"\n");
    aux = nextMap(mapa); 
  }
  
  fclose(fp);
  printf("\n%i Jugadores exportados al archivo ' %s '!\n\n", nombreArchivo, );
}

// Programa principal
int main() 
{
  /*
                      ____ _             _           _             
                     / ___(_) __ _  __ _| |__  _   _| |_ ___     
                    | |  _| |/ _` |/ _` | '_ \| | | | __/ _ \     
                    | |_| | | (_| | (_| | |_) | |_| | ||  __/     
                     \____|_|\__, |\__,_|_.__/ \__, |\__\___| 
                             |___/             |___/                                                        
  */

  HashMap *mapa = createMap(10001);
  
  int user_continue = 1;

  int registrado = 0;

  printf("Bienvenido! :D\n");

  while(user_continue)
  {
    menu();
    
    int opcion = 0;
    
    validarOpcion(&opcion); // Validamos que opción sea un número.
    
    if(registrado == 0 && opcion != 1 && opcion != 8 && opcion != 10) printf("No hay jugadores registrados, registre uno primero.\n");
    else
    {
      switch(opcion)
      {
        case 1 :
          crearPerfil(mapa, &registrado);
          validar(&user_continue);
          break;

        case 2 :
          mostrarPerfil(mapa);
          validar(&user_continue);
          break;

        case 3 :
          agregarItem(mapa);
          validar(&user_continue);
          break;

        case 4 :
          eliminarItem(mapa);
          validar(&user_continue);
          break;

        case 5 :
          agregarPuntosHabilidad(mapa);
          validar(&user_continue);
          break;

        case 6 :
          mostrarJugadoresConItemEsp(mapa);
          validar(&user_continue);
          break;

        case 7 :
          deshacerAccion(mapa);
          validar(&user_continue);
          break;

        case 8 :
          importarJugadoresCSV(mapa, &registrado);
          validar(&user_continue);
          break;

        case 9 : 
          exportarJugadoresCSV(mapa);
          validar(&user_continue);
          break;
        
        case 10 :
          printf("Gracias por usar el programa, adiós!");
          free(mapa);
          return 0;
      }
    }
  }
}