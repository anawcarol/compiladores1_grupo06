#include "backend_c.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_VARS 500
char *declaredVars[MAX_VARS];
int varCount = 0;

void clearVars() { varCount = 0; }
void addVar(char *name) { declaredVars[varCount++] = strdup(name); }
int isDeclared(char *name) {
    for(int i=0; i<varCount; i++) if(strcmp(declaredVars[i],name)==0) return 1;
    return 0;
}

char* resolve(char* op) {
    if (!op) return "new_nil()";
    if (strcmp(op, "POP_PARAM") == 0) return "pop()";
    
    if (op[0] == '\"') {
        static char buf[256]; sprintf(buf, "new_string(%s)", op); return buf;
    }
    if (isdigit(op[0]) || (op[0]=='-' && isdigit(op[1]))) {
        static char buf[64]; sprintf(buf, "new_number(%s)", op); return buf;
    }
    if (strcmp(op, "true")==0) return "new_bool(1)";
    if (strcmp(op, "false")==0) return "new_bool(0)";
    if (strcmp(op, "nil")==0) return "new_nil()";
    
    return op; 
}

void processarBloco(TacNode *start, TacNode *end) {
    TacNode *curr = start;
    while (curr != end) {
        // Pula labels de função (exceto labels de fluxo como L1, L2)
        if (curr->op == TAC_LABEL && curr->res[0] != 'L') { curr=curr->next; continue; }
        
        printf("  ");
        switch (curr->op) {
            case TAC_ADD: printf("Value %s = op_add(%s, %s);\n", curr->res, resolve(curr->arg1), resolve(curr->arg2)); break;
            case TAC_SUB: printf("Value %s = op_sub(%s, %s);\n", curr->res, resolve(curr->arg1), resolve(curr->arg2)); break;
            case TAC_MUL: printf("Value %s = op_mul(%s, %s);\n", curr->res, resolve(curr->arg1), resolve(curr->arg2)); break;
            case TAC_DIV: printf("Value %s = op_div(%s, %s);\n", curr->res, resolve(curr->arg1), resolve(curr->arg2)); break;
            case TAC_GT:  printf("Value %s = op_gt(%s, %s);\n", curr->res, resolve(curr->arg1), resolve(curr->arg2)); break;
            case TAC_LT:  printf("Value %s = op_lt(%s, %s);\n", curr->res, resolve(curr->arg1), resolve(curr->arg2)); break;
            case TAC_EQ:  printf("Value %s = op_eq(%s, %s);\n", curr->res, resolve(curr->arg1), resolve(curr->arg2)); break;
            
            case TAC_COPY: 
                if(!isDeclared(curr->res)) { printf("Value "); addVar(curr->res); }
                printf("%s = %s;\n", curr->res, resolve(curr->arg1)); 
                break;
            
            case TAC_PARAM: printf("push(%s);\n", resolve(curr->res)); break;
            
            case TAC_CALL: {
                if (strcmp(curr->arg1, "print") == 0) {
                     if (curr->arg2) printf("print_value(%s);\n", resolve(curr->arg2));
                     else printf("print_value(pop());\n");
                } else {
                    if (curr->res) {
                        if(!isDeclared(curr->res)) { printf("Value "); addVar(curr->res); }
                        if (isDeclared(curr->arg1)) 
                            printf("%s = call_value(%s);\n", curr->res, curr->arg1);
                        else
                            printf("%s = %s();\n", curr->res, curr->arg1);
                    } else {
                         if (isDeclared(curr->arg1)) printf("call_value(%s);\n", curr->arg1);
                         else printf("%s();\n", curr->arg1);
                    }
                }
                break;
            }
            
            case TAC_RETURN: printf("return %s;\n", curr->res ? resolve(curr->res) : "new_nil()"); break;
            
            case TAC_GET_ATTR:
                if(!isDeclared(curr->res)) { printf("Value "); addVar(curr->res); }
                printf("%s = get_attr(%s, %s);\n", curr->res, resolve(curr->arg1), curr->arg2); 
                break;
            case TAC_SET_ATTR:
                printf("set_attr(%s, %s, %s);\n", resolve(curr->res), curr->arg1, resolve(curr->arg2));
                break;
                
            case TAC_LABEL: printf("%s:\n", curr->res); break;
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

    // 1. Identifica classes e métodos (ignora 'main')
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
            printf("Value %s();\n", scan->res);
        }
        scan = scan->next;
    }
    
    // 2. Gera Construtores Automáticos
    for(int i=0; i<classCount; i++) {
        printf("Value %s() {\n", classes[i].name);
        printf("  Value inst = new_object(\"%s\");\n", classes[i].name);
        for(int j=0; j<classes[i].mCount; j++) {
            char* fullName = classes[i].methods[j];
            char* shortName = strchr(fullName, '_') + 1;
            printf("  set_attr(inst, \"%s\", new_native(%s));\n", shortName, fullName);
        }
        char initName[128]; sprintf(initName, "%s_init", classes[i].name);
        printf("  %s();\n", initName); 
        printf("  return inst;\n}\n\n");
    }

    // 3. Gerar Funções Normais (Para antes da main)
    TacNode* curr = head;
    while (curr) {
        if (curr->op == TAC_LABEL && curr->res[0] != 'L') {
            // SE FOR O LABEL MAIN, PARA A GERAÇÃO DE FUNÇÕES!
            if (strcmp(curr->res, "main") == 0) break;

            TacNode* end = curr->next;
            // Avança até o próximo label de função ou até o label 'main'
            while(end) {
                if (end->op == TAC_LABEL && end->res[0] != 'L') break;
                end = end->next;
            }
            
            printf("Value %s() {\n", curr->res);
            clearVars();
            processarBloco(curr->next, end);
            printf("}\n\n");
            curr = end; continue;
        }
        curr = curr->next;
    }

    // 4. Gerar Main (Começa a partir do label 'main')
    printf("int main() {\n");
    clearVars();
    
    // Encontra o ponto de início da main
    curr = head;
    while(curr) {
        if (curr->op == TAC_LABEL && strcmp(curr->res, "main") == 0) break;
        curr = curr->next;
    }

    // Processa do main até o fim da lista
    if (curr) {
        processarBloco(curr->next, NULL);
    }
    
    printf("  return 0;\n}\n");
}