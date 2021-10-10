/** @file
  Implementacja stosu wielomianów rzadkich wielu zmiennych

  @author Krystyna Gasińska <kg429192@students.mimuw.edu.pl>
  @date 2021
*/

#include <stdlib.h>
#include "stack.h"
#include "error_handle.h"
#include "consts.h"

/** To jest stos wielomianów reprezentowany przez dynamiczną tablicę. */
static Poly *polyStack;

/** To jest licznik wielomianów na stosie. */
static size_t polyCount = 0;

/**
 * To jest liczba jednostek pamięci aktualnie przeznaczona na stos
 * wielomianów.
 */
static size_t polyMem = 0;

/** To jest zmienna informująca, czy stos wielomianów został utworzony. */
static bool isStack = false;

/**
 * Zwraca wielomian będący w odległości @p depth od wierzchołka stosu. Dla
 * @f$depth=0@f$ funkcja zwraca wielomian z wierzchołka. Jeśli stos nie jest
 * odpowiednio wysoki, funkcja zwraca wielomian zerowy.
 * @param[in] depth : głębokość żądanego wielomianu od wierzchołka stosu
 * @return wielomian ze stosu lub wielomian zerowy
 */
const Poly *showStackTop(size_t depth) {
    if (polyCount <= depth) {
        handleError("STACK UNDERFLOW");
        return getDummy();
    }
    return &polyStack[polyCount - depth - 1];
}

/**
 * Usuwa wielomian z wierzchołka stosu. Zakłada, że na stosie jest co najmniej
 * jeden wielomian.
 */
void removeFromStack(void) {
    assert(polyCount > 0);
    PolyDestroy(&polyStack[polyCount - 1]);
    polyCount--;
}

/**
 * Umieszcza wielomian na wierzchołku stosu. Sprawdza, czy stos ma przeznaczone
 * odpowiednio dużo pamięci, jeśli nie, powiększa go.
 * @param[in] p : wielomian do umieszczenia na stosie
 */
void putOnStack(Poly p) {
    if (polyMem == 0) {
        isStack = true;
        polyMem = INIT_MEM;
        polyStack = malloc(polyMem * sizeof(Poly));
        allocCheck(polyStack);
    }

    if (polyCount >= polyMem) {
        polyMem = polyMem * 2 + ADD_MEM;
        polyStack = realloc(polyStack, polyMem * sizeof(Poly));
        allocCheck(polyStack);
    }

    *(polyStack + polyCount) = p;
    polyCount++;
}

/**
 * Zwalnia pamięć przeznaczoną na stos i wszystkie aktualnie umieszczone na
 * stosie wielomiany.
 */
void cleanPolys(void) {
    for (size_t i = 0; i < polyCount; i++)
        PolyDestroy(&polyStack[i]);

    if (isStack)
        free(polyStack);
}

/**
 * Zwraca wartość zmiennej @p polyCount, czyli liczbę wielomianów na stosie.
 * @return @p polyCount
 */
size_t getStackSize(void) {
    return polyCount;
}
