""" Python Character Mapping Codec generated from 'CP869.TXT'.


Written by Marc-Andre Lemburg (mal@lemburg.com).

(c) Copyright CNRI, All Rights Reserved. NO WARRANTY.

"""#"

import codecs

### Codec APIs

class Codec(codecs.Codec):

    def encode(self,input,errors='strict'):

        return codecs.charmap_encode(input,errors,encoding_map)
        
    def decode(self,input,errors='strict'):

        return codecs.charmap_decode(input,errors,decoding_map)

class StreamWriter(Codec,codecs.StreamWriter):
    pass
        
class StreamReader(Codec,codecs.StreamReader):
    pass

### encodings module API

def getregentry():

    return (Codec().encode,Codec().decode,StreamReader,StreamWriter)

### Decoding Map

decoding_map = {

	0x0080: None,	# UNDEFINED
	0x0081: None,	# UNDEFINED
	0x0082: None,	# UNDEFINED
	0x0083: None,	# UNDEFINED
	0x0084: None,	# UNDEFINED
	0x0085: None,	# UNDEFINED
	0x0086: 0x0386,	# GREEK CAPITAL LETTER ALPHA WITH TONOS
	0x0087: None,	# UNDEFINED
	0x0088: 0x00b7,	# MIDDLE DOT
	0x0089: 0x00ac,	# NOT SIGN
	0x008a: 0x00a6,	# BROKEN BAR
	0x008b: 0x2018,	# LEFT SINGLE QUOTATION MARK
	0x008c: 0x2019,	# RIGHT SINGLE QUOTATION MARK
	0x008d: 0x0388,	# GREEK CAPITAL LETTER EPSILON WITH TONOS
	0x008e: 0x2015,	# HORIZONTAL BAR
	0x008f: 0x0389,	# GREEK CAPITAL LETTER ETA WITH TONOS
	0x0090: 0x038a,	# GREEK CAPITAL LETTER IOTA WITH TONOS
	0x0091: 0x03aa,	# GREEK CAPITAL LETTER IOTA WITH DIALYTIKA
	0x0092: 0x038c,	# GREEK CAPITAL LETTER OMICRON WITH TONOS
	0x0093: None,	# UNDEFINED
	0x0094: None,	# UNDEFINED
	0x0095: 0x038e,	# GREEK CAPITAL LETTER UPSILON WITH TONOS
	0x0096: 0x03ab,	# GREEK CAPITAL LETTER UPSILON WITH DIALYTIKA
	0x0097: 0x00a9,	# COPYRIGHT SIGN
	0x0098: 0x038f,	# GREEK CAPITAL LETTER OMEGA WITH TONOS
	0x0099: 0x00b2,	# SUPERSCRIPT TWO
	0x009a: 0x00b3,	# SUPERSCRIPT THREE
	0x009b: 0x03ac,	# GREEK SMALL LETTER ALPHA WITH TONOS
	0x009c: 0x00a3,	# POUND SIGN
	0x009d: 0x03ad,	# GREEK SMALL LETTER EPSILON WITH TONOS
	0x009e: 0x03ae,	# GREEK SMALL LETTER ETA WITH TONOS
	0x009f: 0x03af,	# GREEK SMALL LETTER IOTA WITH TONOS
	0x00a0: 0x03ca,	# GREEK SMALL LETTER IOTA WITH DIALYTIKA
	0x00a1: 0x0390,	# GREEK SMALL LETTER IOTA WITH DIALYTIKA AND TONOS
	0x00a2: 0x03cc,	# GREEK SMALL LETTER OMICRON WITH TONOS
	0x00a3: 0x03cd,	# GREEK SMALL LETTER UPSILON WITH TONOS
	0x00a4: 0x0391,	# GREEK CAPITAL LETTER ALPHA
	0x00a5: 0x0392,	# GREEK CAPITAL LETTER BETA
	0x00a6: 0x0393,	# GREEK CAPITAL LETTER GAMMA
	0x00a7: 0x0394,	# GREEK CAPITAL LETTER DELTA
	0x00a8: 0x0395,	# GREEK CAPITAL LETTER EPSILON
	0x00a9: 0x0396,	# GREEK CAPITAL LETTER ZETA
	0x00aa: 0x0397,	# GREEK CAPITAL LETTER ETA
	0x00ab: 0x00bd,	# VULGAR FRACTION ONE HALF
	0x00ac: 0x0398,	# GREEK CAPITAL LETTER THETA
	0x00ad: 0x0399,	# GREEK CAPITAL LETTER IOTA
	0x00ae: 0x00ab,	# LEFT-POINTING DOUBLE ANGLE QUOTATION MARK
	0x00af: 0x00bb,	# RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK
	0x00b0: 0x2591,	# LIGHT SHADE
	0x00b1: 0x2592,	# MEDIUM SHADE
	0x00b2: 0x2593,	# DARK SHADE
	0x00b3: 0x2502,	# BOX DRAWINGS LIGHT VERTICAL
	0x00b4: 0x2524,	# BOX DRAWINGS LIGHT VERTICAL AND LEFT
	0x00b5: 0x039a,	# GREEK CAPITAL LETTER KAPPA
	0x00b6: 0x039b,	# GREEK CAPITAL LETTER LAMDA
	0x00b7: 0x039c,	# GREEK CAPITAL LETTER MU
	0x00b8: 0x039d,	# GREEK CAPITAL LETTER NU
	0x00b9: 0x2563,	# BOX DRAWINGS DOUBLE VERTICAL AND LEFT
	0x00ba: 0x2551,	# BOX DRAWINGS DOUBLE VERTICAL
	0x00bb: 0x2557,	# BOX DRAWINGS DOUBLE DOWN AND LEFT
	0x00bc: 0x255d,	# BOX DRAWINGS DOUBLE UP AND LEFT
	0x00bd: 0x039e,	# GREEK CAPITAL LETTER XI
	0x00be: 0x039f,	# GREEK CAPITAL LETTER OMICRON
	0x00bf: 0x2510,	# BOX DRAWINGS LIGHT DOWN AND LEFT
	0x00c0: 0x2514,	# BOX DRAWINGS LIGHT UP AND RIGHT
	0x00c1: 0x2534,	# BOX DRAWINGS LIGHT UP AND HORIZONTAL
	0x00c2: 0x252c,	# BOX DRAWINGS LIGHT DOWN AND HORIZONTAL
	0x00c3: 0x251c,	# BOX DRAWINGS LIGHT VERTICAL AND RIGHT
	0x00c4: 0x2500,	# BOX DRAWINGS LIGHT HORIZONTAL
	0x00c5: 0x253c,	# BOX DRAWINGS LIGHT VERTICAL AND HORIZONTAL
	0x00c6: 0x03a0,	# GREEK CAPITAL LETTER PI
	0x00c7: 0x03a1,	# GREEK CAPITAL LETTER RHO
	0x00c8: 0x255a,	# BOX DRAWINGS DOUBLE UP AND RIGHT
	0x00c9: 0x2554,	# BOX DRAWINGS DOUBLE DOWN AND RIGHT
	0x00ca: 0x2569,	# BOX DRAWINGS DOUBLE UP AND HORIZONTAL
	0x00cb: 0x2566,	# BOX DRAWINGS DOUBLE DOWN AND HORIZONTAL
	0x00cc: 0x2560,	# BOX DRAWINGS DOUBLE VERTICAL AND RIGHT
	0x00cd: 0x2550,	# BOX DRAWINGS DOUBLE HORIZONTAL
	0x00ce: 0x256c,	# BOX DRAWINGS DOUBLE VERTICAL AND HORIZONTAL
	0x00cf: 0x03a3,	# GREEK CAPITAL LETTER SIGMA
	0x00d0: 0x03a4,	# GREEK CAPITAL LETTER TAU
	0x00d1: 0x03a5,	# GREEK CAPITAL LETTER UPSILON
	0x00d2: 0x03a6,	# GREEK CAPITAL LETTER PHI
	0x00d3: 0x03a7,	# GREEK CAPITAL LETTER CHI
	0x00d4: 0x03a8,	# GREEK CAPITAL LETTER PSI
	0x00d5: 0x03a9,	# GREEK CAPITAL LETTER OMEGA
	0x00d6: 0x03b1,	# GREEK SMALL LETTER ALPHA
	0x00d7: 0x03b2,	# GREEK SMALL LETTER BETA
	0x00d8: 0x03b3,	# GREEK SMALL LETTER GAMMA
	0x00d9: 0x2518,	# BOX DRAWINGS LIGHT UP AND LEFT
	0x00da: 0x250c,	# BOX DRAWINGS LIGHT DOWN AND RIGHT
	0x00db: 0x2588,	# FULL BLOCK
	0x00dc: 0x2584,	# LOWER HALF BLOCK
	0x00dd: 0x03b4,	# GREEK SMALL LETTER DELTA
	0x00de: 0x03b5,	# GREEK SMALL LETTER EPSILON
	0x00df: 0x2580,	# UPPER HALF BLOCK
	0x00e0: 0x03b6,	# GREEK SMALL LETTER ZETA
	0x00e1: 0x03b7,	# GREEK SMALL LETTER ETA
	0x00e2: 0x03b8,	# GREEK SMALL LETTER THETA
	0x00e3: 0x03b9,	# GREEK SMALL LETTER IOTA
	0x00e4: 0x03ba,	# GREEK SMALL LETTER KAPPA
	0x00e5: 0x03bb,	# GREEK SMALL LETTER LAMDA
	0x00e6: 0x03bc,	# GREEK SMALL LETTER MU
	0x00e7: 0x03bd,	# GREEK SMALL LETTER NU
	0x00e8: 0x03be,	# GREEK SMALL LETTER XI
	0x00e9: 0x03bf,	# GREEK SMALL LETTER OMICRON
	0x00ea: 0x03c0,	# GREEK SMALL LETTER PI
	0x00eb: 0x03c1,	# GREEK SMALL LETTER RHO
	0x00ec: 0x03c3,	# GREEK SMALL LETTER SIGMA
	0x00ed: 0x03c2,	# GREEK SMALL LETTER FINAL SIGMA
	0x00ee: 0x03c4,	# GREEK SMALL LETTER TAU
	0x00ef: 0x0384,	# GREEK TONOS
	0x00f0: 0x00ad,	# SOFT HYPHEN
	0x00f1: 0x00b1,	# PLUS-MINUS SIGN
	0x00f2: 0x03c5,	# GREEK SMALL LETTER UPSILON
	0x00f3: 0x03c6,	# GREEK SMALL LETTER PHI
	0x00f4: 0x03c7,	# GREEK SMALL LETTER CHI
	0x00f5: 0x00a7,	# SECTION SIGN
	0x00f6: 0x03c8,	# GREEK SMALL LETTER PSI
	0x00f7: 0x0385,	# GREEK DIALYTIKA TONOS
	0x00f8: 0x00b0,	# DEGREE SIGN
	0x00f9: 0x00a8,	# DIAERESIS
	0x00fa: 0x03c9,	# GREEK SMALL LETTER OMEGA
	0x00fb: 0x03cb,	# GREEK SMALL LETTER UPSILON WITH DIALYTIKA
	0x00fc: 0x03b0,	# GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND TONOS
	0x00fd: 0x03ce,	# GREEK SMALL LETTER OMEGA WITH TONOS
	0x00fe: 0x25a0,	# BLACK SQUARE
	0x00ff: 0x00a0,	# NO-BREAK SPACE
}

### Encoding Map

encoding_map = {}
for k,v in decoding_map.items():
    encoding_map[v] = k
