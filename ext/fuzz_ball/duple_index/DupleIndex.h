#define MAX_CHARS 1000

/* duples_hash is the struct that is the "core" of the duple index.
 * The id field is a unique integer that represents a unique duple.
 * the duple_pos pointer is a pointer to a linked list that stores
 * all of the strings that a given duple appears in.
 */
struct duples_hash {
  int id;
  struct duple_pos *strings;
  UT_hash_handle hh;
};

/* duple_pos is a node on a doubly-linked list that stores the index
 * of the string and where inside the string a particular duple appears.
 * The linked list will store all of the locations where a duple appears.
 */
struct duple_pos {
  int index;
  int pos;

  struct duple_pos *next;
  struct duple_pos *prev;
};

/* match is a struct that keeps track of how many duples match in a given
 * string. The id field is the id of the string, n_matches is the number
 * of duples that have matched, the last_matched_position is the position
 * of the last matching duple, and last_matched_ca and last_matched_cb 
 * records the first and second character of the last matched duple.
 */
struct match {
  int id;
  int n_matches;
  int last_matched_position;
  int last_matched_ca;
  int last_matched_cb;

  UT_hash_handle hh;
};

// Ruby-related declarations
VALUE FuzzBall   = Qnil;
VALUE DupleIndex = Qnil;

void        Init_duple_index();
VALUE       method_alloc_index(VALUE self);
static void method_free_index(void *duples);
VALUE       method_add(VALUE self, VALUE r_str_id, VALUE r_str);
VALUE       method_match(VALUE self, VALUE needle);

// Internally-used C-declarations (i.e., private methods)
void               add_duple(struct duples_hash *duples, int c_a, int c_b, int index, int pos);
struct duples_hash *duple_at(struct duples_hash *duples, int c_a, int c_b);
int                duple_id(int c_a, int c_b);
struct duple_pos   *create_duple_pos(int index, int pos, struct duple_pos *next, struct duple_pos *prev);
void               destroy_index(struct duples_hash *duples);
void               destroy_duple_pos(struct duple_pos *head);
struct match       *create_match(int id, int pos, int c_a, int c_b);
void               update_match(struct match* match, int pos, int c_a, int c_b);
