/*  examples/utilities/clone_ini_dispatch.h  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <confini.h>

/**

  @brief    Make a newly allocated hard copy of an `IniDispatch`
  @param    source          The `IniDispatch` to copy
  @param    nonconst_parent A placeholder for a non-`const` version of the
                            cloned `IniDispatch::append_to` pointer (it can be
                            `NULL`)
  @return   A hard copy of the `IniDispatch` passed as argument

  Example:

  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
  char * nonconst_parent;

  IniDispatch * my_clone = clone_ini_dispatch(
    dispatch,
    &nonconst_parent
  );

  if (!my_clone) {
    fprintf(stderr, "malloc() failed");
    exit(1);
  }

  printf("DATA: `%s`\n", my_clone->data);
  printf("VALUE: `%s`\n", my_clone->value);
  printf("PARENT: `%s`\n", nonconst_parent);
  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  The address returned (and **only that**) must be freed afterwards.

**/
IniDispatch * clone_ini_dispatch (
  const IniDispatch * const source,
  char ** const nonconst_parent
) {

  IniDispatch * dest = malloc(sizeof(IniDispatch) + source->d_len + source->v_len + source->at_len + 3);

  if (dest) {

    char * _append_to_;
    memcpy(dest, source, sizeof(IniDispatch));
    _append_to_ = memcpy((char *) dest + sizeof(IniDispatch), source->append_to, source->at_len + 1);
    dest->append_to = _append_to_;
    dest->data = memcpy(_append_to_ + dest->at_len + 1, source->data, source->d_len + 1);
    dest->value = dest->data ? memcpy(dest->data + dest->d_len + 1, source->value, source->v_len) : dest->data + dest->d_len + 1;
    dest->value[dest->v_len] = '\0';

    if (nonconst_parent) {
      *nonconst_parent = _append_to_;
    }

  }

  return dest;

}

