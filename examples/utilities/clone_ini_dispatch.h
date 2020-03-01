/*  examples/utilities/clone_ini_dispatch.h  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <confini.h>

/**

  @brief    Makes a newly allocated hard copy of an `IniDispatch`
  @param    source          The `IniDispatch` to copy
  @return   A hard copy of the `IniDispatch` passed as argument

  Example:

  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
  IniDispatch * my_clone = clone_ini_dispatch(dispatch);

  if (!my_clone) {
    fprintf(stderr, "malloc() failed");
    exit(1);
  }

  printf("DATA: |%s|\n", my_clone->data);
  printf("VALUE: |%s|\n", my_clone->value);
  printf("PARENT: |%s|\n", my_clone->append_to);

  free(my_clone);
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  The address returned (and **only that**) must be freed afterwards.

**/
IniDispatch * clone_ini_dispatch (IniDispatch * source) {

  IniDispatch * dest = malloc(sizeof(IniDispatch) + source->d_len + source->v_len + source->at_len + 3);

  if (dest) {

    dest->d_len = source->d_len;
    dest->v_len = source->v_len;
    dest->at_len = source->at_len;
    dest->append_to = memcpy((char *) dest + sizeof(IniDispatch), source->append_to, source->at_len + 1);
    dest->data = memcpy((char *) dest->append_to + dest->at_len + 1, source->data, source->d_len + 1);
    dest->value = dest->data ? memcpy((char *) dest->data + dest->d_len + 1, source->value, source->v_len) : dest->data + dest->d_len + 1;
    dest->value[dest->v_len] = '\0';

  }

  return dest;

}

