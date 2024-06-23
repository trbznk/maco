#pragma once

#include <stdlib.h>

//
// forward declarations
//

typedef struct AST AST;
typedef struct Parser Parser;
typedef struct Arena Arena;
typedef struct Allocator Allocator;
typedef struct String String;
typedef struct Lexer Lexer;

//
// Basic Types
//

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef size_t usize;
typedef float f32;
typedef double f64;

struct String {
    char *str;
    usize size;
};

String init_string(const char *str);
String char_to_string(Allocator *allocator, char c);
String string_slice(Allocator *allocator, String s, usize start, usize stop);
String string_concat(Allocator *allocator, String s1, String s2);
String string_insert(Allocator *allocator, String s1, String s2, usize idx);
String string_insert_char(Allocator *allocator, String s1, char c, usize idx);
bool string_eq(String s1, String s2);
void print(String s);

//
// lexer
//

extern const char *BUILTIN_FUNCTIONS[];
extern const usize BUILTIN_FUNCTIONS_COUNT;

// maybe we treat 'constant' symbols like pi or e in the future as normal variables
// that already have a value @todo @remove
extern const char *BUILTIN_CONSTANTS[];
extern const usize BUILTIN_CONSTANTS_COUNT;

typedef enum {
    TOKEN_NUMBER,
    TOKEN_IDENTIFIER,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,
    TOKEN_CARET,

    TOKEN_L_PAREN,
    TOKEN_R_PAREN,

    TOKEN_COMMA,
    TOKEN_HASH,

    TOKEN_EOF,

    TOKEN_TYPE_COUNT
} TokenType;

typedef struct {
    TokenType type;
    String text;
    
    // TODO: replace this and the occurences with string method
    //       string_contains
    bool contains_dot;
} Token;

struct Lexer {
    Allocator *allocator;
    String source;
    usize pos;
};

char lexer_current_char(Lexer*);
Token lexer_peek_token(Lexer*);
Token lexer_next_token(Lexer*);
void lexer_print_tokens(Lexer*);

const char* token_type_to_string(TokenType);

bool is_builtin_function(String);
bool is_builtin_constant(String);

//
// ast
//

#define INTEGER(value) init_ast_integer(ip->allocator, value)
#define REAL(value) init_ast_real(ip->allocator, value)
#define SYMBOL(name) init_ast_symbol(ip->allocator, name)
#define ADD(left, right) init_ast_binop(ip->allocator, left, right, OP_ADD)
#define SUB(left, right) init_ast_binop(ip->allocator, left, right, OP_SUB)
#define MUL(left, right) init_ast_binop(ip->allocator, left, right, OP_MUL)
#define DIV(left, right) init_ast_binop(ip->allocator, left, right, OP_DIV)
#define POW(left, right) init_ast_binop(ip->allocator, left, right, OP_POW)
#define CALL(name, args) init_ast_call(ip->allocator, name, args)

typedef struct ASTArray ASTArray;

typedef enum {
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_POW,

    OP_UADD,
    OP_USUB,

    OP_TYPE_COUNT
} OpType;

typedef enum {
    AST_INTEGER,
    AST_REAL,
    AST_SYMBOL,
    AST_BINOP,
    AST_UNARYOP,

    AST_CALL,

    // @remove AST_EMPTY type when there are more high level nodes like NODE_PROGRAMM or something similar
    AST_EMPTY,

    AST_TYPE_COUNT
} ASTType;

struct ASTArray {
    usize size;
    AST **data;
};

struct AST {
    ASTType type;
    union {

        struct {
            i64 value;
        } integer;

        struct {
            f64 value;
        } real;

        struct {
            String name;
        } symbol;

        struct {
            AST *left;
            AST *right;
            OpType op;
        } binop;

        struct {
            AST *operand;
            OpType op;
        } unaryop;

        struct {
            String name;
            ASTArray args;
        } func_call;

        bool empty; // temporary for ASTType empty - @remove
    };
};

AST* init_ast_integer(Allocator*, i64);
AST* init_ast_real(Allocator*, f64);
AST* init_ast_symbol(Allocator*, String);
AST* init_ast_binop(Allocator*, AST*, AST*, OpType);
AST* init_ast_unaryop(Allocator*, AST*, OpType);
AST* init_ast_call(Allocator*, String, ASTArray);
AST* init_ast_empty(Allocator*);

void ast_array_append(Allocator*, ASTArray*, AST*);

const char *op_type_to_debug_string(OpType);
const char *op_type_to_string(OpType);
const char *ast_type_to_debug_string(ASTType);
uint8_t op_type_precedence(OpType);

ASTArray ast_to_flat_array(Allocator*, AST*);
bool ast_contains(AST*, AST*);
bool ast_is_fraction(AST*);
bool ast_is_numeric(AST*);

f64 ast_to_f64(AST*);


//
// parser
//

AST* parse(Lexer*);
AST* parse_expr(Lexer*);
AST* parse_factor(Lexer*);

//
// interp
//

typedef struct {
    Allocator *allocator;
} Interp;

AST *interp(Interp*, AST*);
AST *interp_binop_pow(Interp*, AST*, AST*);

bool ast_match(AST*, AST*);
bool ast_match_type(AST*, AST*);

String _ast_to_string(Allocator*, AST*, u8);
#define ast_to_string(allocator, node) _ast_to_string(allocator, node, 0)
String ast_to_debug_string(Allocator*, AST*);

//
// gui
//

void init_gui();

//
// general
//

#define panic(msg) fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, msg); exit(1);
#define todo() panic("not implemented");

struct Arena {
    void *memory;
    Arena *prev;

    usize offset;

    // TODO: when arena size is always the same we dont need this one
    usize size;
};

struct Allocator {
    Arena *arena; 
};

Allocator init_allocator();
void *alloc(Allocator *allocator, usize size);
void free_allocator(Allocator *allocator);
