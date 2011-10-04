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

/* method_alloc_index
 *
 * This method is a custom allocation method where we initialize
 * the hash that will serve as the duple_index and store it in the
 * duples pointer. We then store it within the instantiated ruby 
 * object for later use.
 */
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

/* method_free_index
 *
 * called by ruby when it's trying to free the instantiated
 * DupleIndex class. In this method, we start the deallocation
 * of memory by iterating over each hash member and calling
 * free successively.
 */
static void method_free_index(void *duples) {
  destroy_index( duples );
}

/* destroy_index
 *
 * loops over each key in the duples hash, frees the memory
 * associated with each hash member by deallocating the linked
 * list that stores duple positions.
 */
void destroy_index(struct duples_hash *duples) {
  struct duples_hash *d, *d_tmp;

  HASH_ITER(hh, duples, d, d_tmp) {
    destroy_duple_pos(d->strings);

    HASH_DEL(duples, d);
    free(d);
  }
}

/* destroy_duple_pos
 *
 * frees the memory used to store duple_pos structs
 */
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

/* method_add
 *
 * ruby public method that allows one to add strings to the duple index.
 * strings are represented by a unique index (r_str_id) and by an array
 * of ints, that represent each character. The add method will then
 * index where each duple appears, allowing quick recall of what strings
 * and where in those strings a given duple appears.
 */
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

/* duple_id
 *
 * method that hashes the two intergers that represent a duple into a
 * unique integer value. Pretty simple algo.
 */
int duple_id(int c_a, int c_b) {
 return c_b + (c_a % MAX_CHARS) * MAX_CHARS;
}

/* duple_at
 *
 * method that allows one to find which strings (and where in those strings)
 * a given duple appears (represented by two ints, c_a and c_b)
 */
struct duples_hash *duple_at(struct duples_hash *duples, int c_a, int c_b) {
  int d_id;
  struct duples_hash *d;

  d_id = duple_id(c_a, c_b);
  HASH_FIND_INT(duples, &d_id, d);

  return d;
}

/* add_duple
 *
 * Add a duple (represented by c_a and c_b) to the index, given the string (index)
 * and position(pos) it appears.
 */
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

/* method_match
 *
 * this is a ruby public method that allows us to query the duple index. For a given
 * needle string (represented as an array of numbers), we want to return a hash that
 * lists which strings matched the needle, by how many times a duple in the needle
 * matched a duple in the hits. To do that, we loop over the duples in the needle
 * string, find which strings match that duple, then keep track of how many times a
 * string matched a duple in the needle.
 * */
VALUE method_match(VALUE self, VALUE needle) {

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
    return matches_by_score; // If the needle has fewer than two chars,
                             // it's not a duple so return immediately

  // Loop over each duple in the needle string
  for (i=0; i<(n_needle-1); i++) {
    c_a = NUM2INT( RARRAY_PTR(needle)[i] );
    c_b = NUM2INT( RARRAY_PTR(needle)[i+1] );

    duple = duple_at(duples, c_a, c_b); // Find the strings and positions
                                        // where this duple is found

    if (duple != NULL ) {
      pos = duple->strings;
      while (1) { // Loop over the strings where duple is found

        // if String not found in matches hash, create new pointer
        match_id = pos->index;
        HASH_FIND_INT(matches, &match_id, match);

        if (match == NULL) {
          match = create_match(match_id, pos->pos, c_a, c_b);
          HASH_ADD_INT(matches, id, match);

        } else {
          /* Only update the match count if the next matching duple appears
           * *AFTER* the last matched duple. For instance, with a needle string
           * of 'abc' and a indexed string of 'bcab', we don't want the 'bc'
           * duple to match since it appears before 'ab' in the indexed string,
           * whereas it appears after in the needle string.
           */
          if ((match->last_matched_position < pos->pos) && (match->last_matched_ca != c_a) && (match->last_matched_cb != c_b)) {
            update_match( match, pos->pos, c_a, c_b );
          }
        }

        if (pos->next == NULL)
          break;

        pos = pos->next;
      }
    }
  }

  // Loop over matches and construct the Ruby hash that stores the matching
  // strings by the number of times they matched. Also free up memory as we go.
  HASH_ITER(hh, matches, match, match_tmp) {
    arr = rb_hash_aref(matches_by_score, INT2NUM(match->n_matches));
    if (arr == Qnil)
      arr = rb_ary_new();

    rb_ary_push(arr, INT2NUM(match->id));
    rb_hash_aset(matches_by_score, INT2NUM(match->n_matches), arr);

    HASH_DEL(matches, match);
    free(match);
  }

  return matches_by_score;
}

struct match *create_match(int id, int pos, int c_a, int c_b) {
  struct match *new_match;

  new_match = malloc( sizeof(struct match) );

  new_match->id                    = id;
  new_match->n_matches             = 1;
  new_match->last_matched_position = pos;
  new_match->last_matched_ca       = c_a;
  new_match->last_matched_cb       = c_b;

  return new_match;
}

void update_match(struct match* match, int pos, int c_a, int c_b) {
  match->n_matches++;
  match->last_matched_position = pos;
  match->last_matched_ca       = c_a;
  match->last_matched_cb       = c_b;
}

