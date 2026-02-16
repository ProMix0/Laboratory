#include <stdbool.h>
#include <stdio.h>

typedef enum {
  TYPE_STRING,
  TYPE_DOUBLE,
  TYPE_BOOL,
  TYPE_ENUM,
  TYPE_ONEHOT,
} column_type;

typedef union {
  const char *s;
  bool b;
  double d;
  ssize_t e;
  bool oh;
} data_union;

struct data_column {
  const char *name;
  column_type type;

  data_union *data;
};

struct data_frame {
  struct data_column **columns;
  size_t columns_count, columns_size;

  // Rows
  size_t count, size;

  // string data
  char **_string_rows;
};

typedef bool (*row_predicate)(struct data_frame *, size_t);

struct data_frame *read_csv(FILE *csv, size_t size);
void delete_rows_if(struct data_frame *df, row_predicate predicate);
void print_dataframe(struct data_frame *df, FILE *out);
void free_dataframe(struct data_frame *df);

ssize_t column_index_by_name(struct data_frame *df, const char *name);
void parse_as_double(struct data_frame *df, size_t column);
void parse_as_bool(struct data_frame *df, size_t column);
void parse_as_enum(struct data_frame *df, size_t column, const char **values);

void get_stats(struct data_frame *df, size_t column, double *min_ptr,
               double *max_ptr, double *mean_ptr);
void get_std(struct data_frame *df, size_t column, double mean,
             double *std_ptr);
void normalize(struct data_frame *df, size_t column, double min, double max);
void standardize(struct data_frame *df, size_t column, double mean, double std);
void one_hot(struct data_frame *df, size_t column, size_t count);
