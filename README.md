**** U2boot *****
License LGPL 3
## Unigen2urqmdBOOsTer
This program use data from unigen-like tree and pass it to UrQMD code and/or decay long living particles by using
THERMINATOR 2 decay tables (see https://therminator2.ifj.edu.pl/).
## 1. Prerequisites
ROOT >5.34 + cmake 
To use all features of this program ou will need also:
- UrQMD to call cascades (you can download UrQMD from: https://urqmd.org/)
- Unigen to convert files from various models into readable to U2boot tree (http://svn.cern.ch/guest/unigen/trunk/)

## 2. How to install
download code from git then
```bash
cd dowloaded_code
mkdir build
cd build 
cmake -DCMAKE_PREFIX_INSTALL="installation path" -DURMQD_PATH:STRING="path to urqmd model" ..
make 
make install
```
## 3. How to use
After compilation you should see file config.sh in build directory. This scripts sets enviromental variables.
Then you have to call 
```bash
U2boot inFile outFile optional_flags
```
following flags are supported:
-f clean temporary files (by default this code produce diretory with text files processed by UrQMD, usually they are not needed
after so yo can remove them)
-s=N where you specify status of processed particles - other particles are not processed.
-n=N to specify number of events to process (by default process all events)
-decay - only decay particles, don't call UrQMD
-no-decay - only call UrQMD, don't decay particles
-t=tau_flat. UrQMD must have "event's frame" - all particles should have the same "freezout" time. If particle was created before this time then trajectory of this particle is extrapolated (if later then interpolated). This is serious limiation of this model because interpolated/extrapolated particles don't interact during inter/extrapolation (all rescattering are done later when UrQMD is called). 
There are few ways to define this time
-t=min - start cascades when first particle is created 
-t=fmXX - star with fixed time (XX in fm/c) 
-t=av = - start with time equal to average freezout time
-t=max - start with time creation of last particle
Output tree there has 3 types of 
status:
0 - for bad particles (with PDG that is not supported in UrQMD)
1 - for particles that passed UrQMD cascades
2 - for particles that comes from weak decays
