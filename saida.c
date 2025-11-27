#include "backend/c_runtime.c"

int main() {
  Value i_0, t0, t1, t2, a_1, b_2, t3, t4, t5, t6, t7, t8, t9, t10, x_3, t11, t12, y_4, t13, t14, t15, t16, k_5, t17, t18, m_6, t19, t20, t21, t22, total_7, cont_8, t23, t24, t25, t26, n_9, t1_10, t2_11, t27, nextTerm_12, t28, t29, j_13, t30, t31, t32;
  print_value(new_string("--- 1. While Decremental Simples ---"));
  i_0 = new_number(3.00);
  L0:
  ;
  t0 = op_gt(i_0, new_number(0.00));
  if (!is_truthy(t0)) goto L1;
  t1 = op_add(new_string("Contagem regressiva: "), i_0);
  print_value(t1);
  t2 = op_sub(i_0, new_number(1.00));
  i_0 = t2;
  goto L0;
  L1:
  ;
  print_value(new_string("Decolagem!"));
  print_value(new_string(""));
  print_value(new_string("--- 2. While com Condicao Composta (AND/OR) ---"));
  a_1 = new_number(0.00);
  b_2 = new_number(10.00);
  L2:
  ;
  t3 = op_lt(a_1, new_number(3.00));
  t4 = op_gt(b_2, new_number(7.00));
  t5 = op_and(t3, t4);
  if (!is_truthy(t5)) goto L3;
  t6 = op_add(new_string("a="), a_1);
  t7 = op_add(t6, new_string(", b="));
  t8 = op_add(t7, b_2);
  print_value(t8);
  t9 = op_add(a_1, new_number(1.00));
  a_1 = t9;
  t10 = op_sub(b_2, new_number(1.00));
  b_2 = t10;
  goto L2;
  L3:
  ;
  print_value(new_string(""));
  print_value(new_string("--- 3. While Aninhado (Labels unicos) ---"));
  x_3 = new_number(1.00);
  L4:
  ;
  t11 = op_le(x_3, new_number(2.00));
  if (!is_truthy(t11)) goto L5;
  t12 = op_add(new_string("Externo x: "), x_3);
  print_value(t12);
  y_4 = new_number(1.00);
  L6:
  ;
  t13 = op_le(y_4, new_number(2.00));
  if (!is_truthy(t13)) goto L7;
  t14 = op_add(new_string("  Interno y: "), y_4);
  print_value(t14);
  t15 = op_add(y_4, new_number(1.00));
  y_4 = t15;
  goto L6;
  L7:
  ;
  t16 = op_add(x_3, new_number(1.00));
  x_3 = t16;
  goto L4;
  L5:
  ;
  print_value(new_string(""));
  print_value(new_string("--- 4. While dentro de For ---"));
  k_5 = new_number(1.00);
  L8:
  ;
  t17 = op_le(k_5, new_number(2.00));
  if (!is_truthy(t17)) goto L9;
  t18 = op_add(new_string("For k: "), k_5);
  print_value(t18);
  m_6 = new_number(3.00);
  L10:
  ;
  t19 = op_gt(m_6, new_number(1.00));
  if (!is_truthy(t19)) goto L11;
  t20 = op_add(new_string("  While m: "), m_6);
  print_value(t20);
  t21 = op_sub(m_6, new_number(1.00));
  m_6 = t21;
  goto L10;
  L11:
  ;
  t22 = op_add(k_5, new_number(1.00));
  k_5 = t22;
  goto L8;
  L9:
  ;
  print_value(new_string(""));
  print_value(new_string("--- 5. Modificando Global dentro do While ---"));
  total_7 = new_number(0.00);
  cont_8 = new_number(0.00);
  L12:
  ;
  t23 = op_lt(cont_8, new_number(5.00));
  if (!is_truthy(t23)) goto L13;
  t24 = op_add(total_7, new_number(10.00));
  total_7 = t24;
  t25 = op_add(cont_8, new_number(1.00));
  cont_8 = t25;
  goto L12;
  L13:
  ;
  t26 = op_add(new_string("Total calculado (5 * 10): "), total_7);
  print_value(t26);
  print_value(new_string(""));
  print_value(new_string("--- 6. Fibonacci com While ---"));
  n_9 = new_number(10.00);
  t1_10 = new_number(0.00);
  t2_11 = new_number(1.00);
  t27 = op_add(t1_10, t2_11);
  nextTerm_12 = t27;
  t28 = op_add(new_string("Fib ate "), n_9);
  t29 = op_add(t28, new_string(":"));
  print_value(t29);
  print_value(t1_10);
  print_value(t2_11);
  j_13 = new_number(3.00);
  L14:
  ;
  t30 = op_le(j_13, n_9);
  if (!is_truthy(t30)) goto L15;
  print_value(nextTerm_12);
  t1_10 = t2_11;
  t2_11 = nextTerm_12;
  t31 = op_add(t1_10, t2_11);
  nextTerm_12 = t31;
  t32 = op_add(j_13, new_number(1.00));
  j_13 = t32;
  goto L14;
  L15:
  ;
  print_value(new_string("--- Fim dos Testes de Loop ---"));
  return 0;
}
