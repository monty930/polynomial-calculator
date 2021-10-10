/** @file
  Implementacja obsługi wiersza znaków

  @author Krystyna Gasińska <kg429192@students.mimuw.edu.pl>
  @date 2021
*/

#include <stdio.h>
#include <stdlib.h>
#include "line.h"
#include "consts.h"
#include "stack.h"
#include "error_handle.h"

/** To jest indeks aktualnie odczytywanego znaku we wczytanym wierszu. */
static size_t lineIdx = 0;

/** To jest aktualnie parsowany wiersz (tablica znaków). */
static char *line = NULL;

/** To jest pamięć zarezerwowana na znaki we wczytywanym wierszu. */
static size_t lineMem = 0;

/**
 * To jest długość aktualnie parsowanego wiersza,
 * bez znaku końca linii lub końca pliku.
 */
static size_t length = 0;

/** To jest licznik wierszy. */
static size_t lineCounter = 0;

/** To jest licznik znaków w wierszu. */
static size_t charCounter = 0;

/** Zmienna ma wartość 'true' jeśli wczytano znak końca pliku (EOF). */
static bool eof = false;

static void endLine(void);

static void endFile(void);

static void allocLine(void);

/**
 * Zwraca wartość zmiennej @p lineIdx, czyli indeks aktualnie
 * odczytywanego znaku.
 * @return @p lineIdx
 */
size_t getLineIdx(void) {
    return lineIdx;
}

/** Ustawia wartość zmiennej @p lineIdx na zero. */
void resetLineIdx(void) {
    lineIdx = 0;
}

/** Zwiększa wartość zmiennej @p o jeden. */
void increaseLineIdx(void) {
    lineIdx++;
}

/**
 * Sprawdza czy wartość zmiennej @p lineIdx jest większa niż wartość indeksu
 * ostatniego znaku w wierszu (o jeden mniej niż długość wiersza).
 * @return Czy @f$lineIdx > length - 1@f$
 */
bool isOutOfBounds(void) {
    if (lineIdx > length - 1)
        return true;
    return false;
}

/**
 * Obsługuje wczytanie znaku. Sprawdza, czy znak zakończył wiersz lub całe
 * wejście, jeśli nie, wstawia go na odpowiednie miejsce w tablicy znaków.
 * Zwiększa wartość licznika znaków. Zwraca wartość 1, w przypadku znaku końca
 * linii, 2, w przypadku znaku końca pliku, -1, jeśli poprzednio wczytany znak
 * był znakiem końca pliku, lub 0 w przeciwnym wypadku.
 * @param[in] c : znak odczytany ze standardowego wejścia
 * @return wartość -1, 0, 1 lub 2 w zależności od znaku @p c
 */
int newChar(char c) {
    if (eof == true) {
        endFile();
        return -1;
    }
    if (c == EOF) {
        eof = true;
        endLine();
        return 2;
    }
    if (c == '\n') {
        endLine();
        return 1;
    }
    allocLine();
    line[charCounter] = c;
    charCounter++;
    return 0;
}

/**
 * Zwraca wartość zmiennej @p lineCounter, czyli licznika wierszy.
 * @return @p lineCounter
 */
size_t getLineCounter(void) {
    return lineCounter;
}

/**
 * Zwraca wiersz, czyli tablicę znaków.
 * @return @p line
 */
char *getMyLine(void) {
    return line;
}

/**
 * Zwraca wartość zmiennej @p length, czyli długość aktualnie badanego wiersza.
 * @return @p length
 */
size_t getLength(void) {
    return length;
}

/**
 * Obsługuje koniec wczytywanego wiersza. Resetuje licznik znaków w wierszu
 * i ustawia poprawną długość zakończonego wiersza. Zwiększa licznik wierszy
 * o jeden.
 */
static void endLine(void) {
    lineCounter++;
    length = charCounter;
    charCounter = 0;
}

/**
 * Obsługuje koniec standardowego wejścia. Zwalnia pamięć przeznaczoną na
 * znaki w wierszu i na wielomiany na stosie.
 */
static void endFile(void) {
    free(line);
    cleanPolys();
}

/**
 * Alokuje pamięć na znaki w wierszu, jeśli długość wiersza przekracza
 * poprzednio przeznaczoną pamięć, lub jeśli wiersz nie ma jeszcze
 * przeznaczonej pamięci. Jeśli alokacja zakończy się niepowodzeniem,
 * funkcja kończy program z kodem 1.
 */
static void allocLine(void) {
    if (lineMem == 0) {
        lineMem = INIT_MEM;
        line = malloc(lineMem * sizeof(char));
        line[0] = '#';
    } else if (charCounter >= lineMem) {
        lineMem = lineMem * 2 + ADD_MEM;
        line = realloc(line, lineMem * sizeof(char));
    }

    allocCheck(line);
}
