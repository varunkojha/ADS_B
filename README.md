# Project
This project was done 4 years back, committing now

AircraftADS_B
ADS_B Message Parser on SBC/RTOS


20 Aug 2019: 
Read "Recording_2-28-2019.txt" and extracted "DF_17" messages into "DF_17_messages.txt"


Run following:
./build.sh
Press 'y' for "yes" and 'n' for "no"
Do you want to Run the Application (y/n)? y

gcc extractDF17.c -o extractDF17
./extractDF17
cat DF_17_messages.txt


SQUITTER TYPE
1. SHORT SQUITTER
CONTROL | MODE S ADDRESS | PARITY
(8)         (24)            (24)   =   56 BITS

2. LONG SQUITTER
CONTROL | MODE S ADDRESS | ADS-B MESSAGE | PARITY
(8)		(24)		(56 BITS)   (24)   =  112 BITS

---------------------------------------------------------------------------------------
NOTE: Will sort the files, as of now printing on console
Use:
gcc genDecodeMessage.c -o genDecodeMessage.c
./genDecodeMessage
