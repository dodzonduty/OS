#include <stdio.h>
#include <limits.h>
#define MAX_PROCESSES 100

typedef struct {
    int pid;
    int arrival_time;
    int burst_time;
    int priority;
    int waiting_time;
    int turnaround_time;
    int remaining_time;
    int completion_time;
} Process;

void print_gantt_chart(Process processes[], int n) {
    printf("\nGantt Chart:\n| ");
    for (int i = 0; i < n; i++) {
        printf("P%d ", processes[i].pid);
        printf("| ");
    }
    printf("\n");
}

void print_averages(Process processes[], int n) {
    double total_waiting_time = 0, total_turnaround_time = 0;

    for (int i = 0; i < n; i++) {
        total_waiting_time += processes[i].waiting_time;
        total_turnaround_time += processes[i].turnaround_time;
    }

    printf("\nAverage Waiting Time: %.2f\n", total_waiting_time / n);
    printf("Average Turnaround Time: %.2f\n", total_turnaround_time / n);
}

void fcfs(Process processes[], int n) {
    int current_time = 0;

    for (int i = 0; i < n; i++) {
        if (current_time < processes[i].arrival_time)
            current_time = processes[i].arrival_time;
        processes[i].waiting_time = current_time - processes[i].arrival_time;
        processes[i].turnaround_time = processes[i].waiting_time + processes[i].burst_time;
        current_time += processes[i].burst_time;
    }

    printf("\nFCFS Results:\n");
    printf("PID\tArrival\tBurst\tWaiting\tTurnaround\n");
    for (int i = 0; i < n; i++) {
        printf("%d\t%d\t%d\t%d\t%d\n", processes[i].pid, processes[i].arrival_time,
               processes[i].burst_time, processes[i].waiting_time, processes[i].turnaround_time);
    }
    print_gantt_chart(processes, n);
    print_averages(processes, n);
}

void sjf(Process processes[], int n) {
    int completed = 0, current_time = 0;
    int min_index;

    while (completed < n) {
        min_index = -1;

        for (int i = 0; i < n; i++) {
            if (processes[i].remaining_time > 0 && processes[i].arrival_time <= current_time) {
                if (min_index == -1 || processes[i].remaining_time < processes[min_index].remaining_time) {
                    min_index = i;
                }
            }
        }

        if (min_index == -1) {
            current_time++;
            continue;
        }

        current_time += processes[min_index].burst_time;
        processes[min_index].completion_time = current_time;
        processes[min_index].turnaround_time = processes[min_index].completion_time - processes[min_index].arrival_time;
        processes[min_index].waiting_time = processes[min_index].turnaround_time - processes[min_index].burst_time;
        processes[min_index].remaining_time = 0;
        completed++;
    }

    printf("\nSJF Results:\n");
    printf("PID\tArrival\tBurst\tWaiting\tTurnaround\n");
    for (int i = 0; i < n; i++) {
        printf("%d\t%d\t%d\t%d\t%d\n", processes[i].pid, processes[i].arrival_time,
               processes[i].burst_time, processes[i].waiting_time, processes[i].turnaround_time);
    }
    print_gantt_chart(processes, n);
    print_averages(processes, n);
}

void rr(Process processes[], int n, int quantum) {
    int current_time = 0, completed = 0;
    int remaining_bt[n];
    for (int i = 0; i < n; i++) {
        remaining_bt[i] = processes[i].burst_time;
    }

    printf("\nRR Results:\n");
    printf("PID\tArrival\tBurst\tWaiting\tTurnaround\n");

    printf("\nGantt Chart:\n| ");
    while (completed < n) {
        int executed = 0;

        for (int i = 0; i < n; i++) {
            if (processes[i].arrival_time <= current_time && remaining_bt[i] > 0) {
                executed = 1;
                if (remaining_bt[i] > quantum) {
                    for (int j = 0; j < quantum; j++) printf("P%d ", processes[i].pid);
                    current_time += quantum;
                    remaining_bt[i] -= quantum;
                    printf("|");
                } else {
                    for (int j = 0; j < remaining_bt[i]; j++) printf("P%d ", processes[i].pid);
                    current_time += remaining_bt[i];
                    processes[i].waiting_time = current_time - processes[i].arrival_time - processes[i].burst_time;
                    processes[i].turnaround_time = current_time - processes[i].arrival_time;
                    remaining_bt[i] = 0;
                    completed++;
                    printf("|");
                }
            }
        }

        if (!executed)
            current_time++;
    }
    printf("\n");
    print_averages(processes, n);
}

void priority(Process processes[], int n) {
    int completed = 0, current_time = 0;
    int visited[n];
    for (int i = 0; i < n; i++) {
        visited[i] = 0;
    }

    while (completed < n) {
        int idx = -1, min_priority = INT_MAX;

        for (int i = 0; i < n; i++) {
            if (!visited[i] && processes[i].arrival_time <= current_time) {
                if (processes[i].priority < min_priority ||
                    (processes[i].priority == min_priority && processes[i].arrival_time < processes[idx].arrival_time)) {
                    min_priority = processes[i].priority;
                    idx = i;
                }
            }
        }

        if (idx != -1) {
            visited[idx] = 1;
            current_time += processes[idx].burst_time;
            processes[idx].completion_time = current_time;
            processes[idx].turnaround_time = processes[idx].completion_time - processes[idx].arrival_time;
            processes[idx].waiting_time = processes[idx].turnaround_time - processes[idx].burst_time;
            completed++;
        } else {
            current_time++;
        }
    }

    printf("\nPriority Results:\n");
    printf("PID\tArrival\tBurst\tPriority\tWaiting\tTurnaround\n");
    for (int i = 0; i < n; i++) {
        printf("%d\t%d\t%d\t%d\t%d\t%d\n", processes[i].pid, processes[i].arrival_time,
               processes[i].burst_time, processes[i].priority, processes[i].waiting_time, processes[i].turnaround_time);
    }
    print_gantt_chart(processes, n);
    print_averages(processes, n);
}

void srt(Process processes[], int n) {
    int completed = 0, current_time = 0, shortest = -1, min_remaining;

    for (int i = 0; i < n; i++) {
        processes[i].remaining_time = processes[i].burst_time;
    }

    printf("\nSRT Results:\n");
    printf("PID\tArrival\tBurst\tWaiting\tTurnaround\n");

    printf("\nGantt Chart:\n| ");
    while (completed < n) {
        shortest = -1;
        min_remaining = INT_MAX;

        for (int i = 0; i < n; i++) {
            if (processes[i].remaining_time > 0 && processes[i].arrival_time <= current_time &&
                processes[i].remaining_time < min_remaining) {
                min_remaining = processes[i].remaining_time;
                shortest = i;
            }
        }

        if (shortest == -1) {
            current_time++;
            continue;
        }

        printf("P%d ", processes[shortest].pid);
        current_time++;
        processes[shortest].remaining_time--;

        if (processes[shortest].remaining_time == 0) {
            completed++;
            processes[shortest].completion_time = current_time;
            processes[shortest].turnaround_time = current_time - processes[shortest].arrival_time;
            processes[shortest].waiting_time = processes[shortest].turnaround_time - processes[shortest].burst_time;
            printf("|");
        }
    }
    printf("\n");
    print_averages(processes, n);
}

int main() {
    int n, quantum = 0;
    int choice;
    Process processes[MAX_PROCESSES];

    printf("=== CPU Scheduling Simulator ===\n\n");
    printf("Enter the number of processes: ");
    scanf("%d", &n);

    printf("\n--- Process Information ---\n");
    for (int i = 0; i < n; i++) {
        processes[i].pid = i + 1;
        printf("Process %d:\n", i + 1);
        printf("  Arrival time: ");
        scanf("%d", &processes[i].arrival_time);
        printf("  Burst time: ");
        scanf("%d", &processes[i].burst_time);
        processes[i].remaining_time = processes[i].burst_time;
        processes[i].priority = 0; // Default priority
    }

    printf("\n=== Select Scheduling Algorithms ===\n");
    printf("1. First Come First Serve (FCFS)\n");
    printf("2. Shortest Job First (SJF)\n");
    printf("3. Round Robin (RR)\n");
    printf("4. Priority Scheduling\n");
    printf("5. Shortest Remaining Time (SRT)\n");
    printf("6. Run All Algorithms\n");
    printf("\nEnter your choice (1-6): ");
    scanf("%d", &choice);

    // Ask for quantum only if RR is selected
    if (choice == 3 || choice == 6) {
        printf("\nEnter time quantum for Round Robin: ");
        scanf("%d", &quantum);
    }

    // Ask for priority only if Priority scheduling is selected
    if (choice == 4 || choice == 6) {
        printf("\n--- Priority Information ---\n");
        for (int i = 0; i < n; i++) {
            printf("Enter priority for Process %d (lower number = higher priority): ", i + 1);
            scanf("%d", &processes[i].priority);
        }
    }

    printf("\n========================================\n");

    // Run selected algorithm(s)
    switch (choice) {
        case 1:
            fcfs(processes, n);
            break;
        case 2:
            sjf(processes, n);
            break;
        case 3:
            rr(processes, n, quantum);
            break;
        case 4:
            priority(processes, n);
            break;
        case 5:
            srt(processes, n);
            break;
        case 6:
            fcfs(processes, n);
            printf("\n========================================\n");
            sjf(processes, n);
            printf("\n========================================\n");
            rr(processes, n, quantum);
            printf("\n========================================\n");
            priority(processes, n);
            printf("\n========================================\n");
            srt(processes, n);
            break;
        default:
            printf("\nInvalid choice! Please run the program again.\n");
            return 1;
    }

    printf("\n========================================\n");
    printf("\nPress Enter to exit...");
    getchar(); 
    getchar(); 

    return 0;
}
