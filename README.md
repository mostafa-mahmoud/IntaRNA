[![Build Status](https://travis-ci.org/BackofenLab/IntaRNA.svg?branch=master)](https://travis-ci.org/BackofenLab/IntaRNA)

# IntaRNA version 2.*

**Efficient RNA-RNA interaction prediction incorporating accessibility and 
seeding of interaction sites**

During the last few years, several new small regulatory RNAs 
(sRNAs) have been discovered in bacteria. Most of them act as post-transcriptional 
regulators by base pairing to a target mRNA, causing translational repression 
or activation, or mRNA degradation. Numerous sRNAs have already been identified, 
but the number of experimentally verified targets is considerably lower. 
Consequently, computational target prediction is in great demand. Many existing 
target prediction programs neglect the accessibility of target sites and the 
existence of a seed, while other approaches are either specialized to certain 
types of RNAs or too slow for genome-wide searches.

IntaRNA, developed by
[Prof. Backofen's bioinformatics group at Freiburg University](http://www.bioinf.uni-freiburg.de),
is a general and fast approach to the 
prediction of RNA-RNA interactions incorporating both the accessibility of 
interacting sites 
as well as the existence of a user-definable seed interaction. We successfully applied 
IntaRNA to the prediction of bacterial sRNA targets and determined the exact 
locations of the interactions with a higher accuracy than competing programs. 

For testing or ad hoc use of IntaRNA, you can use its webinterface at the

**==> [Freiburg RNA tools IntaRNA webserver](http://rna.informatik.uni-freiburg.de/IntaRNA/) <==**


## Contribution

Feel free to contribute to this project by writing 
[Issues](https://github.com/BackofenLab/IntaRNA/issues) 
with feature requests, bug reports, or just contact messages.

## Citation
If you use IntaRNA, please cite our articles
- [IntaRNA: efficient prediction of bacterial sRNA targets incorporating target site accessibility and seed regions](http://dx.doi.org/10.1093/bioinformatics/btn544)
  Anke Busch, Andreas S. Richter, and Rolf Backofen, 
  Bioinformatics, 24 no. 24 pp. 2849-56, 2008, DOI(10.1093/bioinformatics/btn544).
- [CopraRNA and IntaRNA: predicting small RNA targets, networks and interaction domains](http://dx.doi.org/10.1093/nar/gku359)
  Patrick R. Wright, Jens Georg, Martin Mann, Dragos A. Sorescu, Andreas S. Richter, Steffen Lott, Robert Kleinkauf, Wolfgang R. Hess, and Rolf Backofen
  Nucleic Acids Research, 42 (W1), W119-W123, 2014, DOI(10.1093/nar/gku359).



<br /><br /><br /><br />
<a name="doc" />
# Documentation

## Overview

The following topics are covered by this documentation:

- [Installation](#install)
  - [Dependencies](#deps)
  - [Cloning from github](#instgithub)
  - [Source code distribution](#instsource)
- [Usage and Parameters](#usage)
  - [Prediction modes, their features and emulated tools](#predModes)
  - [Suboptimal RNA-RNA interaction prediction and output restrictions](#subopts)
  - [Accessibility and unpaired probabilities](#accessibility)
    - [Local versus global unpaired probabilities](#accLocalGlobal)
    - [Read/write accessibility from/to file or stream](#accFromFile)
  - [Multi-threading and parallelized computation](#multithreading)



<br /><br /><br /><br />
<a name="install" />
# Installation

<br /><br />
<a name="deps" />
## Dependencies

- compiler supporting C++11 standard and OpenMP
- GNU autotools (automake, autoconf, ..)
- [boost C++ library](http://www.boost.org/) version >= 1.50.0
- [Vienna RNA package](http://www.tbi.univie.ac.at/RNA/) version >= 2.3.0

<br /><br />
<a name="instgithub" />
## Cloning from github (or downloading ZIP-file)

The data provided within the github repository is no complete distribution and
lacks all system specific generated files. Thus, in order to get started with 
a fresh clone of the IntaRNA repository you have to run the GNU autotools 
to generate all needed files for a proper `configure` and `make`. To this end,
we provide a helper script that as shown in the following.
```bash
# call aclocal, automake, autoconf
bash ./autotools-init.sh
```
Afterwards, you can continue as if you would have downloaded a 
[IntaRNA source code distribution](#instsource).

<br /><br />
<a name="instsource" />
## Cloning from github (or downloading ZIP-file)

When downloading an IntaRNA source code distribution, e.g. from the 
[IntaRNA release page](https://github.com/BackofenLab/IntaRNA/releases), you should 
first ensure, that you have all [dependencies](#deps) installed. If so, you can
simply run the following (assuming `bash` shell).
```bash
# generate system specific files (use -h for options)
./configure
# compile IntaRNA from source
make
# run tests to ensure all went fine
make tests
# install (use 'configure --prefix=XXX' to change install directory)
make install
# install to directory XYZ
make install prefix=XYZ
```





<br /><br /><br /><br />
<a name="usage" />
# Usage and parameters

IntaRNA comes with a vast variety of ways to tune or enhance *YOUR* RNA-RNA prediction.
To this end, different [prediction modes](#predModes) are implemented that allow
to balance 


<br /><br />
<a name="predModes" />
## Prediction modes, their features and emulated tools

For the prediction of *minimum free energy interactions*, the following modes
and according features are supported and can be set via the `--mode` parameter.
The tiem and space complexities are given for the prediction of two sequences
of equal length *n*.

| Features | Heuristic `--mode=H` | Exact-SE `--mode=S` | Exact `--mode=E` |
| -------- | :------------------: | :-----------------: | :--------------: |
| Time complexity (prediction only) | O(*n*^2) | O(*n*^4) | O(*n*^4) |
| Space complexity | O(*n*^2) | O(*n*^2) | O(*n*^4) |
| Seed constraint | x | x | x |
| No seed constraint | x | x | x |
| Minimum free energy interaction | not guaranteed | x | x |
| Overlapping suboptimal interactions | x | x | x |
| Non-overlapping suboptimal interactions | x | - | x |

Note, due to the low run-time requirement of the heuristic prediction mode
(`--mode=H`), heuristic IntaRNA interaction predictions are widely used to screen
for interaction in a genome-wide scale. If you are more interested in specific
details of an interaction site or of two relatively short RNA molecules, you 
should investigate the exact prediction mode (`--mode=S`, or `--mode=E`
if non-overlapping suboptimal prediction is required).

Given these features, we can emulate and extend a couple of RNA-RNA interaction
tools using IntaRNA.

**TargetScan** and **RNAhybrid** are approaches that predict the interaction hybrid with 
minimal interaction energy without consideratio whether or not the interacting 
subsequences are probably involved involved in intramolecular base pairings. Furthermore,
no seed constraint is taken into account.
This prediction result can be emulated (depending on the used prediction mode) 
by running IntaRNA when disabling both the seed constraint
as well as the accessibility integration using
```bash
# prediction results similar to TargetScan/RNAhybrid
IntaRNA [..] --noSeed --qAcc=N --tAcc=N
```
We *add seed-constraint support to TargetScan/RNAhybrid-like computations* by removing the 
`--noSeed` flag from the above call.

**RNAup** was one of the first RNA-RNA interaction prediction approaches that took the 
accessibility of the interacting subsequences into account while not considering the seed feature. 
IntaRNA's exact prediction mode is eventually an alternative implementation when disabling
seed constraint incorporation. Furthermore, the unpaired probabilities used by RNAup to score
the accessibility of subregions are covering the respective overall structural ensemble for each
interacting RNA, such that we have to disable accessibility computation based on local folding (RNAplfold)
using
```bash
# prediction results similar to RNAup
IntaRNA --mode=S --noSeed --qAccW=0 --qAccL=0 --tAccW=0 --tAccL=0
```
We *add seed-constraint support to RNAup-like computations* by removing the 
`--noSeed` flag from the above call.


<br /><br />
<a name="subopts" />
## Suboptimal RNA-RNA interaction prediction and output restrictions

Besides the identification of the optimal (e.g. minimum-free-energy) RNA-RNA 
interaction, IntaRNA enables the enumeration of suboptimal interactions. To this
end, the argument `-n N` or `--outNumber=N` can be used to generate up to `N`
interactions for each query-target pair (including the optimal one). Note, the
suboptimal enumeration is increasingly sorted by energy.

Furthermore, it is possible to *restrict (sub)optimal enumeration* using
- `--outMaxE` : maximal energy for any interaction reported
- `--outDeltaE` : maximal energy difference of suboptimal interactions' energy
  to the minimum free energy interaction
- `--outOverlap` : defines if an where overlapping of reported interaction sites
  is allowed (Note, IntaRNA v1.* used implicitly the 'T' mode):
  - 'N' : no overlap neither in target nor query allowed for reported interactions
  - 'B' : overlap allowed for interacting subsequences for both target and query
  - 'T' : overlap allowed for interacting subsequences in target only 
  - 'Q' : overlap allowed for interacting subsequences in query only 


<br /><br />
<a name="accessibility" />
## Accessibility and unpaired probabilities

Accessibility describes the availability of an RNA subsequence for intermolecular
base pairing. It can be expressed in terms of the probability of the subsequence
to be unpaired (its *unpaired probability* *Pu*).

A limited accessibility, i.e. a low unpaired probability, can be incorporated into
the RNA-RNA interaction prediction by adding according energy penalties. 
These so called *ED* values are transformed unpaired probabilities, i.e. the
penalty for a subsequence partaking in an interaction is given by *ED=-RT log(Pu)*, 
where *Pu* denotes the unpaired probability of the subsequence. Within the 
IntaRNA energy model, *ED* values for both interacting subsequences are considered.

Accessibility incorporation can be disabled for query or target sequences using
`--qAcc=N` or `--tAcc=N`, respectively.

A setup of `--qAcc=C` or `--tAcc=C` (default) enables accessibility computation 
using the Vienna RNA package routines for query or target sequences, respectively.


<a name="accLocalGlobal" />
### Local versus global unpaired probabilities

Exact computation of unpaired probabilities (*Pu* terms) is considers all possible
structures the sequence can adopt (the whole structure ensemble). This is referred
to as *global unpaired probabilities* as computed e.g. by **RNAup**.

Since global probability computation is (a) computationally demanding and (b) not
reasonable for long sequences, local RNA folding was suggested, which also enables
according *local unpaired probability* computation, as e.g. done by **RNAplfold**.
Here, a folding window of a defined length 'screens' along the RNA and computes
unpaired probabilities within the window (while only intramolecular base pairs 
within the window are considered).

IntaRNA enables both global as well as local unpaired probability computation.
To this end, the sliding window length has to be specified in order to enable/disable
local folding.

#### Use case examples global/local unpaired probability computation
The use of global or local accessibilities can be defined independently 
for query and target sequences using `--qAccW|L` and `--tAccW|L`, respectively.
Here, `--?AccW` defines the sliding window length (0 sets it to the whole sequence length)
and `--?AccL` defines the maximal length of considered intramolecular base pairs,
i.e. the maximal number of positions enclosed by a base pair
(0 sets it to the whole sequence length). Both can be defined
independently while respecting `AccL <= AccW`.
```bash
# using global accessibilities for query and target
IntaRNA [..] --qAccW=0 --qAccL=0 --tAccW=0 --qAccL=0
# using local accessibilities for target and global for query
IntaRNA [..] --qAccW=0 --qAccL=0 --tAccW=150 --qAccL=100
```


<a name="accFromFile" />
### Read/write accessibility from/to file or stream

It is possible to read precomputed accessibility values from file or stream to
avoid their runtime demanding computation. To this end, we support the following
formats

| Input format | produced by |
| ---- | --- |
| RNAplfold unpaired probabilities | `RNAplfold -u` or `IntaRNA --out*PuFile` |
| RNAplfold-styled ED values | `IntaRNA --out*AccFile` |

The **RNAplfold** format is a table encoding of a banded upper triangular matrix 
with band width l. First row contains a header comment on the data starting with
`#`. Second line encodes the column headers, i.e. the window width per column.
Every successive line starts with the index (starting from 1) of the window end
followed by a tabulator separated list for each windows value in increasing
window length order. That is, column 2 holds values for window length 1, column 
3 for length 2, ... . The following provides a short output/input 
example for a sequence of length 5 with a maximal window length of 3.

```
#unpaired probabilities
 #i$	l=1	2	3	
1	0.9949492	NA	NA	
2	0.9949079	0.9941056	NA	
3	0.9554214	0.9518663	0.9511048		
4	0.9165814	0.9122866	0.9090283		
5	0.998999	0.915609	0.9117766		
6	0.8549929	0.8541667	0.8448852		

```

#### Use case examples for read/write accessibilities and unpaired probabilities
If you have precomputed data, e.g. the file `plfold_lunp` with unpaired probabilities
computed by **RNAplfold**, you can run
```bash
# fill accessibilities from RNAplfold unpaired probabilities
IntaRNA [..] --qAcc=P --qAccFile=plfold_lunp
# fill accessibilities from RNAplfold unpaired probabilities via pipe
cat plfold_lunp | IntaRNA [..] --qAcc=P --qAccFile=STDIN
```
Another option is to store the accessibility data computed by IntaRNA for 
successive calls using 
```bash
# storing and reusing (target) accessibility data for successive IntaRNA calls
IntaRNA [..] --outPuFilet=intarna.target.pu
IntaRNA [..] --tAcc=P --tAccFile=intarna.target.pu
# piping (target) accessibilities between IntaRNA calls
IntaRNA [..] --outPuFilet=STDOUT | IntaRNA [..] --tAcc=P --tAccFile=STDIN
```


<br /><br />
<a name="outmodes" />
## Output modes

The RNA-RNA interactions predicted by IntaRNA can be provided in different
formats. The style is set via the argument `--outMode` and the different modes
will be discussed below.

Furthermore, it is possible to define *where to output*, i.e. using `--out` 
you can either name a file or one of the stream names `STDOUT`|`STDERR`. Note,
any string not matching one of the two stream names is considered a file name.
The file will be overwritten by IntaRNA!

<a name="outModeDetailed" />
### Detailed RNA-RNA interaction output with ASCII chart

Using `--outMode=0`, a detailed ASCII chart of the interaction together with
various interaction details will be provided. An example is given below.

```bash
# call: IntaRNA.exe -t AAACACCCCCGGUGGUUUGG -q AAACACCCCCGGUGGUUUGG --outMode=0 --noSeed

target
             4         14
             |         |
       5'-AAA    CCC    GUUUGG-3'
             CACC   GGUG
             ||||   ||||
             GUGG   CCAC
    3'-GGUUUG    CCC    AAA-5'
             |         |
            14         4
query

interaction seq1   = 4 -- 14
interaction seq2   = 4 -- 14

interaction energy = -4.14154 kcal/mol
  = E(init)        = 4.1
  + E(loops)       = -13.2
  + E(dangleLeft)  = -1.59828
  + E(dangleRight) = -1.59828
  + E(endLeft)     = 0
  + E(endRight)    = 0
  + ED(seq1)       = 4.07751
  + ED(seq2)       = 4.07751
  + Pu(seq1)       = 0.00133893
  + Pu(seq2)       = 0.00133893
```
Position annotations start indexing with 1.

<a name="outModeCsv" />
### Customizable CSV RNA-RNA interaction output

IntaRNA provides via `--outMode=1` a flexible interface to generate RNA-RNA 
interaction output in CSV format (using `;` as separator).

```bash
# call: IntaRNA.exe -t AAACACCCCCGGUGGUUUGG -q AAACACCCCCGGUGGUUUGG --outMode=1 --noSeed --outOverlap=B -n 3
id1;start1;end1;id2;start2;end2;subseqDP;hybridDP;E
target;4;14;query;4;14;CACCCCCGGUG&CACCCCCGGUG;((((...((((&))))...))));-4.14154
target;5;16;query;5;16;ACCCCCGGUGGU&ACCCCCGGUGGU;(((((.((.(((&))))).)).)));-4.04334
target;1;14;query;4;18;AAACACCCCCGGUG&CACCCCCGGUGGUUU;(((((((...((((&))))...)))).)));-2.94305
```
For each prediction, a row in the CSV is generated.

Using the argument `--outCsvCols`, the user can specify what columns are 
printed to the output using a comma-separated list of colIds. Available colIds 
are
- `id1` : id of first sequence
- `id2` : id of second sequence
- `seq1` : full first sequence
- `seq2` : full second sequence
- `subseq1` : interacting subsequence of first sequence
- `subseq2` : interacting subsequence of second sequence
- `subseqDP` : hybrid subsequences compatible with hybridDP
- `subseqDB` : hybrid subsequences compatible with hybridDB
- `start1` : start index of hybrid in seq1
- `end1` : end index of hybrid in seq1
- `start2` : start index of hybrid in seq2
- `end2` : end index of hybrid in seq2
- `hybridDP` : hybrid in VRNA dot-bracket notation
- `hybridDB` : hybrid in dot-bar notation
- `E` : overall hybridization energy
- `ED1` : ED value of seq1
- `ED2` : ED value of seq2
- `Pu1` : probability to be accessible for seq1
- `Pu2` : probability to be accessible for seq2
- `E_init` : initiation energy
- `E_loops` : sum of loop energies (excluding E_init)
- `E_dangleL` : dangling end contribution of base pair (start1,end2)
- `E_dangleR` : dangling end contribution of base pair (end1,start2)
- `E_endL` : penalty of closing base pair (start1,end2)
- `E_endR` : penalty of closing base pair (end1,start2)
- `seedStart1` : start index of the seed in seq1
- `seedEnd1` : end index of the seed in seq1
- `seedStart2` : start index of the seed in seq2
- `seedEnd2` : end index of the seed in seq2
- `seedE` : overall hybridization energy of the seed only (excluding rest)
- `seedED1` : ED value of seq1 of the seed only (excluding rest)
- `seedED2` : ED value of seq2 of the seed only (excluding rest)
- `seedPu1` : probability of seed region to be accessible for seq1
- `seedPu2` : probability of seed region to be accessible for seq2

Using `--outCsvCols ''`, all available columns are added to the output.

Energies are provided in unit *kcal/mol*, probabilities in the interval [0,1].
Position annotations start indexing with 1.

The `hybridDP` format is a dot-bracket notation as e.g. generated by **RNAup**.
Here, for each target sequence position within the interaction, 
a '.' represents a position not involved
in the interaction while a '(' marks an interacting position. For the query
sequence this is done analogously but using a ')' for interacting positions.
Both resulting strings are concatenated by a separator '&' to yield a single
string encoding of the interaction's base pairing details.

The `hybridDB` format is similar to the `hybridDP` but also provides site information. 
Here, a bar '|' is used in both base pairing encodings (which makes it a 'dot-bar encoding'). 
Furthermore, each interaction string is prefixed 
with the start position of the respective interaction site.

In the following, an altered CSV output for the example from above is generated.
```bash
# call: IntaRNA.exe -t AAACACCCCCGGUGGUUUGG -q AAACACCCCCGGUGGUUUGG --outMode=1 --noSeed --outOverlap=B -n 3 --outCsvCols=Pu1,Pu2,subseqDB,hybridDB
Pu1;Pu2;subseqDB;hybridDB
0.00133893;0.00133893;4CACCCCCGGUG&4CACCCCCGGUG;4||||...||||&4||||...||||
0.00134094;0.00134094;5ACCCCCGGUGGU&5ACCCCCGGUGGU;5|||||.||.|||&5|||||.||.|||
0.00133686;0.0013368;1AAACACCCCCGGUG&4CACCCCCGGUGGUUU;1|||||||...||||&4||||...||||.|||
```


<br /><br />
<a name="multithreading" />
## Multi-threading and parallelized computation

IntaRNA supports the parallelization of the target-query-combination processing. 
The maximal number of threads to be used can be specified using the `--threads` parameter.
If `--threads=k > 0`, than *k* predictions are processed in parallel.

When using parallelization, you should have the following things in mind:

- Most of the IntaRNA runtime (in heuristic prediction mode) 
  is consumed by [accessibility computation](#accessibility) 
  (if not [loaded from file](#accFromFile)). 
  So far, due to some thread-safety issues with the 
  routines from the Vienna RNA package, the IntaRNA
  accessibility computation is done serially. This significantly reduces the
  multi-threading effect when running IntaRNA in the fast heuristic mode (`--mode=H`).
  If you run a non-heuristic prediction mode, multi-threading will show a more
  dramatic decrease in runtime performance, since here the interaction prediction
  is the computationally more demanding step.
- The memory consumption will be much higher, since each thread runs an independent
  prediction (with according memory consumption). Thus, ensure you have enough
  RAM available when using many threads of memory-demanding 
  [prediction modes](#predModes).
 
The support for multi-threading can be completely disabled before compilation
using `configure --disable-multithreading`.
