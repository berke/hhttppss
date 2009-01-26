/* stack.c */

/* Part of hhttppss, a simple HTTP server skeleton
 * Author: Berke Durak
 * Released in the public domain
 */

#include "bailout.h"
#include "memory.h"
#include "stack.h"

/*(*** stack_create */
stack *stack_create(void)
{
  stack *st;

  st = xmalloc(sizeof(stack));
  st->st_head = 0;
  return st;
}
/***)*/
/*(*** stack_delete */
void stack_delete(stack *st, void (*delete)(void *))
{
  stack_cell *sc1, *sc2;

  sc1 = st->st_head;
  while(sc1) {
    sc2 = sc1->sc_next;
    if(delete) delete(sc1->sc_data);
    xfree(sc1);
    sc1 = sc2;
  }
  xfree(st);
}
/***)*/
/*(*** stack_is_empty */
int stack_is_empty(stack *st)
{
  return !st->st_head;
}
/***)*/
/*(*** stack_push */
void stack_push(stack *st, void *data)
{
  stack_cell *sc;

  sc = xmalloc(sizeof(stack_cell));
  sc->sc_next = st->st_head;
  sc->sc_data = data;
  st->st_head = sc;
}
/***)*/
/*(*** stack_pop */
void *stack_pop(stack *st)
{
  if(st->st_head) {
    stack_cell *sc;
    void *data;

    sc = st->st_head;
    data = sc->sc_data;
    st->st_head = sc->sc_next;
    xfree(sc);
    return data;
  } else {
    bailout("stack_pop: Empty stack");
    return 0; /* Not reached */
  }
}
/***)*/
