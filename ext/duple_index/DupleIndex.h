// Prototype some shit

// TEST globas
int n_malloc_calls = 0;
int n_free_calls   = 0;

struct duple_pos {
  int index;
  int pos;

  struct duple_pos *next;
  struct duple_pos *prev;
};

struct duples_hash {
  int id;
  struct duple_pos *strings;
  UT_hash_handle hh;
};

struct duple_store {
  struct duples_hash *duples;
};

// Ruby-related declarations
VALUE FuzzBall   = Qnil;
VALUE DupleIndex = Qnil;

void Init_duple_index();
VALUE method_alloc_index(VALUE self);
VALUE method_free_index(VALUE self);
VALUE method_add(VALUE self, VALUE str);

// Internally-used C-declarations
