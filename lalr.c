#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define SIZE 40
#define MAX_STATES 50
#define MAX_PRODUCTIONS 26

typedef struct {
    char lhs;
    char rhs[20];
    int dot_pos;
    char lookahead;
} LR1Item;

typedef struct {
    LR1Item items[SIZE];
    int count;
} LR1State;

LR1State lr1_states[MAX_STATES];
int lr1_state_count = 0;
char productions[MAX_PRODUCTIONS][20];
char follow_sets[MAX_PRODUCTIONS][SIZE];
char terminals[25] = {0};
int termsnum;

// Action and Goto tables for LALR(1)
int lalr_action[MAX_STATES][25];
int lalr_goto[MAX_STATES][MAX_PRODUCTIONS];

// Function to check if an item exists in a state
int contains_lr1_item(LR1State state, LR1Item item) {
    for (int i = 0; i < state.count; i++) {
        if (state.items[i].lhs == item.lhs &&
            strcmp(state.items[i].rhs, item.rhs) == 0 &&
            state.items[i].dot_pos == item.dot_pos &&
            state.items[i].lookahead == item.lookahead) {
            return 1;
        }
    }
    return 0;
}

// Function to add an item to a state
void add_lr1_item(LR1State *state, LR1Item item) {
    if (!contains_lr1_item(*state, item)) {
        state->items[state->count++] = item;
    }
}

// Function to print an LR(1) state
void print_lr1_state(LR1State state) {
    for (int i = 0; i < state.count; i++) {
        printf("%c -> ", state.items[i].lhs);
        for (int j = 0; j < strlen(state.items[i].rhs); j++) {
            if (j == state.items[i].dot_pos) {
                printf(".");
            }
            printf("%c", state.items[i].rhs[j]);
        }
        if (state.items[i].dot_pos == strlen(state.items[i].rhs)) {
            printf(".");
        }
        printf(", %c\n", state.items[i].lookahead);
    }
}

// Closure function for LR(1) items
void lr1_closure(LR1State *state) {
    for (int i = 0; i < state->count; i++) {
        LR1Item item = state->items[i];
        if (item.dot_pos < strlen(item.rhs)) {
            char symbol = item.rhs[item.dot_pos];
            if (isupper(symbol)) { // Non-terminal
                for (int j = 0; j < MAX_PRODUCTIONS; j++) {
                    if (productions[j][0] == symbol) {
                        LR1Item new_item = {symbol, "", 0, item.lookahead};
                        strcpy(new_item.rhs, productions[j] + 3); // Skip the "A->"
                        add_lr1_item(state, new_item);
                    }
                }
            }
        }
    }
}

// Goto function for LR(1) items
LR1State lr1_goto(LR1State state, char symbol) {
    LR1State new_state = {0};
    for (int i = 0; i < state.count; i++) {
        LR1Item item = state.items[i];
        if (item.dot_pos < strlen(item.rhs) && item.rhs[item.dot_pos] == symbol) {
            LR1Item new_item = item;
            new_item.dot_pos++;
            add_lr1_item(&new_state, new_item);
        }
    }
    lr1_closure(&new_state);
    return new_state;
}

// Function to check if two LR(1) states are equal (ignoring lookahead)
int are_lr1_states_equal(LR1State state1, LR1State state2) {
    if (state1.count != state2.count) {
        return 0;
    }
    for (int i = 0; i < state1.count; i++) {
        if (state1.items[i].lhs != state2.items[i].lhs ||
            strcmp(state1.items[i].rhs, state2.items[i].rhs) != 0 ||
            state1.items[i].dot_pos != state2.items[i].dot_pos) {
            return 0;
        }
    }
    return 1;
}

// Function to merge two LR(1) states
void merge_lr1_states(LR1State *state1, LR1State state2) {
    for (int i = 0; i < state2.count; i++) {
        add_lr1_item(state1, state2.items[i]);
    }
}

// Function to get or add an LR(1) state
int get_or_add_lr1_state(LR1State new_state) {
    for (int i = 0; i < lr1_state_count; i++) {
        if (are_lr1_states_equal(lr1_states[i], new_state)) {
            merge_lr1_states(&lr1_states[i], new_state);
            return i;
        }
    }
    lr1_states[lr1_state_count++] = new_state;
    return lr1_state_count - 1;
}

// Function to construct the LALR(1) parsing table
void construct_lalr_table() {
    // Start with the augmented grammar S' -> S
    LR1State start_state = {0};
    LR1Item start_item = {'S', "S", 0, '$'};
    add_lr1_item(&start_state, start_item);
    lr1_closure(&start_state);

    int start_state_index = get_or_add_lr1_state(start_state);

    // Initialize action and goto tables
    memset(lalr_action, 0, sizeof(int) * MAX_STATES * 25);
    memset(lalr_goto, -1, sizeof(int) * MAX_STATES * MAX_PRODUCTIONS);

    // Generate the LALR(1) table
    for (int i = 0; i < lr1_state_count; i++) {
        for (int j = 0; j < termsnum; j++) {
            char terminal = terminals[j];
            LR1State new_state = lr1_goto(lr1_states[i], terminal);
            if (new_state.count > 0) {
                int new_state_index = get_or_add_lr1_state(new_state);
                lalr_action[i][j] = new_state_index; // Shift action
            }
        }

        for (int j = 0; j < MAX_PRODUCTIONS; j++) {
            char non_terminal = 'A' + j;
            LR1State new_state = lr1_goto(lr1_states[i], non_terminal);
            if (new_state.count > 0) {
                int new_state_index = get_or_add_lr1_state(new_state);
                lalr_goto[i][j] = new_state_index; // Goto action
            }
        }
    }

    // Print the LALR(1) table
    printf("LALR(1) Parsing Table:\n");
    for (int i = 0; i < lr1_state_count; i++) {
        printf("State I%d:\n", i);
        print_lr1_state(lr1_states[i]);
    }
}

// Function to parse a given input string using the LALR(1) table
int parse_string(char *input) {
    int stack[MAX_STATES];
    int top = 0;
    stack[top++] = 0; // Start state
    int i = 0;

    while (1) {
        int state = stack[top - 1];
        char symbol = input[i];

        if (lalr_action[state][symbol] == -1) { // Accept
            printf("Accepted\n");
            return 1;
        } else if (lalr_action[state][symbol] == 0) { // Error
            printf("Rejected\n");
            return 0;
        } else { // Shift
            stack[top++] = lalr_action[state][symbol];
            i++;
        }
    }
}

void set_insert(char set_array[SIZE], char x) {
    int present=0;
    for(int i=0;i<SIZE;i++) {
        if(set_array[i]==x) {
            present=1;
            break;
        }
        if(set_array[i]==0) {
            set_array[i]=x;
            break;
        }
    }
    //set_print(set_array);
    return;
}

int findnonterms(char productions[26][20], char terminals[25]) {
    int nums = 0;
    for(int i=0;i<26;i++) {
        int len = strlen(productions[i]);
        for(int j=0;j<len;j++) {
            char currchar = productions[i][j];
            if(!isupper(currchar) && currchar != '|' && currchar != '#') {
                nums += 1;
                set_insert(terminals, currchar);
            }
        }
    }
    nums += 1;
    set_insert(terminals, '$');
    //for(int i=0;i<25;i++) printf("%c ", terminals[i]);
    return nums;
}

int main() {
    // Input: CFG productions
    printf("Enter the productions: (enter 0 to stop)\n");
    while (1) {
        char currstring[20] = {0};
        char lhs = 'A';
        scanf("%c->%s", &lhs, currstring);
        if (lhs == '0') break;
        strcpy(productions[lhs - 'A'], currstring);
    }

    termsnum = findnonterms(productions, terminals);

    // Construct the LALR(1) table
    construct_lalr_table();

    // Input string to parse
    char input[100];
    printf("Enter the string to parse:\n");
    scanf("%s", input);

    // Parse the string
    if (parse_string(input)) {
        printf("String is accepted by the grammar.\n");
    } else {
        printf("String is rejected by the grammar.\n");
    }

    return 0;
}