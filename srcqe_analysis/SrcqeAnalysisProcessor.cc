#include "TLorentzVector.h"

#include <fmt/core.h>

#include <JANA/JEvent.h>
#include <dis/functions.h>
#include <MinimalistModel/McFluxHit.h>
#include <MinimalistModel/McGeneratedParticle.h>
#include <MinimalistModel/McTrack.h>

#include "SrcqeAnalysisProcessor.h"

#include <TClingRuntime.h>
#include <fmt/format.h>     // For format and print functions
#include "Math/Vector4D.h"
#include <ejana/EServicePool.h>
#include <ejana/plugins/io/lund_reader/LundEventData.h>
#include <ejana/EStringHelpers.h>


// TODO repalce by some 'conventional' PDG info provider. Like TDatabasePDG
//==================================================================
Double_t MassPI = 0.139570,
        MassK = 0.493677,
        MassProton = 0.93827,
        MassNeutron = 0.939565,
        MassE = 0.000511,
        MassMU = 0.105658;
//==================================================================


using namespace std;
using namespace fmt;


struct MesonStructureRecord {
    double Q2_true;
    double Q2_smeared;
    double Q2_trkng;
    double Xel, Yel, Q2el;
    // double Xtrue,Ytrue, Q2true;
    double Xgen, Ygen, Q2gen;
    double Eth;
    double Ee;
    double p;
    double pt;
};

static MesonStructureRecord vmrec;


void SrcqeAnalysisProcessor::Init() {
    ///  Called once at program start.
    print("SrcqeAnalysisProcessor::Init()\n");

    // initialize class that holds histograms and other root objects
    // 'services' is a service locator, we ask it for TFile
    root_out.init(services.Get<TFile>());

    //

    // Ask service locator for parameter manager. We want to get this plugin parameters.
    auto pm = services.Get<JParameterManager>();

    // Verbosity level. 0 = show nothing. 1 - show some. 2 - mad printer
    // SetDefaultParameter actually sets the parameter value from arguments if it is specified
    verbose = 0;
    pm->SetDefaultParameter("srcqe_analysis:verbose", verbose,
                            "Plugin output level. 0-almost nothing, 1-some, 2-everything");

    if (verbose) {
        print("Parameters:\n");
        print("  srcqe_analysis:verbose: {0}\n", verbose);
    }
}


void SrcqeAnalysisProcessor::Process(const std::shared_ptr<const JEvent> &event) {
    ///< Called every event.
    using namespace fmt;
    using namespace std;
    using namespace minimodel;
    std::lock_guard<std::recursive_mutex> locker(root_out.lock);

    auto hits = event->Get<minimodel::McFluxHit>();
    std::unordered_set<uint64_t> track_ids_in_d2;

    // Loop over hits
    for (auto hit: hits) {

        // Look at hits at D2
        if (ej::StartsWith(hit->vol_name, "ffi_D2")) {

            // Save the track id for further track processing
            track_ids_in_d2.insert(hit->track_id);

            // Fill some histograms
            root_out.xy_hits_d2->Fill(hit->x, hit->y);
            root_out.z_hits_d2->Fill(hit->z);
        }
    }

    // Now loop over tracks
    auto tracks = event->Get<minimodel::McTrack>();
    for(auto track: tracks) {

        // Is track in D2?
        if(track_ids_in_d2.count(track->id)
            && track->parent_id == 0    // Take only particles from a generator
            // && track->pdg == ...     // Filter by particle type
            ) {
            TLorentzVector p_lv;
            p_lv.SetXYZM(track->px, track->py, track->pz, MassProton);

            // Fill histo?

            if (verbose >= 3) {
                print("srcqe_analysis: track {:<5} px: {:<11} py: {:<11} pz: {:<11} pt: {:<11}  ptot: {:<11}\n",
                      track->pdg, track->px, track->py, track->pz, p_lv.Pt(), p_lv.P());
            }
        }
    }
}


void SrcqeAnalysisProcessor::Finish() {
    ///< Called after last event of last event source has been processed.
    print("SrcqeAnalysisProcessor::Finish(). Cleanup\n");
}
