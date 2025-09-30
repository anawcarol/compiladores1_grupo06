#include <CUnit/Basic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_OUTPUT_SIZE 1024

void test_compilador_output(const char* arquivo_lox, const char* output_esperado) {
    char comando[512];
    snprintf(comando, sizeof(comando), "./compilador %s > output.txt", arquivo_lox);
    system(comando);

    FILE* file = fopen("output.txt", "r");
    char output[MAX_OUTPUT_SIZE];
    
    if (file == NULL) {
        CU_FAIL("Erro ao abrir o arquivo de saída.");
        return;
    }
    fgets(output, sizeof(output), file);
    fclose(file);

    CU_ASSERT_STRING_EQUAL(output, output_esperado);
}

void test_funcionalidade_lox() {
    test_compilador_output("testes/teste.lox", "X não é maior que Y\n30\n-10\n11\n12\n2\n10\nfalse\ntrue\ntrue and false\ntrue\nX é maior que Y\nHello, world!\n15\nfalse\nfalse");

    test_compilador_output("testes/teste2.lox", "Olá, João!\n0\n1\n2\n3\n4\nx é maior que 5\nModelo: Fusca, Cor: Azul\n16");

    test_compilador_output("testes/teste3.lox", "--- Iniciando Testes Abrangentes ---\n--- Testando Literais e Expressões ---\n123\n98.76\nOlá, Lox!\ntrue\nfalse\nnil\n7\n9\n0\n-3\ntrue\nfalse\ntrue\nfalse\ntrue\nCondição verdadeira\nDeve acontecer (OR)\nwhile: 0\nwhile: 1\nwhile: 2\nfor: 0\nfor: 1\nfor: 2\nFibonacci de 7 é: 13\n1\n2\nAssando um bolo de chocolate.\nCobertura: granulado");

    test_compilador_output("testes/teste_erros.lox", "Erro: Caractere '#' não reconhecido.\nErro: String não terminada.\nErro: Número com múltiplos pontos decimais.\nErro: Faltando operando após o '+'.\nErro: Faltando operando antes do '*'.\nErro: Parêntese de agrupamento não fechado.\nErro: O lado esquerdo de uma atribuição deve ser uma variável (l-value).\nErro: Faltando ponto e vírgula.\nErro: 'else' sem um 'if' precedente.\nErro: 'return' só pode ser usado dentro de uma função.\nErro: Faltando a condição entre parênteses.\nErro: Sintaxe da declaração de variável incompleta dentro do for.\nErro: Nome da função faltando em uma declaração de alto nível.\nErro: Parâmetros devem ser identificadores, não literais.\nErro: 'this' não pode ser usado fora do método de uma classe.\nErro: Esperado um nome de propriedade após o '.'");
}

int main() {
    CU_initialize_registry();

    CU_pSuite pSuite = CU_add_suite("Testes do Compilador Lox", 0, 0);

    CU_add_test(pSuite, "Testar Compilador Lox", test_funcionalidade_lox);

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();

    return 0;
}
