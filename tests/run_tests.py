#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import subprocess, sys, os, difflib, pathlib

ROOT = pathlib.Path(__file__).resolve().parents[1]
LOX_TESTS_DIR = ROOT / "tests" / "lox_tests"  # Diretório atualizado para os arquivos de testes Lox
EXPECTED_TESTS_DIR = ROOT / "tests" / "expected_tests"  # Diretório atualizado para os arquivos de expectativa

def pick_compiler() -> pathlib.Path:
    env = os.environ.get("COMPILER")
    if env:
        p = pathlib.Path(env)
        if p.exists():
            return p.resolve()
        print(f"[ERRO] COMPILER não encontrado: {p}", file=sys.stderr)
        sys.exit(127)
    if os.name == "nt":
        for cand in [ROOT / "compilador.exe", ROOT / "compilador"]:
            if cand.exists():
                return cand.resolve()
    else:
        cand = ROOT / "compilador"
        if cand.exists():
            return cand.resolve()
    print("[ERRO] não achei o binário do compilador; rode 'make' ou defina COMPILER", file=sys.stderr)
    sys.exit(127)

def norm(s: str) -> str:
    if s is None:
        return ""
    s = s.replace("\r\n", "\n").replace("\r", "\n")
    lines = [ln.rstrip() for ln in s.split("\n")]
    out = "\n".join(lines)
    if out and not out.endswith("\n"):
        out += "\n"
    return out

def expected_for(lox: pathlib.Path) -> pathlib.Path:
    name = lox.name
    # Atualiza o caminho para buscar os arquivos de expectativa em 'expected_tests'
    if not name.startswith("teste") or not name.endswith(".lox"):
        return EXPECTED_TESTS_DIR / (lox.stem + ".out.expected")
    suffix = name[len("teste"):-len(".lox")]
    return EXPECTED_TESTS_DIR / f"testes{suffix}.lox.expected"

def discover():
    all_lox = sorted(LOX_TESTS_DIR.glob("teste*.lox"))  # Atualiza para ler os testes de 'lox_tests'
    only = os.environ.get("ONLY")
    if not only:
        return all_lox
    prefs = [p.strip() for p in only.split(",") if p.strip()]
    return [t for t in all_lox if any(t.name.startswith(p) for p in prefs)]

def main():
    compiler = pick_compiler()
    parse_only = os.environ.get("PARSE_ONLY") == "1"
    error_strict = os.environ.get("ERROR_STRICT", "1") == "1"
    tests = discover()
    if not tests:
        print("[AVISO] Nenhum 'teste*.lox' em 'lox_tests/'.")
        sys.exit(0)
    print(f"Usando compilador: {compiler}")
    total = len(tests)
    fails_total = 0
    print(f"Descobertos {total} testes em 'lox_tests/'.\n")
    for lox in tests:
        exp = expected_for(lox)
        proc = subprocess.run([str(compiler), str(lox)], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        out = norm(proc.stdout)
        code = proc.returncode
        is_error_case = ("erros" in lox.stem)
        failures = []
        if parse_only and not is_error_case:
            if code != 0:
                failures.append(("exit", f"esperado=0 obtido={code}"))
        elif is_error_case and not error_strict:
            if code == 0:
                failures.append(("exit", "esperado != 0 obtido=0"))
        else:
            if exp.exists():
                want = norm(exp.read_text(encoding="utf-8", errors="replace"))
                if out != want:
                    diff = "\n".join(difflib.unified_diff(
                        want.splitlines(), out.splitlines(),
                        fromfile=str(exp), tofile=f"(stdout {lox.name})", lineterm=""
                    ))
                    failures.append(("stdout", diff))
            else:
                if not is_error_case and code != 0:
                    failures.append(("exit", f"esperado=0 obtido={code}"))
        label = lox.relative_to(ROOT)
        if failures:
            print(f"[FAIL] {label}")
            fails_total += 1
            for kind, detail in failures:
                print(f"  └─ {kind}:")
                if detail.strip().startswith("---"):
                    print(detail)
                else:
                    print("-----")
                    print(detail)
                    print("-----")
        else:
            extra = []
            if parse_only and not is_error_case:
                extra.append("parse ok")
            if is_error_case and not error_strict:
                extra.append("erro detectado")
            if not exp.exists():
                extra.append("sem expected")
            suffix = f"  ({'; '.join(extra)})" if extra else ""
            print(f"[ OK ] {label}{suffix}")
    print("\nResumo:")
    print(f"  Total: {total}")
    print(f"  Pass:  {total - fails_total}")
    print(f"  Fail:  {fails_total}")
    sys.exit(fails_total)

if __name__ == "__main__":
    main()