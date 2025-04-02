#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define LEVELCOUNT 1000
#define LEVELSIZE 60
#define PATHLEN 30
#define POINTS 600
#define COUNTPATH 300

#define WIDTH (LEVELSIZE-2)

void genlevel(char * name, uint seed){
    int level[WIDTH+2][WIDTH+2];
    for(int i=0; i<WIDTH+2; i++){
        for(int j=0; j<WIDTH+2; j++){
            level[i][j]=1;
        }
    }

    srand(seed);

    //start
    level[WIDTH/2][WIDTH/2]=3;

    //paths
    #define MAXLEN PATHLEN;
    for(int iter=0; iter<COUNTPATH; iter++){
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
    FILE * f = fopen(name, "w+");
    fwrite(level, sizeof(int), (WIDTH+2)*(WIDTH+2), f);
    fseek(f, (WIDTH+2)*(WIDTH+2), SEEK_SET);
    fwrite(&seed, sizeof(uint), 1, f);
    fclose(f);
}

int main(int argc, char * argv[]){
    char name[10000], nm[100];
    int l = strlen(argv[2]);
    memcpy(name, argv[2], l);
    for(int i=1; i<=atoi(argv[1]); i++){
        sprintf(nm, "%d.lev", i);
        memcpy(name+l, nm, 100);
        genlevel(name, time(NULL)+i);
    }
    return 0;
}
