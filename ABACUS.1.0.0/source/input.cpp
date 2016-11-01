//==========================================================
// Author: Lixin He,mohan
// DATE : 2008-11-6
//==========================================================
//#include "global.h"
#include "src_pw/tools.h"
#include "input.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <stdio.h>
#include <string.h>

Input INPUT;

Input::Input() 
{
// all values set in Default	
}
Input::~Input() {}

void Input::Init(const string &fn)
{
	timer::tick("Input","Init",'B');
    this->Default();

    bool success = this->Read(fn);

//xiaohui add 2015-09-16
#ifdef __MPI
	Parallel_Common::bcast_bool(input_error);
#endif
	if(input_error ==1 )
	{
		WARNING_QUIT("Input","Bad parameter, please check the input parameters in file INPUT");
	}

#ifdef __MPI
	Parallel_Common::bcast_bool(success);
#endif
	if(!success)
	{
		WARNING_QUIT("Input::Init","Error during readin parameters.");
	}
#ifdef __MPI
    Bcast();
#endif

	// mohan move forward 2011-02-26
//----------------------------------------------------------
// OTHRE CLASS MEMBER FUNCTION :
// NAME : Run::make_dir( dir name : OUT.suffix)
//----------------------------------------------------------
    //Global_File::make_dir_out( this->suffix , this->calculation, MY_RANK, this->linear_scaling, this->out_alllog); xiaohui modify 2013-09-01, delete "this->linear_scaling"
	Global_File::make_dir_out( this->suffix , this->calculation, MY_RANK, this->out_alllog); //xiaohui add 2013-09-01
	Check();

	time_t  time_now = time(NULL);
	//xiaohui modify 2015-03-25
	/*
	ofs_running << " ------------------------------------------------------------------------------------" << endl;
	ofs_running << "                                                                                     " << endl;
	ofs_running << "                                              ############                           " << endl; 
	ofs_running << "                                            ##################                       " << endl;
	ofs_running << "                                          #####     ####  #####                      " << endl;         
	ofs_running << "                                         ###        ##########                       " << endl;         
	ofs_running << "   #####            #####   ##############            ###### ###   ############      " << endl;         
	ofs_running << "  ########       ######### #################         ####    ####  ################  " << endl;         
	ofs_running << "  ###########  ###### #### ####       #######       ####     ####             ###### " << endl;         
	ofs_running << "  ####   ##########   #### ####        ######      ####      ####    #########  #### " << endl;          
	ofs_running << "  ####      ####      #### ####  ###########      ####       ####  ###########  #### " << endl;         
	ofs_running << "  ####                #### #####  #####          ####        ####  ####         #### " << endl;         
	ofs_running << "  ####                ####  ################    ####         ####  ################# " << endl;         
	ofs_running << "  ####                ####    ##############   ####          ####    ############### " << endl;         
	ofs_running << "                                             #####                                   " << endl;         
	ofs_running << "                                          ######                                     " << endl;         
	ofs_running << "                                        ####                                         " << endl; 
	ofs_running << "                                                                                     " << endl;
	ofs_running << " ------------------------------------------------------------------------------------" << endl;

	ofs_running << "                                                                                     " << endl;
	ofs_running << "                             WELCOME TO MESIA                                        " << endl;
	ofs_running << "                                                                                     " << endl;
    	ofs_running << "   'Massive Electronic simulation based on Systematically Improvable Atomic bases'   " << endl; 
	ofs_running << "                                                                                     " << endl;
	*/
	ofs_running << "                                                                                     " << endl;
	ofs_running << "                             WELCOME TO ABACUS                                       " << endl;
	ofs_running << "                                                                                     " << endl;
    	ofs_running << "               'Atomic-orbital Based Ab-initio Computation at UStc'                  " << endl; 
	ofs_running << "                                                                                     " << endl;

	ofs_running << setiosflags(ios::right);
                                                                                                                             


#ifdef __MPI
	ofs_running << "    Version: Parallel, under ALPHA test" << endl;
	ofs_running << "    Processor Number is " << NPROC << endl;
	TITLE("Input","init");
	TITLE("Input","Bcast");
#else
	ofs_running << "    This is SERIES version." << endl;
	TITLE("Input","init");
#endif
    	ofs_running << "    Start Time is " << ctime(&time_now);
	ofs_running << "                                                                                     " << endl;
	ofs_running << " ------------------------------------------------------------------------------------" << endl;

	ofs_running << setiosflags(ios::left);
	cout << setiosflags(ios::left);

	ofs_running << "\n READING GENERAL INFORMATION" << endl;
	OUT(ofs_running,"global_out_dir", global_out_dir);
	OUT(ofs_running,"global_in_card", global_in_card);
	OUT(ofs_running,"pseudo_dir", global_pseudo_dir);

	OUT(ofs_running,"pseudo_type", pseudo_type); // mohan add 2013-05-20 (xiaohui add 2013-06-23, global_pseudo_type -> pseudo_type)

	timer::tick("Input","Init",'B');
    return;
}

void Input::Default(void)
{
    TITLE("Input","Default");
//----------------------------------------------------------
// main parameters
//----------------------------------------------------------
	//xiaohui modify 2015-03-25
    //suffix = "MESIA";
    suffix = "ABACUS";
    atom_file = "";//xiaohui modify 2015-02-01
    kpoint_file = "";//xiaohui modify 2015-02-01
    pseudo_dir = "";
    pseudo_type = "auto"; // mohan add 2013-05-20 (xiaohui add 2013-06-23)
	wannier_card = "";
    epm_pseudo_card = "";
    latname = "test";
    //xiaohui modify 2015-09-15, relax -> scf
    //calculation = "relax";
    calculation = "scf";
    ntype = 0;
    nbands = 0;
	nbands_istate = 5;
	npool = 1;
    epm_spin_orbital = 0;
    berry_phase = false;

    efield = 0;
	edir = 1;
	emaxpos = 0.5;
	eopreg = 0.1;
	eamp = 0.001; // (a.u. = 51.44 * 10^10 V/m )

	bfield = 0;
	bfield_teslax = 0.0;
	bfield_teslay = 0.0;
	bfield_teslaz = 0.0;
	bfield_gauge_x = 0.0;
	bfield_gauge_y = 0.0;
	bfield_gauge_z = 0.0;
	
	opt_epsilon2 = false;//mohan add 2010-03-24
	opt_nbands = 0;
    lda_plus_u = false;
//----------------------------------------------------------
// electrons / spin
//----------------------------------------------------------
	dft_functional = "none";
    nspin = 1;
    nelec = 0.0;
    lmaxmax = 2;
    tot_magnetization = 0.0;
//----------------------------------------------------------
// new function
//----------------------------------------------------------
    //local_basis=0; xiaohui modify 2013-09-01
    //linear_scaling=false; xiaohui modify 2013-09-01
	basis_type = "pw"; //xiaohui add 2013-09-01
	ks_solver = "default"; //xiaohui add 2013-09-01 
    search_radius=-1.0; // unit: a.u. -1.0 has no meaning.
    search_pbc=true;
    sparse_matrix=false;
	atom_distribution=0;
    symmetry=false;
	mlwf_flag=false;
	vna = 0;
	grid_speed=1; //mohan add 2012-03-29
    force=0;
    force_set=0;
    force_thr=1.0e-3;
	force_thr_ev2=0;
	stress=false;
	ion_dynamics="cg"; // pengfei  2014-10-13
    cg_threshold=0.5; // pengfei add 2013-08-15
	out_level="ie";
	bfgs_w1 = 0.01;		// mohan add 2011-03-13
	bfgs_w2 = 0.5;
	trust_radius_max = 0.8; // bohr
	trust_radius_min = 1e-5;
	trust_radius_ini = 0.5; //bohr
//----------------------------------------------------------
// ecutwfc
//----------------------------------------------------------
    //gamma_only = false;
    gamma_only = false;
	gamma_only_local = false;
    ecutwfc = 0.0;
    ecutrho = 0.0;
    ncx = 0;
    ncy = 0;
    ncz = 0;
    nx = 0;
    ny = 0;
    nz = 0;
	bx = 2;
	by = 2;
	bz = 2;
//----------------------------------------------------------
// diagonalization
//----------------------------------------------------------
    //diago_type = "default"; xiaohui modify 2013-09-01 //mohan update 2012-02-06
	diago_proc = 0; //if 0, then diago_proc = NPROC
    diago_cg_maxiter = 50;
	diago_cg_prec=1; //mohan add 2012-03-31
    diago_david_ndim = 10;
    ethr = 1.0e-2;
	nb2d = 0;
	nurse = 0;
	colour = 0;
	t_in_h = 1;
	vl_in_h = 1;
	vnl_in_h = 1;
	zeeman_in_h = 1;
	test_force = 0;
    fs_ref_energy  = 0.0;
//----------------------------------------------------------
// iteration
//----------------------------------------------------------
    dr2 = 1.0e-9;
    niter = 40;
    nstep = 1;
	out_stru = 0;
//----------------------------------------------------------
// occupation
//----------------------------------------------------------
    occupations = "smearing";  //pengfei 2014-10-13
    smearing = "fixed";
    degauss = 0.01;
//----------------------------------------------------------
//  charge mixing
//----------------------------------------------------------
    mixing_mode = "pulay";
    mixing_beta = 0.7;
    mixing_ndim = 8;
	mixing_gg0 = 0.00; // used in kerker method. mohan add 2014-09-27
//----------------------------------------------------------
// potential / charge / wavefunction / energy
//----------------------------------------------------------
    restart_mode = "new";
    start_wfc = "atomic";
	mem_saver = 0;
	printe = 100; // must > 0
    start_pot = "atomic";
	charge_extrap = "atomic";//xiaohui modify 2015-02-01
    out_charge = 0;
	out_dm = 0;
    out_potential = 0;
    out_wf = false;
	out_dos = 0;
        out_band = 0;
	out_hs = 0;
	out_lowf = false;
	out_alllog = false;
	dos_emin_ev = -15;//(ev)
	dos_emax_ev = 15;//(ev)
	dos_edelta_ev = 0.01;//(ev)
        b_coef = 0.07;
//----------------------------------------------------------
// LCAO 
//----------------------------------------------------------
	lcao_ecut = 0; // (Ry)
	lcao_dk = 0.01;
	lcao_dr = 0.01;
	lcao_rmax = 30; // (a.u.)
//----------------------------------------------------------
// Divide and Conqure
//----------------------------------------------------------
	dc_nx = 1;
	dc_ny = 1;
	dc_nz = 1;
//----------------------------------------------------------
// Selinv 
//----------------------------------------------------------
	selinv_npole = 40;
	selinv_temp = 2000;
	selinv_gap = 0.0;
	selinv_deltae = 2.0;
	selinv_mu = -1.0;
	selinv_threshold = 1.0e-3;
	selinv_niter = 50;
//----------------------------------------------------------
// Molecular Dynamics 
//----------------------------------------------------------
/*
	md_dt=20.0; //unit is 1 a.u., which is 4.8378*10e-17 s
	md_restart=0; 
	md_tolv=100.0;
	md_thermostat="not_controlled"; //"rescaling","rescale-v","rescale-t","reduce-t"...
	md_temp0=300; //kelvin
	md_tstep=1; //reduec md_delt every md_tstep step.
	md_delt=1.0;
*/
//md and related parameters(added by zheng da ye)
        md_mdtype=1;
        md_tauthermo=0;
        md_taubaro=0;
        md_dt=-1;
        md_nresn=3;
        md_nyosh=3;
        md_qmass=1;
        md_tfirst=0;         //kelvin
        md_tlast=md_tfirst;
        md_dumpmdfred=1;
        md_mdoutpath="mdoutput";
	md_domsd=1;
        md_domsdatom=1;
        md_rstmd=0;
        md_outputstressperiod=1;
        md_fixtemperature=1;
        md_ediff=1e-4;
	md_ediffg=1e-3;
        md_msdstartTime=1;
//end of zhengdaye's add.

//----------------------------------------------------------
// vdwD2									//Peize Lin add 2014-03-31, update 2015-09-30
//----------------------------------------------------------
	vdwD2=false;
	vdwD2_scaling=0.75;
	vdwD2_d=20;
	vdwD2_C6_file="default";
	vdwD2_C6_unit="Jnm6/mol";
	vdwD2_R0_file="default";
	vdwD2_R0_unit="A";
	vdwD2_model="radius";
	vdwD2_period[0]= vdwD2_period[1]= vdwD2_period[2]= 3;
	vdwD2_radius=30.0/BOHR_TO_A;
	vdwD2_radius_unit="Bohr";

	//xiaohui add 2015-09-16
	input_error = 0;
	
    return;
}

bool Input::Read(const string &fn)
{
    TITLE("Input","Read");

    if (MY_RANK!=0) return false;

    ifstream ifs(fn.c_str(), ios::in);	// "in_datas/input_parameters"

    if (!ifs) 
	{
		cout << " Can't find the INPUT file." << endl;
		return false;
	}

    ifs.clear();
    ifs.seekg(0);

    char word[80];
    char word1[80];
    int ierr = 0;

    //ifs >> setiosflags(ios::uppercase);
    ifs.rdstate();
    while (ifs.good())
    {
        ifs >> word;
        ifs.ignore(150, '\n');
        if (strcmp(word , "INPUT_PARAMETERS") == 0)
        {
            ierr = 1;
            break;
        }
        ifs.rdstate();
    }

    if (ierr == 0)
    {
		cout << " Error parameter list." << endl;
		return false;// return error : false
    }

    ifs.rdstate();

    while (ifs.good())
    {
        ifs >> word1;
        strtolower(word1, word);

//----------------------------------------------------------
// main parameters
//----------------------------------------------------------
        if (strcmp("suffix", word) == 0)// out dir
        {
            read_value(ifs, suffix);
        }
        else if (strcmp("atom_file", word) == 0)//xiaohui modify 2015-02-01
        {
            read_value(ifs, atom_file);//xiaohui modify 2015-02-01
        }
        else if (strcmp("pseudo_dir", word) == 0)
        {
            read_value(ifs, pseudo_dir);
        }
		else if (strcmp("pseudo_type", word) == 0) // mohan add 2013-05-20 (xiaohui add 2013-06-23)
		{
			read_value(ifs, pseudo_type);
		}
        else if (strcmp("kpoint_file", word) == 0)//xiaohui modify 2015-02-01
        {
            read_value(ifs, kpoint_file);//xiaohui modify 2015-02-01
        }
		else if (strcmp("wannier_card", word) == 0) //mohan add 2009-12-25
		{
			read_value(ifs, wannier_card);
		}
        else if (strcmp("epm_pseudo_card",word) == 0 )
        {
            read_value(ifs, epm_pseudo_card);
        }
        else if (strcmp("latname", word) == 0)// which material
        {
            read_value(ifs, latname);
        }
        else if (strcmp("calculation", word) == 0)// which type calculation
        {
            read_value(ifs, calculation);
        }
        else if (strcmp("ntype", word) == 0)// number of atom types
        {
            read_value(ifs, ntype);
        }
        else if (strcmp("nbands", word) == 0)// number of atom bands
        {
            read_value(ifs, nbands);
        }
        else if (strcmp("nbands_istate", word) == 0)// number of atom bands
        {
            read_value(ifs, nbands_istate);
        }
        else if (strcmp("npool", word) == 0)// number of pools
        {
            read_value(ifs, npool);
        }
        else if (strcmp("epm_spin_orbital", word) == 0)
        {
            read_value(ifs, epm_spin_orbital);
        }
        else if (strcmp("epm_zeeman", word) == 0)
        {
            read_value(ifs, epm_zeeman);
        }
        else if (strcmp("epm_mag_field_x", word) == 0)
        {
            read_value(ifs, epm_mag_field_x);
        }
        else if (strcmp("epm_mag_field_y", word) == 0)
        {
            read_value(ifs, epm_mag_field_y);
        }
        else if (strcmp("epm_mag_field_z", word) == 0)
        {
            read_value(ifs, epm_mag_field_z);
        }
        else if (strcmp("berry_phase", word) == 0)// berry phase calculation
        {
            read_value(ifs, berry_phase);
        }
        else if (strcmp("efield", word) == 0)// electrical field
        {
            read_value(ifs, efield);
        }
        else if (strcmp("edir", word) == 0)// electrical field direction
        {
            read_value(ifs, edir);
        }
        else if (strcmp("emaxpos", word) == 0)// electrical field maximal field
        {
            read_value(ifs, emaxpos);
        }
        else if (strcmp("eopreg", word) == 0)// amplitute of the inverse region
        {
            read_value(ifs, eopreg);
        }
        else if (strcmp("eamp", word) == 0)// electrical field amplitute
        {
            read_value(ifs, eamp);
        }
        else if (strcmp("bfield", word) == 0)// magnetic B field
        {
            read_value(ifs, bfield);
        }
        else if (strcmp("bfield_teslax", word) == 0)// magnetic Bx field
        {
            read_value(ifs, bfield_teslax);
        }
        else if (strcmp("bfield_teslay", word) == 0)// magnetic By field
        {
            read_value(ifs, bfield_teslay);
        }
        else if (strcmp("bfield_teslaz", word) == 0)// magnetic Bz field
        {
            read_value(ifs, bfield_teslaz);
        }
        else if (strcmp("bfield_gauge_x", word) == 0)// origin of magnetic Bz field
        {
            read_value(ifs, bfield_gauge_x);
        }
        else if (strcmp("bfield_gauge_y", word) == 0)// origin of magnetic Bz field
        {
            read_value(ifs, bfield_gauge_y);
        }
        else if (strcmp("bfield_gauge_z", word) == 0)// origin of magnetic Bz field
        {
            read_value(ifs, bfield_gauge_z);
        }
        else if (strcmp("opt_epsilon2", word) == 0)// optical field
        {
            read_value(ifs, opt_epsilon2);
        }
        else if (strcmp("opt_nbands", word) == 0)// bands for optical calculations
        {
            read_value(ifs, opt_nbands);
        }
        else if (strcmp("lda_plus_u", word) == 0)// lda + u
        {
            read_value(ifs, lda_plus_u);
        }
//----------------------------------------------------------
// electrons / spin
//----------------------------------------------------------
        else if (strcmp("dft_functional", word) == 0)
        {
            read_value(ifs, dft_functional);
        }
        else if (strcmp("nspin", word) == 0)
        {
            read_value(ifs, nspin);
        }
        else if (strcmp("nelec", word) == 0)
        {
            read_value(ifs, nelec);
        }
        else if (strcmp("lmaxmax", word) == 0)
        {
            read_value(ifs, lmaxmax);
        }

        else if (strcmp("tot_magnetization", word) == 0)
        {
            read_value(ifs, tot_magnetization);
        }
//----------------------------------------------------------
// new function
//----------------------------------------------------------
        //else if (strcmp("local_basis", word) == 0)
        //{
        //    read_value(ifs, local_basis);
        //} xiaohui modify 2013-09-01
		else if (strcmp("basis_type", word) == 0)
		{
			read_value(ifs, basis_type);
		} //xiaohui add 2013-09-01
        //else if (strcmp("linear_scaling", word) == 0)
        //{
        //    read_value(ifs, linear_scaling);
        //} xiaohui modify 2013-09-01
		else if (strcmp("ks_solver", word) == 0)
		{
			read_value(ifs, ks_solver);
		} //xiaohui add 2013-09-01
        else if (strcmp("search_radius", word) == 0)
        {
            read_value(ifs, search_radius);
        }
        else if (strcmp("search_pbc", word) == 0)
        {
            read_value(ifs, search_pbc);
        }
        else if (strcmp("sparse_matrix", word) == 0)
        {
            read_value(ifs, sparse_matrix);
        }
        else if (strcmp("atom_distribution", word) == 0)
        {
            read_value(ifs, atom_distribution);
        }
        else if (strcmp("symmetry", word) == 0)
        {
            read_value(ifs, symmetry);
        }
        else if (strcmp("mlwf_flag", word) == 0)
        {
            read_value(ifs, mlwf_flag);
        }
        else if (strcmp("vna", word) == 0)
        {
            read_value(ifs, vna);
        }
        else if (strcmp("grid_speed", word) == 0)//mohan 2012-03-29
        {
            read_value(ifs, grid_speed);
        }
        /*else if (strcmp("force", word) == 0)
        {
            read_value(ifs, force);
        }*/
        else if (strcmp("force_set", word) == 0)
        {
            read_value(ifs, force_set);
        }
        else if (strcmp("force_thr", word) == 0)
        {
            read_value(ifs, force_thr);
        }
        else if (strcmp("force_thr_ev", word) == 0)
        {
            read_value(ifs, force_thr);
			force_thr = force_thr / 13.6058 * 0.529177;
        }
        else if (strcmp("force_thr_ev2", word) == 0)
        {
            read_value(ifs, force_thr_ev2);
        }
        else if (strcmp("stress", word) == 0)
        {
            read_value(ifs, stress);
        }
		else if (strcmp("move_method", word) == 0)
        {
            read_value(ifs, ion_dynamics);
        }
        else if (strcmp("cg_threshold",word) == 0) // pengfei add 2013-08-15
        {
            read_value(ifs, cg_threshold);
        }
        else if (strcmp("out_level", word) == 0)
        {
            read_value(ifs, out_level);
        }
        else if (strcmp("bfgs_w1", word) == 0)
        {
            read_value(ifs, bfgs_w1);
        }
        else if (strcmp("bfgs_w2", word) == 0)
        {
            read_value(ifs, bfgs_w2);
        }
        else if (strcmp("trust_radius_max", word) == 0)
        {
            read_value(ifs, trust_radius_max);
        }
        else if (strcmp("trust_radius_min", word) == 0)
        {
            read_value(ifs, trust_radius_min);
        }
        else if (strcmp("trust_radius_ini", word) == 0)
        {
            read_value(ifs, trust_radius_ini);
        }
//		else if (strcmp("gauss_pao_flag", word) == 0)
//		else if (strcmp("gauss_pao_flag", word) == 0)
//		else if (strcmp("gauss_pao_flag", word) == 0)
//		{
//			read_value(ifs, gauss_PAO_flag);
//		}
//----------------------------------------------------------
// plane waves
//----------------------------------------------------------
        else if (strcmp("gamma_only", word) == 0)
        {
            read_value(ifs, gamma_only);
        }
        else if (strcmp("ecutwfc", word) == 0)
        {
            read_value(ifs, ecutwfc);
			ecutrho = 4.0 * ecutwfc;
        }
        else if (strcmp("nx", word) == 0)
        {
            read_value(ifs, nx);
			ncx = nx;
        }
        else if (strcmp("ny", word) == 0)
        {
            read_value(ifs, ny);
			ncy = ny;
        }
        else if (strcmp("nz", word) == 0)
        {
            read_value(ifs, nz);
			ncz = nz;
        }
        else if (strcmp("bx", word) == 0)
        {
            read_value(ifs, bx);
        }
        else if (strcmp("by", word) == 0)
        {
            read_value(ifs, by);
        }
        else if (strcmp("bz", word) == 0)
        {
            read_value(ifs, bz);
        }
//----------------------------------------------------------
// diagonalization
//----------------------------------------------------------
        //else if (strcmp("diago_type", word) == 0)
        //{
        //    read_value(ifs, diago_type);
        //} xiaohui modify 2013-09-01
        else if (strcmp("diago_proc", word) == 0)
        {
            read_value(ifs, diago_proc);
        }
        else if (strcmp("diago_cg_maxiter", word) == 0)
        {
            read_value(ifs, diago_cg_maxiter);
        }
        else if (strcmp("diago_cg_prec", word) == 0)//mohan add 2012-03-31
        {
            read_value(ifs, diago_cg_prec);
        }
        else if (strcmp("diago_david_ndim", word) == 0)
        {
            read_value(ifs, diago_david_ndim);
        }
        else if (strcmp("ethr", word) == 0)
        {
            read_value(ifs, ethr);
        }
        else if (strcmp("nb2d", word) == 0)
        {
            read_value(ifs, nb2d);
        }
        else if (strcmp("nurse", word) == 0)
        {
            read_value(ifs, nurse);
        }
        else if (strcmp("colour", word) == 0)
        {
            read_value(ifs, colour);
        }
        else if (strcmp("t_in_h", word) == 0)
        {
            read_value(ifs, t_in_h);
        }
        else if (strcmp("vl_in_h", word) == 0)
        {
            read_value(ifs, vl_in_h);
        }
        else if (strcmp("vnl_in_h", word) == 0)
        {
            read_value(ifs, vnl_in_h);
        }
        else if (strcmp("zeeman_in_h", word) == 0)
        {
            read_value(ifs, zeeman_in_h);
        }
        else if (strcmp("test_force", word) == 0)
        {
            read_value(ifs, test_force);
        }
        else if (strcmp("fs_ref_energy_ev", word) == 0)
        {
            read_value(ifs, fs_ref_energy);
			fs_ref_energy = fs_ref_energy / Ry_to_eV; 
        }
        else if (strcmp("fs_ref_energy_ry", word) == 0)
        {
            read_value(ifs, fs_ref_energy);
        }
//----------------------------------------------------------
// iteration
//----------------------------------------------------------
        else if (strcmp("dr2", word) == 0)
        {
            read_value(ifs, dr2);
        }
        else if (strcmp("niter", word) == 0)
        {
            read_value(ifs, niter);
        }
        else if (strcmp("nstep", word) == 0)
        {
            read_value(ifs, nstep);
        }
        else if (strcmp("out_stru", word) == 0)
        {
            read_value(ifs, out_stru);
        }
//----------------------------------------------------------
// occupation
//----------------------------------------------------------
        //else if (strcmp("occupations", word) == 0)
        //{
        //    read_value(ifs, occupations);
        //}
        else if (strcmp("smearing", word) == 0)
        {
            read_value(ifs, smearing);
        }
        else if (strcmp("sigma", word) == 0)
        {
            read_value(ifs, degauss);
        }
        else if (strcmp("degauss_temp", word) == 0)
        {
			double degauss_temp;
            read_value(ifs, degauss_temp);
			degauss = degauss_temp * 3.166815e-6;
        }
//----------------------------------------------------------
// charge mixing
//----------------------------------------------------------
        else if (strcmp("mixing_type", word) == 0)
        {
            read_value(ifs, mixing_mode);
//2015-06-15, xiaohui
            if(mixing_mode == "pulay-kerker")
            {
                  mixing_gg0 = 1.5;
            }
        }
        else if (strcmp("mixing_beta", word) == 0)
        {
            read_value(ifs, mixing_beta);
        }
        else if  (strcmp("mixing_ndim", word) == 0)
        {
            read_value(ifs, mixing_ndim);
        }
        else if  (strcmp("mixing_gg0", word) == 0) //mohan add 2014-09-27
        {
            read_value(ifs, mixing_gg0);
        }
//----------------------------------------------------------
// charge / potential / wavefunction
//----------------------------------------------------------
        else if (strcmp("restart_mode", word) == 0)
        {
            read_value(ifs, restart_mode);
        }
        else if (strcmp("start_wfc", word) == 0)
        {
            read_value(ifs, start_wfc);
        }
        else if (strcmp("mem_saver", word) == 0)
        {
            read_value(ifs, mem_saver);
        }
        else if (strcmp("printe", word) == 0)
        {
            read_value(ifs, printe);
        }
        else if (strcmp("start_charge", word) == 0)
        {
            read_value(ifs, start_pot);
        }
        else if (strcmp("charge_extrap", word) == 0)//xiaohui modify 2015-02-01
        {
            read_value(ifs, charge_extrap);//xiaohui modify 2015-02-01
        }
        else if (strcmp("out_charge", word) == 0)
        {
            read_value(ifs, out_charge);
        }
        else if (strcmp("out_dm", word) == 0)
        {
            read_value(ifs, out_dm);
        }
        else if (strcmp("out_potential", word) == 0)
        {
            read_value(ifs, out_potential);
        }
        else if (strcmp("out_wf", word) == 0)
        {
            read_value(ifs, out_wf);
        }
		//mohan add 20090909
        else if (strcmp("out_dos", word) == 0)
        {
            read_value(ifs, out_dos);
        }
        else if (strcmp("out_band", word) == 0)
        {
            read_value(ifs, out_band);
        }
        
        else if (strcmp("out_hs", word) == 0)
        {
            read_value(ifs, out_hs);
        }
        else if (strcmp("out_lowf", word) == 0)
        {
            read_value(ifs, out_lowf);
        }
        else if (strcmp("out_alllog", word) == 0)
        {
            read_value(ifs, out_alllog);
        }
        else if (strcmp("dos_emin_ev", word) == 0)
        {
            read_value(ifs, dos_emin_ev);
        }
        else if (strcmp("dos_emax_ev", word) == 0)
        {
            read_value(ifs, dos_emax_ev);
        }
        else if (strcmp("dos_edelta_ev", word) == 0)
        {
            read_value(ifs, dos_edelta_ev);
        }
        else if (strcmp("dos_sigma", word) == 0)
        {
            read_value(ifs, b_coef);
        }

//----------------------------------------------------------
// Parameters about LCAO
// mohan add 2009-11-11
//----------------------------------------------------------
        else if (strcmp("lcao_ecut", word) == 0)
        {
            read_value(ifs, lcao_ecut);
        }
        else if (strcmp("lcao_dk", word) == 0)
        {
            read_value(ifs, lcao_dk);
        }
        else if (strcmp("lcao_dr", word) == 0)
        {
            read_value(ifs, lcao_dr);
        }
        else if (strcmp("lcao_rmax", word) == 0)
        {
            read_value(ifs, lcao_rmax);
        }
// Divide&Conqure
        else if (strcmp("dc_nx", word) == 0)
        {
            read_value(ifs, dc_nx);
        }
        else if (strcmp("dc_ny", word) == 0)
        {
            read_value(ifs, dc_ny);
        }
        else if (strcmp("dc_nz", word) == 0)
        {
            read_value(ifs, dc_nz);
        }
        else if (strcmp("selinv_npole", word) == 0)
        {
            read_value(ifs, selinv_npole);
        }
        else if (strcmp("selinv_temp", word) == 0)
        {
            read_value(ifs, selinv_temp);
        }
        else if (strcmp("selinv_deltae", word) == 0)
        {
            read_value(ifs, selinv_deltae);
        }
        else if (strcmp("selinv_gap", word) == 0)
        {
            read_value(ifs, selinv_gap);
        }
        else if (strcmp("selinv_mu", word) == 0)
        {
            read_value(ifs, selinv_mu);
        }
        else if (strcmp("selinv_threshold", word) == 0)
        {
            read_value(ifs, selinv_threshold);
        }
        else if (strcmp("selinv_niter", word) == 0)
        {
            read_value(ifs, selinv_niter);
        }
		// about molecular dynamics
/*
        else if (strcmp("md_dt", word) == 0)
        {
            read_value(ifs, md_dt);
        }
        else if (strcmp("md_restart", word) == 0)
        {
            read_value(ifs, md_restart);
        }
        else if (strcmp("md_tolv", word) == 0)
        {
            read_value(ifs, md_tolv);
        }
        else if (strcmp("md_thermostat", word) == 0)
        {
            read_value(ifs, md_thermostat);
        }
        else if (strcmp("md_temp0", word) == 0)
        {
            read_value(ifs, md_temp0);
        }
        else if (strcmp("md_tstep", word) == 0)
        {
            read_value(ifs, md_tstep);
        }
        else if (strcmp("md_delt", word) == 0)
        {
            read_value(ifs, md_delt);
        }
*/
//added begin by zheng daye
	else if (strcmp("md_mdtype",word) == 0)
	{
		read_value(ifs, md_mdtype);
	}
	else if (strcmp("md_tauthermo",word) == 0)
	{
		read_value(ifs, md_tauthermo);
	}
	else if (strcmp("md_taubaro",word) == 0)
	{
		read_value(ifs,md_taubaro );
	}
	else if (strcmp("md_dt",word) == 0)
	{
		read_value(ifs, md_dt);
	}
	else if (strcmp("md_nresn",word) == 0)
	{
		read_value(ifs,md_nresn );
	}
	else if (strcmp("md_nyosh",word) == 0)
	{
		read_value(ifs, md_nyosh);
	}
	else if (strcmp("md_qmass",word) == 0)
	{
		read_value(ifs,md_qmass );
	}
	else if (strcmp("md_tfirst",word) == 0)
	{
		read_value(ifs, md_tfirst);
	}
	else if (strcmp("md_tlast",word) == 0)
	{
		read_value(ifs,md_tlast );
		if(md_tfirst!=md_tlast)
		{
			ifstream file1;
			file1.open("ChangeTemp.dat");
			if(!file1)						// Peize Lin fix bug 2016-08-06
			{
				ofstream file;
				file.open("ChangeTemp.dat");
				for(int ii=0;ii<30;ii++)
				{
					file<<md_tfirst+(md_tlast-md_tfirst)/double(30)*double(ii+1)<<" ";
				}
				file.close();
			}
			else 
				file1.close();
		}
	}
	else if (strcmp("md_dumpmdfred",word) == 0)
	{
		read_value(ifs, md_dumpmdfred);
	}
	else if (strcmp("md_mdoutpath",word) == 0)
	{
		read_value(ifs,md_mdoutpath );
	}
	else if (strcmp("md_domsd",word) == 0)
	{
		read_value(ifs, md_domsd);
	}
	else if (strcmp("md_domsdatom",word) == 0)
	{
		read_value(ifs, md_domsdatom);
	}
	else if (strcmp("md_rstmd",word) == 0)
	{
		read_value(ifs,md_rstmd );
	}
	else if (strcmp("md_outputstressperiod",word) == 0)
	{
		read_value(ifs,md_outputstressperiod );
	}
	else if (strcmp("md_fixtemperature",word) == 0)
	{
		read_value(ifs,md_fixtemperature );
	}
	else if (strcmp("md_ediff",word) == 0)
	{
		read_value(ifs,md_ediff );
	}
	else if (strcmp("md_ediffg",word) == 0)
	{
		read_value(ifs,md_ediffg );
	}
        else if (strcmp("md_msdstarttime",word) == 0)
        {
                 read_value(ifs,md_msdstartTime );
        }
//added by zheng daye
//----------------------------------------------------------
// vdwD2
// Peize Lin add 2014-03-31
//----------------------------------------------------------
        else if (strcmp("vdwd2", word) == 0)
        {
            read_value(ifs, vdwD2);
        }
        else if (strcmp("vdwd2_scaling", word) == 0)
        {
            read_value(ifs, vdwD2_scaling);
        }
        else if (strcmp("vdwd2_d", word) == 0)
        {
            read_value(ifs, vdwD2_d);
        }		
        else if (strcmp("vdwd2_c6_file", word) == 0)
        {
            read_value(ifs, vdwD2_C6_file);
        }
        else if (strcmp("vdwd2_c6_unit", word) == 0)
        {
            read_value(ifs, vdwD2_C6_unit);
        }
        else if (strcmp("vdwd2_r0_file", word) == 0)
        {
            read_value(ifs, vdwD2_R0_file);
        }
        else if (strcmp("vdwd2_r0_unit", word) == 0)
        {
            read_value(ifs, vdwD2_R0_unit);
        }
        else if (strcmp("vdwd2_model", word) == 0)
        {
            read_value(ifs, vdwD2_model);
        }
        else if (strcmp("vdwd2_period", word) == 0)
        {
			ifs>> vdwD2_period[0]>> vdwD2_period[1];
            read_value(ifs, vdwD2_period[2]);
        }
        else if (strcmp("vdwd2_radius", word) == 0)
        {
            read_value(ifs, vdwD2_radius);
        }
        else if (strcmp("vdwd2_radius_unit", word) == 0)
        {
            read_value(ifs, vdwD2_radius_unit);
        }
        else
        {
		//xiaohui add 2015-09-15
		if(word[0] != '#' && word[0] != '/')
		{
			input_error = 1;
			cout<<" THE PARAMETER NAME '" << word << "' IS NOT USED!" << endl;
		}
// mohan screen this 2012-06-30
//            cout << " THE PARAMETER NAME '" << word
//               << "' IS NOT USED!" << endl;
            ifs.ignore(150, '\n');
        }

        ifs.rdstate();

        /*if(gamma_only == 1)
        {
           gamma_only_local = 1;      //pengfei 2014-10-15
           gamma_only = 0;
           cout << "gamma_only_local = " << gamma_only_local <<endl;
        }*/

        if (ifs.eof() != 0)
        {
			break;
        }
        else if (ifs.bad() != 0)
        {
			cout << " Bad input parameters. " << endl;
            return false;
        }
        else if (ifs.fail() != 0)
        {
			cout << " word = " << word << endl;
			cout << " Fail to read parameters. " << endl; 
            ifs.clear();
			return false;
        }
        else if (ifs.good() == 0)
        {
			break;
        }
    }
        if (basis_type == "pw")  // pengfei Li add 2015-1-31
        {
            gamma_only = 0;
            //cout << "gamma_only =" << gamma_only << endl;
        }
        else if ((basis_type == "lcao" || basis_type == "lcao_in_pw")&&(gamma_only == 1))
        {
            gamma_only_local = 1;
            //cout << "gamma_only_local =" << gamma_only_local << endl;
        }

    return true;
}//end read_parameters

#ifdef __MPI
void Input::Bcast()
{
    TITLE("Input","Bcast");

//	cout << "\n Bcast()" << endl;
//----------------------------------------------------------
// main parameters
//----------------------------------------------------------
    Parallel_Common::bcast_string( suffix );
    Parallel_Common::bcast_string( atom_file );//xiaohui modify 2015-02-01
    Parallel_Common::bcast_string( pseudo_dir );
    Parallel_Common::bcast_string( pseudo_type ); // mohan add 2013-05-20 (xiaohui add 2013-06-23)
    Parallel_Common::bcast_string( kpoint_file );//xiaohui modify 2015-02-01
    Parallel_Common::bcast_string( wannier_card );
    Parallel_Common::bcast_string( epm_pseudo_card );
    Parallel_Common::bcast_string( latname );
    Parallel_Common::bcast_string( calculation );
    Parallel_Common::bcast_int( ntype );
    Parallel_Common::bcast_int( nbands );
    Parallel_Common::bcast_int( nbands_istate );
	Parallel_Common::bcast_int( npool );
    Parallel_Common::bcast_int( epm_spin_orbital );
    Parallel_Common::bcast_bool( berry_phase );
    Parallel_Common::bcast_int( efield );
    Parallel_Common::bcast_int( edir );
    Parallel_Common::bcast_double( emaxpos );
    Parallel_Common::bcast_double( eopreg );
    Parallel_Common::bcast_double( eamp );


    Parallel_Common::bcast_int( bfield );
    Parallel_Common::bcast_double( bfield_teslax );
    Parallel_Common::bcast_double( bfield_teslay );
    Parallel_Common::bcast_double( bfield_teslaz );
    Parallel_Common::bcast_double( bfield_gauge_x );//sunzhiyuan
    Parallel_Common::bcast_double( bfield_gauge_y );
    Parallel_Common::bcast_double( bfield_gauge_z );
    Parallel_Common::bcast_bool( opt_epsilon2 );
    Parallel_Common::bcast_int( opt_nbands );
    Parallel_Common::bcast_bool( lda_plus_u );

	Parallel_Common::bcast_string( dft_functional );
    Parallel_Common::bcast_int( nspin );
    Parallel_Common::bcast_double( nelec );
    Parallel_Common::bcast_int( lmaxmax );

    Parallel_Common::bcast_double( tot_magnetization );

    //Parallel_Common::bcast_int( local_basis ); xiaohui modify 2013-09-01
	Parallel_Common::bcast_string( basis_type ); //xiaohui add 2013-09-01
    //Parallel_Common::bcast_int ( linear_scaling ); xiaohui modify 2013-09-01
	Parallel_Common::bcast_string( ks_solver ); //xiaohui add 2013-09-01
	Parallel_Common::bcast_double( search_radius );
	Parallel_Common::bcast_bool( search_pbc );
    Parallel_Common::bcast_bool ( sparse_matrix );
    Parallel_Common::bcast_int ( atom_distribution );
    Parallel_Common::bcast_double( search_radius );
    Parallel_Common::bcast_bool( symmetry );
    Parallel_Common::bcast_bool( mlwf_flag );
    Parallel_Common::bcast_int( vna );
	Parallel_Common::bcast_int( grid_speed );//mohan add 2012-03-29
    Parallel_Common::bcast_int( force );
    Parallel_Common::bcast_int( force_set );
    Parallel_Common::bcast_double( force_thr);
    Parallel_Common::bcast_double( force_thr_ev2);
    Parallel_Common::bcast_bool( stress );
    Parallel_Common::bcast_string( ion_dynamics );
    Parallel_Common::bcast_double( cg_threshold); // pengfei add 2013-08-15
	Parallel_Common::bcast_string( out_level);
    Parallel_Common::bcast_double( bfgs_w1);
    Parallel_Common::bcast_double( bfgs_w2);
    Parallel_Common::bcast_double( trust_radius_max);
    Parallel_Common::bcast_double( trust_radius_min);
    Parallel_Common::bcast_double( trust_radius_ini);

    //Parallel_Common::bcast_bool( gamma_only );
    Parallel_Common::bcast_bool( gamma_only );
    Parallel_Common::bcast_bool( gamma_only_local );
    Parallel_Common::bcast_double( ecutwfc );
    Parallel_Common::bcast_double( ecutrho );
    Parallel_Common::bcast_int( ncx );
    Parallel_Common::bcast_int( ncy );
    Parallel_Common::bcast_int( ncz );
    Parallel_Common::bcast_int( nx );
    Parallel_Common::bcast_int( ny );
    Parallel_Common::bcast_int( nz );
    Parallel_Common::bcast_int( bx );
    Parallel_Common::bcast_int( by );
    Parallel_Common::bcast_int( bz );

    //Parallel_Common::bcast_string( diago_type ); xiaohui modify 2013-09-01
    Parallel_Common::bcast_int( diago_proc ); //mohan add 2012-01-03
    Parallel_Common::bcast_int( diago_cg_maxiter );
	Parallel_Common::bcast_int( diago_cg_prec );
    Parallel_Common::bcast_int( diago_david_ndim );
    Parallel_Common::bcast_double( ethr );
	Parallel_Common::bcast_int( nb2d );
	Parallel_Common::bcast_int( nurse );
	Parallel_Common::bcast_bool( colour );
	Parallel_Common::bcast_int( t_in_h );
	Parallel_Common::bcast_int( vl_in_h );
	Parallel_Common::bcast_int( vnl_in_h );
	Parallel_Common::bcast_int( zeeman_in_h );

	Parallel_Common::bcast_int( test_force );

    Parallel_Common::bcast_double( fs_ref_energy );

    Parallel_Common::bcast_double( dr2 );
    Parallel_Common::bcast_int( niter );
    Parallel_Common::bcast_int( nstep );
	Parallel_Common::bcast_int( out_stru ); //mohan add 2012-03-23

    //Parallel_Common::bcast_string( occupations );
    Parallel_Common::bcast_string( smearing );
    Parallel_Common::bcast_double( degauss );

    Parallel_Common::bcast_string( mixing_mode );
    Parallel_Common::bcast_double( mixing_beta );
    Parallel_Common::bcast_int( mixing_ndim );
    Parallel_Common::bcast_double( mixing_gg0 ); //mohan add 2014-09-27

    Parallel_Common::bcast_string( restart_mode );
    Parallel_Common::bcast_string( start_wfc );
	Parallel_Common::bcast_int( mem_saver );
	Parallel_Common::bcast_int( printe );
    Parallel_Common::bcast_string( start_pot );
    Parallel_Common::bcast_string( charge_extrap );//xiaohui modify 2015-02-01
    Parallel_Common::bcast_int( out_charge );
    Parallel_Common::bcast_int( out_dm );
    Parallel_Common::bcast_int( out_potential );
    Parallel_Common::bcast_bool( out_wf );
	Parallel_Common::bcast_int( out_dos );
        Parallel_Common::bcast_int( out_band );
	Parallel_Common::bcast_int( out_hs );
	Parallel_Common::bcast_bool( out_lowf );
	Parallel_Common::bcast_bool( out_alllog );

	Parallel_Common::bcast_double( dos_emin_ev );
	Parallel_Common::bcast_double( dos_emax_ev );
	Parallel_Common::bcast_double( dos_edelta_ev );
        Parallel_Common::bcast_double( b_coef );

	// mohan add 2009-11-11
	Parallel_Common::bcast_double( lcao_ecut );
	Parallel_Common::bcast_double( lcao_dk );
	Parallel_Common::bcast_double( lcao_dr );
	Parallel_Common::bcast_double( lcao_rmax );

	// mohan add 2011-06-12
	Parallel_Common::bcast_int( dc_nx );
	Parallel_Common::bcast_int( dc_ny );
	Parallel_Common::bcast_int( dc_nz );

	// mohan add 2011-09-28
	Parallel_Common::bcast_int( selinv_npole);
	Parallel_Common::bcast_double( selinv_temp);
	Parallel_Common::bcast_double( selinv_gap);
	Parallel_Common::bcast_double( selinv_deltae);
	Parallel_Common::bcast_double( selinv_mu);
	Parallel_Common::bcast_double( selinv_threshold);
	Parallel_Common::bcast_int( selinv_niter);
/*
	// mohan add 2011-11-07
	Parallel_Common::bcast_double( md_dt );
	Parallel_Common::bcast_int( md_restart );
	Parallel_Common::bcast_double( md_tolv );
	Parallel_Common::bcast_string( md_thermostat );
	Parallel_Common::bcast_double( md_temp0 );
	Parallel_Common::bcast_int( md_tstep );
	Parallel_Common::bcast_double( md_delt );
*/
	//zheng daye add 2014/5/5
        Parallel_Common::bcast_int(md_mdtype);
        Parallel_Common::bcast_double(md_tauthermo);
        Parallel_Common::bcast_double(md_taubaro);
        Parallel_Common::bcast_double(md_dt);
        Parallel_Common::bcast_int(md_nresn);
        Parallel_Common::bcast_int(md_nyosh);
        Parallel_Common::bcast_double(md_qmass);
        Parallel_Common::bcast_double(md_tfirst);
        Parallel_Common::bcast_double(md_tlast);
        Parallel_Common::bcast_int(md_dumpmdfred);
        Parallel_Common::bcast_string(md_mdoutpath);
        Parallel_Common::bcast_bool(md_domsd);
        Parallel_Common::bcast_bool(md_domsdatom);
        Parallel_Common::bcast_int(md_rstmd);
        Parallel_Common::bcast_int(md_outputstressperiod);
        Parallel_Common::bcast_int(md_fixtemperature);
        Parallel_Common::bcast_double(md_ediff);
        Parallel_Common::bcast_double(md_ediffg);
        Parallel_Common::bcast_int(md_msdstartTime);
	// Peize Lin add 2014-04-07
	Parallel_Common::bcast_bool( vdwD2 );
	Parallel_Common::bcast_double( vdwD2_scaling );
	Parallel_Common::bcast_double( vdwD2_d );
	Parallel_Common::bcast_string( vdwD2_C6_file );
	Parallel_Common::bcast_string( vdwD2_C6_unit );
	Parallel_Common::bcast_string( vdwD2_R0_file );
	Parallel_Common::bcast_string( vdwD2_R0_unit );
	Parallel_Common::bcast_string( vdwD2_model );
	Parallel_Common::bcast_int( vdwD2_period[0] );
	Parallel_Common::bcast_int( vdwD2_period[1] );
	Parallel_Common::bcast_int( vdwD2_period[2] );
	Parallel_Common::bcast_double( vdwD2_radius );
	Parallel_Common::bcast_string( vdwD2_radius_unit );
	
    return;
}
#endif

void Input::Check(void)
{
    TITLE("Input","Check");

	if(nbands < 0) WARNING_QUIT("Input","NBANDS must > 0");
//	if(nbands_istate < 0) WARNING_QUIT("Input","NBANDS_ISTATE must > 0");
	if(nb2d < 0) WARNING_QUIT("Input","nb2d must > 0");
	if(ntype < 0) WARNING_QUIT("Input","ntype must > 0");

	//cout << "diago_proc=" << diago_proc << endl;
	//cout << " NPROC=" << NPROC << endl;
	if(diago_proc<=0)
	{
		diago_proc = NPROC;
	}
	else if(diago_proc>NPROC)
	{
		diago_proc = NPROC;
	}

	// mohan add 2010/03/29
	//if(!local_basis && diago_type=="lapack") xiaohui modify 2013-09-01
	//if(basis_type=="pw" && ks_solver=="lapack") xiaohui modify 2013-09-04 //xiaohui add 2013-09-01
	//{
	//	WARNING_QUIT("Input","lapack can not be used in plane wave basis.");
	//} xiaohui modify 2013-09-04	

	//xiaohui move 4 lines, 2015-09-30
	//if(symmetry)
	//{
	//	WARNING("Input","symmetry is only correct for total energy calculations now,not for nonlocal force." );
	//}

    if (efield && symmetry)
    {
        symmetry = false;
        WARNING_QUIT("Input","Presently no symmetry can be used with electric field");
    }

    if (efield && nspin>2)
    {
        WARNING_QUIT("Input","nspin>2 not available with electric field.");
    }

	if (edir < 1 || edir > 3)
	{
		WARNING_QUIT("Input","edir should be 1, 2 or 3.");
	}

	if (emaxpos < 0.0 || emaxpos >= 1.0)
	{
		WARNING_QUIT("Input","emaxpos should be [0,1)");
	}

	if (eopreg < 0.0 || eopreg >= 1.0)
	{
		WARNING_QUIT("Input","eopreg should be [0,1)");
	}



    if (nbands < 0)
    {
        WARNING_QUIT("Input","nbands < 0 is not allowed !");
    }

    if (nelec < 0.0)
    {
        WARNING_QUIT("Input","nelec < 0 is not allowed !");
    }

//----------------------------------------------------------
// main parameters / electrons / spin ( 1/16 )
//----------------------------------------------------------
    if (calculation == "scf")
    {
		if(mem_saver == 1)
		{
			mem_saver = 0;
			AUTO_SET("mem_savre","0");
		}
		//xiaohui modify 2015-09-15, 0 -> 1
                //force = 0;
                force = 1;
                nstep = 1;

    }
    else if (calculation == "relax")  // pengfei 2014-10-13
    {
                if(mem_saver == 1)
                {
                        mem_saver = 0;
                        AUTO_SET("mem_savre","0");
                }
                force = 1;
    }

    else if (calculation == "nscf")
    {
		CALCULATION == "nscf";
        nstep = 1;
		out_stru = 0;
        
		//if (local_basis == 0 && linear_scaling == 0) xiaohui modify 2013-09-01
		if (basis_type == "pw") //xiaohui add 2013-09-01. Attention! maybe there is some problem
		{
			if (ethr>1.0e-3)
        	{
        	    WARNING_QUIT("Input::Check","nscf : ethr > 1.0e-3, ethr too large.");
        	}
		}
		if(force) // mohan add 2010-09-07
		{
			force = false;
			AUTO_SET("force","false");
		}
		if (out_dos == 3 && symmetry)
		{
			WARNING_QUIT("Input::Check","symmetry can't be used for out_dos==3(Fermi Surface Plotting) by now.");
		}
    }
	else if(calculation == "istate")
	{
		CALCULATION = "istate";
		nstep = 1;
		out_stru = 0;
		out_dos = 0;
                out_band = 0;
		force=0;
		start_wfc = "file";
		start_pot = "atomic"; // useless, 
		charge_extrap = "atomic"; //xiaohui modify 2015-02-01
		out_charge = 1; // this leads to the calculation of state charge.
		out_dm = 0;
		out_potential = 0;

		//if(!local_basis || !linear_scaling) xiaohui modify 2013-09-01
		if(basis_type == "pw") //xiaohui add 2013-09-01
		{
			WARNING_QUIT("Input::Check","calculate = istate is only availble for LCAO.");
		}
	}
	else if(calculation == "ienvelope")
	{
		CALCULATION = "ienvelope"; // mohan fix 2011-11-04
		nstep = 1;
		out_stru = 0;
		out_dos = 0;
                out_band = 0;
		force = 0;
		start_wfc = "file";
		start_pot = "atomic";
		charge_extrap = "atomic"; //xiaohui modify 2015-02-01
		out_charge = 1;
		out_dm = 0;
		out_potential = 0;
		//if(!local_basis || !linear_scaling) xiaohui modify 2013-09-01
		if(basis_type == "pw") //xiaohui add 2013-09-01
		{
			WARNING_QUIT("Input::Check","calculate = istate is only availble for LCAO.");
		}	
	}
	else if(calculation == "md") // mohan add 2011-11-04
	{
		CALCULATION = "md"; 
		force = 1;
	}
	else if(calculation == "test")
	{
	}
    else
    {
        WARNING_QUIT("Input","check 'calculation' !");
    }
    if (start_pot != "atomic" && start_pot != "file")
    {
        WARNING_QUIT("Input","wrong 'start_pot',not 'atomic', 'file',please check");
    }
	//xiaohui modify 2014-05-10, extra_pot value changes to 0~7	
	//if (extra_pot <0 ||extra_pot > 7)
	//{
	//	WARNING_QUIT("Input","wrong 'extra_pot',neither 0~7.");
	//}xiaohui modify 2015-02-01
	if(gamma_only_local==0)
	{
		if(out_dm==1)
		{
			WARNING_QUIT("Input","out_dm with k-point algorithm is not implemented yet.");
		}
	}

	//if(extra_pot==4 && local_basis==0) xiaohui modify 2013-09-01
	if(charge_extrap=="dm" && basis_type=="pw") //xiaohui add 2013-09-01, xiaohui modify 2015-02-01
	{
		WARNING_QUIT("Input","wrong 'charge_extrap=dm' is only available for local orbitals.");//xiaohui modify 2015-02-01
	}

	if(charge_extrap=="dm" || force>1)
	{
		//if(out_dm==0) out_dm = 10000;//at least must output the density matrix at the last electron iteration step.
	}
	//if(charge_extrap != "dm")//xiaohui add 2015-02-01
	//{
	//	if(calculation=="relax")//xiaohui add 2015-02-01
	//	{
	//		charge_extrap = "first-order";
	//	}
	//	if(calculation=="md")//xiaohui add 2015-02-01
	//	{
	//		charge_extrap = "second-order";
	//	}
	//}

    if (CALCULATION=="nscf" && start_pot != "file")
    {
        start_pot = "file";
        AUTO_SET("start_pot",start_pot);
    }

    if (start_wfc != "atomic" && start_wfc != "random" &&
            start_wfc != "file")
    {
        WARNING_QUIT("Input","wrong start_wfc, please use 'atomic' or 'random' or 'file' ");
    }

    if (nbands > nbndxx)
    {
        WARNING_QUIT("Input","nbnd out of range, increase nbndxx!");
    }
    if ( nelec > 0 && nbands > 0 && nelec > 2*nbands )
    {
        WARNING_QUIT("Input","nelec > 2*nbnd , bands not enough!");
    }
    if (nspin < 1  || nspin > nspinx)
    {
        WARNING_QUIT("Input","nspin out of range!");
    }
	

	//if(local_basis==0) xiaohui modify 2013-09-01
	if(basis_type=="pw") //xiaohui add 2013-09-01
	{
		//if(diago_type=="default") xiaohui modify 2013-09-01
		if(ks_solver=="default") //xiaohui add 2013-09-01
		{
			//diago_type = "cg";
			ks_solver = "cg";
			//AUTO_SET("diago_type","cg");
			AUTO_SET("ks_solver","cg");
		}
		else if(ks_solver=="cg")
		{
			ofs_warning << " It's ok to use cg." << endl;
		}
		else if(ks_solver=="dav")
		{
			ofs_warning << " It's ok to use dav." << endl;
		}
		//if(diago_type=="hpseps") xiaohui modify 2013-09-01
		else if(ks_solver=="hpseps") //xiaohui add 2013-09-01
		{
			//ofs_warning << " hpseps can't be used with plane wave basis." << endl; xiaohui modify 2013-09-04
			//diago_type = "cg";
			//ks_solver = "cg"; xiaohui modify 2013-09-04
			//AUTO_SET("diago_type","cg");
			//AUTO_SET("ks_solver","cg"); xiaohui modify 2013-09-04
			WARNING_QUIT("Input","hpseps can not be used with plane wave basis."); //xiaohui add 2013-09-04
		}
		//else if(diago_type=="selinv") xiaohui modify 2013-09-01
		else if(ks_solver=="selinv") //xiaohui add 2013-09-01
		{
			//ofs_warning << " selinv can't be used with plane wave basis." << endl; xiaohui modify 2013-09-04
			//diago_type = "cg";
			//ks_solver = "cg"; xiaohui modify 2013-09-04
			//AUTO_SET("diago_type","cg");
			//AUTO_SET("ks_solver","cg"); xiaohui modify 2013-09-04
			WARNING_QUIT("Input","selinv can not be used with plane wave basis."); //xiaohui add 2013-09-04
		}
		//xiaohui add 2013-09-04
		else if(ks_solver=="lapack")
		{
			//ofs_warning << " lapack can't be used with plane wave basis." << endl; xiaohui modify 2013-09-04
			WARNING_QUIT("Input","lapack can not be used with plane wave basis.");
		}//xiaohui add 2013-09-04
		else //xiaohui add 2013-09-04
		{
			WARNING_QUIT("Input","please check the ks_solver parameter!");
		} //xiaohui add 2013-09-04
	}
	//else if(local_basis==4) xiaohui modify 2013-09-01
	else if(basis_type=="lcao") //xiaohui add 2013-09-01
	{
		//if(linear_scaling == 1) xiaohui modify 2013-09-01
		//{
			//if(diago_type == "default")
			if(ks_solver == "default")
			{
				//diago_type = "hpseps";
				ks_solver = "hpseps";
				//AUTO_SET("diago_type","hpseps");
				AUTO_SET("ks_solver","hpseps");
			}
			//else if(diago_type == "cg" )
			else if (ks_solver == "cg")
			{
				//ofs_warning << " Use CG method in LCAO." << endl; xiaohui 2013-09-04
				//atom_distribution=1; xiaohui 2013-09-04
				//AUTO_SET("atom_distribution",1); xiaohui 2013-09-04
				WARNING_QUIT("Input","not ready for cg method in lcao ."); //xiaohui add 2013-09-04
			}
			//else if( diago_type == "hpseps" )
			else if (ks_solver == "hpseps")
			{
#ifdef __MPI
				ofs_warning << "It's a good choice to use hpseps!" << endl;
#else
				WARNING_QUIT("Input","hpseps can not be used for series version.");
#endif
			}
			//else if( diago_type == "lapack" )
			else if (ks_solver == "lapack")
			{
#ifdef __MPI
				//WARNING_QUIT("Input","diago_type=lapack is not an option for parallel version of MESIA (try hpseps).");
				//xiaohui modify 2015-03-25
				//WARNING_QUIT("Input","ks_solver=lapack is not an option for parallel version of MESIA (try hpseps).");	
				WARNING_QUIT("Input","ks_solver=lapack is not an option for parallel version of ABACUS (try hpseps).");	
#else
				ofs_warning << " It's ok to use lapack." << endl;
#endif
			}
			//else if( diago_type == "selinv")
			else if (ks_solver == "selinv")
			{
				WARNING_QUIT("Input","not ready for selinv method in lcao ."); //xiaohui add 2013-09-04
			}
			//xiaohui add 2013-09-04
			else if(ks_solver == "linear_scaling")
			{
				WARNING_QUIT("Input","not ready for linear_scaling method in lcao .");
			} //xiaohui add 2013-09-04
			else
			{
				//WARNING_QUIT("Input","please check the diago_type parameter!");
				WARNING_QUIT("Input","please check the ks_solver parameter!");
			}
		//}xiaohui modify 2013-09-01
		//else if(linear_scaling == 2) xiaohui modify 2013-09-01. Attention! Maybe there is some problem.
		//{
			//if(diago_type != "canonical" && diago_type != "trace_correcting" && diago_type != "trace_resetting")
			//{	
			//	diago_type = "canonical";
			//	AUTO_SET("diago_type","canonical");
			//}
		//}
		//else xiaohui modify 2013-09-01, move this part to "lcao in pw"
		//{
			//if( diago_type != "lapack" )
			//{
				//ofs_warning << " LCAO in plane wave can only done with lapack." << endl;
				//diago_type = "lapack";
				//AUTO_SET("diago_type","lapack");
			//}
		//}
	}
	else if(basis_type=="lcao_in_pw") //xiaohui add 2013-09-01
	{
		if( ks_solver != "lapack" )
		{
			//ofs_warning << " LCAO in plane wave can only done with lapack." << endl; xiaohui modify 2013-09-04
			//ks_solver = "lapack"; xiaohui modify 2013-09-04
			//AUTO_SET("ks_solver","lapack"); xiaohui modify 2013-09-04
			WARNING_QUIT("Input","LCAO in plane wave can only done with lapack.");
		}
	}
	else //xiaohui add 2013-09-01
	{
		WARNING_QUIT("Input","please check the basis_type parameter!");
	}

	//if(local_basis==0 && linear_scaling>0) xiaohui modify 2013-09-01. Attention! Maybe there is some problem.
	//{
	//	WARNING_QUIT("Input","linear scaling method can not used for plane wave basis!");
	//}

	// add 2010-09-04
	//if(local_basis==0 && gamma_only) xiaohui modify 2013-09-01
	if(basis_type=="pw" && gamma_only) //xiaohui add 2013-09-01
	{
		WARNING_QUIT("Input","gamma_only not implemented for plane wave now.");
	}

	// add 2010-09-06
	//if(local_basis==0 || (local_basis==4 && !linear_scaling) ) xiaohui modify 2013-09-01
	if(basis_type=="pw" || basis_type=="lcao_in_pw") //xiaohui add 2013-09-01
	{
		if(gamma_only_local)
		{
			// means you can use > 1 number of k points.
			gamma_only_local = 0;
			AUTO_SET("gamma_only_local","0");
		}
	}

	//if( (local_basis>0 && linear_scaling) && !gamma_only_local) xiaohui modify 2013-09-01
	if(basis_type=="lcao" && !gamma_only_local) //xiaohui add 2013-09-01. Attention! Maybe there is some problem.
	{
		WARNING("Input","gamma_only_local algorithm is not used.");
	}

	// new rule, mohan add 2012-02-11
	// otherwise, there need wave functions transfers
	//if(diago_type=="cg") xiaohui modify 2013-09-01
	if(ks_solver=="cg") //xiaohui add 2013-09-01
	{
		if(diago_proc!=NPROC)
		{
			WARNING("Input","when CG is used for diago, diago_proc==NPROC");
			diago_proc=NPROC;
		}
	}

	//if(NPROC>1 && diago_type=="lapack") xiaohui modify 2013-09-01
	if(NPROC>1 && ks_solver=="lapack") //xiaohui add 2013-09-01
	{
		//if(local_basis ==4 && linear_scaling==0) xiaohui modify 2013-09-01
		if(basis_type=="lcao_in_pw") //xiaohui add 2013-09-01
		{

		}
		else
		{
			WARNING_QUIT("Input","lapack can not be used when nproc > 1");
		}
	}

	if(ion_dynamics!= "sd" && ion_dynamics!="cg" && ion_dynamics!="bfgs" && ion_dynamics!="cg_bfgs")   // pengfei add 13-8-10  a new method cg to bfgs
	{
		 WARNING_QUIT("Input","ion_dynamics can only be sd, cg, bfgs or cg_bfgs.");
	}

	if(opt_epsilon2==true && opt_nbands==0)
	{
		WARNING_QUIT("Input","please Input the opt_nbands for optical properties calculations");
	}

	//if(local_basis==0) xiaohui modify 2013-09-01
	if(basis_type=="pw") //xiaohui add 2013-09-01
	{
		bx=1;
		by=1;
		bz=1;
	}
	else if(bx>10)
	{
		WARNING_QUIT("Input","bx is too large!");
	}
	else if(by>10)
	{
		WARNING_QUIT("Input","by is too large!");
	}
	else if(bz>10)
	{
		WARNING_QUIT("Input","bz is too large!");
	}	

	if(lcao_ecut == 0) 
	{
		lcao_ecut = ecutwfc; 
		AUTO_SET("lcao_ecut",ecutwfc);
	}

	if(vdwD2)														//Peize Lin add 2-14-04-05, update 2015-09-30
	{
		if( (vdwD2_C6_unit!="Jnm6/mol") && (vdwD2_C6_unit!="eVA6") )
		{
			WARNING_QUIT("Input","vdwD2_C6_unit must be Jnm6/mol or eVA6");
		}
		if( (vdwD2_R0_unit!="A") && (vdwD2_R0_unit!="Bohr") )
		{
			WARNING_QUIT("Input","vdwD2_R0_unit must be A or Bohr");
		}
		if( (vdwD2_model!="radius") && (vdwD2_model!="period") )
		{
			WARNING_QUIT("Input","vdwD2_model must be radius or period");
		}
		if( (vdwD2_period[0]<=0) || (vdwD2_period[1]<=0) || (vdwD2_period[2]<=0) )
		{
			WARNING_QUIT("Input","vdwD2_period <= 0 is not allowd");
		}
		if( vdwD2_radius<=0 )
		{
			WARNING_QUIT("Input","vdwD2_radius <= 0 is not allowd");
		}
		if( (vdwD2_radius_unit!="A") && (vdwD2_radius_unit!="Bohr") )
		{
			WARNING_QUIT("Input","vdwD2_radius_unit must be A or Bohr");
		}
	}
//2015-06-15, xiaohui
        if(mixing_mode == "pulay" && mixing_gg0 > 0.0)
        {
                 WARNING("Input","To use pulay-kerker mixing method, please set mixing_type=pulay-kerker");
        }
	
    return;
}



void Input::Print(const string &fn)const
{
    if (MY_RANK!=0) return;

    TITLE("Input","Print");

    ofstream ofs(fn.c_str());

	//----------------------------------
	// output the information in INPUT.
	//----------------------------------
    ofs << "INPUT_PARAMETERS" << endl;
	ofs << setiosflags(ios::left);
	
	ofs << "#Parameters (1.General)" << endl;
	OUTP(ofs,"suffix",suffix,"the name of main output directory");
	OUTP(ofs,"latname",latname,"the name of lattice name");
	OUTP(ofs,"atom_file",global_atom_card,"the filename of file containing atom positions");//xiaohui modify 2015-02-01
	OUTP(ofs,"kpoint_file",global_kpoint_card,"the name of file containing k points");//xiaohui modify 2015-02-01
	OUTP(ofs,"pseudo_dir",global_pseudo_dir,"the directory containing pseudo files");
	OUTP(ofs,"pseudo_type",global_pseudo_type,"the type pseudo files"); // mohan add 2013-05-20 (xiaohui add 2013-06-23)
	OUTP(ofs,"dft_functional",dft_functional,"exchange correlation functional"); // xiaohui add 2015-03-24
//	OUTP(ofs,"wannier_card",wannier_card,"not used now");
#ifdef __EPM
	OUTP(ofs,"epm_pseudo_card",epm_pseudo_card,"name of empirical pseudo");
	OUTP(ofs,"epm_spin_orbital",epm_spin_orbital,"spin orbital in emprical pseudo");
#endif
	OUTP(ofs,"calculation",calculation,"test; scf; relax; nscf; ienvelope; istate;");
	OUTP(ofs,"ntype",ntype,"atom species number");
	OUTP(ofs,"nspin",nspin,"1: single spin; 2: up and down spin;");
	OUTP(ofs,"nbands",nbands,"number of bands");
	OUTP(ofs,"nbands_istate",nbands_istate,"number of bands around Fermi level for istate calulation");
	OUTP(ofs,"symmetry",symmetry,"turn symmetry on or off");	
	OUTP(ofs,"nelec",nelec,"input number of electrons");
        //OUTP(ofs,"lmax1",lmax1,"lmax");
	ofs << "\n#Parameters (2.PW)" << endl;
	OUTP(ofs,"ecutwfc",ecutwfc,"#energy cutoff for wave functions");
	//if(diago_type=="cg") xiaohui modify 2013-09-01
	if(ks_solver=="cg") //xiaohui add 2013-09-01
	{
		OUTP(ofs,"diago_cg_maxiter",diago_cg_maxiter,"max iteration number for cg");
		OUTP(ofs,"diago_cg_prec",diago_cg_prec,"diago_cg_prec");
	}
	//else if(diago_type=="dav") xiaohui modify 2013-09-01
	else if(ks_solver=="dav") //xiaohui add 2013-09-01
	{
		OUTP(ofs,"diago_david_ndim",diago_david_ndim,"max dimension for davidson");
	}
	OUTP(ofs,"ethr",ethr,"threshold for eigenvalues is cg electron iterations");
	OUTP(ofs,"dr2",dr2,"charge density error");
	OUTP(ofs,"start_wfc",start_wfc,"start wave functions are from 'atomic' or 'file'");
	OUTP(ofs,"start_charge",start_pot,"start charge is from 'atomic' or file");
	OUTP(ofs,"charge_extrap",charge_extrap,"atomic; first-order; second-order; dm:coefficients of SIA");
	OUTP(ofs,"out_charge",out_charge,">0 output charge density for selected electron steps");
	OUTP(ofs,"out_potential",out_potential,"output realspace potential");
	OUTP(ofs,"out_wf",out_wf,"output wave functions");
	OUTP(ofs,"out_dos",out_dos,"output energy and dos");
        OUTP(ofs,"out_band",out_band,"output energy and band structure");
//	OUTP(ofs,"ecutrho",ecutrho);
//	OUTP(ofs,"ncx",ncx);
//	OUTP(ofs,"ncy",ncy);
//	OUTP(ofs,"ncz",ncz);
	OUTP(ofs,"nx",nx,"number of points along x axis for FFT grid");
	OUTP(ofs,"ny",ny,"number of points along y axis for FFT grid");
	OUTP(ofs,"nz",nz,"number of points along z axis for FFT grid");	
	
	ofs << "\n#Parameters (3.Relaxation)" << endl;
	//OUTP(ofs,"diago_type",DIAGO_TYPE,"cg; david; lapack; hpseps;"); xiaohui modify 2013-09-01
	OUTP(ofs,"ks_solver",KS_SOLVER,"cg; david; lapack; hpseps;");
	OUTP(ofs,"niter",niter,"#number of electron iterations");
	OUTP(ofs,"vna",vna,"use the vna or not");
	OUTP(ofs,"grid_speed",grid_speed,"1:normal 2:fast");//mohan add 2012-03-29
	//OUTP(ofs,"force",force,"calculate the force or not");
        OUTP(ofs,"force_set",force_set,"output the force_set or not"); 
	OUTP(ofs,"nstep",nstep,"number of ion iteration steps");
	OUTP(ofs,"out_stru",out_stru,"output the structure files after each ion step");
	OUTP(ofs,"force_thr",force_thr,"force threshold, unit: Ry/Bohr");
	OUTP(ofs,"force_thr_ev",force_thr*13.6058/0.529177,"force threshold, unit: eV/Angstrom");
	OUTP(ofs,"force_thr_ev2",force_thr_ev2,"force invalid threshold, unit: eV/Angstrom");
	OUTP(ofs,"bfgs_w1",bfgs_w1,"wolfe condition 1 for bfgs");
	OUTP(ofs,"bfgs_w2",bfgs_w2,"wolfe condition 2 for bfgs");
	OUTP(ofs,"trust_radius_max", trust_radius_max,"maximal trust radius, unit: Bohr");
	OUTP(ofs,"trust_radius_min", trust_radius_min,"minimal trust radius, unit: Bohr");
	OUTP(ofs,"trust_radius_ini", trust_radius_ini,"initial trust radius, unit: Bohr");
	OUTP(ofs,"stress",stress,"calculate the stress or not");
	OUTP(ofs,"move_method",ion_dynamics,"bfgs; sd; cg; cg_bfgs;"); //pengfei add 2013-08-15
	OUTP(ofs,"out_level",out_level,"ie(for electrons); i(for ions);");
	OUTP(ofs,"out_dm",out_dm,">0 output density matrix");

	ofs << "\n#Parameters (4.LCAO)" << endl;
	//OUTP(ofs,"local_basis",local_basis,"0:PW; 1:LO in pw; 4:LCAO"); xiaohui modify 2013-09-01
	OUTP(ofs,"basis_type",basis_type,"PW; LCAO in pw; LCAO"); //xiaohui add 2013-09-01
	//OUTP(ofs,"linear_scaling",linear_scaling,"0:PW 1:LCAO 2:DMM"); xiaohui modify 2013-09-01
	//if(diago_type=="HPSEPS") xiaohui modify 2013-09-01
	if(ks_solver=="HPSEPS") //xiaohui add 2013-09-01
	{
		OUTP(ofs,"nb2d",nb2d,"2d distribution of atoms");
	}
	OUTP(ofs,"search_radius",search_radius,"input search radius (Bohr)");
	OUTP(ofs,"search_pbc",search_pbc,"input periodic boundary condition");
	OUTP(ofs,"lcao_ecut",lcao_ecut,"energy cutoff for LCAO");
	OUTP(ofs,"lcao_dk",lcao_dk,"delta k for 1D integration in LCAO");
	OUTP(ofs,"lcao_dr",lcao_dr,"delta r for 1D integration in LCAO");
	OUTP(ofs,"lcao_rmax",lcao_rmax,"max R for 1D two-center integration table");
	OUTP(ofs,"out_hs",out_hs,"output H and S matrix");
	OUTP(ofs,"out_lowf",out_lowf,"ouput LCAO wave functions");
	OUTP(ofs,"bx",bx,"division of an element grid in FFT grid along x");
	OUTP(ofs,"by",by,"division of an element grid in FFT grid along y");
	OUTP(ofs,"bz",bz,"division of an element grid in FFT grid along z");

	ofs << "\n#Parameters (5.Smearing)" << endl;
	//OUTP(ofs,"occupations",occupations,"fixed; smearing");
	OUTP(ofs,"smearing",smearing,"type of smearing: gauss; fd; fixed; mp; mp2");
	OUTP(ofs,"sigma",degauss,"energy range for smearing");
	
	ofs << "\n#Parameters (6.Charge Mixing)" << endl;
//2015-06-15
        OUTP(ofs,"mixing_type",mixing_mode,"plain; kerker; pulay; pulay-kerker");
	OUTP(ofs,"mixing_beta",mixing_beta,"mixing parameter: 0 means no new charge");
	OUTP(ofs,"mixing_ndim",mixing_ndim,"mixing dimension in pulay");
	OUTP(ofs,"mixing_gg0",mixing_gg0,"mixing parameter in kerker");

	ofs << "\n#Parameters (7.DOS)" << endl;
	OUTP(ofs,"dos_emin_ev",dos_emin_ev,"minimal range for dos");
	OUTP(ofs,"dos_emax_ev",dos_emax_ev,"maximal range for dos");
	OUTP(ofs,"dos_edelta_ev",dos_edelta_ev,"delta energy for dos");
        OUTP(ofs,"dos_sigma",b_coef,"gauss b coefficeinet(default=0.07)");
	
	ofs << "\n#Parameters (8.Technique)" << endl;
        OUTP(ofs,"gamma_only",gamma_only,"gamma only");
	//OUTP(ofs,"gamma_only_local",gamma_only_local,"gamma only in LCAO (important)");
	OUTP(ofs,"diago_proc",DIAGO_PROC,"number of proc used to diago");//mohan add 2012-01-13
	//OUTP(ofs,"gamma_only_pw",gamma_only,"gamma only in pw");
	OUTP(ofs,"npool",npool,"number of pools for k points, pw only");
	OUTP(ofs,"sparse_matrix",sparse_matrix,"use sparse matrix, in DMM");
	OUTP(ofs,"atom_distribution",atom_distribution,"distribute atoms, in DMM");
	OUTP(ofs,"mem_saver",mem_saver,"memory saver for many k points used");
	OUTP(ofs,"printe",printe,"print band energy for selectively ionic steps");

//	ofs << "\n#Parameters (11.Divide&Conqure)" << endl;
//	OUTP(ofs,"DC_nx",dc_nx,"division of atoms along x");
//	OUTP(ofs,"DC_ny",dc_ny,"division of atoms along y");
//	OUTP(ofs,"DC_nz",dc_nz,"division of atoms along z");

	ofs << "\n#Parameters (9.SIAO)" << endl;
	OUTP(ofs,"selinv_npole",selinv_npole,"number of selected poles");
	OUTP(ofs,"selinv_temp",selinv_temp,"temperature for Fermi-Dirac distribution");
	OUTP(ofs,"selinv_gap",selinv_gap,"supposed gap in the calculation");
	OUTP(ofs,"selinv_deltae",selinv_deltae,"expected energy range");
	OUTP(ofs,"selinv_mu",selinv_mu,"chosen mu as Fermi energy");
	OUTP(ofs,"selinv_threshold",selinv_threshold,"threshold for calculated electron number");
	OUTP(ofs,"selinv_niter",selinv_niter,"max number of steps to update mu");

	ofs << "\n#Parameters (10.Molecular dynamics)" << endl;
/*	
	OUTP(ofs,"md_dt",md_dt,"time step for molecular dynamics");
	OUTP(ofs,"md_restart",md_restart,"restart molecular dynamics from previous steps.");
	OUTP(ofs,"md_thermostat",md_thermostat,"ionic temperature: various md_thermostat");
	OUTP(ofs,"md_temp0",md_temp0,"start temperature");
	OUTP(ofs,"md_tolv",md_tolv,"tolerence for velocity scaling");
	OUTP(ofs,"md_tstep",md_tstep,"the temperature will reduce every md_tstep");
	OUTP(ofs,"md_delt",md_delt,"the reduce amount of temperature");
*/
//added by zheng daye
        OUTP(ofs,"md_mdtype",md_mdtype,"choose ensemble");
	//OUTP(ofs,"md_tauthermo",md_tauthermo,);
        //OUTP(ofs,"md_taubaro",md_taubaro,);
        OUTP(ofs,"md_dt",md_dt,"time step");
        OUTP(ofs,"md_nresn",md_nresn,"parameter during integrater");
        OUTP(ofs,"md_nyosh",md_nyosh,"parameter during integrater");
        OUTP(ofs,"md_qmass",md_qmass,"mass of thermostat");
        OUTP(ofs,"md_tfirst",md_tfirst,"temperature first");
        OUTP(ofs,"md_tlast",md_tlast,"temperature last");
        OUTP(ofs,"md_dumpmdfred",md_dumpmdfred,"The period to dump MD information for monitoring and restarting MD");
        OUTP(ofs,"md_mdoutpath",md_mdoutpath,"output path of md");
        OUTP(ofs,"md_domsd",md_domsd,"whether compute <r(t)-r(0)>");
        OUTP(ofs,"md_domsdatom",md_domsdatom,"whether compute msd for each atom");
        OUTP(ofs,"md_rstmd",md_rstmd,"whether restart");
        //OUTP(ofs,"md_outputstressperiod",md_outputstressperiod,"period to output stress");
        OUTP(ofs,"md_fixtemperature",md_fixtemperature,"period to change temperature");
        OUTP(ofs,"md_ediff",md_ediff,"parameter for constraining total energy change");
        OUTP(ofs,"md_ediffg",md_ediffg,"parameter for constraining max force change");
        OUTP(ofs,"md_msdstarttime",md_msdstartTime,"choose which step that msd be calculated");
//end of zheng daye's adding

	ofs << "\n#Parameters (11.Efield)" << endl;
	OUTP(ofs,"efield",efield,"add electric field");
	OUTP(ofs,"edir",edir,"add electric field");
	OUTP(ofs,"emaxpos",emaxpos,"maximal position of efield [0,1)");
	OUTP(ofs,"eopreg",eopreg,"where sawlike potential decrease");
	OUTP(ofs,"eamp",eamp,"amplitute of the efield, unit is a.u.");
	OUTP(ofs,"eamp_v",eamp*51.44,"amplitute of the efield, unit is V/A");

	ofs << "\n#Parameters (12.Bfield)" << endl;
	OUTP(ofs,"bfield",bfield,"add magnetic field");
	OUTP(ofs,"bfield_teslax",bfield_teslax,"magnetic field strength");
	OUTP(ofs,"bfield_teslay",bfield_teslay,"magnetic field strength");
	OUTP(ofs,"bfield_teslaz",bfield_teslaz,"magnetic field strength");
	OUTP(ofs,"bfield_gauge_x",bfield_gauge_x,"magnetic field gauge origin");
	OUTP(ofs,"bfield_gauge_y",bfield_gauge_y,"magnetic field gauge origin");
	OUTP(ofs,"bfield_gauge_z",bfield_gauge_z,"magnetic field gauge origin");

	ofs << "\n#Parameters (13.Test)" << endl;
	OUTP(ofs,"out_alllog",out_alllog,"output information for each processor, when parallel");
	OUTP(ofs,"nurse", nurse,"for coders");
	OUTP(ofs,"colour", colour,"for coders, make their live colourful");
	OUTP(ofs,"t_in_h", t_in_h,"calculate the kinetic energy or not");
	OUTP(ofs,"vl_in_h", vl_in_h,"calculate the local potential or not");
	OUTP(ofs,"vnl_in_h", vnl_in_h,"calculate the nonlocal potential or not");
	OUTP(ofs,"zeeman_in_h", zeeman_in_h,"calculate the zeeman term or not");
	OUTP(ofs,"test_force", test_force, "test the force");
	
#ifdef __EPM_
	OUTP(ofs,"fs_ref_energy",fs_ref_energy);
#endif	

	ofs << "\n#Parameters (14.Other Methods)" << endl;
	OUTP(ofs,"mlwf_flag",mlwf_flag,"turn MLWF on or off");
	OUTP(ofs,"opt_epsilon2",opt_epsilon2,"calculate the dielectic function");
	OUTP(ofs,"opt_nbands",opt_nbands,"number of bands for optical calculation");
//	OUTP(ofs,"berry_phase",berry_phase);
//	OUTP(ofs,"lda_plus_u",lda_plus_u);
	
	ofs << "\n#Parameters (15.vdw-D2)" << endl;												//Peize Lin add 2014-04-05, update 2015-09-30
	OUTP(ofs,"vdwD2",vdwD2,"calculate vdw-D2 or not");
	OUTP(ofs,"vdwD2_scaling",vdwD2_scaling,"scaling of vdw-D2");
	OUTP(ofs,"vdwD2_d",vdwD2_d,"damping parameter");
	OUTP(ofs,"vdwD2_C6_file",vdwD2_C6_file,"filename of C6");
	OUTP(ofs,"vdwD2_C6_unit",vdwD2_C6_unit,"unit of C6, Jnm6/mol or eVA6");
	OUTP(ofs,"vdwD2_R0_file",vdwD2_R0_file,"filename of R0");
	OUTP(ofs,"vdwD2_R0_unit",vdwD2_R0_unit,"unit of R0, A or Bohr");
	OUTP(ofs,"vdwD2_model",vdwD2_model,"expression model of periodic structure, radius or period");
	OUTP(ofs,"vdwD2_radius",vdwD2_radius,"radius cutoff for periodic structure");
	OUTP(ofs,"vdwD2_radius_unit",vdwD2_radius_unit,"unit of radius cutoff for periodic structure");	
	ofs << setw(20) << "vdwD2_period" << vdwD2_period[0] << " " << vdwD2_period[1] << " " << vdwD2_period[2]<< " #periods of periodic structure" << endl;

	
    ofs.close();
    return;
}

void Input::close_log(void)const
{
	
    Global_File::close_all_log(MY_RANK, this->out_alllog);
}

void Input::readbool(ifstream &ifs, bool &var)
{
    string str;
    ifs >> str;
    if (str == "true")
    {
        var = true;
    }
    else
    {
        var = false;
    }
    ifs.ignore(100, '\n');
    return;
}

void Input::strtolower(char *sa, char *sb)
{
    char c;
    int len = strlen(sa);
    for (int i = 0; i < len; i++)
    {
        c = sa[i];
        sb[i] = tolower(c);
    }
    sb[len] = '\0';
}

