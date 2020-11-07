#AC2
====
AC2 is an amino acid compressor that uses a neural network to do the mixing of experts.

Build
----
AC2 depends on `make` and `gcc`.

```
cd src
make
```
**NOTE:** The default compilation options use micro-architecture dependent instructions, because AC2 greatly benefits from vector instructions. This means that due to the different floating point accuracies (e.g. fused multiply–add) the compressed file might not decompress with a binary compressed in a different computer or with a different compiler version or options. To ensure the file decompresses, use binaries (AC2 and AD2) that were compiled in the same environment.

Example usage
----
Reference free:
```
# Compression of file BT, using level 7, learning rate 0.16 and 80 hidden nodes.
./AC2 -l 7 -lr 0.16 -hs 80 BT

# Decompression
./AD2 BT.co
```

A complete description of the parameters can be read by invoking:
```
./AC2 -h
./AD2 -h
```

Export mixer to other compressors
----
The mixer interface and implementation is done in:
```
mix.h
mix.c
```

The neural network implementation used by the mixer is done in:
```
nn.h
nn.c
```

`fastonebigheader.h` contains fast math functions, and is used by the neural network and the mixer.

**NOTE:** `#include "defs.h"` should be removed from nn.h.

With these five files integrated into the project, the compressor now needs to `#include "mix.h"` in the file where the mixing will occur.

The mixer is initialized with: `mix_state_t *mxs = mix_init(NUMBER_OF_MODELS, ALPHABET_SIZE, NUMBER_OF_HIDDEN_NODES);`

Every time a new symbol is seen the mixing is done with `const float* y = mix(mxs, probs);`. 

`probs[MODEL_IDX][SYMBOL_IDX]`is a matrix with the probabilities (]0,1[) for each model and for each symbol in the alphabet. The return value contains the non-normalized probabilities for each symbol. One way of normalizing these probabilities is to divide each by the total sum.

Training must be done after invoking the mixing with `mix_update_state(mxs, probs, sym, lr);`. Where `sym` is the actual symbol seen encoded as an int [0, ALPHABET_SIZE[ and`lr`is the learning rate.

Finally, the mixer can be freed with `mix_free(mxs);`
