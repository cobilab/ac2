#include "mem.h"
#include "translate.h"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// INVERTED REPEAT AMINO ACID
//
uint8_t InvRevAmAcid(uint8_t code, uint8_t amino_acid){
  switch(amino_acid){
/*	  

    // XXX: THE INPUT SEQUENCE MUST CONTAIN ALL SYMBOLS THAT HAVE BEEN MAPPED!
      
    case 'A': return 'R'; break;
    case 'C': return 'A'; break;
    case 'D': return 'I'; break;
    case 'E': return 'F'; break;
    case 'F': return 'K'; break;
    case 'G': return 'A'; break;
    case 'H': return 'M'; break;
    case 'I': return 'N'; break;
    case 'K': return 'F'; break;
    case 'L': return 'Q'; break;
    case 'M': return 'H'; break;
    case 'N': return 'V'; break;
    case 'P': return 'R'; break;
    case 'Q': return 'L'; break;
    case 'R': return 'A'; break;
    case 'S': return 'A'; break;
    case 'T': return 'G'; break;
    case 'V': return 'H'; break;
    case 'W': return 'P'; break;
    case 'X': return 'X'; break;
    case 'Y': return 'I'; break;
    case '*': return 'L'; break;
    case '-': return 'L'; break;
*/
    case 'A': return 'H'; break;
    case 'B': return 'I'; break;
    case 'C': return 'J'; break;
    case 'D': return 'K'; break;
    case 'E': return 'L'; break;
    case 'F': return 'M'; break;
    case 'G': return 'N'; break;
    case 'H': return 'O'; break;
    case 'I': return 'P'; break;
    case 'J': return 'Q'; break;
    case 'K': return 'R'; break;
    case 'L': return 'S'; break;
    case 'M': return 'T'; break;
    case 'N': return 'U'; break;
    case 'O': return 'V'; break;
    case 'P': return 'W'; break;
    case 'Q': return 'X'; break;
    case 'R': return 'Y'; break;
    case 'S': return 'Z'; break;
    case 'T': return 'A'; break;
    case 'U': return 'B'; break;
    case 'V': return 'C'; break;
    case 'W': return 'D'; break;
    case 'X': return 'E'; break;
    case 'Y': return 'F'; break;
    case 'Z': return 'G'; break;

    default: return amino_acid;

/*
    case 'A': return 'T';
    case 'T': return 'A';
    case 'C': return 'G';
    case 'G': return 'C';
*/
    }
  return amino_acid;
  }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
