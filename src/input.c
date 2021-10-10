/** @file
  Implementacja obsługi standardowego wejścia

  @author Krystyna Gasińska <kg429192@students.mimuw.edu.pl>
  @date 2021
*/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "input.h"
#include "command.h"
#include "error_handle.h"
#include "poly.h"
#include "stack.h"
#include "line.h"
#include "parse.h"
#include "consts.h"

static Poly scanPoly(void);

static Poly endPolyWithError(Mono **monos, size_t monosCount, bool clean);

static Poly scanPolyCoeff(void);

static void allocMonos(bool newArray, size_t *monosMem, Mono **monos);

static Mono scanMono(void);

static void cleanMonos(Mono **monos, bool cleanInside, size_t monosCount);

static Mono endMonoWithError(Poly *p, bool cleanPoly);

static poly_exp_t scanExp(void);

/** Obsługuje standardowe wejście. */
void handleInput(void) {
    int c = getc(stdin);
    int res = newChar((char) c);

    while (res != -1) {
        if (res == 1 || res == 2) {
            resetLineIdx();
            setIgnoreLine(false);

            if (getLength() == 0 || getMyLine()[0] == '#') {
                setIgnoreLine(true);
            } else {
                if (isLetter(getMyLine()[0])) {
                    scanAndDoCommand();
                } else {
                    Poly p = scanPoly();
                    if (getLineIdx() != getLength())
                        handleError("WRONG POLY");
                    if (!getIgnoreLine())
                        putOnStack(p);
                    else
                        PolyDestroy(&p);
                }
            }
        }
        if (res != 2)
            c = getc(stdin);
        res = newChar((char) c);
    }
}

/**
 * Obsługuje wczytanie wielomianu. Wielomian zaczyna się na indeksie @p lineIdx
 * przechowywanym przez moduł obsługujący linię. Poprawny wielomian kończy się
 * przecinkiem lub końcem wiersza. Jeśli w podanym miejscu w tablicy nie
 * znajduje się poprawny wielomian, funkcja zwraca wielomian zerowy.
 * @return wczytany wielomian @p p
 */
static Poly scanPoly(void) {
    if (isOutOfBounds())
        return endPolyWithError(NULL, 0, false);

    if (isDigit(getMyLine()[getLineIdx()]) || getMyLine()[getLineIdx()] == '-')
        return scanPolyCoeff();

    size_t monosCount = 0;
    size_t monosMem;
    Mono *monos;
    allocMonos(true, &monosMem, &monos);

    if (getMyLine()[getLineIdx()] == '(') {
        bool isEnd = false;
        while (!isEnd) {
            Mono m = scanMono();

            if (monosCount >= monosMem)
                allocMonos(false, &monosMem, &monos);

            monos[monosCount] = m;
            monosCount++;

            if (getIgnoreLine())
                return endPolyWithError(&monos, monosCount, true);
            if (isOutOfBounds() || getMyLine()[getLineIdx()] == ',')
                isEnd = true;
            else if (getMyLine()[getLineIdx()] == '+')
                increaseLineIdx();
            else
                return endPolyWithError(&monos, monosCount, true);
        }
        Poly p = PolyAddMonos(monosCount, monos);

        cleanMonos(&monos, false, monosCount);
        return p;
    }
    return endPolyWithError(&monos, monosCount, true);
}

/**
 * Obsługuje błędne zakończenie funkcji odczytującej i interpretującej
 * wielomian. Wywołuje obsługę błędu i zwolnienie wskazanej pamięci.
 * Zwraca wielomian zerowy.
 * @return wielomian zerowy
 */
static Poly endPolyWithError(Mono **monos, size_t monosCount, bool clean) {
    handleError("WRONG POLY");
    if (clean)
        cleanMonos(monos, true, monosCount);
    Poly *dummy = getDummy();
    return *dummy;
}

/**
 * Obsługuje wczytanie wielomianu stałego, czyli liczby. Liczba zaczyna się na
 * indeksie @p lineIdx przechowywanym przez moduł obsługujący linię. Jeśli na
 * podanym miejscu w tablicy nie znajduje się poprawna liczba z odpowiedniego
 * zakresu, funkcja zwraca wielomian zerowy.
 * @return wielomian stały @p p, powstały z odczytanego współczynnika
 */
static Poly scanPolyCoeff(void) {
    if (isOutOfBounds())
        return endPolyWithError(NULL, 0, false);

    size_t beg = getLineIdx();
    bool isEnd = false;

    if (isOutOfBounds())
        isEnd = true;

    while (!isEnd) {
        if (isDigit(getMyLine()[getLineIdx()]) || getMyLine()[getLineIdx()] == '-')
            increaseLineIdx();
        else
            isEnd = true;
        if (isOutOfBounds())
            isEnd = true;
    }

    if (beg == getLineIdx())
        return endPolyWithError(NULL, 0, false);

    size_t end = getLineIdx();
    long res = parseLong(beg, end, "WRONG POLY");

    if (getIgnoreLine()) {
        Poly *dummy = getDummy();
        return *dummy;
    }

    return PolyFromCoeff((poly_coeff_t) res);
}

/**
 * Alokuje pamięć na tablicę jednomianów. W zależności od parametru @p newArray
 * przeznacza na tablicę nową pamięć, lub zwiększa ilość już przeznaczonej
 * pamięci.
 * @param[in] newArray : Czy dla tablicy nie była jeszcze alokowana pamięć?
 * @param[in] monosMem : ilość pamięci jaką teraz dysponuje tablica
 * @param[in] monos : tablica jednomianów
 */
static void allocMonos(bool newArray, size_t *monosMem, Mono **monos) {
    if (newArray) {
        *monosMem = INIT_MEM;
        *monos = malloc(*monosMem * sizeof(Mono));
    } else {
        *monosMem = 2 * *monosMem + ADD_MEM;
        *monos = realloc(*monos, *monosMem * sizeof(Mono));
    }
    allocCheck(*monos);
}

/**
 * Obsługuje wczytanie jednomianu. Jednomian zaczyna się na indeksie @p lineIdx
 * przechowywanym przez moduł obsługujący linię. Poprawny jednomian kończy się
 * znakiem plus, końcem wiersza lub przecinkiem. Jeśli w podanym miejscu w
 * tablicy nie znajduje się poprawny wielomian, funkcja zwraca jednomian
 * @f$0x^0@f$.
 * @return wczytany jednomian @p m
 */
static Mono scanMono(void) {
    if (isOutOfBounds())
        return endMonoWithError(NULL, false);
    if (getMyLine()[getLineIdx()] != '(')
        return endMonoWithError(NULL, false);

    increaseLineIdx();
    Poly p = scanPoly();

    if (isOutOfBounds())
        return endMonoWithError(&p, true);

    if (getMyLine()[getLineIdx()] == ',') {
        increaseLineIdx();
        poly_exp_t exp = scanExp();
        if (getIgnoreLine()) {
            return MonoFromPoly(&p, 0);
        }
        if (getMyLine()[getLineIdx()] != ')') {
            handleError("WRONG POLY");
        } else {
            increaseLineIdx();
            if (PolyIsZero(&p) && exp != 0)
                exp = 0;
            return MonoFromPoly(&p, exp);
        }
    } else {
        handleError("WRONG POLY");
    }

    return endMonoWithError(&p, true);
}

/**
 * Zwalnia pamięć przeznaczoną na tablicę jednomianów. W zależności od
 * parametru @p cleanInside, zwalnia bądź nie, pamięć przeznaczoną na
 * jednomiany zawarte w tablicy.
 * @param[in] monos : tablica jednomianów
 * @param[in] cleanInside : Czy pamięć przeznaczoną na jednomiany należy
 * zwolnić?
 * @param[in] monosCount : liczba jednomianów w tablicy @p monos
 */
static void cleanMonos(Mono **monos, bool cleanInside, size_t monosCount) {
    if (cleanInside)
        for (size_t i = 0; i < monosCount; i++)
            MonoDestroy(&(*monos)[i]);

    free(*monos);
}

/**
 * Obsługuje błędne zakończenie funkcji odczytującej i interpretującej
 * jednomian. Wywołuje obsługę błędu i zwolnienie wskazanej pamięci.
 * Zwraca jednomian @f$0x^0@f$.
 * @return jednomian @f$0x^0@f$
 */
static Mono endMonoWithError(Poly *p, bool cleanPoly) {
    handleError("WRONG POLY");
    if (cleanPoly)
        PolyDestroy(p);
    Poly *dummy = getDummy();
    return (MonoFromPoly(dummy, 0));
}

/**
 * Obsługuje wczytanie wykładnika jednomianu. Liczba, będąca wykładnikiem
 * zaczyna się na indeksie @p lineIdx przechowywanym przez moduł obsługujący
 * linię. Jeśli na podanym miejscu w tablicy nie znajduje się poprawna liczba
 * z odpowiedniego zakresu, funkcja zwraca 0.
 * @return odczytany wykładnik jednomianu
 */
static poly_exp_t scanExp(void) {
    size_t beg = getLineIdx();
    while (isDigit(getMyLine()[getLineIdx()]))
        increaseLineIdx();

    if (beg == getLineIdx()) {
        handleError("WRONG POLY");
        return 0;
    }

    size_t end = getLineIdx();
    long res = parseLong(beg, end, "WRONG POLY");

    if (res < 0 || res > INT_MAX) {
        handleError("WRONG POLY");
    }

    if (getIgnoreLine())
        return 0;

    return (poly_exp_t) res;
}
