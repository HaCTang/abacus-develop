#include "read_pp.h"
#include "module_base/atom_in.h"
#include "module_base/element_name.h"

int Pseudopot_upf::read_pseudo_blps(std::ifstream &ifs)
{
    // double bohr2a = 0.529177249;
    this->nlcc = false;
    this->tvanp = false;
    this->has_so = false;

    this->nbeta = 0;
    this->kkbeta = 0;
    delete[] kbeta;
    delete[] lll;
    this->kbeta = nullptr;
    this->lll = nullptr;
    this->beta.create(1, 1);
    this->dion.create(1, 1);

    this->nwfc = 0;
    delete[] nn;
    delete[] jchi;
    delete[] jjj;
    this->nn = new int[1];
    this->jchi = new double[1];
    this->jjj = new double[1];
    ModuleBase::GlobalFunc::ZEROS(nn, 1);
    ModuleBase::GlobalFunc::ZEROS(jchi, 1);
    ModuleBase::GlobalFunc::ZEROS(jjj, 1);

    ifs >> this->psd;
    // if(!SCAN_BEGIN(ifs,"BLPS")) WARNING_QUIT("read_pp_blps","Find no PP_HEADER");
    ifs.ignore(300, '\n');

    double zatom;
    double zion;
    ifs >> zatom >> zion;
    this->zp = static_cast<int>(zion);
    ifs.ignore(300, '\n');

    atom_in ai;
    for (auto each_type:  ModuleBase::element_name)
    {
        if (zatom == ai.atom_Z[each_type])
        {
            this->psd = each_type;
            break;
        }
    }

    int pspcod, pspxc, lloc, r2well;
    ifs >> pspcod >> pspxc >> this->lmax >> lloc >> this->mesh >> r2well;

    if (pspxc == 2)
    {
        this->xc_func = "PZ";
    }
    else if (pspxc == 11)
    {
        this->xc_func = "PBE";
    }
    else
    {
        std::string msg = "Unknown pspxc: " + std::to_string(pspxc);
        ModuleBase::WARNING_QUIT("Pseudopot_upf::read_pseudo_blps", msg);
    }

    if (pspcod == 8)
    {
        for (int i = 0; i < 5; ++i)
        {
            ifs.ignore(300, '\n');
        }
    }
    else if (pspcod == 6)
    {
        for (int i = 0; i < 17; ++i)
        {
            ifs.ignore(300, '\n');
        }
    }
    else
    {
        std::string msg = "Unknown pspcod: " + std::to_string(pspcod);
        ModuleBase::WARNING_QUIT("Pseudopot_upf::read_pseudo_blps", msg);
    }

    assert(mesh > 0);

    delete[] r;
    delete[] rab;
    delete[] vloc;
    this->r = new double[mesh]; // Bohr
    this->rab = new double[mesh];
    this->vloc = new double[mesh]; // Hartree
    ModuleBase::GlobalFunc::ZEROS(r,mesh);
    ModuleBase::GlobalFunc::ZEROS(rab,mesh);
    ModuleBase::GlobalFunc::ZEROS(vloc,mesh);
    int num = 0;
    if (pspcod == 8)
    {
        for(int i = 0;i < mesh; ++i)
        {
            ifs >> num >> this->r[i] >> this->vloc[i];
            this->vloc[i] = this->vloc[i]*2; // Hartree to Ry
        }
    }
    else if (pspcod == 6)
    {
        double temp = 0.;
        for(int i = 0;i < mesh; ++i)
        {
            ifs >> num >> this->r[i] >> temp >> this->vloc[i];
            this->vloc[i] = this->vloc[i]*2; // Hartree to Ry
        }
    }
    rab[0] = r[1] - r[0];
    for(int i = 1; i < mesh - 1; ++i)
    {
        rab[i] = (r[i+1] - r[i-1])/2.0;
    }
    rab[mesh-1] = r[mesh-1] - r[mesh-2];

    delete[] rho_at;
    this->rho_at = new double[mesh];
    ModuleBase::GlobalFunc::ZEROS(rho_at,mesh);
    double charge = zion/r[mesh-1];
    for(int i = 0;i < mesh; ++i)
    {
        rho_at[i] = charge;
    }
    return 0;
}

//parameters
//read_pp.h <--> blps_real
//nv             -
//psd            head
//pp_type(NC or US) -
//tvanp          False
//nlcc           False
//dft            pspxc 2->lda, 11->gga
//zp             zion
//etotps         -
//ecutwfc        -
//ecutrho        -
//lmax           lmax
//mesh           mmax
//nwfc           -
//nbeta          -
//els            -
//lchi           -
//oc             -

//rab            rab[ir]=(r[ir+1]-r[ir-1])/2.0
//rho_atc(nonlocal) -
//vloc
//chi            -
//rho_at         -

// lll            -
// kbeta         -
// beta           -
// dion           -

//nn             -
//jchi           -
//jjj            -
//nd             -
