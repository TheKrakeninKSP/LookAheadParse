# LALR(1) Parser in C

![LALR(1) Parser](https://img.shields.io/badge/Parsing-LALR(1)-blue.svg) ![Language](https://img.shields.io/badge/Language-C-orange.svg)

## Overview
This project implements an **LALR(1) parser** in **C**, which constructs parsing tables for a given context-free grammar and checks whether input strings are accepted by the grammar. The program follows the **Look-Ahead LR(1) (LALR(1)) parsing** approach used in modern compiler design.

## Features
- Parses **context-free grammars** using **LALR(1) parsing**.
- Implements **LR(1) item closure** and **goto functions**.
- Constructs **action and goto tables** for LALR(1) parsing.
- Merges equivalent **LR(1) states** to minimize state explosion.
- Accepts grammar and input string dynamically from the user.
- Displays parsing tables and parsing steps for debugging.

## How It Works
1. **Input the Grammar**: Define productions of a grammar interactively.
2. **Construct Parsing Table**: The program generates **action** and **goto tables** for LALR(1) parsing.
3. **Parse Input Strings**: The parser checks if an input string is **accepted or rejected** by the grammar.

## Installation & Usage
### Prerequisites
- A C compiler (GCC, Clang, or MSVC)

### Compilation
```sh
gcc lalr.c -o lalr
```

### Running the Parser
```sh
./lalr
```

### Example Usage
#### Input:
```
Enter the productions: (enter 0 to stop)
E->E+T
E->T
T->T*F
T->F
F->(E)
F->id
0
Enter the string to parse:
id+id*id
```

#### Output:
```
LALR(1) Parsing Table:
State I0:
E -> . E, $
E -> . T, $
T -> . T * F, +
...
String is accepted by the grammar.
```

## Code Structure
- **`lalr.c`** → Main implementation file containing parsing logic.
- **Functions:**
  - `lr1_closure()`: Computes closure of LR(1) items.
  - `lr1_goto()`: Implements goto function for LR(1) items.
  - `construct_lalr_table()`: Generates action and goto tables.
  - `parse_string()`: Parses input using the generated parsing table.

---

**Contributions are welcome!** Feel free to open an issue or submit a pull request. 🚀
