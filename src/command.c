/** @file
  Implementacja obsługi komend kalkulatora wielomianów rzadkich wielu zmiennych

  @author Krystyna Gasińska <kg429192@students.mimuw.edu.pl>
  @date 2021
*/

#include <stdio.h>
#include <stdlib.h>
#include "command.h"
#include "error_handle.h"
#include "stack.h"
#include "poly.h"
#include "consts.h"
#include "parse.h"
#include "output.h"
#include "line.h"

static bool areStringsEq(const char *word1, const char *word2,
                         size_t len1, size_t len2);

static size_t min(size_t x, size_t y);

static void doZero(void);

static void doIsCoeff(void);

static void doIsZero(void);

static void doClone(void);

static void doAdd(void);

static void doMul(void);

static void doNeg(void);

static void doSub(void);

static void doIsEq(void);

static void doDeg(void);

static void doDegBy(void);

static void doAt(void);

static void doPrint(void);

static void doPop(void);

static void doCompose(void);


/**
 * Sprawdza, czy podany wiersz jest poprawną komendą. Jeśli tak, wywołuje
 * wykonanie tej komendy. W przeciwnym razie wywołuje obsługę błędu.
 */
void scanAndDoCommand(void) {
    if (areStringsEq(getMyLine(), "ZERO", getLength(), ZERO_LEN))
        doZero();
    else if (areStringsEq(getMyLine(), "IS_COEFF", getLength(), IS_COEFF_LEN))
        doIsCoeff();
    else if (areStringsEq(getMyLine(), "IS_ZERO", getLength(), IS_ZERO_LEN))
        doIsZero();
    else if (areStringsEq(getMyLine(), "CLONE", getLength(), CLONE_LEN))
        doClone();
    else if (areStringsEq(getMyLine(), "ADD", getLength(), ADD_LEN))
        doAdd();
    else if (areStringsEq(getMyLine(), "MUL", getLength(), MUL_LEN))
        doMul();
    else if (areStringsEq(getMyLine(), "NEG", getLength(), NEG_LEN))
        doNeg();
    else if (areStringsEq(getMyLine(), "SUB", getLength(), SUB_LEN))
        doSub();
    else if (areStringsEq(getMyLine(), "IS_EQ", getLength(), IS_EQ_LEN))
        doIsEq();
    else if (areStringsEq(getMyLine(), "DEG", getLength(), DEG_LEN))
        doDeg();
    else if (areStringsEq(getMyLine(), "DEG_BY",
                          min(getLength(), DEG_BY_LEN), DEG_BY_LEN))
        doDegBy();
    else if (areStringsEq(getMyLine(), "AT",
                          min(getLength(), AT_LEN), AT_LEN))
        doAt();
    else if (areStringsEq(getMyLine(), "PRINT", getLength(), PRINT_LEN))
        doPrint();
    else if (areStringsEq(getMyLine(), "POP", getLength(), POP_LEN))
        doPop();
    else if (areStringsEq(getMyLine(), "COMPOSE",
                          min(getLength(), COMPOSE_LEN), COMPOSE_LEN))
        doCompose();
    else
        handleError("WRONG COMMAND");
}

/**
 * Sprawdza, czy dwa słowa (tablice znaków) o podanych długościach są
 * leksykograficznie identyczne.
 * @param[in] word1 : pierwsze słowo
 * @param[in] word2 : drugie słowo
 * @param[in] len1 : długość pierwszego słowa
 * @param[in] len2 : długość drugiego słowa
 * @return Czy pierwsze i drugie słowo są takie same?
 */
static bool areStringsEq(const char *word1, const char *word2,
                         size_t len1, size_t len2) {
    if (len1 != len2)
        return false;

    for (size_t i = 0; i < len1; i++)
        if (word1[i] != word2[i])
            return false;

    return true;
}

/**
 * Wykonuje komendę 'ZERO'. Jeśli na stosie brakuje wielomianów funkcja
 * wywołuje obsługę błędu. W przeciwnym razie wstawia na wierzchołek stosu
 * wielomian tożsamościowo równy zeru.
 */
static void doZero(void) {
    Poly p = PolyZero();
    putOnStack(p);
}

/**
 * Wykonuje komendę 'IS COEFF'. Jeśli na stosie brakuje wielomianów funkcja
 * wywołuje obsługę błędu. W przeciwnym razie sprawdza, czy wielomian na
 * wierzchołku stosu jest współczynnikiem – wypisuje na standardowe wyjście
 * 0 lub 1.
 */
static void doIsCoeff(void) {
    const Poly *p = showStackTop(0);
    if (getIgnoreLine())
        return;
    if (PolyIsCoeff(p))
        printf("1");
    else
        printf("0");
    printf("\n");
}

/**
 * Wykonuje komendę 'IS ZERO'. Jeśli na stosie brakuje wielomianów funkcja
 * wywołuje obsługę błędu. W przeciwnym razie sprawdza, czy wielomian na
 * wierzchołku stosu jest tożsamościowo równy zeru – wypisuje na standardowe
 * wyjście 0 lub 1.
 */
static void doIsZero(void) {
    const Poly *p = showStackTop(0);
    if (getIgnoreLine())
        return;
    if (PolyIsZero(p))
        printf("1");
    else
        printf("0");
    printf("\n");
}

/**
 * Wykonuje komendę 'CLONE'. Jeśli na stosie brakuje wielomianów funkcja
 * wywołuje obsługę błędu. W przeciwnym razie wstawia na stos kopię wielomianu
 * z wierzchołka.
 */
static void doClone(void) {
    const Poly *p1 = showStackTop(0);
    if (getIgnoreLine())
        return;
    Poly p2 = PolyClone(p1);
    putOnStack(p2);
}

/**
 * Wykonuje komendę 'ADD'. Jeśli na stosie brakuje wielomianów funkcja
 * wywołuje obsługę błędu. W przeciwnym razie dodaje dwa wielomiany z
 * wierzchu stosu, usuwa je ze stosu i wstawia na wierzchołek stosu
 * wielomian będący ich sumą.
 */
static void doAdd(void) {
    const Poly *p2 = showStackTop(1);
    if (getIgnoreLine())
        return;
    const Poly *p1 = showStackTop(0);
    Poly p = PolyAdd(p1, p2);
    removeFromStack();
    removeFromStack();
    putOnStack(p);
}

/**
 * Wykonuje komendę 'MUL'. Jeśli na stosie brakuje wielomianów funkcja
 * wywołuje obsługę błędu. W przeciwnym razie mnoży dwa wielomiany z
 * wierzchu stosu, usuwa je ze stosu i wstawia na wierzchołek stosu
 * wielomian będący ich iloczynem.
 */
static void doMul(void) {
    const Poly *p2 = showStackTop(1);
    if (getIgnoreLine())
        return;
    const Poly *p1 = showStackTop(0);

    Poly p = PolyMul(p1, p2);
    removeFromStack();
    removeFromStack();
    putOnStack(p);
}

/**
 * Wykonuje komendę 'NEG'. Jeśli na stosie brakuje wielomianów funkcja
 * wywołuje obsługę błędu. W przeciwnym razie neguje wielomian na wierzchołku
 * stosu.
 */
static void doNeg(void) {
    const Poly *p1 = showStackTop(0);
    if (getIgnoreLine())
        return;
    Poly p2 = PolyNeg(p1);
    removeFromStack();
    putOnStack(p2);
}

/**
 * Wykonuje komendę 'SUB'. Jeśli na stosie brakuje wielomianów funkcja
 * wywołuje obsługę błędu. W przeciwnym razie odejmuje od wielomianu z
 * wierzchołka wielomian pod wierzchołkiem, usuwa je ze stosu i wstawia na
 * wierzchołek stosu wielomian będący ich różnicą.
 */
static void doSub(void) {
    const Poly *p2 = showStackTop(1);
    if (getIgnoreLine())
        return;
    const Poly *p1 = showStackTop(0);

    Poly p = PolySub(p1, p2);
    removeFromStack();
    removeFromStack();
    putOnStack(p);
}

/**
 * Wykonuje komendę 'IS EQ'. Jeśli na stosie brakuje wielomianów funkcja
 * wywołuje obsługę błędu. W przeciwnym razie sprawdza, czy dwa wielomiany
 * na wierzchu stosu są równe – wypisuje na standardowe wyjście 0 lub 1.
 */
static void doIsEq(void) {
    const Poly *p2 = showStackTop(1);
    if (getIgnoreLine())
        return;
    const Poly *p1 = showStackTop(0);
    if (PolyIsEq(p1, p2))
        printf("1");
    else
        printf("0");
    printf("\n");
}

/**
 * Wykonuje komendę 'DEG'. Jeśli na stosie brakuje wielomianów funkcja wywołuje
 * obsługę błędu. W przeciwnym razie wypisuje na standardowe wyjście stopień
 * wielomianu (−1 dla wielomianu tożsamościowo równego zeru).
 */
static void doDeg(void) {
    const Poly *p = showStackTop(0);
    if (getIgnoreLine())
        return;
    printf("%d\n", PolyDeg(p));
}

/**
 * Wykonuje komendę 'DEG BY'. Jeśli na stosie brakuje wielomianów lub nie
 * został podany argument komendy, lub został podany niepoprawny argument,
 * funkcja wywołuje obsługę odpowiedniego błędu. W przeciwnym razie odczytuje
 * wartość argumentu komendy @p idx. Wypisuje na standardowe wyjście stopień
 * wielomianu ze względu na zmienną o numerze idx (−1 dla wielomianu
 * tożsamościowo równego zeru).
 */
static void doDegBy(void) {
    if (getLength() <= DEG_BY_LEN + 1) {
        handleError("DEG BY WRONG VARIABLE");
        return;
    }
    if (getMyLine()[DEG_BY_LEN] != ' ') {
        handleError("DEG BY WRONG VARIABLE");
        return;
    }

    size_t beg = DEG_BY_LEN + 1;
    size_t end = getLength();

    unsigned long long idx = parseULL(beg, end, "DEG BY WRONG VARIABLE");
    if (getIgnoreLine())
        return;

    const Poly *p = showStackTop(0);
    if (getIgnoreLine())
        return;
    printf("%d\n", PolyDegBy(p, idx));
}

/**
 * Wykonuje komendę 'AT'. Jeśli na stosie brakuje wielomianów lub nie
 * został podany argument komendy, lub został podany niepoprawny argument,
 * funkcja wywołuje obsługę odpowiedniego błędu. W przeciwnym razie odczytuje
 * wartość argumentu komendy @p x. Wylicza wartość wielomianu w punkcie @p x,
 * usuwa wielomian z wierzchołka stosu i wstawia na stos wynik operacji.
 */
static void doAt(void) {
    if (getLength() <= AT_LEN + 1) {
        handleError("AT WRONG VALUE");
        return;
    }
    if (getMyLine()[AT_LEN] != ' ') {
        handleError("AT WRONG VALUE");
        return;
    }

    size_t beg = AT_LEN + 1;
    size_t end = getLength();

    long x = parseLong(beg, end, "AT WRONG VALUE");
    if (getIgnoreLine())
        return;

    const Poly *p1 = showStackTop(0);
    if (getIgnoreLine())
        return;
    Poly p2 = PolyAt(p1, x);
    removeFromStack();
    putOnStack(p2);
}

/**
 * Wykonuje komendę 'PRINT'. Jeśli na stosie brakuje wielomianów funkcja
 * wywołuje obsługę błędu. W przeciwnym razie wywołuję obsługę wypisania
 * na standardowe wyjście wielomianu z wierzchołka stosu.
 */
static void doPrint(void) {
    const Poly *p = showStackTop(0);
    if (getIgnoreLine())
        return;
    printPoly(*p);
    printf("\n");
}

/**
 * Wykonuje komendę 'POP'. Jeśli na stosie brakuje wielomianów funkcja
 * wywołuje obsługę błędu. W przeciwnym razie usuwa wielomian z wierzchołka
 * stosu.
 */
static void doPop(void) {
    showStackTop(0);
    if (getIgnoreLine())
        return;
    removeFromStack();
}

/**
 * Wykonuje komendę 'COMPOSE'. Jeśli na stosie brakuje wielomianów lub nie
 * został podany argument komendy, lub został podany niepoprawny argument,
 * funkcja wywołuje obsługę odpowiedniego błędu. W przeciwnym razie odczytuje
 * wartość argumentu komendy @p k. Zdejmuje ze stosu @f$k+1@f$. wielomianów,
 * odpowiednio: @f$p, q_{k-1}, q_{k-2}, \ldots, q_0@f$. Tworzy wielomian
 * @f$p(q_0, q_1, q_2, \ldots)@f$ i wstawia go na stos.
 */
static void doCompose(void) {
    if (getLength() <= COMPOSE_LEN + 1) {
        handleError("COMPOSE WRONG PARAMETER");
        return;
    }
    if (getMyLine()[COMPOSE_LEN] != ' ') {
        handleError("COMPOSE WRONG PARAMETER");
        return;
    }

    size_t beg = COMPOSE_LEN + 1;
    size_t end = getLength();
    unsigned long long k = parseULL(beg, end, "COMPOSE WRONG PARAMETER");
    if (getIgnoreLine())
        return;

    if (k >= getStackSize()) {
        handleError("STACK UNDERFLOW");
        return;
    }
    const Poly *p = showStackTop(0);
    if (getIgnoreLine())
        return;

    Poly *q = malloc(k * sizeof(Poly));
    allocCheck(q);
    for (size_t i = 1; i <= k; i++) {
        q[k - i] = *showStackTop(i);
        if (getIgnoreLine())
            return;
    }

    Poly res = PolyCompose(p, k, q);

    for (size_t i = 0; i <= k; i++) {
        removeFromStack();
    }
    free(q);
    putOnStack(res);
}

/** Zwraca minimum dwóch liczb. */
static size_t min(size_t x, size_t y) {
    if (x < y) return x;
    return y;
}