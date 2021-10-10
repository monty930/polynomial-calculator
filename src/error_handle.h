/** @file
  Interfejs obsługi błędów standardowego wejścia

  @author Krystyna Gasińska <kg429192@students.mimuw.edu.pl>
  @date 2021
*/

#ifndef ERROR_HANDLE_H
#define ERROR_HANDLE_H

#include "poly.h"

void handleError(const char *message);

bool getIgnoreLine(void);

void setIgnoreLine(bool newIgnoreLine);

Poly *getDummy(void);

void allocCheck(void *memory);

#endif /* ERROR_HANDLE_H */