// Include the Ruby headers and goodies
#include <ruby.h>
#include "uthash.h"
#include <DupleIndex.h>

// The initialization method for this module
void Init_duple_index() {
  FuzzBall   = rb_define_module("FuzzBall");
  DupleIndex = rb_define_class_under(FuzzBall, "DupleIndex", rb_cObject);

  rb_define_alloc_func(DupleIndex, method_alloc_index);
  rb_define_method(DupleIndex, "add", method_add, 1);
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

VALUE method_add(VALUE self, VALUE str) {
  printf("\n\n**** method_add\n");
  struct duple_store *dp;


  Data_Get_Struct(self, struct duple_store, dp);

  return Qtrue;
}

