#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data_frame.h"

#define CPU_NUM 12

static void append_column(struct data_frame *df, struct data_column *column);
static struct data_column *alloc_column(const char *name, size_t size,
                                        column_type type);
static struct data_frame *alloc_dataframe(size_t size);
static ssize_t index_of_string(const char **values, const char *str);

static void to_slices(struct data_frame *df, struct data_frame *slices,
                      size_t count);
static void free_slices(struct data_frame *slices, size_t count);

struct data_frame *read_csv(FILE *csv, size_t size) {
  char *buf = NULL;
  size_t str_size;
  struct data_frame *result;

  result = alloc_dataframe(size);

  // TODO read_and_parse_line()
  if (getline(&buf, &str_size, csv) != -1) {
    char *name;
    char *state;
    char *header = buf;

    while (name = strtok_r(buf, ",\n", &state)) {
      struct data_column *column =
          alloc_column(strdup(name), size, TYPE_STRING);
      buf = NULL;
      append_column(result, column);
    }

    free(header);
    header = NULL;

    {
      char *buf = NULL;
      size_t rows = 0;
      for (size_t i = 0; i < size && (getline(&buf, &str_size, csv) != -1);
           i++) {
        size_t column = 0;
        char *value;
        char *state;

        result->_string_rows[i] = buf;

        while (value = strtok_r(buf, ",\n", &state)) {
          buf = NULL;
          if (column >= result->columns_count) {
            fprintf(stderr, "Too many columns in row\n");
            break;
          }
          result->columns[column++]->data[i].s = value;
        }
        buf = NULL;
        if (column < result->columns_count) {
          fprintf(stderr, "Not enough columns in row\n");
        }

        result->count++;
      }
    }

  } else {
    fprintf(stderr, "Error - getline\n");
    free(buf);
  }

  return result;
}

void free_dataframe(struct data_frame *df) {
  for (size_t i = 0; i < df->columns_count; i++) {
    free(df->columns[i]->name);
    df->columns[i]->name = NULL;
    free(df->columns[i]->data);
    df->columns[i]->data = NULL;
    free(df->columns[i]);
    df->columns[i] = NULL;
  }
  free(df->columns);
  df->columns = NULL;
  for (size_t i = 0; i < df->size; i++) {
    free(df->_string_rows[i]);
    df->_string_rows[i] = NULL;
  }
  free(df->_string_rows);
  df->_string_rows = NULL;

  free(df);
}

void print_dataframe(struct data_frame *df, FILE *out) {
  // Names
  for (size_t column = 0; column < df->columns_count; column++) {
    fprintf(out, "%s,", (const char *)df->columns[column]->name);
  }
  fprintf(out, "\n");
  // Values
  for (size_t row = 0; row < df->count; row++) {
    for (size_t column = 0; column < df->columns_count; column++) {
      switch (df->columns[column]->type) {
      case TYPE_STRING:
        fprintf(out, "%s,", (const char *)df->columns[column]->data[row].s);
        break;
      case TYPE_DOUBLE:
        fprintf(out, "%lf,", df->columns[column]->data[row].d);
        break;
      case TYPE_BOOL:
        fprintf(out, "%d,", df->columns[column]->data[row].b);
        break;
      case TYPE_ENUM:
        fprintf(out, "%d,", df->columns[column]->data[row].e);
        break;
      case TYPE_ONEHOT:
        fprintf(out, "%d,", df->columns[column]->data[row].oh);
        break;
      default:
        fprintf(stderr, "Unknown type value!!!\n");
        break;
      }
    }
    fprintf(out, "\n");
  }
}

void delete_rows_if(struct data_frame *df, row_predicate predicate) {
  for (size_t row = 0; row < df->count; row++) {
    if (predicate(df, row)) {
      size_t replacement_row = df->count - 1;
      while (replacement_row >= row && predicate(df, replacement_row)) {
        free(df->_string_rows[replacement_row]);
        df->_string_rows[replacement_row] = NULL;

        replacement_row--;
      }
      df->count = replacement_row;

      if (replacement_row > row) {
        for (size_t column = 0; column < df->columns_count; column++) {
          df->columns[column]->data[row] =
              df->columns[column]->data[replacement_row];
        }
        free(df->_string_rows[row]);
        df->_string_rows[row] = df->_string_rows[replacement_row];
        df->_string_rows[replacement_row] = NULL;
      }
    }
  }
}

ssize_t column_index_by_name(struct data_frame *df, const char *name) {
  for (size_t i = 0; i < df->columns_count; i++) {
    if (!strcmp(name, df->columns[i]->name)) {
      return i;
    }
  }
  return -1;
}

void parse_as_double(struct data_frame *df, size_t column) {
  if (df->columns[column]->type != TYPE_STRING) {
    fprintf(stderr, "Parsing non-string column!!!\n");
    return;
  }

  df->columns[column]->type = TYPE_DOUBLE;

  struct data_frame slices[CPU_NUM];
  to_slices(df, slices, CPU_NUM);
#pragma omp parallel for num_threads(CPU_NUM) schedule(static, 1)
  for (size_t i = 0; i < CPU_NUM; i++) {
    for (size_t row = 0; row < slices[i].count; row++) {
      double value;
      value = atof(slices[i].columns[column]->data[row].s);
      slices[i].columns[column]->data[row].d = value;
    }
  }

  free_slices(slices, CPU_NUM);
}

void parse_as_bool(struct data_frame *df, size_t column) {
  if (df->columns[column]->type != TYPE_STRING) {
    fprintf(stderr, "Parsing non-string column!!!\n");
    return;
  }

  df->columns[column]->type = TYPE_BOOL;

  for (size_t row = 0; row < df->count; row++) {
    bool value = !!atoi(df->columns[column]->data[row].s);
    df->columns[column]->data[row].b = (void *)value;
  }
}

void parse_as_enum(struct data_frame *df, size_t column, const char **values) {
  if (df->columns[column]->type != TYPE_STRING) {
    fprintf(stderr, "Parsing non-string column!!!\n");
    return;
  }

  df->columns[column]->type = TYPE_ENUM;

  for (size_t row = 0; row < df->count; row++) {
    ssize_t value = index_of_string(values, df->columns[column]->data[row].s);
    df->columns[column]->data[row].e = value;
  }
}

static ssize_t index_of_string(const char **values, const char *str) {
  size_t index = 0;
  while (*values) {
    if (!strcmp(*values, str)) {
      return index;
    }
    index++;
    values++;
  }
  return -1;
}

static struct data_column *alloc_column(const char *name, size_t size,
                                        column_type type) {
  struct data_column *result;

  result = malloc(sizeof(*result));
  if (!result)
    return result;

  result->data = malloc(size * sizeof(result->data[0]));
  result->name = name;
  result->type = type;

  if (type == TYPE_ONEHOT) {
    memset(result->data, 0, size * sizeof(result->data[0]));
  }

  return result;
}

static struct data_frame *alloc_dataframe(size_t size) {
  struct data_frame *result;

  result = malloc(sizeof(*result));
  if (!result)
    return result;

  result->columns = NULL;
  result->columns_count = 0;
  result->columns_size = 0;
  result->count = 0;
  result->size = size;
  result->_string_rows = malloc(sizeof(result->_string_rows[0]) * size);
  memset(result->_string_rows, 0, sizeof(result->_string_rows[0]) * size);
  return result;
}

static void append_column(struct data_frame *df, struct data_column *column) {
  if (!df->columns || !df->columns_size) {
    const size_t default_size = 8;
    df->columns = malloc(sizeof(df->columns[0]) * default_size);
    df->columns_size = default_size;
    df->columns_count = 0;
  }
  if (df->columns_count >= df->columns_size) {
    df->columns_size *= 2;
    df->columns =
        realloc(df->columns, sizeof(df->columns[0]) * df->columns_size);
  }
  df->columns[df->columns_count++] = column;
}

void get_stats(struct data_frame *df, size_t column, double *min_ptr,
               double *max_ptr, double *mean_ptr) {
  double min, max, mean;
  min = df->columns[column]->data[0].d;
  max = df->columns[column]->data[0].d;
  mean = df->columns[column]->data[0].d;
  for (size_t row = 1; row < df->count; row++) {
    double elem = df->columns[column]->data[row].d;
    if (elem < min)
      min = elem;
    if (elem > max)
      max = elem;

    mean += elem;
  }

  if (min_ptr)
    *min_ptr = min;
  if (max_ptr)
    *max_ptr = max;
  if (mean_ptr)
    *mean_ptr = mean / df->count;
}

void get_std(struct data_frame *df, size_t column, double mean,
             double *std_ptr) {
  double std = 0;
  for (size_t row = 0; row < df->count; row++) {
    double deviation = df->columns[column]->data[row].d - mean;
    std += deviation * deviation;
  }

  if (std_ptr)
    *std_ptr = sqrt(std / df->count);
}

void normalize(struct data_frame *df, size_t column, double min, double max) {
  for (size_t row = 0; row < df->count; row++) {
    df->columns[column]->data[row].d =
        (df->columns[column]->data[row].d - min) / (max - min);
  }
}

void standardize(struct data_frame *df, size_t column, double mean,
                 double std) {

  for (size_t row = 0; row < df->count; row++) {
    df->columns[column]->data[row].d =
        (df->columns[column]->data[row].d - mean) / std;
  }
}

void one_hot(struct data_frame *df, size_t column, size_t count) {
  // WARNING - dynamic array
  struct data_column *columns[count];
  // TODO omp parallel for
  for (size_t i = 0; i < count; i++) {
    const char *name;
    asprintf(&name, "%s:%d", df->columns[column]->name, i);
    columns[i] = alloc_column(name, df->count, TYPE_ONEHOT);
    append_column(df, columns[i]);
  }
  for (size_t row = 0; row < df->count; row++) {
    size_t hot_column = df->columns[column]->data[row].e;
    columns[hot_column]->data[row].oh = true;
  }
}

static void to_slices(struct data_frame *df, struct data_frame *slices,
                      size_t count) {
  size_t rest = df->count;
  size_t slice_size = df->count / count;
  for (size_t i = 0; i < count; i++) {
    struct data_column **columns =
        malloc(sizeof(struct data_column *) * df->columns_count +
               sizeof(struct data_column) * df->columns_count);
    for (size_t j = 0; j < df->columns_count; j++) {
      columns[j] = (char *)columns +
                   sizeof(struct data_column *) * df->columns_count +
                   sizeof(struct data_column) * j;
      memcpy(columns[j], df->columns[j], sizeof(struct data_column));
      columns[j]->data += slice_size * i;
    }
    slices[i].columns = columns;
    slices[i].columns_count = df->columns_count;
    slices[i].columns_size = 0;
    slices[i].count = slice_size;
    slices[i].size = 0;
    slices[i]._string_rows = NULL;

    rest -= slice_size;
  }
  slices[count - 1].count += rest;
}

static void free_slices(struct data_frame *slices, size_t count) {
  for (size_t i = 0; i < count; i++) {
    free(slices[i].columns);
    slices[i].columns = NULL;
  }
}
