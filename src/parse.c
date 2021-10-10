/** @file
  Implementacja obsługi odczytu i parsowania liczb całkowitych, cyfr i liter

  @author Krystyna Gasińska <kg429192@students.mimuw.edu.pl>
  @date 2021
*/

#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include "parse.h"
#include "error_handle.h"
#include "line.h"

/**
 * Sprawdza, czy podany znak jest małą lub dużą literą alfabetu angielskiego.
 * @param[in] c : znak
 * @return Czy znak jest literą?
 */
bool isLetter(char c) {
    if (c >= 'a' && c <= 'z')
        return true;
    if (c >= 'A' && c <= 'Z')
        return true;
    return false;
}

/**
 * Sprawdza, czy podany znak jest cyfrą z zakresu 0 - 9.
 * @param[in] c : znak
 * @return Czy znak jest cyfrą?
 */
bool isDigit(char c) {
    if (c >= '0' && c <= '9')
        return true;
    return false;
}

/**
 * Sprawdza, czy podany znak jest cyfrą z zakresu 0 - 9 lub znakiem '-'.
 * @param[in] c : znak
 * @return Czy znak jest cyfrą?
 */
bool isMinusOrDigit(char c) {
    if (c >= '0' && c <= '9')
        return true;
    if (c == '-')
        return true;
    return false;
}

/**
 * Parsuje fragment wiersza (od @f$beg@f$ do @f$end@f$). Jeśli dany fragment
 * jest poprawną liczbą z zakresu 0 - 18446744073709551615 funkcja zwraca ją.
 * Jeśli nie, wywoływana jest obsługa błędu z komunikatem @f$errorMessage@f$,
 * a funkcja zwraca 0.
 * @param beg : indeks początku parsowanego fragmentu
 * @param end : indeks końca parsowanego fragmentu
 * @param errorMessage : komunikat błędu
 * @return sparsowana liczba
 */
unsigned long long parseULL(size_t beg, size_t end, char *errorMessage) {
    char str[end - beg + 1];
    memcpy(str, &getMyLine()[beg], end - beg);
    str[end - beg] = '\0';

    char *endptr;
    errno = 0;
    unsigned long long x = strtoull(str, &endptr, 10);

    if (endptr != str && isDigit(str[0]) && *endptr == '\0')
        if ((x != 0 && x != ULLONG_MAX) || errno != ERANGE)
            return x;

    handleError(errorMessage);
    return 0;
}

/**
 * Parsuje fragment wiersza (od @f$beg@f$ do @f$end@f$). Jeśli dany fragment
 * jest poprawną liczbą z zakresu -9223372036854775808 - 9223372036854775807
 * funkcja zwraca ją.
 * Jeśli nie, wywoływana jest obsługa błędu z komunikatem @f$errorMessage@f$,
 * a funkcja zwraca 0.
 * @param beg : indeks początku parsowanego fragmentu
 * @param end : indeks końca parsowanego fragmentu
 * @param errorMessage : komunikat błędu
 * @return sparsowana liczba
 */
long parseLong(size_t beg, size_t end, char *errorMessage) {
    char str[end - beg + 1];
    memcpy(str, &getMyLine()[beg], end - beg);
    str[end - beg] = '\0';

    char *endptr;
    errno = 0;
    long x = strtol(str, &endptr, 10);

    if (endptr != str && isMinusOrDigit(str[0]) && *endptr == '\0')
        if ((x != LONG_MIN && x != LONG_MAX) || errno != ERANGE)
            return x;

    handleError(errorMessage);
    return 0;
}