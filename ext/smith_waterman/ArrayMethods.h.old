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
