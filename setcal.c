#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_LINE_SIZE 30

// UTILS
// result has to be initialized with proper size and zeros as default (or other joker char), splitted strings will then replace the joker chars
void my_split(char line[], char separator, char **result, int line_length, int *result_len);

//VALIDATORS
// Returns 1 if input is correct, otherwise 0
int validate_user_input(int argc, char const *argv[]);

// MAIN FUNCTIONS
// returns 1 if is end of file, else returns 0
int readline(FILE *file, char *line);
// creates array of lines from file
void process_file(char filename[], char **result);
void process_rows(char *lines[]);
void process_operation(char command_name[], char *lines[], int line1, int line2);

//mnoziny declaration
int empty_com(int line1, char *lines[]);
int card_com(int line1, char *lines[]);
int complement_com(int line1, char *lines[]);
int union_com(int line1, int line2, char *lines[]);
int intersect_com(int line1, int line2, char *lines[]);
int minus_com(int line1, int line2, char *lines[]);
int subseteq_com(int line1, int line2, char *lines[]);
int subset_com(int line1, int line2, char *lines[]);
int equals_com(int line1, int line2, char *lines[]);

//relace declaration
int reflexive_com(int line1, char *lines[]);
int symmetric_com(int line1, char *lines[]);
int antisymmetric_com(int line1, char *lines[]);
int transitive_com(int line1, char *lines[]);
int function_com(int line1, char *lines[]);
int domain_com(int line1, char *lines[]);
int codomain_com(int line1, char *lines[]);
int injective_com(int line1, char *lines[]);
int surjective_com(int line1, char *lines[]);
int bijective_com(int line1, char *lines[]);

//STRUCTS
//struct for Sets
typedef struct {
    unsigned size;
    int row;
    char **items;
} Set;

//struct for Array of Sets
typedef struct {
    unsigned len;
    Set *data;
} SetArray;


int main(int argc, char const *argv[])
{
    char *lines[1000];
    char filename[10] = "file.txt";
    process_file(filename, lines);
    process_rows(lines);
    return 0;
}

// UTILS
// result has to be initialized with proper size and zeros as default (or other joker char), splitted strings will then replace the joker chars
void my_split(char line[], char separator, char **result, int line_length, int *result_len)
{
    int subst_start = 0;  // where should creation of the substring start
    int substs_count = 0; // how many substrings I have (index in result)
    char *substr;

    substr = (char *)malloc(1); // dummy malloc
    for (int i = 0; i <= line_length; i++)
    {
        if ((line[i] == separator) || (line[i] == '\0'))
        {
            substr = (char *)realloc(substr, (i - subst_start) + 1);      //reallocing dummy with size of string +1 for 0 char
            strncpy(substr, &line[subst_start], i - subst_start);         // giving right value to substring
            substr[i - subst_start] = '\0';                               // adding 0 to end of substring
            result[substs_count] = (char *)malloc((i - subst_start) + 1); // allocating result size
            strncpy(result[substs_count], substr, (i - subst_start) + 1); // giving right value to result
            subst_start = i + 1;
            substs_count++;
            //printf("%d\n", substs_count);
            //result = (char **)realloc(result, (substs_count + 1) * sizeof *result);
        }
    }
    *result_len = substs_count; //withdrawing length of array - needed in Set_ctor etc.
    free(substr);
}

void set_ctor(Set *s, char **items, int row, int items_len){
    s->row = row;
    for (int i = 1; i < items_len; i++){
        strcpy(s->items[i], items[i]);
        s->size = i;
    }
}
//VALIDATORS
// Returns 1 if input is correct, otherwise 0
int validate_user_input(int argc, char const *argv[])
{
    if (argc > 2)
    {
        fprintf(stderr, "Prilis mnoho zadanych argumentu!\n");
        return 0;
    }
    if (argc < 2)
    {
        fprintf(stderr, "Prilis malo zadanych argumentu!\n");
        return 0;
    }
    char buffer[4];
    for (int i = 4; i > 0; i--)
    {
        buffer[4 - i] = argv[1][strlen(argv[1]) - i];
    }
    if (!strcmp(buffer, ".txt") && strlen(argv[1]) > 4)
        return 1;
    fprintf(stderr, "Spatne zadany argument!\n");
    return 0;
}

// MAIN FUNCTIONS
// returns 1 if is end of file, else returns 0
int readline(FILE *file, char *line)
{
    int output_length = DEFAULT_LINE_SIZE;
    char ch = getc(file);
    int c_index = 0;
    while ((ch != '\n') && (ch != EOF))
    {
        if (c_index == output_length - 1)
        {
            output_length *= 2;
            line = (char *)realloc(line, output_length);
        }
        line[c_index] = ch;

        c_index++;
        ch = getc(file);
    }
    line[c_index] = '\0';

    if (ch == '\n')
    {
        return 0;
    }
    return 1;
}

// creates array of lines from file
void process_file(char filename[], char **result)
{
    FILE *file;
    file = fopen(filename, "r");
    for (int i = 0; i < 1000; i++)
    {
        result[i] = (char *)malloc(DEFAULT_LINE_SIZE);
        if (readline(file, result[i]))
        {
            break;
        }
    }
}

void process_rows(char *lines[])
{
    for (int i = 0; i < 1000; i++)
    { //předávání toho řádku musím posunout o 1
        if (lines[i][0] == 'C')
        {
            char **output = (char **)malloc(4 * sizeof (char *));
            int items_len = 0;
            my_split(lines[i], ' ', output, strlen(lines[i]), &items_len);
            for (int i = 0; i < items_len; i++){
                printf("for: %s\n", output[i]);
            }
            printf("%d\n", items_len);
            if (items_len == 3) output[3] = "_";
            printf("%s\n", output[3]);
            process_operation(output[1], lines, atoi(output[2]), atoi(output[3]));
            
            for (int i = 0; i < items_len; i++)
                {
                    free(output[i]);
                }
            free(output);
        }
        
        //else if (lines[i][0] == 'S')
        else
        {

            printf("%s\n", lines[i]);
        }
    }
}

// execute proper function when line contains command
void process_operation(char command_name[], char *lines[], int line1, int line2)
{
    //mnoziny
    if (!strcmp(command_name, "empty"))
    {
        empty_com(line1, lines);
    }
    else if (!strcmp(command_name, "card"))
    {
        card_com(line1, lines);
    }
    else if (!strcmp(command_name, "complement"))
    {
        complement_com(line1, lines);
    }
    else if (!strcmp(command_name, "union"))
    {
        union_com(line1, line2, lines);
    }
    else if (!strcmp(command_name, "intersect"))
    {
        intersect_com(line1, line2, lines);
    }
    else if (!strcmp(command_name, "minus"))
    {
        minus_com(line1, line2, lines);
    }
    else if (!strcmp(command_name, "subseteq"))
    {
        subseteq_com(line1, line2, lines);
    }
    else if (!strcmp(command_name, "subset"))
    {
        subset_com(line1, line2, lines);
    }
    else if (!strcmp(command_name, "equals"))
    {
        equals_com(line1, line2, lines);
    }
    //relacie
    else if (!strcmp(command_name, "reflexive"))
    {
        reflexive_com(line1, lines);
    }
    else if (!strcmp(command_name, "symmetric"))
    {
        symmetric_com(line1, lines);
    }
    else if (!strcmp(command_name, "antisymmetric"))
    {
        antisymmetric_com(line1, lines);
    }
    else if (!strcmp(command_name, "transitive"))
    {
        transitive_com(line1, lines);
    }
    else if (!strcmp(command_name, "function"))
    {
        function_com(line1, lines);
    }
    else if (!strcmp(command_name, "domain"))
    {
        domain_com(line1, lines);
    }
    else if (!strcmp(command_name, "codomain"))
    {
        codomain_com(line1, lines);
    }
    else if (!strcmp(command_name, "injective"))
    {
        injective_com(line1, lines);
    }
    else if (!strcmp(command_name, "surjective"))
    {
        surjective_com(line1, lines);
    }
    else if (!strcmp(command_name, "bijective"))
    {
        bijective_com(line1, lines);
    }
    else
    {
        fprintf(stderr, "Chyba vstupu, spatny nazev fce\n"); // TODO - ukoncit program protoze spatny vstup
    }
}

int empty_com(int line1, char *lines[])
{
    printf("empty\n");
    return 0;
}
int card_com(int line1, char *lines[])
{
    printf("card\n");
    return 0;
}
int complement_com(int line1, char *lines[])
{
    printf("complement\n");
    return 0;
}
int union_com(int line1, int line2, char *lines[])
{
    printf("union\n");
    return 0;
}
int intersect_com(int line1, int line2, char *lines[])
{
    printf("intersect\n");
    return 0;
}
int minus_com(int line1, int line2, char *lines[])
{
    printf("minus\n");
    return 0;
}
int subseteq_com(int line1, int line2, char *lines[])
{
    printf("subseteq\n");
    return 0;
}
int subset_com(int line1, int line2, char *lines[])
{
    printf("subset\n");
    return 0;
}
int equals_com(int line1, int line2, char *lines[])
{
    printf("equals\n");
    return 0;
}

int reflexive_com(int line1, char *lines[])
{
    printf("reflexive\n");
    return 0;
}
int symmetric_com(int line1, char *lines[])
{
    printf("symetric\n");
    return 0;
}
int antisymmetric_com(int line1, char *lines[])
{
    printf("antisymmetric\n");
    return 0;
}
int transitive_com(int line1, char *lines[])
{
    printf("transitive\n");
    return 0;
}
int function_com(int line1, char *lines[])
{
    printf("function\n");
    return 0;
}
int domain_com(int line1, char *lines[])
{
    printf("domain\n");
    return 0;
}
int codomain_com(int line1, char *lines[])
{
    printf("codomain\n");
    return 0;
}
int injective_com(int line1, char *lines[])
{
    printf("injective\n");
    return 0;
}
int surjective_com(int line1, char *lines[])
{
    printf("surjective\n");
    return 0;
}
int bijective_com(int line1, char *lines[])
{
    printf("bijective\n");
    return 0;
}
