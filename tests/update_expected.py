#Gera/atualiza os arquivos testes*.lox.expected executando o compilador para cada teste*.lox e salvando o stdout normalizado como novo expected.
import os, sys, pathlib, subprocess

ROOT = pathlib.Path(__file__).resolve().parents[1]
TEST_DIR = ROOT / "testes"

def pick_compiler() -> pathlib.Path:
    env = os.environ.get("COMPILER")
    if env:
        p = pathlib.Path(env)
        if p.exists(): return p.resolve()
        print(f"[ERRO] COMPILER não encontrado: {p}", file=sys.stderr); sys.exit(127)
    if os.name == "nt":
        for cand in [ROOT / "compilador.exe", ROOT / "compilador"]:
            if cand.exists(): return cand.resolve()
    else:
        cand = ROOT / "compilador"
        if cand.exists(): return cand.resolve()
    print("[ERRO] não achei o binário do compilador; rode 'make' ou defina COMPILER", file=sys.stderr)
    sys.exit(127)

def norm(s: str) -> str:
    if s is None: return ""
    s = s.replace("\r\n","\n").replace("\r","\n")
    lines = [ln.rstrip() for ln in s.split("\n")]
    out = "\n".join(lines)
    if out and not out.endswith("\n"): out += "\n"
    return out

def expected_for(lox: pathlib.Path) -> pathlib.Path:
    name = lox.name
    if not name.startswith("teste") or not name.endswith(".lox"):
        return lox.with_suffix(".out.expected")
    suffix = name[len("teste"):-len(".lox")]
    return lox.with_name(f"testes{suffix}.lox.expected")

def discover():
    only = os.environ.get("ONLY")
    tests = sorted(TEST_DIR.glob("teste*.lox"))
    if not only: return tests
    prefs = [p.strip() for p in only.split(",") if p.strip()]
    return [t for t in tests if any(t.name.startswith(p) for p in prefs)]

def main():
    compiler = pick_compiler()
    tests = discover()
    if not tests:
        print("[AVISO] nenhum 'teste*.lox' encontrado em 'testes/'."); return
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
