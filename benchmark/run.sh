cd ../src && make && cp -f AC2 ../benchmark && cd ../benchmark

./AC2 -l 1 -lr 0.2 -hs 16 ../benchmark/AminoAcidsCorpus/EP
./AC2 -l 1 -lr 0.2 -hs 16 ../benchmark/AminoAcidsCorpus/XV
./AC2 -l 2 -lr 0.05 -hs 24 ../benchmark/AminoAcidsCorpus/FV
./AC2 -l 7 -lr 0.5 -hs 48 ../benchmark/AminoAcidsCorpus/FM
./AC2 -l 2 -lr 0.12 -hs 24 ../benchmark/AminoAcidsCorpus/HA
./AC2 -l 5 -lr 0.4 -hs 40 ../benchmark/AminoAcidsCorpus/AP
./AC2 -l 4 -lr 0.4 -hs 40 ../benchmark/AminoAcidsCorpus/DA
./AC2 -l 4 -lr 0.3 -hs 40 ../benchmark/AminoAcidsCorpus/MJ
./AC2 -l 4 -lr 0.5 -hs 40 ../benchmark/AminoAcidsCorpus/HI
./AC2 -l 5 -lr 0.4 -hs 48 ../benchmark/AminoAcidsCorpus/SA
./AC2 -l 5 -lr 0.4 -hs 64 ../benchmark/AminoAcidsCorpus/LC
./AC2 -l 5 -lr 0.4 -hs 64 ../benchmark/AminoAcidsCorpus/EC
./AC2 -l 7 -lr 0.17 -hs 64 ../benchmark/AminoAcidsCorpus/HT
./AC2 -l 6 -lr 0.17 -hs 64 ../benchmark/AminoAcidsCorpus/SC
./AC2 -l 7 -lr 0.17 -hs 64 ../benchmark/AminoAcidsCorpus/HS
./AC2 -l 7 -lr 0.16 -hs 64 ../benchmark/AminoAcidsCorpus/BT
