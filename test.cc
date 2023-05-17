extern "C" {
#include "fixpoint_util.h"
}

extern void ro_mem_0_to_program_memory( int32_t program_offset, int32_t ro_offset, int32_t len )
__attribute( ( import_module( "util" ), import_name( "ro_mem_0_to_program_memory" ) ) );

extern void program_memory_to_rw_0( int32_t rw_offset, int32_t program_offset, int32_t len )
__attribute( ( import_module( "util" ), import_name( "program_memory_to_rw_0" ) ) );

#include "test.hh"

const vector<string> lex(const string& src)
{
  vector<string> tokens;
  size_t idx = 0;
  while (1) 
  {
    if (idx >= src.size()) break;
    else if (src[idx] == ' ') idx++;
    else if (src[idx] == '(' || src[idx] == ')')
    {   
      string tmp(1, src[idx]);
      tokens.push_back(tmp);
      idx++;
    }   
    else
    {   
      size_t tmp = idx;
      while (src[tmp] != ')' && src[tmp] != ' ')
        tmp++;
      string s = src.substr(idx, tmp - idx);
      tokens.push_back(s);
      idx = tmp;
    }   
  }   
  return tokens;
}

void test_lex(const vector<string>& tokens)
{
  for (auto& token: tokens)
  {
    cout << token << endl;
  }
}

pair<int, vector<Node>> _parse(const vector<string>& tokens)
{
  vector<Node> ast;
  size_t i = 0;
  while (i < tokens.size() - 1)
  {
    i++;
    if (tokens[i] ==  "(")
    {
      auto _pair = _parse(vector<string>(tokens.begin() + i, tokens.end()));
      i += _pair.first;
      Node node(_pair.second);
      ast.push_back(node);
    }
    else if (tokens[i] == ")")
    {
      return make_pair(i, ast);
    }
    else
    {
      Node node(tokens[i]);
      ast.push_back(node);
    }
  } 
  assert(false);
}

vector<vector<Node>> parse(const vector<string>& tokens)
{
  vector<vector<Node>> parsed_lines;
  size_t start = 0;
  while (1)
  {
    auto parsed = _parse(vector<string>(tokens.begin() + start, tokens.end()));
    parsed_lines.push_back(parsed.second);
    if (start + parsed.first + 1 < tokens.size())
    {
      start += parsed.first + 1;
    }
    else break;
  }
  return parsed_lines;
}

void _test_parse(const vector<Node> ast, int indent = 0)
{
  for (auto &s: ast)
  {
    if (s.is_list) _test_parse(s.list, indent + 2);
    else cout << string(indent, ' ') << s.arg << endl;
  }
}

void test_parse(const vector<vector<Node>> parsed_lines)
{
  for (auto &line: parsed_lines)
    _test_parse(line);
}

Value apply(ApplyCtx apply_ctx)
{
  auto proc = apply_ctx.proc;
  auto values = apply_ctx.values;

  if (proc == "+")
    return Value(values[0].get_int() + values[1].get_int());
  else if (proc == "-")
    return Value(values[0].get_int() - values[1].get_int());
  else if (proc == "*")
    return Value(values[0].get_int() * values[1].get_int());
  assert(false);
}

Value eval(EvalCtx ctx)
{
  auto ast = ctx.ast;
  auto env = ctx.env;

  if (!ast.is_list)
  {
    if (isdigit(ast.arg[0]))
      return Value(stoi(ast.arg));
    else
      return Value(env[ast.arg]);
  }
  
  vector<Value> vec;
  for (auto it = ast.list.begin() + 1; it != ast.list.end(); it++)
  {
    EvalCtx eval_ctx(*it, env);
    vec.push_back(eval(eval_ctx));
  }
  ApplyCtx apply_ctx(ast[0].arg, vec);
  return apply(apply_ctx);
}

void driver()
{
  const string src = "(* 3 (* (* 3 4) 7))";
  const auto tokens = lex(src);
  const auto lines = parse(tokens);

  const map<string, int> env;
  Value result;
  for (auto& line: lines)
  {
    Node wrapper(line);
    EvalCtx eval_ctx(wrapper, env);
    result = eval(eval_ctx);
  }
  // cout << result.get_int() << endl;
  fixpoint_unsafe_io(to_string(result.get_int()).c_str(), 5);
}

/* encode[0]: resource limits
   encode[1]: this program
   encode[2]: serialized state
*/
__attribute__(( export_name("_fixpoint_apply")))
externref _fixpoint_apply(externref encode)
{
/*
1. Read encode
2. Construct EvalCtx
3. Call eval(ctx)
  3.1 Construct encodes for eval of each arg
  3.2 Put these encodes into a tree
4. Call apply
  4.1 It always takes two ints and a string (proc)
*/

  driver();
  return create_blob_i32(93);
}

