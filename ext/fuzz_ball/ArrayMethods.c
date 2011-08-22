// Include the Ruby headers and goodies
#include "ruby.h"

#define SCORE_INSERT -0.1
#define SCORE_DELETE -1.0
#define SCORE_MISS   -1.0
#define SCORE_MATCH   2.0



// Prototype some shit
VALUE ArrayMethods = Qnil;
void Init_array_methods();
VALUE method_count_duples(VALUE self, VALUE needle, VALUE haystack);
VALUE method_smith_waterman(VALUE self, VALUE needle, VALUE candidate);

void assign_cells(double **mat, int *needle, int *candidate, int n_needle, int n_candidate, int *i_max, int *j_max, double *max_score);
void alloc_vars(double ***mat, int **needle, int **candidate, int n_needle, int n_candidate);
void free_vars(double ***mat, int **needle, int **candidate, int n_needle, int n_candidate);
void max_of4(double arg1, double arg2, double arg3, double arg4);


// The initialization method for this module
void Init_array_methods() {
  ArrayMethods = rb_define_module("ArrayMethods");
  rb_define_private_method(ArrayMethods, "count_duples", method_count_duples, 2);
  rb_define_private_method(ArrayMethods, "smith_waterman", method_smith_waterman, 2);
}

double max_of(double arg1, double arg2, double arg3, double arg4) {

  double max_val = -10000.0;

  if (arg1 > max_val)
    max_val = arg1;

  if (arg2 > max_val)
    max_val = arg2;

  if (arg3 > max_val)
    max_val = arg3;

  if (arg4 > max_val)
    max_val = arg4;

  return max_val;
}

void alloc_vars(double ***mat, int **needle, int **candidate, int n_needle, int n_candidate) {
  int i;

  *mat = malloc(n_needle * sizeof(double *));
  for (i=0; i<n_needle; i++) {
    *((*mat) + i) = malloc(n_candidate * sizeof(double));
  }

  *needle = malloc(n_needle * sizeof(int));
  *candidate = malloc(n_candidate * sizeof(int));
}

void free_vars(double ***mat, int **needle, int **candidate, int n_needle, int n_candidate) {

  int i, j;

  free(*needle);    *needle = NULL;
  free(*candidate); *candidate = NULL;

  for (i=0; i<n_needle; i++) {
    free( (*mat)[i] );
  }
  free( *mat );
  *mat = NULL;
}

VALUE method_smith_waterman(VALUE self, VALUE needle, VALUE candidate) {
  int i, j, i_max, j_max;
  int n_needle    = (int) RARRAY_LEN(needle);
  int n_candidate = (int) RARRAY_LEN(candidate);
  int *c_needle, *c_candidate;

  double max_score;
  double **mat;

  alloc_vars(&mat, &c_needle, &c_candidate, n_needle, n_candidate);

  for (i=0; i<n_needle; i++)
    c_needle[i] = NUM2INT( RARRAY_PTR(needle)[i] );

  for (i=0; i<n_candidate; i++)
    c_candidate[i] = NUM2INT( RARRAY_PTR(candidate)[i] );

  assign_cells(mat, c_needle, c_candidate, n_needle, n_candidate, &i_max, &j_max, &max_score);

  printf("imax, jmax = %d, %d - max_score = %f\n", i_max, j_max, max_score);



  free_vars(&mat, &c_needle, &c_candidate, n_needle, n_candidate);
  return Qtrue; 
}

void assign_cells(double **mat, int *needle, int *candidate, int n_needle, int n_candidate, int *i_max, int *j_max, double *max_score) {
  int i, j;
  double score, value;

  for (i=0; i<n_needle; i++) {
    for (j=0; j<n_candidate; j++) {
      mat[i][j] = 0.0;
    }
  }

  *max_score = -10000.0;
  for (i=1; i<n_needle; i++) {
    for (j=1; j<n_candidate; j++) {
      if (needle[i-1] == candidate[j-1]) {
        score = SCORE_MATCH;
      } else {
        score = SCORE_MISS;
      }

      value = max_of(0.0, mat[i-1][j-1] + score, mat[i-1][j] + SCORE_DELETE, mat[i][j-1] + SCORE_INSERT);
      mat[i][j] = value;

      if (value > *max_score) {
        *max_score = value;
        *i_max     = i;
        *j_max     = j;
      }
    }
  }
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
