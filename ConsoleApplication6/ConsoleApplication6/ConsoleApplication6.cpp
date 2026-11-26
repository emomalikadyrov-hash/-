#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <locale.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>

#define SIZE 10
#define SHIPS_COUNT 10

struct Ship {
    int length, row, col, isHorizontal, hits, sunk;
};

static const char* RU_COLS[SIZE] = {
    "А","Б","В","Г","Д","Е","Ж","З","И","К"
};

/* Длины кораблей: 4x1, 3x2, 2x3, 1x4 */
static const int SHIP_LENGTHS[SHIPS_COUNT] = {
    1,1,1,1,  2,2,2,  3,3,  4
};

/* ---------- Отчёт ---------- */

static int reportMove = 0;      /* номер хода */
static FILE* reportFile = NULL;

void reportOpen(void) {
    reportFile = fopen("report.txt", "w");
    if (reportFile) {
        fprintf(reportFile, "=== ОТЧЁТ ПО ИГРЕ В МОРСКОЙ БОЙ ===\n");
        fprintf(reportFile, "Флот: 4x1, 3x2, 2x3, 1x4 (всего %d кораблей)\n\n", SHIPS_COUNT);
        fprintf(reportFile, "%-4s %-10s %-10s %s\n", "№", "Кто", "Куда", "Результат");
        fprintf(reportFile, "-----------------------------------------------\n");
        fflush(reportFile);
    }
}

void reportClose(void) {
    if (reportFile) {
        fprintf(reportFile, "\n=== ИТОГ ===\n");
        fprintf(reportFile, "Всего ходов: %d\n", reportMove);
        fclose(reportFile);
        reportFile = NULL;
    }
}

/* who: "Я" или "Противник" */
void reportAdd(const char* who, int r, int c, const char* result) {
    reportMove++;
    if (reportFile) {
        fprintf(reportFile, "%-4d %-10s %2d-%-7s %s\n",
            reportMove, who, r, RU_COLS[c], result);
        fflush(reportFile);
    }
}

void reportShow(void) {
    FILE* f = fopen("report.txt", "r");
    if (!f) {
        printf("[ИНФО] Отчёт ещё не создан — сделайте хотя бы один ход.\n");
        return;
    }
    printf("\n=== СОДЕРЖИМОЕ report.txt ===\n");
    char line[256];
    while (fgets(line, sizeof(line), f)) fputs(line, stdout);
    fclose(f);
    printf("==============================\n");
}

/* ---------- Базовые функции ---------- */

void initBoard(char b[SIZE][SIZE]) {
    int i, j;
    for (i = 0; i < SIZE; ++i)
        for (j = 0; j < SIZE; ++j) b[i][j] = '~';
}

void drawBoard(char b[SIZE][SIZE], int hideShips, const char* title) {
    int i, j;
    printf("\n=== %s ===\n   ", title);
    for (j = 0; j < SIZE; ++j) printf("%s ", RU_COLS[j]);
    printf("\n");
    for (i = 0; i < SIZE; ++i) {
        printf("%2d ", i);
        for (j = 0; j < SIZE; ++j) {
            char c = b[i][j];
            if (hideShips && c == 'O') printf("~ ");
            else                       printf("%c ", c);
        }
        printf("\n");
    }
}

int inside(int r, int c, int len, int h) {
    return h ? (r >= 0 && r < SIZE && c >= 0 && c + len - 1 < SIZE)
        : (c >= 0 && c < SIZE && r >= 0 && r + len - 1 < SIZE);
}

int cellFree(char b[SIZE][SIZE], int r, int c) {
    int dr, dc;
    for (dr = -1; dr <= 1; ++dr)
        for (dc = -1; dc <= 1; ++dc) {
            int nr = r + dr, nc = c + dc;
            if (nr >= 0 && nr < SIZE && nc >= 0 && nc < SIZE && b[nr][nc] != '~')
                return 0;
        }
    return 1;
}

int canPlace(char b[SIZE][SIZE], int r, int c, int len, int h) {
    if (!inside(r, c, len, h)) return 0;
    int i;
    for (i = 0; i < len; ++i) {
        int rr = r, cc = c;
        if (h) cc += i; else rr += i;
        if (!cellFree(b, rr, cc)) return 0;
    }
    return 1;
}

void putShip(char b[SIZE][SIZE], struct Ship* s) {
    int i;
    for (i = 0; i < s->length; ++i) {
        int r = s->row, c = s->col;
        if (s->isHorizontal) c += i; else r += i;
        b[r][c] = 'O';
    }
}

int shipHas(struct Ship* s, int r, int c) {
    int i;
    for (i = 0; i < s->length; ++i) {
        int rr = s->row, cc = s->col;
        if (s->isHorizontal) cc += i; else rr += i;
        if (rr == r && cc == c) return 1;
    }
    return 0;
}

/* ---------- Ввод ---------- */

int readRow(int* row) {
    char buf[32];
    printf("  Строка (0-9): ");
    if (scanf("%31s", buf) != 1) return 0;
    if (buf[0] == 'q' || buf[0] == 'Q' || buf[0] == 'й' || buf[0] == 'Й') return 0;
    if (sscanf(buf, "%d", row) != 1 || *row < 0 || *row >= SIZE) {
        printf("  [ОШИБКА] Нужно число 0..9!\n");
        return -1;
    }
    return 1;
}

int readCol(int* col) {
    char buf[32];
    printf("  Столбец (А-К): ");
    if (scanf("%31s", buf) != 1) return 0;
    if (buf[0] == 'q' || buf[0] == 'Q') return 0;

    unsigned char b0 = (unsigned char)buf[0];
    unsigned char b1 = (unsigned char)buf[1];

    if (b0 >= 'a' && b0 <= 'j') { *col = b0 - 'a'; return 1; }
    if (b0 >= 'A' && b0 <= 'J') { *col = b0 - 'A'; return 1; }

    if (b0 == 0xD0 && b1 != 0) {
        if (b1 >= 0xB0 && b1 <= 0xBA) { *col = b1 - 0xB0; return 1; }
        if (b1 >= 0x90 && b1 <= 0x9A) { *col = b1 - 0x90; return 1; }
    }
    if (b0 >= 0xC0 && b0 <= 0xCA && b1 == 0) {
        int idx = b0 - 0xC0;
        if (idx == 9) return 0;
        if (idx > 9) idx -= 1;
        *col = idx; return 1;
    }
    if (b0 >= 0xE0 && b0 <= 0xEA && b1 == 0) {
        int idx = b0 - 0xE0;
        if (idx == 9) return 0;
        if (idx > 9) idx -= 1;
        *col = idx; return 1;
    }
    printf("  [ОШИБКА] Нужно букву А..К!\n");
    return -1;
}

int readResult(void) {
    while (1) {
        printf("  Результат: (м)имо / (р)анил / (у)бил : ");
        char a[8];
        if (scanf("%7s", a) != 1) return -1;
        char ch = a[0];
        if (ch == 'q' || ch == 'Q') return -1;
        if (ch == 'м' || ch == 'М' || ch == 'm' || ch == 'M') return 0;
        if (ch == 'р' || ch == 'Р' || ch == 'r' || ch == 'R') return 1;
        if (ch == 'у' || ch == 'У' || ch == 'u' || ch == 'U') return 2;
        printf("  [ОШИБКА] Введите м, р или у.\n");
    }
}

/* ---------- Подсказка ---------- */

void printSetupHelp(void) {
    printf("\n--- КАК РАССТАВЛЯТЬ КОРАБЛИ ---\n");
    printf("Флот: 4 однопалубных, 3 двухпалубных, 2 трёхпалубных, 1 четырёхпалубный.\n");
    printf("Для каждого корабля введите:\n");
    printf("  1) Строка   — 0..9\n");
    printf("  2) Столбец  — А..К\n");
    printf("  3) Ориентация: h — вправо, v — вниз\n");
    printf("Указывается ПЕРВАЯ (левая/верхняя) клетка.\n");
    printf("Пример 3-палубного: 5 Г h -> 5-Г, 5-Д, 5-Е\n");
    printf("Пример 2-палубного: 2 Б v -> 2-Б, 3-Б\n");
    printf("Корабли не должны пересекаться и касаться (даже углом).\n");
    printf("-------------------------------\n");
}

/* ---------- Расстановка ---------- */

int setup(char b[SIZE][SIZE], const int* lens, int n, struct Ship s[]) {
    int k;
    initBoard(b);
    printf("\n=== РАССТАНОВКА (всего %d кораблей) ===\n", n);
    printSetupHelp();

    for (k = 0; k < n; ++k) {
        while (1) {
            printf("\nКорабль #%d (длина %d):\n", k + 1, lens[k]);
            drawBoard(b, 0, "МОЁ ПОЛЕ");

            int r, c, o;
            int rr = readRow(&r); if (rr == 0) return 0; if (rr == -1) continue;
            int cc = readCol(&c); if (cc == 0) return 0; if (cc == -1) continue;

            char ob[8];
            printf("  Ориентация (h — вправо, v — вниз): ");
            if (scanf("%7s", ob) != 1) return 0;
            o = (ob[0] == 'h' || ob[0] == 'H' || ob[0] == 'г' || ob[0] == 'Г');

            if (!canPlace(b, r, c, lens[k], o)) {
                printf("  [ОШИБКА] Нельзя поставить здесь.\n");
                printSetupHelp();
                continue;
            }
            s[k].length = lens[k];
            s[k].row = r; s[k].col = c; s[k].isHorizontal = o;
            s[k].hits = 0; s[k].sunk = 0;
            putShip(b, &s[k]);
            break;
        }
    }
    drawBoard(b, 0, "ИТОГОВОЕ МОЁ ПОЛЕ");
    return 1;
}

/* ---------- Противник стрелял в меня ---------- */

int enemyShot(char b[SIZE][SIZE], struct Ship s[], int n) {
    drawBoard(b, 0, "МОЁ ПОЛЕ (стреляет противник)");
    printf("--- Противник стрелял ---\n");
    int r, c;
    int rr = readRow(&r); if (rr == 0) return 0; if (rr == -1) return -1;
    int cc = readCol(&c); if (cc == 0) return 0; if (cc == -1) return -1;

    if (b[r][c] != '~' && b[r][c] != 'O') {
        printf("[ВНИМАНИЕ] Сюда уже стреляли.\n");
        return -1;
    }
    struct Ship* hs = NULL;
    int k;
    for (k = 0; k < n; ++k) if (shipHas(&s[k], r, c)) { hs = &s[k]; break; }

    if (hs == NULL) {
        printf("  (%d, %s) — мимо.\n", r, RU_COLS[c]);
        b[r][c] = '.';
        reportAdd("Противник", r, c, "мимо");
        return 1;
    }
    printf("  (%d, %s) — мой корабль.\n", r, RU_COLS[c]);
    int res = readResult();
    if (res == -1) return 0;
    if (res == 0) { b[r][c] = '.'; reportAdd("Противник", r, c, "мимо"); }
    else if (res == 1) { b[r][c] = 'X'; hs->hits++; reportAdd("Противник", r, c, "ранил"); }
    else {
        hs->hits = hs->length; hs->sunk = 1;
        int i;
        for (i = 0; i < hs->length; ++i) {
            int sr = hs->row, sc = hs->col;
            if (hs->isHorizontal) sc += i; else sr += i;
            b[sr][sc] = '*';
        }
        reportAdd("Противник", r, c, "УБИЛ");
    }
    return 1;
}

/* ---------- Мой выстрел ---------- */

int myShot(char view[SIZE][SIZE]) {
    drawBoard(view, 1, "ПОЛЕ ПРОТИВНИКА (мои отметки)");
    printf("--- Мой выстрел ---\n");
    int r, c;
    int rr = readRow(&r); if (rr == 0) return 0; if (rr == -1) return -1;
    int cc = readCol(&c); if (cc == 0) return 0; if (cc == -1) return -1;

    if (view[r][c] != '~') {
        printf("[ВНИМАНИЕ] Сюда уже стреляли.\n");
        return -1;
    }
    int res = readResult();
    if (res == -1) return 0;
    if (res == 0) { view[r][c] = '.'; reportAdd("Я", r, c, "мимо"); return 1; }
    if (res == 1) { view[r][c] = 'X'; reportAdd("Я", r, c, "ранил"); return 1; }

    view[r][c] = '*';
    reportAdd("Я", r, c, "УБИЛ");
    printf("  Корабль противника длиннее 1? Введите остальные клетки (q — конец).\n");
    while (1) {
        printf("  Клетка (q — конец): ");
        char buf[32];
        if (scanf("%31s", buf) != 1) break;
        if (buf[0] == 'q' || buf[0] == 'Q') break;
        int er;
        if (sscanf(buf, "%d", &er) != 1) continue;
        if (er < 0 || er >= SIZE) { printf("  плохая строка\n"); continue; }
        int ec;
        int cr = readCol(&ec);
        if (cr == 0) break;
        if (cr == -1) continue;
        view[er][ec] = '*';
    }
    return 3;
}

/* ---------- main ---------- */

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    setlocale(LC_ALL, "Russian");

    char myBoard[SIZE][SIZE];
    char enemyView[SIZE][SIZE];
    struct Ship myShips[SHIPS_COUNT];
    int n = SHIPS_COUNT;

    int mySunk = 0, enemySunk = 0;
    int haveSetup = 0, running = 1;

    initBoard(myBoard);
    initBoard(enemyView);
    reportOpen();

    printf("=== МОРСКОЙ БОЙ ===\n");
    printf("Флот: 4x1, 3x2, 2x3, 1x4 (всего %d кораблей).\n", SHIPS_COUNT);
    printf("Вместо любого ввода можно ввести q для выхода.\n");

    while (running) {
        drawBoard(myBoard, 0, "МОЁ ПОЛЕ");
        drawBoard(enemyView, 1, "ПОЛЕ ПРОТИВНИКА (мои отметки)");
        printf("\nСчёт: моих потоплено %d/%d | противника потоплено %d/%d | ходов: %d\n",
            mySunk, n, enemySunk, n, reportMove);

        printf("\n=== МЕНЮ ===\n");
        printf("  1. Расставить мои корабли\n");
        printf("  2. Противник стрелял в меня\n");
        printf("  3. Мой выстрел\n");
        printf("  4. Показать отчёт (report.txt)\n");
        printf("  5. Выход\n");
        printf("Выбор: ");

        int ch = 0;
        if (scanf("%d", &ch) != 1) break;

        if (ch == 1) {
            if (setup(myBoard, SHIP_LENGTHS, n, myShips)) {
                haveSetup = 1;
                mySunk = 0; enemySunk = 0;
                initBoard(enemyView);
                printf("[OK] Корабли расставлены.\n");
            }
        }
        else if (ch == 2) {
            if (!haveSetup) { printf("[ИНФО] сначала п.1.\n"); continue; }
            int res = enemyShot(myBoard, myShips, n);
            if (res == 1) {
                int k, cnt = 0;
                for (k = 0; k < n; ++k) if (myShips[k].sunk) cnt++;
                mySunk = cnt;
                if (mySunk >= n) {
                    drawBoard(myBoard, 0, "МОЁ ПОЛЕ");
                    printf("\n*** ВЫ ПРОИГРАЛИ. ***\n");
                    running = 0;
                }
            }
        }
        else if (ch == 3) {
            if (!haveSetup) { printf("[ИНФО] сначала п.1.\n"); continue; }
            int res = myShot(enemyView);
            if (res == 3) {
                enemySunk++;
                if (enemySunk >= n) {
                    drawBoard(enemyView, 1, "ПОЛЕ ПРОТИВНИКА");
                    printf("\n*** ВЫ ПОБЕДИЛИ! ***\n");
                    running = 0;
                }
            }
        }
        else if (ch == 4) {
            reportShow();
        }
        else if (ch == 5) {
            running = 0;
        }
        else {
            printf("[ОШИБКА] нет такого пункта.\n");
        }
    }

    reportClose();
    printf("\nИгра завершена. Отчёт сохранён в report.txt (ходов: %d).\n", reportMove);
    return 0;
}    