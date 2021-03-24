void g4e_analysis (TString inputstring, int targetA, int setting, double electronmomentum)
{
  //This only works for older g4e version outputs
  bool debug = false;
  bool fillhit = true;
  double ZDC_r_cut = 250; //[mm]
  gStyle->SetOptStat(0);
  TFile *inputfile = new TFile(inputstring,"READ");
  TTree *evt_tree = (TTree*)inputfile->Get("events");

  TFile *output  = new TFile("output.root","RECREATE");
  TTree *outtree = new TTree("skim","skimmed g4e for acceptance");

  //Values for Output tree
  double skim_weight;
  TLorentzVector skim_electron;
  TLorentzVector skim_leading;
  TLorentzVector skim_recoil;
  int skim_pid_lead = 0;
  int skim_pid_recoil = 0;
  double skim_ebeam_mom = 0;
  int skim_targetA = 0;;
  int skim_targetZ = 0;
  double skim_targetmass= 0;
  double skim_ionenergy = 0;
  int skim_pp_pair = 0;
  int skim_pn_pair = 0;
  int skim_np_pair = 0;
  int skim_nn_pair = 0;

  //Define outtree Branches
  outtree->Branch("weight", &skim_weight);
  outtree->Branch("electron", &skim_electron);
  outtree->Branch("leading", &skim_leading);
  outtree->Branch("recoil", &skim_recoil);
  outtree->Branch("pid_leading", &skim_pid_lead);
  outtree->Branch("pid_recoil", &skim_pid_recoil);
  outtree->Branch("ebeam_mom", &skim_pid_recoil);
  outtree->Branch("target_A", &skim_targetA);
  outtree->Branch("target_Z", &skim_targetZ);
  outtree->Branch("target_mass", &skim_targetmass);
  outtree->Branch("ionenergy", &skim_ionenergy);
  outtree->Branch("pp_pair_accept", &skim_pp_pair);
  outtree->Branch("np_pair_accept", &skim_np_pair);
  outtree->Branch("pn_pair_accept", &skim_pn_pair);
  outtree->Branch("nn_pair_accept", &skim_nn_pair);

  //make this dependent on file !!!!!
  TLorentzVector ebeam_coll(0,0,-electronmomentum,electronmomentum);

  int numberofevents = evt_tree->GetEntriesFast();
  numberofevents--; //Problem with last event in some files
  //numberofevents = 20;
  cout << "Number of events " << numberofevents << endl;

  double crossingangle = -0.025;
  crossingangle = crossingangle/2.;
  //Boost vector beta = 0.025 in x-direction
  TVector3 boost_vector(crossingangle,0,0);

  double u_in_GeV = 0.9315; //u in GeV
  double m_proton = 0.938272; //mass in GeV
  double m_electron = 0.000511;
  double ionmomentum = 0;
  double ionenergy = 0;

  if (setting == 1) { //275 GeV protons
  //  ionmomentum = 275./2 * targetA;
    ionenergy = 275 * targetA;
  }
  else if (setting == 2) { //100GeV proton setting
  //  ionmomentum = 100./2 * targetA;
    ionenergy = 110 * targetA;
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
    mass_target = 12*u_in_GeV-6*m_electron;
    targetZ = 6;
  }
  else if (targetA == 4) {
    cout << "Assuming He 4 running" << endl;
    mass_target = 4.002603*u_in_GeV-2*m_electron;
    targetZ = 2;
  }
  else if (targetA == 2) {
    cout << "Assuming Deuterium running" << endl;
    mass_target = 2.014102*u_in_GeV-m_electron;
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
//Weigted counting
  double p_lead_generated_weighted = 0; //leading protons
  double n_lead_generated_weighted = 0; //leading neutrons
  double p_reco_generated_weighted = 0; //recoil protons
  double n_reco_generated_weighted = 0; //recoil neutrons
  double p_lead_tracks_weighted = 0; //leading protons output track
  double n_lead_tracks_weighted = 0; //leading neutrons output track
  double p_reco_tracks_weighted = 0; //recoil protons output track
  double n_reco_tracks_weighted = 0; //recoil neutrons output track
  double pp_generated_weighted = 0;
  double nn_generated_weighted = 0;
  double np_generated_weighted = 0; //leading neutron and recoil proton
  double pn_generated_weighted = 0; //leading proton and recoil neutron
  double pp_tracks_weighted = 0; //leading proton and recoil proton tracks in event
  double nn_tracks_weighted = 0; //leading neutron and recoil neutron tracks in event
  double np_tracks_weighted = 0; //leading neutron and recoil proton tracks in event
  double pn_tracks_weighted = 0; //leading proton and recoil neutron tracks in event


  //Event Infos
  ULong64_t event_id = 0;
  double evt_true_q2 = 0;
  double evt_true_x = 0;
  double evt_true_y = 0;
  double evt_true_w2 = 0;
  double evt_true_nu = 0;
  double evt_true_t_hat = 0;
  double evt_weight = 0;

  ULong64_t hit_count; //max usually 55000
  ULong64_t gen_prt_count; //max usually 12
  ULong64_t trk_count; //max usually 17

  std::vector<double> *hit_x = 0;
  std::vector<double> *hit_y = 0;
  std::vector<double> *hit_z = 0;
  std::vector<ULong64_t> *hit_id = 0;
  std::vector<ULong64_t> *hit_trk_id = 0;
  std::vector<ULong64_t> *hit_ptr_id = 0;  //is this particle id?
  std::vector<ULong64_t> *hit_parent_trk_id = 0;
  std::vector<string> *hit_vol_name = 0;
  std::vector<ULong64_t> *hit_i_rep = 0;
  std::vector<ULong64_t> *hit_j_rep = 0;
  std::vector<double> *hit_e_loss = 0;

  std::vector<ULong64_t> *gen_prt_id = 0;
  std::vector<ULong64_t> *gen_prt_vtx_id = 0;
  std::vector<ULong64_t> *gen_prt_pdg = 0;
  std::vector<double> *gen_prt_charge = 0;
  std::vector<ULong64_t> *gen_prt_trk_id = 0;
  std::vector<double> *gen_prt_dir_x = 0;
  std::vector<double> *gen_prt_dir_y = 0;
  std::vector<double> *gen_prt_dir_z = 0;
  std::vector<double> *gen_prt_tot_mom = 0;
  std::vector<double> *gen_prt_tot_e = 0;

  std::vector<ULong64_t> *trk_id = 0;
  std::vector<double> *trk_pdg = 0;
  std::vector<ULong64_t> *trk_parent_id = 0;
  std::vector<double> *trk_create_proc = 0;
  std::vector<ULong64_t> *trk_level = 0;
  std::vector<double> *trk_mom = 0;

  evt_tree->SetBranchAddress("event_id",&event_id);
  evt_tree->SetBranchAddress("evt_true_q2",&evt_true_q2);
  evt_tree->SetBranchAddress("evt_true_w2",&evt_true_w2);
  evt_tree->SetBranchAddress("evt_true_nu",&evt_true_nu);
  evt_tree->SetBranchAddress("evt_true_x",&evt_true_x);
  evt_tree->SetBranchAddress("evt_true_y",&evt_true_y);
  evt_tree->SetBranchAddress("evt_true_t_hat",&evt_true_t_hat);
  evt_tree->SetBranchAddress("evt_weight",&evt_weight);

  evt_tree->SetBranchAddress("hit_count",&hit_count);
  evt_tree->SetBranchAddress("hit_x",&hit_x);
  evt_tree->SetBranchAddress("hit_y",&hit_y);
  evt_tree->SetBranchAddress("hit_z",&hit_z);
  evt_tree->SetBranchAddress("hit_id",&hit_id);
  evt_tree->SetBranchAddress("hit_trk_id",&hit_trk_id);
  evt_tree->SetBranchAddress("hit_ptr_id",&hit_ptr_id);
  evt_tree->SetBranchAddress("hit_parent_trk_id",&hit_parent_trk_id);
  evt_tree->SetBranchAddress("hit_vol_name",&hit_vol_name);
  evt_tree->SetBranchAddress("hit_i_rep",&hit_i_rep);
  evt_tree->SetBranchAddress("hit_j_rep",&hit_j_rep);
  evt_tree->SetBranchAddress("hit_e_loss",&hit_e_loss);

  evt_tree->SetBranchAddress("gen_prt_count",&gen_prt_count);
  evt_tree->SetBranchAddress("gen_prt_id",&gen_prt_id);
  evt_tree->SetBranchAddress("gen_prt_vtx_id",&gen_prt_vtx_id);
  evt_tree->SetBranchAddress("gen_prt_pdg",&gen_prt_pdg);
  evt_tree->SetBranchAddress("gen_prt_charge",&gen_prt_charge);
  evt_tree->SetBranchAddress("gen_prt_trk_id",&gen_prt_trk_id);
  evt_tree->SetBranchAddress("gen_prt_dir_x",&gen_prt_dir_x);
  evt_tree->SetBranchAddress("gen_prt_dir_y",&gen_prt_dir_y);
  evt_tree->SetBranchAddress("gen_prt_dir_z",&gen_prt_dir_z);
  evt_tree->SetBranchAddress("gen_prt_tot_mom",&gen_prt_tot_mom);
  evt_tree->SetBranchAddress("gen_prt_tot_e",&gen_prt_tot_e);

  evt_tree->SetBranchAddress("trk_count",&trk_count);
  evt_tree->SetBranchAddress("trk_id",&trk_id);
  evt_tree->SetBranchAddress("trk_pdg",&trk_pdg);
  evt_tree->SetBranchAddress("trk_parent_id",&trk_parent_id);
  evt_tree->SetBranchAddress("trk_create_proc",&trk_create_proc);
  evt_tree->SetBranchAddress("trk_level",&trk_level);
  evt_tree->SetBranchAddress("trk_mom",&trk_mom);

  TH1F *h1_Q2  = new TH1F("h1_Q2","Q2 distribution",40,0,20);
  TH1F *h1_x = new TH1F("h1_x","x distribution",68,0.8,2.5);
  TH2F *h2_Q2_x = new TH2F("h2_Q2_x","Q2 versus x",40,1,21,24,1,2.2);


  TH1F *h1_hits_count = new TH1F("h1_hits_count","",60,0,600);
  TH1F *h1_gen_count = new TH1F("h1_gen_count","",20,0,20);
  TH1F *h1_trk_count = new TH1F("h1_trk_count","",80,0,80);

  TH1I *h1_recoil_p_names = new TH1I("h1_recoil_p_names","Detector strings of hits for recoil protons",20,0,20);
  TH1I *h1_recoil_n_names = new TH1I("h1_recoil_n_names","Detector strings of hits for recoil neutrons",20,0,20);
  TH1I *h1_lead_p_names = new TH1I("h1_lead_p_names","Detector strings of hits for leading protons",20,0,20);
  TH1I *h1_lead_n_names = new TH1I("h1_lead_n_names","Detector strings of hits for leading neutrons",20,0,20);

  TH2F *h2_lead_mom_theta = new TH2F("h2_lead_mom_theta","Leading nucleon mom-theta dist. (no weight)",50,0,100,50,0,100);
  TH2F *h2_lead_mom_theta_accepted = new TH2F("h2_lead_mom_theta_accepted","Leading nucleon mom-theta dist. accepted (no weight)",50,0,100,50,0,100);
  TH2F *h2_reco_mom_theta = new TH2F("h2_reco_mom_theta","Recoil nucleon mom-theta dist. (no weight)",50,0,100,50,0,100);
  TH2F *h2_reco_mom_theta_accepted = new TH2F("h2_reco_mom_theta_accepted","Recoil nucleon mom-theta dist. accepted (no weight)",50,0,100,50,0,100);
  //Histo for theta acceptance
  TH1F *h1_recoil_theta_generated = new TH1F("h1_recoil_theta_generated","Theta generated recoil",160,0,80);
  TH1F *h1_recoil_theta_n_generated = new TH1F("h1_recoil_theta_n_generated","Theta generated neutron",160,0,80);
  TH1F *h1_recoil_theta_p_generated = new TH1F("h1_recoil_theta_p_generated","Theta generated proton",160,0,80);
  TH1F *h1_recoil_theta_accepted = new TH1F("h1_recoil_theta_accepted","Theta accepted recoil",160,0,80);
  TH1F *h1_recoil_theta_n_accepted = new TH1F("h1_recoil_theta_n_accepted","Theta accepted neutron",160,0,80);
  TH1F *h1_recoil_theta_p_accepted = new TH1F("h1_recoil_theta_p_accepted","Theta accepted proton",160,0,80);
  TH1F *h1_lead_theta_generated = new TH1F("h1_lead_theta_generated","Theta generated leading",160,0,80);
  TH1F *h1_lead_theta_n_generated = new TH1F("h1_lead_theta_n_generated","Theta generated neutron",160,0,80);
  TH1F *h1_lead_theta_p_generated = new TH1F("h1_lead_theta_p_generated","Theta generated proton",160,0,80);
  TH1F *h1_lead_theta_accepted = new TH1F("h1_lead_theta_accepted","Theta accepted leading",160,0,80);
  TH1F *h1_lead_theta_n_accepted = new TH1F("h1_lead_theta_n_accepted","Theta accepted neutron",160,0,80);
  TH1F *h1_lead_theta_p_accepted = new TH1F("h1_lead_theta_p_accepted","Theta accepted proton",160,0,80);

  TH1F *h1_recoil_theta_generated_noweight = new TH1F("h1_recoil_theta_generated_noweight","Theta generated recoil (no weight)",160,0,80);
  TH1F *h1_recoil_theta_accepted_noweight  = new TH1F("h1_recoil_theta_accepted_noweight","Theta accepted recoil (no weight)",160,0,80);
  TH1F *h1_lead_theta_generated_noweight   = new TH1F("h1_lead_theta_generated_noweight","Theta generated leading (no weight)",160,0,80);
  TH1F *h1_lead_theta_accepted_noweight    = new TH1F("h1_lead_theta_accepted_noweight","Theta accepted leading (no weight)",160,0,80);
   //Histo for pIRF acceptance
  TH1F *h1_recoil_generated = new TH1F("h1_recoil_generated","P_irf generated recoil",100,0,2);
  TH1F *h1_recoil_n_generated = new TH1F("h1_recoil_n_generated","P_irf generated neutron",100,0,2);
  TH1F *h1_recoil_p_generated = new TH1F("h1_recoil_p_generated","P_irf generated proton",100,0,2);
  TH1F *h1_recoil_accepted = new TH1F("h1_recoil_accepted","P_irf accepted recoil",100,0,2);
  TH1F *h1_recoil_n_accepted = new TH1F("h1_recoil_n_accepted","P_irf accepted neutron",100,0,2);
  TH1F *h1_recoil_p_accepted = new TH1F("h1_recoil_p_accepted","P_irf accepted proton",100,0,2);

  TH1F *h1_recoil_generated_noweight   = new TH1F("h1_recoil_generated_noweight","P_irf generated recoil",200,0,2);
  TH1F *h1_recoil_n_generated_noweight = new TH1F("h1_recoil_n_generated_noweight","P_irf generated neutron",200,0,2);
  TH1F *h1_recoil_p_generated_noweight = new TH1F("h1_recoil_p_generated_noweight","P_irf generated proton",200,0,2);
  TH1F *h1_recoil_accepted_noweight   = new TH1F("h1_recoil_accepted_noweight","P_irf accepted recoil",200,0,2);
  TH1F *h1_recoil_n_accepted_noweight = new TH1F("h1_recoil_n_accepted_noweight","P_irf accepted neutron",200,0,2);
  TH1F *h1_recoil_p_accepted_noweight = new TH1F("h1_recoil_p_accepted_noweight","P_irf accepted proton",200,0,2);

  //Histo for pmiss acceptance
 TH1F *h1_pmiss_generated = new TH1F("h1_pmiss_generated","Pmiss generated recoil",50,0,2);
 TH1F *h1_pmiss_n_generated = new TH1F("h1_pmiss_n_generated","Pmiss generated neutron",50,0,2);
 TH1F *h1_pmiss_p_generated = new TH1F("h1_pmiss_p_generated","Pmiss generated proton",50,0,2);
 TH1F *h1_pmiss_accepted = new TH1F("h1_pmiss_accepted","Pmiss accepted recoil",50,0,2);
 TH1F *h1_pmiss_n_accepted = new TH1F("h1_pmiss_n_accepted","Pmiss accepted neutron",50,0,2);
 TH1F *h1_pmiss_p_accepted = new TH1F("h1_pmiss_p_accepted","Pmiss accepted proton",50,0,2);


//ffi_RPOT_D2_lay
//fi_D1_TRK_
//ffi_ZDC_
  TH2F *h2_x_y_ZDC_np_lead = new TH2F("h2_x_y_ZDC_np_lead","ZDC: n lead  (np) ",100,400,1300,100,-300,300);
  TH2F *h2_x_y_ZDC_np_reco = new TH2F("h2_x_y_ZDC_np_reco","ZDC: p recoil (np)",100,400,1300,100,-300,300);
  TH2F *h2_x_y_ZDC_pn_lead = new TH2F("h2_x_y_ZDC_pn_lead","ZDC: p lead  (pn) ",100,400,1300,100,-300,300);
  TH2F *h2_x_y_ZDC_pn_reco = new TH2F("h2_x_y_ZDC_pn_reco","ZDC: n recoil (pn)",100,400,1300,100,-300,300);
  TH2F *h2_x_y_ZDC_nn_lead = new TH2F("h2_x_y_ZDC_nn_lead","ZDC: n lead  (nn) ",100,400,1300,100,-300,300);
  TH2F *h2_x_y_ZDC_nn_reco = new TH2F("h2_x_y_ZDC_nn_reco","ZDC: n recoil (nn)",100,400,1300,100,-300,300);
  TH2F *h2_x_y_ZDC_pp_lead = new TH2F("h2_x_y_ZDC_pp_lead","ZDC: p lead  (pp) ",100,400,1300,100,-300,300);
  TH2F *h2_x_y_ZDC_pp_reco = new TH2F("h2_x_y_ZDC_pp_reco","ZDC: p recoil (pp)",100,400,1300,100,-300,300);
  TH2F *h2_x_y_ZDC_lead_n = new TH2F("h2_x_y_ZDC_lead_n","ZDC: n lead (all pairs) ",100,400,1300,100,-300,300);
  TH2F *h2_x_y_ZDC_lead_p = new TH2F("h2_x_y_ZDC_lead_p","ZDC: p lead (all pairs) ",100,400,1300,100,-300,300);
  TH2F *h2_x_y_ZDC_reco_n = new TH2F("h2_x_y_ZDC_reco_n","ZDC: n recoil (all pairs) ",100,400,1300,100,-300,300);
  TH2F *h2_x_y_ZDC_reco_p = new TH2F("h2_x_y_ZDC_reco_p","ZDC: p recoil (all pairs) ",100,400,1300,100,-300,300);

  TH2F *h2_x_y_Roman1_np_lead = new TH2F("h2_x_y_Roman1_np_lead","Roman1: n lead (np) ",100,650,1050,80,-60,60);
  TH2F *h2_x_y_Roman1_np_reco = new TH2F("h2_x_y_Roman1_np_reco","Roman1: p recoil (np)",100,650,1050,80,-60,60);
  TH2F *h2_x_y_Roman1_pn_lead = new TH2F("h2_x_y_Roman1_pn_lead","Roman1: p lead (pn) ",100,650,1050,80,-60,60);
  TH2F *h2_x_y_Roman1_pn_reco = new TH2F("h2_x_y_Roman1_pn_reco","Roman1: n recoil (pn)",100,650,1050,80,-60,60);
  TH2F *h2_x_y_Roman1_nn_lead = new TH2F("h2_x_y_Roman1_nn_lead","Roman1: n lead (nn) ",100,650,1050,80,-60,60);
  TH2F *h2_x_y_Roman1_nn_reco = new TH2F("h2_x_y_Roman1_nn_reco","Roman1: n recoil (nn)",100,650,1050,80,-60,60);
  TH2F *h2_x_y_Roman1_pp_lead = new TH2F("h2_x_y_Roman1_pp_lead","Roman1: p lead (pp) ",100,650,1050,80,-60,60);
  TH2F *h2_x_y_Roman1_pp_reco = new TH2F("h2_x_y_Roman1_pp_reco","Roman1: p recoil (pp)",100,650,1050,80,-60,60);
  TH2F *h2_x_y_Roman1_lead_p = new TH2F("h2_x_y_Roman1_lead_p","Roman1: p lead (all pairs)",100,650,1050,80,-60,60);
  TH2F *h2_x_y_Roman1_lead_n = new TH2F("h2_x_y_Roman1_lead_n","Roman1: n lead (all pairs)",100,650,1050,80,-60,60);
  TH2F *h2_x_y_Roman1_reco_p = new TH2F("h2_x_y_Roman1_reco_p","Roman1: p recoil (all pairs)",100,650,1050,80,-60,60);
  TH2F *h2_x_y_Roman1_reco_n = new TH2F("h2_x_y_Roman1_reco_n","Roman1: n recoil (all pairs)",100,650,1050,80,-60,60);


  TH2F *h2_x_y_B0_lead_p = new TH2F("h2_x_y_B0_lead_p","B0: p lead (all pairs)",100,-100,400,200,-250,250);
  TH2F *h2_x_y_B0_lead_n = new TH2F("h2_x_y_B0_lead_n","B0: n lead (all pairs)",100,-100,400,200,-250,250);
  TH2F *h2_x_y_B0_reco_p = new TH2F("h2_x_y_B0_reco_p","B0: p recoil (all pairs)",100,-100,400,200,-250,250);
  TH2F *h2_x_y_B0_reco_n = new TH2F("h2_x_y_B0_reco_n","B0: n recoil (all pairs)",100,-100,400,200,-250,250);

  TH2F *h2_x_y_OFF_lead_p = new TH2F("h2_x_y_OFF_lead_p","OFF: p lead (all pairs)",100,500,900,100,-150,150);
  TH2F *h2_x_y_OFF_lead_n = new TH2F("h2_x_y_OFF_lead_n","OFF: n lead (all pairs)",100,500,900,100,-150,150);
  TH2F *h2_x_y_OFF_reco_p = new TH2F("h2_x_y_OFF_reco_p","OFF: p recoil (all pairs)",100,500,900,100,-150,150);
  TH2F *h2_x_y_OFF_reco_n = new TH2F("h2_x_y_OFF_reco_n","OFF: n recoil (all pairs)",100,500,900,100,-150,150);

  TH2F *h2_x_y_OFF2_lead_p = new TH2F("h2_x_y_OFF2_lead_p","OFF2: p lead (all pairs)",100,650,1250,100,-150,150);
  TH2F *h2_x_y_OFF2_lead_n = new TH2F("h2_x_y_OFF2_lead_n","OFF2: n lead (all pairs)",100,650,1250,100,-150,150);
  TH2F *h2_x_y_OFF2_reco_p = new TH2F("h2_x_y_OFF2_reco_p","OFF2: p recoil (all pairs)",100,650,1250,100,-150,150);
  TH2F *h2_x_y_OFF2_reco_n = new TH2F("h2_x_y_OFF2_reco_n","OFF2: n recoil (all pairs)",100,650,1250,100,-150,150);

  /*TH2F *h2_x_y_Roman2_np = new TH2F("h2_x_y_Roman2_np","Roman2: n lead (np) ",100,600,1100,80,-40,40);
  TH2F *h2_x_y_Roman2_pn = new TH2F("h2_x_y_Roman2_pn","Roman2: n lead (np) ",100,600,1100,80,-40,40);
  TH2F *h2_x_y_Roman3_np = new TH2F("h2_x_y_Roman3_np","Roman3: n lead (np)",100,600,1100,80,-40,40);
  TH2F *h2_x_y_Roman3_pn = new TH2F("h2_x_y_Roman3_pn","Roman3: n lead (np)",100,600,1100,80,-40,40);
  TH2F *h2_x_y_Roman4_np = new TH2F("h2_x_y_Roman4_np","Roman4: n lead (np)",100,600,1100,80,-40,40);
  TH2F *h2_x_y_Roman4_pn = new TH2F("h2_x_y_Roman4_pn","Roman4: n lead (np)",100,600,1100,80,-40,40);
*/
  //TH1F *h1_gen_pdg_one = new TH1F("h1_gen_pdg_one","",20,0,20);
  for (int i = 0; i < numberofevents; i++) {

    //Clear values for outtree Branches
    skim_weight = 0;
    skim_electron.Clear();
    skim_leading.Clear();
    skim_recoil.Clear();
    skim_pid_lead = 0;
    skim_pid_recoil = 0;
    skim_ebeam_mom = 0;
    skim_targetA = 0;;
    skim_targetZ = 0;
    skim_targetmass= 0;
    skim_ionenergy = 0;
    skim_pp_pair = 0;
    skim_pn_pair = 0;
    skim_np_pair = 0;
    skim_nn_pair = 0;

    //Set target and momentum for output Tree
    skim_ebeam_mom = electronmomentum;
    skim_targetA    = targetA;
    skim_targetZ    = targetZ;
    skim_targetmass = mass_target;
    skim_ionenergy  = ionenergy;

//  for (int i = 0; i < 100; i++) {
    evt_tree->GetEntry(i);
  //  if (i < 1000 && hit_vol_name->at(2).rfind("ffi_RPOT_D2_lay",0) == 0 ) {
  //    cout << "hit_vol_name " << hit_vol_name->at(2) << endl;
  // }

    h1_hits_count->Fill(hit_count);
    h1_gen_count->Fill(gen_prt_count);
    h1_trk_count->Fill(trk_count);
  //  cout << "True Q2 " << evt_true_q2 << " , x " << evt_true_x << " , weight " << evt_weight << endl;
    h1_Q2->Fill(evt_true_q2,evt_weight);
    h1_x->Fill(evt_true_x,evt_weight);
    h2_Q2_x->Fill(evt_true_q2,evt_true_x,evt_weight);
  //  h1_Q2->Fill(evt_true_q2);
  //  h1_x->Fill(evt_true_x);
  //  h2_Q2_x->Fill(evt_true_q2,evt_true_x);
    if (debug) {

      cout << "Event " << i << endl;
      cout << "Gen track " << gen_prt_count << " , trk count " << trk_count << " , hit_count " << hit_count << endl;

      for (int gentrack = 0; gentrack < gen_prt_count; gentrack++ ) {

        cout << "Gen track " << gentrack << " , gen_prt_id: " << gen_prt_id->at(gentrack) ;
        cout << ", gen_prt_pdg: " << gen_prt_pdg->at(gentrack) << ", gen_prt_trkid: " << gen_prt_trk_id->at(gentrack) ;
        cout << ", gen_prt_charge: " << gen_prt_charge->at(gentrack) << ", gen_prt_tot_mom: " << gen_prt_tot_mom->at(gentrack);
        cout << ", gen_prt_tot_e: " << gen_prt_tot_e->at(gentrack)  << endl; ;
      }

      for (int track = 0; track < trk_count; track++ ) {

        cout << "Track " << track << " , trk_id: " << trk_id->at(track) ;
        cout << ", trk_pdg: " << trk_pdg->at(track) << ", trk_parent_id: " << trk_parent_id->at(track) ;
        cout << ", trk_level: " << trk_level->at(track) << ", trk_mom: " << trk_mom->at(track) << endl; ;
      }

      for (int hit = 0; hit < hit_count; hit++ ) {
       if (hit_trk_id->at(hit) == 2 || hit_trk_id->at(hit) == 3) {
        cout << "Hit " << hit << " , hit_id " << hit_id->at(hit) << ", hit_trk_id: " << hit_trk_id->at(hit) ;
        cout << ", hit_ptr_id: " << hit_ptr_id->at(hit) ; // << ", hit_parent_trk_id: " << hit_parent_trk_id->at(hit) ;
//cout << ", hit_i_rep: " << hit_i_rep->at(hit) << ", hit_j_rep: " << hit_j_rep->at(hit)  ;
        cout << ", hit_x: " << hit_x->at(hit) << ", hit_y: " << hit_y->at(hit) <<  ", hit_z: " << hit_z->at(hit) ;
        cout << ", hit_e_loss: " << hit_e_loss->at(hit) << ", hit_volname: " << hit_vol_name->at(hit) << endl ;
       }  //c_str()
      }
    }
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


//Determine by the hits if leading or recoil track is good
    bool lead_track_good = false;
    bool recoil_track_good = false;
//For proton hits
    int count_lead_hits_B0 = 0;
    int count_lead_hits_OFF = 0;
    int count_lead_hits_RP = 0;
    int count_lead_central = 0;
    int count_recoil_hits_B0 = 0;
    int count_recoil_hits_OFF = 0;
    int count_recoil_hits_RP = 0;

    for (int ihit = 0 ; ihit < hit_count ; ihit++) {
         if (hit_trk_id->at(ihit) == 2 && hit_ptr_id->at(ihit) == 2212   ) { //leading proton information
          if (hit_vol_name->at(ihit).rfind("ffi_RPOT_D2_lay_Phys_",0) == 0 ) { //Roman Pot
              count_lead_hits_RP++;
          }
          if (hit_vol_name->at(ihit).rfind("ffi_OFFM_TRK_lay_Phys",0) == 0 ) { //OFF Momentum
              count_lead_hits_OFF++;
          }
          if (hit_vol_name->at(ihit).rfind("fi_B0_TRK_lay_Phys",0) == 0 ) { //B0 Momentum
              count_lead_hits_B0++;
          }
          if (hit_vol_name->at(ihit).rfind("ci_",0) == 0 || hit_vol_name->at(ihit).rfind("cb_",0) == 0 || hit_vol_name->at(ihit).rfind("H_C",0) == 0 ) { //central hits
              count_lead_central++;
          }
        }
        if (hit_trk_id->at(ihit) == 2 && hit_ptr_id->at(ihit) == 2112  ) { //leading neutron information
          if (hit_vol_name->at(ihit).rfind("ffi_ZDC_",0) == 0  ) { //ZDC for neutron
              double xlocal = hit_x->at(ihit)-900; //units in mm
              double ylocal = hit_y->at(ihit); //units in mm
              double r = sqrt(xlocal*xlocal + ylocal*ylocal);
              if (r < ZDC_r_cut)
              {
                    lead_track_good = true; //assume good leading neutron track if one ZDC hit
              }
          }
        }
        if (hit_trk_id->at(ihit) == 3 && hit_ptr_id->at(ihit) == 2212  ) { //recoil proton information
          if (hit_vol_name->at(ihit).rfind("ffi_RPOT_D2_lay_Phys_",0) == 0 ) { //Roman Pot
              count_recoil_hits_RP++;
          }
          if (hit_vol_name->at(ihit).rfind("ffi_OFFM_TRK_lay_Phys",0) == 0 ) { //OFF Momentum
              count_recoil_hits_OFF++;
          }
          if (hit_vol_name->at(ihit).rfind("fi_B0_TRK_lay_Phys",0) == 0 ) { //B0 Momentum
              count_recoil_hits_B0++;
          }
        }
        if (hit_trk_id->at(ihit) == 3 && hit_ptr_id->at(ihit) == 2112  ) { //recoil neutron information
          if (hit_vol_name->at(ihit).rfind("ffi_ZDC_",0) == 0 ) { //ZDC for neutron
            double xlocal = hit_x->at(ihit)-900; //units in mm
            double ylocal = hit_y->at(ihit); //units in mm
            double r = sqrt(xlocal*xlocal + ylocal*ylocal);
            if (r < ZDC_r_cut)
            {
                recoil_track_good = true; //assume good recoil neutron track if one ZDC hit
            }

          }
        }
    }

  //  if (count_lead_hits_RP > 0 || count_lead_hits_B0 > 0 || count_lead_hits_OFF > 0) {
  //    cout << "RP " <<  count_lead_hits_RP<< " , B0 " << count_lead_hits_B0<< " , OFF " << count_lead_hits_OFF << endl;
  //  }
    //Assign if good leading or recoil proton track based on number of hits in detectors (no central for leading protons)
    if ( (count_lead_hits_RP > 2 || count_lead_hits_B0 > 3 || count_lead_hits_OFF > 1) && count_lead_central==0) {
      lead_track_good = true; //good leading proton track
    }
    if (count_recoil_hits_RP > 2 || count_recoil_hits_B0 > 3 || count_recoil_hits_OFF > 1) {
      recoil_track_good = true; //good recoil proton track
    }
    if (debug) {
    cout << "Lead track good " << lead_track_good << " Recoil track good " << recoil_track_good << endl;
    }

    bool lead_track_exists = false;
    bool recoil_track_exists = false;
    bool nn_track_exists = false;
    bool pp_track_exists = false;
    bool pn_track_exists = false;
    bool np_track_exists = false;


    TLorentzVector T4_lead_n_gen;
    TLorentzVector T4_lead_n_acc;
    TLorentzVector T4_lead_p_gen;
    TLorentzVector T4_lead_p_acc;
    TLorentzVector T4_recoil_n_gen;
    TLorentzVector T4_recoil_n_acc;
    TLorentzVector T4_recoil_p_gen;
    TLorentzVector T4_recoil_p_acc;
    // scattered_electron.SetPxPyPzE(PHKK1[track], PHKK2[track], PHKK3[track], PHKK4[track]);
    //Identifying event topologies and check if leading and recoil track ID does exists (meaning particles produced hits). Also compares PID values from Track and Generated value
    //Sets boolean values for the later loop over hits.
//Getting Electron kinematics
    TLorentzVector T4_ele_gen;
    double ele_px = gen_prt_dir_x->at(0)*gen_prt_tot_mom->at(0);
    double ele_py = gen_prt_dir_y->at(0)*gen_prt_tot_mom->at(0);
    double ele_pz = gen_prt_dir_z->at(0)*gen_prt_tot_mom->at(0);
    double ele_en = gen_prt_tot_e->at(0);
    T4_ele_gen.SetPxPyPzE(ele_px,ele_py,ele_pz,ele_en);
    T4_ele_gen.RotateY(crossingangle);
    T4_ele_gen.Boost(boost_vector);

    TLorentzVector T4_temp_ebeam_coll = ebeam_coll;
    T4_ele_gen.Boost(-boost_irf);
    T4_temp_ebeam_coll.Boost(-boost_irf);
    TLorentzVector q = T4_temp_ebeam_coll - T4_ele_gen;
    //Check Q2 and nu for sanity for first 20 events
    if (i < 10){
      cout << "Event " << i << endl;
      cout << " check of Q2: trueQ2 = " << evt_true_q2 << " , determined Q2 = " << -q.M2() << endl;
      cout << " check of nu: truenu = " << evt_true_nu << " , determined nu = " << q.E() << endl;

    }

    //Set electron values for outtree ( from lab system) and weight
    skim_electron.SetPxPyPzE(ele_px,ele_py,ele_pz,ele_en);
    skim_weight = evt_weight;

    if (gen_prt_pdg->at(1) == 2112) { //leading is neutron
       skim_pid_lead = 2112;
       n_lead_generated++;
       n_lead_generated_weighted+= evt_weight;
       double lead_px = gen_prt_dir_x->at(1)*gen_prt_tot_mom->at(1);
       double lead_py = gen_prt_dir_y->at(1)*gen_prt_tot_mom->at(1);
       double lead_pz = gen_prt_dir_z->at(1)*gen_prt_tot_mom->at(1);
       double lead_en = gen_prt_tot_e->at(1);
       T4_lead_n_gen.SetPxPyPzE(lead_px,lead_py,lead_pz,lead_en);
       T4_lead_n_gen.RotateY(crossingangle);
       T4_lead_n_gen.Boost(boost_vector);
       h1_lead_theta_generated->Fill(T4_lead_n_gen.Theta() * 1000, evt_weight);
       h1_lead_theta_n_generated->Fill(T4_lead_n_gen.Theta() * 1000, evt_weight);
       h1_lead_theta_generated_noweight->Fill(T4_lead_n_gen.Theta() * 1000);

       //output tree info in lab frame
       skim_leading.SetPxPyPzE(lead_px,lead_py,lead_pz,lead_en);

       //Calculation of pmiss
       TLorentzVector T4_temp_lead_n_gen = T4_lead_n_gen;
       T4_temp_lead_n_gen.Boost(-boost_irf);
       //Calculation of pmiss in IRF
       TLorentzVector T4_pmiss_gen = q - T4_temp_lead_n_gen;
       double pmiss = T4_pmiss_gen.P();
       h1_pmiss_generated->Fill(pmiss, evt_weight); //all Recoils independently of type

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
       if (lead_track_exists == true && lead_track_good == true) {
          n_lead_tracks++;
          n_lead_tracks_weighted+= evt_weight;
          T4_lead_n_acc.SetPxPyPzE(lead_px,lead_py,lead_pz,lead_en);
          T4_lead_n_acc.RotateY(crossingangle);
          T4_lead_n_acc.Boost(boost_vector);
          h1_lead_theta_accepted->Fill(T4_lead_n_gen.Theta() * 1000, evt_weight);
          h1_lead_theta_n_accepted->Fill(T4_lead_n_gen.Theta() * 1000, evt_weight);
          h1_lead_theta_accepted_noweight->Fill(T4_lead_n_gen.Theta() * 1000);
          h1_pmiss_accepted->Fill(pmiss, evt_weight); //all Recoils independently of type if leading is accepted
        }

       if (gen_prt_pdg->at(2) == 2112) { //recoil is neutron
         skim_pid_recoil = 2112;
         //nn events
         nn_generated++;
         nn_generated_weighted+= evt_weight;
         n_reco_generated++;
         n_reco_generated_weighted+= evt_weight;
         h1_pmiss_n_generated->Fill(pmiss, evt_weight); //calculate pmiss for recoil neuton
         if (lead_track_exists == true && lead_track_good == true) h1_pmiss_n_accepted->Fill(pmiss, evt_weight); //calculated pmiss for recoil neuton when leading is accepted

         double px = gen_prt_dir_x->at(2)*gen_prt_tot_mom->at(2);
         double py = gen_prt_dir_y->at(2)*gen_prt_tot_mom->at(2);
         double pz = gen_prt_dir_z->at(2)*gen_prt_tot_mom->at(2);
         double er = gen_prt_tot_e->at(2);
         T4_recoil_n_gen.SetPxPyPzE(px,py,pz,er);
      //   cout << "Recoil neutron 4-vector before boost " << px << " , " << py << " , " << pz << " , " << er << " , "<< T4_recoil_n_gen.M() << endl;
         T4_recoil_n_gen.RotateY(crossingangle);
         T4_recoil_n_gen.Boost(boost_vector);
         h1_recoil_theta_generated->Fill(T4_recoil_n_gen.Theta() * 1000, evt_weight);
         h1_recoil_theta_n_generated->Fill(T4_recoil_n_gen.Theta() * 1000, evt_weight);
         h1_recoil_theta_generated_noweight->Fill(T4_recoil_n_gen.Theta() * 1000);
         T4_recoil_n_gen.Boost(-boost_irf);

         //output tree info in lab frame
         skim_recoil.SetPxPyPzE(px,py,pz,er);

      //   cout << "Recoil neutron generated 4-vector after boost " << T4_recoil_n_gen.X() << " , " << T4_recoil_n_gen.Y() << " , " << T4_recoil_n_gen.Z() << " , " << T4_recoil_n_gen.E() << " , "<< T4_recoil_n_gen.M() << endl;
         h1_recoil_n_generated_noweight->Fill(T4_recoil_n_gen.Vect().Mag());
         h1_recoil_generated_noweight->Fill(T4_recoil_n_gen.Vect().Mag());
         h1_recoil_n_generated->Fill(T4_recoil_n_gen.Vect().Mag(), evt_weight);
         h1_recoil_generated->Fill(T4_recoil_n_gen.Vect().Mag(), evt_weight);
         if (recoil_track_exists == true && recoil_track_good == true) { //independently if leading nucleon is also found in tracks
           if (tmp_track_recoil_pdg != gen_prt_pdg->at(2) ) {
             cout << "Event: " << i << " Mismatch track recoil PDG " << tmp_track_recoil_pdg << " and Generated PDG " << gen_prt_pdg->at(2) << endl;
             continue;
           }
           n_reco_tracks++;
           n_reco_tracks_weighted+= evt_weight;
           T4_recoil_n_acc.SetPxPyPzE(px,py,pz,er);
           T4_recoil_n_acc.RotateY(crossingangle);
           T4_recoil_n_acc.Boost(boost_vector);
           h1_recoil_theta_accepted->Fill(T4_recoil_n_acc.Theta() * 1000, evt_weight);
           h1_recoil_theta_n_accepted->Fill(T4_recoil_n_acc.Theta() * 1000, evt_weight);
           h1_recoil_theta_accepted_noweight->Fill(T4_recoil_n_acc.Theta() * 1000);

           T4_recoil_n_acc.Boost(-boost_irf);
      //     cout << "Recoil neutron accepted 4-vector after boost " << T4_recoil_n_acc.X() << " , " << T4_recoil_n_acc.Y() << " , " << T4_recoil_n_acc.Z() << " , " << T4_recoil_n_acc.E() << " , "<< T4_recoil_n_acc.M() << endl;
           h1_recoil_n_accepted_noweight->Fill(T4_recoil_n_acc.Vect().Mag());
           h1_recoil_accepted_noweight->Fill(T4_recoil_n_acc.Vect().Mag());
           h1_recoil_n_accepted->Fill(T4_recoil_n_acc.Vect().Mag(), evt_weight);
           h1_recoil_accepted->Fill(T4_recoil_n_acc.Vect().Mag(), evt_weight);
           //Fill outttree if at least recoil is accepted!
           outtree->Fill();
           if (lead_track_exists == true && lead_track_good == true) { //found both tracks of the nn pair
              nn_tracks++;
              nn_tracks_weighted+= evt_weight;
              nn_track_exists = true;
              skim_nn_pair = 1;
           }
         }
       }
       if (gen_prt_pdg->at(2) == 2212) { //recoil is proton
         skim_pid_recoil = 2212;
         //np events
         np_generated++;
         np_generated_weighted+= evt_weight;
         p_reco_generated++;
         p_reco_generated_weighted+= evt_weight;
         h1_pmiss_p_generated->Fill(pmiss, evt_weight); //calculate pmiss for recoil proton
         if (lead_track_exists == true && lead_track_good == true) h1_pmiss_p_accepted->Fill(pmiss, evt_weight); //calculated pmiss for recoil proton when leading is accepted

         double px = gen_prt_dir_x->at(2)*gen_prt_tot_mom->at(2);
         double py = gen_prt_dir_y->at(2)*gen_prt_tot_mom->at(2);
         double pz = gen_prt_dir_z->at(2)*gen_prt_tot_mom->at(2);
         double er = gen_prt_tot_e->at(2);
         T4_recoil_p_gen.SetPxPyPzE(px,py,pz,er);
      //   cout << "Recoil proton 4-vector before boost " << px << " , " << py << " , " << pz << " , " << er << " , "<< T4_recoil_p_gen.M() << endl;
         T4_recoil_p_gen.RotateY(crossingangle);
         T4_recoil_p_gen.Boost(boost_vector);
         h1_recoil_theta_generated->Fill(T4_recoil_p_gen.Theta() * 1000, evt_weight);
         h1_recoil_theta_p_generated->Fill(T4_recoil_p_gen.Theta() * 1000, evt_weight);
         h1_recoil_theta_generated_noweight->Fill(T4_recoil_p_gen.Theta() * 1000);

         //output tree info in lab frame
         skim_recoil.SetPxPyPzE(px,py,pz,er);

         T4_recoil_p_gen.Boost(-boost_irf);
      //   cout << "Recoil proton generated 4-vector after boost " << T4_recoil_p_gen.X() << " , " << T4_recoil_p_gen.Y() << " , " << T4_recoil_p_gen.Z() << " , " << T4_recoil_p_gen.E() << " , "<< T4_recoil_p_gen.M() << endl;
         h1_recoil_p_generated_noweight->Fill(T4_recoil_p_gen.Vect().Mag());
         h1_recoil_generated_noweight->Fill(T4_recoil_p_gen.Vect().Mag());
         h1_recoil_p_generated->Fill(T4_recoil_p_gen.Vect().Mag(), evt_weight);
         h1_recoil_generated->Fill(T4_recoil_p_gen.Vect().Mag(), evt_weight);
         if (recoil_track_exists == true && recoil_track_good == true) { //independently if leading nucleon is also found in tracks
           if (tmp_track_recoil_pdg != gen_prt_pdg->at(2) ) {
             cout << "Event: " << i << " Mismatch track recoil PDG " << tmp_track_recoil_pdg << " and Generated PDG " << gen_prt_pdg->at(2) << endl;
             continue;
           }
           p_reco_tracks++;
           p_reco_tracks_weighted+= evt_weight;
           T4_recoil_p_acc.SetPxPyPzE(px,py,pz,er);
           T4_recoil_p_acc.RotateY(crossingangle);
           T4_recoil_p_acc.Boost(boost_vector);
           h1_recoil_theta_accepted->Fill(T4_recoil_p_acc.Theta() * 1000, evt_weight);
           h1_recoil_theta_p_accepted->Fill(T4_recoil_p_acc.Theta() * 1000, evt_weight);
           h1_recoil_theta_accepted_noweight->Fill(T4_recoil_p_acc.Theta() * 1000);
           T4_recoil_p_acc.Boost(-boost_irf);
        //   cout << "Recoil proton accepted 4-vector after boost " << T4_recoil_p_acc.X() << " , " << T4_recoil_p_acc.Y() << " , " << T4_recoil_p_acc.Z() << " , " << T4_recoil_p_acc.E() << " , "<< T4_recoil_p_acc.M() << endl;
           h1_recoil_p_accepted_noweight->Fill(T4_recoil_p_acc.Vect().Mag());
           h1_recoil_accepted_noweight->Fill(T4_recoil_p_acc.Vect().Mag());
           h1_recoil_p_accepted->Fill(T4_recoil_p_acc.Vect().Mag(), evt_weight);
           h1_recoil_accepted->Fill(T4_recoil_p_acc.Vect().Mag(), evt_weight);
           //Fill outttree if at least recoil is accepted!
           outtree->Fill();
           if (lead_track_exists == true && lead_track_good == true) { //found both tracks of the np pair
              np_tracks++;
              np_tracks_weighted+= evt_weight;
              np_track_exists = true;
              skim_np_pair = 1;
           }
         }
       }
    }
    if (gen_prt_pdg->at(1) == 2212) { //leading is proton
       skim_pid_lead = 2212;
       p_lead_generated++;
       p_lead_generated_weighted+= evt_weight;
       double lead_px = gen_prt_dir_x->at(1)*gen_prt_tot_mom->at(1);
       double lead_py = gen_prt_dir_y->at(1)*gen_prt_tot_mom->at(1);
       double lead_pz = gen_prt_dir_z->at(1)*gen_prt_tot_mom->at(1);
       double lead_en = gen_prt_tot_e->at(1);
       T4_lead_p_gen.SetPxPyPzE(lead_px,lead_py,lead_pz,lead_en);
       T4_lead_p_gen.RotateY(crossingangle);
       T4_lead_p_gen.Boost(boost_vector);
       h1_lead_theta_generated->Fill(T4_lead_p_gen.Theta() * 1000, evt_weight);
       h1_lead_theta_p_generated->Fill(T4_lead_p_gen.Theta() * 1000, evt_weight);
       h1_lead_theta_generated_noweight->Fill(T4_lead_p_gen.Theta() * 1000);

       //output tree info in lab frame
       skim_leading.SetPxPyPzE(lead_px,lead_py,lead_pz,lead_en);

       //Calculation of pmiss
       TLorentzVector T4_temp_lead_p_gen = T4_lead_p_gen;
       T4_temp_lead_p_gen.Boost(-boost_irf);
       //Calculation of pmiss in IRF
       TLorentzVector T4_pmiss_gen = q - T4_temp_lead_p_gen;
       double pmiss = T4_pmiss_gen.P();
       h1_pmiss_generated->Fill(pmiss, evt_weight); //all Recoils independently of type

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
       if (lead_track_exists == true && lead_track_good == true) {
          p_lead_tracks++;
          p_lead_tracks_weighted+= evt_weight;

          T4_lead_p_acc.SetPxPyPzE(lead_px,lead_py,lead_pz,lead_en);
          T4_lead_p_acc.RotateY(crossingangle);
          T4_lead_p_acc.Boost(boost_vector);
          h1_lead_theta_accepted->Fill(T4_lead_p_gen.Theta() * 1000, evt_weight);
          h1_lead_theta_p_accepted->Fill(T4_lead_p_gen.Theta() * 1000, evt_weight);
          h1_lead_theta_accepted_noweight->Fill(T4_lead_p_gen.Theta() * 1000);
          h1_pmiss_accepted->Fill(pmiss, evt_weight); //all Recoils independently of type if leading is accepted
        }


       if (gen_prt_pdg->at(2) == 2112) { //recoil is neutron
         skim_pid_recoil = 2112;
         //pn events
         pn_generated++;
         pn_generated_weighted+= evt_weight;
         n_reco_generated++;
         n_reco_generated_weighted+= evt_weight;
         h1_pmiss_n_generated->Fill(pmiss, evt_weight); //calculate pmiss for recoil neuton
         if (lead_track_exists == true && lead_track_good == true) h1_pmiss_n_accepted->Fill(pmiss, evt_weight); //calculated pmiss for recoil neuton when leading is accepted

         double px = gen_prt_dir_x->at(2)*gen_prt_tot_mom->at(2);
         double py = gen_prt_dir_y->at(2)*gen_prt_tot_mom->at(2);
         double pz = gen_prt_dir_z->at(2)*gen_prt_tot_mom->at(2);
         double er = gen_prt_tot_e->at(2);
         T4_recoil_n_gen.SetPxPyPzE(px,py,pz,er);
    //     cout << "Recoil neutron 4-vector before boost " << px << " , " << py << " , " << pz << " , " << er << " , "<< T4_recoil_n_gen.M() << endl;
         T4_recoil_n_gen.RotateY(crossingangle);
         T4_recoil_n_gen.Boost(boost_vector);
         h1_recoil_theta_generated->Fill(T4_recoil_n_gen.Theta() * 1000, evt_weight);
         h1_recoil_theta_n_generated->Fill(T4_recoil_n_gen.Theta() * 1000, evt_weight);
         h1_recoil_theta_generated_noweight->Fill(T4_recoil_n_gen.Theta() * 1000);
         T4_recoil_n_gen.Boost(-boost_irf);
      //   cout << "Recoil neutron generated 4-vector after boost " << T4_recoil_n_gen.X() << " , " << T4_recoil_n_gen.Y() << " , " << T4_recoil_n_gen.Z() << " , " << T4_recoil_n_gen.E() << " , "<< T4_recoil_n_gen.M() << endl;
         h1_recoil_n_generated_noweight->Fill(T4_recoil_n_gen.Vect().Mag());
         h1_recoil_generated_noweight->Fill(T4_recoil_n_gen.Vect().Mag());
         h1_recoil_n_generated->Fill(T4_recoil_n_gen.Vect().Mag(), evt_weight);
         h1_recoil_generated->Fill(T4_recoil_n_gen.Vect().Mag(), evt_weight);

         //output tree info in lab frame
         skim_recoil.SetPxPyPzE(px,py,pz,er);


         if (recoil_track_exists == true && recoil_track_good == true) { //independently if leading nucleon is also found in tracks
           if (tmp_track_recoil_pdg != gen_prt_pdg->at(2) ) {
             cout << "Event: " << i << " Mismatch track recoil PDG " << tmp_track_recoil_pdg << " and Generated PDG " << gen_prt_pdg->at(2) << endl;
             continue;
           }
           n_reco_tracks++;
           n_reco_tracks_weighted+= evt_weight;
           T4_recoil_n_acc.SetPxPyPzE(px,py,pz,er);
           T4_recoil_n_acc.RotateY(crossingangle);
           T4_recoil_n_acc.Boost(boost_vector);
           h1_recoil_theta_accepted->Fill(T4_recoil_n_acc.Theta() * 1000, evt_weight);
           h1_recoil_theta_n_accepted->Fill(T4_recoil_n_acc.Theta() * 1000, evt_weight);
           h1_recoil_theta_accepted_noweight->Fill(T4_recoil_n_acc.Theta() * 1000);
           T4_recoil_n_acc.Boost(-boost_irf);
      //     cout << "Recoil neutron accepted 4-vector after boost " << T4_recoil_n_acc.X() << " , " << T4_recoil_n_acc.Y() << " , " << T4_recoil_n_acc.Z() << " , " << T4_recoil_n_acc.E() << " , "<< T4_recoil_n_acc.M() << endl;
           h1_recoil_n_accepted_noweight->Fill(T4_recoil_n_acc.Vect().Mag());
           h1_recoil_accepted_noweight->Fill(T4_recoil_n_acc.Vect().Mag());
           h1_recoil_n_accepted->Fill(T4_recoil_n_acc.Vect().Mag(), evt_weight);
           h1_recoil_accepted->Fill(T4_recoil_n_acc.Vect().Mag(), evt_weight);
           //Fill outttree if at least recoil is accepted!
           outtree->Fill();
           if (lead_track_exists == true && lead_track_good == true) { //found both tracks of the pn pair
              pn_tracks++;
              pn_tracks_weighted+= evt_weight;
              pn_track_exists = true;
              skim_pn_pair = 1;
           }
         }
       }
       if (gen_prt_pdg->at(2) == 2212) { //recoil is proton
         skim_pid_recoil = 2212;
         //pp events
         pp_generated++;
         pp_generated_weighted+= evt_weight;
         p_reco_generated++;
         p_reco_generated_weighted+= evt_weight;
         h1_pmiss_p_generated->Fill(pmiss, evt_weight); //calculate pmiss for recoil proton
         if (lead_track_exists == true && lead_track_good == true) h1_pmiss_p_accepted->Fill(pmiss, evt_weight); //calculated pmiss for recoil proton when leading is accepted

         double px = gen_prt_dir_x->at(2)*gen_prt_tot_mom->at(2);
         double py = gen_prt_dir_y->at(2)*gen_prt_tot_mom->at(2);
         double pz = gen_prt_dir_z->at(2)*gen_prt_tot_mom->at(2);
         double er = gen_prt_tot_e->at(2);
         T4_recoil_p_gen.SetPxPyPzE(px,py,pz,er);
      //   cout << "Recoil proton 4-vector before boost " << px << " , " << py << " , " << pz << " , " << er << " , "<< T4_recoil_p_gen.M() << endl;
         T4_recoil_p_gen.RotateY(crossingangle);
         T4_recoil_p_gen.Boost(boost_vector);
         h1_recoil_theta_generated->Fill(T4_recoil_p_gen.Theta() * 1000, evt_weight);
         h1_recoil_theta_p_generated->Fill(T4_recoil_p_gen.Theta() * 1000, evt_weight);
         h1_recoil_theta_generated_noweight->Fill(T4_recoil_p_gen.Theta() * 1000);
         T4_recoil_p_gen.Boost(-boost_irf);
      //   cout << "Recoil proton generated 4-vector after boost " << T4_recoil_p_gen.X() << " , " << T4_recoil_p_gen.Y() << " , " << T4_recoil_p_gen.Z() << " , " << T4_recoil_p_gen.E() << " , "<< T4_recoil_p_gen.M() << endl;
         h1_recoil_p_generated_noweight->Fill(T4_recoil_p_gen.Vect().Mag());
         h1_recoil_generated_noweight->Fill(T4_recoil_p_gen.Vect().Mag());
         h1_recoil_p_generated->Fill(T4_recoil_p_gen.Vect().Mag(), evt_weight);
         h1_recoil_generated->Fill(T4_recoil_p_gen.Vect().Mag(), evt_weight);

         //output tree info in lab frame
         skim_recoil.SetPxPyPzE(px,py,pz,er);


         if (recoil_track_exists == true && recoil_track_good == true) { //independently if leading nucleon is also found in tracks
           if (tmp_track_recoil_pdg != gen_prt_pdg->at(2) ) {
             cout << "Event: " << i << " Mismatch track recoil PDG " << tmp_track_recoil_pdg << " and Generated PDG " << gen_prt_pdg->at(2) << endl;
             continue;
           }
           p_reco_tracks++;
           p_reco_tracks_weighted+= evt_weight;
           T4_recoil_p_acc.SetPxPyPzE(px,py,pz,er);
           T4_recoil_p_acc.RotateY(crossingangle);
           T4_recoil_p_acc.Boost(boost_vector);
           h1_recoil_theta_accepted->Fill(T4_recoil_p_acc.Theta() * 1000, evt_weight);
           h1_recoil_theta_p_accepted->Fill(T4_recoil_p_acc.Theta() * 1000, evt_weight);
           h1_recoil_theta_accepted_noweight->Fill(T4_recoil_p_acc.Theta() * 1000);
           T4_recoil_p_acc.Boost(-boost_irf);
      //     cout << "Recoil proton accepted 4-vector after boost " << T4_recoil_p_acc.X() << " , " << T4_recoil_p_acc.Y() << " , " << T4_recoil_p_acc.Z() << " , " << T4_recoil_p_acc.E() << " , "<< T4_recoil_p_acc.M() << endl;
           h1_recoil_p_accepted_noweight->Fill(T4_recoil_p_acc.Vect().Mag());
           h1_recoil_accepted_noweight->Fill(T4_recoil_p_acc.Vect().Mag());
           h1_recoil_p_accepted->Fill(T4_recoil_p_acc.Vect().Mag(), evt_weight);
           h1_recoil_accepted->Fill(T4_recoil_p_acc.Vect().Mag(), evt_weight);
           //Fill outttree if at least recoil is accepted!
           outtree->Fill();
           if (lead_track_exists == true && lead_track_good == true) { //found both tracks of the pp pair
              pp_tracks++;
              pp_tracks_weighted+= evt_weight;
              pp_track_exists = true;
              skim_pp_pair = 1;
           }
         }
       }
    } //end if of checking for event topologies and existing tracks



   if (fillhit) {
    //Hit fill
    for (int ihit = 0 ; ihit < hit_count ; ihit++) {
        if (hit_trk_id->at(ihit) == 2 && lead_track_exists) { //leading information
           if (trk_pdg->at(hit_trk_id->at(ihit)) == 2112) { //neutron
             string temp_name = hit_vol_name->at(ihit);
             if (temp_name.length() > 7) temp_name = temp_name.substr(0, temp_name.length() - 6);
             h1_lead_n_names->Fill(temp_name.c_str(),1);
           }
           if (trk_pdg->at(hit_trk_id->at(ihit)) == 2212) { //proton
             string temp_name = hit_vol_name->at(ihit);
             if (temp_name.length() > 7) temp_name = temp_name.substr(0, temp_name.length() - 6);
             h1_lead_p_names->Fill(temp_name.c_str(),1);
           }
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
           if (hit_vol_name->at(ihit).rfind("ffi_OFFM_TRK_lay_Phys",0) == 0 ) { //OFF Momentum
          // if (hit_vol_name->at(ihit).rfind("ffi_OFFM_TRK_lay_Phys",0) == 0 ) { //OFF Momentum Tracker 1
             if (trk_pdg->at(hit_trk_id->at(ihit)) == 2112) h2_x_y_OFF_lead_n->Fill(hit_x->at(ihit),hit_y->at(ihit));
             if (trk_pdg->at(hit_trk_id->at(ihit)) == 2212) h2_x_y_OFF_lead_p->Fill(hit_x->at(ihit),hit_y->at(ihit));
           }
           if (hit_vol_name->at(ihit).rfind("ffi_OFFM_TRK2_lay_Phys",0) == 0 ) { //OFF Momentum
          // if (hit_vol_name->at(ihit).rfind("ffi_OFFM_TRK2_lay_Phys",0) == 0 ) { //OFF Momentum Tracker 2
             if (trk_pdg->at(hit_trk_id->at(ihit)) == 2112) h2_x_y_OFF2_lead_n->Fill(hit_x->at(ihit),hit_y->at(ihit));
             if (trk_pdg->at(hit_trk_id->at(ihit)) == 2212) h2_x_y_OFF2_lead_p->Fill(hit_x->at(ihit),hit_y->at(ihit));
           }
           if (hit_vol_name->at(ihit).rfind("fi_B0_TRK_lay_Phys",0) == 0 ) { //B0 Momentum
          // if (hit_vol_name->at(ihit).rfind("fi_B0_TRK_lay_Phys",0) == 0 ) { //B0 Detector
             if (trk_pdg->at(hit_trk_id->at(ihit)) == 2112) h2_x_y_B0_lead_n->Fill(hit_x->at(ihit),hit_y->at(ihit));
             if (trk_pdg->at(hit_trk_id->at(ihit)) == 2212) h2_x_y_B0_lead_p->Fill(hit_x->at(ihit),hit_y->at(ihit));
           }
      //     if (hit_vol_name->at(ihit).rfind("fi_B0_TRK_lay_Phys",0) == 0 ) { //B0 Momentum

      //     }
        }
        if (hit_trk_id->at(ihit) == 3 && recoil_track_exists) { //recoil information

           if (trk_pdg->at(hit_trk_id->at(ihit)) == 2112) { //neutron
             h1_recoil_n_names->Fill(hit_vol_name->at(ihit).c_str(),1);
           }
           if (trk_pdg->at(hit_trk_id->at(ihit)) == 2212) { //proton
             h1_recoil_p_names->Fill(hit_vol_name->at(ihit).c_str(),1);
           }
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
           if (hit_vol_name->at(ihit).rfind("ffi_OFFM_TRK_lay_Phys",0) == 0 ) { //OFF Momentum
          // if (hit_vol_name->at(ihit).rfind("ffi_OFFM_TRK_lay_Phys",0) == 0 ) { //OFF Momentum Tracker 1
             if (trk_pdg->at(hit_trk_id->at(ihit)) == 2112) h2_x_y_OFF_reco_n->Fill(hit_x->at(ihit),hit_y->at(ihit));
             if (trk_pdg->at(hit_trk_id->at(ihit)) == 2212) h2_x_y_OFF_reco_p->Fill(hit_x->at(ihit),hit_y->at(ihit));
           }
           if (hit_vol_name->at(ihit).rfind("ffi_OFFM_TRK2_lay_Phys",0) == 0 ) { //OFF Momentum
          // if (hit_vol_name->at(ihit).rfind("ffi_OFFM_TRK2_lay_Phys",0) == 0 ) { //OFF Momentum Tracker 2
             if (trk_pdg->at(hit_trk_id->at(ihit)) == 2112) h2_x_y_OFF2_reco_n->Fill(hit_x->at(ihit),hit_y->at(ihit));
             if (trk_pdg->at(hit_trk_id->at(ihit)) == 2212) h2_x_y_OFF2_reco_p->Fill(hit_x->at(ihit),hit_y->at(ihit));
           }
           if (hit_vol_name->at(ihit).rfind("fi_B0_TRK_lay_Phys",0) == 0 ) { //B0 Momentum
          // if (hit_vol_name->at(ihit).rfind("fi_B0_TRK_lay_Phys",0) == 0 ) { //B0 Detector
             if (trk_pdg->at(hit_trk_id->at(ihit)) == 2112) h2_x_y_B0_reco_n->Fill(hit_x->at(ihit),hit_y->at(ihit));
             if (trk_pdg->at(hit_trk_id->at(ihit)) == 2212) h2_x_y_B0_reco_p->Fill(hit_x->at(ihit),hit_y->at(ihit));
           }
        }
     }
   } //if hit fill

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

  double pp_weighted_ratio = pp_tracks_weighted / (double)pp_generated_weighted;
  double pn_weighted_ratio = pn_tracks_weighted / (double)pn_generated_weighted;
  double np_weighted_ratio = np_tracks_weighted / (double)np_generated_weighted;
  double nn_weighted_ratio = nn_tracks_weighted / (double)nn_generated_weighted;

  double p_lead_ratio = p_lead_tracks / (double)p_lead_generated;
  double n_lead_ratio = n_lead_tracks / (double)n_lead_generated;
  double p_reco_ratio = p_reco_tracks / (double)p_reco_generated;
  double n_reco_ratio = n_reco_tracks / (double)n_reco_generated;

  double p_lead_weighted_ratio = p_lead_tracks_weighted / p_lead_generated_weighted;
  double n_lead_weighted_ratio = n_lead_tracks_weighted / n_lead_generated_weighted;
  double p_reco_weighted_ratio = p_reco_tracks_weighted / p_reco_generated_weighted;
  double n_reco_weighted_ratio = n_reco_tracks_weighted / n_reco_generated_weighted;


  cout << " Generated pp events " << pp_generated;
  cout << " and #events with pp tracks " << pp_tracks << " = " << pp_ratio*100;
  cout << " and with weights ratio = " << pp_weighted_ratio*100 << endl;
  cout << " Generated pn events " << pn_generated;
  cout << " and #events with pn tracks " << pn_tracks << " = " << pn_ratio*100;
  cout << " and with weights ratio = " << pn_weighted_ratio*100 << endl;
  cout << " Generated nn events " << nn_generated;
  cout << " and #events with nn tracks " << nn_tracks << " = " << nn_ratio*100 ;
  cout << " and with weights ratio = " << nn_weighted_ratio*100 << endl;
  cout << " Generated np events " << np_generated;
  cout << " and #events with np tracks " << np_tracks << " = " << np_ratio*100;
  cout << " and with weights ratio = " << np_weighted_ratio*100 << endl;
  cout << " Generated Leading protons "  << p_lead_generated;
  cout << " and #events with lead p tracks " << p_lead_tracks << " = " << p_lead_ratio*100;
  cout << " and with weights ratio = " << p_lead_weighted_ratio*100 << endl;
  cout << " Generated Leading neutrons " << n_lead_generated ;
  cout << " and #events with lead n tracks " << n_lead_tracks << " = " << n_lead_ratio*100;
  cout << " and with weights ratio = " << n_lead_weighted_ratio*100 << endl;
  cout << " Generated Recoil protons "   << p_reco_generated;
  cout << " and #events with recoil p tracks " << p_reco_tracks << " = " << p_reco_ratio*100;
  cout << " and with weights ratio = " << p_reco_weighted_ratio*100 << endl;
  cout << " Generated Recoil neutrons "  << n_reco_generated;
  cout << " and #events with recoil n tracks " << n_reco_tracks << " = " << n_reco_ratio*100;
  cout << " and with weights ratio = " << n_reco_weighted_ratio*100 << endl;

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
/*TCanvas *c6 = new TCanvas("c6","acceptances ");
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
*/

  output->cd();
  h1_hits_count->Write();
  h1_trk_count->Write();
  h1_gen_count->Write();

  h1_Q2->Write();
  h1_x->Write();
  h2_Q2_x->Write();

  h1_recoil_p_names->Write();
  h1_recoil_n_names->Write();
  h1_lead_p_names->Write();
  h1_lead_n_names->Write();
//  h1_recoil_p_names->LabelsOption("h","X");
//  h1_recoil_n_names->LabelsOption("h","X");
//  h1_lead_n_names->LabelsOption("h","X");
//  h1_lead_p_names->LabelsOption("h","X");

/*  h2_x_y_ZDC_np_lead->Write();
  h2_x_y_ZDC_np_reco->Write();
  h2_x_y_ZDC_pn_lead->Write();
  h2_x_y_ZDC_pn_reco->Write();
  h2_x_y_ZDC_nn_lead->Write();
  h2_x_y_ZDC_nn_reco->Write();
  h2_x_y_ZDC_pp_lead->Write();
  h2_x_y_ZDC_pp_reco->Write();*/
  h2_x_y_ZDC_lead_n->Write();
  h2_x_y_ZDC_lead_p->Write();
  h2_x_y_ZDC_reco_n->Write();
  h2_x_y_ZDC_reco_p->Write();

  /*h2_x_y_Roman1_np_lead->Write();
  h2_x_y_Roman1_np_reco->Write();
  h2_x_y_Roman1_pn_lead->Write();
  h2_x_y_Roman1_pn_reco->Write();
  h2_x_y_Roman1_nn_lead->Write();
  h2_x_y_Roman1_nn_reco->Write();
  h2_x_y_Roman1_pp_lead->Write();
  h2_x_y_Roman1_pp_reco->Write();*/
  h2_x_y_Roman1_lead_p->Write();
  h2_x_y_Roman1_lead_n->Write();
  h2_x_y_Roman1_reco_p->Write();
  h2_x_y_Roman1_reco_n->Write();

//BO histos
  h2_x_y_B0_lead_p->Write();
  h2_x_y_B0_lead_n->Write();
  h2_x_y_B0_reco_p->Write();
  h2_x_y_B0_reco_n->Write();

  //OFF momentum histos (2 detectors)
  h2_x_y_OFF_lead_p->Write();
  h2_x_y_OFF_lead_n->Write();
  h2_x_y_OFF_reco_p->Write();
  h2_x_y_OFF_reco_n->Write();
  h2_x_y_OFF2_lead_p->Write();
  h2_x_y_OFF2_lead_n->Write();
  h2_x_y_OFF2_reco_p->Write();
  h2_x_y_OFF2_reco_n->Write();
  /*h2_x_y_Roman2_np->Write();
  h2_x_y_Roman2_pn->Write();
  h2_x_y_Roman3_np->Write();
  h2_x_y_Roman3_pn->Write();
  h2_x_y_Roman4_np->Write();
  h2_x_y_Roman4_pn->Write();*/
//Pmiss histos
  h1_pmiss_generated->Write();
  h1_pmiss_n_generated->Write();
  h1_pmiss_p_generated->Write();
  h1_pmiss_accepted->Write();
  h1_pmiss_n_accepted->Write();
  h1_pmiss_p_accepted->Write();
//Recoil momentum in IRF
  h1_recoil_generated->Write();
  h1_recoil_n_generated->Write();
  h1_recoil_p_generated->Write();
  h1_recoil_accepted->Write();
  h1_recoil_n_accepted->Write();
  h1_recoil_p_accepted->Write();
  h1_recoil_generated_noweight->Write();
  h1_recoil_n_generated_noweight->Write();
  h1_recoil_p_generated_noweight->Write();
  h1_recoil_accepted_noweight->Write();
  h1_recoil_n_accepted_noweight->Write();
  h1_recoil_p_accepted_noweight->Write();

//Theta distribution Recoil and Leading
  h1_recoil_theta_generated->Write();
  h1_recoil_theta_n_generated->Write();
  h1_recoil_theta_p_generated->Write();
  h1_recoil_theta_accepted->Write();
  h1_recoil_theta_n_accepted->Write();
  h1_recoil_theta_p_accepted->Write();
  h1_lead_theta_generated->Write();
  h1_lead_theta_n_generated->Write();
  h1_lead_theta_p_generated->Write();
  h1_lead_theta_accepted->Write();
  h1_lead_theta_n_accepted->Write();
  h1_lead_theta_p_accepted->Write();
  h1_recoil_theta_accepted_noweight->Write();
  h1_recoil_theta_generated_noweight->Write();
  h1_lead_theta_accepted_noweight->Write();
  h1_lead_theta_generated_noweight->Write();

  outtree->Write();
  output->Close();

}
