# High Frequency CPU Monitor
Tool meant for aiding in monitoring singular process CPU loads. Polls /proc/ every n ms.

## Usage
`gcc monitorProc.c -o cpuMon`

`./cpuMon ParentPID SpawnedThread/ParentPID SpawnedThread2/ParentPID [...]` followed by a `p` for /proc/stat poll only mode or leave empty for main thread + spawned threads 

Example: `./cpuMon 988 1233 1232 p` or `./cpuMon 988 1233 1232`