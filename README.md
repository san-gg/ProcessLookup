## ProcessLookup
ProcessLookup lists CPU utilization (in percentage) for all the processes.
Following is the output format:-

> CPU : xx%
> \---------
> Pid    : <pid>
> Path  : <Exe path>
> Cmd  : <Command line of Exe>

...
```
ProcessLookup.exe [/t <time>] [/tg <time_granularity>] [/noe <entries>]
```

| Command/Arguments | Function |
| ----------------- | -------- |
| /t | Total time(in minutes) to monitor the processes |
| /tg   | Selected processes accounted for CPU usage calculation after Time granularity(in minutes) |
| /noe | Specifies number of CPU usage to display |

eg:-
```bash
ProcessLookup.exe /t 10 /tg 2 /noe 10
/t 10  -  Program will run for 10 mins.
/tg 2  -  After 2 mins CPU usage % will be calculated for all the processes.
/noe   -  Display 10 entries of CPU usage % .
```
