/** @file
  Interfejs obsługi odczytu i parsowania liczb całkowitych, cyfr i liter

  @author Krystyna Gasińska <kg429192@students.mimuw.edu.pl>
  @date 2021
*/

#ifndef CHECK_LINE_H
#define CHECK_LINE_H

#include <stddef.h>
#include <stdbool.h>

bool isLetter(char c);

bool isDigit(char c);

bool isMinusOrDigit(char c);

unsigned long long parseULL(size_t beg, size_t end, char *errorMessage);

long parseLong(size_t beg, size_t end, char *errorMessage);

#endif /* CHECK_LINE_H */