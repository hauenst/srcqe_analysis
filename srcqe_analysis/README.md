/data/yulia/Herwig6/herwig6_P19104_Q10_N5e+06_e-p_5x100.hepmc

## Clion configuration
Target: meson_structure
Executable: ejana

Program arguments:
```
-Pplugins=hepmc_reader,open_charm
-Popen_charm:verbose=2
-Popen_charm:e_beam_energy=5
-Popen_charm:ion_beam_energy=100
/data/yulia/Herwig6/herwig6_P19104_Q10_N5e+06_e-p_5x100.hepmc
```

Working directory:
```
/home/yulia/work/ejana2/ejana/dev/work
```

Environment variables:
```
JANA_PLUGIN_PATH=/home/yulia/work/ejana2/ejana/dev/cmake-build-debug
```

## Advanced plugin params:

```bash
-Pplugins=hepmc_reader,open_charm
-Popen_charm:verbose=2
-Popen_charm:smearing=1
-Popen_charm:e_beam_energy=5
-Popen_charm:ion_beam_energy=100
-Pjana:DEBUG_PLUGIN_LOADING=1
-Pnevents=5000
-CCCPnthreads=1
-PROOT:StartGui=1
/data/yulia/Herwig6/herwig6_P19104_Q10_N5e+06_e-p_5x100.hepmc
```