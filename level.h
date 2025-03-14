/**
 * @file level.h
 * @brief Определение структуры уровня и функций для работы с ним.
 */

#ifndef LEVEL_H
#define LEVEL_H

#include <stddef.h>
#include <stdint.h>

/** @brief Типы объектов в уровне. */
typedef enum {
    PATH,
    BORDER,
    FOOD,
    NOT_GENERATED
} object_type_t;

/** @brief Символы, соответствующие объектам уровня. */
static const char object_chars[] = {
    [PATH]          = ' ',
    [BORDER]        = '#',
    [FOOD]          = '.',
    [NOT_GENERATED] = ' '
};

/** @brief Структура, содержащая параметры уровня. */
typedef struct {
    uint64_t random;
    uint16_t height;
    uint16_t width;
    uint16_t paths;
    uint16_t max_path_length;
    uint16_t points;
    char** level_arr;
} level_t;

/** @brief Фиксированная длина строки seed. */
#define SEED_STRING_LENGTH 45

/** @brief Размер буфера для хранения seed (включая завершающий '\0'). */
#define SEED_BUFFER_SIZE (SEED_STRING_LENGTH + 1)

/**
 * @brief Генерирует seed на основе параметров уровня.
 *
 * @param level Указатель на структуру уровня.
 * @param buffer Буфер для хранения строки seed.
 * @param buffer_size Размер буфера.
 * @return 0 при успешном выполнении, -1 если буфер мал, -2 при ошибке форматирования.
 */
int get_seed(level_t* level, char* buffer, size_t buffer_size);

/**
 * @brief Читает параметры уровня из строки seed.
 *
 * @param buffer Строка, содержащая seed.
 * @param level Указатель на структуру уровня.
 * @return 0 при успешном выполнении, -1 при ошибке аргументов, -2 при неверной длине, -3 при ошибке чтения.
 */
int read_from_string(const char* buffer, level_t* level);

/**
 * @brief Выделяет память для массива уровня.
 *
 * @param level Указатель на структуру уровня.
 * @return 0 при успешном выделении, -1 если размеры уровня некорректны, -2 при ошибке выделения памяти.
 */
int alloc_level(level_t* level);

/**
 * @brief Освобождает память, выделенную для массива уровня.
 *
 * @param level Указатель на структуру уровня.
 */
void free_level(level_t* level);

/**
 * @brief Инициализирует уровень.
 *
 * @param level Указатель на структуру уровня.
 * @param height Высота уровня.
 * @param width Ширина уровня.
 * @param paths Количество путей на уровне.
 * @param max_path_length Максимальная длина пути.
 * @param points Количество очков на уровне.
 */
void init_level(level_t *level, uint16_t height, uint16_t width, uint16_t paths,
                uint16_t max_path_length, uint16_t points);

/**
 * @brief Генерирует уровень.
 *
 * @param level Указатель на структуру уровня.
 */
void generate_level(level_t *level);

/**
 * @brief Генерирует пути на уровне.
 *
 * @param level Указатель на структуру уровня.
 */
void generate_paths(level_t *level);

#endif // LEVEL_H
