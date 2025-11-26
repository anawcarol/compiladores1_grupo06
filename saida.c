#include "backend/c_runtime.c"

int main() {
  print_value(new_string("--- Inicio do Teste Simples ---"));
  Value a = new_number(10.00);
  Value b = new_number(5.00);
  Value t0 = op_add(a, b);
  Value soma = t0;
  print_value(soma);
  Value t1 = op_gt(soma, new_number(12.00));
  if (!is_truthy(t1)) goto L0;
  print_value(new_string("Soma eh maior que 12"));
  goto L1;
  L0:
  print_value(new_string("Soma eh menor que 12"));
  L1:
  print_value(new_string("--- Fim ---"));
  return 0;
}
