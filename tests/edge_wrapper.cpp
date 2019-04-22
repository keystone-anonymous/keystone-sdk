//******************************************************************************
// Copyright (c) 2018,  .
// All Rights Reserved. See LICENSE for license details.
//------------------------------------------------------------------------------
#include "edge_wrapper.h"
#include "report.h"
#include <string.h>
/* Really all of this file should be autogenerated, that will happen
   eventually. */

#define OCALL_PRINT_BUFFER 1
#define OCALL_PRINT_VALUE 2
#define OCALL_COPY_REPORT 3
#define OCALL_GET_STRING 4


int edge_init(Keystone* enclave){

  enclave->registerOcallDispatch(incoming_call_dispatch);
  register_call(OCALL_PRINT_BUFFER, print_buffer_wrapper);
  register_call(OCALL_PRINT_VALUE, print_value_wrapper);
  register_call(OCALL_COPY_REPORT, copy_report_wrapper);
  register_call(OCALL_GET_STRING, get_host_string_wrapper);

  edge_call_init_internals((uintptr_t)enclave->getSharedBuffer(),
			   enclave->getSharedBufferSize());
}
void print_buffer_wrapper(void* buffer)
{
  /* For now we assume the call struct is at the front of the shared
   * buffer. This will have to change to allow nested calls. */
  struct edge_call_t* edge_call = (struct edge_call_t*)buffer;

  uintptr_t call_args;
  unsigned long ret_val;
  if(edge_call_args_ptr(edge_call, &call_args) != 0){
    edge_call->return_data.call_status = CALL_STATUS_BAD_OFFSET;
    return;
  }
  
  ret_val = print_buffer((char*)call_args);

  // We are done with the data section for args, use as return region
  // TODO safety check?
  uintptr_t data_section = edge_call_data_ptr();

  memcpy((void*)data_section, &ret_val, sizeof(unsigned long));  

  if( edge_call_setup_ret(edge_call, (void*) data_section, sizeof(unsigned long))){
    edge_call->return_data.call_status = CALL_STATUS_BAD_PTR;
  }
  else{
    edge_call->return_data.call_status = CALL_STATUS_OK;
  }
      
  return;

}

void print_value_wrapper(void* buffer)
{
  /* For now we assume the call struct is at the front of the shared
   * buffer. This will have to change to allow nested calls. */
  struct edge_call_t* edge_call = (struct edge_call_t*)buffer;

  uintptr_t call_args;
  unsigned long ret_val;
  if(edge_call_args_ptr(edge_call, &call_args) != 0){
    edge_call->return_data.call_status = CALL_STATUS_BAD_OFFSET;
    return;
  }
  
  print_value(*(unsigned long*)call_args);

  edge_call->return_data.call_status = CALL_STATUS_OK;
  return;
}

void copy_report_wrapper(void* buffer)
{

  /* For now we assume the call struct is at the front of the shared
   * buffer. This will have to change to allow nested calls. */
  struct edge_call_t* edge_call = (struct edge_call_t*)buffer;

  uintptr_t data_section;
  unsigned long ret_val;
  //TODO check the other side of this
  if(edge_call_get_ptr_from_offset(edge_call->call_arg_offset, sizeof(report_t),
				   &data_section) != 0) {
    edge_call->return_data.call_status = CALL_STATUS_BAD_OFFSET;
    return;
  }
  
  copy_report((void*)data_section);

  edge_call->return_data.call_status = CALL_STATUS_OK;

  return;
}

void get_host_string_wrapper(void* buffer)
{

  /* For now we assume the call struct is at the front of the shared
   * buffer. This will have to change to allow nested calls. */
  struct edge_call_t* edge_call = (struct edge_call_t*)buffer;

  uintptr_t call_args;
  unsigned long ret_val;
  if(edge_call_args_ptr(edge_call, &call_args) != 0){
    edge_call->return_data.call_status = CALL_STATUS_BAD_OFFSET;
    return;
  }

  const char* host_str = get_host_string();
  size_t host_str_len = strlen(host_str)+1;

  // This handles wrapping the data into an edge_data_t and storing it
  // in the shared region.
  if( edge_call_setup_wrapped_ret(edge_call, (void*)host_str, host_str_len)){
    edge_call->return_data.call_status = CALL_STATUS_BAD_PTR;
  }
  else{
    edge_call->return_data.call_status = CALL_STATUS_OK;
  }

  return;
}
