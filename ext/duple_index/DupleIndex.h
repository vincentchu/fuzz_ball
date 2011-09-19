#define MAX_CHARS 1000
// Prototype some shit

// TEST globals
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

// Ruby-related declarations
VALUE FuzzBall   = Qnil;
VALUE DupleIndex = Qnil;

void Init_duple_index();
VALUE method_alloc_index(VALUE self);
VALUE method_free_index(VALUE self);
VALUE method_add(VALUE self, VALUE r_str_id, VALUE r_str);
VALUE method_query(VALUE self, VALUE r_a, VALUE r_b);

// Internally-used C-declarations
void add_duple(struct duples_hash *duples, int c_a, int c_b, int index, int pos);
struct duples_hash *duple_at(struct duples_hash *duples, int c_a, int c_b);
int duple_id(int c_a, int c_b);
struct duple_pos *create_duple_pos(int index, int pos, struct duple_pos *next, struct duple_pos *prev);
