#ifndef SRCQE_ROOT_OUTPUT_HEADER
#define SRCQE_ROOT_OUTPUT_HEADER

#include <TFile.h>
#include <TDirectory.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TCanvas.h>
#include <TTree.h>
#include <TApplication.h>

class SrcqeRootOutput
{
public:
    void init(TFile *file)
    {
        // 'locker' locks mutex so other threads can't interfere with TFile doing its job
        std::lock_guard<std::recursive_mutex> locker(lock);

        // create a subdirectory "hist_dir" in this file
        plugin_root_dir = file->mkdir("srcqe_analysis");
        plugin_root_dir->cd();
        file->cd();             // Just in case some other root file is the main TDirectory now

        // TTree with recoiled electron
        tree_rec_e = new TTree("rec_e", "a Tree with vect");
        tree_rec_e->SetDirectory(plugin_root_dir);

        xy_hits_d2 = new TH2I("xy_hits_b0", "X,Y hits in D2", 100, 400, 750, 100, -200, 200);
        z_hits_d2 = new TH1I("z_hits_b0", "X hits in D2", 1000, 22000, 23000);
    }

    // ---- kinematic variables---

    TH2I  *xy_hits_d2;
    TH1I  *z_hits_d2;

    std::recursive_mutex lock;

    TTree * tree_rec_e;     // Tree to store electron related data

private:

    TDirectory* plugin_root_dir;   // Main TDirectory for Plugin histograms and data

};

#endif // OPEN_CHARM_ROOT_OUTPUT_HEADER