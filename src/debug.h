#pragma once

void debug_setup(void);
void log(const char* log_group, const char* msg);
void log(const char* msg);
void log_begin(const char* log_group, const char* msg);
void log_indicate_wait(void);
void log_end(void);
