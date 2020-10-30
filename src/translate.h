#ifndef TRANSLATE_H_INCLUDED
#define TRANSLATE_H_INCLUDED

#include "defs.h"

/*
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

const char *AA = "FLIMVSPTAY*HQNKDECWRGX";

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   
const char TA[ 17 ][ 22 ][ 3 ] = 
   // Standard genetic code
   {{"TTY","YTN","ATH","ATG","GTN","WSN","CCN","ACN","GCN","TAY",
     "TRR","CAY","CAR","AAY","AAR","GAY","GAR","TGY","TGG","MGN",
     "GGN","NNN"},
   // Vertebrate Mitochondrial
    {"TTY","YTN","ATY","ATR","GTN","WSN","CCN","ACN","GCN","TAY",
     "WRR","CAY","CAR","AAY","AAR","GAY","GAR","TGY","TGR","CGN",
     "GGN","NNN"},
   // Yeast Mitochondrial
    {"TTY","TTR","ATY","ATR","GTN","WSN","CCN","MYN","GCN","TAY",
     "TAR","CAY","CAR","AAY","AAR","GAY","GAR","TGY","TGR","MGN",
     "GGN","NNN"},
   // Mold, Protozoan and Coelenterate Mitochondrial. Mycoplasma, Spiroplasma
    {"TTY","YTN","ATH","ATG","GTN","WSN","CCN","ACN","GCN","TAY",
     "TAR","CAY","CAR","AAY","AAR","GAY","GAR","TGY","TGR","MGN",
     "GGN","NNN"},
   // Invertebrate Mitochondrial
    {"TTY","YTN","ATY","ATR","GTN","WSN","CCN","WSN","GCN","TAY",
     "TAR","CAY","CAR","AAY","AAR","GAY","GAR","TGY","TGR","CGN",
     "GGN","NNN"},
   // Ciliate Nuclear; Dasycladacean Nuclear; Hexamita Nuclear
    {"TTY","YTN","ATH","ATG","GTN","WSN","CCN","ACN","GCN","TAY",
     "TGA","CAY","YAR","AAY","AAR","GAY","GAR","TGY","TGG","MGN",
     "GGN","NNN"},
   // Echinoderm Mitochondrial
    {"TTY","YTN","ATH","ATG","GTN","WSN","CCN","WCN","GCN","TAY",
     "TAR","CAY","CAR","AAH","AAG","GAY","GAR","TGY","TGR","CGN",
     "GGN","NNN"},
   // Euplotid Nuclear
    {"TTY","YTN","ATH","ATG","GTN","WSN","CCN","ACN","GCN","TAY",
     "TAR","CAY","CAR","AAY","AAR","GAY","GAR","TGH","TGG","MGN",
     "GGN","NNN"},
   // Bacterial and Plant Plastid
    {"TTY","YTN","ATH","ATG","GTN","WSN","CCN","ACN","GCN","TAY",
     "TRR","CAY","CAR","AAY","AAR","GAY","GAR","TGY","TGG","MGN",
     "GGN","NNN"},
   // Alternative Yeast Nuclear
    {"TTY","YTN","ATH","ATG","GTN","HBN","CCN","ACN","GCN","TAY",
     "TRR","CAY","CAR","AAY","AAR","GAY","GAR","TGY","TGG","MGN",
     "GGN","NNN"},
   // Ascidian Mitochondrial
    {"TTY","YTN","ATY","ATR","GTN","WSN","CCN","ACN","GCN","TAY",
     "TAR","CAY","CAR","AAY","AAR","GAY","GAR","TGY","TGR","CGN",
     "RGN","NNN"},
   // Flatworm Mitochondrial
    {"TTY","YTN","ATH","ATG","GTN","WSN","CCN","ACN","GCN","TAH",
     "TAG","CAY","CAR","ATH","AAG","GAY","GAR","TGY","TGR","CGN",
     "GGN","NNN"},
   // Blepharisma Macronuclear
    {"TTY","YTN","ATH","ATG","GTN","WSN","CCN","ACN","GCN","TAY",
     "TRA","CAY","YAR","AAY","AAR","GAY","GAR","TGY","TGG","MGN",
     "GGN","NNN"},
   // Chlorophycean Mitochondrial
    {"TTY","YWN","ATH","ATG","GTN","WSN","CCN","ACN","GCN","TAY",
     "TRA","CAY","CAR","AAY","AAR","GAY","GAR","TGY","TGG","MGN",
     "GGN","NNN"},
   // Trematode Mitochondrial
    {"TTY","YTN","ATY","ATR","GTN","WSN","CCN","ACN","GCN","TAY",
     "TAR","CAY","CAR","AAH","AAG","GAY","GAR","TGY","TGR","CGN",
     "GGN","NNN"},
   // Scenedesmus obliquus mitochondrial
    {"TTY","YWN","ATH","ATG","GTN","WSB","CCN","ACN","GCN","TAY",
     "TVR","CAY","CAR","AAY","AAR","GAY","GAR","TGY","TGG","MGN",
     "GGN","NNN"},
   // Thraustochytrium mitochondrial code
    {"TTY","YTN","ATH","ATG","GTN","WSN","CCN","ACN","GCN","TAY",
     "TDR","CAY","CAR","AAY","AAR","GAY","GAR","TGY","TGG","MGN",
     "GGN","NNN"}};
*/

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

uint8_t InvRevAmAcid(uint8_t, uint8_t);

#endif
