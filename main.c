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

typedef struct {
    int algorithm;      // 1=FCFS, 2=SJF, 3=RR, 4=Priority
    int quantum;        // For RR
    int process_count;  // Number of processes in this queue
    int process_indices[MAX_PROCESSES]; // Indices of processes in this queue
} Queue;



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

    print_averages(processes, n);
}

void srt(Process processes[], int n) {
    int completed = 0, current_time = 0, shortest = -1, min_remaining;

    for (int i = 0; i < n; i++) {
        processes[i].remaining_time = processes[i].burst_time;
    }

    printf("\nSRT Results:\n");
    printf("PID\tArrival\tBurst\tWaiting\tTurnaround\n");

   
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

void multilevel_queue(Process processes[], int n, Queue queues[], int num_queues) {
    int current_time = 0;
    
    printf("\n=== Multilevel Queue Scheduling ===\n");
    
    // Process each queue in priority order (each queue completes before next one starts)
    for (int q = 0; q < num_queues; q++) {
        if (queues[q].process_count == 0) {
            printf("\n--- Queue %d: Empty ---\n", q);
            continue;
        }
        
        // Create a temporary array with only processes from this queue
        Process queue_processes[MAX_PROCESSES];
        int queue_size = queues[q].process_count;
        
        // Copy processes for this queue
        for (int i = 0; i < queue_size; i++) {
            int idx = queues[q].process_indices[i];
            queue_processes[i] = processes[idx];
            // Processes in this queue can't start before the queue becomes active
            if (queue_processes[i].arrival_time < current_time) {
                queue_processes[i].arrival_time = current_time;
            }
        }
        
        // Display queue header
        printf("\n--- Queue %d ", q);
        switch (queues[q].algorithm) {
            case 1: printf("(FCFS)"); break;
            case 2: printf("(SJF)"); break;
            case 3: printf("(RR, q=%d)", queues[q].quantum); break;
            case 4: printf("(Priority)"); break;
        }
        printf(" ---\n");
        
        int start_time = current_time;
        
        // Execute the queue's algorithm
        switch (queues[q].algorithm) {
            case 1: // FCFS
                {
                   
                    for (int i = 0; i < queue_size; i++) {
                        if (current_time < queue_processes[i].arrival_time)
                            current_time = queue_processes[i].arrival_time;
                        
                        printf("| P%d ", queue_processes[i].pid);
                        queue_processes[i].waiting_time = current_time - queue_processes[i].arrival_time;
                        current_time += queue_processes[i].burst_time;
                        queue_processes[i].turnaround_time = queue_processes[i].waiting_time + queue_processes[i].burst_time;
                    }
                    printf("|\n");
                }
                break;
                
            case 2: // SJF (Non-preemptive)
                {
                    int completed = 0;
                    int is_completed[MAX_PROCESSES] = {0};
                    
                    
                    while (completed < queue_size) {
                        int min_index = -1;
                        int min_burst = INT_MAX;
                        
                        // Find process with shortest burst time that has arrived
                        for (int i = 0; i < queue_size; i++) {
                            if (!is_completed[i] && 
                                queue_processes[i].arrival_time <= current_time &&
                                queue_processes[i].burst_time < min_burst) {
                                min_burst = queue_processes[i].burst_time;
                                min_index = i;
                            }
                        }
                        
                        if (min_index == -1) {
                            current_time++;
                            continue;
                        }
                        
                        printf("| P%d ", queue_processes[min_index].pid);
                        queue_processes[min_index].waiting_time = current_time - queue_processes[min_index].arrival_time;
                        current_time += queue_processes[min_index].burst_time;
                        queue_processes[min_index].turnaround_time = current_time - queue_processes[min_index].arrival_time;
                        is_completed[min_index] = 1;
                        completed++;
                    }
                    printf("|\n");
                }
                break;
                
            case 3: // Round Robin
                {
                    int completed = 0;
                    int remaining_bt[MAX_PROCESSES];
                    
                    for (int i = 0; i < queue_size; i++) {
                        remaining_bt[i] = queue_processes[i].burst_time;
                    }
                    
                  
                    while (completed < queue_size) {
                        int executed = 0;
                        
                        for (int i = 0; i < queue_size; i++) {
                            if (queue_processes[i].arrival_time <= current_time && remaining_bt[i] > 0) {
                                executed = 1;
                                printf("| P%d ", queue_processes[i].pid);
                                
                                if (remaining_bt[i] > queues[q].quantum) {
                                    current_time += queues[q].quantum;
                                    remaining_bt[i] -= queues[q].quantum;
                                } else {
                                    current_time += remaining_bt[i];
                                    queue_processes[i].turnaround_time = current_time - queue_processes[i].arrival_time;
                                    queue_processes[i].waiting_time = queue_processes[i].turnaround_time - queue_processes[i].burst_time;
                                    remaining_bt[i] = 0;
                                    completed++;
                                }
                            }
                        }
                        
                        if (!executed)
                            current_time++;
                    }
                    printf("|\n");
                }
                break;
                
            case 4: // Priority (Non-preemptive)
                {
                    int completed = 0;
                    int is_completed[MAX_PROCESSES] = {0};
                    
                    
                    while (completed < queue_size) {
                        int idx = -1;
                        int min_priority = INT_MAX;
                        
                        // Find highest priority process that has arrived
                        for (int i = 0; i < queue_size; i++) {
                            if (!is_completed[i] && 
                                queue_processes[i].arrival_time <= current_time &&
                                queue_processes[i].priority < min_priority) {
                                min_priority = queue_processes[i].priority;
                                idx = i;
                            }
                        }
                        
                        if (idx == -1) {
                            current_time++;
                            continue;
                        }
                        
                        printf("| P%d ", queue_processes[idx].pid);
                        queue_processes[idx].waiting_time = current_time - queue_processes[idx].arrival_time;
                        current_time += queue_processes[idx].burst_time;
                        queue_processes[idx].turnaround_time = current_time - queue_processes[idx].arrival_time;
                        is_completed[idx] = 1;
                        completed++;
                    }
                    printf("|\n");
                }
                break;
        }
        
        // Display process details for this queue
        printf("\nPID\tArrival\tBurst\tWaiting\tTurnaround\n");
        for (int i = 0; i < queue_size; i++) {
            printf("%d\t%d\t%d\t%d\t%d\n", 
                   queue_processes[i].pid, 
                   queue_processes[i].arrival_time,
                   queue_processes[i].burst_time, 
                   queue_processes[i].waiting_time, 
                   queue_processes[i].turnaround_time);
        }
        
        // Calculate average for this queue
        double total_waiting = 0, total_turnaround = 0;
        for (int i = 0; i < queue_size; i++) {
            total_waiting += queue_processes[i].waiting_time;
            total_turnaround += queue_processes[i].turnaround_time;
        }
        printf("Average Waiting Time: %.2f\n", total_waiting / queue_size);
        printf("Average Turnaround Time: %.2f\n", total_turnaround / queue_size);
        
        // Update original processes array with calculated times
        for (int i = 0; i < queue_size; i++) {
            int idx = queues[q].process_indices[i];
            processes[idx].waiting_time = queue_processes[i].waiting_time;
            processes[idx].turnaround_time = queue_processes[i].turnaround_time;
        }
    }
    
    // Display overall statistics
    printf("\n=== Overall Statistics ===\n");
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
    printf("7. Multilevel Queue Scheduling\n");
    printf("\nEnter your choice (1-7): ");
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

    // Multilevel Queue Configuration
    Queue queues[MAX_PROCESSES];
    int num_queues = 0;
    
    if (choice == 7) {
        printf("\n=== Multilevel Queue Configuration ===\n");
        printf("Enter number of queues: ");
        scanf("%d", &num_queues);
        
        // Configure each queue
        for (int q = 0; q < num_queues; q++) {
            printf("\n--- Queue %d Configuration (Priority: %d = Highest) ---\n", q, q);
            printf("Select algorithm for Queue %d:\n", q);
            printf("  1. FCFS\n");
            printf("  2. SJF\n");
            printf("  3. Round Robin\n");
            printf("  4. Priority Scheduling\n");
            printf("Choice: ");
            scanf("%d", &queues[q].algorithm);
            
            // Ask for quantum if RR is selected for this queue
            if (queues[q].algorithm == 3) {
                printf("Enter time quantum for this queue: ");
                scanf("%d", &queues[q].quantum);
            } else {
                queues[q].quantum = 0;
            }
            
            queues[q].process_count = 0;
        }
        
        // Ask for priority values if any queue uses Priority scheduling
        int need_priority = 0;
        for (int q = 0; q < num_queues; q++) {
            if (queues[q].algorithm == 4) {
                need_priority = 1;
                break;
            }
        }
        
        if (need_priority) {
            printf("\n--- Priority Information ---\n");
            for (int i = 0; i < n; i++) {
                printf("Enter priority for Process %d (lower number = higher priority): ", i + 1);
                scanf("%d", &processes[i].priority);
            }
        }
        
        // Assign processes to queues
        printf("\n--- Assign Processes to Queues ---\n");
        for (int i = 0; i < n; i++) {
            int queue_num;
            printf("Assign Process %d to which queue (0-%d)? ", i + 1, num_queues - 1);
            scanf("%d", &queue_num);
            
            if (queue_num >= 0 && queue_num < num_queues) {
                queues[queue_num].process_indices[queues[queue_num].process_count] = i;
                queues[queue_num].process_count++;
            } else {
                printf("Invalid queue number! Assigning to Queue 0.\n");
                queues[0].process_indices[queues[0].process_count] = i;
                queues[0].process_count++;
            }
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
        case 7:
            multilevel_queue(processes, n, queues, num_queues);
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
