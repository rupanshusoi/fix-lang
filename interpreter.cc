#include "interpreter.hh"

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

// Value apply(ApplyCtx apply_ctx)
// {
//   auto proc = apply_ctx.proc;
//   auto values = apply_ctx.values;
// 
//   if (proc == "+")
//     return Value(values[0].get_int() + values[1].get_int());
//   else if (proc == "-")
//     return Value(values[0].get_int() - values[1].get_int());
//   else if (proc == "*")
//     return Value(values[0].get_int() * values[1].get_int());
//   assert(false);
// }

externref eval(Node ast)
{
  // if (!ast.is_list)
  // {
  //   if (isdigit(ast.arg[0]))
  //     return ApplyCtx(Value(stoi(ast.arg)));
  //   else
  //     return ApplyCtx(Value(env[ast.arg]));
  // }
  
  fio("G", 1);
  grow_rw_table_0(5, get_ro_table_0(0));
  set_rw_table_0(0, get_ro_table_0(0));
  set_rw_table_0(1, get_ro_table_0(1));

  grow_rw_mem_0_pages(1000);
  fio("H", 1);

  const char* op = "+";
  program_memory_to_rw_0(0, (int32_t)op, 1);
  set_rw_table_0(2, create_blob_rw_mem_0(1));

  fio("I", 1);
  // Serialize Node(ast[1])
  grow_rw_table_1(3, get_ro_table_0(0));
  set_rw_table_1(0, get_ro_table_0(0));
  set_rw_table_1(1, get_ro_table_0(1));
  Node node = ast[1];
  fio("1", 1);
  stringstream s;
  {
    cereal::XMLOutputArchive oarchive(s);
    oarchive(node);
  }
  fio("2", 1);
  string ss = s.str();
  const char *ss_ptr = ss.c_str();
  grow_rw_mem_0_pages(1000);
  program_memory_to_rw_0(0, (int32_t)ss_ptr, strlen(ss_ptr));
  fio("3", 1);
  set_rw_table_1(2, create_blob_rw_mem_0(ss.size()));
  fio("4", 1);
  fio("J", 1);

  {
    // Serialize Node(ast[2])
    grow_rw_table_2(3, get_ro_table_0(0));
    set_rw_table_2(0, get_ro_table_0(0));
    set_rw_table_2(1, get_ro_table_0(1));
    Node node = ast[2];
    fio("1", 1);
    stringstream s;
    {
      cereal::XMLOutputArchive oarchive(s);
      oarchive(node);
    }
    fio("2", 1);
    string ss = s.str();
    const char *ss_ptr = ss.c_str();
    grow_rw_mem_0_pages(1000);
    program_memory_to_rw_0(0, (int32_t)ss_ptr, strlen(ss_ptr));
    fio("3", 1);
    set_rw_table_2(2, create_blob_rw_mem_0(ss.size()));
    fio("4", 1);
    fio("J", 1);
  }

  set_rw_table_0(3, create_thunk(create_tree_rw_table_1(3)));
  fio("5", 1);
  set_rw_table_0(4, create_thunk(create_tree_rw_table_2(3)));
  fio("6", 1);

  return create_thunk(create_tree_rw_table_0(5));
}

__attribute__(( export_name("_fixpoint_apply")))
externref _fixpoint_apply(externref encode)
{
  fio("A", 1);

  attach_tree_ro_table_0(encode);
  attach_blob_ro_mem_0(get_ro_table_0(2));
  size_t size = byte_size_ro_mem_0();
  char *buf = (char *)malloc(size);
  ro_mem_0_to_program_memory((int32_t)buf, 0, size);

  fio("B", 1);
  // Trim whitespace
  if (buf[0] == '(')
  {
    fio("C", 1);
    const string src(buf, size);
    const auto tokens = lex(src);

    // Todo
    const auto line = parse(tokens)[0];

    const map<string, int> env;

    Node wrapper(line);
    return eval(wrapper);
  }
  else if (buf[0] == '+')
  {
    fio("D", 1);
    // This is apply, basically
    attach_blob_ro_mem_0(get_ro_table_0(3));
    int x = get_i32_ro_mem_0(0);
    attach_blob_ro_mem_0(get_ro_table_0(4));
    int y = get_i32_ro_mem_0(0);
    fio(to_string(x + y).c_str(), 2);
    return create_blob_i32(x + y);
  }
  else if (buf[0] == '<')
  {
    string s(buf, size);
    stringstream ss(s);
    cereal::XMLInputArchive iarchive(ss);
    Node node;
    iarchive(node);
    return create_blob_i32(stoi(node.arg));
  }

  fio("F", 1);
  assert(false);
}
