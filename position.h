#include <windows.h>    // header file for gotoxy
#include <conio.h>      // For getch()
#include <stdio.h>      // header file for standard input output

COORD coord= {0,0};     // this is global variable

void gotoxy(int x,int y)
{
    coord.X=x;
    coord.Y=y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),coord);
}

void print_menu(){
    printf("\n\n\n\t\t\t\t     ------------------------------------------\n");
    printf("\t\t\t\t    |                                          |\n");
    printf("\t\t\t\t    |    FIRST-COME, FIRST-SERVE        [ ]    |\n");
    printf("\t\t\t\t    |    SHORTEST JOB FIRST             [ ]    |\n");
    printf("\t\t\t\t    |    SHORTEST REMAINING TIME FIRST  [ ]    |\n");
    printf("\t\t\t\t    |    NON - PREEMPTIVE PRIORITY      [ ]    |\n");
    printf("\t\t\t\t    |    PREEMPTIVE PRIORITY            [ ]    |\n");
    printf("\t\t\t\t    |    ROUND-ROBIN                    [ ]    |\n");
    printf("\t\t\t\t    |                                          |\n");
    printf("\t\t\t\t     ------------------------------------------\n");
    printf("\t\t\t\t    |                                          |\n");
    printf("\t\t\t\t    |                SUBMIT [ ]                |\n");
    printf("\t\t\t\t    |                                          |\n");
    printf("\t\t\t\t     ------------------------------------------\n");
    printf("\n\n\t\t\t\t       [W] move up [S] move down [Entr] Enable \n");
}

int * select_from_menu(){
    int i, current_selection = 0;
    static int technique[] = {0,0,0,0,0,0,0,0};
    gotoxy(73,5);
    while((!technique[6]) || (technique[7] < 1)){
        i = getch();
        if(i == 119 || i == 87)
            current_selection--;
        else if(i == 115 || i == 83)
            current_selection++;
        else if(i == 13){
            if(technique[current_selection] == 0) {
                technique[current_selection] = 1;
                printf("X");
                if(current_selection == 6 && technique[7] < 1){
                    gotoxy(61,14);
                    technique[current_selection] = 0;
                    printf(" ");
                }
                if(current_selection != 6)
                    technique[7] += 1;
            }
            else if(technique[current_selection] == 1){
                technique[current_selection] = 0;
                printf(" ");
                if(current_selection != 6)
                    technique[7] -= 1;
            }
        }

        if(current_selection < 0)
            current_selection = 6;
        else if(current_selection > 6)
            current_selection = 0;

        switch(current_selection){
            case 0: gotoxy(73,5);
            break;
            case 1: gotoxy(73,6);
            break;
            case 2: gotoxy(73,7);
            break;
            case 3: gotoxy(73,8);
            break;
            case 4: gotoxy(73,9);
            break;
            case 5: gotoxy(73,10);
            break;
            case 6: gotoxy(61,14);
            break;
            default: printf("error");
        }

    }

    return technique;
}
