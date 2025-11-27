#include "backend_c.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Lista para armazenar variáveis locais
#define MAX_LOCALS 500
char *localVars[MAX_LOCALS];
int localCount = 0;

// Lista para armazenar funções globais conhecidas
char *funcVars[MAX_LOCALS];
int funcCount = 0;

void resetLocals() { localCount = 0; }

int hasLocal(char* name) {
    for(int i=0; i<localCount; i++) if(strcmp(localVars[i], name)==0) return 1;
    return 0;
}

void addLocal(char* name) {
    if(hasLocal(name)) return;
    if(localCount < MAX_LOCALS) localVars[localCount++] = strdup(name);
}

// Rastreamento de funções
void addFunction(char* name) {
    for(int i=0; i<funcCount; i++) if(strcmp(funcVars[i], name)==0) return;
    if(funcCount < MAX_LOCALS) funcVars[funcCount++] = strdup(name);
}

int isFunction(char* name) {
    for(int i=0; i<funcCount; i++) if(strcmp(funcVars[i], name)==0) return 1;
    return 0;
}

// Helper para resolver operandos 
char* resolve(char* op) {
    if (!op) return "new_nil()";
    if (strcmp(op, "POP_PARAM") == 0) return "pop()";
    
    // Buffer rotativo para evitar sobrescrita em argumentos múltiplos
    static char buffers[4][256];
    static int buf_idx = 0;
    char* buf = buffers[buf_idx];
    buf_idx = (buf_idx + 1) % 4;

    if (op[0] == '\"') {
        sprintf(buf, "new_string(%s)", op); return buf;
    }
    if (isdigit(op[0]) || (op[0]=='-' && isdigit(op[1]))) {
        sprintf(buf, "new_number(%s)", op); return buf;
    }
    if (strcmp(op, "true")==0) return "new_bool(1)";
    if (strcmp(op, "false")==0) return "new_bool(0)";
    if (strcmp(op, "nil")==0) return "new_nil()";
    
    // Se for nome de função, embrulha em new_native
    if (isFunction(op)) {
        sprintf(buf, "new_native(%s)", op); return buf;
    }

    return op; 
}

// Imprimir declarações de variáveis no topo da função C
void declararVariaveis(TacNode *start, TacNode *end) {
    resetLocals();
    TacNode *curr = start;
    while (curr != end) {
        if (curr->res && 
            !isdigit(curr->res[0]) && 
            curr->res[0] != '\"' &&
            curr->op != TAC_LABEL &&
            curr->op != TAC_JUMP && 
            curr->op != TAC_JUMP_TRUE && 
            curr->op != TAC_JUMP_FALSE &&
            curr->op != TAC_PARAM &&
            curr->op != TAC_RETURN &&
            curr->op != TAC_SET_ATTR
           ) {
            addLocal(curr->res);
        }
        curr = curr->next;
    }
    
    if (localCount > 0) {
        printf("  Value ");
        for (int i = 0; i < localCount; i++) {
            printf("%s%s", localVars[i], (i < localCount-1) ? ", " : ";\n");
        }
    }
}

// Gerar o código lógico
void processarBloco(TacNode *start, TacNode *end) {
    declararVariaveis(start, end);

    TacNode *curr = start;
    while (curr != end) {
        if (curr->op == TAC_LABEL && curr->res[0] != 'L') { curr=curr->next; continue; } 
        
        printf("  ");
        switch (curr->op) {
            case TAC_ADD: printf("%s = op_add(%s, %s);\n", curr->res, resolve(curr->arg1), resolve(curr->arg2)); break;
            case TAC_SUB: printf("%s = op_sub(%s, %s);\n", curr->res, resolve(curr->arg1), resolve(curr->arg2)); break;
            case TAC_MUL: printf("%s = op_mul(%s, %s);\n", curr->res, resolve(curr->arg1), resolve(curr->arg2)); break;
            case TAC_DIV: printf("%s = op_div(%s, %s);\n", curr->res, resolve(curr->arg1), resolve(curr->arg2)); break;
            case TAC_GT:  printf("%s = op_gt(%s, %s);\n", curr->res, resolve(curr->arg1), resolve(curr->arg2)); break;
            case TAC_LT:  printf("%s = op_lt(%s, %s);\n", curr->res, resolve(curr->arg1), resolve(curr->arg2)); break;
            case TAC_GE:  printf("%s = op_ge(%s, %s);\n", curr->res, resolve(curr->arg1), resolve(curr->arg2)); break;
            case TAC_LE:  printf("%s = op_le(%s, %s);\n", curr->res, resolve(curr->arg1), resolve(curr->arg2)); break;
            case TAC_EQ:  printf("%s = op_eq(%s, %s);\n", curr->res, resolve(curr->arg1), resolve(curr->arg2)); break;
            case TAC_NEQ: printf("%s = op_neq(%s, %s);\n", curr->res, resolve(curr->arg1), resolve(curr->arg2)); break;
            case TAC_AND: printf("%s = op_and(%s, %s);\n", curr->res, resolve(curr->arg1), resolve(curr->arg2)); break;
            case TAC_OR:  printf("%s = op_or(%s, %s);\n", curr->res, resolve(curr->arg1), resolve(curr->arg2)); break;
            case TAC_NOT: printf("%s = op_not(%s);\n", curr->res, resolve(curr->arg1)); break;
            case TAC_NEG: printf("%s = op_sub(new_number(0), %s);\n", curr->res, resolve(curr->arg1)); break;
            case TAC_COPY: printf("%s = %s;\n", curr->res, resolve(curr->arg1)); break;
            case TAC_PARAM: printf("push(%s);\n", resolve(curr->res)); break;
            
            case TAC_CALL: {
                if (strcmp(curr->arg1, "print") == 0) {
                     if (curr->arg2) printf("print_value(%s);\n", resolve(curr->arg2));
                     else printf("print_value(pop());\n");
                } else {
                    if (curr->res) {
                        if (hasLocal(curr->arg1)) printf("%s = call_value(%s);\n", curr->res, curr->arg1);
                        else printf("%s = %s();\n", curr->res, curr->arg1);
                    } else {
                         if (hasLocal(curr->arg1)) printf("call_value(%s);\n", curr->arg1);
                         else printf("%s();\n", curr->arg1);
                    }
                }
                break;
            }
            case TAC_RETURN: printf("return %s;\n", curr->res ? resolve(curr->res) : "new_nil()"); break;
            case TAC_GET_ATTR: printf("%s = get_attr(%s, %s);\n", curr->res, resolve(curr->arg1), curr->arg2); break;
            case TAC_SET_ATTR: printf("set_attr(%s, %s, %s);\n", resolve(curr->res), curr->arg1, resolve(curr->arg2)); break;
            case TAC_LABEL: printf("%s:\n  ;\n", curr->res); break;
            case TAC_JUMP: printf("goto %s;\n", curr->res); break;
            case TAC_JUMP_FALSE: printf("if (!is_truthy(%s)) goto %s;\n", resolve(curr->arg1), curr->res); break;
            default: break;
        }
        curr = curr->next;
    }
}

// Estrutura auxiliar para classes
typedef struct { char name[64]; char methods[20][64]; int mCount; } ClassInfo;
ClassInfo classes[20];
int classCount = 0;

void registerMethod(char* className, char* methodName) {
    for(int i=0; i<classCount; i++) {
        if(strcmp(classes[i].name, className)==0) {
            strcpy(classes[i].methods[classes[i].mCount++], methodName);
            return;
        }
    }
    strcpy(classes[classCount].name, className);
    strcpy(classes[classCount].methods[0], methodName);
    classes[classCount].mCount = 1;
    classCount++;
}

void gerarCodigoC(TacNode* head) {
    printf("#include \"backend/c_runtime.c\"\n\n");

    // Identifica classes e métodos (protótipos)
    TacNode* scan = head;
    while(scan) {
        if(scan->op == TAC_LABEL && scan->res[0]!='L') {
            if (strcmp(scan->res, "main") == 0) { scan = scan->next; continue; }
            char *underscore = strchr(scan->res, '_');
            if(underscore) {
                char cls[64];
                int len = underscore - scan->res;
                strncpy(cls, scan->res, len); cls[len] = '\0';
                registerMethod(cls, scan->res);
            }
            addFunction(scan->res);
            printf("Value %s();\n", scan->res);
        }
        scan = scan->next;
    }
    
    // Adicionado push(inst)
    for(int i=0; i<classCount; i++) {
        printf("Value %s() {\n", classes[i].name);
        printf("  Value inst = new_object(\"%s\");\n", classes[i].name);
        for(int j=0; j<classes[i].mCount; j++) {
            char* fullName = classes[i].methods[j];
            char* shortName = strchr(fullName, '_') + 1;
            printf("  set_attr(inst, \"%s\", new_native(%s));\n", shortName, fullName);
        }
        printf("  push(inst);\n"); // <--- AQUI: Passa 'this' para o init
        char initName[128]; sprintf(initName, "%s_init", classes[i].name);
        printf("  %s();\n", initName); 
        printf("  return inst;\n}\n\n");
    }

    // 3. Funções e Main
    TacNode* curr = head;
    while (curr) {
        if (curr->op == TAC_LABEL && curr->res[0] != 'L') {
            int isMain = (strcmp(curr->res, "main") == 0);
            if (isMain) printf("int main() {\n");
            else printf("Value %s() {\n", curr->res);
            
            TacNode* end = curr->next;
            while(end) {
                if (end->op == TAC_LABEL && end->res[0] != 'L') break;
                end = end->next;
            }
            processarBloco(curr->next, end);
            
            if (isMain) printf("  return 0;\n}\n");
            else printf("  return new_nil();\n}\n\n"); 
            curr = end; continue;
        }
        curr = curr->next;
    }
}