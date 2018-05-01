#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>

/* The maximum amount of bytes for a file name */
#define MAX_FILE_NAME_SIZE 255

/* The maximum amount of bytes for each I/O operation */
#define MAX_IO_BUFFER_SIZE 1024

// Linked list node to keep track of all subprocesses
typedef struct pid_node {
    pid_t pid;
    struct pid_node* next;
} pid_node_t;

// Linked list node used in reading/writing output txt files
typedef struct vote_node {
	char* name;
	int count;
	struct vote_node* next;
} vote_node_t;

// Gets start of string following last slash which has non-slash chars to the right
// If no slashes are in string, or if all slashes appear at end, return original string
char* get_current_dir_name(char* path) {
    char* result = strrchr(path, '/');
    while (result != NULL && result[1] == '\0') {
        // Trim trailing slashes
        *result = '\0';
        result = strrchr(path, '/');
    }
    if (result == NULL) {
        result = path;
    }
    while (result != NULL && *result == '/') {
        // Trim leading slashes
        result += 1;
    }
    return result;
}

/**********************************
* 
* Taken from Unix Systems Programming, Robbins & Robbins, p37
* 
*********************************/
int makeargv(const char *s, const char *delimiters, char ***argvp) {
   int error;
   int i;
   int numtokens;
   const char *snew;
   char *t;

   if ((s == NULL) || (delimiters == NULL) || (argvp == NULL)) {
      errno = EINVAL;
      return -1;
   }
   *argvp = NULL;
   snew = s + strspn(s, delimiters);
   if ((t = malloc(strlen(snew) + 1)) == NULL)
      return -1;
   strcpy(t,snew);
   numtokens = 0;
   if (strtok(t, delimiters) != NULL)
      for (numtokens = 1; strtok(NULL, delimiters) != NULL; numtokens++) ;

   if ((*argvp = malloc((numtokens + 1)*sizeof(char *))) == NULL) {
      error = errno;
      free(t);
      errno = error;
      return -1;
   }

   if (numtokens == 0)
      free(t);
   else {
      strcpy(t,snew);
      **argvp = strtok(t,delimiters);
      for (i=1; i<numtokens; i++)
         *((*argvp) +i) = strtok(NULL,delimiters);
   }
   *((*argvp) + numtokens) = NULL;
   return numtokens;
}

/**********************************
* 
* Taken from Unix Systems Programming, Robbins & Robbins, p38
* 
*********************************/
void freemakeargv(char **argv) {
   if (argv == NULL)
      return;
   if (*argv != NULL)
      free(*argv);
   free(argv);
}

char *trimwhitespace(char *str) {
  char *end;
  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  // All spaces?
    return str;

  // Trim trailing space
  end = str + strlen(str) - 1;

  while(end > str && isspace((unsigned char)*end)) end--;

  // Write new null terminator
  *(end+1) = 0;

  return str;
}
