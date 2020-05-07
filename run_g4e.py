from g4epy import Geant4Eic

g4e = Geant4Eic(beamline='erhic')


# Set beam energies
g4e.command(['/detsetup/eBeam 18',
             '/detsetup/pBeam 135'
             ])

# To control how many generation of secondaries (tracks and their hits) to save,
# there is a configuration:
#    /rootOutput/saveSecondaryLevel <ancestry-level>
#
# <ancestry-level> sets 0-n levels of ancestry which are saved in root file.
#
# Example:
#
# -1 - save everything
# 0 - save only primary particles
# 1 - save primaries and their daughters
# 2 - save primaries, daughters and daughters’ daughters
# n - save n generations of secondaries
#
# (primaries - particles that came from a generator/input file)
#
# The default level is 3, which corresponds to:
#
# /rootOutput/saveSecondaryLevel 3
#
# We set it to 2. If only vertex particles are of the interest, set it to 0

g4e.command(['/rootOutput/saveSecondaryLevel 2'])

# Extension is ommited here
# g4e creates a bunch of files with this name and different extensions
g4e.output('OUTPUTS/g4e_srcqe')

# Run g4e run!!!
g4e.source('/home/romanov/eic/data/k_lambda_18on275_lund_new.dat') \
    .beam_on(1000) \
    .run()
