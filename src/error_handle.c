/** @file
  Implementacja obsługi błędów standardowego wejścia

  @author Krystyna Gasińska <kg429192@students.mimuw.edu.pl>
  @date 2021
*/

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "error_handle.h"
#include "poly.h"
#include "line.h"

/** To jest zmienna informująca, czy aktualnie wczytywany wiersz powinien
 * być ignorowany. Wiersz jest ignorowany jeśli zawiera błąd, jest pusty
 * lub jest komentarzem.
 */
static bool ignoreLine = false;

/** To jest wielomian zwracany przez funkcje parsujące wielomiany lub
 * jednomiany, w przypadku napotkania błędu.
 */
static Poly dummy;

/** Obsługuje błędy programu. Zmienia wartość zmiennej @p ignoreLine na 'true'
 * i wypisuje dany komunikat o błędzie na standardowe wyjście diagnostyczne.
 * @param[in] message : komunikat
 */
void handleError(const char *message) {
    if (getIgnoreLine() == false)
        fprintf(stderr, "ERROR %zu %s\n", getLineCounter(), message);
    setIgnoreLine(true);
}

/**
 * Sprawdza, czy aktualnie wczytywany wiersz jest ignorowany, tzn. czy jest
 * pusty, jest komentarzem lub zawiera błąd.
 * @return Czy wiersz jest ignorowany?
 */
bool getIgnoreLine(void) {
    return ignoreLine;
}

/** Ustawia wartość zmiennej @p ignoreLine na wskazaną wartość @p
 * newIgnoreLine.
 * @param[in] newIgnoreLine : Czy wczytywany wiersz jest ignorowany?
 */
void setIgnoreLine(bool newIgnoreLine) {
    ignoreLine = newIgnoreLine;
}

/** Tworzy wielomian zerowy i go zwraca.
 * @return wielomian zerowy
 */
Poly *getDummy(void) {
    dummy = PolyZero();
    return &dummy;
}

/** Sprawdza, czy @p memory jest równe NULL. Jeśli tak, kończy program
 * awaryjnie z kodem 1.
 * @param[in] memory : wskaźnik na zaalokowaną pamięć (lub NULL)
 */
void allocCheck(void *memory) {
    if (memory == NULL) {
        printf("tu\n");
        exit(1);
    }
}