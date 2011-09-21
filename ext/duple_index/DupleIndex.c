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
  rb_define_method(DupleIndex, "query", method_query, 2);
}

VALUE method_alloc_index(VALUE self) {
  struct duples_hash *duples, *ptr;

  // Initialize the hash with a single, hidden entry so that
  // the *duples pointer doesn't point to NULL
  ptr          = malloc(sizeof(struct duples_hash));
  ptr->id      = duple_id(-1, -1);
  ptr->strings = create_duple_pos(-1, -1, NULL, NULL);

  HASH_ADD_INT(duples, id, ptr);

  return Data_Wrap_Struct(self, NULL, method_free_index, duples);
}

static void method_free_index(void *duples) {
  destroy_index( duples );
}

void destroy_index(struct duples_hash *duples) {
  struct duples_hash *d, *d_tmp;

  HASH_ITER(hh, duples, d, d_tmp) {

    destroy_duple_pos(d->strings);

    HASH_DEL(duples, d);
    free(d);
  }
}

void destroy_duple_pos(struct duple_pos *head) {
  struct duple_pos *c_pos, *n_pos;

  c_pos = head;

  while (1) {
    n_pos = c_pos->next;
    free(c_pos);

    if (n_pos == NULL) {
      break;
    } else {
      c_pos = n_pos;
    }
  }
}


VALUE method_add(VALUE self, VALUE r_str_id, VALUE r_str) {
  int i, str_id, str_len, c_a, c_b;
  struct duples_hash *duples;

  Data_Get_Struct(self, struct duples_hash, duples);
  str_len = (int) RARRAY_LEN(r_str);
  str_id  = NUM2INT( r_str_id );

  for (i=0; i<(str_len-1); i++) {
    c_a = NUM2INT( RARRAY_PTR(r_str)[i] );
    c_b = NUM2INT( RARRAY_PTR(r_str)[i+1] );
    add_duple(duples, c_a, c_b, str_id, i);
  }

  return Qtrue;
}

VALUE method_query(VALUE self, VALUE r_a, VALUE r_b) {
  struct duples_hash *duples, *ptr;
  struct duple_pos *pos;
  int c_a, c_b;
  VALUE query_raw = rb_ary_new();
  VALUE entry;

  Data_Get_Struct(self, struct duples_hash, duples);

  c_a = NUM2INT( r_a );
  c_b = NUM2INT( r_b );

  ptr = duple_at(duples, c_a, c_b);;
  if (ptr != NULL) {
    pos = ptr->strings;

     while(1) {
      entry = rb_ary_new2(2);
      rb_ary_store(entry, 0, INT2NUM( pos->index ));
      rb_ary_store(entry, 1, INT2NUM( pos->pos ));

      rb_ary_push(query_raw, entry);

      if (pos->next == NULL)
        break;

      pos = pos->next;
    }
  }

  return query_raw;
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
    ptr          = malloc(sizeof(struct duples_hash));
    ptr->id      = duple_id(c_a, c_b);
    ptr->strings = create_duple_pos(index, pos, NULL, NULL);

    HASH_ADD_INT(duples, id, ptr);

  } else {
    d_pos              = create_duple_pos(index, pos, ptr->strings, NULL);
    ptr->strings->prev = d_pos;
    ptr->strings       = d_pos;
  }
}

struct duple_pos *create_duple_pos(int index, int pos, struct duple_pos *next, struct duple_pos *prev) {
  struct duple_pos *ptr;

  ptr = malloc( sizeof(struct duple_pos) );

  ptr->index = index;
  ptr->pos   = pos;
  ptr->next  = next;
  ptr->prev  = prev;

  return ptr;
}


