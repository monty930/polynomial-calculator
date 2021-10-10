/** @file
  Interfejs obsługi wiersza znaków

  @author Krystyna Gasińska <kg429192@students.mimuw.edu.pl>
  @date 2021
*/

#ifndef LINE_H
#define LINE_H

#include <stdbool.h>
#include <stddef.h>

size_t getLineIdx(void);

void resetLineIdx(void);

void increaseLineIdx(void);

bool isOutOfBounds(void);

int newChar(char c);

size_t getLineCounter(void);

char *getMyLine(void);

size_t getLength(void);

#endif /* LINE_H */