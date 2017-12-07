# LG_SW_SMART_DOOR
LG 일등 SW 32기 C반 SMART DOOR 프로젝트

How to Compile
	1. move LG_SW_SMART_DOOR folder into home directory
	2. ssh into Raspberry Pi
	3. nfs mount home directory onto pi
	4. cd into LG_SW_SMART_DOOR/workspace/face_detect
	5. execute the following command
		sudo ./make_sd.sh

How to Run
	1. install VNC Viewer
	2. go to your VNC Viewer of the Raspberry Pi
	3. cd into LG_SW_SMART_DOOR/workspace/face_detect
	4. execute the following command
		sudo ./test
