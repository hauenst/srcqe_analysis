# Make sure pyjano is installed in your system
# do:
#     pip install --upgrade pyjano          # for conda, venv or root install
#     pip install --user --upgrade pyjano   # for user local install
#
# One can download a sample file with 20k of events
# https://gitlab.com/eic/escalate/workspace/-/raw/master/data/beagle_eD.txt
#
from pyjano.jana import Jana, PluginFromSource

my_plugin = PluginFromSource('./srcqe_analysis')   # Name must correspond to one in CMakeFileLists.txt
# add name=<...> for custom name

jana = Jana(nevents=1000, output='OUTPUTS/anaout.root')
jana.plugin('g4e_reader') \
    .source('OUTPUTS/g4e_srcqe.root')

jana.plugin(my_plugin, verbose=1)

jana.run()
