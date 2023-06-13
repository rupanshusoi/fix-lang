import os
import subprocess as sp
import argparse

BOOT_STR = 'thunk: tree:3 string:none name:BAIZUnvLN502KN1e-55uw-AAAAAAAAAAkHH44D-KGwQ '
PROC_STR = 'tree:{} string:none ' + BOOT_STR + 'file:build/test-fix.wasm uint32:1 uint32:1 uint32:0'
VAR_STR = 'tree:6 string:none ' + BOOT_STR + ' file:build/test-fix.wasm uint32:1 uint32:0 uint32:0 string:{}'
BLOB_LOC = '/home/rsoi/fix-lang/build/'

def call_fix(cmd):
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

def fix_eval(S):
  if type(S) == list:
    eval_str = PROC_STR.format(len(S) + 5)
    for s in S:
      eval_str += ' ' + fix_eval(s)
    return eval_str
  elif type(S) == str:
    return VAR_STR.format(S)
  else:
    assert False

def add_blobs(eval_str):
  wasm_blobs = [x for x in os.listdir(BLOB_LOC) if x.endswith('.wasm')]
  rstr = 'tree:2 tree:{} '.format(len(wasm_blobs))
  rstr += ' '.join(list(map(lambda x: 'string:' + x, wasm_blobs)))
  rstr += ' tree:{} '.format(len(wasm_blobs))
  rstr += ' '.join(list(map(lambda x: BOOT_STR + ' file:' + BLOB_LOC + x, wasm_blobs)))
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

  eval_str = fix_eval(parsed)
  eval_str = "/home/rsoi/fix/build/src/tester/stateless-tester " + eval_str
  eval_str = add_blobs(eval_str)
  call_fix(eval_str)

if __name__ == '__main__':
  parser = argparse.ArgumentParser()
  parser.add_argument('-i', '--inline', type=str)
  args = parser.parse_args()
  main(args.inline)
