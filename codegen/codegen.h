#ifndef CODEGEN_H
#define CODEGEN_H

#include "../ast/ast.h"
#include "tac.h"

// Variáveis globais para contadores
extern int tempCount;
extern int labelCount;

TacNode* gerarCodigo(NoAST *ast);

#endif