/** @file
  Stałe wykorzystywane w kalkulatorze wielomianów rzadkich wielu zmiennych

  @author Krystyna Gasińska <kg429192@students.mimuw.edu.pl>
  @date 2021
*/

#ifndef CONSTS_H
#define CONSTS_H

/**
 * To jest liczba jednostek pamięci alokowana przy inicjowaniu dynamicznych
 * tablic.
 */
#define INIT_MEM 104

/**
 * To jest dodatkowa liczba jednostek pamięci alokowana przy zwiększaniu
 * pamięci przeznaczonej na dynamiczną tablicę.
 */
#define ADD_MEM 24

/** To jest długość komendy 'ZERO'. */
#define ZERO_LEN 4

/** To jest długość komendy 'IS_COEFF'. */
#define IS_COEFF_LEN 8

/** To jest długość komendy 'IS_ZERO'. */
#define IS_ZERO_LEN 7

/** To jest długość komendy 'CLONE'. */
#define CLONE_LEN 5

/** To jest długość komendy 'ADD'. */
#define ADD_LEN 3

/** To jest długość komendy 'MUL'. */
#define MUL_LEN 3

/** To jest długość komendy 'NEG'. */
#define NEG_LEN 3

/** To jest długość komendy 'SUB'. */
#define SUB_LEN 3

/** To jest długość komendy 'IS_EQ'. */
#define IS_EQ_LEN 5

/** To jest długość komendy 'DEG'. */
#define DEG_LEN 3

/** To jest długość komendy 'DEG_BY' - bez parametru 'idx'. */
#define DEG_BY_LEN 6

/** To jest długość komendy 'AT' - bez parametru 'x'. */
#define AT_LEN 2

/** To jest długość komendy 'PRINT'. */
#define PRINT_LEN 5

/** To jest długość komendy 'POP'. */
#define POP_LEN 3

/** To jest długość komendy 'COMPOSE'. */
#define COMPOSE_LEN 7

#endif /* CONSTS_H */