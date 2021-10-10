/** @file
  Implementacja obsługi standardowego wyjścia

  @author Krystyna Gasińska <kg429192@students.mimuw.edu.pl>
  @date 2021
*/

#include <stdio.h>
#include "output.h"
#include "poly.h"

/**
 * Wypisuje na standardowe wyjście jednomian w formacie @f$(p,n)@f$.
 * @param[in] m : jednomian
 */
void printMono(Mono m) {
    printf("(");
    printPoly(m.p);
    printf(",%d)", m.exp);
}

/**
 * Wypisuje na standardowe wyjście wielomian w formacie:
 * @f$(p,n) + (p,n) + ... + (p,n)@f$, w przypadku wielomianu będącego sumą
 * jednomianów lub @f$c@f$ w przypadku wielomianu stałego równego @f$c@f$.
 * @param[in] p : wielomian
 */
void printPoly(Poly p) {
    if (PolyIsCoeff(&p)) {
        printf("%ld", p.coeff);
    } else {
        size_t size = p.size;
        for (size_t i = 0; i < size; i++) {
            printMono(*(p.arr + i));
            if (i < size - 1)
                printf("+");
        }
    }
}
