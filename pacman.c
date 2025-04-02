#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>

#define LEVELSIZE 60

#define WIDTH (LEVELSIZE-2)

#define fract(x,f)((x%(f))<0?(x%(f)+50):(x%(f)))

FILE *log_init(uint seed){
  time_t t = time(NULL);
  char filename[23];
  struct tm *tp = localtime(&t);
  strftime(filename, sizeof(filename), "%d_%m_%Y-%H:%M:%S.log", tp);
  FILE *file = fopen(filename, "w+");
  fprintf(file, "%s\n", asctime(tp));
  fprintf(file, "seed: %d\n", seed);
  return file;
}

void log_movement(FILE *file, int x, int y, int score, int mv) {
  fprintf(file, "x: %d y: %d score: %d move_direction: %d\n", x, y, score, mv);
}

int main(int argc, char * argv[]){
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    curs_set(0);
    start_color();
    init_pair(1, COLOR_YELLOW, COLOR_BLACK);
    //init_pair(2, COLOR_WHITE, COLOR_WHITE);
    int xb,yb;
    getmaxyx(stdscr, yb, xb);
    xb/=4;yb/=2;
    int coords[2] = {WIDTH/2,WIDTH/2}, run = 1, v = 0;
    int level[WIDTH+2][WIDTH+2];
    char * name = argv[1];
    FILE * f = fopen(name, "r+");
    fread(level, sizeof(int), (WIDTH+2)*(WIDTH+2), f);
    uint seed;
    fseek(f, (WIDTH+2)*(WIDTH+2), SEEK_SET);
    fread(&seed, sizeof(uint), 1, f);
    fclose(f);
    FILE * logf = log_init(seed);

    #define FOGDIST 1300
    int score=0;
    while(run){
        getmaxyx(stdscr, yb, xb);
        xb/=2;yb/=2;
        clear();
        for(int i=0; i<WIDTH+2; i++){
            for(int j=0; j<WIDTH+2; j++){
                if(abs(coords[1]-i)>FOGDIST || abs(coords[0]-j)>FOGDIST){
                    mvprintw(yb-WIDTH/2-1+i, xb-WIDTH-1+j*2, " ");
                } else {
                    if(level[i][j]==1){
                        mvprintw(yb-WIDTH/2-1+i, xb-WIDTH-1+j*2, "#");
                    } else {
                        if(level[i][j]==2){
                            mvprintw(yb-WIDTH/2-1+i, xb-WIDTH-1+j*2, ".");
                        } else {
                            mvprintw(yb-WIDTH/2-1+i, xb-WIDTH-1+j*2, " ");
                        }
                    }
                }
            }
        }
        //player
        attron(COLOR_PAIR(1)|A_BOLD);
        mvprintw(coords[1]+yb-WIDTH/2-1, coords[0]*2+xb-WIDTH-1, "@");
        mvprintw(0,0,"x:%i y:%i",coords[0], coords[1]);
        mvprintw(1,0,"score:%i",score);
        attroff(COLOR_PAIR(1)|A_BOLD);
        usleep(140000);
        if(level[coords[1]][coords[0]]==2){
            level[coords[1]][coords[0]]=0;
            score++;
        }
        int key = getch();
        switch (key) {
            case 'q': run=0; break;
            case 'w': v=1; break;
            case 's': v=2; break;
            case 'a': v=3; break;
            case 'd': v=4; break;
        }
        switch (v){
            case 1:
                if(level[coords[1]-1][coords[0]]==1){
                    v=0;
                }else{
                    coords[1]--;
                }
            break;
            case 2:
                if(level[coords[1]+1][coords[0]]==1){
                    v=0;
                }else{
                    coords[1]++;
                }
            break;
            case 3:
                if(level[coords[1]][coords[0]-1]==1){
                    v=0;
                }else{
                    coords[0]--;
                }
            break;
            case 4:
                if(level[coords[1]][coords[0]+1]==1){
                    v=0;
                }else{
                    coords[0]++;
                }
            break;
        }
        coords[0] = fract(coords[0], WIDTH+1);
        coords[1] = fract(coords[1], WIDTH+1);
        log_movement(logf, coords[0], coords[1], score, v);
    }
    endwin();
}
