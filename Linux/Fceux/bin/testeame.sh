cd ../../FceuxHeadlessSrc
./install.sh 
cd ../Fceux\ Headless/bin
./fceux -d $3 $1 > ../../Fceux/bin/$2 
cd ../../Fceux/bin 
./fceux -I 0 -p $2 $1
