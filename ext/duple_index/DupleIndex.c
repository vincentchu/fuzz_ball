// Include the Ruby headers and goodies
#include <ruby.h>
#include "uthash.h"
#include <DupleIndex.h>

// The initialization method for this module
void Init_duple_index() {
  FuzzBall   = rb_define_module("FuzzBall");
  DupleIndex = rb_define_class_under(FuzzBall, "DupleIndex", rb_cObject);

  rb_define_alloc_func(DupleIndex, method_alloc_index);
  rb_define_method(DupleIndex, "add", method_add, 2);
}

VALUE method_alloc_index(VALUE self) {
  printf("\n\n**** method_alloc_index\n");
  struct duple_store *dp;

  n_malloc_calls++;
  dp = malloc( sizeof(struct duple_store) );
  dp->duples = NULL;

  return Data_Wrap_Struct(self, NULL, method_free_index, dp);
}

VALUE method_free_index(VALUE self) {
  printf("\n\n**** method_free_index\n");

  printf("malloc called %d times\n", n_malloc_calls);
  printf("free   called %d times\n", n_free_calls);
}

VALUE method_add(VALUE self, VALUE r_str_id, VALUE r_str) {
  printf("\n\n**** method_add\n");
  struct duple_store *dp;
  int i, str_id, str_len, c_a, c_b;

  Data_Get_Struct(self, struct duple_store, dp);

  str_len = (int) RARRAY_LEN(r_str);
  str_id  = NUM2INT( r_str_id );

  for (i=0; i<(str_len-1); i++) {
    c_a = NUM2INT( RARRAY_PTR(r_str)[i] );
    c_b = NUM2INT( RARRAY_PTR(r_str)[i+1] );
    add_duple(dp->duples, c_a, c_b, str_id, i);
  }

  return Qtrue;
}

int duple_id(int c_a, int c_b) {
 return c_b + (c_a % MAX_CHARS) * MAX_CHARS;
}

struct duples_hash *duple_at(struct duples_hash *duples, int c_a, int c_b) {
  int d_id;
  struct duples_hash *d;

  d_id = duple_id(c_a, c_b);
  HASH_FIND_INT(duples, &d_id, d);

  return d;
}

void add_duple(struct duples_hash *duples, int c_a, int c_b, int index, int pos) {
  struct duples_hash *ptr;
  struct duple_pos *d_pos;

  ptr = duple_at(duples, c_a, c_b);
  if (ptr == NULL) {
    printf("duple %5d (%3d, %3d) MISS!\n", duple_id(c_a, c_b), c_a, c_b);

    n_malloc_calls++;
    ptr          = malloc(sizeof(struct duples_hash));
    ptr->id      = duple_id(c_a, c_b);
    ptr->strings = create_duple_pos(index, pos, NULL, NULL);

    HASH_ADD_INT(duples, id, ptr);

  } else {
    printf("duple %5d (%3d, %3d)  HIT!\n", duple_id(c_a, c_b), c_a, c_b);

    d_pos              = create_duple_pos(index, pos, ptr->strings, NULL);
    ptr->strings->prev = d_pos;
    ptr->strings       = d_pos;
  }


}

struct duple_pos *create_duple_pos(int index, int pos, struct duple_pos *next, struct duple_pos *prev) {
  struct duple_pos *ptr;

  n_malloc_calls++;
  ptr = malloc( sizeof(struct duple_pos) );

  ptr->index = index;
  ptr->pos   = pos;
  ptr->next  = next;
  ptr->prev  = prev;

  return ptr;
}


