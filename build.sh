#! /bin/bash

today=`date +%d%m%y_%H%M`

exiting()
{
	echo "Exiting \"build.sh\" Application"
	echo
	make clean
}

run()
{
	echo "Running the Application on $today"
	./adsB
	read -n1 -p "Do you want to Run the Application (y/n)? " resp
	echo
	if [ $resp = "y" ]
	then
		exiting
	else
		echo "Never Mind ** Exiting ** Thanks"
	fi
}

read -n1 -p "Do you want to Run the Application (y/n)? " resp
echo

if [ $resp = "y" ]
then
	if ( make )
	then
		run
	else
		echo "Never Mind ** Exiting ** Thanks"
		echo 
	fi
else
	exiting
fi
