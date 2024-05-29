#include <assert.h>
#include <stdbool.h>
#include <string.h>

#include "casc.h"

const char* op_type_to_debug_string(OpType type) {
    switch(type) {
        case OP_ADD: return "Add";
        case OP_SUB: return "Sub";
        case OP_MUL: return "Mul";
        case OP_DIV: return "Div";
        case OP_POW: return "Pow";
        case OP_UADD: return "UAdd";
        case OP_USUB: return "USub";
        default: assert(false);
    }
}

const char* op_type_to_string(OpType type) {
    switch (type) {
        case OP_ADD:
        case OP_UADD: return "+";
        case OP_SUB:
        case OP_USUB: return "-";
        case OP_MUL: return "*";
        case OP_DIV: return "/";
        case OP_POW: return "^";
        default: assert(false);
    }
}

const char* ast_type_to_debug_string(ASTType type) {
    switch (type) {
        case AST_INTEGER: return "Integer";
        case AST_SYMBOL: return "Symbol";
        case AST_BINOP: return "BinOp";
        case AST_UNARYOP: return "UnaryOp";
        case AST_FUNC_CALL: return "FuncCall";
        case AST_EMPTY: return "Empty";
        case AST_TYPE_COUNT: assert(false);
    }
}

uint8_t op_type_precedence(OpType type) {
    switch (type) {
        case OP_POW:
            return 4;
        case OP_UADD:
        case OP_USUB:
            return 3;
        case OP_MUL:
        case OP_DIV:
            return 2;
        case OP_ADD:
        case OP_SUB:
            return 1;
        default:
            return 0;
    }
}

void ast_array_append(ASTArray *array, AST *node) {
    if (array->size == 0) {
        array->data = malloc(sizeof(AST));
    } else {
        array->data = realloc(array->data, (array->size+1)*sizeof(AST));
    }
    array->data[array->size] = node;
    array->size++;
};

AST* create_ast_integer(Arena* arena, int64_t value) {
    AST* node = arena_alloc(arena, sizeof(AST));
    node->type = AST_INTEGER;
    node->integer.value = value;
    return node;
}

AST* create_ast_symbol(Arena* arena, char *name) {
    AST *node = arena_alloc(arena, sizeof(AST));
    node->type = AST_SYMBOL;
    node->symbol.name = arena_alloc(arena, strlen(name)+1);
    strcpy(node->symbol.name, name);
    assert(strlen(node->symbol.name) == strlen(name));
    return node;
}

AST* create_ast_binop(Arena* arena, AST* left, AST* right, OpType type) {
    AST *node = arena_alloc(arena, sizeof(AST));
    node->type = AST_BINOP;
    node->binop.left = left;
    node->binop.right = right;
    node->binop.type = type;
    return node;
}

AST* create_ast_unaryop(Arena* arena, AST* expr, OpType type) {
    AST* node = arena_alloc(arena, sizeof(AST));
    node->type = AST_UNARYOP;
    node->unaryop.expr = expr;
    node->unaryop.type = type;
    return node;
}

AST* create_ast_func_call(Arena* arena, Token name, ASTArray args) {
    AST *node = arena_alloc(arena, sizeof(AST));
    node->type = AST_FUNC_CALL;
    node->func_call.name = name;
    node->func_call.args = args;
    return node;
}

AST* create_ast_empty(Arena* arena) {
    AST* node = arena_alloc(arena, sizeof(AST));
    node->type = AST_EMPTY;
    node->empty = true;
    return node;
}