# High Frequency CPU Monitor
Tool meant for aiding in monitoring singular process CPU loads. Polls /proc/ every n ms.

## Usage
`gcc monitorProc.c -o cpuMon`

`./cpuMon ParentPID SpawnedThread1 SpawnedThread2 ...`