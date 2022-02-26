## ProcessLookup
ProcessLookup lists CPU utilization (percentage) of all the processes.<br>
It is a console program.<br>
Following is the output format:-

> CPU : xx%<br>
> \---------<br>
> Pid  : \<pid\><br>
> Path : \<Exe path\><br>
> Cmd  : \<Command line of Exe\><br>
>
> Pid  : \<pid\><br>
> Path : \<Exe path\><br>
> Cmd  : \<Command line of Exe\><br>

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
/noe   -  Displays top 10 CPU usage %.
```
## Installation
If you want to run exe from anywhere in the command prompt, just `Double-Click Installation.bat` - This will add extracted directory path in users environment variable. Otherwise you can simply run in command prompt.<br>
Currently the exe supports Windows 10, Windows server 2019 and Windows server 2022.
