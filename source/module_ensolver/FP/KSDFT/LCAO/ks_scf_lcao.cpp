#include "ks_scf_lcao.h"

//--------------temporary----------------------------
#include "../../../../src_pw/global.h"
#include "../../../../module_base/global_function.h"
#include "src_io/print_info.h"

#ifdef __DEEPKS
#include "module_deepks/LCAO_deepks.h"
#endif
//-----force-------------------

//-----stress------------------

//---------------------------------------------------

namespace ModuleEnSover
{

void KS_SCF_LCAO::Init(Input &inp, UnitCell_pseudo &ucell)
{

	// setup GlobalV::NBANDS
	// Yu Liu add 2021-07-03
	GlobalC::CHR.cal_nelec();

	// mohan add 2010-09-06
	// Yu Liu move here 2021-06-27
	// because the number of element type
	// will easily be ignored, so here
	// I warn the user again for each type.
	for(int it=0; it<ucell.ntype; it++)
	{
		GlobalC::xcf.which_dft(ucell.atoms[it].dft);
	}

    //ucell.setup_cell( GlobalV::global_pseudo_dir , GlobalV::global_atom_card , GlobalV::ofs_running, GlobalV::NLOCAL, GlobalV::NBANDS);
    ModuleBase::GlobalFunc::DONE(GlobalV::ofs_running, "SETUP UNITCELL");

    // symmetry analysis should be performed every time the cell is changed
    if (ModuleSymmetry::Symmetry::symm_flag)
    {
        GlobalC::symm.analy_sys(ucell, GlobalV::ofs_running);
        ModuleBase::GlobalFunc::DONE(GlobalV::ofs_running, "SYMMETRY");
    }

    // Setup the k points according to symmetry.
    GlobalC::kv.set(GlobalC::symm, GlobalV::global_kpoint_card, GlobalV::NSPIN, ucell.G, ucell.latvec );
    ModuleBase::GlobalFunc::DONE(GlobalV::ofs_running,"INIT K-POINTS");

    // print information
    // mohan add 2021-01-30
    Print_Info::setup_parameters(ucell, GlobalC::kv, GlobalC::xcf);

//--------------------------------------
// cell relaxation should begin here
//--------------------------------------

    // Initalize the plane wave basis set
    GlobalC::pw.gen_pw(GlobalV::ofs_running, ucell, GlobalC::kv);
    ModuleBase::GlobalFunc::DONE(GlobalV::ofs_running,"INIT PLANEWAVE");
    std::cout << " UNIFORM GRID DIM     : " << GlobalC::pw.nx <<" * " << GlobalC::pw.ny <<" * "<< GlobalC::pw.nz << std::endl;
    std::cout << " UNIFORM GRID DIM(BIG): " << GlobalC::pw.nbx <<" * " << GlobalC::pw.nby <<" * "<< GlobalC::pw.nbz << std::endl;

    // initialize the real-space uniform grid for FFT and parallel
    // distribution of plane waves
    GlobalC::Pgrid.init(GlobalC::pw.ncx, GlobalC::pw.ncy, GlobalC::pw.ncz, GlobalC::pw.nczp,
        GlobalC::pw.nrxx, GlobalC::pw.nbz, GlobalC::pw.bz); // mohan add 2010-07-22, update 2011-05-04
	// Calculate Structure factor
    GlobalC::pw.setup_structure_factor();

	// Inititlize the charge density.
    GlobalC::CHR.allocate(GlobalV::NSPIN, GlobalC::pw.nrxx, GlobalC::pw.ngmc);
    ModuleBase::GlobalFunc::DONE(GlobalV::ofs_running,"INIT CHARGE");

	// Initializee the potential.
    GlobalC::pot.allocate(GlobalC::pw.nrxx);
    ModuleBase::GlobalFunc::DONE(GlobalV::ofs_running,"INIT POTENTIAL");


	// Peize Lin add 2018-11-30
#ifdef __MPI 
	if(GlobalV::CALCULATION=="nscf")
	{
		switch(GlobalC::exx_global.info.hybrid_type)
		{
			case Exx_Global::Hybrid_Type::HF:
			case Exx_Global::Hybrid_Type::PBE0:
			case Exx_Global::Hybrid_Type::HSE:
				GlobalC::exx_global.info.set_xcfunc(GlobalC::xcf);
				break;
		}
	}
#endif

#ifdef __DEEPKS
	//wenfei 2021-12-19
	//if we are performing DeePKS calculations, we need to load a model
	if (GlobalV::out_descriptor)
	{
		if (GlobalV::deepks_scf)
		{
			// load the DeePKS model from deep neural network
    		GlobalC::ld.load_model(INPUT.model_file);
		}
	}
#endif

}

void KS_SCF_LCAO::Run(
    int istep, 
    Record_adj& ra,
    Local_Orbital_Charge& loc,
    Local_Orbital_wfc& lowf,
    LCAO_Hamilt& uhm)
{
    this->LOE.solve_elec_stru(istep, ra, loc, lowf, uhm);
    return ;
}

void KS_SCF_LCAO::cal_Energy(energy &en)
{

}

void KS_SCF_LCAO::cal_Force(ModuleBase::matrix &force)
{

}
void KS_SCF_LCAO::cal_Stress(ModuleBase::matrix &stress)
{

}

}