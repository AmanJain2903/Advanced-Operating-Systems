#include<stdio.h>
#include<system.h>
#include<common_files_header.h>
void srtf(Process processes[], int count) {
	int currentTime = 0, completed = 0, minRemaining = INT_MAX;
	int shortest = -1;
	int isRunning = 0;
	int wait_time = 0, completion_time = 0, turnaround_time = 0;
	while(completed!=count) {
		for(int i=0;i<n;i++) {
			if(processes[i].arrivalTime <= currentTime && processes[i].remainingTime > 0 && processes[i].remainingTime < minRemaining) {
				shortest = i;
				minRemaining = process[i].remainingTime;
				isRunning = 1;	
			}
		}
		if(isRunning==0) {
			curentTime++;
			continue;
		}
		processes[shortest].remainingTime--;
		minRemaining = processes[shortest].remainingTime;
		if(processes[shortest].remaining==0) {
			completed++;
			isRunning=0;
			completion_time = currentTime+1;
			turnaround_time = completion_time - processes[shortest].arrivalTime;
			wait_time = turnaround_time - processes[shortest].runTime;
			minRemaining = INT_MAX;
			printf("Process_Shortest: %d, CompletionTime: %d, TurnAroundTime: %d, WaitTime: %d",shortest,completion_time,turnaround_time,wait_time);
			//calculateMetrics(completion_time,turnaround_time,wait_time);
		}
		currentTime++;
	}
}
