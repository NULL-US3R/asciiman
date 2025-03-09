#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <ncurses.h>
#include <unistd.h>
#include <stdlib.h>

#define LEVELSIZE 60

#define WIDTH (LEVELSIZE-2)

#define fract(x,f)((x%f)<0?(x%f+50):(x%f))

int main(){
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
    for(int i=0; i<WIDTH+2; i++){
        for(int j=0; j<WIDTH+2; j++){
            level[i][j]=1;
        }
    }

    //start
    // for(int i=WIDTH/2-2; i<WIDTH/2+3; i++){
    //     for(int j=WIDTH/2-2; j<WIDTH/2+3; j++){
    //         level[i][j]=2;
    //     }
    // }
    level[WIDTH/2][WIDTH/2]=3;

    //paths
    #define MAXLEN (WIDTH/2);
    for(int iter=0; iter<WIDTH*5; iter++){
        for(int i=0; i<WIDTH+2; i++){
            for(int j=0; j<WIDTH+2; j++){
                if(level[i][j]==3){
                    int direction = random()%4, length = random()%MAXLEN;
                    switch (direction) {
                        case 0:
                            for(int l=0; l<=length && (j+l)<WIDTH+2; l++){
                                level[i][j+l]=0;
                                if(l==length || (j+l)==WIDTH+1){
                                    level[i][j+l]=3;
                                }
                            }
                        break;
                        case 1:
                            for(int l=0; l<=length && (j-l)>0; l++){
                                level[i][j-l]=0;
                                if(l==length || (j-l)==1){
                                    level[i][j-l]=3;
                                }
                            }
                        break;
                        case 2:
                            for(int l=0; l<=length && (i+l)<WIDTH+2; l++){
                                level[i+l][j]=0;
                                if(l==length || (i+l)==WIDTH+1){
                                    level[i+l][j]=3;
                                }
                            }
                        break;
                        case 3:
                            for(int l=0; l<=length && (i-l)>0; l++){
                                level[i-l][j]=0;
                                if(l==length || (i-l)==1){
                                    level[i-l][j]=3;
                                }
                            }
                        break;
                    }
                }
            }
        }
    }
    for(int i=0; i<WIDTH+2; i++){
        for(int j=0; j<WIDTH+2; j++){
            if(level[i][j]==3){
                level[i][j]=0;
            }
        }
    }

    //points
    #define POINTS 600
    #define NUMPOINTS (POINTS+1)
    int count=0;
    while(count!=NUMPOINTS){
        for(int i=0; i<WIDTH+2; i++){
            for(int j=0; j<WIDTH+2; j++){
                if(level[i][j]==0){
                    if(random()%WIDTH<2){
                        if(count<NUMPOINTS){
                            level[i][j]=2;
                            count++;
                        }
                    }
                }
            }
        }
    }

    //border
    for(int i=0; i<WIDTH+2; i++){
        for(int j=0; j<WIDTH+2; j++){
            if(i==0 || j==0 || i==WIDTH+1 || j==WIDTH+1){
                level[i][j]=1;
            }
        }
    }

    #define FOGDIST 13
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
        //mvprintw(coords[1]+1+yb, coords[0]*2+xb, "@@");
        attroff(COLOR_PAIR(1)|A_BOLD);
        usleep(140000);
        if(level[coords[1]][coords[0]]==2){
            level[coords[1]][coords[0]]=0;
            score++;
        }
        int key = getch();
        if(key=='q'){
            run=0;
        }
        if(key=='w' && v!=1){
            v=1;
        }
        if(key=='s' && v!=2){
            v=2;
        }
        if(key=='a' && v!=3){
            v=3;
        }
        if(key=='d' && v!=4){
            v=4;
        }

        if(v==1){
            if(level[coords[1]-1][coords[0]]==1){
                v=0;
            }else{
                coords[1]--;
            }
        }
        if(v==2){
            if(level[coords[1]+1][coords[0]]==1){
                v=0;
            }else{
                coords[1]++;
            }
        }
        if(v==3){
            if(level[coords[1]][coords[0]-1]==1){
                v=0;
            }else{
                coords[0]--;
            }
        }
        if(v==4){
            if(level[coords[1]][coords[0]+1]==1){
                v=0;
            }else{
                coords[0]++;
            }
        }

        if(coords[0]>WIDTH){
            coords[0]=1;
        }
        if(coords[0]<=0){
            coords[0]=WIDTH;
        }
        if(coords[1]>WIDTH){
            coords[1]=1;
        }
        if(coords[1]<=0){
            coords[1]=WIDTH;
        }
    }
    endwin();
}
