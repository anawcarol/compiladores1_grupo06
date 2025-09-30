#include <CUnit/Basic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_OUTPUT_SIZE 1024

void test_compilador_output(const char* arquivo_lox, const char* output_esperado) {
    // Comando para rodar o compilador no arquivo Lox
    char comando[512];
    snprintf(comando, sizeof(comando), "./compilador %s > output.txt", arquivo_lox);

    // Rodando o compilador
    system(comando);

    // Abrindo o arquivo de saída
    FILE* file = fopen("output.txt", "r");
    char output[MAX_OUTPUT_SIZE];
    
    if (file == NULL) {
        CU_FAIL("Erro ao abrir o arquivo de saída.");
        return;
    }

    // Lendo a saída do compilador
    fgets(output, sizeof(output), file);
    fclose(file);

    // Verificando se a saída gerada corresponde à saída esperada
    CU_ASSERT_STRING_EQUAL(output, output_esperado);
}

// Função de teste principal
void test_funcionalidade_lox() {
    test_compilador_output("testes/teste.lox", "Saída esperada para teste.lox");
    test_compilador_output("testes/teste2.lox", "Saída esperada para teste2.lox");
    test_compilador_output("testes/teste3.lox", "Saída esperada para teste3.lox");
    test_compilador_output("testes/teste_erros.lox", "Erro esperado para teste_erros.lox");
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
