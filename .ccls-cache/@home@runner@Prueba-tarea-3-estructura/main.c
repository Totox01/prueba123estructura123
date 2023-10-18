#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <math.h>
#include "list.h"
#include "hashmap.h"
#include "heap.h"

typedef struct {
  int id;
  char * title;
  int words;
  int chars;
  HashMap * word_map;
} Book;

typedef struct {
  char * text;
  int freq;
  double rel;
  List * pos_list;
} Word;

Book * read_book(char * filename) {
  Book * book = (Book *) malloc(sizeof(Book));
  FILE * file = fopen(filename, "r");
  if (file == NULL) {
    printf("Error al abrir el archivo %s\n", filename);
    return NULL;
  }
  char line[256];
  fgets(line, 256, file);
  sscanf(line, "%d", &book->id);
  fgets(line, 256, file);
  book->title = strdup(line);
  strtok(book->title, "\n");
  book->words = 0;
  book->chars = 0;
  book->word_map = createMap(1000);
  long pos = ftell(file);
  while (fgets(line, 256, file) != NULL) {
    char * token = strtok(line, " ");
    while (token != NULL) {
      book->words++;
      book->chars += strlen(token);
      token = strtok(token, "\n.,;:!?\"");
      token = strlwr(token);
      Word * word = (Word *) searchMap(book->word_map, token);
      if (word == NULL) {
        word = (Word *) malloc(sizeof(Word));
        word->text = strdup(token);
        word->freq = 0;
        word->rel = 0.0;
        word->pos_list = createList();
        insertMap(book->word_map, token, word);
      }
      word->freq++;
      pushBack(word->pos_list, &pos);
      pos = ftell(file);
      token = strtok(NULL, " ");
    }
  }
  fclose(file);
  return book;
}

void index_book(Book * book, HashMap * global_map) {
  HashMap * doc_count = createMap(1000);
  Word * word = (Word *) firstMap(book->word_map);
  while (word != NULL) {
    List * book_list = (List *) searchMap(global_map, word->text);
    if (book_list == NULL) {
      book_list = createList();
      insertMap(global_map, word->text, book_list);
    }
    pushBack(book_list, book);
    int * count = (int *) searchMap(doc_count, word->text);
    if (count == NULL) {
      count = (int *) malloc(sizeof(int));
      *count = 0;
      insertMap(doc_count, word->text, count);
    }
    (*count)++;
    word = (Word *) nextMap(book->word_map);
  }
  word = (Word *) firstMap(book->word_map);
  while (word != NULL) {
    int * count = (int *) searchMap(doc_count, word->text);
    word->rel = ((double) word->freq / book->words) * log((double) global_map->size / *count);
    word = (Word *) nextMap(book->word_map);
  }
}

HashMap * load_books(char * input) {
  HashMap * book_map = createMap(1000);
  HashMap * global_map = createMap(10000);
  char * name = strdup(input);
  strtok(name, " ");
  if (strstr(name, ".txt") != NULL) {
    Book * book = read_book(name);
    if (book != NULL) {
      insertMap(book_map, &book->id, book);
      index_book(book, global_map);
    }
  } else {
    DIR * dir = opendir(name);
    if (dir == NULL) {
      printf("Error al abrir el directorio %s\n", name);
      return NULL;
    }
    struct dirent * entry = readdir(dir);
    while (entry != NULL) {
      if (entry->d_type == DT_REG && strstr(entry->d_name, ".txt") != NULL) {
        char * path = (char *) malloc(strlen(name) + strlen(entry->d_name) + 2);
        sprintf(path, "%s/%s", name, entry->d_name);
        Book * book = read_book(path);
        if (book != NULL) {
          insertMap(book_map, &book->id, book);
          index_book(book, global_map);
        }
        free(path);
      }
      entry = readdir(dir);
    }
    closedir(dir);
  }
  free(name);
  return book_map;
}

void show_books(HashMap * book_map) {
  Heap * book_heap = createHeap();
  Book * book = (Book *) firstMap(book_map);
  while (book != NULL) {
    heap_push(book_heap, book, book->title);
    book = (Book *) nextMap(book_map);
  }
  while (!heap_empty(book_heap)) {
    book = (Book *) heap_top(book_heap);
    printf("Id: %d\n", book->id);
    printf("Título: %s\n", book->title);
    printf("Palabras: %d\n", book->words);
    printf("Caracteres: %d\n", book->chars);
    printf("\n");
    heap_pop(book_heap);
  }
  free(book_heap);
}

void search_by_title(char * input, HashMap * global_map) {
  Heap * book_heap = createHeap();
  int words = 0;
  char * token = strtok(input, " ");
  while (token != NULL) {
    words++;
    List * book_list = (List *) searchMap(global_map, token);
    if (book_list != NULL) {
      Book * book = (Book *) firstList(book_list);
      while (book != NULL) {
        int match = 1;
        char * title = strdup(book->title);
        char * word = strtok(input, " ");
        while (word != NULL) {
          if (strstr(title, word) == NULL) {
            match = 0;
            break;
          }
          word = strtok(NULL, " ");
        }
        free(title);
          heap_push(book_heap, book, book->title);
        }
        book = (Book *) nextList(book_list);
      }
    }
    token = strtok(NULL, " ");
  }
  printf("Se encontraron %d libros que coinciden con las %d palabras ingresadas:\n", heap_size(book_heap), words);
  while (!heap_empty(book_heap)) {
    Book * book = (Book *) heap_top(book_heap);
    printf("Id: %d\n", book->id);
    printf("Título: %s\n", book->title);
    printf("\n");
    heap_pop(book_heap);
  }
  free(book_heap);
}

void show_most_frequent_words(int id, HashMap * book_map) {
  Book * book = (Book *) searchMap(book_map, &id);
  if (book == NULL) {
    printf("No se encontró ningún libro con ese id\n");
    return;
  }
  Heap * word_heap = createHeap();
  Word * word = (Word *) firstMap(book->word_map);
  while (word != NULL) {
    heap_push(word_heap, word, &word->freq);
    word = (Word *) nextMap(book->word_map);
  }
  printf("Las 10 palabras que se repiten con mayor frecuencia en el contenido del libro %s son:\n", book->title);
  int count = 0;
  while (!heap_empty(word_heap) && count < 10) {
    word = (Word *) heap_top(word_heap);
    printf("%s: %d\n", word->text, word->freq);
    count++;
    heap_pop(word_heap);
  }
  free(word_heap);
}

void show_most_relevant_words(char * title, HashMap * global_map) {
  Heap * word_heap = createHeap();
  List * book_list = (List *) searchMap(global_map, title);
  if (book_list == NULL) {
    printf("No se encontró ningún libro con ese título\n");
    return;
  }
  Book * book = (Book *) firstList(book_list);
  Word * word = (Word *) firstMap(book->word_map);
  while (word != NULL) {
    heap_push(word_heap, word, &word->rel);
    word = (Word *) nextMap(book->word_map);
  }
  printf("Las 10 palabras más relevantes del libro %s son:\n", book->title);
  int count = 0;
  while (!heap_empty(word_heap) && count < 10) {
    word = (Word *) heap_top(word_heap);
    printf("%s: %.2f\n", word->text, word->rel);
    count++;
    heap_pop(word_heap);
  }
  free(word_heap);
}

void search_by_word(char * input, HashMap * global_map) {
  Heap * book_heap = createHeap();
  char * token = strtok(input, " ");
  if (token == NULL) {
    printf("No se ingresó ninguna palabra\n");
    return;
  }
  List * book_list = (List *) searchMap(global_map, token);
  if (book_list == NULL) {
    printf("No se encontró ningún libro que contenga la palabra %s\n", token);
    return;
  }
  Book * book = (Book *) firstList(book_list);
  while (book != NULL) {
    Word * word = (Word *) searchMap(book->word_map, token);
    heap_push(book_heap, book, &word->rel);
    book = (Book *) nextList(book_list);
  }
  printf("Los libros que contienen la palabra %s en su contenido son:\n", token);
  while (!heap_empty(book_heap)) {
    book = (Book *) heap_top(book_heap);
    printf("Id: %d\n", book->id);
    printf("Título: %s\n", book->title);
    printf("\n");
    heap_pop(book_heap);
  }
  free(book_heap);
}

void show_word_context(char * title, char * word, HashMap * global_map) {
  List * book_list = (List *) searchMap(global_map, title);
  if (book_list == NULL) {
    printf("No se encontró ningún libro con ese título\n");
    return;
  }
  Book * book = (Book *) firstList(book_list);
  Word * word = (Word *) searchMap(book->word_map, word);
  if (word == NULL) {
    printf("No se encontró la palabra %s en el libro %s\n", word, title);
    return;
  }
  printf("Las apariciones de la palabra %s en el contexto del libro %s son:\n", word, title);
  char * filename = (char *) malloc(strlen(title) + 5);
  sprintf(filename, "%d.txt", book->id);
  FILE * file = fopen(filename, "r");
  if (file == NULL) {
    printf("Error al abrir el archivo %s\n", filename);
    return;
  }
  long * pos = (long *) firstList(word->pos_list);
  while (pos != NULL) {
    fseek(file, *pos, SEEK_SET);
    char line[256];
    fgets(line, 256, file);
    printf("%s\n", line);
    pos = (long *) nextList(word->pos_list);
  }
  fclose(file);
}

void show_menu(HashMap * book_map, HashMap * global_map) {
  int option = 0;
  do {
    printf("Menú de opciones:\n");
    printf("1. Cargar documentos\n");
    printf("2. Mostrar documentos ordenados\n");
    printf("3. Buscar un libro por título\n");
    printf("4. Palabras con mayor frecuencia\n");
    printf("5. Palabras más relevantes\n");
    printf("6. Buscar por palabra\n");
    printf("7. Mostrar palabra en su contexto dentro del libro\n");
    printf("8. Salir\n");
    printf("Ingrese el número de la opción que desea realizar:\n");
    scanf("%d", &option);
    switch (option) {
      case 1:
        char input[256];
        printf("Ingrese el nombre de un archivo txt o una carpeta con archivos txt:\n");
        fgets(input, 256, stdin);
        book_map = load_books(input);
        break;
      case 2:
        show_books(book_map);
        break;
      case 3:
        char title[256];
        printf("Ingrese algunas palabras separadas por espacio para buscar un libro por título:\n");
        fgets(title, 256, stdin);
        search_by_title(title, global_map);
        break;
      case 4:
        int id;
        printf("Ingrese el id de un libro para mostrar las palabras con mayor frecuencia:\n");
        scanf("%d", &id);
        show_most_frequent_words(id, book_map);
        break;
      case 5:
        char title[256];
        printf("Ingrese el título de un libro para mostrar las palabras más relevantes:\n");
        fgets(title, 256, stdin);
        show_most_relevant_words(title, global_map);
        break;
      case 6:
        char word[256];
        printf("Ingrese una palabra para buscar los libros que la contienen:\n");
        fgets(word, 256, stdin);
        search_by_word(word, global_map);
        break;
      case 7:
        char title[256];
        char word[256];
        printf("Ingrese el título de un libro y una palabra a buscar separados por espacio:\n");
        scanf("%s %s", title, word);
        show_word_context(title, word, global_map);
        break;
      case 8:
        printf("Gracias por usar la aplicación. Hasta pronto.\n");
        break;
      default:
        printf("Opción inválida. Intente nuevamente.\n");
        break;
    }
  } while (option != 8);
}


int main() {
  char input[256];
  printf("Ingrese el nombre de un archivo txt o una carpeta con archivos txt:\n");
  fgets(input, 256, stdin);
  HashMap * book_map = load_books(input);
  if (book_map != NULL) {
    show_menu(book_map, global_map); 
    } else {
      printf("No se pudo cargar ningún libro\n");
    }
  return 0;
}