#!/bin/bash
###############################################################################
INSTALL_NAF=0;
INSTALL_BBB=0;
INSTALL_CMIX=0;
INSTALL_LZMA=0;
INSTALL_AC=0;
INSTALL_AC2=0;
###############################################################################
DOWNLOAD=0;
###############################################################################
RUN_NAF=0;
RUN_BBB=0;
RUN_CMIX=0;
RUN_LZMA=0;
RUN_AC=0;
RUN_AC2=1;
#
function RunAC {
   # 1 - params
   # 2 - seq
if test -f "../../ds/$2"; then
   cp ../../ds/$2 .
   rm -f $2.co
   (/usr/bin/time -v ./AC -v $1 $2 ) &> ../../res/C_AC_$2
   ls -la $2.co | awk '{ print $5;}' > ../../res/BC_AC_$2
   rm -f $2 $2.co;
fi
}

function RunAC2 {
    # 1 - params
    # 2 - seq
if test -f "../../ds/$2"; then
   cp ../../ds/$2 .
   rm -f $2.co
   (/usr/bin/time -v ./AC2 -v $1 $2 ) &> ../../res/C_AC2_$2
   ls -la $2.co | awk '{ print $5;}' > ../../res/BC_AC2_$2
   rm -f $2 $2.co;
fi
}

function RunNAF {
    # 1 - seq
if test -f "../../ds/$1"; then
   echo ">" > $1.fasta
   cat ../../ds/$1 >> $1.fasta
   rm -f $1.fasta.naf
   (/usr/bin/time -v ./ENNAF --temp-dir /tmp -22 --protein $1.fasta -o $1.naf ) &> ../../res/C_NAF_$1
   ls -la $1.naf | awk '{ print $5;}' > ../../res/BC_NAF_$1
   rm -f $1.fasta $1.naf;
fi
}

function RunCMIX {
    # 1 - seq
if test -f "../../ds/$1"; then
   cp ../../ds/$1 .
   rm -f $1.cmix
   (/usr/bin/time -v ./cmix -c $1 $1.cmix ) &> ../../res/C_CMIX_$1
   ls -la $1.cmix | awk '{ print $5;}' > ../../res/BC_CMIX_$1
   rm -f $1 $1.cmix;
fi
}

function RunBBB {
    # 1 - seq
if test -f "../../ds/$1"; then
   cp ../../ds/$1 .
   rm -f $1.bbb
   (/usr/bin/time -v ./bbb cfm100q $1 $1.bbb ) &> ../../res/C_BBB_$1
   ls -la $1.bbb | awk '{ print $5;}' > ../../res/BC_BBB_$1
   rm -f $1 $1.bbb;
fi
}

function RunLZMA {
    # 1 - seq
if test -f "../../ds/$1"; then
   cp ../../ds/$1 .
   rm -f $1.lzma
   (/usr/bin/time -v lzma -9 -e -k $1 ) &> ../../res/C_LZMA_$1
   ls -la $1.lzma | awk '{ print $5;}' > ../../res/BC_LZMA_$1
   rm -f $1 $1.lzma;
fi
}



#
###############################################################################
# INSTALL
mkdir -p ds
mkdir -p progs
cd progs/
###############################################################################
# GET AC
#====================================================================
if [[ "$INSTALL_AC" -eq "1" ]]; then
   rm -fr ac/
   git clone https://github.com/cobilab/ac.git
   cd ac/src/
   cp Makefile.linux Makefile
   make
   cp AC ../
   cp AD ../
   cd ../../
fi
###############################################################################
# GET AC2
#====================================================================
if [[ "$INSTALL_AC2" -eq "1" ]]; then
   rm -fr ac2/
   git clone https://github.com/miltondts/ac2.git
   cd ac2/src/
   make
   cp AC2 ../
   cp AD2 ../
   cd ../../
fi
###############################################################################
# GET NAF
#====================================================================
if [[ "$INSTALL_NAF" -eq "1" ]]; then
   rm -fr naf/
   git clone --recurse-submodules https://github.com/KirillKryukov/naf.git
   cd naf && make
   cp ennaf/ennaf ENNAF # to avoid dir name collision
   cd ../
fi
###############################################################################
# GET CMIX
#====================================================================
if [[ "$INSTALL_CMIX" -eq "1" ]]; then
    rm -fr cmix/
    git clone https://github.com/byronknoll/cmix.git
    cd cmix
    make
    cd ../
fi
###############################################################################
# GET BBB
#====================================================================
if [[ "$INSTALL_BBB" -eq "1" ]]; then
    rm -fr bbb/
    mkdir bbb/
    cd bbb/
    wget http://mattmahoney.net/dc/bbb.zip
    unzip bbb.zip
    rm bbb.zip bbb bbb.exe
    g++ -Wall -O3 -march=native -flto -o bbb bbb.cpp
    cd ../
fi
###############################################################################
# GET LZMA
#====================================================================
if [[ "$INSTALL_LZMA" -eq "1" ]]; then
    rm -fr lzma/
    mkdir lzma/
fi


##############################################################################
cd ..
###############################################################################
# DOWNLOAD
if [[ "$DOWNLOAD" -eq "1" ]]; then
    echo "Downloading ..."
    rm -rf ds/
    mkdir ds/
    cd ds
    cd ..
fi
#
# RUN
#=========================================================================
## Gen purpose
if [[ "$RUN_CMIX" -eq "1" ]]; then
   echo "Running CMIX ...";
   mkdir -p res
   cd progs/cmix

   RunCMIX  "EP"
   RunCMIX  "XV"
   RunCMIX  "FV"
   RunCMIX  "FM"
   RunCMIX  "HA"
   RunCMIX  "AP"
   RunCMIX  "DA"
   RunCMIX  "MJ"
   RunCMIX  "HI"
   RunCMIX  "SA"
   RunCMIX  "LC"
   RunCMIX  "EC"
   RunCMIX  "HT"
   RunCMIX  "SC"
   RunCMIX  "HS"
   RunCMIX  "BT"
   
   RunCMIX  "pdbaa"
   RunCMIX  "GRCh38"
   RunCMIX  "uniprot"
   #
   cd ../../
   echo "Done!";
fi


if [[ "$RUN_LZMA" -eq "1" ]]; then
   echo "Running LZMA ...";
   mkdir -p res
   cd progs/lzma
  
   RunLZMA  "EP"
   RunLZMA  "XV"
   RunLZMA  "FV"
   RunLZMA  "FM"
   RunLZMA  "HA"
   RunLZMA  "AP"
   RunLZMA  "DA"
   RunLZMA  "MJ"
   RunLZMA  "HI"
   RunLZMA  "SA"
   RunLZMA  "LC"
   RunLZMA  "EC"
   RunLZMA  "HT"
   RunLZMA  "SC"
   RunLZMA  "HS"
   RunLZMA  "BT"
   
   RunLZMA  "uniprot"
   RunLZMA  "GRCh38"
   RunLZMA  "pdbaa"


   #
   cd ../../
   echo "Done!";
fi

if [[ "$RUN_BBB" -eq "1" ]]; then
   echo "Running BBB ...";
   mkdir -p res
   cd progs/bbb
   

   RunBBB  "EP"
   RunBBB  "XV"
   RunBBB  "FV"
   RunBBB  "FM"
   RunBBB  "HA"
   RunBBB  "AP"
   RunBBB  "DA"
   RunBBB  "MJ"
   RunBBB  "HI"
   RunBBB  "SA"
   RunBBB  "LC"
   RunBBB  "EC"
   RunBBB  "HT"
   RunBBB  "SC"
   RunBBB  "HS"
   RunBBB  "BT"

   RunBBB  "uniprot"
   RunBBB  "GRCh38"
   RunBBB  "pdbaa"

   #
   cd ../../
   echo "Done!";
fi
###################################################################
#### Specific
if [[ "$RUN_NAF" -eq "1" ]]; then
   echo "Running NAF ...";
   mkdir -p res
   cd progs/naf


   RunNAF  "EP"
   RunNAF  "XV"
   RunNAF  "FV"
   RunNAF  "FM"
   RunNAF  "HA"
   RunNAF  "AP"
   RunNAF  "DA"
   RunNAF  "MJ"
   RunNAF  "HI"
   RunNAF  "SA"
   RunNAF  "LC"
   RunNAF  "EC"
   RunNAF  "HT"
   RunNAF  "SC"
   RunNAF  "HS"
   RunNAF  "BT"

   RunNAF  "uniprot"
   RunNAF  "GRCh38"
   RunNAF  "pdbaa"
   #
   cd ../../
   echo "Done!";
fi


###############################################################################
if [[ "$RUN_AC" -eq "1" ]]; then
    echo "Running AC ...";
    
   mkdir -p res
   cd progs/ac

   RunAC  "-l 1 -lr 0.2 -hs 16" "EP"
   RunAC  "-l 1 -lr 0.2 -hs 16" "XV"
   RunAC  "-l 2 -lr 0.05 -hs 24" "FV"
   RunAC  "-l 7 -lr 0.5 -hs 48" "FM"
   RunAC  "-l 2 -lr 0.12 -hs 24" "HA"
   RunAC  "-l 5 -lr 0.4 -hs 40" "AP"
   RunAC  "-l 4 -lr 0.4 -hs 40" "DA"
   RunAC  "-l 4 -lr 0.3 -hs 40" "MJ"
   RunAC  "-l 4 -lr 0.5 -hs 40" "HI"
   RunAC  "-l 5 -lr 0.4 -hs 48" "SA"
   RunAC  "-l 5 -lr 0.4 -hs 64" "LC"
   RunAC  "-l 5 -lr 0.4 -hs 64" "EC"
   RunAC  "-l 7 -lr 0.17 -hs 64" "HT"
   RunAC  "-l 6 -lr 0.17 -hs 64" "SC"
   RunAC  "-l 7 -lr 0.17 -hs 64" "HS"
   RunAC  "-l 7 -lr 0.16 -hs 64" "BT"

   RunAC  "-l 7 -lr 0.16 -hs 64" "GRCh38"
   RunAC  "-l 7 -lr 0.16 -hs 64" "pdbaa"
   RunAC  "-l 5 -lr 0.16 -hs 64" "uniprot"
   #
   cd ../../
   echo "Done!";
fi

if [[ "$RUN_AC2" -eq "1" ]]; then
    echo "Running AC2 ...";
    
   mkdir -p res
   cd progs/ac2

   RunAC2  "-l 1 -lr 0.16 -hs 16" "EP"
   RunAC2  "-l 1 -lr 0.16 -hs 16" "XV"
   RunAC2  "-l 2 -lr 0.16 -hs 40" "FV"
   RunAC2  "-l 7 -lr 0.5 -hs 64" "FM"
   RunAC2  "-l 2 -lr 0.16 -hs 24" "HA"
   RunAC2  "-l 5 -lr 0.3 -hs 64" "AP"
   RunAC2  "-l 4 -lr 0.3 -hs 40" "DA"
   RunAC2  "-l 4 -lr 0.3 -hs 40" "MJ"
   RunAC2  "-l 4 -lr 0.3 -hs 40" "HI"
   RunAC2  "-l 5 -lr 0.3 -hs 48" "SA"
   RunAC2  "-l 5 -lr 0.3 -hs 48" "LC"
   RunAC2  "-l 5 -lr 0.4 -hs 80" "EC"
   RunAC2  "-l 7 -lr 0.3 -hs 64" "HT"
   RunAC2  "-l 6 -lr 0.17 -hs 64" "SC"
   RunAC2  "-l 7 -lr 0.17 -hs 80" "HS"
   RunAC2  "-l 7 -lr 0.16 -hs 80" "BT"

   RunAC2  "-l 7 -lr 0.16 -hs 64" "GRCh38"
   RunAC2  "-l 7 -lr 0.16 -hs 64" "pdbaa"
   RunAC2  "-l 5 -lr 0.16 -hs 64" "uniprot"

   #RunAC2  "-l 1 -lr 0.16 -hs 16" "EP"
   #RunAC2  "-l 1 -lr 0.16 -hs 16" "XV"
   #RunAC2  "-l 2 -lr 0.16 -hs 24" "FV"
   #RunAC2  "-l 7 -lr 0.16 -hs 48" "FM"
   #RunAC2  "-l 2 -lr 0.16 -hs 24" "HA"
   #RunAC2  "-l 5 -lr 0.16 -hs 40" "AP"
   #RunAC2  "-l 4 -lr 0.16 -hs 40" "DA"
   #RunAC2  "-l 4 -lr 0.16 -hs 40" "MJ"
   #RunAC2  "-l 4 -lr 0.16 -hs 40" "HI"
   #RunAC2  "-l 5 -lr 0.16 -hs 48" "SA"
   #RunAC2  "-l 5 -lr 0.16 -hs 64" "LC"
   #RunAC2  "-l 5 -lr 0.16 -hs 64" "EC"
   #RunAC2  "-l 7 -lr 0.16 -hs 64" "HT"
   #RunAC2  "-l 6 -lr 0.16 -hs 64" "SC"
   #RunAC2  "-l 7 -lr 0.16 -hs 64" "HS"
   #RunAC2  "-l 7 -lr 0.16 -hs 64" "BT"

   #RunAC2  "-l 7 -lr 0.16 -hs 64" "GRCh38"
   #RunAC2  "-l 7 -lr 0.16 -hs 64" "pdbaa"
   #RunAC2  "-l 5 -lr 0.16 -hs 64" "uniprot"

   #
   cd ../../
   echo "Done!";
fi

