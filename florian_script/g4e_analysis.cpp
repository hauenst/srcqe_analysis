void g4e_analysis (TString inputstring)
{
  gStyle->SetOptStat(0);
  TFile *inputfile = new TFile(inputstring,"READ");
  TTree *evt_tree = (TTree*)inputfile->Get("events");

  int numberofevents = evt_tree->GetEntriesFast();
  cout << "Number of events " << numberofevents << endl;

  //Integer for counting event topologies
  int pp_generated = 0;
  int nn_generated = 0;
  int np_generated = 0; //leading neutron and recoil proton
  int pn_generated = 0; //leading proton and recoil neutron
  int p_lead_generated = 0; //leading protons
  int n_lead_generated = 0; //leading neutrons
  int p_reco_generated = 0; //recoil protons
  int n_reco_generated = 0; //recoil neutrons
  int pp_tracks = 0; //leading proton and recoil proton tracks in event
  int nn_tracks = 0; //leading neutron and recoil neutron tracks in event
  int np_tracks = 0; //leading neutron and recoil proton tracks in event
  int pn_tracks = 0; //leading proton and recoil neutron tracks in event
  int p_lead_tracks = 0; //leading protons output track
  int n_lead_tracks = 0; //leading neutrons output track
  int p_reco_tracks = 0; //recoil protons output track
  int n_reco_tracks = 0; //recoil neutrons output track

  ULong64_t hit_count; //max usually 55000
  ULong64_t gen_prt_count; //max usually 12
  ULong64_t trk_count; //max usually 17
  std::vector<double> *hit_x = 0;
  std::vector<double> *hit_y = 0;
  std::vector<double> *hit_z = 0;
  std::vector<double> *hit_id = 0;
  std::vector<double> *hit_trk_id = 0;
  //add parent track id: is 0 if from event generation
  std::vector<string> *hit_vol_name = 0;

  std::vector<double> *gen_prt_id = 0;
  std::vector<double> *gen_prt_pdg = 0;
  std::vector<double> *gen_prt_charge = 0;
  std::vector<double> *gen_prt_trk_id = 0;

  std::vector<double> *trk_id = 0;
  std::vector<double> *trk_pdg = 0;

  evt_tree->SetBranchAddress("trk_count",&trk_count);

  evt_tree->SetBranchAddress("gen_prt_count",&gen_prt_count);
  evt_tree->SetBranchAddress("gen_prt_id",&gen_prt_id);
  evt_tree->SetBranchAddress("gen_prt_pdg",&gen_prt_pdg);
  evt_tree->SetBranchAddress("gen_prt_charge",&gen_prt_charge);
  evt_tree->SetBranchAddress("gen_prt_trk_id",&gen_prt_trk_id);

  evt_tree->SetBranchAddress("hit_count",&hit_count);
  evt_tree->SetBranchAddress("hit_x",&hit_x);
  evt_tree->SetBranchAddress("hit_y",&hit_y);
  evt_tree->SetBranchAddress("hit_z",&hit_z);
  evt_tree->SetBranchAddress("hit_id",&hit_id);
  evt_tree->SetBranchAddress("hit_trk_id",&hit_trk_id);
  evt_tree->SetBranchAddress("hit_vol_name",&hit_vol_name);

  evt_tree->SetBranchAddress("trk_id",&trk_id);
  evt_tree->SetBranchAddress("trk_pdg",&trk_pdg);

  TH1F *h1_hits_count = new TH1F("h1_hits_count","",60,0,600);
  TH1F *h1_gen_count = new TH1F("h1_gen_count","",20,0,20);
  TH1F *h1_trk_count = new TH1F("h1_trk_count","",20,0,20);

//ffi_RPOT_D2_lay
//fi_D1_TRK_
//ffi_ZDC_
  TH2F *h2_x_y_ZDC_np_lead = new TH2F("h2_x_y_ZDC_np_lead"," ",100,600,1100,100,0,100);
  TH2F *h2_x_y_ZDC_np_reco = new TH2F("h2_x_y_ZDC_np_reco"," ",100,600,1100,100,0,100);
  TH2F *h2_x_y_ZDC_pn_lead = new TH2F("h2_x_y_ZDC_pn_lead"," ",100,600,1100,100,0,100);
  TH2F *h2_x_y_ZDC_pn_reco = new TH2F("h2_x_y_ZDC_pn_reco"," ",100,600,1100,100,0,100);
  TH2F *h2_x_y_ZDC_nn_lead = new TH2F("h2_x_y_ZDC_nn_lead"," ",100,600,1100,100,0,100);
  TH2F *h2_x_y_ZDC_nn_reco = new TH2F("h2_x_y_ZDC_nn_reco"," ",100,600,1100,100,0,100);
  TH2F *h2_x_y_ZDC_pp_lead = new TH2F("h2_x_y_ZDC_pp_lead"," ",100,600,1100,100,0,100);
  TH2F *h2_x_y_ZDC_pp_reco = new TH2F("h2_x_y_ZDC_pp_reco"," ",100,600,1100,100,0,100);
  TH2F *h2_x_y_ZDC_lead_n = new TH2F("h2_x_y_ZDC_lead_n"," ",100,600,1100,100,0,100);
  TH2F *h2_x_y_ZDC_lead_p = new TH2F("h2_x_y_ZDC_lead_p"," ",100,600,1100,100,0,100);
  TH2F *h2_x_y_ZDC_reco_n = new TH2F("h2_x_y_ZDC_reco_n"," ",100,600,1100,100,0,100);
  TH2F *h2_x_y_ZDC_reco_p = new TH2F("h2_x_y_ZDC_reco_p"," ",100,600,1100,100,0,100);

  TH2F *h2_x_y_Roman1_np_lead = new TH2F("h2_x_y_Roman1_np_lead","",100,600,1100,80,-40,40);
  TH2F *h2_x_y_Roman1_np_reco = new TH2F("h2_x_y_Roman1_np_reco","",100,600,1100,80,-40,40);
  TH2F *h2_x_y_Roman1_pn_lead = new TH2F("h2_x_y_Roman1_pn_lead","",100,600,1100,80,-40,40);
  TH2F *h2_x_y_Roman1_pn_reco = new TH2F("h2_x_y_Roman1_pn_reco","",100,600,1100,80,-40,40);
  TH2F *h2_x_y_Roman1_nn_lead = new TH2F("h2_x_y_Roman1_nn_lead","",100,600,1100,80,-40,40);
  TH2F *h2_x_y_Roman1_nn_reco = new TH2F("h2_x_y_Roman1_nn_reco","",100,600,1100,80,-40,40);
  TH2F *h2_x_y_Roman1_pp_lead = new TH2F("h2_x_y_Roman1_pp_lead","",100,600,1100,80,-40,40);
  TH2F *h2_x_y_Roman1_pp_reco = new TH2F("h2_x_y_Roman1_pp_reco","",100,600,1100,80,-40,40);
  TH2F *h2_x_y_Roman1_lead_p = new TH2F("h2_x_y_Roman1_lead_p","",100,600,1100,80,-40,40);
  TH2F *h2_x_y_Roman1_lead_n = new TH2F("h2_x_y_Roman1_lead_n","",100,600,1100,80,-40,40);
  TH2F *h2_x_y_Roman1_reco_p = new TH2F("h2_x_y_Roman1_reco_p","",100,600,1100,80,-40,40);
  TH2F *h2_x_y_Roman1_reco_n = new TH2F("h2_x_y_Roman1_reco_n","",100,600,1100,80,-40,40);

  TH2F *h2_x_y_Roman2_np = new TH2F("h2_x_y_Roman2_np","",100,600,1100,80,-40,40);
  TH2F *h2_x_y_Roman2_pn = new TH2F("h2_x_y_Roman2_pn","",100,600,1100,80,-40,40);
  TH2F *h2_x_y_Roman3_np = new TH2F("h2_x_y_Roman3_np","",100,600,1100,80,-40,40);
  TH2F *h2_x_y_Roman3_pn = new TH2F("h2_x_y_Roman3_pn","",100,600,1100,80,-40,40);
  TH2F *h2_x_y_Roman4_np = new TH2F("h2_x_y_Roman4_np","",100,600,1100,80,-40,40);
  TH2F *h2_x_y_Roman4_pn = new TH2F("h2_x_y_Roman4_pn","",100,600,1100,80,-40,40);

  //TH1F *h1_gen_pdg_one = new TH1F("h1_gen_pdg_one","",20,0,20);
  for (int i = 0; i < numberofevents; i++) {
//  for (int i = 0; i < 100; i++) {
    evt_tree->GetEntry(i);
  //  if (i < 1000 && hit_vol_name->at(2).rfind("ffi_RPOT_D2_lay",0) == 0 ) {
  //    cout << "hit_vol_name " << hit_vol_name->at(2) << endl;
  // }

/*   for (int ihit = 0 ; ihit < hit_count ; ihit++) {
      if (hit_vol_name->at(ihit).rfind("ffi_ZDC_",0) == 0 ) {
        cout << "hit vol name " << hit_vol_name->at(ihit) << " with track ID " <<  hit_trk_id->at(ihit) <<  endl;
      }
      if (hit_vol_name->at(ihit).rfind("fi_D1_TRK_",0) == 0 ) {
        cout << "hit vol name " << hit_vol_name->at(ihit) << " with track ID " <<  hit_trk_id->at(ihit) <<  endl;
      }
      if (hit_vol_name->at(ihit).rfind("ffi_RPOT_D2_lay",0) == 0 ) {
        cout << "hit vol name " << hit_vol_name->at(ihit) << " with track ID " <<  hit_trk_id->at(ihit) << " track OPDG"  <<  endl;
      }
   }

*/
    h1_hits_count->Fill(hit_count);
    h1_gen_count->Fill(gen_prt_count);
    h1_trk_count->Fill(trk_count);
    //cout << "Event " << i << endl;
    //GenID 0 is electron, corresponding track ID is 1
    //GenID 1 is leading Nucleon, corresponding track ID is 2
    //GenID 2 is recoil nucleon, corresponding track ID is 3
/*    if (gen_prt_pdg->at(1) == 2112) { //leading is neutron
      cout << "Leading Neutron: 2112" << endl;
      cout << gen_prt_id->at(1) << " ID and trk ID " << gen_prt_trk_id->at(1) << endl;
    //  for (int it = 0; it < trk_count; it++) {
    //    cout << "track ID " << trk_id->at(it) << " with PID " << trk_pdg->at(it) << endl;
  //    }
  //    cout << "GEN PID " << gen_prt_pdg->at( gen_prt_id->at(1)) << " , Track PDG " << trk_pdg->at(gen_prt_trk_id->at(1)) << endl;
    }
    if (gen_prt_pdg->at(1) == 2212) { //leading is proton
      cout << "Leading Proton: 2212" << endl;
      cout << gen_prt_id->at(1) << " ID and trk ID " << gen_prt_trk_id->at(1) << endl;
  //    for (int it = 0; it < trk_count; it++) {
  //      cout << "track ID " << trk_id->at(it) << " with PID " << trk_pdg->at(it) << endl;
  //    }
  //    cout << "GEN PID " << gen_prt_pdg->at(gen_prt_id->at(1)) << " , Track PDG " << trk_pdg->at(gen_prt_trk_id->at(1)) << endl;
    }
    if (gen_prt_pdg->at(2) == 2112) { //recoil is neutron
      cout << "Recoil Neutron: 2112" << endl;
      cout << gen_prt_id->at(2) << " ID and trk ID " << gen_prt_trk_id->at(2) << endl;
  //    for (int it = 0; it < trk_count; it++) {
  //      cout << "track ID " << trk_id->at(it) << " with PID " << trk_pdg->at(it) << endl;
  //    }
  //    cout << "GEN PID " << gen_prt_pdg->at(gen_prt_id->at(2)) << " , Track PDG " << trk_pdg->at(gen_prt_trk_id->at(2)) << endl;
    }
    if (gen_prt_pdg->at(2) == 2212) { //recoil is proton
      cout << "Recoil Proton: 2212" << endl;
      cout << gen_prt_id->at(2) << " ID and trk ID " << gen_prt_trk_id->at(2) << endl;
  //    for (int it = 0; it < trk_count; it++) {
  //      cout << "track ID " << trk_id->at(it) << " with PID " << trk_pdg->at(it) << endl;
  //    }
    }
*/
    //Counting of pairs pp,np,pn,nn and single recoil/leading nucleons and how many are also in tracks

    bool lead_track_exists = false;
    bool recoil_track_exists = false;
    bool nn_track_exists = false;
    bool pp_track_exists = false;
    bool pn_track_exists = false;
    bool np_track_exists = false;

    //Identifying event topologies and check if leading and recoil track ID does exists (meaning particles produced hits). Also compares PID values from Track and Generated value
    //Sets boolean values for the later loop over hits.
    if (gen_prt_pdg->at(1) == 2112) { //leading is neutron
       n_lead_generated++;
       int tmp_track_recoil_pdg = 0;
       for (int it = 0; it < trk_count; it++) {
         if (trk_id->at(it) == 2 && trk_id->at(it) == gen_prt_trk_id->at(1)) { //check if leading track id exists and corresponds to track id assigned to generated particle
      //     cout << "found leading neutron track id. Particle is " << trk_pdg->at(it) << endl;
           lead_track_exists = true;
         }
         if (trk_id->at(it) == 3 && trk_id->at(it) == gen_prt_trk_id->at(2) ) {
      //     cout << "found recoil nucleon track id"  << endl;
           recoil_track_exists = true;
      //     cout << "Recoil nucleon is " << trk_pdg->at(it) << " (track) or " << gen_prt_id->at(2) << " from generated value " << endl;
           tmp_track_recoil_pdg = trk_pdg->at(it);
         }
       }
       if (lead_track_exists == true) { n_lead_tracks++; }

       if (gen_prt_pdg->at(2) == 2112) { //recoil is neutron
         //nn events
         nn_generated++;
         n_reco_generated++;
         if (recoil_track_exists == true) { //indepdently if leading nucleon is also found in tracks
           if (tmp_track_recoil_pdg != gen_prt_pdg->at(2) ) {
             cout << "Event: " << i << " Mismatch track recoil PDG " << tmp_track_recoil_pdg << " and Generated PDG " << gen_prt_pdg->at(2) << endl;
             continue;
           }
           n_reco_tracks++;
           if (lead_track_exists == true) { //found both tracks of the nn pair
              nn_tracks++;
              nn_track_exists = true;
           }
         }
       }
       if (gen_prt_pdg->at(2) == 2212) { //recoil is proton
         //np events
         np_generated++;
         p_reco_generated++;
         if (recoil_track_exists == true) { //indepdently if leading nucleon is also found in tracks
           if (tmp_track_recoil_pdg != gen_prt_pdg->at(2) ) {
             cout << "Event: " << i << " Mismatch track recoil PDG " << tmp_track_recoil_pdg << " and Generated PDG " << gen_prt_pdg->at(2) << endl;
             continue;
           }
           p_reco_tracks++;
           if (lead_track_exists == true) { //found both tracks of the np pair
              np_tracks++;
              np_track_exists = true;
           }
         }
       }
    }
    if (gen_prt_pdg->at(1) == 2212) { //leading is proton
       p_lead_generated++;
       int tmp_track_recoil_pdg = 0;
       for (int it = 0; it < trk_count; it++) {
         if (trk_id->at(it) == 2 && trk_id->at(it) == gen_prt_trk_id->at(1)) { //check if leading track id exists and corresponds to track id assigned to generated particle
      //     cout << "found leading proton track id. Particle is " << trk_pdg->at(it) << endl;
           lead_track_exists = true;
         }
         if (trk_id->at(it) == 3 && trk_id->at(it) == gen_prt_trk_id->at(2) ) {
      //     cout << "found recoil nucleon track id"  << endl;
           recoil_track_exists = true;
      //     cout << "Recoil nucleon is " << trk_pdg->at(it) << " (track) or " << gen_prt_id->at(2) << " from generated value " << endl;
           tmp_track_recoil_pdg = trk_pdg->at(it);
         }
       }
       if (lead_track_exists == true) { p_lead_tracks++; }

       if (gen_prt_pdg->at(2) == 2112) { //recoil is neutron
         //pn events
         pn_generated++;
         n_reco_generated++;
         if (recoil_track_exists == true) { //indepdently if leading nucleon is also found in tracks
           if (tmp_track_recoil_pdg != gen_prt_pdg->at(2) ) {
             cout << "Event: " << i << " Mismatch track recoil PDG " << tmp_track_recoil_pdg << " and Generated PDG " << gen_prt_pdg->at(2) << endl;
             continue;
           }
           n_reco_tracks++;
           if (lead_track_exists == true) { //found both tracks of the pn pair
              pn_tracks++;
              pn_track_exists = true;
           }
         }
       }
       if (gen_prt_pdg->at(2) == 2212) { //recoil is proton
         //pp events
         pp_generated++;
         p_reco_generated++;
         if (recoil_track_exists == true) { //indepdently if leading nucleon is also found in tracks
           if (tmp_track_recoil_pdg != gen_prt_pdg->at(2) ) {
             cout << "Event: " << i << " Mismatch track recoil PDG " << tmp_track_recoil_pdg << " and Generated PDG " << gen_prt_pdg->at(2) << endl;
             continue;
           }
           p_reco_tracks++;
           if (lead_track_exists == true) { //found both tracks of the pp pair
              pp_tracks++;
              pp_track_exists = true;
           }
         }
       }
    } //end if of checking for event topologies and existing tracks

    for (int ihit = 0 ; ihit < hit_count ; ihit++) {
        if (hit_trk_id->at(ihit) == 2 && lead_track_exists) { //leading information
           if (hit_vol_name->at(ihit).rfind("ffi_ZDC_",0) == 0  ) { //ZDC hits
             if (trk_pdg->at(hit_trk_id->at(ihit)) == 2112) h2_x_y_ZDC_lead_n->Fill(hit_x->at(ihit),hit_y->at(ihit));
             if (trk_pdg->at(hit_trk_id->at(ihit)) == 2212) h2_x_y_ZDC_lead_p->Fill(hit_x->at(ihit),hit_y->at(ihit));
             if (np_track_exists == true)  h2_x_y_ZDC_np_lead->Fill(hit_x->at(ihit),hit_y->at(ihit));
             if (pp_track_exists == true)  h2_x_y_ZDC_pp_lead->Fill(hit_x->at(ihit),hit_y->at(ihit));
             if (pn_track_exists == true) {
                h2_x_y_ZDC_pn_lead->Fill(hit_x->at(ihit),hit_y->at(ihit));
                 cout << "ZDC x y " << hit_x->at(ihit)<< "  ,  " << hit_y->at(ihit)<< endl;
             }
             if (nn_track_exists == true)  h2_x_y_ZDC_nn_lead->Fill(hit_x->at(ihit),hit_y->at(ihit));
           }
           if (hit_vol_name->at(ihit).rfind("ffi_RPOT_D2_lay_Phys_",0) == 0 ) { //Roman Pot 1
             if (trk_pdg->at(hit_trk_id->at(ihit)) == 2112) h2_x_y_Roman1_lead_n->Fill(hit_x->at(ihit),hit_y->at(ihit));
             if (trk_pdg->at(hit_trk_id->at(ihit)) == 2212) h2_x_y_Roman1_lead_p->Fill(hit_x->at(ihit),hit_y->at(ihit));
             if (np_track_exists == true)  h2_x_y_Roman1_np_lead->Fill(hit_x->at(ihit),hit_y->at(ihit));
             if (pp_track_exists == true)  h2_x_y_Roman1_pp_lead->Fill(hit_x->at(ihit),hit_y->at(ihit));
             if (pn_track_exists == true)  {
                h2_x_y_Roman1_pn_lead->Fill(hit_x->at(ihit),hit_y->at(ihit));
                 cout << "Roman x y " << hit_x->at(ihit)<< "  ,  " << hit_y->at(ihit)<< endl;
             }
             if (nn_track_exists == true)  h2_x_y_Roman1_nn_lead->Fill(hit_x->at(ihit),hit_y->at(ihit));
           }
           if (hit_vol_name->at(ihit).rfind("ffi_RPOT_D2_lay_Phys_1",0) == 0 ) { //Roman Pot 2

           }
           if (hit_vol_name->at(ihit).rfind("ffi_RPOT_D2_lay_Phys_2",0) == 0 ) { //Roman Pot 3

           }
           if (hit_vol_name->at(ihit).rfind("ffi_RPOT_D2_lay_Phys_3",0) == 0 ) { //Roman Pot 4

           }
        }
        if (hit_trk_id->at(ihit) == 3 && recoil_track_exists) { //recoil information
           if (hit_vol_name->at(ihit).rfind("ffi_ZDC_",0) == 0  ) { //ZDC hits
             if (trk_pdg->at(hit_trk_id->at(ihit)) == 2112) h2_x_y_ZDC_reco_n->Fill(hit_x->at(ihit),hit_y->at(ihit));
             if (trk_pdg->at(hit_trk_id->at(ihit)) == 2212) h2_x_y_ZDC_reco_p->Fill(hit_x->at(ihit),hit_y->at(ihit));
             if (np_track_exists == true)  h2_x_y_ZDC_np_reco->Fill(hit_x->at(ihit),hit_y->at(ihit));
             if (pp_track_exists == true)  h2_x_y_ZDC_pp_reco->Fill(hit_x->at(ihit),hit_y->at(ihit));
             if (pn_track_exists == true)  {
               h2_x_y_ZDC_pn_reco->Fill(hit_x->at(ihit),hit_y->at(ihit));
              cout << "ZDC x y " << hit_x->at(ihit)<< "  ,  " << hit_y->at(ihit)<< endl;
             }
             if (nn_track_exists == true)  h2_x_y_ZDC_nn_reco->Fill(hit_x->at(ihit),hit_y->at(ihit));
           }
           if (hit_vol_name->at(ihit).rfind("ffi_RPOT_D2_lay_Phys_",0) == 0 ) { //Roman Pot 1
             if (trk_pdg->at(hit_trk_id->at(ihit)) == 2112) h2_x_y_Roman1_reco_n->Fill(hit_x->at(ihit),hit_y->at(ihit));
             if (trk_pdg->at(hit_trk_id->at(ihit)) == 2212) h2_x_y_Roman1_reco_p->Fill(hit_x->at(ihit),hit_y->at(ihit));
             if (np_track_exists == true)  h2_x_y_Roman1_np_reco->Fill(hit_x->at(ihit),hit_y->at(ihit));
             if (pp_track_exists == true)  h2_x_y_Roman1_pp_reco->Fill(hit_x->at(ihit),hit_y->at(ihit));
             if (pn_track_exists == true)  {
               h2_x_y_Roman1_pn_reco->Fill(hit_x->at(ihit),hit_y->at(ihit));
                 cout << "Roman x y " << hit_x->at(ihit)<< "  ,  " << hit_y->at(ihit)<< endl;
             }
             if (nn_track_exists == true)  h2_x_y_Roman1_nn_reco->Fill(hit_x->at(ihit),hit_y->at(ihit));
           }
        }
    }
    /*  if (gen_prt_pdg->at(1) == 2112 && gen_prt_pdg->at(2) == 2212) { //leading is neutron and recoil  is proton
      //  cout << " FOund np event" << endl;
         for (int ihit = 0 ; ihit < hit_count ; ihit++) {
           if (hit_vol_name->at(ihit).rfind("ffi_ZDC_",0) == 0 ) {
             h2_x_y_ZDC_np->Fill(hit_x->at(ihit),hit_y->at(ihit));
        //     cout << "ZDC x y " << hit_x->at(ihit)<< "  ,  " << hit_y->at(ihit)<< endl;
           }
           if (hit_vol_name->at(ihit).rfind("ffi_RPOT_D2_lay_Phys_0",0) == 0 ) {
             h2_x_y_Roman1_np->Fill(hit_x->at(ihit),hit_y->at(ihit));
        //      cout << "Roman1 x y " << hit_x->at(ihit)<< "  ,  " << hit_y->at(ihit)<< endl;
           }
           if (hit_vol_name->at(ihit).rfind("ffi_RPOT_D2_lay_Phys_1",0) == 0 ) {
             h2_x_y_Roman2_np->Fill(hit_x->at(ihit),hit_y->at(ihit));
        //         cout << "Roman2 x y " << hit_x->at(ihit)<< "  ,  " << hit_y->at(ihit)<< endl;
           }
           if (hit_vol_name->at(ihit).rfind("ffi_RPOT_D2_lay_Phys_2",0) == 0 ) {
             h2_x_y_Roman3_np->Fill(hit_x->at(ihit),hit_y->at(ihit));
        //        cout << "Roman3 x y " << hit_x->at(ihit)<< "  ,  " << hit_y->at(ihit)<< endl;
           }
           if (hit_vol_name->at(ihit).rfind("ffi_RPOT_D2_lay_Phys_3",0) == 0 ) {
             h2_x_y_Roman4_np->Fill(hit_x->at(ihit),hit_y->at(ihit));
        //        cout << "Roman4 x y " << hit_x->at(ihit)<< "  ,  " << hit_y->at(ihit)<< endl;
           }
         }
      }
      if (gen_prt_pdg->at(1) == 2212 && gen_prt_pdg->at(2) == 2112) { //leading is proton and recoil  is neutron
        //  cout << " FOund pn event" << endl;
        for (int ihit = 0 ; ihit < hit_count ; ihit++) {
          if (hit_vol_name->at(ihit).rfind("ffi_ZDC_",0) == 0 ) {
            h2_x_y_ZDC_pn->Fill(hit_x->at(ihit),hit_y->at(ihit));
      //       cout << "ZDC x y " << hit_x->at(ihit)<< "  ,  " << hit_y->at(ihit)<< endl;
          }
          if (hit_vol_name->at(ihit).rfind("ffi_RPOT_D2_lay_Phys_0",0) == 0 ) {
            h2_x_y_Roman1_pn->Fill(hit_x->at(ihit),hit_y->at(ihit));
      //        cout << "Roman1 x y " << hit_x->at(ihit)<< "  ,  " << hit_y->at(ihit)<< endl;
          }
          if (hit_vol_name->at(ihit).rfind("ffi_RPOT_D2_lay_Phys_1",0) == 0 ) {
            h2_x_y_Roman2_pn->Fill(hit_x->at(ihit),hit_y->at(ihit));
      //     cout << "Roman2 x y " << hit_x->at(ihit)<< "  ,  " << hit_y->at(ihit)<< endl;
          }
          if (hit_vol_name->at(ihit).rfind("ffi_RPOT_D2_lay_Phys_2",0) == 0 ) {
            h2_x_y_Roman3_pn->Fill(hit_x->at(ihit),hit_y->at(ihit));
      //     cout << "Roman3 x y " << hit_x->at(ihit)<< "  ,  " << hit_y->at(ihit)<< endl;
          }
          if (hit_vol_name->at(ihit).rfind("ffi_RPOT_D2_lay_Phys_3",0) == 0 ) {
            h2_x_y_Roman4_pn->Fill(hit_x->at(ihit),hit_y->at(ihit));
      //       cout << "Roman4 x y " << hit_x->at(ihit)<< "  ,  " << hit_y->at(ihit)<< endl;
          }
        }
      }

    */

//    cout << "Next Event " << endl;
  }
//Ratios
  double pp_ratio = pp_tracks / (double)pp_generated;
  double pn_ratio = pn_tracks / (double)pn_generated;
  double np_ratio = np_tracks / (double)np_generated;
  double nn_ratio = nn_tracks / (double)nn_generated;

  double p_lead_ratio = p_lead_tracks / (double)p_lead_generated;
  double n_lead_ratio = n_lead_tracks / (double)n_lead_generated;
  double p_reco_ratio = p_reco_tracks / (double)p_reco_generated;
  double n_reco_ratio = n_reco_tracks / (double)n_reco_generated;


  cout << " Generated pp events " << pp_generated << " and #events with pp tracks " << pp_tracks << " = " << pp_ratio*100 << endl;
  cout << " Generated pn events " << pn_generated << " and #events with pn tracks " << pn_tracks << " = " << pn_ratio*100 << endl;
  cout << " Generated nn events " << nn_generated << " and #events with nn tracks " << nn_tracks << " = " << nn_ratio*100 << endl;
  cout << " Generated np events " << np_generated << " and #events with np tracks " << np_tracks << " = " << np_ratio*100 << endl;
  cout << " Generated Leading protons "  << p_lead_generated;
  cout << " and #events with lead p tracks " << p_lead_tracks << " = " << p_lead_ratio*100 << endl;
  cout << " Generated Leading neutrons " << n_lead_generated;
  cout << " and #events with lead n tracks " << n_lead_tracks << " = " << n_lead_ratio*100 << endl;
  cout << " Generated Recoil protons "   << p_reco_generated;
  cout << " and #events with recoil p tracks " << p_reco_tracks << " = " << p_reco_ratio*100 << endl;
  cout << " Generated Recoil neutrons "  << n_reco_generated;
  cout << " and #events with recoil n tracks " << n_reco_tracks << " = " << n_reco_ratio*100 << endl;

  TCanvas *c1 = new TCanvas("c1","Number");
  c1->Divide(2,2);
  c1->cd(1);
  h1_hits_count->Draw();
  c1->cd(2);
  h1_gen_count->Draw();
  c1->cd(3);
  h1_trk_count->Draw();
  c1->cd(4);

//  c1->SaveAs("Numberhist.root");
  TCanvas *lead_canv = new TCanvas("lead_canv","all leading ");
  lead_canv->Divide(2,2);
  lead_canv->cd(1);
  h2_x_y_ZDC_lead_n->Draw("COLZ");
  lead_canv->cd(2);
  h2_x_y_ZDC_lead_p->Draw("COLZ");
  lead_canv->cd(3);
  h2_x_y_Roman1_lead_n->Draw("COLZ");
  lead_canv->cd(4);
  h2_x_y_Roman1_lead_p->Draw("COLZ");

  lead_canv->SaveAs("Leading_hist.root");

  TCanvas *reco_canv = new TCanvas("reco_canv","all recoil ");
  reco_canv->Divide(2,2);
  reco_canv->cd(1);
  h2_x_y_ZDC_reco_n->Draw("COLZ");
  reco_canv->cd(2);
  h2_x_y_ZDC_reco_p->Draw("COLZ");
  reco_canv->cd(3);
  h2_x_y_Roman1_reco_n->Draw("COLZ");
  reco_canv->cd(4);
  h2_x_y_Roman1_reco_p->Draw("COLZ");

  reco_canv->SaveAs("Recoil_hist.root");

  TCanvas *c2 = new TCanvas("c2","np events ");
  c2->Divide(2,2);
  c2->cd(1);
  h2_x_y_ZDC_np_lead->Draw("COLZ");
  c2->cd(2);
  h2_x_y_ZDC_np_reco->Draw("COLZ");
  c2->cd(3);
  h2_x_y_Roman1_np_lead->Draw("COLZ");
  c2->cd(4);
  h2_x_y_Roman1_np_reco->Draw("COLZ");
//    c2->SaveAs("np_hists.root");
  TCanvas *c3 = new TCanvas("c3","pn events ");
  c3->Divide(2,2);
  c3->cd(1);
  h2_x_y_ZDC_pn_lead->Draw("COLZ");
  c3->cd(2);
  h2_x_y_ZDC_pn_reco->Draw("COLZ");
  c3->cd(3);
  h2_x_y_Roman1_pn_lead->Draw("COLZ");
  c3->cd(4);
  h2_x_y_Roman1_pn_reco->Draw("COLZ");
//  c3->SaveAs("pn_hists.root");
  TCanvas *c4 = new TCanvas("c4","pp events ");
  c4->Divide(2,2);
  c4->cd(1);
  h2_x_y_ZDC_pp_lead->Draw("COLZ");
  c4->cd(2);
  h2_x_y_ZDC_pp_reco->Draw("COLZ");
  c4->cd(3);
  h2_x_y_Roman1_pp_lead->Draw("COLZ");
  c4->cd(4);
  h2_x_y_Roman1_pp_reco->Draw("COLZ");
//  c4->SaveAs("pp_hists.root");
  TCanvas *c5 = new TCanvas("c5","nn events ");
  c5->Divide(2,2);
  c5->cd(1);
  h2_x_y_ZDC_nn_lead->Draw("COLZ");
  c5->cd(2);
  h2_x_y_ZDC_nn_reco->Draw("COLZ");
  c5->cd(3);
  h2_x_y_Roman1_nn_lead->Draw("COLZ");
  c5->cd(4);
  h2_x_y_Roman1_nn_reco->Draw("COLZ");
//  c5->SaveAs("nn_hists.root");
}
