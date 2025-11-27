#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

// --- SISTEMA DE TIPOS ---
typedef struct Value Value;
typedef Value (*NativeFn)(); // Ponteiro para função C que retorna Value

typedef enum { 
    VAL_NUM, VAL_STR, VAL_BOOL, VAL_NIL, VAL_OBJ, VAL_FN, VAL_BOUND_METHOD 
} ValueType;

struct Obj; 
struct BoundMethod;

struct Value {
    ValueType type;
    union {
        double number;
        char* string;
        int boolean;
        struct Obj* obj;
        NativeFn fn; // Guarda a função C
        struct BoundMethod* bound;
    } as;
};

// Dicionário de propriedades
typedef struct Entry {
    char* key;
    Value value;
    struct Entry* next;
} Entry;

typedef struct Obj {
    char* className;
    Entry* fields; 
} Obj;

typedef struct BoundMethod {
    Obj* receiver;
    NativeFn method;
} BoundMethod;

// --- PILHA ---
#define MAX_STACK 1024
Value stack[MAX_STACK];
int sp = 0;

void push(Value v) {
    if (sp < MAX_STACK) stack[sp++] = v;
    else { fprintf(stderr, "Erro: Stack overflow\n"); exit(1); }
}

Value pop() {
    if (sp > 0) return stack[--sp];
    return (Value){VAL_NIL};
}

// --- CONSTRUTORES ---
Value new_number(double num) {
    Value v; v.type = VAL_NUM; v.as.number = num; return v;
}
Value new_string(const char* str) {
    Value v; v.type = VAL_STR; v.as.string = strdup(str ? str : ""); return v;
}
Value new_bool(int b) {
    Value v; v.type = VAL_BOOL; v.as.boolean = b; return v;
}
Value new_nil() {
    Value v; v.type = VAL_NIL; return v;
}
Value new_object(char* className) {
    Value v; v.type = VAL_OBJ;
    Obj* o = (Obj*)malloc(sizeof(Obj));
    o->className = strdup(className);
    o->fields = NULL;
    v.as.obj = o;
    return v;
}
// Novo: Cria um valor que aponta para uma função C
Value new_native(NativeFn fn) {
    Value v; v.type = VAL_FN; v.as.fn = fn; return v;
}
Value new_bound_method(Obj* receiver, NativeFn method) {
    Value v; v.type = VAL_BOUND_METHOD;
    BoundMethod* bm = (BoundMethod*)malloc(sizeof(BoundMethod));
    bm->receiver = receiver;
    bm->method = method;
    v.as.bound = bm;
    return v;
}

// --- OPERAÇÕES ---
void print_value(Value v) {
    switch (v.type) {
        case VAL_NUM:  printf("%.2f\n", v.as.number); break;
        case VAL_STR:  printf("%s\n", v.as.string); break;
        case VAL_BOOL: printf("%s\n", v.as.boolean ? "true" : "false"); break;
        case VAL_NIL:  printf("nil\n"); break;
        case VAL_OBJ:  printf("<instance of %s>\n", v.as.obj->className); break;
        case VAL_FN:   printf("<native fn>\n"); break;
        case VAL_BOUND_METHOD: printf("<bound method of %s>\n", v.as.bound->receiver->className); break;
    }
}

int is_truthy(Value v) {
    if (v.type == VAL_NIL) return 0;
    if (v.type == VAL_BOOL) return v.as.boolean;
    return 1;
}

Value op_add(Value a, Value b) {
    // 1. Num + Num
    if (a.type == VAL_NUM && b.type == VAL_NUM) {
        return new_number(a.as.number + b.as.number);
    }
    
    // 2. String + String
    if (a.type == VAL_STR && b.type == VAL_STR) {
        char* res = malloc(strlen(a.as.string) + strlen(b.as.string) + 1);
        sprintf(res, "%s%s", a.as.string, b.as.string);
        return new_string(res);
    }

    // 3. String + Num (Conversão Implícita)
    if (a.type == VAL_STR && b.type == VAL_NUM) {
        // Aloca espaço suficiente para a string + um número double (~24 chars)
        char* res = malloc(strlen(a.as.string) + 32); 
        sprintf(res, "%s%.2f", a.as.string, b.as.number); // Formata o número
        return new_string(res);
    }

    // 4. Num + String (Conversão Implícita)
    if (a.type == VAL_NUM && b.type == VAL_STR) {
        char* res = malloc(strlen(b.as.string) + 32);
        sprintf(res, "%.2f%s", a.as.number, b.as.string);
        return new_string(res);
    }

    return new_nil();
}

Value op_sub(Value a, Value b) { return (a.type == VAL_NUM && b.type == VAL_NUM) ? new_number(a.as.number - b.as.number) : new_nil(); }
Value op_mul(Value a, Value b) { return (a.type == VAL_NUM && b.type == VAL_NUM) ? new_number(a.as.number * b.as.number) : new_nil(); }
Value op_div(Value a, Value b) { return (a.type == VAL_NUM && b.type == VAL_NUM && b.as.number != 0) ? new_number(a.as.number / b.as.number) : new_nil(); }

Value op_lt(Value a, Value b) { return (a.type == VAL_NUM && b.type == VAL_NUM) ? new_bool(a.as.number < b.as.number) : new_bool(0); }
Value op_gt(Value a, Value b) { return (a.type == VAL_NUM && b.type == VAL_NUM) ? new_bool(a.as.number > b.as.number) : new_bool(0); }
Value op_eq(Value a, Value b) { 
    if (a.type != b.type) return new_bool(0);
    if (a.type == VAL_NUM) return new_bool(a.as.number == b.as.number);
    if (a.type == VAL_STR) return new_bool(strcmp(a.as.string, b.as.string) == 0);
    return new_bool(0);
}

Value op_neq(Value a, Value b) { return new_bool(!is_truthy(op_eq(a, b))); }
Value op_le(Value a, Value b) { return (a.type == VAL_NUM && b.type == VAL_NUM) ? new_bool(a.as.number <= b.as.number) : new_bool(0); }
Value op_ge(Value a, Value b) { return (a.type == VAL_NUM && b.type == VAL_NUM) ? new_bool(a.as.number >= b.as.number) : new_bool(0); }

Value op_not(Value v) { return new_bool(!is_truthy(v)); }

Value op_and(Value a, Value b) { return is_truthy(a) ? b : a; }
Value op_or(Value a, Value b)  { return is_truthy(a) ? a : b; }

// Objetos e Atributos
Value get_attr(Value objVal, char* key) {
    if (objVal.type != VAL_OBJ) return new_nil();
    Obj* obj = objVal.as.obj;
    Entry* cur = obj->fields;
    while (cur) {
        if (strcmp(cur->key, key) == 0) {
            if (cur->value.type == VAL_FN) return new_bound_method(obj, cur->value.as.fn);
            return cur->value;
        }
        cur = cur->next;
    }
    return new_nil();
}

Value set_attr(Value objVal, char* key, Value val) {
    if (objVal.type != VAL_OBJ) return val;
    Obj* obj = objVal.as.obj;
    Entry* cur = obj->fields;
    while (cur) {
        if (strcmp(cur->key, key) == 0) {
            cur->value = val;
            return val;
        }
        cur = cur->next;
    }
    Entry* new_entry = malloc(sizeof(Entry));
    new_entry->key = strdup(key);
    new_entry->value = val;
    new_entry->next = obj->fields;
    obj->fields = new_entry;
    return val;
}

// Helper para chamar função guardada em variável
Value call_value(Value f) {
    if (f.type == VAL_FN && f.as.fn) return f.as.fn();
    if (f.type == VAL_BOUND_METHOD) {
        Value receiver; receiver.type = VAL_OBJ; receiver.as.obj = f.as.bound->receiver;
        push(receiver);
        return f.as.bound->method();
    }
    fprintf(stderr, "Erro: Tentando chamar algo que nao e funcao.\n");
    return new_nil();
}