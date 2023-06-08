import os
import subprocess as sp
import argparse

PROC_STR = 'tree:{} string:none file:build/test-fix.wasm uint32:1 uint32:1 uint32:0'
VAR_STR = 'tree:6 string:none file:build/test-fix.wasm uint32:1 uint32:0 uint32:0 string:{}'

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
    eval_str = PROC_STR.format(len(S) + 5)
    for s in S:
      eval_str += ' ' + fix_eval(s, env)
    return eval_str
  elif type(S) == str:
    return VAR_STR.format(S)
  else:
    assert False

def repl():
  global_env = {}
  while True:
    source = input('fix> ')
    lexed = lex(source)
    parsed, _ = parse(lexed)
    fix_eval(parsed, global_env)

def add_blobs(eval_str):
  wasm_blobs = ['addblob.wasm', 'subblob.wasm']  
  location = '/home/rsoi/fix/build/testing/wasm-examples/'
  rstr = 'tree:2 tree:{} '.format(len(wasm_blobs))
  rstr += ' '.join(list(map(lambda x: 'string:' + x, wasm_blobs)))
  rstr += ' tree:{} '.format(len(wasm_blobs))
  rstr += ' '.join(list(map(lambda x: 'file:' + location + x, wasm_blobs)))
  return eval_str.replace('uint32:0', rstr, 1)

def main(inline):
  if inline is None:
    with open('code.fix', 'r') as file:
      source = file.read()
  else:
    source = inline

  source = source.replace('\n', ' ').replace('\t', ' ')
  tokens = lex(source)
  parsed, _ = parse(tokens)

  global_env = {}
  eval_str = fix_eval(parsed, global_env)
  eval_str = "/home/rsoi/fix/build/src/tester/stateless-tester " + eval_str
  eval_str = add_blobs(eval_str)
  call_fix(eval_str)

if __name__ == '__main__':
  parser = argparse.ArgumentParser()
  parser.add_argument('-r', '--repl', default=False, action=argparse.BooleanOptionalAction)
  parser.add_argument('-i', '--inline', type=str)
  args = parser.parse_args()
  if args.repl: repl()
  else: main(args.inline)
