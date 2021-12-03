#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//_______DEFINITIONS__________________________________________________________________________________
#define DEFAULT_LINE_SIZE 30

//------STRUCTS-FOR-SETS------------------------------------------------------------------------------
typedef struct
{
    int size;
    int row;
    char **items;
} Set;

//struct for Array of Sets
typedef struct
{
    int len;
    Set *data;
} SetArray;

//------STRUCRS-FOR-SESSIONS-----------------------------------------------------------------------------
typedef struct
{
    char *left_val;
    char *right_val;
} Session_pair;

typedef struct
{
    int size;
    int row;
    Session_pair *pairs;
} Session;

//______FUNCTION_HEADERS_________________________________________________________________________________
//------UTILS--------------------------------------------------------------------------------------------
// TODO
char **my_split(char string[], char separator, int line_length, int *result_len);
// returns 1 if is end of file, else returns 0
int readline(FILE *file, char *line);
// creates array of lines from file, returns number of lines
int process_file(char filename[], char **result);
// TODO
void process_rows(char *lines_array[], int line_count);
// TODO
void process_operation(char command_name[], char *lines_array[], int first_line_num, int second_line_num);
void remove_all_chars(char *str, char c);
//------VALIDATORS----------------------------------------------------------------------------------------
// Returns 1 if input is correct, otherwise 0
int validate_user_input(int argc, char const *argv[]);

//------SET-HELPERS------
void set_ctor_from_line_string(Set *set, char *string, int row);
void set_ctor(Set *set, char **items, int row, int items_len);
void set_print_from_array(char **items, int items_count);
void set_dtor(Set *set);
void setarray_ctor(SetArray *setar);
void *setarray_inc(SetArray *setar);
void *set_copy(Set *src, Set *dst);
void *set_array_append(SetArray *setar, Set *s);

//------SET-COMMANDS------
int is_empty_com(int first_line_num, char *lines_array[]);
int card_com(int first_line_num, char *lines_array[]);
int complement_com(int first_line_num, char *lines_array[]);
int union_com(int first_line_num, int second_line_num, char *lines_array[]);
int intersect_com(int first_line_num, int second_line_num, char *lines_array[]);
int minus_com(int first_line_num, int second_line_num, char *lines_array[]);
int subseteq_com(int first_line_num, int second_line_num, char *lines_array[]);
int subset_com(int first_line_num, int second_line_num, char *lines_array[]);
int equals_com(int first_line_num, int second_line_num, char *lines_array[]);

//------SESSION-HELPERS-------
void session_init(Session *session, int row);
void session_append(Session *session, Session_pair *pair);
void session_pair_ctor(Session_pair *pair, char *left_val, char *right_val);
void session_pair_dtor(Session_pair *pair);
void session_ctor_from_line_string(Session *session, char *string, int row);
void session_ctor(Session *session, Session_pair *pairs, int row, int size);
void session_print(Session session);
void session_dtor(Session *session);

//------SESSION-COMMANDS------
int reflexive_com(int first_line_num, char *lines_array[]);
int symmetric_com(int first_line_num, char *lines_array[]);
int antisymmetric_com(int first_line_num, char *lines_array[]);
int transitive_com(int first_line_num, char *lines_array[]);
int function_com(int first_line_num, char *lines_array[]);
int domain_com(int first_line_num, char *lines_array[]);
int codomain_com(int first_line_num, char *lines_array[]);
int injective_com(int first_line_num, char *lines_array[]);
int surjective_com(int first_line_num, char *lines_array[]);
int bijective_com(int first_line_num, char *lines_array[]);

//_______________MAIN_CODE____________________________________________________________________________________

int main(int argc, char const *argv[])
{

    char *lines_array[1000];
    char filename[10] = "file.txt";
    int line_count = process_file(filename, lines_array);
    process_rows(lines_array, line_count);

    printf("___TESTING___\n");
    Session s;
    Session s2;
    Session s3;
    Session s4;
    char str[26] = "R (a b) (c d) (e f) (g h)";
    char str2[200] = "R (grunm brum) (aaa hovno) (bysek kokot) (ahoj igor)";
    char str3[200] = "R (1111 2222) (4444 5555) (6666 7777) (8888 9999)";
    char str4[200] = "R (ff cc) (ss yy) (kk aa) (ww ll)";

    session_ctor_from_line_string(&s, str, 5);
    session_ctor_from_line_string(&s2, str2, 1);
    session_ctor_from_line_string(&s3, str3, 1);
    session_ctor_from_line_string(&s4, str4, 1);
    printf("%s %s \n", s.pairs[2].left_val, s.pairs[2].right_val);
    session_print(s);
    session_print(s2);
    session_print(s3);
    session_print(s4);
    return 0;
}

//-------UTILS-------------------------------------------------------------------------------------------------
// returns result, it is fully dynamic allocated
char **my_split(char string[], char separator, int line_length, int *result_len)
{
    int subst_start = 0;  // where should creation of the substring start
    int substs_count = 0; // how many substrings I have (index in result)
    char *substr;
    char **result = malloc(sizeof(char *));
    substr = (char *)malloc(1); // dummy malloc
    for (int i = 0; i <= line_length; i++)
    {
        if ((string[i] == separator) || (string[i] == '\0'))
        {
            substr = (char *)realloc(substr, (i - subst_start) + 1);      // reallocing dummy with size of string +1 for \0 char
            strncpy(substr, &string[subst_start], i - subst_start);       // giving right value to substring
            substr[i - subst_start] = '\0';                               // adding 0 to end of substring
            result[substs_count] = (char *)malloc((i - subst_start) + 1); // allocating result size
            strncpy(result[substs_count], substr, (i - subst_start) + 1); // giving right value to result
            subst_start = i + 1;
            substs_count++;
            result = realloc(result, sizeof(char *) * (substs_count + 1)); // increase space of result
        }
    }
    *result_len = substs_count; //withdrawing length of array
    free(substr);
    return result;
}
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

// creates array of lines from file, returns line count
int process_file(char filename[], char **result)
{
    FILE *file;
    int line_count;
    file = fopen(filename, "r");
    for (int i = 0; i < 1000; i++)
    {
        result[i] = (char *)malloc(DEFAULT_LINE_SIZE);
        if (readline(file, result[i]))
        {
            line_count = i+1;
            break;
        }
    }
    return line_count;
}

void process_rows(char *lines_array[], int line_count)
{
    for (int i = 0; i < line_count; i++)
    {
        if (lines_array[i][0] == 'C')
        {
            int items_len = 0;
            char **output_com = my_split(lines_array[i], ' ', strlen(lines_array[i]), &items_len);
            for (int i = 0; i < items_len; i++)
                if (items_len == 3)
                    output_com[3] = "_";
            process_operation(output_com[1], lines_array, atoi(output_com[2]), atoi(output_com[3]));

            for (int i = 0; i < items_len; i++)
            {
                free(output_com[i]);
            }
            free(output_com);
        }
        else
        {
            printf("%s\n", lines_array[i]);
        }
    }
}

// execute proper function when line contains command
void process_operation(char command_name[], char *lines_array[], int first_line_num, int second_line_num)
{
    //SETS
    if (!strcmp(command_name, "empty"))
    {
        is_empty_com(first_line_num, lines_array);
    }
    else if (!strcmp(command_name, "card"))
    {
        card_com(first_line_num, lines_array);
    }
    else if (!strcmp(command_name, "complement"))
    {
        complement_com(first_line_num, lines_array);
    }
    else if (!strcmp(command_name, "union"))
    {
        union_com(first_line_num, second_line_num, lines_array);
    }
    else if (!strcmp(command_name, "intersect"))
    {
        intersect_com(first_line_num, second_line_num, lines_array);
    }
    else if (!strcmp(command_name, "minus"))
    {
        minus_com(first_line_num, second_line_num, lines_array);
    }
    else if (!strcmp(command_name, "subseteq"))
    {
        subseteq_com(first_line_num, second_line_num, lines_array);
    }
    else if (!strcmp(command_name, "subset"))
    {
        subset_com(first_line_num, second_line_num, lines_array);
    }
    else if (!strcmp(command_name, "equals"))
    {
        equals_com(first_line_num, second_line_num, lines_array);
    }
    //SESSIONS
    else if (!strcmp(command_name, "reflexive"))
    {
        reflexive_com(first_line_num, lines_array);
    }
    else if (!strcmp(command_name, "symmetric"))
    {
        symmetric_com(first_line_num, lines_array);
    }
    else if (!strcmp(command_name, "antisymmetric"))
    {
        antisymmetric_com(first_line_num, lines_array);
    }
    else if (!strcmp(command_name, "transitive"))
    {
        transitive_com(first_line_num, lines_array);
    }
    else if (!strcmp(command_name, "function"))
    {
        function_com(first_line_num, lines_array);
    }
    else if (!strcmp(command_name, "domain"))
    {
        domain_com(first_line_num, lines_array);
    }
    else if (!strcmp(command_name, "codomain"))
    {
        codomain_com(first_line_num, lines_array);
    }
    else if (!strcmp(command_name, "injective"))
    {
        injective_com(first_line_num, lines_array);
    }
    else if (!strcmp(command_name, "surjective"))
    {
        surjective_com(first_line_num, lines_array);
    }
    else if (!strcmp(command_name, "bijective"))
    {
        bijective_com(first_line_num, lines_array);
    }
    else
    {
        fprintf(stderr, "Chyba vstupu, spatny nazev fce\n"); // TODO - ukoncit program protoze spatny vstup
    }
}
void remove_all_chars(char *str, char c)
{
    char *pr = str, *pw = str;
    while (*pr)
    {
        *pw = *pr++;
        pw += (*pw != c);
    }
    *pw = '\0';
}

//------VALIDATORS------
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
// Validates lines from file
bool validate_lines(char lines_array[])
{
    // array of banned strings
    char *banned_strings[19] = {"complement", "union", "intersect", "minus", "subseteq", "subset", "equals", "reflexive", "symmetric", "antisymmetric", "transitive", "function", "domain", "codomain", "injective", "surjective", "bijective", "true", "false"};
    int num_items = 0;
    // converts line to array
    char **line_arr = my_split(lines_array, ' ', strlen(lines_array), &num_items);
    // UNIVERSE AND SET VALIDATIONS
    if (strcmp(line_arr[0], "U") == 0 || strcmp(line_arr[0], "S") == 0)
    {
        for (int i = 1; i < num_items; i++)
        {
            for (int j = 0; j < 19; j++)
            {
                // comparison with banned strings
                if (strcmp(line_arr[i], banned_strings[j]) == 0)
                {
                    fprintf(stderr, "Prvky se nesmi jmenovat stejne jako prikazy!\n");
                    return false;
                }
            }
            // char type validation
            for (int j = 0; j < strlen(line_arr[i]); j++)
            {
                if (!((line_arr[i][j] >= 'a' && line_arr[i][j] <= 'z') || (line_arr[i][j] >= 'A' && line_arr[i][j] <= 'Z')))
                {
                    fprintf(stderr, "Prvky musi byt retezce pouze z malych nebo velkych pismen abecedy!\n");
                    return false;
                }
            }
            // checks if there are no same values
            for (int j = 1; j < num_items; j++)
            {
                if (i != j)
                {
                    if (strcmp(line_arr[i], line_arr[j]) == 0)
                    {
                        if (*line_arr[0] == 'U')
                        {
                            fprintf(stderr, "Vsechny prvky universa musi byt ruzne!\n");
                            return false;
                        }
                        if (*line_arr[0] == 'S')
                        {
                            fprintf(stderr, "Vsechny prvky mnoziny musi byt ruzne!\n");
                            return false;
                        }
                    }
                }
            }
        }
    }
    // COMMAND VALIDATIONS
    else if (strcmp(line_arr[0], "C") == 0)
    {
        // checks if has accurate number of items
        if (num_items > 4 || num_items < 3)
        {
            fprintf(stderr, "Spatne zadany prikaz!\n");
            return false;
        }
        // validation of correctly written command
        bool is_cmd = false;
        for (int i = 0; i < 17; i++)
        {
            if (strcmp(line_arr[1], banned_strings[i]) == 0)
            {
                is_cmd = true;
                break;
            }
        }
        if (is_cmd == false)
        {
            fprintf(stderr, "Spatne zadany prikaz!\n");
            return false;
        }
        // one line number is necessary after command
        if (num_items == 3)
        {
            for (int i = 0; i < strlen(line_arr[2]); i++)
            {
                if (!(line_arr[2][i] >= '0' && line_arr[2][i] <= '9'))
                {
                    fprintf(stderr, "Spatne zadany prikaz!\n");
                    return false;
                }
            }
        }
        // two line numbers are necessary after command
        if (num_items == 4)
        {
            for (int i = 0; i < strlen(line_arr[2]); i++)
            {
                if (!(line_arr[2][i] >= '0' && line_arr[2][i] <= '9'))
                {
                    fprintf(stderr, "Spatne zadany prikaz!\n");
                    return false;
                }
            }
            for (int i = 0; i < strlen(line_arr[3]); i++)
            {
                if (!(line_arr[3][i] >= '0' && line_arr[3][i] <= '9'))
                {
                    fprintf(stderr, "Spatne zadany prikaz!\n");
                    return false;
                }
            }
        }
    }
    // SESSIONS VALIDATION
    else if (strcmp(line_arr[0], "R") == 0)
    {
        // parenthesis removal
        int move = 0;
        for (int i = 1; i < num_items; i++)
        {
            for (int j = 0; j < strlen(line_arr[i]); j++)
            {
                if (line_arr[i][j] == ')')
                {
                    line_arr[i][j] = '\0';
                }
                if (line_arr[i][j] == '(')
                {
                    move++;
                }
                line_arr[i][j] = line_arr[i][j + move];
            }
            move = 0;
        }
        // check for banned strings
        for (int i = 1; i < num_items; i++)
        {
            for (int j = 0; j < 19; j++)
            {
                if (strcmp(line_arr[i], banned_strings[j]) == 0)
                {
                    fprintf(stderr, "Prvky se nesmi jmenovat stejne jako prikazy!\n");
                    return false;
                }
            }
            // char type validation
            for (int j = 0; j < strlen(line_arr[i]); j++)
            {
                if (!((line_arr[i][j] >= 'a' && line_arr[i][j] <= 'z') || (line_arr[i][j] >= 'A' && line_arr[i][j] <= 'Z')))
                {
                    fprintf(stderr, "Prvky musi byt retezce pouze z malych nebo velkych pismen abecedy!\n");
                    return false;
                }
            }
        }
        // checks if there are no same pairs
        for (int i = 1; i < num_items; i += 2)
        {
            for (int j = 1; j < num_items; j += 2)
            {
                if (i != j)
                {
                    if (strcmp(line_arr[i], line_arr[j]) == 0 && strcmp(line_arr[i + 1], line_arr[j + 1]) == 0)
                    {
                        fprintf(stderr, "Relace nesmi obsahovat vice stejnych dvojic!\n");
                        return false;
                    }
                }
            }
        }
    }
    // INVALID LINE IDENTIFIER
    else
    {
        fprintf(stderr, "Spatne zadany identifikator radku!\n");
        return false;
    }
    return true;
}
//------SET-HELPERS------
void set_ctor_from_line_string(Set *set, char *string, int row)
{
    int items_count = 0;
    char **splitted_line = my_split(string, ' ', strlen(string), &items_count);
    set->row = row;
    set->size = 0;
    set->items = malloc(items_count * sizeof(char *));
    for (int i = 1; i < items_count; i++)
    {
        set->items[i - 1] = malloc(strlen(splitted_line[i]) + 1);
        strcpy(set->items[i - 1], splitted_line[i]);
        set->size = i;
    }
    for (int i = 0; i < items_count; i++)
    {
        free(splitted_line[i]);
    }
    free(splitted_line);
}

void set_ctor(Set *set, char **items, int row, int items_len)
{
    set->row = row;
    set->items = malloc(items_len * sizeof(char *));
    for (int i = 0; i < items_len; i++)
    {
        set->items[i] = malloc(strlen(items[i]) + 1);
        strcpy(set->items[i], items[i]);
        set->size = i + 1;
    }
}

void set_print_from_array(char **items, int items_count)
{
    printf("S ");
    if (items_count > 0)
    {
        for (int i = 0; i < items_count; i++)
        {
            if (i == items_count - 1)
            {
                printf("%s\n", items[i]);
            }
            else
                printf("%s ", items[i]);
        }
    }
    else
        printf("{}\n");
}

void set_dtor(Set *set)
{
    set->row = 0;
    for (int i = 0; i < set->size; i++)
    {
        free(set->items[i]);
    }
    free(set->items);
    set->items = NULL;
    set->size = 0;
}

void setarray_ctor(SetArray *setar)
{
    setar->len = 0;
    setar->data = NULL;
}

void *setarray_inc(SetArray *setar)
{
    void *p = realloc(setar->data, (setar->len + 1) * sizeof(Set));
    if (p == NULL)
        return NULL;
    setar->len++;
    return setar->data = p;
}

void *set_copy(Set *src, Set *dst)
{
    set_ctor(dst, src->items, src->row, src->size);
    return dst;
}

void *set_array_append(SetArray *setar, Set *s)
{
    if (setarray_inc(setar))
        return set_copy(s, &setar->data[setar->len - 1]); //musím použít jinou fci než set_ctor, protože ta je přizpůsobená na to, aby to předělala z toho stringu (prakticky jen trochu upravím ten creator, ale se správnýma indexama)
    else
        return NULL;
}

//------SET-COMMANDS------
int is_empty_com(int first_line_num, char *lines_array[])
{
    Set set;
    set_ctor_from_line_string(&set, lines_array[first_line_num - 1], first_line_num);
    if (set.size == 0)
        printf("true");
    else
        printf("false");
    set_dtor(&set);
    return 0;
}
int card_com(int first_line_num, char *lines_array[])
{
    Set s;
    set_ctor_from_line_string(&s, lines_array[first_line_num - 1], first_line_num);
    printf("%d\n", s.size);
    set_dtor(&s);
    return 0;
}
int complement_com(int first_line_num, char *lines_array[])
{
    Set set;
    Set universe;
    set_ctor_from_line_string(&set, lines_array[first_line_num - 1], first_line_num);
    set_ctor_from_line_string(&universe, lines_array[0], 1);
    int items_count_comp = 0;
    int is_identical = 0;
    char **comp_arr = malloc(sizeof(char *));
    for (int i = 0; i < universe.size; i++)
    {
        for (int j = 0; j < set.size; j++)
        {
            if (!strcmp(universe.items[i], set.items[j]))
            {
                is_identical = 1;
                break;
            }
        }
        if (is_identical == 0)
        {
            comp_arr = realloc(comp_arr, (items_count_comp + 1) * sizeof(char *));
            comp_arr[items_count_comp] = malloc(strlen(universe.items[i]) + 1);
            strcpy(comp_arr[items_count_comp], universe.items[i]);
            items_count_comp++;
        }
        is_identical = 0;
    }
    set_print_from_array(comp_arr, items_count_comp);
    set_dtor(&set);
    set_dtor(&universe);
    for (int i = 0; i < items_count_comp; i++)
    {
        free(comp_arr[i]);
    }
    free(comp_arr);
    return 0;
}
int union_com(int first_line_num, int second_line_num, char *lines_array[])
{
    Set first_set;
    Set second_set;
    set_ctor_from_line_string(&first_set, lines_array[first_line_num - 1], first_line_num);
    set_ctor_from_line_string(&second_set, lines_array[second_line_num - 1], second_line_num);
    char **uni_array = malloc(sizeof(char *));
    int items_count_uni = 0, is_identical = 0;
    for (int i = 0; i < first_set.size; i++)
    {
        uni_array = realloc(uni_array, (items_count_uni + 1) * sizeof(char *));
        uni_array[items_count_uni] = malloc(strlen(first_set.items[i]) + 1);
        strcpy(uni_array[items_count_uni], first_set.items[i]);
        items_count_uni++;
    }

    for (int i = 0; i < second_set.size; i++)
    {
        for (int j = 0; j < items_count_uni; j++)
        {
            if (!strcmp(second_set.items[i], uni_array[j]))
            {
                is_identical = 1;
                break;
            }
        }
        if (is_identical == 0)
        {
            uni_array = realloc(uni_array, (items_count_uni + 1) * sizeof(char *));
            uni_array[items_count_uni] = malloc(strlen(second_set.items[i]) + 1);
            strcpy(uni_array[items_count_uni], second_set.items[i]);
            items_count_uni++;
        }
        is_identical = 0;
    }
    set_print_from_array(uni_array, items_count_uni);
    set_dtor(&first_set);
    set_dtor(&second_set);
    for (int i = 0; i < items_count_uni; i++)
    {
        free(uni_array[i]);
    }
    free(uni_array);
    return 0;
}
int intersect_com(int first_line_num, int second_line_num, char *lines_array[])
{
    Set first_set;
    Set second_set;
    set_ctor_from_line_string(&first_set, lines_array[first_line_num - 1], first_line_num);
    set_ctor_from_line_string(&second_set, lines_array[second_line_num - 1], second_line_num);
    char **inter_array = malloc(sizeof(char *));
    int items_count_inter = 0, is_identical = 0;
    for (int i = 0; i < first_set.size; i++)
    {
        for (int j = 0; j < second_set.size; j++)
        {
            if (!strcmp(first_set.items[i], second_set.items[j]))
            {
                is_identical = 1;
                break;
            }
        }
        if (is_identical == 1)
        {
            inter_array = realloc(inter_array, (items_count_inter + 1) * sizeof(char *));
            inter_array[items_count_inter] = malloc(strlen(first_set.items[i]) + 1);
            strcpy(inter_array[items_count_inter], first_set.items[i]);
            items_count_inter++;
        }
        is_identical = 0;
    }
    set_print_from_array(inter_array, items_count_inter);
    set_dtor(&first_set);
    set_dtor(&second_set);
    for (int i = 0; i < items_count_inter; i++)
    {
        free(inter_array[i]);
    }
    free(inter_array);
    return 0;
}
int minus_com(int first_line_num, int second_line_num, char *lines_array[])
{
    Set first_set;
    Set second_set;
    set_ctor_from_line_string(&first_set, lines_array[first_line_num - 1], first_line_num);
    set_ctor_from_line_string(&second_set, lines_array[second_line_num - 1], second_line_num);
    char **min_array = malloc(sizeof(char *));
    int items_count_min = 0, is_identical = 0;
    for (int i = 0; i < first_set.size; i++)
    {
        for (int j = 0; j < second_set.size; j++)
        {
            if (!strcmp(first_set.items[i], second_set.items[j]))
            {
                is_identical = 1;
                break;
            }
        }
        if (is_identical == 0)
        {
            min_array = realloc(min_array, (items_count_min + 1) * sizeof(char *));
            min_array[items_count_min] = malloc(strlen(first_set.items[i]) + 1);
            strcpy(min_array[items_count_min], first_set.items[i]);
            items_count_min++;
        }
        is_identical = 0;
    }
    set_print_from_array(min_array, items_count_min);
    set_dtor(&first_set);
    set_dtor(&second_set);
    for (int i = 0; i < items_count_min; i++)
    {
        free(min_array[i]);
    }
    free(min_array);
    return 0;
}
int subseteq_com(int first_line_num, int second_line_num, char *lines_array[])
{
    Set first_set;
    Set second_set;
    set_ctor_from_line_string(&first_set, lines_array[first_line_num - 1], first_line_num);
    set_ctor_from_line_string(&second_set, lines_array[second_line_num - 1], second_line_num);
    int is_identical = 0;
    for (int i = 0; i < first_set.size; i++)
    {
        for (int j = 0; j < second_set.size; j++)
        {
            if (!strcmp(first_set.items[i], second_set.items[j]))
            {
                is_identical = 1;
                break;
            }
        }
        if (is_identical == 0)
        {
            printf("false\n"); // TODO FIX
            set_dtor(&first_set);
            set_dtor(&second_set);
            return 0;
        }
        is_identical = 0;
    }
    set_dtor(&first_set);
    set_dtor(&second_set);
    printf("true\n"); // TODO FIX
    return 1;
}
int subset_com(int first_line_num, int second_line_num, char *lines_array[])
{
    if (subseteq_com(first_line_num, second_line_num, lines_array) && !equals_com(first_line_num, second_line_num, lines_array))
        printf("true\n"); // TODO FIX
    else
        printf("false\n"); // TODO FIX
    return 0;
}
int equals_com(int first_line_num, int second_line_num, char *lines_array[])
{
    Set first_set;
    Set second_set;
    set_ctor_from_line_string(&first_set, lines_array[first_line_num - 1], first_line_num);
    set_ctor_from_line_string(&second_set, lines_array[second_line_num - 1], second_line_num);
    int is_identical = 0;
    if (first_set.size == second_set.size)
    {
        for (int i = 0; i < first_set.size; i++)
        {
            for (int j = 0; j < second_set.size; j++)
            {
                if (!strcmp(first_set.items[i], second_set.items[j]))
                {
                    is_identical = 1;
                    break;
                }
            }
            if (is_identical == 0)
            {
                printf("false\n");
                set_dtor(&first_set);
                set_dtor(&second_set);
                return 0;
            }
            is_identical = 0;
        }
        set_dtor(&first_set);
        set_dtor(&second_set);
        printf("true\n");
        return 1;
    }
    else
        printf("false\n");
    set_dtor(&first_set);
    set_dtor(&second_set);
    return 0;
}

//------SESSION-HELPERS-------
void session_init(Session *session, int row) {
    session->row = row;
    session->size= 0;
    session->pairs=NULL;
}

void session_append(Session *session, Session_pair *pair) {
    session->size += 1;
    session->pairs = realloc(session->pairs, session->size*sizeof(Session_pair));
    session->pairs[session->size-1].left_val = malloc(strlen(pair->left_val)*sizeof(char *));
    strcpy(session->pairs[session->size-1].left_val, pair->left_val);
    session->pairs[session->size-1].right_val = malloc(strlen(pair->right_val)*sizeof(char *));
    strcpy(session->pairs[session->size-1].right_val, pair->right_val);


}
void session_pair_ctor(Session_pair *pair, char *left_val, char *right_val)
{
    pair->left_val = malloc(strlen(left_val) * sizeof(char *));
    pair->right_val = malloc(strlen(right_val) * sizeof(char *));
    pair->left_val = left_val;
    pair->right_val = right_val;
}
void session_pair_dtor(Session_pair *pair) {
    free(pair->left_val);
    free(pair->right_val);
}

void session_ctor_from_line_string(Session *session, char *string, int row)
{
    session_init(session, row);
    int items_count = 0;
    char *working_string;
    working_string = malloc(strlen(string) * sizeof(char *));
    strcpy(working_string, string);
    remove_all_chars(working_string, ')');
    remove_all_chars(working_string, '(');
    remove_all_chars(working_string, 'R');

    char **splitted_line = my_split(working_string, ' ', strlen(working_string), &items_count);
    Session_pair tmp_pair;
    for (int i = 1; i < items_count; i+=2)
    {   
        session_pair_ctor(&tmp_pair, splitted_line[i], splitted_line[i+1]);
        session_append(session, &tmp_pair);
    }
    
    session_pair_dtor(&tmp_pair);
    
}
void session_ctor(Session *session, Session_pair *pairs, int row, int size) {
    for (int i = 0; i < size; i++)
    {
        session_append(session, &(pairs[i]));
    }
    
}
void session_print(Session session) {
    printf("R ");
    for (int i = 0; i < session.size; i++)
    {
        printf("(%s %s) ", session.pairs[i].left_val, session.pairs[i].right_val);
    }
    printf("\n");
}
void session_dtor(Session *session) {
    for (int i = 0; i < session->size; i++)
    {
        session_pair_dtor(&(session->pairs[i]));
    }
    free(session->pairs);
}

//------SESSION-COMMANDS------
int reflexive_com(int first_line_num, char *lines_array[])
{
    printf("reflexive\n");
    return 0;
}
int symmetric_com(int first_line_num, char *lines_array[])
{
    printf("symetric\n");
    return 0;
}
int antisymmetric_com(int first_line_num, char *lines_array[])
{
    printf("antisymmetric\n");
    return 0;
}
int transitive_com(int first_line_num, char *lines_array[])
{
    printf("transitive\n");
    return 0;
}
int function_com(int first_line_num, char *lines_array[])
{
    printf("function\n");
    return 0;
}
int domain_com(int first_line_num, char *lines_array[])
{
    printf("domain\n");
    return 0;
}
int codomain_com(int first_line_num, char *lines_array[])
{
    printf("codomain\n");
    return 0;
}
int injective_com(int first_line_num, char *lines_array[])
{
    printf("injective\n");
    return 0;
}
int surjective_com(int first_line_num, char *lines_array[])
{
    printf("surjective\n");
    return 0;
}
int bijective_com(int first_line_num, char *lines_array[])
{
    printf("bijective\n");
    return 0;
}
