/* stack.h */

/* Part of hhttppss, a simple HTTP server skeleton
 * Author: Berke Durak
 * Released in the public domain
 */

#ifndef STACK_H
#define STACK_H

typedef struct {
  void *sc_next;
  void *sc_data;
} stack_cell;

typedef struct {
  stack_cell *st_head;
} stack;

stack *stack_create(void);
void stack_delete(stack *st, void (*delete)(void *));
int stack_is_empty(stack *st);
void *stack_pop(stack *st);
void stack_push(stack *st, void *data);

#endif
