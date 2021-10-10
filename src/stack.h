/** @file
  Interfejs stosu wielomianów rzadkich wielu zmiennych

  @author Krystyna Gasińska <kg429192@students.mimuw.edu.pl>
  @date 2021
*/

#ifndef STACK_H
#define STACK_H

#include "poly.h"

const Poly *showStackTop(size_t depth);

void removeFromStack(void);

void putOnStack(Poly p);

void cleanPolys(void);

size_t getStackSize(void);

#endif /* STACK_H */