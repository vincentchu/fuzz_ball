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
void recurse_optimal_path(double **mat, int i, int j, VALUE alignment);
void alloc_vars(double ***mat, int **needle, int **candidate, int n_needle, int n_candidate);
void free_vars(double ***mat, int **needle, int **candidate, int n_needle, int n_candidate);

double max(double a, double b);
double max3(double a, double b, double c);
double max4(double a, double b, double c, double d);

// The initialization method for this module
void Init_array_methods() {
  ArrayMethods = rb_define_module("ArrayMethods");
  rb_define_private_method(ArrayMethods, "count_duples", method_count_duples, 2);
  rb_define_private_method(ArrayMethods, "smith_waterman", method_smith_waterman, 2);
}

// A few convenience methods for determining the max of 2, 3, and 4 doubles
double max(double a, double b) {
  return ( (a > b) ? a : b );
}

double max3(double a, double b, double c) {
  return max(a, max(b, c));
}

double max4(double a, double b, double c, double d) {
  return max(a, max3(b, c, d));
}

/* alloc_vars
 *
 * A simple function that allocates memory for the alignment matrix, as well
 * as the arrays that store the characters of the needle and candidate strings
 * given their lengths.
 */
void alloc_vars(
    double ***mat, 
    int **needle, 
    int **candidate, 
    int n_needle, 
    int n_candidate
) {
  int i;

  *mat = malloc(n_needle * sizeof(double *));
  for (i=0; i<n_needle; i++) {
    *((*mat) + i) = malloc(n_candidate * sizeof(double)); // ptr arithmetic FTW
  }

  *needle = malloc(n_needle * sizeof(int));
  *candidate = malloc(n_candidate * sizeof(int));
}

/* free_vars
 *
 * The method that frees memory associated with the alignment matrix, and the 
 * needle and candidate strings.
 */
void free_vars(
    double ***mat, 
    int **needle, 
    int **candidate, 
    int n_needle, 
    int n_candidate
) {

  int i;

  free(*needle);    *needle = NULL;
  free(*candidate); *candidate = NULL;

  for (i=0; i<n_needle; i++) {
    free( (*mat)[i] );
  }
  free( *mat );
  *mat = NULL;
}

/* method_smith_waterman
 *
 * We use the Smith-Waterman algorithm to align each candidate string with the
 * needle string and to see how well the two fit. The Smith-Waterman algorithm
 * is a dynamic programming algorith that keeps track of different alignments 
 * between two strings using a matrix. The best alignment is determined using
 * a recursive search through the alignment matrix. For more information, see:
 *
 * http://en.wikipedia.org/wiki/Smith%E2%80%93Waterman_algorithm
 */
VALUE method_smith_waterman(VALUE self, VALUE needle, VALUE candidate) {
  int i, j, i_max, j_max;
  int n_needle    = (int) RARRAY_LEN(needle);
  int n_candidate = (int) RARRAY_LEN(candidate);
  int *c_needle, *c_candidate;

  double max_score;
  double **mat;

  VALUE alignment = rb_ary_new(); 

  alloc_vars(&mat, &c_needle, &c_candidate, n_needle, n_candidate);

  // Copy the needle / candidate strings from their ruby versions
  // into plain old C-integer arrays.
  for (i=0; i<n_needle; i++) {
    c_needle[i] = NUM2INT( RARRAY_PTR(needle)[i] );
  }

  for (i=0; i<n_candidate; i++) {
    c_candidate[i] = NUM2INT( RARRAY_PTR(candidate)[i] );
  }

  assign_cells(mat, c_needle, c_candidate, n_needle, n_candidate, &i_max, &j_max, &max_score);
  recurse_optimal_path(mat, i_max, j_max, alignment);
  
  rb_iv_set(self, "@curr_alignment", alignment);
  rb_iv_set(self, "@curr_score", DBL2NUM(max_score));

  free_vars(&mat, &c_needle, &c_candidate, n_needle, n_candidate);
  return Qtrue; 
}

/* assign_cells
 *
 * Called within the smith_waterman loop; this is the function that assigns
 * each cell of the alignment matrix; the value of each cell represents the 
 * score of that given alignment, up to that point, taking into account any
 * deletions, additions, etc.. that result in that alignment. For two strings
 * of length m, n the alignment matrix is m x n large. As the values are
 * assigned, we keep track of the cell with the higest score. At the end of 
 * the assignment, we start at this highest-scoring cell and recursively walk
 * backwards through the cells, maximizing the score at each step. This
 * becomes the highest scoring alignment
 */
void assign_cells(
    double **mat, 
    int *needle, 
    int *candidate, 
    int n_needle, 
    int n_candidate, 
    int *i_max, 
    int *j_max, 
    double *max_score
) {
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

      value = max4(0.0, mat[i-1][j-1] + score, mat[i-1][j] + SCORE_DELETE, mat[i][j-1] + SCORE_INSERT);
      mat[i][j] = value;

      if (value > *max_score) {
        *max_score = value;
        *i_max     = i;
        *j_max     = j;
      }
    }
  }
}

/* recurse_optimal_path 
 *
 * This method searches the alignment matrix from a starting point,
 * and recursively walks backwards, maximizing the score at each step. 
 * This will result in an alignment of greatest score.
 */
void recurse_optimal_path(double **mat, int i, int j, VALUE alignment) {
  int ii, jj;
  double max_value;

  // Push current position of the alignment into the array that stores it.
  rb_ary_push(alignment, INT2NUM(i));
  rb_ary_push(alignment, INT2NUM(j));

  max_value = max3(mat[i-1][j-1], mat[i-1][j], mat[i][j-1]);

  if (max_value == mat[i-1][j]) {
    ii = i-1;
    jj = j;
  }

  if (max_value == mat[i][j-1]) {
    ii = i;
    jj = j-1;
  }

  if (max_value == mat[i-1][j-1]) {
    ii = i-1;
    jj = j-1;
  }
  
  // The recursive loop. If we reach an edge (i.e., m(i,j) == 0), then stop
  // the recursion. Otherwise, keep going!
  if (mat[i][j] == 0) {
    return;
  } else {
    return recurse_optimal_path(mat, ii, jj, alignment);
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
