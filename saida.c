#include "backend/c_runtime.c"

Value Pessoa_init();
Value Pessoa_digaOla();
Value executar();
Value Contador_init();
Value Contador_inc();
Value Pessoa() {
  Value inst = new_object("Pessoa");
  set_attr(inst, "init", new_native(Pessoa_init));
  set_attr(inst, "digaOla", new_native(Pessoa_digaOla));
  push(inst);
  Pessoa_init();
  return inst;
}

Value Contador() {
  Value inst = new_object("Contador");
  set_attr(inst, "init", new_native(Contador_init));
  set_attr(inst, "inc", new_native(Contador_inc));
  push(inst);
  Contador_init();
  return inst;
}

Value Pessoa_init() {
  Value this, nome_0;
  this = pop();
  nome_0 = pop();
  set_attr(this, "nome", nome_0);
  return new_nil();
  return new_nil();
}

Value Pessoa_digaOla() {
  Value this, t0, t1;
  this = pop();
  t0 = get_attr(this, "nome");
  t1 = op_add(new_string("Ola, eu sou "), t0);
  print_value(t1);
  return new_nil();
  return new_nil();
}

Value executar() {
  Value funcao_1, t2;
  funcao_1 = pop();
  print_value(new_string("Executando callback..."));
  t2 = call_value(funcao_1);
  return new_nil();
  return new_nil();
}

Value Contador_init() {
  Value this;
  this = pop();
  set_attr(this, "val", new_number(0.00));
  return new_nil();
  return new_nil();
}

Value Contador_inc() {
  Value this, t3, t4, t5;
  this = pop();
  t3 = get_attr(this, "val");
  t4 = op_add(t3, new_number(1.00));
  set_attr(this, "val", t4);
  t5 = get_attr(this, "val");
  print_value(t5);
  return new_nil();
  return new_nil();
}

int main() {
  Value t6, p_2, t7, t8, t9, f_3, t10, t11, t12, t13, c_4, t14, aumentar_5, t15, t16, t17, t18;
  push(new_string("Maria"));
  t6 = Pessoa();
  p_2 = t6;
  print_value(new_string("1. Chamada direta (obj.metodo):"));
  t7 = get_attr(p_2, "digaOla");
  t8 = call_value(t7);
  print_value(new_string(""));
  print_value(new_string("2. Metodo ligado a variavel (var f = obj.metodo):"));
  t9 = get_attr(p_2, "digaOla");
  f_3 = t9;
  t10 = call_value(f_3);
  print_value(new_string(""));
  print_value(new_string("3. Metodo passado como callback:"));
  t11 = get_attr(p_2, "digaOla");
  push(t11);
  t12 = executar();
  print_value(new_string(""));
  print_value(new_string("4. Teste de Sobrescrita de 'this' (Seguranca):"));
  t13 = Contador();
  c_4 = t13;
  t14 = get_attr(c_4, "inc");
  aumentar_5 = t14;
  t15 = call_value(aumentar_5);
  t16 = call_value(aumentar_5);
  t17 = get_attr(c_4, "inc");
  t18 = call_value(t17);
  return 0;
}
