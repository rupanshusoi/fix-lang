import os
import subprocess as sp
import argparse

PROC_STR = 'thunk: tree:{} string:none file:build/test-fix.wasm uint32:1 uint32:1 string:{}'
VAR_STR = 'thunk: tree:5 string:none file:build/test-fix.wasm uint32:1 uint32:0 string:{}'

def call_fix(cmd):
  if os.uname()[1] == 'rootpi':
    print(cmd)
  else:
    print(cmd)
    # exit()
    sp.run('cmake --build build --parallel 256'.split())
    sp.run(cmd.split())

def lex(line):
  return line.replace('(', ' ( ').replace(')', ' ) ').split()

def parse(lexed):
  i = 0
  S = []
  assert lexed[i] == '(', 'ill-formed expression'
  while i < len(lexed) - 1:
    i += 1
    if lexed[i] == '(':
      new_S, new_i = parse(lexed[i:])
      i += new_i
      S.append(new_S)
    elif lexed[i] == ')':
      return S, i
    else:
      S.append(lexed[i])
  assert False, 'ill-formed expression'

def fix_eval(S, env):
  if type(S) == list:
    eval_str = PROC_STR.format(len(S) + 4, S[0])
    for s in S[1:]:
      eval_str += ' ' + fix_eval(s, env)
    return eval_str
  elif type(S) == str:
    try:
      return VAR_STR.format(S)
    except:
      return fix_eval(env[S], env)
  else:
    assert False

def repl():
  global_env = {}
  while True:
    source = input('fix> ')
    lexed = lex(source)
    parsed, _ = parse(lexed)
    fix_eval(parsed, global_env)

def main(inline):
  if inline is None:
    with open('code.fix', 'r') as file:
      source = file.readlines()
  else:
    source = [inline]

  lexed = list(map(lex, source))
  parsed = list(map(lambda tokens: parse(tokens)[0], lexed))
  parsed = parsed[0]
  
  global_env = {}
  eval_str = fix_eval(parsed, global_env)
  eval_str = "/home/rsoi/fix/build/src/tester/stateless-tester " + eval_str
  call_fix(eval_str)

if __name__ == '__main__':
  parser = argparse.ArgumentParser()
  parser.add_argument('-r', '--repl', default=False, action=argparse.BooleanOptionalAction)
  parser.add_argument('-i', '--inline', type=str)
  args = parser.parse_args()
  if args.repl: repl()
  else: main(args.inline)
