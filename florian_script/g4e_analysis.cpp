void g4e_analysis (TString inputstring, int targetA, int setting)
{
  gStyle->SetOptStat(0);
  TFile *inputfile = new TFile(inputstring,"READ");
  TTree *evt_tree = (TTree*)inputfile->Get("events");

  TFile *output = new TFile("output.root","RECREATE");

  int numberofevents = evt_tree->GetEntriesFast();
  numberofevents--;
  cout << "Number of events " << numberofevents << endl;

  double u_in_GeV = 0.9315; //u in GeV
  double mproton = 0.938; //GeV
  double ionmomentum = 0;
  double ionenergy = 0;

  if (setting == 1) { //275 GeV protons
  //  ionmomentum = 275./2 * targetA;
    ionenergy = 275 * targetA;
  }
  else if (setting == 2) { //100GeV proton setting
  //  ionmomentum = 100./2 * targetA;
    ionenergy = 100 * targetA;
  }
  else if (setting == 3) { //41 GeV proton setting
  //  ionmomentum = 41./2 * targetA;
    ionenergy = 41 * targetA;
  }
  else {
    cout << "Setting for Ions not implemented. Exit script" << endl;
    std::exit;
  }
  double mass_target = 0;
  double targetZ = 0;
  if (targetA == 12) {
    cout << "Assuming Carbon 12 running" << endl;
    mass_target = 12*u_in_GeV;
    targetZ = 6;
  }
  else if (targetA == 4) {
    cout << "Assuming He 4 running" << endl;
    mass_target = 4.002603*u_in_GeV;
    targetZ = 2;
  }
  else if (targetA == 2) {
    cout << "Assuming Deuterium running" << endl;
    mass_target = 2.014102*u_in_GeV;
    targetZ = 1;
  }
  else { cout << "Target A option not implemented. Exit program" << endl; exit;}

  //NEW version using ionenergy * A
  TLorentzVector target_coll(0,0,sqrt(ionenergy*ionenergy - mass_target*mass_target),ionenergy);
  TVector3 boost_irf = target_coll.BoostVector();

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
  std::vector<double> *gen_prt_dir_x = 0;
  std::vector<double> *gen_prt_dir_y = 0;
  std::vector<double> *gen_prt_dir_z = 0;
  std::vector<double> *gen_prt_tot_mom = 0;
  std::vector<double> *gen_prt_tot_e = 0;

  std::vector<double> *trk_id = 0;
  std::vector<double> *trk_pdg = 0;

  evt_tree->SetBranchAddress("trk_count",&trk_count);

  evt_tree->SetBranchAddress("gen_prt_count",&gen_prt_count);
  evt_tree->SetBranchAddress("gen_prt_id",&gen_prt_id);
  evt_tree->SetBranchAddress("gen_prt_pdg",&gen_prt_pdg);
  evt_tree->SetBranchAddress("gen_prt_charge",&gen_prt_charge);
  evt_tree->SetBranchAddress("gen_prt_trk_id",&gen_prt_trk_id);
  evt_tree->SetBranchAddress("gen_prt_dir_x",&gen_prt_dir_x);
  evt_tree->SetBranchAddress("gen_prt_dir_y",&gen_prt_dir_y);
  evt_tree->SetBranchAddress("gen_prt_dir_z",&gen_prt_dir_z);
  evt_tree->SetBranchAddress("gen_prt_tot_mom",&gen_prt_tot_mom);
  evt_tree->SetBranchAddress("gen_prt_tot_e",&gen_prt_tot_e);

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

  TH2F *h2_lead_mom_theta = new TH2F("h2_lead_mom_theta","Leading nucleon mom-theta dist. (no weight)",50,0,100,50,0,100);
  TH2F *h2_lead_mom_theta_accepted = new TH2F("h2_lead_mom_theta_accepted","Leading nucleon mom-theta dist. accepted (no weight)",50,0,100,50,0,100);
  TH2F *h2_reco_mom_theta = new TH2F("h2_reco_mom_theta","Recoil nucleon mom-theta dist. (no weight)",50,0,100,50,0,100);
  TH2F *h2_reco_mom_theta_accepted = new TH2F("h2_reco_mom_theta_accepted","Recoil nucleon mom-theta dist. accepted (no weight)",50,0,100,50,0,100);

   //Histo for pmiss acceptance
  TH1F *h1_recoil_generated = new TH1F("h1_recoil_generated","Pmiss generated recoil",200,0,2);
  TH1F *h1_recoil_n_generated = new TH1F("h1_recoil_n_generated","Pmiss generated neutron",200,0,2);
  TH1F *h1_recoil_p_generated = new TH1F("h1_recoil_p_generated","Pmiss generated proton",200,0,2);
  TH1F *h1_recoil_accepted = new TH1F("h1_recoil_accepted","Pmiss accepted recoil",200,0,2);
  TH1F *h1_recoil_n_accepted = new TH1F("h1_recoil_n_accepted","Pmiss accepted neutron",200,0,2);
  TH1F *h1_recoil_p_accepted = new TH1F("h1_recoil_p_accepted","Pmiss accepted proton",200,0,2);
//ffi_RPOT_D2_lay
//fi_D1_TRK_
//ffi_ZDC_
  TH2F *h2_x_y_ZDC_np_lead = new TH2F("h2_x_y_ZDC_np_lead","ZDC: n lead  (np) ",100,600,1100,100,0,100);
  TH2F *h2_x_y_ZDC_np_reco = new TH2F("h2_x_y_ZDC_np_reco","ZDC: p recoil (np)",100,600,1100,100,0,100);
  TH2F *h2_x_y_ZDC_pn_lead = new TH2F("h2_x_y_ZDC_pn_lead","ZDC: p lead  (pn) ",100,600,1100,100,0,100);
  TH2F *h2_x_y_ZDC_pn_reco = new TH2F("h2_x_y_ZDC_pn_reco","ZDC: n recoil (pn)",100,600,1100,100,0,100);
  TH2F *h2_x_y_ZDC_nn_lead = new TH2F("h2_x_y_ZDC_nn_lead","ZDC: n lead  (nn) ",100,600,1100,100,0,100);
  TH2F *h2_x_y_ZDC_nn_reco = new TH2F("h2_x_y_ZDC_nn_reco","ZDC: n recoil (nn)",100,600,1100,100,0,100);
  TH2F *h2_x_y_ZDC_pp_lead = new TH2F("h2_x_y_ZDC_pp_lead","ZDC: p lead  (pp) ",100,600,1100,100,0,100);
  TH2F *h2_x_y_ZDC_pp_reco = new TH2F("h2_x_y_ZDC_pp_reco","ZDC: p recoil (pp)",100,600,1100,100,0,100);
  TH2F *h2_x_y_ZDC_lead_n = new TH2F("h2_x_y_ZDC_lead_n","ZDC: n lead (all pairs) ",100,600,1100,100,0,100);
  TH2F *h2_x_y_ZDC_lead_p = new TH2F("h2_x_y_ZDC_lead_p","ZDC: p lead (all pairs) ",100,600,1100,100,0,100);
  TH2F *h2_x_y_ZDC_reco_n = new TH2F("h2_x_y_ZDC_reco_n","ZDC: n recoil (all pairs) ",100,600,1100,100,0,100);
  TH2F *h2_x_y_ZDC_reco_p = new TH2F("h2_x_y_ZDC_reco_p","ZDC: p recoil (all pairs) ",100,600,1100,100,0,100);

  TH2F *h2_x_y_Roman1_np_lead = new TH2F("h2_x_y_Roman1_np_lead","Roman1: n lead (np) ",100,600,1100,80,-40,40);
  TH2F *h2_x_y_Roman1_np_reco = new TH2F("h2_x_y_Roman1_np_reco","Roman1: p recoil (np)",100,600,1100,80,-40,40);
  TH2F *h2_x_y_Roman1_pn_lead = new TH2F("h2_x_y_Roman1_pn_lead","Roman1: p lead (pn) ",100,600,1100,80,-40,40);
  TH2F *h2_x_y_Roman1_pn_reco = new TH2F("h2_x_y_Roman1_pn_reco","Roman1: n recoil (pn)",100,600,1100,80,-40,40);
  TH2F *h2_x_y_Roman1_nn_lead = new TH2F("h2_x_y_Roman1_nn_lead","Roman1: n lead (nn) ",100,600,1100,80,-40,40);
  TH2F *h2_x_y_Roman1_nn_reco = new TH2F("h2_x_y_Roman1_nn_reco","Roman1: n recoil (nn)",100,600,1100,80,-40,40);
  TH2F *h2_x_y_Roman1_pp_lead = new TH2F("h2_x_y_Roman1_pp_lead","Roman1: p lead (pp) ",100,600,1100,80,-40,40);
  TH2F *h2_x_y_Roman1_pp_reco = new TH2F("h2_x_y_Roman1_pp_reco","Roman1: p recoil (pp)",100,600,1100,80,-40,40);
  TH2F *h2_x_y_Roman1_lead_p = new TH2F("h2_x_y_Roman1_lead_p","Roman1: p lead (all pairs)",100,600,1100,80,-40,40);
  TH2F *h2_x_y_Roman1_lead_n = new TH2F("h2_x_y_Roman1_lead_n","Roman1: n lead (all pairs)",100,600,1100,80,-40,40);
  TH2F *h2_x_y_Roman1_reco_p = new TH2F("h2_x_y_Roman1_reco_p","Roman1: p recoil (all pairs)",100,600,1100,80,-40,40);
  TH2F *h2_x_y_Roman1_reco_n = new TH2F("h2_x_y_Roman1_reco_n","Roman1: n recoil (all pairs)",100,600,1100,80,-40,40);

  TH2F *h2_x_y_Roman2_np = new TH2F("h2_x_y_Roman2_np","Roman2: n lead (np) ",100,600,1100,80,-40,40);
  TH2F *h2_x_y_Roman2_pn = new TH2F("h2_x_y_Roman2_pn","Roman2: n lead (np) ",100,600,1100,80,-40,40);
  TH2F *h2_x_y_Roman3_np = new TH2F("h2_x_y_Roman3_np","Roman3: n lead (np)",100,600,1100,80,-40,40);
  TH2F *h2_x_y_Roman3_pn = new TH2F("h2_x_y_Roman3_pn","Roman3: n lead (np)",100,600,1100,80,-40,40);
  TH2F *h2_x_y_Roman4_np = new TH2F("h2_x_y_Roman4_np","Roman4: n lead (np)",100,600,1100,80,-40,40);
  TH2F *h2_x_y_Roman4_pn = new TH2F("h2_x_y_Roman4_pn","Roman4: n lead (np)",100,600,1100,80,-40,40);

  //TH1F *h1_gen_pdg_one = new TH1F("h1_gen_pdg_one","",20,0,20);
  for (int i = 0; i < numberofevents; i++) {
//  for (int i = 0; i < 100; i++) {
    evt_tree->GetEntry(i);
  //  if (i < 1000 && hit_vol_name->at(2).rfind("ffi_RPOT_D2_lay",0) == 0 ) {
  //    cout << "hit_vol_name " << hit_vol_name->at(2) << endl;
  // }

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

    TLorentzVector T4_recoil_n_gen;
    TLorentzVector T4_recoil_n_acc;
    TLorentzVector T4_recoil_p_gen;
    TLorentzVector T4_recoil_p_acc;
    // scattered_electron.SetPxPyPzE(PHKK1[track], PHKK2[track], PHKK3[track], PHKK4[track]);
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
         double px = gen_prt_dir_x->at(2)*gen_prt_tot_mom->at(2);
         double py = gen_prt_dir_y->at(2)*gen_prt_tot_mom->at(2);
         double pz = gen_prt_dir_z->at(2)*gen_prt_tot_mom->at(2);
         double er = gen_prt_tot_e->at(2);
         T4_recoil_n_gen.SetPxPyPzE(px,py,pz,er);
      //   cout << "Recoil neutron 4-vector before boost " << px << " , " << py << " , " << pz << " , " << er << " , "<< T4_recoil_n_gen.M() << endl;
         T4_recoil_n_gen.Boost(-boost_irf);
      //   cout << "Recoil neutron generated 4-vector after boost " << T4_recoil_n_gen.X() << " , " << T4_recoil_n_gen.Y() << " , " << T4_recoil_n_gen.Z() << " , " << T4_recoil_n_gen.E() << " , "<< T4_recoil_n_gen.M() << endl;
         h1_recoil_n_generated->Fill(T4_recoil_n_gen.Vect().Mag());
         h1_recoil_generated->Fill(T4_recoil_n_gen.Vect().Mag());
         if (recoil_track_exists == true) { //independently if leading nucleon is also found in tracks
           if (tmp_track_recoil_pdg != gen_prt_pdg->at(2) ) {
             cout << "Event: " << i << " Mismatch track recoil PDG " << tmp_track_recoil_pdg << " and Generated PDG " << gen_prt_pdg->at(2) << endl;
             continue;
           }
           n_reco_tracks++;
           T4_recoil_n_acc.SetPxPyPzE(px,py,pz,er);
           T4_recoil_n_acc.Boost(-boost_irf);
      //     cout << "Recoil neutron accepted 4-vector after boost " << T4_recoil_n_acc.X() << " , " << T4_recoil_n_acc.Y() << " , " << T4_recoil_n_acc.Z() << " , " << T4_recoil_n_acc.E() << " , "<< T4_recoil_n_acc.M() << endl;
           h1_recoil_n_accepted->Fill(T4_recoil_n_acc.Vect().Mag());
           h1_recoil_accepted->Fill(T4_recoil_n_acc.Vect().Mag());
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
         double px = gen_prt_dir_x->at(2)*gen_prt_tot_mom->at(2);
         double py = gen_prt_dir_y->at(2)*gen_prt_tot_mom->at(2);
         double pz = gen_prt_dir_z->at(2)*gen_prt_tot_mom->at(2);
         double er = gen_prt_tot_e->at(2);
         T4_recoil_p_gen.SetPxPyPzE(px,py,pz,er);
      //   cout << "Recoil proton 4-vector before boost " << px << " , " << py << " , " << pz << " , " << er << " , "<< T4_recoil_p_gen.M() << endl;
         T4_recoil_p_gen.Boost(-boost_irf);
      //   cout << "Recoil proton generated 4-vector after boost " << T4_recoil_p_gen.X() << " , " << T4_recoil_p_gen.Y() << " , " << T4_recoil_p_gen.Z() << " , " << T4_recoil_p_gen.E() << " , "<< T4_recoil_p_gen.M() << endl;
         h1_recoil_p_generated->Fill(T4_recoil_p_gen.Vect().Mag());
         h1_recoil_generated->Fill(T4_recoil_p_gen.Vect().Mag());
         if (recoil_track_exists == true) { //independently if leading nucleon is also found in tracks
           if (tmp_track_recoil_pdg != gen_prt_pdg->at(2) ) {
             cout << "Event: " << i << " Mismatch track recoil PDG " << tmp_track_recoil_pdg << " and Generated PDG " << gen_prt_pdg->at(2) << endl;
             continue;
           }
           p_reco_tracks++;
           T4_recoil_p_acc.SetPxPyPzE(px,py,pz,er);
           T4_recoil_p_acc.Boost(-boost_irf);
        //   cout << "Recoil proton accepted 4-vector after boost " << T4_recoil_p_acc.X() << " , " << T4_recoil_p_acc.Y() << " , " << T4_recoil_p_acc.Z() << " , " << T4_recoil_p_acc.E() << " , "<< T4_recoil_p_acc.M() << endl;
           h1_recoil_p_accepted->Fill(T4_recoil_p_acc.Vect().Mag());
           h1_recoil_accepted->Fill(T4_recoil_p_acc.Vect().Mag());
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
         double px = gen_prt_dir_x->at(2)*gen_prt_tot_mom->at(2);
         double py = gen_prt_dir_y->at(2)*gen_prt_tot_mom->at(2);
         double pz = gen_prt_dir_z->at(2)*gen_prt_tot_mom->at(2);
         double er = gen_prt_tot_e->at(2);
         T4_recoil_n_gen.SetPxPyPzE(px,py,pz,er);
    //     cout << "Recoil neutron 4-vector before boost " << px << " , " << py << " , " << pz << " , " << er << " , "<< T4_recoil_n_gen.M() << endl;
         T4_recoil_n_gen.Boost(-boost_irf);
      //   cout << "Recoil neutron generated 4-vector after boost " << T4_recoil_n_gen.X() << " , " << T4_recoil_n_gen.Y() << " , " << T4_recoil_n_gen.Z() << " , " << T4_recoil_n_gen.E() << " , "<< T4_recoil_n_gen.M() << endl;
         h1_recoil_n_generated->Fill(T4_recoil_n_gen.Vect().Mag());
         h1_recoil_generated->Fill(T4_recoil_n_gen.Vect().Mag());
         if (recoil_track_exists == true) { //independently if leading nucleon is also found in tracks
           if (tmp_track_recoil_pdg != gen_prt_pdg->at(2) ) {
             cout << "Event: " << i << " Mismatch track recoil PDG " << tmp_track_recoil_pdg << " and Generated PDG " << gen_prt_pdg->at(2) << endl;
             continue;
           }
           n_reco_tracks++;
           T4_recoil_n_acc.SetPxPyPzE(px,py,pz,er);
           T4_recoil_n_acc.Boost(-boost_irf);
      //     cout << "Recoil neutron accepted 4-vector after boost " << T4_recoil_n_acc.X() << " , " << T4_recoil_n_acc.Y() << " , " << T4_recoil_n_acc.Z() << " , " << T4_recoil_n_acc.E() << " , "<< T4_recoil_n_acc.M() << endl;
           h1_recoil_n_accepted->Fill(T4_recoil_n_acc.Vect().Mag());
           h1_recoil_accepted->Fill(T4_recoil_n_acc.Vect().Mag());
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
         double px = gen_prt_dir_x->at(2)*gen_prt_tot_mom->at(2);
         double py = gen_prt_dir_y->at(2)*gen_prt_tot_mom->at(2);
         double pz = gen_prt_dir_z->at(2)*gen_prt_tot_mom->at(2);
         double er = gen_prt_tot_e->at(2);
         T4_recoil_p_gen.SetPxPyPzE(px,py,pz,er);
      //   cout << "Recoil proton 4-vector before boost " << px << " , " << py << " , " << pz << " , " << er << " , "<< T4_recoil_p_gen.M() << endl;
         T4_recoil_p_gen.Boost(-boost_irf);
      //   cout << "Recoil proton generated 4-vector after boost " << T4_recoil_p_gen.X() << " , " << T4_recoil_p_gen.Y() << " , " << T4_recoil_p_gen.Z() << " , " << T4_recoil_p_gen.E() << " , "<< T4_recoil_p_gen.M() << endl;
         h1_recoil_p_generated->Fill(T4_recoil_p_gen.Vect().Mag());
         h1_recoil_generated->Fill(T4_recoil_p_gen.Vect().Mag());
         if (recoil_track_exists == true) { //independently if leading nucleon is also found in tracks
           if (tmp_track_recoil_pdg != gen_prt_pdg->at(2) ) {
             cout << "Event: " << i << " Mismatch track recoil PDG " << tmp_track_recoil_pdg << " and Generated PDG " << gen_prt_pdg->at(2) << endl;
             continue;
           }
           p_reco_tracks++;
           T4_recoil_p_acc.SetPxPyPzE(px,py,pz,er);
           T4_recoil_p_acc.Boost(-boost_irf);
      //     cout << "Recoil proton accepted 4-vector after boost " << T4_recoil_p_acc.X() << " , " << T4_recoil_p_acc.Y() << " , " << T4_recoil_p_acc.Z() << " , " << T4_recoil_p_acc.E() << " , "<< T4_recoil_p_acc.M() << endl;
           h1_recoil_p_accepted->Fill(T4_recoil_p_acc.Vect().Mag());
           h1_recoil_accepted->Fill(T4_recoil_p_acc.Vect().Mag());

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
            //     cout << "ZDC x y " << hit_x->at(ihit)<< "  ,  " << hit_y->at(ihit)<< endl;
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
            //     cout << "Roman x y " << hit_x->at(ihit)<< "  ,  " << hit_y->at(ihit)<< endl;
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
        //      cout << "ZDC x y " << hit_x->at(ihit)<< "  ,  " << hit_y->at(ihit)<< endl;
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
            //     cout << "Roman x y " << hit_x->at(ihit)<< "  ,  " << hit_y->at(ihit)<< endl;
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

  /*TCanvas *c1 = new TCanvas("c1","Number");
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
*/
TCanvas *c6 = new TCanvas("c6","acceptances ");
c6->Divide(2,2);
c6->cd(1);
h1_recoil_generated->Draw("E");
c6->cd(2);
h1_recoil_p_generated->SetLineColor(2); //red
h1_recoil_n_generated->SetLineColor(4); //blue
h1_recoil_p_generated->Draw("E");
h1_recoil_n_generated->Draw("SAME");
c6->cd(3);
h1_recoil_accepted->Draw("E");
c6->cd(4);
h1_recoil_p_accepted->SetLineColor(2); //red
h1_recoil_n_accepted->SetLineColor(4); //blue
h1_recoil_p_accepted->Draw("E");
h1_recoil_accepted->Draw("SAME");
  c6->SaveAs("accepted.root");


  output->cd();
  h1_hits_count->Write();
  h1_trk_count->Write();
  h1_gen_count->Write();
  h2_x_y_ZDC_np_lead->Write();
  h2_x_y_ZDC_np_reco->Write();
  h2_x_y_ZDC_pn_lead->Write();
  h2_x_y_ZDC_pn_reco->Write();
  h2_x_y_ZDC_nn_lead->Write();
  h2_x_y_ZDC_nn_reco->Write();
  h2_x_y_ZDC_pp_lead->Write();
  h2_x_y_ZDC_pp_reco->Write();
  h2_x_y_ZDC_lead_n->Write();
  h2_x_y_ZDC_lead_p->Write();
  h2_x_y_ZDC_reco_n->Write();
  h2_x_y_ZDC_reco_p->Write();

  h2_x_y_Roman1_np_lead->Write();
  h2_x_y_Roman1_np_reco->Write();
  h2_x_y_Roman1_pn_lead->Write();
  h2_x_y_Roman1_pn_reco->Write();
  h2_x_y_Roman1_nn_lead->Write();
  h2_x_y_Roman1_nn_reco->Write();
  h2_x_y_Roman1_pp_lead->Write();
  h2_x_y_Roman1_pp_reco->Write();
  h2_x_y_Roman1_lead_p->Write();
  h2_x_y_Roman1_lead_n->Write();
  h2_x_y_Roman1_reco_p->Write();
  h2_x_y_Roman1_reco_n->Write();

  h2_x_y_Roman2_np->Write();
  h2_x_y_Roman2_pn->Write();
  h2_x_y_Roman3_np->Write();
  h2_x_y_Roman3_pn->Write();
  h2_x_y_Roman4_np->Write();
  h2_x_y_Roman4_pn->Write();

  h1_recoil_generated->Write();
  h1_recoil_n_generated->Write();
  h1_recoil_p_generated->Write();
  h1_recoil_accepted->Write();
  h1_recoil_n_accepted->Write();
  h1_recoil_p_accepted->Write();

  output->Close();

}
