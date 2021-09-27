#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

#include "color.h"
#include "position.h"

#define MAX 10   //MAXIMUM NUMBER OF PROCESS
#define MIN 3    //MINIMUM NUMBER OF PROCESS


void fill_schedule_arrays();
void reset_values();

void np_gantt_chart(int technique);
void print_np_chart(int overall_time, int arrival_time, int burst_time, int index, int technique);

void preemptive_gantt_chart();
void print_preemptive_chart(int overall_time, int current_arrival_time, int remaining_time, int color);
void free_preemptive_nodes();

void chart_label(int x_start, int x_end, int start_time, int end_time);

void fcfs_fill();
void sjf_fill();
void srtf_fill();
void np_priority_fill();
void p_priority_fill();
void round_robin_fill();

int get_minimum_time(float array[]);
int get_maximum_time(float array[]);

typedef struct data{
    char process_name[50];
    int arrival_time;
    int burst_time;
    int priority;
    int start_time;
    int end_time;
    int response_time;
    int turn_around_time;
    int wait_time;
    int x_start,x_end;
    int remaining_time;
    int current_arrival_time;
}DATA;

typedef struct chart{
    int current_arrival_time;
    int remaining_time;
    int start_time;
    int end_time;
    int index;
    int x_start,x_end;
    struct chart *next;
}CHART;

DATA normal_data[MAX];
DATA fcfs[MAX];
DATA sjf[MAX];
DATA srtf[MAX];
DATA np_priority[MAX];
DATA p_priority[MAX];
DATA round_robin[MAX];

CHART *gantt_chart_head = NULL;
CHART *previous_process = NULL;
CHART *p;

int *enabled_technique;
int time_quantum = -1;
int total_process = 0;
int current_time = 0;
int completed_process = 0;
int minimum = -1;
int prev_minimum = -1;
int y = 0;
int x = 10;

//NON - PREEMPTIVE READY QUEUES
int fcfs_rq[] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
int sjf_rq[] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
int np_priority_rq[] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

float average_rt = 0;
float average_tt = 0;
float average_wt = 0;
float cpu_utl = 0;

float average_rt_set[] = {-1.0, -1.0, -1.0, -1.0, -1.0, -1.0};
float average_tt_set[] = {-1.0, -1.0, -1.0, -1.0, -1.0, -1.0};
float average_wt_set[] = {-1.0, -1.0, -1.0, -1.0, -1.0, -1.0};

int min_rt_index, min_tt_index, min_wt_index;
int max_rt_index, max_tt_index, max_wt_index;

int main()
{
    char prompt = 'A';

    print_menu();
    enabled_technique = select_from_menu();
    system("cls");
    normal_data[total_process].priority = -1;

    while(total_process < MAX){
        if(total_process >= MIN){
            while(1){
                printf("Do you want to add more process? [Y\\N]: ");
                y++;
                scanf(" %c",&prompt);
                if(prompt == 'N' || prompt == 'n' || prompt == 'Y' || prompt == 'y'){
                    getchar();
                    printf("\n");
                    y++;
                    break;
                }
            }
        }
        if(prompt == 'N' || prompt == 'n')
            break;
        if(time_quantum == -1 && enabled_technique[5] == 1){
            printf("Enter Time Quantum: ");
            scanf(" %d",&time_quantum);
            printf("\n");
            y += 2;
            getchar();
        }
        printf("Enter Process Name: ");
        gets(normal_data[total_process].process_name);
        printf("Enter Arrival Time: ");
        scanf(" %d",&normal_data[total_process].arrival_time);
        printf("Enter Burst Time: ");
        scanf(" %d",&normal_data[total_process].burst_time);
        if(enabled_technique[3] == 1 || enabled_technique[4] == 1){
            getchar();
            while(normal_data[total_process].priority < 0){
                printf("Enter Priority: ");
                scanf(" %d",&normal_data[total_process].priority);
                y++;
            }
        }
        getchar();
        printf("\n");
        y += 4;
        total_process++;
        normal_data[total_process].priority = -1;
    }

    fill_schedule_arrays();

    ////FCFS SECTION
    if(enabled_technique[0] == 1){
        fcfs_fill();
        printf("\n\n\t\t ____ FIRST-COME, FIRST SERVE __________________________________________________\n");
        printf("\t\t|                                                                               |\n");
        printf("\t\t|\tColor\t| Name\t| AT\t| BT\t| ST\t| ET\t| RT\t| TT\t| WT\t|\n");
        for(int i = 0; i < total_process; i++){
            printf("\t\t|\t");
            SetColor(i + 1);
            printf("%c%c%c%c\t",219,219,219,219);
            SetColor(7);
            printf("| %s\t| %d\t| %d\t| %d\t| %d\t| %d\t| %d\t| %d\t|\n",fcfs[i].process_name,fcfs[i].arrival_time, fcfs[i].burst_time, fcfs[i].start_time, fcfs[i].end_time, fcfs[i].response_time, fcfs[i].turn_around_time, fcfs[i].wait_time);
            y++;
        }
        printf("\t\t|                                                                               |\n");
        printf("\t\t -------------------------------------------------------------------------------\n\n");
        for(int i = 0; i < total_process; i++){
            average_rt += (float) fcfs[i].response_time;
            average_tt += (float) fcfs[i].turn_around_time;
            average_wt += (float) fcfs[i].wait_time;
            cpu_utl    += (float) fcfs[i].burst_time;
        }
        average_rt /= (float) total_process;
        average_tt /= (float) total_process;
        average_wt /= (float) total_process;
        average_rt_set[0] = average_rt;
        average_tt_set[0] = average_tt;
        average_wt_set[0] = average_wt;
        cpu_utl    /= (float) fcfs[fcfs_rq[total_process - 1]].end_time;
        cpu_utl    *= 100.0;
        printf("          Average RT = %.2f\tAverage TT = %.2f\tAverage WT = %.2f\tCPU Utilization  = %.2f%%",average_rt,average_tt,average_wt,cpu_utl);
        y += 7;
        np_gantt_chart(1);
    }


    ////SJF SECTION
    if(enabled_technique[1] == 1){
        reset_values();
        sjf_fill();
        printf("\n\n\t\t ____ SHORTEST JOB FIRST _______________________________________________________\n");
        printf("\t\t|                                                                               |\n");
        printf("\t\t|\tColor\t| Name\t| AT\t| BT\t| ST\t| ET\t| RT\t| TT\t| WT\t|\n");
        for(int i = 0; i < total_process; i++){
            printf("\t\t|\t");
            SetColor(i + 1);
            printf("%c%c%c%c\t",219,219,219,219);
            SetColor(7);
            printf("| %s\t| %d\t| %d\t| %d\t| %d\t| %d\t| %d\t| %d\t|\n",sjf[i].process_name, sjf[i].arrival_time, sjf[i].burst_time, sjf[i].start_time, sjf[i].end_time, sjf[i].response_time, sjf[i].turn_around_time, sjf[i].wait_time);
            y++;
        }
        printf("\t\t|                                                                               |\n");
        printf("\t\t -------------------------------------------------------------------------------\n\n");
        for(int i = 0; i < total_process; i++){
            average_rt += (float) sjf[i].response_time;
            average_tt += (float) sjf[i].turn_around_time;
            average_wt += (float) sjf[i].wait_time;
            cpu_utl    += (float) sjf[i].burst_time;
        }
        average_rt /= (float) total_process;
        average_tt /= (float) total_process;
        average_wt /= (float) total_process;
        average_rt_set[1] = average_rt;
        average_tt_set[1] = average_tt;
        average_wt_set[1] = average_wt;
        cpu_utl    /= (float) sjf[sjf_rq[total_process - 1]].end_time;
        cpu_utl    *= 100.0;
        printf("          Average RT = %.2f\tAverage TT = %.2f\tAverage WT = %.2f\tCPU Utilization  = %.2f%%",average_rt,average_tt,average_wt,cpu_utl);
        y += 7;
        np_gantt_chart(2);
    }

    ////SRTF SECTION
    if(enabled_technique[2] == 1){
        reset_values();
        srtf_fill();
        printf("\n\n\t\t ____ SHORTEST REMAINING TIME FIRST ____________________________________________\n");
        printf("\t\t|                                                                               |\n");
        printf("\t\t|\tColor\t| Name\t| AT\t| BT\t| ST\t| ET\t| RT\t| TT\t| WT\t|\n");
        for(int i = 0; i < total_process; i++){
            printf("\t\t|\t");
            SetColor(i + 1);
            printf("%c%c%c%c\t",219,219,219,219);
            SetColor(7);
            printf("| %s\t| %d\t| %d\t| %d\t| %d\t| %d\t| %d\t| %d\t|\n",srtf[i].process_name,srtf[i].arrival_time, srtf[i].burst_time, srtf[i].start_time, srtf[i].end_time, srtf[i].response_time, srtf[i].turn_around_time, srtf[i].wait_time);
            y++;
        }
        printf("\t\t|                                                                               |\n");
        printf("\t\t -------------------------------------------------------------------------------\n\n");
        for(int i = 0; i < total_process; i++){
            average_rt += (float) srtf[i].response_time;
            average_tt += (float) srtf[i].turn_around_time;
            average_wt += (float) srtf[i].wait_time;
            cpu_utl    += (float) srtf[i].burst_time;
        }
        average_rt /= (float) total_process;
        average_tt /= (float) total_process;
        average_wt /= (float) total_process;
        average_rt_set[2] = average_rt;
        average_tt_set[2] = average_tt;
        average_wt_set[2] = average_wt;
        cpu_utl    /= (float) previous_process -> end_time;
        cpu_utl    *= 100.0;
        printf("          Average RT = %.2f\tAverage TT = %.2f\tAverage WT = %.2f\tCPU Utilization  = %.2f%%",average_rt,average_tt,average_wt,cpu_utl);
        y += 7;
        preemptive_gantt_chart();
        free_preemptive_nodes();
    }


    ////NON-PREEMPTIVE PRIORITY SECTION
    if(enabled_technique[3] == 1){
        reset_values();
        np_priority_fill();
        printf("\n\n\t ____ NON-PREEMPTIVE PRIORITY __________________________________________________________________\n");
        printf("\t|                                                                                               |\n");
        printf("\t|\tColor\t| Name\t| AT\t| BT\t| PRIORITY\t| ST\t| ET\t| RT\t| TT\t| WT\t|\n");
        for(int i = 0; i < total_process; i++){
            printf("\t|\t");
            SetColor(i + 1);
            printf("%c%c%c%c\t",219,219,219,219);
            SetColor(7);
            printf("| %s\t| %d\t| %d\t| %d\t\t| %d\t| %d\t| %d\t| %d\t| %d\t|\n",np_priority[i].process_name,np_priority[i].arrival_time, np_priority[i].burst_time, np_priority[i].priority, np_priority[i].start_time, np_priority[i].end_time, np_priority[i].response_time, np_priority[i].turn_around_time, np_priority[i].wait_time);
            y++;
        }
        printf("\t|                                                                                               |\n");
        printf("\t -----------------------------------------------------------------------------------------------\n\n");
         for(int i = 0; i < total_process; i++){
            average_rt += (float) np_priority[i].response_time;
            average_tt += (float) np_priority[i].turn_around_time;
            average_wt += (float) np_priority[i].wait_time;
            cpu_utl    += (float) np_priority[i].burst_time;
        }
        average_rt /= (float) total_process;
        average_tt /= (float) total_process;
        average_wt /= (float) total_process;
        average_rt_set[3] = average_rt;
        average_tt_set[3] = average_tt;
        average_wt_set[3] = average_wt;
        cpu_utl    /= (float) np_priority[np_priority_rq[total_process - 1]].end_time;
        cpu_utl    *= 100.0;
        printf("          Average RT = %.2f\tAverage TT = %.2f\tAverage WT = %.2f\tCPU Utilization  = %.2f%%",average_rt,average_tt,average_wt,cpu_utl);
        y += 7;
        np_gantt_chart(3);
    }

    ////PREEMPTIVE PRIORITY SECTION
    if(enabled_technique[4] == 1){
        reset_values();
        p_priority_fill();
        printf("\n\n\t ____ PREEMPTIVE PRIORITY ______________________________________________________________________\n");
        printf("\t|                                                                                               |\n");
        printf("\t|\tColor\t| Name\t| AT\t| BT\t| PRIORITY\t| ST\t| ET\t| RT\t| TT\t| WT\t|\n");
        for(int i = 0; i < total_process; i++){
            printf("\t|\t");
            SetColor(i + 1);
            printf("%c%c%c%c\t",219,219,219,219);
            SetColor(7);
            printf("| %s\t| %d\t| %d\t| %d\t\t| %d\t| %d\t| %d\t| %d\t| %d\t|\n",p_priority[i].process_name,p_priority[i].arrival_time, p_priority[i].burst_time, p_priority[i].priority, p_priority[i].start_time, p_priority[i].end_time, p_priority[i].response_time, p_priority[i].turn_around_time, p_priority[i].wait_time);
            y++;
        }
        printf("\t|                                                                                               |\n");
        printf("\t -----------------------------------------------------------------------------------------------\n\n");
        for(int i = 0; i < total_process; i++){
            average_rt += (float) p_priority[i].response_time;
            average_tt += (float) p_priority[i].turn_around_time;
            average_wt += (float) p_priority[i].wait_time;
            cpu_utl    += (float) p_priority[i].burst_time;
        }
        average_rt /= (float) total_process;
        average_tt /= (float) total_process;
        average_wt /= (float) total_process;
        average_rt_set[4] = average_rt;
        average_tt_set[4] = average_tt;
        average_wt_set[4] = average_wt;
        cpu_utl    /= (float) previous_process -> end_time;
        cpu_utl    *= 100.0;
        printf("          Average RT = %.2f\tAverage TT = %.2f\tAverage WT = %.2f\tCPU Utilization  = %.2f%%",average_rt,average_tt,average_wt,cpu_utl);
        y += 7;
        preemptive_gantt_chart();
        free_preemptive_nodes();
    }

    ////ROUND-ROBIN SECTION
    if(enabled_technique[5] == 1){
        reset_values();
        round_robin_fill();
        printf("\n\n\t\t ____ ROUND - ROBIN (TQ = %d) __________________________________________________\n",time_quantum);
        printf("\t\t|                                                                               |\n");
        printf("\t\t|\tColor\t| Name\t| AT\t| BT\t| ST\t| ET\t| RT\t| TT\t| WT\t|\n");
        for(int i = 0; i < total_process; i++){
            printf("\t\t|\t");
            SetColor(i + 1);
            printf("%c%c%c%c\t",219,219,219,219);
            SetColor(7);
            printf("| %s\t| %d\t| %d\t| %d\t| %d\t| %d\t| %d\t| %d\t|\n",round_robin[i].process_name,round_robin[i].arrival_time, round_robin[i].burst_time, round_robin[i].start_time, round_robin[i].end_time, round_robin[i].response_time, round_robin[i].turn_around_time, round_robin[i].wait_time);
            y++;
        }
        printf("\t\t|                                                                               |\n");
        printf("\t\t -------------------------------------------------------------------------------\n\n");
        for(int i = 0; i < total_process; i++){
            average_rt += (float) round_robin[i].response_time;
            average_tt += (float) round_robin[i].turn_around_time;
            average_wt += (float) round_robin[i].wait_time;
            cpu_utl    += (float) round_robin[i].burst_time;
        }
        average_rt /= (float) total_process;
        average_tt /= (float) total_process;
        average_wt /= (float) total_process;
        average_rt_set[5] = average_rt;
        average_tt_set[5] = average_tt;
        average_wt_set[5] = average_wt;
        cpu_utl    /= (float) previous_process -> end_time;
        cpu_utl    *= 100.0;
        printf("          Average RT = %.2f\tAverage TT = %.2f\tAverage WT = %.2f\tCPU Utilization  = %.2f%%",average_rt,average_tt,average_wt,cpu_utl);
        y += 7;
        preemptive_gantt_chart();
        free_preemptive_nodes();
    }

    if(enabled_technique[7] > 1){
        min_rt_index = get_minimum_time(average_rt_set);
        min_tt_index = get_minimum_time(average_tt_set);
        min_wt_index = get_minimum_time(average_wt_set);
        max_rt_index = get_maximum_time(average_rt_set);
        max_tt_index = get_maximum_time(average_tt_set);
        max_wt_index = get_maximum_time(average_wt_set);
        printf("\n\n\n\n\t ____ COMPARATIVE TABLE ________________________________________________________________________\n");
        printf("\t|                                                                                               |\n");
        printf("\t|\t\t|  Average Response-time   |  Average Turn-Around-Time   |   Average Wait-Time\t|\n");
        printf("\t| --------------------------------------------------------------------------------------------- |\n");
        //printf("\t|       |       |       |       |       |       |       |       |       |       |       |       |\n");
        if(enabled_technique[0]){
            printf("\t| FCFS\t\t|");
            if(average_rt_set[min_rt_index] != average_rt_set[max_rt_index]){
                if(average_rt_set[min_rt_index] == average_rt_set[0])
                    SetColor(2);
                else if(average_rt_set[max_rt_index] == average_rt_set[0])
                    SetColor(14);
            }
            printf("\t   %.2f",average_rt_set[0]);
            SetColor(7);
            if(average_tt_set[min_tt_index] != average_tt_set[max_tt_index]){
                if(average_tt_set[min_tt_index] == average_tt_set[0])
                    SetColor(2);
                else if(average_tt_set[max_tt_index] == average_tt_set[0])
                    SetColor(14);
            }
            printf("\t\t\t\t%.2f",average_tt_set[0]);
            SetColor(7);
            if(average_wt_set[min_wt_index] != average_wt_set[max_wt_index]){
                if(average_wt_set[min_wt_index] == average_wt_set[0])
                    SetColor(2);
                else if(average_wt_set[max_wt_index] == average_wt_set[0])
                    SetColor(14);
            }
            printf("\t\t\t   %.2f\t\t",average_wt_set[0]);
            SetColor(7);
            printf("|\n");

        }
        if(enabled_technique[1]){
            printf("\t| SJF\t\t|");
            if(average_rt_set[min_rt_index] != average_rt_set[max_rt_index]){
                if(average_rt_set[min_rt_index] == average_rt_set[1])
                    SetColor(2);
                else if(average_rt_set[max_rt_index] == average_rt_set[1])
                    SetColor(14);
            }
            printf("\t   %.2f",average_rt_set[1]);
            SetColor(7);
            if(average_tt_set[min_tt_index] != average_tt_set[max_tt_index]){
                if(average_tt_set[min_tt_index] == average_tt_set[1])
                    SetColor(2);
                else if(average_tt_set[max_tt_index] == average_tt_set[1])
                    SetColor(14);
            }
            printf("\t\t\t\t%.2f",average_tt_set[1]);
            SetColor(7);
            if(average_wt_set[min_wt_index] != average_wt_set[max_wt_index]){
                if(average_wt_set[min_wt_index] == average_wt_set[1])
                    SetColor(2);
                else if(average_wt_set[max_wt_index] == average_wt_set[1])
                    SetColor(14);
            }
            printf("\t\t\t   %.2f\t\t",average_wt_set[1]);
            SetColor(7);
            printf("|\n");
        }
        if(enabled_technique[2]){
            printf("\t| SRTF\t\t|");
            if(average_rt_set[min_rt_index] != average_rt_set[max_rt_index]){
                if(average_rt_set[min_rt_index] == average_rt_set[2])
                    SetColor(2);
                else if(average_rt_set[max_rt_index] == average_rt_set[2])
                    SetColor(14);
            }
            printf("\t   %.2f",average_rt_set[2]);
            SetColor(7);
            if(average_tt_set[min_tt_index] != average_tt_set[max_tt_index]){
                if(average_tt_set[min_tt_index] == average_tt_set[2])
                    SetColor(2);
                else if(average_tt_set[max_tt_index] == average_tt_set[2])
                    SetColor(14);
            }
            printf("\t\t\t\t%.2f",average_tt_set[2]);
            SetColor(7);
            if(average_wt_set[min_wt_index] != average_wt_set[max_wt_index]){
                if(average_wt_set[min_wt_index] == average_wt_set[2])
                    SetColor(2);
                else if(average_wt_set[max_wt_index] == average_wt_set[2])
                    SetColor(14);
            }
            printf("\t\t\t   %.2f\t\t",average_wt_set[2]);
            SetColor(7);
            printf("|\n");
        }
        if(enabled_technique[3]){
            printf("\t| NP - PRIORITY |");
            if(average_rt_set[min_rt_index] != average_rt_set[max_rt_index]){
                if(average_rt_set[min_rt_index] == average_rt_set[3])
                    SetColor(2);
                else if(average_rt_set[max_rt_index] == average_rt_set[3])
                    SetColor(14);
            }
            printf("\t   %.2f",average_rt_set[3]);
            SetColor(7);
            if(average_tt_set[min_tt_index] != average_tt_set[max_tt_index]){
                if(average_tt_set[min_tt_index] == average_tt_set[3])
                    SetColor(2);
                else if(average_tt_set[max_tt_index] == average_tt_set[3])
                    SetColor(14);
            }
            printf("\t\t\t\t%.2f",average_tt_set[3]);
            SetColor(7);
            if(average_wt_set[min_wt_index] != average_wt_set[max_wt_index]){
                if(average_wt_set[min_wt_index] == average_wt_set[3])
                    SetColor(2);
                else if(average_wt_set[max_wt_index] == average_wt_set[3])
                    SetColor(14);
            }
            printf("\t\t\t   %.2f\t\t",average_wt_set[3]);
            SetColor(7);
            printf("|\n");
        }
        if(enabled_technique[4]){
            printf("\t| P - PRIORITY  |");
            if(average_rt_set[min_rt_index] != average_rt_set[max_rt_index]){
                if(average_rt_set[min_rt_index] == average_rt_set[4])
                    SetColor(2);
                else if(average_rt_set[max_rt_index] == average_rt_set[4])
                    SetColor(14);
            }
            printf("\t   %.2f",average_rt_set[4]);
            SetColor(7);
            if(average_tt_set[min_tt_index] != average_tt_set[max_tt_index]){
                if(average_tt_set[min_tt_index] == average_tt_set[4])
                    SetColor(2);
                else if(average_tt_set[max_tt_index] == average_tt_set[4])
                    SetColor(14);
            }
            printf("\t\t\t\t%.2f",average_tt_set[4]);
            SetColor(7);
            if(average_wt_set[min_wt_index] != average_wt_set[max_wt_index]){
                if(average_wt_set[min_wt_index] == average_wt_set[4])
                    SetColor(2);
                else if(average_wt_set[max_wt_index] == average_wt_set[4])
                    SetColor(14);
            }
            printf("\t\t\t   %.2f\t\t",average_wt_set[4]);
            SetColor(7);
            printf("|\n");
        }
        if(enabled_technique[5]){
            printf("\t| ROUND - ROBIN |");
           if(average_rt_set[min_rt_index] != average_rt_set[max_rt_index]){
                if(average_rt_set[min_rt_index] == average_rt_set[5])
                    SetColor(2);
                else if(average_rt_set[max_rt_index] == average_rt_set[5])
                    SetColor(14);
            }
            printf("\t   %.2f",average_rt_set[5]);
            SetColor(7);
            if(average_tt_set[min_tt_index] != average_tt_set[max_tt_index]){
                if(average_tt_set[min_tt_index] == average_tt_set[5])
                    SetColor(2);
                else if(average_tt_set[max_tt_index] == average_tt_set[5])
                    SetColor(14);
            }
            printf("\t\t\t\t%.2f",average_tt_set[5]);
            SetColor(7);
            if(average_wt_set[min_wt_index] != average_wt_set[max_wt_index]){
                if(average_wt_set[min_wt_index] == average_wt_set[5])
                    SetColor(2);
                else if(average_wt_set[max_wt_index] == average_wt_set[5])
                    SetColor(14);
            }
            printf("\t\t\t   %.2f\t\t",average_wt_set[5]);
            SetColor(7);
            printf("|\n");
        }
        printf("\t ----------------------------------------------------------------------------------------------- \n\n");
        printf("\t\t\t\t\t\t\t\t\t    MINIMUM =");
        SetColor(2);
        printf(" %c%c%c%c",219,219,219,219);
        SetColor(7);
        printf(" MAXIMUM =");
        SetColor(14);
        printf(" %c%c%c%c",219,219,219,219);
        SetColor(7);
    }

    getch();

    return 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////                                      FUNCTIONS                                               /////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void fill_schedule_arrays(){
    for(int i = 0; i < total_process; i++){
        //FCFS
        strcpy(fcfs[i].process_name, normal_data[i].process_name);
        fcfs[i].arrival_time = normal_data[i].arrival_time;
        fcfs[i].burst_time = fcfs[i].remaining_time = normal_data[i].burst_time;

        //SJF
        strcpy(sjf[i].process_name, normal_data[i].process_name);
        sjf[i].arrival_time = normal_data[i].arrival_time;
        sjf[i].burst_time = sjf[i].remaining_time = normal_data[i].burst_time;

        //SRTF
        strcpy(srtf[i].process_name,normal_data[i].process_name);
        srtf[i].arrival_time = srtf[i].current_arrival_time = normal_data[i].arrival_time;
        srtf[i].burst_time = srtf[i].remaining_time = normal_data[i].burst_time;

        //NON-PREEMPTIVE PRIORITY
        strcpy(np_priority[i].process_name,normal_data[i].process_name);
        np_priority[i].arrival_time = normal_data[i].arrival_time;
        np_priority[i].burst_time = np_priority[i].remaining_time = normal_data[i].burst_time;
        np_priority[i].priority = normal_data[i].priority;

        //PREEMPTIVE PRIORITY
        strcpy(p_priority[i].process_name,normal_data[i].process_name);
        p_priority[i].arrival_time = p_priority[i].current_arrival_time = normal_data[i].arrival_time;
        p_priority[i].burst_time = p_priority[i].remaining_time = normal_data[i].burst_time;
        p_priority[i].priority = normal_data[i].priority;

        //ROUND-ROBIN
        strcpy(round_robin[i].process_name,normal_data[i].process_name);
        round_robin[i].arrival_time = round_robin[i].current_arrival_time = normal_data[i].arrival_time;
        round_robin[i].burst_time = round_robin[i].remaining_time = normal_data[i].burst_time;
    }
}

void reset_values(){
    printf("\n\n");
    y += 2;
    completed_process = 0;
    current_time = 0;
    minimum -1;
    prev_minimum = -1;
    average_rt = 0;
    average_tt = 0;
    average_wt = 0;
    cpu_utl    = 0;
}

void chart_label(int x_start, int x_end, int start_time, int end_time){
    gotoxy(x_start,y);
    printf("%d",start_time);
    gotoxy(x_end,y);
    printf("%d",end_time);
}

int get_minimum_time(float array[]){
    int min = -1;
    for(int i = 0; i < 6; i++){
        if(array[i] != -1.0){
            if(min == -1)
                min = i;
            else if(array[i] <  array[min])
                min = i;
        }
    }
    return min;
}

int get_maximum_time(float array[]){
    int max = -1;
    for(int i = 0; i < 6; i++){
        if(array[i] != -1.0){
            if(max == -1)
                max = i;
            else if(array[i] >  array[max])
                max = i;
        }
    }
    return max;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////                                 NON - PREEMPTIVE  FUNCTIONS                                  /////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void np_gantt_chart(int technique){
    y += 3;
    gotoxy(x,y);
    current_time = 0;
    for(int i = 0; i < 2; i++){
        for(int j = 0; j < total_process; j++){
            if(technique == 1)
                print_np_chart(fcfs[fcfs_rq[total_process - 1]].end_time, fcfs[fcfs_rq[j]].arrival_time, fcfs[fcfs_rq[j]].burst_time, fcfs_rq[j], 1);
            else if(technique == 2)
                print_np_chart(sjf[sjf_rq[total_process - 1]].end_time, sjf[sjf_rq[j]].arrival_time, sjf[sjf_rq[j]].burst_time, sjf_rq[j], 2);
            else if(technique ==3)
                print_np_chart(np_priority[np_priority_rq[total_process - 1]].end_time, np_priority[np_priority_rq[j]].arrival_time, np_priority[np_priority_rq[j]].burst_time, np_priority_rq[j], 3);
        }
        y++;
        x = 10;
        printf("\n          ");
        current_time = 0;
    }

    gotoxy(x,y);
    SetColor(7);
    printf("0");
    for(int i = 0; i < total_process; i++){
        if(technique == 1)
            chart_label(fcfs[fcfs_rq[i]].x_start, fcfs[fcfs_rq[i]].x_end, fcfs[fcfs_rq[i]].start_time, fcfs[fcfs_rq[i]].end_time);
        else if(technique == 2)
            chart_label(sjf[sjf_rq[i]].x_start, sjf[sjf_rq[i]].x_end, sjf[sjf_rq[i]].start_time, sjf[sjf_rq[i]].end_time);
        else if(technique == 3)
            chart_label(np_priority[np_priority_rq[i]].x_start, np_priority[np_priority_rq[i]].x_end, np_priority[np_priority_rq[i]].start_time, np_priority[np_priority_rq[i]].end_time);
    }
}

void print_np_chart(int overall_time, int arrival_time, int burst_time, int index, int technique){
    float burst_time_percentage, idle_time_percentage;
    int bt_percentage, it_percentage;

    burst_time_percentage = (float) burst_time;
    burst_time_percentage /= (float) overall_time;
    burst_time_percentage *= 100.0;
    //TRUNCATING burst_time_percentage DECIMAL VALUES BY TYPE CASTING TO INTEGER
    bt_percentage = (int) burst_time_percentage;

    if(current_time < arrival_time){

        idle_time_percentage = (float) arrival_time - current_time;
        idle_time_percentage /= (float) overall_time;
        idle_time_percentage *= 100.0;
        //TRUNCATING idle_time_percentage DECIMAL VALUES BY TYPE CASTING TO INTEGER
        it_percentage = (int) idle_time_percentage;

        SetColor(7);
        for(int i = 0; i < it_percentage; i++){
            printf("%c",176);
            x++;
        }
    }

    if(technique == 1)
        fcfs[index].x_start = x;
    else if(technique == 2)
        sjf[index].x_start = x;
    else if(technique == 3)
        np_priority[index].x_start = x;

    SetColor(index + 1);
    for(int i = 0; i < bt_percentage; i++){
        printf("%c",219);
        x++;
    }

    if(technique == 1){
        fcfs[index].x_end = x;
        current_time = fcfs[index].end_time;
    }
    else if(technique == 2){
        sjf[index].x_end = x;
        current_time = sjf[index].end_time;
    }
    else if(technique == 3){
        np_priority[index].x_end = x;
        current_time = np_priority[index].end_time;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////                                    PREEMPTIVE FUNCTIONS                                      /////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void preemptive_gantt_chart(){
    y += 3;
    gotoxy(x,y);
    p = gantt_chart_head;
    current_time = 0;
    for(int i = 0; i < 2; i++){
        while(p != NULL){
            print_preemptive_chart(previous_process -> end_time, p -> current_arrival_time, p -> remaining_time, p -> index + 1);
            p = p -> next;
        }
        y++;
        x = 10;
        printf("\n          ");
        p = gantt_chart_head;
        current_time = 0;
    }

    SetColor(7);
    gotoxy(x,y);
    printf("0");
    p = gantt_chart_head;
    while(p != NULL){
        chart_label(p -> x_start, p -> x_end, p -> start_time, p -> end_time);
        p = p -> next;
    }
}

void print_preemptive_chart(int overall_time, int current_arrival_time, int remaining_time, int color){
    float burst_time_percentage, idle_time_percentage;
    int bt_percentage, it_percentage;

    burst_time_percentage = (float) remaining_time;
    burst_time_percentage /= (float) overall_time;
    burst_time_percentage *= 100.0;
    //TRUNCATING burst_time_percentage DECIMAL VALUES BY TYPE CASTING TO INTEGER
    bt_percentage = (int) burst_time_percentage;

    if(current_time < current_arrival_time){

        idle_time_percentage = (float) current_arrival_time - current_time;
        idle_time_percentage /= (float) overall_time;
        idle_time_percentage *= 100.0;
        //TRUNCATING idle_time_percentage DECIMAL VALUES BY TYPE CASTING TO INTEGER
        it_percentage = (int) idle_time_percentage;

        SetColor(7);
        for(int i = 0; i < it_percentage; i++){
            printf("%c",176);
            x++;
        }
    }
    p -> x_start = x;
    SetColor(color);
    for(int i = 0; i < bt_percentage; i++){
        printf("%c",219);
        x++;
    }
    p -> x_end = x;

    current_time = p -> end_time;
}

void free_preemptive_nodes(){
    CHART *zombie;
    previous_process = NULL;
    p = gantt_chart_head;
    gantt_chart_head = NULL;
    while(p != NULL){
        zombie = p;
        p = p -> next;
        free(zombie);
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////                                    FCFS SECTION                                              /////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void fcfs_fill(){
    while(completed_process < total_process){
        for(int i = 0; i < total_process; i++){
            if(minimum == -1){
                if(fcfs[i].remaining_time != 0)
                    minimum = i;
            }
            else if(minimum != -1 && fcfs[i].remaining_time != 0){
                if(fcfs[i].arrival_time < fcfs[minimum].arrival_time)
                    minimum = i;
            }
        }

        if(current_time < fcfs[minimum].arrival_time)
            fcfs[minimum].start_time = fcfs[minimum].arrival_time;
        else if(current_time >= fcfs[minimum].arrival_time)
            fcfs[minimum].start_time = current_time;

        current_time = fcfs[minimum].start_time + fcfs[minimum].burst_time;
        fcfs[minimum].end_time = current_time;
        fcfs_rq[completed_process] = minimum;
        fcfs[minimum].remaining_time = 0;
        minimum = -1;
        completed_process++;
    }
    for(int i = 0; i < total_process; i++){
        fcfs[i].response_time    = fcfs[i].start_time - fcfs[i].arrival_time;
        fcfs[i].turn_around_time = fcfs[i].end_time - fcfs[i].arrival_time;
        fcfs[i].wait_time        = fcfs[i].turn_around_time - fcfs[i].burst_time;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////                                      SJF SECTION                                             /////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void sjf_fill(){
    while(completed_process < total_process){
        for(int i = 0; i < total_process; i++){
            if(minimum == -1){
                if(sjf[i].remaining_time != 0)
                    minimum = i;
            }
            else if(minimum != -1 && sjf[i].remaining_time != 0){
                if(current_time >= sjf[i].arrival_time){
                    if(sjf[minimum].arrival_time > current_time)
                        minimum = i;
                    else if(sjf[minimum].arrival_time <= current_time){
                        if(sjf[i].burst_time < sjf[minimum].burst_time)
                            minimum = i;
                        else if(sjf[i].burst_time == sjf[minimum].burst_time){
                            if(sjf[i].arrival_time < sjf[minimum].arrival_time)
                                minimum = i;
                        }
                    }
                }
                else if(sjf[minimum].arrival_time > current_time){
                    if(sjf[minimum].arrival_time == sjf[i].arrival_time){
                        if(sjf[i].burst_time < sjf[minimum].burst_time)
                            minimum = i;
                    }
                    else if(sjf[i].arrival_time < sjf[minimum].arrival_time)
                        minimum = i;
                }
            }
        }

        if(current_time < sjf[minimum].arrival_time)
            sjf[minimum].start_time = sjf[minimum].arrival_time;
        else if(current_time >= sjf[minimum].arrival_time)
            sjf[minimum].start_time = current_time;

        current_time = sjf[minimum].start_time + sjf[minimum].burst_time;
        sjf[minimum].end_time = current_time;
        sjf_rq[completed_process] = minimum;
        sjf[minimum].remaining_time = 0;
        minimum = -1;
        completed_process++;
    }

    for(int i = 0; i < total_process; i++){
        sjf[i].response_time    = sjf[i].start_time - sjf[i].arrival_time;
        sjf[i].turn_around_time = sjf[i].end_time - sjf[i].arrival_time;
        sjf[i].wait_time        = sjf[i].turn_around_time - sjf[i].burst_time;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////                               NON - PREEMPTIVE PRIORITY                                      /////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void np_priority_fill(){
    while(completed_process < total_process){
        for(int i = 0; i < total_process; i++){
            if(minimum == -1){
                if(np_priority[i].remaining_time != 0)
                    minimum = i;
            }
            else if(minimum != -1 && np_priority[i].remaining_time != 0){
                if(current_time >= np_priority[i].arrival_time){
                    if(np_priority[minimum].arrival_time > current_time)
                        minimum = i;
                    else if(np_priority[minimum].arrival_time <= current_time){
                        if(np_priority[i].priority < np_priority[minimum].priority)
                            minimum = i;
                        else if(np_priority[i].priority == np_priority[minimum].priority){
                            if(np_priority[i].arrival_time < np_priority[minimum].arrival_time)
                                minimum = i;
                        }
                    }
                }
                else if(np_priority[minimum].arrival_time > current_time){
                    if(np_priority[minimum].arrival_time == np_priority[i].arrival_time){
                        if(np_priority[i].priority < np_priority[minimum].priority)
                            minimum = i;
                    }
                    else if(np_priority[i].arrival_time < np_priority[minimum].arrival_time)
                        minimum = i;
                }
            }
        }

        if(current_time < np_priority[minimum].arrival_time)
            np_priority[minimum].start_time = np_priority[minimum].arrival_time;
        else if(current_time >= np_priority[minimum].arrival_time)
            np_priority[minimum].start_time = current_time;

        current_time = np_priority[minimum].start_time + np_priority[minimum].burst_time;
        np_priority[minimum].end_time = current_time;
        np_priority_rq[completed_process] = minimum;
        np_priority[minimum].remaining_time = 0;
        minimum = -1;
        completed_process++;
    }
    for(int i = 0; i < total_process; i++){
        np_priority[i].response_time    = np_priority[i].start_time - np_priority[i].arrival_time;
        np_priority[i].turn_around_time = np_priority[i].end_time - np_priority[i].arrival_time;
        np_priority[i].wait_time        = np_priority[i].turn_around_time - np_priority[i].burst_time;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////                                    SRTF SECTION                                              /////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void srtf_fill(){
    CHART *preempting_process;
    while(completed_process < total_process){
        for(int i = 0; i < total_process; i++){
            if(minimum == -1){
                if(srtf[i].remaining_time != 0)
                    minimum = i;
            }
            else if(minimum != -1 && srtf[i].remaining_time != 0){
                if(srtf[i].current_arrival_time <= current_time){
                    if(srtf[minimum].current_arrival_time > current_time)
                        minimum = i;
                    else if(srtf[minimum].current_arrival_time <= current_time){
                        if(srtf[i].remaining_time < srtf[minimum].remaining_time)
                            minimum = i;
                        else if(srtf[i].remaining_time == srtf[minimum].remaining_time){
                            if(srtf[i].current_arrival_time < srtf[minimum].current_arrival_time)
                                minimum = i;
                        }
                    }
                }
                else if(srtf[minimum].current_arrival_time > current_time){
                    if(srtf[minimum].current_arrival_time == srtf[i].current_arrival_time){
                        if(srtf[i].remaining_time < srtf[minimum].remaining_time)
                            minimum = i;
                    }
                    else if(srtf[i].current_arrival_time < srtf[minimum].current_arrival_time)
                        minimum = i;
                }
            }
        }

        if(srtf[minimum].remaining_time == srtf[minimum].burst_time){
            if(srtf[minimum].current_arrival_time <= current_time)
                srtf[minimum].start_time = current_time;
            else if(srtf[minimum].current_arrival_time > current_time){
                srtf[minimum].start_time = srtf[minimum].current_arrival_time;
                current_time = srtf[minimum].start_time;
            }
        }

        if(gantt_chart_head == NULL){
            preempting_process = (CHART*)malloc(sizeof(CHART));
            preempting_process -> current_arrival_time = srtf[minimum].current_arrival_time;
            preempting_process -> remaining_time       = 0;
            preempting_process -> start_time           = current_time;
            preempting_process -> index                = minimum;
            preempting_process -> next                 = gantt_chart_head;
            gantt_chart_head = preempting_process;
            previous_process = preempting_process;
        }
        else if(gantt_chart_head != NULL){
            if(prev_minimum != minimum){
                if(srtf[prev_minimum].remaining_time != 0)
                    preempting_process -> end_time = current_time;
                preempting_process = NULL;
                preempting_process = (CHART*)malloc(sizeof(CHART));
                preempting_process -> current_arrival_time = srtf[minimum].current_arrival_time;
                preempting_process -> remaining_time       = 0;
                preempting_process -> start_time           = current_time;
                preempting_process -> index                = minimum;
                preempting_process -> next = previous_process -> next;
                previous_process -> next = preempting_process;
                previous_process = preempting_process;
            }
        }

        current_time++;
        preempting_process -> remaining_time += 1;
        srtf[minimum].remaining_time--;
        if(prev_minimum != minimum && prev_minimum != -1)
            srtf[prev_minimum].current_arrival_time = current_time;
        if(srtf[minimum].remaining_time == 0){
            srtf[minimum].end_time = current_time;
            preempting_process -> end_time = current_time;
            completed_process++;
        }
        prev_minimum = minimum;
        minimum = -1;
    }

    for(int i = 0; i < total_process; i++){
        srtf[i].response_time    = srtf[i].start_time - srtf[i].arrival_time;
        srtf[i].turn_around_time = srtf[i].end_time - srtf[i].arrival_time;
        srtf[i].wait_time        = srtf[i].turn_around_time - srtf[i].burst_time;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////                                    PREEMPTIVE PRIORITY                                       /////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void p_priority_fill(){
    CHART *preempting_process;
    while(completed_process < total_process){
        for(int i = 0; i < total_process; i++){
            if(minimum == -1){
                if(p_priority[i].remaining_time != 0)
                    minimum = i;
            }
            else if(minimum != -1 && p_priority[i].remaining_time != 0){
                if(p_priority[i].current_arrival_time <= current_time){
                    if(p_priority[minimum].current_arrival_time > current_time)
                        minimum = i;
                    else if(p_priority[minimum].current_arrival_time <= current_time){
                        if(p_priority[i].priority < p_priority[minimum].priority)
                            minimum = i;
                        else if(p_priority[i].priority == p_priority[minimum].priority){
                            if(p_priority[i].current_arrival_time < p_priority[minimum].current_arrival_time)
                                minimum = i;
                        }
                    }
                }
                else if(p_priority[minimum].current_arrival_time > current_time){
                    if(p_priority[minimum].current_arrival_time == p_priority[i].current_arrival_time){
                        if(p_priority[i].priority < p_priority[minimum].priority)
                            minimum = i;
                    }
                    else if(p_priority[i].current_arrival_time < p_priority[minimum].current_arrival_time)
                        minimum = i;
                }
            }
        }

        if(p_priority[minimum].remaining_time == p_priority[minimum].burst_time){
            if(p_priority[minimum].current_arrival_time <= current_time)
                p_priority[minimum].start_time = current_time;
            else if(p_priority[minimum].current_arrival_time > current_time){
                p_priority[minimum].start_time = p_priority[minimum].current_arrival_time;
                current_time = p_priority[minimum].start_time;
            }
        }

        if(gantt_chart_head == NULL){
            preempting_process = (CHART*)malloc(sizeof(CHART));
            preempting_process -> current_arrival_time = p_priority[minimum].current_arrival_time;
            preempting_process -> remaining_time       = 0;
            preempting_process -> start_time           = current_time;
            preempting_process -> index                = minimum;
            preempting_process -> next                 = gantt_chart_head;
            gantt_chart_head = preempting_process;
            previous_process = preempting_process;
        }
        else if(gantt_chart_head != NULL){
            if(prev_minimum != minimum){
                if(p_priority[prev_minimum].remaining_time != 0)
                    preempting_process -> end_time = current_time;
                preempting_process = NULL;
                preempting_process = (CHART*)malloc(sizeof(CHART));
                preempting_process -> current_arrival_time = p_priority[minimum].current_arrival_time;
                preempting_process -> remaining_time       = 0;
                preempting_process -> start_time           = current_time;
                preempting_process -> index                = minimum;
                preempting_process -> next = previous_process -> next;
                previous_process -> next = preempting_process;
                previous_process = preempting_process;
            }
        }

        current_time++;
        preempting_process -> remaining_time += 1;
        p_priority[minimum].remaining_time--;
        if(prev_minimum != minimum && prev_minimum != -1)
            p_priority[prev_minimum].current_arrival_time = current_time;
        if(p_priority[minimum].remaining_time == 0){
            p_priority[minimum].end_time = current_time;
            preempting_process -> end_time = current_time;
            completed_process++;
        }
        prev_minimum = minimum;
        minimum = -1;
    }

    for(int i = 0; i < total_process; i++){
        p_priority[i].response_time    = p_priority[i].start_time - p_priority[i].arrival_time;
        p_priority[i].turn_around_time = p_priority[i].end_time - p_priority[i].arrival_time;
        p_priority[i].wait_time        = p_priority[i].turn_around_time - p_priority[i].burst_time;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////                                  ROUND - ROBIN SECTION                                       /////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void round_robin_fill(){
    CHART *preempting_process;
    while(completed_process < total_process){
        for(int i = 0; i < total_process; i++){
            if(minimum == -1){
                if(round_robin[i].remaining_time != 0)
                    minimum = i;
            }
            else if(minimum != -1 && round_robin[i].remaining_time != 0){
                if(round_robin[i].current_arrival_time < round_robin[minimum].current_arrival_time)
                    minimum = i;
            }
        }

        if(round_robin[minimum].remaining_time == round_robin[minimum].burst_time){
            if(round_robin[minimum].current_arrival_time <= current_time)
                round_robin[minimum].start_time = current_time;
            else if(round_robin[minimum].current_arrival_time > current_time){
                round_robin[minimum].start_time = round_robin[minimum].current_arrival_time;
                current_time = round_robin[minimum].start_time;
            }
        }

        if(gantt_chart_head == NULL){
            preempting_process = (CHART*)malloc(sizeof(CHART));
            preempting_process -> current_arrival_time = round_robin[minimum].current_arrival_time;
            preempting_process -> remaining_time       = 0;
            preempting_process -> start_time           = current_time;
            preempting_process -> index                = minimum;
            preempting_process -> next                 = gantt_chart_head;
            gantt_chart_head = preempting_process;
            previous_process = preempting_process;
        }
        else if(gantt_chart_head != NULL){
            if(prev_minimum != minimum){
                if(round_robin[prev_minimum].remaining_time != 0)
                    preempting_process -> end_time = current_time;
                preempting_process = NULL;
                preempting_process = (CHART*)malloc(sizeof(CHART));
                preempting_process -> current_arrival_time = round_robin[minimum].current_arrival_time;
                preempting_process -> remaining_time       = 0;
                preempting_process -> start_time           = current_time;
                preempting_process -> index                = minimum;
                preempting_process -> next = previous_process -> next;
                previous_process -> next = preempting_process;
                previous_process = preempting_process;
            }
        }

        for(int i = 0; i < time_quantum; i++){
            current_time++;
            preempting_process -> remaining_time += 1;
            round_robin[minimum].remaining_time--;
            if(round_robin[minimum].remaining_time == 0){
                round_robin[minimum].end_time = current_time;
                preempting_process -> end_time = current_time;
                completed_process++;
                break;
            }
        }
        round_robin[minimum].current_arrival_time = current_time;

        prev_minimum = minimum;
        minimum = -1;
    }

    for(int i = 0; i < total_process; i++){
        round_robin[i].response_time    = round_robin[i].start_time - round_robin[i].arrival_time;
        round_robin[i].turn_around_time = round_robin[i].end_time - round_robin[i].arrival_time;
        round_robin[i].wait_time        = round_robin[i].turn_around_time - round_robin[i].burst_time;
    }
}


