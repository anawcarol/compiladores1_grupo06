#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import os, sys, pathlib, subprocess

ROOT = pathlib.Path(__file__).resolve().parents[1]
LOX_TESTS_DIR = ROOT / "tests" / "lox_tests"  # Diretório de testes lox
EXPECTED_TESTS_DIR = ROOT / "tests" / "expected_tests"  # Diretório para salvar os arquivos .lox.expected

# Garante que o diretório de expectativas existe
EXPECTED_TESTS_DIR.mkdir(parents=True, exist_ok=True)

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
    # Gera o caminho correto para os arquivos .lox.expected dentro de 'expected_tests'
    name = lox.name
    if not name.startswith("teste") or not name.endswith(".lox"):
        return EXPECTED_TESTS_DIR / (lox.stem + ".out.expected")
    suffix = name[len("teste"):-len(".lox")]
    return EXPECTED_TESTS_DIR / f"testes{suffix}.lox.expected"

def discover():
    only = os.environ.get("ONLY")
    tests = sorted(LOX_TESTS_DIR.glob("teste*.lox"))
    if not only:
        return tests
    prefs = [p.strip() for p in only.split(",") if p.strip()]
    return [t for t in tests if any(t.name.startswith(p) for p in prefs)]

def main():
    compiler = pick_compiler()
    tests = discover()
    if not tests:
        print("[AVISO] Nenhum 'teste*.lox' encontrado em 'lox_tests/'.")
        return
    print(f"Usando compilador: {compiler}")
    for lox in tests:
        exp = expected_for(lox)
        proc = subprocess.run([str(compiler), str(lox)], stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
        out = norm(proc.stdout)
        exp.write_text(out, encoding="utf-8")
        print(f"[ok] {lox.name} -> {exp.name} ({len(out.encode('utf-8'))} bytes)")
    print("Feito.")

if __name__ == "__main__":
    main()