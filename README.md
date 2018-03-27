# compiler2016
The compiler is C in the NASM assembler. Implemented lexical, syntactic, semantic analysis, code generation.

The Nextch.c file contains an I / O module.

In the file NEXTSYM4.C - the basic logic of lexical analysis.

In the file LAST.C - the basic logic of syntactic, semantic analysis and code generation.

ERROR.C - entering the information about the error in the error list.

HASH.C is a hash function for the name table.

LITFUNCS.C - functions for working with bit sets, which implement FOLLOW sets (including the verification of belonging to a set, the union of sets,). Implements a check of the coincidence of the scanned token and the expected one. Implements a panic mode (passing tokens to the first "familiar" if the current token does not match the expected one).

RUMTABL3.C - work with the table of names. Implemented the entry in the table of names based on the hash, search in the table of names.

DECKEY.H - language code of the language.

DECW.H - codes of tokens-delimiters, constants, auxiliary constants.

ERR.H is the error data structure for the analyzer.

FUNCS.H - declaration of functions.

IODEFS.H - constants for the I / O block.

RUMERCOD.h - error codes for analysis.

TEXTP.H - structures for the position of the current token in the input stream.

TREE.H - constants for types of identifiers for analysis.

TYPES.H - declaration of codes of basic types.

TYPET.h - declaration of external variables.

# Run tests
make -i -f makefile 

# I/O Examples
Examples of input files in the repository:

1. prog_err - with errors,

2. prog_r - without errors. Input for the program in inp_inc

3. prog_s.pas - without errors, input for the program in inp_cube,

4. prog_exp - without errors, contains an expression,

5. nod.pas - without errors, Euclid's algorithm for computing the GCD of two numbers. Input for the program in inp_nod
Output: 

1. prog_err.lst, 

2. prog_r.lst,

3. prog_s.lst

4. prog_exp.lst

5. nod.lst

# Lang
The language is pascal-like, supports:

1. integer, logical types,

2. section of variables,

3. a section of functions, procedures,

4. transmission in function of parameters by value.

5. Assignment operation,

6. operations of addition, subtraction, division with the remainder, comparison, logical AND, OR, NOT,

7. expressions with brackets,

8. call functions, procedures,

9. I / O functions (read, readln, write, writeln)

10. An empty statement,

11. Conditional operator (full and abbreviated),

12. cycles with a precondition (while do) and with a postcondition (repeat until),

13. Simple and compound operators,

14. Comments.


## Язык следующий:

<программа> ::= **program** <имя> (<имя файла> {, <имя файла>}); <блок>.

<имя файла> ::= <имя>

<имя> ::= <буква> {<буква> | <цифра>}

<блок> ::= <раздел переменных> <раздел процедур и функций> <раздел операторов>

<определение константы> ::= <имя> = <константа>

<константа> ::= <число без знака> | <знак> <число без знака> | <имя константы> | <знак> <имя константы> | <строка>

<число без знака> ::= <целое без знака>

<целое без знака> ::= <цифра> {<цифра>}

<знак> ::= + | - 

<имя константы> ::= <имя>

<строка> ::= ‘<символ> {<символ>}’

<тип> ::= <простой тип> | <составной тип> 

<простой тип> ::= <перечислимый тип> | <ограниченный тип> | <имя типа>

<перечислимый тип> ::=(<имя> {, <имя>})

<ограниченный тип> ::= <константа> .. <константа>

<имя типа> ::= <имя>

<составной тип> ::= <регулярный тип>  

<регулярный тип> ::= **array** [<простой тип > {, < простой тип >}] **of** <тип компоненты >

<тип компоненты> ::= <тип >

<базовый тип> ::= <простой тип>

<раздел переменных> ::= **var** <описание однотипных переменных> ;
                                                    {<описание однотипных переменных>;} | <пусто> 
                                                    
<описание однотипных переменных> ::= <имя> {, <имя>} : <тип>

<раздел процедур и функций> ::= {<описание процедуры или функции> ;}

<описание процедуры или функции> ::= <описание процедуры> | <описание функции>

<описание процедуры> ::= <заголовок процедуры> <блок>

<заголовок процедуры> ::= **procedure** <имя> ; | **procedure** <имя> (<раздел формальных параметров> {; <раздел формальных параметров>}) ;

<раздел формальных параметров> ::= <группа параметров> | **var** <группа параметров> | **function** <группа параметров> | **procedure** <имя> {, <имя>}

<группа параметров> ::= <имя> {, <имя>} : <имя типа>

<описание функции> ::= <заголовок функции> <блок>

<заголовок функции> ::= **function** <имя> : <тип результата> ; | **function** <имя> (<раздел формальных параметров> {; <раздел формальных параметров>}) : <тип результата> ;

<тип результата> ::= <имя типа>

<раздел операторов> ::= <составной оператор>

<оператор> ::= <простой оператор> | <сложный оператор>

<простой оператор> ::= <оператор присваивания> | <оператор процедуры>  | <пустой оператор>

<оператор присваивания> ::= <переменная> := <выражение> | <имя функции> := <выражение>

<переменная> ::= <полная переменная> | <компонента переменной> 

<полная переменная> ::= <имя переменной>

<имя переменной> ::= <имя>

<компонента переменной> ::= <индексированная переменная>

<индексированная переменная> ::= <переменная-массив> [<выражение> {, <выражение>}]

<переменная-массив> ::= <переменная>

<выражение> ::= <простое выражение> | <простое выражение> <операция отношения><простое выражение>

<операция отношения> ::= = | <> | < | <= | >= | > 

<простое выражение> ::=  <знак> <слагаемое> { <аддитивная операция> <слагаемое>}

<аддитивная операция> ::= + | - | **or**

<слагаемое> ::= <множитель> { <мультипликативная операция> <множитель>}

<мультипликативная операция> ::= * | / | **mod** | **and**

<множитель> ::= <переменная> | <константа без знака> | (<выражение>) | <обозначение функции> | not <множитель>

<константа без знака> ::= <число без знака> | <строка> | <имя константы> | **nil**

<обозначение функции> ::= <имя функции> |
                                      <имя функции> (<фактический параметр> {, <фактический параметр>})
<имя функции> ::= <имя>

<оператор процедуры> ::= <имя процедуры> | 
                                   <имя процедуры> (<фактический параметр> {, <фактический параметр>})
                                   
<имя процедуры> ::= <имя>

<фактический параметр> ::= <выражение> | <переменная> | <имя процедуры> | <имя функции>

<пустой оператор> ::= <пусто>

<пусто> ::=

<сложный оператор> ::= <составной оператор> | <выбирающий оператор> | <оператор цикла> | <оператор присоединения>

<составной оператор> ::=  **begin** <оператор> {; <оператор>} **end**

<выбирающий оператор> ::= <условный оператор> 

<условный оператор> ::= **if** <выражение> then <оператор> | **if** <выражение> **then** <оператор> **else** <оператор>

<оператор цикла> ::= <цикл с предусловием> | <цикл с постусловием> | <цикл с параметром>

<цикл с предусловием> ::= **while** <выражение> **do** <оператор>

<цикл с постусловием> ::= **repeat** <оператор> {; <оператор>} **until** <выражение>

<цикл с параметром> ::= **for** <параметр цикла > := <выражение> 
                                               <направление>  <выражение> do  <оператор>     
                                               
<параметр цикла >:= <имя>  

<направление> ::= **to** | **downto**  

