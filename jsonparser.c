#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define LX_EOF 0
#define LX_ERROR -1
#define LX_OPEN_LIST 1
#define LX_CLOSE_LIST 2
#define LX_OPEN_ARRAY 3
#define LX_CLOSE_ARRAY 4
#define LX_COLON 5
#define LX_COMMA 6
#define LX_DQUOTE 7
#define LX_CHAR 8
#define LX_STRING 9

/*********************************
         LEX Part
 *********************************/
struct lex_entry_t {
  int type;
  char *value;
  struct lex_entry_t *next;
};

struct lex_entry_t *create_lex_type(int type, const char *value, size_t len)
{
  struct lex_entry_t *entry;
  if ((entry = malloc(sizeof(struct lex_entry_t))) == NULL) {
    fprintf(stderr, "malloc(%lu) in create_lex_type\n", sizeof(struct lex_entry_t));
    exit(1);
  }
  entry->type = type;
  switch (type) {
  case LX_STRING:
    if ((entry->value = malloc(len + 1)) == NULL) {
      fprintf(stderr, "malloc(%lu) in create_lex_type\n", sizeof(struct lex_entry_t));
      exit(1);
    }
    strncpy(entry->value, value, len);
    entry->value[len] = '\0';
    break;
  case LX_CHAR:
    entry->value = (char *)value;
    break;
  }
  return entry;
}

void dump_lexentry(struct lex_entry_t *entry)
{
  while (entry) {
    switch (entry->type) {
    case LX_STRING:
      printf("Type: STRING(%d), Value: %s\n", entry->type, entry->value);
      break;
    case LX_CHAR:
      printf("Type: CHAR(%d), Value: '%c'\n", entry->type, (char)entry->value);
      break;
    case LX_OPEN_LIST:
      printf("Type: OPENLIST(%d)\n", entry->type);
      break;
    case LX_CLOSE_LIST:
      printf("Type: CLOSELIST(%d)\n", entry->type);
      break;
    case LX_OPEN_ARRAY:
      printf("Type: OPEN_ARRAY(%d)\n", entry->type);
      break;
    case LX_CLOSE_ARRAY:
      printf("Type: CLOSE_ARRAY(%d)\n", entry->type);
      break;
    case LX_COLON:
      printf("Type: COLON(%d)\n", entry->type);
      break;
    case LX_COMMA:
      printf("Type: COMMA(%d)\n", entry->type);
      break;
    default:
      printf("Type: %d\n", entry->type);
      break;
    }
    entry = entry->next;
  }
}


char *lx_value;
char *lx_value_end;

char *strval = NULL;
char *strvalend = NULL;
int strvalsize = 0;

void lxreset()
{
  free(strval);
  strval = strvalend = NULL;
  strvalsize = 0;
}

void strappend(const char bf[])
{
  int i;
  
  if (strvalsize - (strvalend - strval) < 4) {
    if (strval == NULL) {
      if ((strval = malloc(10)) == NULL) {
	fprintf(stderr, "malloc(10) error\n");
	exit(1);
      }
      strvalend = strval;
      strvalsize = 10;
    } else {
      char *p;
      if ((p = realloc(strval, strvalsize + 10)) == NULL) {
	fprintf(stderr, "realloc(0x%p, %d) error\n", strval, strvalsize + 10);
	exit(1);
      }
      strvalend = p + (strvalend - strval);
      strval = p;
      strvalsize += 10;
    }
  }

  for (i = 0; i < 4 && bf[i]; i++, strvalend++)
    *strvalend = bf[i];
}

void getstring(FILE *fp)
{
  int c;
  char bf[4];
  int i;

  i = 0;
  while ((c = fgetc(fp)) != EOF) {
    if (c == '"')
      break;
    bf[i++] = c;
    if (i >= sizeof(bf)) {
      strappend(bf);
      i = 0;
    }
  }
  if (i < sizeof(bf))	/* BF is not filled so need Nul terminate */
    bf[i] = '\0';
  strappend(bf);
}

int gettoken(FILE *fp)
{
  int c;
  
  while ((c = fgetc(fp)) != EOF) {
    if (isspace(c))
      continue;
    switch (c) {
    case '{':
      return LX_OPEN_LIST;
    case '}':
      return LX_CLOSE_LIST;
    case '[':
      return LX_OPEN_ARRAY;
    case ']':
      return LX_CLOSE_ARRAY;
    case ':':
      return LX_COLON;
    case ',':
      return LX_COMMA;
    case '"':
      getstring(fp);
      lx_value = strval;
      lx_value_end = strvalend;
      return LX_STRING;
    default:
      lx_value = (char *)c;
      return LX_CHAR;
    }
  }
  return LX_ERROR;
}

struct lex_entry_t *lxmain(FILE *fp)
{
  int type;
  struct lex_entry_t top, *next;

  next = &top;
  next->next = NULL;
  while ((type = gettoken(fp)) != LX_ERROR) {
    struct lex_entry_t *entry;
    
    entry = create_lex_type(type, lx_value, lx_value_end - lx_value);
    lxreset();
    next->next = entry;
    next = next->next;
    next->next = NULL;
  }

  return top.next;
}

/*********************************
         PARSER Part
 *********************************/

/*****************
 Meanings of VALUE
   Ptr to Array
******************/
struct json_tag_t {
  char *key;
  int type;
  void *value;
};
struct json_list_t {
  struct json_tag_t tag;
  struct json_tag_t *next;
} list;
struct json_array_t {
  void *value;
  size_t cnt;
  size_t size;
} array;

int main(int argc, char *argv[])
{
  FILE *fp;
  struct lex_entry_t top;

  if (argc > 1) {
    if ((fp = fopen(argv[1], "r")) == NULL) {
      perror("fopen");
      exit(1);
    }
  } else
    fp = stdin;

  top.next = lxmain(fp);

  dump_lexentry(top.next);
}
