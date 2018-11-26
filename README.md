**** U2boot *****
License LGPL 3
## Unigen2urqmdBOOsTer
This program use data from unigen-like tree and pass it to UrQMD code and/or decay long living particles by using
THERMINATOR 2 decay tables (see https://therminator2.ifj.edu.pl/).
## 1. Prerequisites
ROOT >5.34 + cmake 
To use all features of this program ou will need also:
- UrQMD 3.4 to call cascades (you can download UrQMD from: https://urqmd.org/)
- Unigen to convert files from various models into readable to U2boot tree (http://svn.cern.ch/guest/unigen/trunk/)

## 2. How to install
download code from git then
```bash
cd dowloaded_code
mkdir build
cd build 
cmake -DCMAKE_INSTALL_PREFIX=[installation path] -DURMQD_PATH:STRING=[path to urqmd model] ..
make 
make install
```
Default UrQMD code seems to start at fixed time.It's impossible to include advanced hypersurface there. Therefore to work with hypersurfaces you need to apply path. 
```bash
cd [urqmd path]
patch < [uboot code path]/coload.path
make
```
After this UrQMD should be rebuild with updates. NOTE: this probably make UrQMD useless in "stand alone" mode.
## 3. How to use
After compilation you should see file config.sh in build directory. This scripts sets enviromental variables.
Then you have to call 
```bash
U2boot inFile outFile optional_flags
```
following flags are supported:
-f clean temporary files (by default this code produce diretory with text files processed by UrQMD, usually they are not needed
after so yo can remove them)<br />
-s=N where you specify status of processed particles - other particles are not processed.<br />
-n=N to specify number of events to process (by default process all events)<br />
-decay - only decay particles, don't call UrQMD<br />
-no-decay - only call UrQMD, don't decay particles<br />
-feeddown - try to decay particles that are not suppored by UrQMD to suppored decay products (note: all decay products are created in postion of creation of parent particle)
UrQMD start at given time. Without patch from uboot all particles positions are interpolated to given time. Then UrQMD starts. If you apply patch particles should not interact until "formation time". <br />
There are few ways to define this time<br />
-t=min - start cascades when first particle is created (recommended, default)<br />
-t=fmXX - star with fixed time (XX in fm/c) <br />
-t=av = - start with time equal to average freezout time<br />
-t=max - start with time creation of last particle<br />
UrQMD time calcualtion times:<br/>
-urqmd_out=XX - where XX is output time (in UrQMD input: tim[1]=outtime)<br />
-urqmd_calc==XX - where XX is calculation time (in UrQMD input: tim[0]=tottime)<br />
-urqmd_dt==XX - where XX is step time (in UrQMD input: cdt)<br />
Output tree there has 4 types of particle status:<br />
0 - for "bad" particles (with PDG that is not supported in UrQMD)<br />
1 - for particles that passed UrQMD cascades and didn't interact<br />
2 - for other particles from UrQMD<br />
3 - for particles that comes from weak decays<br />
Those values are defined in UItyp.h file.<br/>
In UDecayParticle.h by commenting USE_BREIT_WIGNER you can disable using breit wigner procedure, resonance peaks become sharp.
