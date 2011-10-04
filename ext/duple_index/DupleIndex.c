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
  rb_define_method(DupleIndex, "match", method_match, 1);
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

VALUE method_match(VALUE self, VALUE needle) {

  //printf("\nmethod_match\n");

  int i, n_needle, c_a, c_b, match_id;
  struct match *matches, *match, *match_tmp;
  struct duples_hash *duples, *duple;
  struct duple_pos *pos;
  VALUE matches_by_score = rb_hash_new();
  VALUE arr;

  Data_Get_Struct(self, struct duples_hash, duples);
  matches = NULL;

  n_needle = (int) RARRAY_LEN(needle);

  if (n_needle < 2)
    return matches_by_score;

  for (i=0; i<(n_needle-1); i++) {
    c_a = NUM2INT( RARRAY_PTR(needle)[i] );
    c_b = NUM2INT( RARRAY_PTR(needle)[i+1] );

    //printf("Examining duple %d, %d\n", c_a, c_b);
    duple = duple_at(duples, c_a, c_b);

    if (duple != NULL ) {
      pos = duple->strings;
      while (1) {

        //printf("  Found in string %d at pos %d\n", pos->index, pos->pos);

        // if String not found in matches, create new pointer, otherwise
        // append count/position if appropriate
        match_id = pos->index;
        HASH_FIND_INT(matches, &match_id, match);

        if (match == NULL) {
          //printf("  first time matching, creating\n");
          match = malloc( sizeof(struct match) );

          match->id = match_id;
          match->n_matches = 1;
          match->last_matched_position = pos->pos;
          match->last_matched_ca = c_a;
          match->last_matched_cb = c_b;

          HASH_ADD_INT(matches, id, match);

        } else {
          //printf("  string previously found\n");
          if ((match->last_matched_position < pos->pos) && (match->last_matched_ca != c_a) && (match->last_matched_cb != c_b)) {
            match->n_matches++;
            match->last_matched_position = pos->pos;
            match->last_matched_ca = c_a;
            match->last_matched_cb = c_b;
          }
        }

        if (pos->next == NULL)
          break;

        pos = pos->next;
      }
    } else {
      //printf("  Duple not found!\n");
    }
  }

  // Loop over hash entries and spit out results
  HASH_ITER(hh, matches, match, match_tmp) {
    //printf("Match: index %d, %d matches\n", match->id, match->n_matches);

    arr = rb_hash_aref(matches_by_score, INT2NUM(match->n_matches));
    if (arr == Qnil) {
      //printf("NIL!\n");
      arr = rb_ary_new();
      rb_ary_push(arr, INT2NUM(match->id));

    } else {
      //printf("NOT NIL\n");
      rb_ary_push(arr, INT2NUM(match->id));
    }

    rb_hash_aset(matches_by_score, INT2NUM(match->n_matches), arr);
    HASH_DEL(matches, match);
    free(match);
  }

  return matches_by_score;
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

