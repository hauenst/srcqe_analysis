# Standalone plugin example

To run on ifarm

```bash
source /group/eic/escalate/env.sh
```

To run python example

```bash
# G4E
python3 run_g4e.py

# Analysis
python3 run_plugin.py
```

To run the plugin from command line:

```bash
ejana
-Pplugins=g4e_reader,srcqe_analysis
-Pnthreads=1
-Psrcqe_analysis:verbose=1
-Pnevents=1000
-Poutput=OUTPUTS/ana.root
-Pjana:debug_plugin_loading=1
OUTPUTS/g4e_srcqe.root
```

What files to change:

* The main file is 
   [srcqe_analysis/SrcqeAnalysisProcessor.cc](srcqe_analysis/SrcqeAnalysisProcessor.cc)
* For convenience all tons of historgrams are initialized and live in 
   [srcqe_analysis/SrcqeRootOutput.h](srcqe_analysis/SrcqeRootOutput.h)

SrcqeRootOutput.h