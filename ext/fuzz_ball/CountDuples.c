// Include the Ruby headers and goodies
#include "ruby.h"

// Prototype some shit
VALUE CountDuples = Qnil;
void Init_count_duples();
VALUE method_count_duples(VALUE self, VALUE needle, VALUE haystack);

// The initialization method for this module
void Init_count_duples() {
  CountDuples = rb_define_module("CountDuples");
  rb_define_method(CountDuples, "count_duples", method_count_duples, 2);
}

/* count_duples(needle, haystack)
 * 
 * This method counts the number of duples shared between two strings, 
 * represented by two arrays: the needle, and the haystack. The general
 * strategy is to loop over the characters of the needle, and find the 
 * index of that character in the haystack. If the next character of the
 * needle matches the next character in the haystack, then the number of
 * duples is increased by 1
*/
VALUE method_count_duples(VALUE self, VALUE needle, VALUE haystack) {

  int index, c_n, c_h, i, j;
  int n_duples = 0, old_index = 0;
  int len_needle   = (int) RARRAY_LEN(needle);
  int len_haystack = (int) RARRAY_LEN(haystack);

  VALUE *c_needle   = RARRAY_PTR(needle);
  VALUE *c_haystack = RARRAY_PTR(haystack);

  // Loop over each character of the needle
  for (i=0; i<len_needle; i++) {
    c_n = NUM2INT(c_needle[i]);

    // Now, find the index at which the character appears in the haystack
    for (j=old_index; j<len_haystack; j++) {
      c_h = NUM2INT(c_haystack[j]);
      index = j;

      // Break from loop if a match is found
      if (c_n == c_h) {
        break;
      }
    }

    // If a match *IS NOT* found, then break to the next character of the 
    // needle. Note, that the condition of a match not found is if the search
    // loop makes it to the end of the haystack character,s and the current
    // needle character (c_n) doesn't match the last haystack character (c_h)
    if (index == (len_haystack-1) && (c_n != c_h)) {
      break;
    }

    // If the index of the needle character is equal to the start of the substring
    // then there's a duple
    if (index == old_index) {
      n_duples++;
    }

    // Start the next haystack search at the next character after.
    old_index = (index + 1);
  }

  // Account for the case where the first characters match; in this case, the number
  // of duples needs to be decremented.
  if (c_haystack[0] == c_needle[0]) {
    n_duples--;
  }

  return INT2NUM( n_duples );
}

