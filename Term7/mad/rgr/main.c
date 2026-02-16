#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include "data_frame.h"

#define CPU_NUM 1

void column_as_double(struct data_frame *df, const char *name);
void column_as_enum(struct data_frame *df, const char *name,
                    const char **values);
void column_normalize(struct data_frame *df, const char *name);
void column_standardize(struct data_frame *df, const char *name);
void column_one_hot(struct data_frame *df, const char *name,
                    const char **values);

static void print_time(const char *name, struct timeval from,
                       struct timeval to);
static bool is_invalid_row(struct data_frame *df, size_t row);

const char *protocols[] = {"icmp", "tcp", "udp", NULL};

const char *services[] = {"-",   "dhcp",   "dns", "http", "irc", "mqtt",
                          "ntp", "radius", "ssh", "ssl",  NULL};

const char *attacks[] = {"ARP_poisioning",
                         "DDOS_Slowloris",
                         "DOS_SYN_Hping",
                         "Metasploit_Brute_Force_SSH",
                         "MQTT_Publish",
                         "NMAP_FIN_SCAN",
                         "NMAP_OS_DETECTION",
                         "NMAP_TCP_scan",
                         "NMAP_UDP_SCAN",
                         "NMAP_XMAS_TREE_SCAN",
                         "Thing_Speak",
                         "Wipro_bulb",
                         NULL};

const char *numeric_columns[] = {"id",
                                 "id.orig_p",
                                 "id.resp_p",
                                 "flow_duration",
                                 "fwd_pkts_tot",
                                 "bwd_pkts_tot",
                                 "fwd_data_pkts_tot",
                                 "bwd_data_pkts_tot",
                                 "fwd_pkts_per_sec",
                                 "bwd_pkts_per_sec",
                                 "flow_pkts_per_sec",
                                 "down_up_ratio",
                                 "fwd_header_size_tot",
                                 "fwd_header_size_min",
                                 "fwd_header_size_max",
                                 "bwd_header_size_tot",
                                 "bwd_header_size_min",
                                 "bwd_header_size_max",
                                 "flow_FIN_flag_count",
                                 "flow_SYN_flag_count",
                                 "flow_RST_flag_count",
                                 "fwd_PSH_flag_count",
                                 "bwd_PSH_flag_count",
                                 "flow_ACK_flag_count",
                                 "fwd_URG_flag_count",
                                 "bwd_URG_flag_count",
                                 "flow_CWR_flag_count",
                                 "flow_ECE_flag_count",
                                 "fwd_pkts_payload.min",
                                 "fwd_pkts_payload.max",
                                 "fwd_pkts_payload.tot",
                                 "fwd_pkts_payload.avg",
                                 "fwd_pkts_payload.std",
                                 "bwd_pkts_payload.min",
                                 "bwd_pkts_payload.max",
                                 "bwd_pkts_payload.tot",
                                 "bwd_pkts_payload.avg",
                                 "bwd_pkts_payload.std",
                                 "flow_pkts_payload.min",
                                 "flow_pkts_payload.max",
                                 "flow_pkts_payload.tot",
                                 "flow_pkts_payload.avg",
                                 "flow_pkts_payload.std",
                                 "fwd_iat.min",
                                 "fwd_iat.max",
                                 "fwd_iat.tot",
                                 "fwd_iat.avg",
                                 "fwd_iat.std",
                                 "bwd_iat.min",
                                 "bwd_iat.max",
                                 "bwd_iat.tot",
                                 "bwd_iat.avg",
                                 "bwd_iat.std",
                                 "flow_iat.min",
                                 "flow_iat.max",
                                 "flow_iat.tot",
                                 "flow_iat.avg",
                                 "flow_iat.std",
                                 "payload_bytes_per_second",
                                 "fwd_subflow_pkts",
                                 "bwd_subflow_pkts",
                                 "fwd_subflow_bytes",
                                 "bwd_subflow_bytes",
                                 "fwd_bulk_bytes",
                                 "bwd_bulk_bytes",
                                 "fwd_bulk_packets",
                                 "bwd_bulk_packets",
                                 "fwd_bulk_rate",
                                 "bwd_bulk_rate",
                                 "active.min",
                                 "active.max",
                                 "active.tot",
                                 "active.avg",
                                 "active.std",
                                 "idle.min",
                                 "idle.max",
                                 "idle.tot",
                                 "idle.avg",
                                 "idle.std",
                                 "fwd_init_window_size",
                                 "bwd_init_window_size",
                                 "fwd_last_window_size",
                                 NULL};

int main(void) {
  struct data_frame *df = read_csv(stdin, 1UL << 24);

  // Data read
  struct timeval start, end, ts0, ts1, ts2;
  gettimeofday(&start, 0);

  for (size_t i = 0; numeric_columns[i]; i++)
    column_as_double(df, numeric_columns[i]);

  column_as_enum(df, "proto", protocols);
  column_as_enum(df, "service", services);
  column_as_enum(df, "Attack_type", attacks);

  // Data prepared now
  // Filtering
  gettimeofday(&ts0, 0);
  delete_rows_if(df, is_invalid_row);

  gettimeofday(&ts1, 0);
  for (size_t i = 0; numeric_columns[i]; i++) {
    if (i % 2) {
      // Standardization
      column_standardize(df, numeric_columns[i]);
    } else {
      // Normalization
      column_normalize(df, numeric_columns[i]);
    }
  }

  // One-hot encoding
  gettimeofday(&ts2, 0);
  column_one_hot(df, "proto", protocols);
  column_one_hot(df, "service", services);
  column_one_hot(df, "Attack_type", attacks);

  gettimeofday(&end, 0);
  print_time("summary", start, end);
  print_time("parsing", start, ts0);
  print_time("filtering", ts0, ts1);
  print_time("standardization/normalization", ts1, ts2);
  print_time("one-hot", ts2, end);

  print_dataframe(df, stdout);

  free_dataframe(df);

  return 0;
}

static void print_time(const char *name, struct timeval from,
                       struct timeval to) {
  double time_result =
      (to.tv_sec - from.tv_sec) + (to.tv_usec - from.tv_usec) / 1e6;
  fprintf(stderr, "Time %s: %.03lf seconds\n", name, time_result);
}

void column_as_double(struct data_frame *df, const char *name) {
  ssize_t column;
  column = column_index_by_name(df, name);
  if (column >= 0) {
    parse_as_double(df, column);
  } else {
    fprintf(stderr, "Invalid column name - %s!!!\n", name);
  }
}

void column_normalize(struct data_frame *df, const char *name) {
  ssize_t column;
  column = column_index_by_name(df, name);
  if (column >= 0) {
    double min, max;
    if (df->columns[column]->type != TYPE_DOUBLE) {
      fprintf(stderr, "Unable to normalize column %s - wrong type!!!\n", name);
      return;
    }
    get_stats(df, column, &min, &max, NULL);
    normalize(df, column, min, max);
  } else {
    fprintf(stderr, "Invalid column name - %s!!!\n", name);
  }
}

void column_standardize(struct data_frame *df, const char *name) {
  ssize_t column;
  column = column_index_by_name(df, name);
  if (column >= 0) {
    double mean, std;
    if (df->columns[column]->type != TYPE_DOUBLE) {
      fprintf(stderr, "Unable to normalize column %s - wrong type!!!\n", name);
      return;
    }
    get_stats(df, column, NULL, NULL, &mean);
    get_std(df, column, mean, &std);
    standardize(df, column, mean, std);
  } else {
    fprintf(stderr, "Invalid column name - %s!!!\n", name);
  }
}

void column_as_enum(struct data_frame *df, const char *name,
                    const char **values) {
  ssize_t column;
  column = column_index_by_name(df, name);
  if (column >= 0) {
    parse_as_enum(df, column, values);
  } else {
    fprintf(stderr, "Invalid column name - %s!!!\n", name);
  }
}
void column_one_hot(struct data_frame *df, const char *name,
                    const char **values) {
  ssize_t column, count = 0;
  column = column_index_by_name(df, name);
  if (column >= 0) {
    while (*values) {
      count++;
      values++;
    }
    one_hot(df, column, count);
  } else {
    fprintf(stderr, "Invalid column name - %s!!!\n", name);
  }
}

static bool is_invalid_row(struct data_frame *df, size_t row) {
  double magic = df->columns[0]->data[row].d * df->columns[1]->data[row].d *
                 df->columns[2]->data[row].d;
  return ((int)magic % 10) > 8;
}
