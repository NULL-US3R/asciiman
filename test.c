#include "level.h"
#include <stdio.h>
#include <stdlib.h>

int main(){
    level_t level = {
      0, 2, 2, 0, 0, 0, NULL};

    level.level_arr = (char **)malloc(level.height * sizeof(char *));
    if (!level.level_arr) {
      perror("Failed to allocate memory for level_arr");
      return 1;
    }

    // Выделяем память для каждой строки
    for (size_t i = 0; i < level.height; i++) {
      level.level_arr[i] = (char *)malloc(level.width * sizeof(char));
      if (!level.level_arr[i]) {
        perror("Failed to allocate memory for row");
        return 1;
      }
    }

    for (size_t i = 0; i < level.height; i++) {
        for (size_t j = 0; j < level.width; j++){
            (level.level_arr)[i][j] = '%';
        }
    }

    for (size_t i = 0; i < level.height; i++) {
        for (size_t j = 0; j < level.width; j++) {
            printf("%c", (level.level_arr)[i][j]);
        }
        printf("\n");
    }
}