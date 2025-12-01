./procman
ProcMan iniciado. PID: 4028
ProcMan> 
ProcMan> 
ProcMan> 
ProcMan> create sleep 300
Proceso creado con PID: 4029
ProcMan> 
ProcMan> 
ProcMan> 
ProcMan> create ls -l /tmp
Proceso creado con PID: 4031
ProcMan> total 56
drwx------ 4 root root 4096 Dec  1 15:00 snap-private-tmp
drwx------ 3 root root 4096 Dec  1 14:27 systemd-private-6cb93fef35a04461b095bede5f3d0a0a-colord.service-n1gmvs
drwx------ 3 root root 4096 Dec  1 14:36 systemd-private-6cb93fef35a04461b095bede5f3d0a0a-fwupd.service-xalGV6
drwx------ 3 root root 4096 Dec  1 14:27 systemd-private-6cb93fef35a04461b095bede5f3d0a0a-ModemManager.service-pfcFCG
drwx------ 3 root root 4096 Dec  1 14:27 systemd-private-6cb93fef35a04461b095bede5f3d0a0a-polkit.service-KgG4bz
drwx------ 3 root root 4096 Dec  1 14:27 systemd-private-6cb93fef35a04461b095bede5f3d0a0a-power-profiles-daemon.service-9pTSbY
drwx------ 3 root root 4096 Dec  1 14:27 systemd-private-6cb93fef35a04461b095bede5f3d0a0a-switcheroo-control.service-gIl8jt
drwx------ 3 root root 4096 Dec  1 14:27 systemd-private-6cb93fef35a04461b095bede5f3d0a0a-systemd-logind.service-okOXuv
drwx------ 3 root root 4096 Dec  1 14:27 systemd-private-6cb93fef35a04461b095bede5f3d0a0a-systemd-oomd.service-p8p2Ku
drwx------ 3 root root 4096 Dec  1 14:27 systemd-private-6cb93fef35a04461b095bede5f3d0a0a-systemd-resolved.service-BMnVXB
drwx------ 3 root root 4096 Dec  1 14:27 systemd-private-6cb93fef35a04461b095bede5f3d0a0a-systemd-timesyncd.service-7XRohl
drwx------ 3 root root 4096 Dec  1 14:27 systemd-private-6cb93fef35a04461b095bede5f3d0a0a-upower.service-xuHtyz
drwxrwxrwt 2 root root 4096 Dec  1 14:27 VMwareDnD
drwx------ 2 root root 4096 Dec  1 14:27 vmware-root_755-4281712171

ProcMan> 
ProcMan> 
ProcMan> list

PID	COMMAND			RUNTIME		STATUS		EXIT_CODE
-----	---------------------------	-----------	-----------	---------
4029	sleep 300                	00:00:14	Running	
0	ls -l /tmp               	00:00:02	Terminated	0

