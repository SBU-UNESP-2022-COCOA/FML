#ifndef COSMOLOGY_H_table_HEADER
#define COSMOLOGY_H_table_HEADER

#include <FML/FFTWGrid/FFTWGrid.h>
#include <FML/Global/Global.h>
#include <FML/LPT/DisplacementFields.h>
#include <FML/ODESolver/ODESolver.h>
#include <FML/ParameterMap/ParameterMap.h>
#include <FML/Spline/Spline.h>
#include <FML/FileUtils/FileUtils.h> // read table

#include "Cosmology.h"

using DVector = FML::INTERPOLATION::SPLINE::DVector;
using Spline = FML::INTERPOLATION::SPLINE::Spline;

// Htable

class Cosmology_H_table final : public Cosmology {
  public:
    Cosmology_H_table() { name = "H_table"; }
    
    std::string bg_input_filename; // The filename  

    Spline HoverH0_of_a_spline{"HoverH0(a)"}; 
    Spline dlogHdloga_of_a_spline{"dlogHdloga_of_a(a)"};

    //========================================================================
    // Read the parameters we need
    //========================================================================
    void read_parameters(ParameterMap & param) override {
    
        Cosmology::read_parameters(param);
        bg_input_filename = param.get<std::string>("bg_input_filename");
        
        //========================================================================
        // Begin table stuff
        //========================================================================

        // Fileformat: [a  HoverH0(a)  dlogHdloga_of_a(a)] (change below)
        
        const int ncols = 3;
        const int col_a = 0;
        const int col_H = 1;
        const int col_dlogHdloga = 2;
        
        std::vector<int> cols_to_keep{col_a, col_H, col_dlogHdloga};
        const int nheaderlines = 1;
        auto bg_of_a_data = FML::FILEUTILS::read_regular_ascii(bg_input_filename, ncols, cols_to_keep, nheaderlines);
	
	DVector aarr;
        DVector Harr;
        DVector dlogHdlogaarr;
        aarr.resize(bg_of_a_data.size());
        Harr.resize(bg_of_a_data.size());
        dlogHdlogaarr.resize(bg_of_a_data.size());
        	
        // Read in bgs function

        for (size_t i = 0; i < aarr.size(); i++) {
            aarr[i] = bg_of_a_data[i][0];
            Harr[i] = bg_of_a_data[i][1];
            dlogHdlogaarr[i] = bg_of_a_data[i][2];
        }
	HoverH0_of_a_spline.create(aarr, Harr, "HoverH0(a)");
	dlogHdloga_of_a_spline.create(aarr, dlogHdlogaarr, "dlogHdloga_of_a(a)");
        
    }

    //========================================================================
    // Initialize the cosmology
    //========================================================================
    void init() override { Cosmology::init(); }

    //========================================================================
    // Print some info
    //========================================================================
    void info() const override {
        Cosmology::info();
        if (FML::ThisTask == 0) {
            std::cout << "#=====================================================\n";
            std::cout << "# H(0.1)      : " << HoverH0_of_a(0.1) << "\n";
            std::cout << "# H_spline(0.1)      : " << HoverH0_of_a_spline(0.1) << "\n";
            std::cout << "\n";
        }
    }
    //========================================================================
    // Hubble function
    //========================================================================
    double HoverH0_of_a(double a) const override {
        return HoverH0_of_a_spline(a);
    }

    double dlogHdloga_of_a(double a) const override {
        return dlogHdloga_of_a_spline(a);
    }

};
#endif

