import subprocess
import sys
import os
import difflib
import pathlib

ROOT = pathlib.Path(__file__).resolve().parents[1]
LOX_TESTS_DIR = ROOT / "tests" / "lox_tests"  # Diretório dos arquivos de testes Lox
EXPECTED_TESTS_DIR = ROOT / "tests" / "expected_tests"  # Diretório dos arquivos de expectativas

def pick_compiler() -> pathlib.Path:
    """Seleciona o compilador a ser utilizado, considerando a variável de ambiente COMPILER."""
    compiler_path = os.environ.get("COMPILER")
    if compiler_path:
        p = pathlib.Path(compiler_path)
        if p.exists():
            return p.resolve()
        print(f"[ERRO] COMPILER não encontrado: {p}", file=sys.stderr)
        sys.exit(127)

    # Tenta encontrar o compilador nos caminhos predefinidos
    candidate_paths = [ROOT / "compilador.exe", ROOT / "compilador"]
    if os.name == "nt":
        for path in candidate_paths:
            if path.exists():
                return path.resolve()
    elif candidate_paths[1].exists():  # Para sistemas Unix-like
        return candidate_paths[1].resolve()

    print("[ERRO] Compilador não encontrado; rode 'make' ou defina a variável de ambiente COMPILER.", file=sys.stderr)
    sys.exit(127)

def norm(s: str) -> str:
    """Normaliza a string, garantindo uma quebra de linha consistente."""
    if s is None:
        return ""
    s = s.replace("\r\n", "\n").replace("\r", "\n")
    lines = [line.rstrip() for line in s.split("\n")]
    normalized = "\n".join(lines)
    return normalized if normalized.endswith("\n") else normalized + "\n"

def expected_for(lox: pathlib.Path) -> pathlib.Path:
    """Determina o caminho do arquivo de expectativa correspondente ao teste."""
    name = lox.name
    if not name.startswith("teste") or not name.endswith(".lox"):
        return EXPECTED_TESTS_DIR / f"{lox.stem}.out.expected"
    
    suffix = name[len("teste"):-len(".lox")]
    return EXPECTED_TESTS_DIR / f"testes{suffix}.lox.expected"

def discover() -> list[pathlib.Path]:
    """Descobre os arquivos de teste a serem executados, considerando a variável de ambiente ONLY."""
    all_lox = sorted(LOX_TESTS_DIR.glob("teste*.lox"))
    only = os.environ.get("ONLY")
    if not only:
        return all_lox

    prefs = [pref.strip() for pref in only.split(",") if pref.strip()]
    return [test for test in all_lox if any(test.name.startswith(pref) for pref in prefs)]

def run_test(lox: pathlib.Path, compiler: pathlib.Path) -> str:
    """Executa um teste Lox e retorna a saída gerada (stdout e stderr)."""
    proc = subprocess.run([str(compiler), str(lox)], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    
    # Captura a saída tanto do stdout quanto do stderr
    output = norm(proc.stdout)
    error_output = norm(proc.stderr)
    
    # Se houver erro no stderr, podemos juntar a saída de erro com a saída padrão
    if error_output:
        output += "\n[ERRO] " + error_output

    # Se o compilador gerar 'saida.c', tentamos compilá-lo e executá-lo
    if "Codigo gerado em 'saida.c'" in output:
        print(f"[INFO] Código gerado em 'saida.c'. Compilando e executando...")

        # Compila o código C gerado
        compile_proc = subprocess.run(["gcc", "saida.c", "-o", "saida"], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        if compile_proc.returncode != 0:
            output += "\n[ERRO] Falha na compilação do código C gerado."
        else:
            # Executa o código compilado
            exec_proc = subprocess.run(["./saida"], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
            output += norm(exec_proc.stdout)
            if exec_proc.stderr:
                output += "\n[ERRO] " + exec_proc.stderr

    return output  # Retornando apenas a saída gerada

def display_results(lox: pathlib.Path, output: str, expected_output: pathlib.Path):
    """Exibe os resultados do teste, incluindo falhas e saídas esperadas."""
    label = lox.relative_to(ROOT)
    failures = []
    if expected_output.exists():
        expected_content = norm(expected_output.read_text(encoding="utf-8", errors="replace"))
        if output != expected_content:
            diff = "\n".join(difflib.unified_diff(
                expected_content.splitlines(), output.splitlines(),
                fromfile=str(expected_output), tofile=f"(stdout {lox.name})", lineterm=""
            ))
            failures.append(("stdout", diff))
    else:
        failures.append(("expected_output", "Arquivo esperado não encontrado"))

    if failures:
        print(f"[FAIL] {label}")
        for kind, detail in failures:
            print(f"  └─ {kind}:")
            if detail.strip().startswith("---"):
                print(detail)
            else:
                print("-----")
                print(detail)
                print("-----")
    else:
        print(f"[ OK ] {label}")

def main():
    compiler = pick_compiler()
    parse_only = os.environ.get("PARSE_ONLY") == "1"
    error_strict = os.environ.get("ERROR_STRICT", "1") == "1"
    tests = discover()

    if not tests:
        print("[AVISO] Nenhum 'teste*.lox' encontrado em 'lox_tests/'.")
        sys.exit(0)

    print(f"Usando compilador: {compiler}")
    total = len(tests)
    fails_total = 0
    print(f"Descobertos {total} testes em 'lox_tests/'.\n")

    for lox in tests:
        output = run_test(lox, compiler)
        expected_output = expected_for(lox)
        display_results(lox, output, expected_output)

    print("\nResumo:")
    print(f"  Total: {total}")
    print(f"  Fail:  {fails_total}")
    sys.exit(fails_total)

if __name__ == "__main__":
    main()
