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

externref eval()
{
  grow_rw_table_1(5, get_ro_table_0(0));
  set_rw_table_1(0, get_ro_table_0(0));
  set_rw_table_1(1, get_ro_table_0(1));

  attach_blob_ro_mem_0(get_ro_table_0(4));
  size_t size = byte_size_ro_mem_0();
  char *op = (char *)malloc(size);
  ro_mem_0_to_program_memory((int32_t)op, 0, size);
  string op_str(op, size);

  if (op_str == "+")
  {
    set_rw_table_1(2, create_blob_i32(APPLY_ADD));
  }
  else
    assert(false);

  set_rw_table_1(3, get_ro_table_0(5));
  set_rw_table_1(4, get_ro_table_0(6));
  return create_thunk(create_tree_rw_table_1(5));
}

externref apply(Op op)
{
  fio("77", 2);
  attach_blob_ro_mem_0(get_ro_table_0(3));
  fio("78", 2);
  int x = get_i32_ro_mem_0(0);
  attach_blob_ro_mem_0(get_ro_table_0(4));
  int y = get_i32_ro_mem_0(0);

  switch (op)
  {
    case APPLY_ADD:
      return create_blob_i32(x + y);
    case APPLY_SUB:
      return create_blob_i32(x - y);
    case APPLY_MUL:
      return create_blob_i32(x * y);
    default:
      assert(false);
  }
}

__attribute__(( export_name("_fixpoint_apply")))
externref _fixpoint_apply(externref encode)
{
  attach_tree_ro_table_0(encode);
  attach_blob_ro_mem_0(get_ro_table_0(2));
  Op op = static_cast<Op>(get_i32_ro_mem_0(0));

  switch (op)
  {
    case BEGIN:
    {
      assert(false);
      // const string src(buf, size);
      // const auto tokens = lex(src);

      // // Todo
      // const auto line = parse(tokens)[0];

      // Node wrapper(line);
      // return eval();
    }
  
    case EVAL:
    {
      attach_blob_ro_mem_0(get_ro_table_0(3));
      int is_list = get_i32_ro_mem_0(0);
      if (!is_list)
      {
        attach_blob_ro_mem_0(get_ro_table_0(4));
        size_t size = byte_size_ro_mem_0();
        char *buf = (char *)malloc(size);
        ro_mem_0_to_program_memory((int32_t)buf, 0, size);
        string s(buf, size);
        return create_blob_i32(stoi(s));
      }
      return eval();      
    }

    default:
    {
      return apply(op);
    }
  }
}
