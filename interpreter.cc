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

void make_table_1(Node node)
{
  grow_rw_table_1(4, get_ro_table_0(0));
  set_rw_table_1(0, get_ro_table_0(0));
  set_rw_table_1(1, get_ro_table_0(1));
  set_rw_table_1(2, create_blob_i32(EVAL));

  stringstream ss;
  {
    cereal::XMLOutputArchive oarchive(ss);
    oarchive(node);
  }
  string str = ss.str();

  grow_rw_mem_0_pages(str.size());
  program_memory_to_rw_0(0, (int32_t)str.c_str(), str.size());

  set_rw_table_1(3, create_blob_rw_mem_0(str.size()));
}

void make_table_2(Node node)
{
  grow_rw_table_2(4, get_ro_table_0(0));
  set_rw_table_2(0, get_ro_table_0(0));
  set_rw_table_2(1, get_ro_table_0(1));
  set_rw_table_2(2, create_blob_i32(EVAL));

  stringstream ss;
  {
    cereal::XMLOutputArchive oarchive(ss);
    oarchive(node);
  }
  string str = ss.str();

  grow_rw_mem_0_pages(str.size());
  program_memory_to_rw_0(0, (int32_t)str.c_str(), str.size());

  set_rw_table_2(3, create_blob_rw_mem_0(str.size()));
}

externref eval(Node ast)
{
  grow_rw_table_0(5, get_ro_table_0(0));
  set_rw_table_0(0, get_ro_table_0(0));
  set_rw_table_0(1, get_ro_table_0(1));

  if (ast[0].arg == "+")
    set_rw_table_0(2, create_blob_i32(APPLY_ADD));
  else if (ast[0].arg == "-")
    set_rw_table_0(2, create_blob_i32(APPLY_SUB));
  else if (ast[0].arg == "*")
    set_rw_table_0(2, create_blob_i32(APPLY_MUL));
  else
    assert(false);

  make_table_1(ast[1]);
  make_table_2(ast[2]);

  set_rw_table_0(3, create_thunk(create_tree_rw_table_1(4)));
  set_rw_table_0(4, create_thunk(create_tree_rw_table_2(4)));

  return create_thunk(create_tree_rw_table_0(5));
}

Node make_node(const char *buf, size_t size)
{
  string s(buf, size);
  stringstream ss(s);
  cereal::XMLInputArchive iarchive(ss);
  Node node;
  iarchive(node);
  return node;
}

externref apply(Op op)
{
  attach_blob_ro_mem_0(get_ro_table_0(3));
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

  attach_blob_ro_mem_0(get_ro_table_0(3));
  size_t size = byte_size_ro_mem_0();
  char *buf = (char *)malloc(size);
  ro_mem_0_to_program_memory((int32_t)buf, 0, size);

  switch (op)
  {
    case BEGIN:
    {
      const string src(buf, size);
      const auto tokens = lex(src);

      // Todo
      const auto line = parse(tokens)[0];

      Node wrapper(line);
      return eval(wrapper);
    }
  
    case EVAL:
    {
      Node node = make_node(buf, size);
      if (!node.is_list)
        return create_blob_i32(stoi(node.arg));
      return eval(node.list);
    }

    default:
      apply(op);
  }
}
