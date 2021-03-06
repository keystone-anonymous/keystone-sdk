//******************************************************************************
// Copyright (c) 2018, The  .
// All Rights Reserved. See LICENSE for license details.
//------------------------------------------------------------------------------
#include <edge_call.h>
#include "edge_wrapper.h"
#include "eapp_utils.h"
#include "string.h"
#include "syscall.h"

void edge_init(){
  /* Nothing for now, will probably register buffers/callsites
     later */
}

void ocall_print_value(unsigned long val){

  unsigned long val_ = val;
  ocall(2, &val_, sizeof(unsigned long), 0, 0);

  return;
}

unsigned long ocall_print_buffer(char* data, size_t data_len){

  unsigned long retval;
  ocall(1, data, data_len, &retval ,sizeof(unsigned long));

  return retval;
}

void ocall_copy_report(void* report, size_t len) {
  ocall(3, report, len, 0, 0);
}
