// Include the Ruby headers and goodies
#include <ruby.h>
#include <SmithWaterman.h>

// The initialization method for this module
void Init_smith_waterman() {
	FuzzBall = rb_define_module("FuzzBall");
	SmithWaterman = rb_define_class_under(FuzzBall, "SmithWaterman", rb_cObject);
  /* rb_define_private_method(ArrayMethods, "count_duples", method_count_duples, 2);*/
  /* rb_define_private_method(ArrayMethods, "smith_waterman", method_smith_waterman, 2);*/
}
