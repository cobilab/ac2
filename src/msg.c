#include "msg.h"
#include <stdio.h>
#include <stdlib.h>

void ModelsExplanation(void){
  fprintf(stderr,
  "                                                                       \n"
  "  -rm <c>:<d>:<g>/<m>:<e>:<a>  reference model (-rm 1:10:0.9/0:0:0),   \n"
  "  -rm <c>:<d>:<g>/<m>:<e>:<a>  reference model (-rm 5:90:0.9/1:50:0.8),\n"
  "  ...                                                                  \n"
  "  -tm <c>:<d>:<g>/<m>:<e>:<a>  target model (-tm 1:1:0.8/0:0:0),       \n"
  "  -tm <c>:<d>:<g>/<m>:<e>:<a>  target model (-tm 7:100:0.9/2:10:0.85), \n"
  "  ...                                                                  \n"
  "                         target and reference templates use <c> for    \n"
  "                         context-order size, <d> for alpha (1/<d>), <g>\n"
  "                         for gamma (decayment forgetting factor) [0;1),\n"
  "                         <m> to the maximum sets the allowed mutations,\n"
  "                         on the context without being discarded (for   \n"
  "                         deep contexts), under the estimator <e>, using\n"
  "                         <a> for gamma (decayment forgetting factor)   \n"
  "                         [0;1) (tolerant model),                       \n");
  } 

void PrintMenuD(void){
  fprintf(stderr,
  "Usage: AD [OPTION]... -r [FILE]  [FILE]:[...]                          \n"
  "Decompression of amino acid sequences (compressed by AC).              \n"
  "                                                                       \n"
  "Non-mandatory arguments:                                               \n"
  "                                                                       \n"
  "  -h                    give this help,                                \n"
  "  -v                    verbose mode (more information),               \n"
  "                                                                       \n"
  "  -r <FILE>             reference file,                                \n"
  "                                                                       \n"
  "Mandatory arguments:                                                   \n"
  "                                                                       \n"
  "  <FILE>                file to uncompress (last argument). For        \n"
  "                        more files use splitting \":\" characters.     \n"
  "                                                                       \n"
  "Report bugs to <{pratas,seyedmorteza,ap}@ua.pt>.                       \n");
  }

void PrintMenu(void){
  fprintf(stderr,
  "Usage: AC [OPTION]... -r [FILE]  [FILE]:[...]                          \n"
  "Compression of amino acid sequences.                                   \n"
  "                                                                       \n"
  "Non-mandatory arguments:                                               \n"
  "                                                                       \n"
  "  -h                     give this help,                               \n"
  "  -s                     show AC compression levels,                   \n"
  "  -v                     verbose mode (more information),              \n"
  "  -V                     display version number,                       \n"
  "  -f                     force overwrite of output,                    \n"
  "  -l <level>             level of compression [1;7] (lazy -tm setup),  \n"
  "  -t <threshold>         threshold frequency to discard from alphabet,\n");
  #ifdef ESTIMATE
  fprintf(stderr,
  "  -e                     it creates a file with the extension \".iae\" \n"
  "                         with the respective information content.      \n");
  #endif
  ModelsExplanation();
  fprintf(stderr,
  "                                                                       \n"
  "  -r <FILE>              reference file (\"-rm\" are loaded here),     \n"
  "                                                                       \n"
  "Mandatory arguments:                                                   \n"
  "                                                                       \n"
  "  <FILE>:<...>:<...>     file to compress (last argument). For more    \n"
  "                         files use splitting \":\" characters.         \n"
  "                                                                       \n"
  "Example:                                                               \n"
  "                                                                       \n"
  "  [Compress]   ./AC -v -tm 1:1:0.8/0:0:0 -tm 5:20:0.9/3:20:0.9 seq.txt \n"
  "  [Decompress] ./AD -v seq.txt.co                                      \n"
  "                                                                       \n"
  "Report bugs to <{pratas,seyedmorteza,ap}@ua.pt>.                       \n");
  }


void PrintVersion(void){
  fprintf(stderr,
  "                                                                       \n"
  "                          ================                             \n"
  "                          | AC & AD v%u.%u |                        \n"
  "                          ================                             \n"
  "                                                                       \n"
  "   Efficient compression and decompression of Amino Acid sequences.    \n"
  "                                                                       \n"
  "Copyright (C) 2017-2018 University of Aveiro. This is a Free software. \n"
  "You may redistribute copies of it under the terms of the GNU - General \n"
  "Public License v3 <http://www.gnu.org/licenses/gpl.html>. There is NOT \n"
  "ANY WARRANTY, to the extent permitted by law. Developed and Written by \n"
  "Diogo Pratas, Morteza Hosseini and Armando J. Pinho\n\n", VERSION, RELEASE);
  }

