/* * ARQUIVO: ast/ast.h
 * Conteúdo completo e modificado.
 */
#ifndef AST_H
#define AST_H

// Corrigido: O caminho correto para tipos.h
#include "../tabela/tipos.h" 

// Prototipos das novas funcoes construtoras
NoAST *criarNoNum(double value);
NoAST *criarNoString(char* value);
NoAST *criarNoBool(int value);
NoAST *criarNoNil(void);
NoAST *criarNoIdentifier(char* name);
NoAST *criarNoThis(void);

NoAST *criarNoOpUnario(int op, NoAST *operand);
NoAST *criarNoOpBinario(int op, NoAST *left, NoAST *right);
NoAST *criarNoOpLogico(int op, NoAST *left, NoAST *right);

NoAST *criarNoAssign(char* name, NoAST *value);
NoAST *criarNoCall(NoAST *target, NoAST *arguments);
NoAST *criarNoGetAttr(NoAST *object, char *name);
NoAST *criarNoSetAttr(NoAST *object, char *name, NoAST *value);

NoAST *criarNoVarDecl(char *name, NoAST *initializer);
NoAST *criarNoPrintStmt(NoAST *expression);
NoAST *criarNoExprStmt(NoAST *expression);
NoAST *criarNoBlock(NoAST *statements);
NoAST *criarNoIf(NoAST *condition, NoAST *then_branch, NoAST *else_branch);
NoAST *criarNoWhile(NoAST *condition, NoAST *body);
NoAST *criarNoFor(NoAST *init, NoAST *cond, NoAST *inc, NoAST *body);
NoAST *criarNoFunDecl(char *name, NoAST *params, NoAST *body);
NoAST *criarNoClassDecl(char *name, NoAST *methods);
NoAST *criarNoReturn(NoAST *expression);
NoAST *criarNoParam(char *name);

// Funções utilitárias
NoAST *anexarNoLista(NoAST *lista, NoAST *novo);
void imprimirAST(NoAST *raiz, int indent);
void liberarAST(NoAST *raiz);

#endif