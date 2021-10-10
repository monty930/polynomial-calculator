/** @file
  Implementacja klasy wielomianów rzadkich wielu zmiennych

  @author Krystyna Gasińska <kg429192@students.mimuw.edu.pl>
  @date 2021
*/

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include "poly.h"
#include "error_handle.h"

static void alloc(Mono **arr, size_t size, bool newArr);

static Poly PolyAddCons(const Poly *p, const Poly *q);

static Poly PolyAddIncons(const Poly *p, const Poly *q);

static void PolyFixExp(Poly *p);

static int compare(const void *x, const void *y);

static Poly PolyMulCoeff(const Poly *c, const Poly *q);

static Poly PolyMulIncons(const Poly *p, const Poly *q);

static void PolyNegOrig(Poly *p);

static poly_exp_t PolyDegFlat(const Poly *p);

static poly_exp_t max(poly_exp_t x, poly_exp_t y);

static void PolyMulNumber(Poly *p, poly_coeff_t c);

static poly_coeff_t power(poly_coeff_t x, poly_exp_t n);

static void insertMono(Poly *res, size_t *idx,
                       poly_exp_t exp, const Poly *p);

static Poly PolyConstMonoMul(const Mono *m, const Poly *p);

static Poly PolyInconsMonoMul(const Mono *m, const Poly *p);

static Poly PolyMonoMul(const Mono *m, const Poly *p);

static Poly PolyPower(const Poly *p, poly_exp_t exp);

void PolyDestroy(Poly *p) {
    if (!PolyIsCoeff(p)) {
        for (size_t i = 0; i < p->size; i++)
            MonoDestroy(&p->arr[i]);
        free(p->arr);
    }
}

Poly PolyClone(const Poly *p) {
    Poly copy;

    if (PolyIsCoeff(p)) {
        copy = PolyFromCoeff(p->coeff);
    } else {
        copy.size = p->size;
        alloc(&copy.arr, copy.size, true);

        for (size_t i = 0; i < copy.size; i++)
            copy.arr[i] = MonoClone(&p->arr[i]);
    }
    return copy;
}

Poly PolyAdd(const Poly *p, const Poly *q) {
    Poly res;
    if (PolyIsZero(p)) {
        res = PolyClone(q);
    } else if (PolyIsZero(q)) {
        res = PolyClone(p);
    } else if (PolyIsCoeff(p)) {
        if (PolyIsCoeff(q)) // Wielomiany p i q to wielomiany stałe.
            res = PolyFromCoeff(p->coeff + q->coeff);
        else // Wielomian q to wielomian niestały, p - stały.
            res = PolyAddCons(p, q);
    } else if (PolyIsCoeff(q)) { // Wielomian q jest stały, p - nie.
        res = PolyAdd(q, p);
    } else { // Wielomiany p i q, to wielomiany niestałe.
        res = PolyAddIncons(p, q);
    }
    PolyFixExp(&res);
    return res;
}

Poly PolyAddMonos(size_t count, const Mono monos[]) {
    if (count == 0 || monos == NULL)
        return PolyZero();

    Poly res;
    res.size = count;
    alloc(&res.arr, count, true);

    for (size_t i = 0; i < count; i++)
        res.arr[i] = monos[i];

    qsort(res.arr, count, sizeof(Mono), compare);
    size_t lr = 0;

    for (size_t i = 1; i < count; i++) {
        if (MonoGetExp(&res.arr[i]) == MonoGetExp(&res.arr[lr])) {
            Poly polyTemp = res.arr[lr].p;
            res.arr[lr].p = PolyAdd(&polyTemp, &res.arr[i].p);
            PolyDestroy(&res.arr[i].p);
            PolyDestroy(&polyTemp);
            res.arr[lr].exp = MonoGetExp(&res.arr[i]);
        } else {
            lr++;

            if (PolyIsZero(&res.arr[lr - 1].p)) {
                PolyDestroy(&res.arr[lr - 1].p);
                lr--;
            }
            if (lr != i) {
                res.arr[lr] = MonoClone(&res.arr[i]);
                MonoDestroy(&res.arr[i]);
            }
        }
    }

    if (PolyIsZero(&res.arr[lr].p)) {
        PolyDestroy(&res.arr[lr].p);

        if (lr == 0) {
            free(res.arr);
            return PolyZero();
        } else {
            lr--;
        }
    }

    res.size = lr + 1;
    alloc(&res.arr, lr + 1, false);
    PolyFixExp(&res);
    return res;
}

Poly PolyOwnMonos(size_t count, Mono *monos) {
    Poly res = PolyAddMonos(count, monos);
    free(monos);
    return res;
}

Poly PolyCloneMonos(size_t count, const Mono monos[]) {
    Mono *monosCloned = malloc(count * sizeof(Mono));
    allocCheck(monosCloned);
    for (size_t i = 0; i < count; i++) {
        monosCloned[i] = MonoClone(&monos[i]);
    }
    return PolyOwnMonos(count, monosCloned);
}

Poly PolyMul(const Poly *p, const Poly *q) {
    if (PolyIsZero(p) || PolyIsZero(q))
        return PolyZero();
    if (PolyIsCoeff(p) && PolyIsCoeff(q))
        return PolyFromCoeff(p->coeff * q->coeff);
    if (PolyIsCoeff(p))
        return PolyMulCoeff(p, q);
    if (PolyIsCoeff(q))
        return PolyMul(q, p);
    return PolyMulIncons(p, q);
}

Poly PolyCompose(const Poly *p, size_t k, const Poly q[]) {
    Poly res;
    if (PolyIsCoeff(p)) {
        res = PolyClone(p);
    } else {
        res = PolyZero();
        poly_exp_t prevExp = 0;
        Poly prevPoly = PolyFromCoeff(1);

        for (size_t i = 0; i < p->size; i++) {
            Poly clonedResult = PolyClone(&res);
            PolyDestroy(&res);
            Poly composedRecursively;
            if (k > 0) {
                composedRecursively = PolyCompose(&p->arr[i].p, k - 1, &q[1]);
            } else {
                composedRecursively = PolyCompose(&p->arr[i].p, 0, NULL);
            }

            if (q == NULL || k == 0) {
                if (p->arr[i].exp != 0) res = clonedResult;
                else {
                    res = PolyAdd(&composedRecursively, &clonedResult);
                    PolyDestroy(&clonedResult);
                }
            } else {
                // Przy obliczaniu potęgi wielomianu wykorzystywana jest potęga
                // tego samego wielomianu obliczona w poprzednim wykonaniu
                // pętli.
                assert(p->arr[i].exp - prevExp >= 0);
                Poly complementPoly = PolyPower(q, p->arr[i].exp - prevExp);
                Poly newPoly = PolyMul(&prevPoly, &complementPoly);
                Poly finalPoly = PolyMul(&composedRecursively, &newPoly);
                res = PolyAdd(&finalPoly, &clonedResult);

                PolyDestroy(&prevPoly);
                prevPoly = newPoly;
                prevExp = p->arr[i].exp;
                PolyDestroy(&clonedResult);
                PolyDestroy(&complementPoly);
                PolyDestroy(&finalPoly);
            }
            PolyDestroy(&composedRecursively);
        }
        PolyDestroy(&prevPoly);
    }
    return res;
}

Poly PolyNeg(const Poly *p) {
    Poly res = PolyClone(p);
    PolyNegOrig(&res);
    return res;
}

Poly PolySub(const Poly *p, const Poly *q) {
    Poly PolyTemp = PolyNeg(q);
    Poly res = PolyAdd(p, &PolyTemp);
    PolyDestroy(&PolyTemp);
    return res;
}

poly_exp_t PolyDegBy(const Poly *p, size_t var_idx) {
    if (PolyIsZero(p))
        return -1;
    if (PolyIsCoeff(p))
        return 0;
    if (var_idx == 0)
        return PolyDegFlat(p);

    poly_exp_t res = 0;

    for (size_t i = 0; i < p->size; i++)
        res = max(res, PolyDegBy(&p->arr[i].p, var_idx - 1));
    return res;
}

poly_exp_t PolyDeg(const Poly *p) {
    if (PolyIsZero(p))
        return -1;
    if (PolyIsCoeff(p))
        return 0;

    poly_exp_t res = 0;

    for (size_t i = 0; i < p->size; i++)
        res = max(res, PolyDeg(&p->arr[i].p) + MonoGetExp(&p->arr[i]));

    return res;
}

bool PolyIsEq(const Poly *p, const Poly *q) {
    if (PolyIsCoeff(p) == true) {
        if (PolyIsCoeff(q)) { // Wielomiany p i q to wielomiany stałe.
            if (p->coeff != q->coeff)
                return false;
        } else { // Wielomian p to wielomian stały, q nie.
            if (q->size != 1)
                return false;
            if (MonoGetExp(q->arr) != 0)
                return false;

            return PolyIsEq(p, &q->arr->p);
        }
    } else {
        if (PolyIsCoeff(q)) // Wielomian q to wielomian stały, p nie.
            return PolyIsEq(q, p);
        else { // Wielomiany p i q to wielomiany niestałe.
            if (q->size != p->size)
                return false;

            for (size_t i = 0; i < p->size; i++) {
                if (MonoGetExp(&q->arr[i]) != MonoGetExp(&p->arr[i]))
                    return false;
                if (!PolyIsEq(&q->arr[i].p, &p->arr[i].p))
                    return false;
            }
        }
    }

    return true;
}

Poly PolyAt(const Poly *p, poly_coeff_t x) {
    Poly res = PolyZero();

    if (PolyIsCoeff(p))
        return *p;

    for (size_t i = 0; i < p->size; i++) {
        poly_exp_t y = MonoGetExp(&p->arr[i]);
        Poly polyTmp1 = PolyClone(&p->arr[i].p);
        Poly polyTmp2 = PolyClone(&res);
        PolyDestroy(&res);

        PolyMulNumber(&polyTmp1, power(x, y));
        res = PolyAdd(&polyTmp1, &polyTmp2);
        PolyDestroy(&polyTmp1);
        PolyDestroy(&polyTmp2);
    }

    return res;
}

/**
 * Alokuje pamięć na tablicę jednomianów wskazanego rozmiaru.
 * Kończy awaryjnie program z kodem 1, jeśli alokacja się nie powiedzie.
 * @param[in] arr : miejsce do zaalokowania pamięci
 * @param[in] size : liczba jednomianów w tablicy
 * @param[in] newArr : Czy tablicę należy stworzyć? "False" oznacza, że
 * zmieniany jest rozmiar, a tablica była stworzona wcześniej.
 */
static void alloc(Mono **arr, size_t size, bool newArr) {
    if (newArr)
        *arr = malloc(size * sizeof(Mono));
    else
        *arr = realloc(*arr, size * sizeof(Mono));

    allocCheck(*arr);
}

/**
 * Dodaje do siebie wielomian stały i wielomian niestały.
 * Jednomiany w wielomianie niestałym oraz w wielomianie wynikowym są
 * posortowane ściśle rosnąco względem wykładników.
 * @param[in] p : wielomian stały @f$p@f$
 * @param[in] q : posortowany wielomian niestały @f$q@f$
 * @return @f$p + q@f$
 */
static Poly PolyAddCons(const Poly *p, const Poly *q) {
    Poly res;

    // Wielomian q zawiera jednomian o wykładniku 0.
    if (MonoGetExp(q->arr) == 0) {
        alloc(&res.arr, q->size, true);
        res.arr->exp = 0;
        res.arr->p = PolyAdd(p, &(q->arr->p));

        size_t beg = 1;

        // Rozważany jest przypadek, w którym współczynnik wielomianu stałego p
        // oraz współczyynik przy zerowej potędze niestałego wielomianu q
        // sumują się do 0.
        if (PolyIsZero(&res.arr->p)) {
            PolyDestroy(&res.arr->p);
            beg = 0;
            if (q->size - 1 == 0) {
                free(res.arr);
                return PolyZero();
            }
            alloc(&res.arr, q->size - 1, false);
        }

        for (size_t i = beg; i < q->size - 1 + beg; i++) {
            res.arr[i].exp = MonoGetExp(&q->arr[i - beg + 1]);
            res.arr[i].p = PolyClone(&q->arr[i - beg + 1].p);
        }

        res.size = q->size + beg - 1;
    }
        // Wielomian q nie zawiera jednomianu o wykładniku 0.
    else {
        alloc(&res.arr, q->size + 1, true);

        res.arr->exp = 0;
        res.arr->p = PolyFromCoeff(p->coeff);

        for (size_t i = 0; i < q->size; i++) {
            res.arr[i + 1].exp = MonoGetExp(&q->arr[i]);
            res.arr[i + 1].p = PolyClone(&q->arr[i].p);
        }

        res.size = q->size + 1;
    }

    return res;
}

/**
 * Dodaje dwa niestałe wielomiany. Jednomiany w obu wielomianach oraz w
 * wielomianie wynikowym są posortowane ściśle rosnąco względem wykładników.
 * @param[in] p : wielomian niestały @f$p@f$
 * @param[in] q : wielomian niestały @f$q@f$
 * @return @f$p + q@f$
 */
static Poly PolyAddIncons(const Poly *p, const Poly *q) {
    Poly res;
    size_t lp = 0, lq = 0, lr = 0;
    alloc(&res.arr, p->size + q->size, true);
    res.size = p->size + q->size;

    while (lp < p->size && lq < q->size) {
        if (MonoGetExp(&p->arr[lp]) == MonoGetExp(&q->arr[lq])) {
            Poly PolyTemp = PolyAdd(&p->arr[lp].p, &q->arr[lq].p);

            if (!PolyIsZero(&PolyTemp))
                insertMono(&res, &lr, MonoGetExp(&p->arr[lp]), &PolyTemp);

            lp++;
            lq++;
            PolyDestroy(&PolyTemp);
        } else {
            if (MonoGetExp(&p->arr[lp]) < MonoGetExp(&q->arr[lq])) {
                insertMono(&res, &lr, MonoGetExp(&p->arr[lp]), &p->arr[lp].p);
                lp++;
            } else {
                insertMono(&res, &lr, MonoGetExp(&q->arr[lq]), &q->arr[lq].p);
                lq++;
            }
        }
    }

    for (; lp < p->size; lp++)
        insertMono(&res, &lr, MonoGetExp(&p->arr[lp]), &p->arr[lp].p);
    for (; lq < q->size; lq++)
        insertMono(&res, &lr, MonoGetExp(&q->arr[lq]), &q->arr[lq].p);

    if (lr == 0) {
        free(res.arr);
        return PolyZero();
    }

    res.size = lr;
    alloc(&res.arr, res.size, false);
    return res;
}

/**
 * Jeśli wielomian @p p składa się z jednomianu o wykładniku 0, czyli jest
 * postaci @f$cx_0^0@f$, funkcja zamienia go na wielomian stały, czyli
 * współczynnik, równy @f$c@f$ (z arr = NULL).
 * @param[in] p : wielomian
 */
static void PolyFixExp(Poly *p) {
    if (!PolyIsCoeff(p) && p->size == 1) {
        if (PolyIsCoeff(&p->arr[0].p) && p->arr[0].exp == 0) {
            p->coeff = p->arr[0].p.coeff;
            free(p->arr);
            p->arr = NULL;
        }
    }
}

/**
 * Porównuje dwa jednomiany, ze względu na ich wykładnik. Zwraca 1 jeśli
 * pierwszy jest większy, -1 jeśli drugi, 0 jeśli są równe.
 * @param[in] x : jednomian
 * @param[in] y : jednomian
 * @return wynik porównywania
 */
static int compare(const void *x, const void *y) {
    Mono m = *(Mono *) x;
    Mono n = *(Mono *) y;

    if (MonoGetExp(&m) > MonoGetExp(&n))
        return 1;
    else if (MonoGetExp(&m) < MonoGetExp(&n))
        return -1;
    return 0;
}

/**
 * Mnoży dwa wielomiany, z których pierwszy jest wielomianem stałym, a drugi
 * niestałym. Jednomiany w danym niestałym wielomianie i wielomianie wyjściowym
 * są posortowane ściśle rosnąco względem wykładników.
 * @param[in] c : wielomian stały @f$c@f$
 * @param[in] q : wielomian @f$q@f$
 * @return @f$c * q@f$
 */
static Poly PolyMulCoeff(const Poly *c, const Poly *q) {
    if (c->coeff == 0 || PolyIsZero(q))
        return PolyZero();

    Poly res;
    res.size = q->size;
    alloc(&res.arr, res.size, true);
    size_t sizeTemp = res.size;
    size_t j = 0;

    for (size_t i = 0; i < sizeTemp; i++) {
        Mono monoTemp;
        monoTemp.exp = MonoGetExp(&q->arr[j]);
        monoTemp.p = PolyMul(c, &q->arr[j].p);
        if (PolyIsZero(&monoTemp.p)) {
            i--;
            sizeTemp--;
        } else {
            res.arr[i] = monoTemp;
        }
        j++;
    }

    if (sizeTemp == 0) {
        free(res.arr);
        return PolyZero();
    }

    res.size = sizeTemp;
    alloc(&res.arr, sizeTemp, false);
    return res;
}

/**
 * Mnoży dwa wielomiany, z których żaden nie jest wielomianem stałym.
 * Jednomiany w obu wielomianach oraz w wielomianie wynikowym są posortowane
 * ścieśle rosnąco względem wykładników
 * @param[in] p : posortowany wielomian niestały @f$c@f$
 * @param[in] q : posortowany wielomian niestały @f$p@f$
 * @return @f$p * q@f$
 */
static Poly PolyMulIncons(const Poly *p, const Poly *q) {
    Poly res = PolyZero();

    for (size_t i = 0; i < p->size; i++) {
        Poly clone = PolyClone(&res);
        PolyDestroy(&res);
        Poly poly = PolyMonoMul(&p->arr[i], q);
        res = PolyAdd(&clone, &poly);
        PolyDestroy(&clone);
        PolyDestroy(&poly);
    }

    return res;
}

/**
 * Zmienia wielomian na przeciwny
 * @param[in] p : wielomian @f$p@f$
 */
static void PolyNegOrig(Poly *p) {
    if (PolyIsCoeff(p))
        p->coeff *= -1;
    else
        for (size_t i = 0; i < p->size; i++)
            PolyNegOrig(&p->arr[i].p);
}

/**
 * Zwraca stopień wielomianu ze względu na zmienną @f$x_0@f$. Jednomiany danego
 * wielomianu są posortowane ściśle rosnąco względem wykładników.
 * @param[in] p : posortowany wielomian @f$p@f$
 * @return stopień wielomianu @f$p@f$ z względu na zmienną @f$x_0@f$
 */
static poly_exp_t PolyDegFlat(const Poly *p) {
    if (PolyIsZero(p))
        return -1;
    if (PolyIsCoeff(p))
        return 0;
    return MonoGetExp(&p->arr[p->size - 1]);
}

/**
 * Zwraca maksimum dwóch stopni wielomianów (liczb).
 * @param[in] x : pierwszy stopień: @f$x@f$
 * @param[in] y : drugi stopień: @f$y@f$
 * @return większa z liczb @f$x@f$ i @f$y@f$ lub pierwsza z nich jeśli są równe
 */
static poly_exp_t max(poly_exp_t x, poly_exp_t y) {
    if (y > x)
        return y;
    return x;
}

/**
 * Mnoży wielomian przez liczbę. Jednomiany w danym wielomianie i wielomianie
 * wyjściowym są posortowane rosnoąco względem wykładników
 * @param[in] p : wielomian
 * @param[in] c : liczba
 */
static void PolyMulNumber(Poly *p, poly_coeff_t c) {
    if (c == 0) {
        PolyDestroy(p);
        *p = PolyZero();
    } else if (PolyIsCoeff(p) && c != 1) {
        p->coeff = p->coeff * c;
    } else if (c != 1) {
        for (size_t i = 0; i < p->size; i++)
            PolyMulNumber(&p->arr[i].p, c);
    }
}

/**
 * Podnosi liczbę do podanej potęgi
 * @param[in] x : liczba @f$x@f$
 * @param[in] n : potęga @f$n@f$
 * @return @f$x^n@f$
 */
static poly_coeff_t power(poly_coeff_t x, poly_exp_t n) {
    if (x == 1) {
        return 1;
    }
    if (n == 0) {
        return 1;
    }
    if (n % 2 == 1) {
        poly_coeff_t y = power(x, (n - 1) / 2);
        return (x * y * y);
    }
    poly_coeff_t y = power(x, n / 2);
    return (y * y);
}

/**
 * Tworzy jednomian z kopii danego wielomianu i danego wykładnika.
 * Wstawia ten jednomian w odpowiednie miejsce w (innym) danym wielomianie.
 * Funkcja zakłada, że rozmiar tablicy jest większy niż podany indeks.
 * @param[in] res : wielomian @f$res@f$, do którego wstawiony zostanie jednomian
 * @param[in] idx : indeks komórki w tablicy @f$idx@f$
 * @param[in] exp : wykładnik
 * @param[in] p : wielomian @f$p@f$, z którego kopii powstanie jednomian
 * @return  wielomian @f$res@f$, z jednomianem na @f$idx@f$ miejscu w tablicy
 * równym: @f$px_0^{exp}@f$
 */
static void insertMono(Poly *res, size_t *idx,
                       poly_exp_t exp, const Poly *p) {
    assert(*idx < res->size);
    res->arr[*idx].exp = exp;
    res->arr[*idx].p = PolyClone(p);
    (*idx)++;
}

/**
 * Mnoży wielomian stały przez jednomian. Jednomiany w danym wielomianie
 * niestałym oraz wielomianie wyjściowym są posortowane ściśle rosnąco względem
 * wykładników.
 * @param[in] m : jednomian
 * @param[in] p : wielomian stały @f$p@f$
 * @return @f$m * p@f$
 */
static Poly PolyConstMonoMul(const Mono *m, const Poly *p) {
    Poly res;
    res.size = 1;
    alloc(&res.arr, 1, true);

    Mono monoTemp;
    monoTemp.p = PolyMul(&m->p, p);
    monoTemp.exp = MonoGetExp(m);
    *(res.arr) = monoTemp;

    return res;
}

/**
 * Mnoży wielomian niestały przez jednomian. Jednomiany w danym wielomianie oraz
 * wielomianie wyjściowym są posortowane ściśle rosnąco względem wykładników.
 * @param[in] m : jednomian @f$m@f$
 * @param[in] p : wielomian niestały @f$p@f$
 * @return @f$m * p@f$
 */
static Poly PolyInconsMonoMul(const Mono *m, const Poly *p) {
    Poly res;
    res.size = p->size;
    alloc(&res.arr, p->size, true);

    for (size_t i = 0; i < p->size; i++) {
        Mono monoTemp;
        monoTemp.p = PolyMul(&m->p, &p->arr[i].p);
        monoTemp.exp = MonoGetExp(m) + MonoGetExp(&p->arr[i]);
        res.arr[i] = monoTemp;
    }
    return res;
}

/**
 * Mnoży wielomian przez jednomian. Jednomiany w danym wielomianie oraz
 * wielomianie wyjściowym są posortowane ściśle rosnąco względem wykładników.
 * @param[in] m : jednomian @f$m@f$
 * @param[in] p : wielomian @f$p@f$
 * @return @f$m * p@f$
 */
static Poly PolyMonoMul(const Mono *m, const Poly *p) {
    if (PolyIsCoeff(&m->p))
        if (m->p.coeff == 0)
            return PolyZero();
    if (PolyIsCoeff(p))
        return PolyConstMonoMul(m, p);
    return PolyInconsMonoMul(m, p);
}

/**
 * Podnosi wielomian do potęgi. Dla @f$exp=0@f$ wynikiem jest wielomian stały,
 * równy 1, niezależnie od wartości parametru @p p.
 * @param[in] p : wielomian @f$p@f$
 * @param[in] exp : potęga @f$exp@f$
 * @return @f$p^{exp}@f$
 */
static Poly PolyPower(const Poly *p, poly_exp_t exp) {
    Poly res = PolyFromCoeff(1);
    if (PolyIsEq(p, &res) || exp == 0)
        return res;
    PolyDestroy(&res);
    if (exp % 2 == 1) {
        Poly temp = PolyPower(p, (exp - 1) / 2);
        Poly temp2 = PolyMul(&temp, &temp);
        res = PolyMul(p, &temp2);
        PolyDestroy(&temp);
        PolyDestroy(&temp2);
        return res;
    }
    Poly temp = PolyPower(p, exp / 2);
    res = PolyMul(&temp, &temp);
    PolyDestroy(&temp);
    return res;
}
