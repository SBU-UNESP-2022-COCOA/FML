# Conda Environment (trial) - by V. Miranda

     conda create --name cola python=3.7 --quiet --yes && \
     conda install -n cola --quiet --yes  \
       'conda-forge::libgcc-ng=10.3.0' \
       'conda-forge::libstdcxx-ng=10.3.0' \
       'conda-forge::libgfortran-ng=10.3.0' \
       'conda-forge::gxx_linux-64=10.3.0' \
       'conda-forge::gcc_linux-64=10.3.0' \
       'conda-forge::gfortran_linux-64=10.3.0' \
       'conda-forge::openmpi=4.1.1' \
       'conda-forge::git=2.33.1' \
       'conda-forge::git-lfs=3.0.2' \
       'conda-forge::cmake=3.21.3' \
       'conda-forge::gsl=2.7' \
       'conda-forge::openblas=0.3.18' \
       'conda-forge::lapack=3.9.0' \
       'conda-forge::cython=0.29.24' \
       'conda-forge::numpy=1.21.4' \
       'conda-forge::scipy=1.7.2' \
       'conda-forge::pandas=1.3.4' \
       'conda-forge::mpi4py=3.1.3' \
       'conda-forge::matplotlib=3.5.0' \
       'conda-forge::astropy=4.3.1' \
       'conda-forge::lua=5.4.4' \
       'conda-forge::cgal=5.4' \
       'conda-forge::fftw=3.3.8=mpi_openmpi_h50a73e0_1014'

With this installation method, users must activate the cola environment whenever working with Cola, as shown below 

    $ conda activate cola

Cola is made aware of the chosen installation method of required packages via special environment keys located on the [set_installation_options](https://github.com/SBU-UNESP-2022-COCOA/FML/blob/master/set_installation_options) script, as shown below

    [Extracted from set_installation_options script]
    #  ---------------------------------------------------------------------------
    # HOW COLA SHOULD BE INSTALLED? -------------------------------

    #export DOCKER_INSTALLATION=1
    export MINICONDA_INSTALLATION=1
    
The user must uncomment the appropriate key, and then type the following command

    $(cola) source install_cola_local_environment

Finally, type

    $(cola) source compile_cola
    
to compile Cola

## Running Cola Examples

**Step 1 of 5**: activate the conda environment

    $ conda activate cola

**Step 2 of 5**: activate the private python environment

    $(cola) source start_cola

(**warning**) Users will see a terminal that looks like this: `$(cola)(.local)`. *This is a feature, not a bug*! 

(**expert**) Why `$(cola)(.local)` is a feature, not a bug? The Cola environment can be the same for all Cola instances, with [start_cola](https://github.com/SBU-UNESP-2022-COCOA/FML/blob/master/start_cola)/[stop_cola](https://github.com/SBU-UNESP-2022-COCOA/FML/blob/master/start_cola) loading/unloading the corresponding `LD_LIBRARY_PATH`, `CPATH`, `C_INCLUDE_PATH`, `CPLUS_INCLUDE_PATH` and `PATH`. *Why more than one Cola instance?* While users may be running chains in one instance, they might use a second instantiation to make experimental changes.

**Step 4 of 5**: select the number of OpenMP cores
    
    $(cola)(.local) export OMP_PROC_BIND=close
    $(cola)(.local) export OMP_NUM_THREADS=1
    
**Step 5 of 5**: run cola

    $(cola)(.local) mpirun -n 1 --mca btl tcp,self --bind-to core --rank-by core --map-by numa:pe=${OMP_NUM_THREADS} ./FML/COLASolver/nbody
    
# FML

For documentation and more info about the library see the [website](https://fml.wintherscoming.no/). See also the [Doxygen documentation](https://fml.wintherscoming.no/doxygen/).

# About
A C++ library for working with particles and grids and solving PDEs in parallel with up to thousands of tasks. The library uses MPI, OpenMP (or both) for the parallelization, though everything also works without a MPI compiler. We take care of most of the communication between tasks under the cover so the user don't have to deal too much with that. It is made mainly for analyzing the large scale structure of the Universe, but it is written in general so it can be useful outside of cosmology. It is not meant to be a replacement for common cosmology codes that does similar things (like initial condition generation, simple N-body simuations, halo and void finders, reconstruction etc.) although these things are very easy to do with the library. The goal of the library is rather provide users with the basic tools needed to working in parallel so that one don't have to start by making the tools before doing the science. The philosophy of the library is to implement general algorithms without making (too many) assumptions about how they are to be used and let the user take all these decitions thus making it easier to do new things without having to modify the whole code. The classes we provide are therefore templated on dimension (well knowing that nobody is going to use it for anything else than N=2 or N=3) and the types and the algorithms are templated on classes that determine how to process and compile up the data it computes. To be able to do Fourier transforms we settle for a simple slab-based domain decomposition for the parallelization. The library contains algorithms for assigning particles to grids, linking particles together in groups, general watershed, tesselations, fourier transforms, computing correlation function a general multigrid solver and many more things. We also provide wrappers of methods in libraries like GSL, CGAL and LUA to more easily being able to perform various tasks.

# Requirements
You will need atleast a C++14 compatible compiler to compile the library, but C++17 (-std=c++1z so gcc 7, clang 3.9, icpc 19, MSVC 19.11) is highly reccomended as we use a lot of if constexpr to do compile-time computations to make things more efficient. With some small tweeks it is also possible to just use a C++11 compiler, but too much work for me to bother. Parts of the code can also be compiled without any external libraries, but to take full advantage of the features in the code you should atleast have the FFTW3 library (with MPI support) and GSL installed. In addition to this the other two libraries we sometimes use are CGAL and LUA. You should also have a MPI complient compiler though the code can be compiled and run in serial.

 - [FFTW](http://www.fftw.org/download.html) version 3+ : Required to be able to do Fourier transforms and the related algorithms that use Fourier transforms. The grid class FFTWGrid can be compiled without it, but you won't be able to perform Fourier transforms or use any algorithms that relies on this.
 - [CGAL](https://www.cgal.org/download.html) version 5+ : Required to do tesselations and the related algorithms that rely on this.
 - [GSL](https://www.gnu.org/software/gsl/) version 2+ : Required to solve ODEs, make splines, random numbers (though if we don't have this we use C++ <random> instead) and linear algebra (just a few places).
 - [LUA](https://www.lua.org/download.html) version 5+ : Required to use LuaFileParser to read parameterfiles. We don't use this much.

# Compiling

Most of the library is in forms of header files that you just need to include in your code. A few of these needs to be compiled. We could have compiled this up to a shared library, but given the options the user have (with MPI or without, with some external libraries or without, etc.) its better to compiler these files together with your code. For this see the included Makefiles for how to do this. Having tons of define statements in the code sucks, but its the only good/normal way of being able to use the code without having all the features. There are many compile-time defines you can use to customize the library, here is a list of the most important ones:

 - USE\_MPI : Compile with MPI support. You will need to have a MPI compiler to use this.
 - USE\_OMP : Compile with OpenMP support. Can be used together with MPI for which each MPI task will use OMP\_NUM\_THREADS threads in certain loops.
 - USE\_FFTW : If you have the FFTW3 library. You will have to provide the include and lib paths in the Makefile. If used together with USE\_MPI you will need to have compiled the FFTW3 library with MPI support.
 - USE\_FFTW\_THREADS : If you have the FFTW3 library. Use threads to parallelize the FFTs. Can be used together with USE\_MPI.
 - USE\_GSL : If you have the GSL library. You will have to provide the include and lib paths in the Makefile.
 - USE\_CGAL : If you have the CGAL library. You will have to provide the include and lib paths in the Makefile.
 - USE\_LUA : If you have the LUA library. You will have to provide the include and lib paths in the Makefile.
 - USE\_PYTHON : If you have Python with Matplotlib installed. You will have to provide the include and lib paths in the Makefile.

Some define statements that can be added to change how the code works:

 - SINGLE\_PRECISION\_FFTW : Use float instead of double for FFTWGrid and FFTs in general. FFTW needs to be compiled with float support to use this.
 - LONG\_DOUBLE\_PRECISION\_FFTW : Use long double instead of double for FFTWGrid and FFTs in general. FFTW needs to be compiled with long double support to use this.
 - NO\_AUTO\_FML\_SETUP : FML is automatically initialized and finalized in the code. This includes taking care of MPI and FFTW initialization. If you don't want this add this define. NB: if you use this you should know how FFTW should be initialized with MPI and/or threads to avoid issues. You will then also *need* to call FML::init\_fml() after initializing MPI and FFTW to ensure that the few global variables we use are set.
 - USE\_MEMORYLOG : Log allocations over a certain size (see the MemoryLog header). Useful to map out the memory footprint of the code. Currently only a few classes implement this.

In addition to these we have some Makefile options that can be useful:

 - USE\_SANITIZER : Compile with -fsanitize=address to check for bad memory accesses. Useful for debugging.
You will also have to provide the include path to the folder containing FML and in VPATH provide the path to the cpp files that needs to be compiled with the code.
 - USE\_DEBUG : Do some extra asserts, print some more info from the algorithms while running.

# Examples

Almost every folders within FML contains examples, e.g. [FFTWGrid/example](https://github.com/HAWinther/FML/tree/master/FML/FFTWGrid/example) ; [MultigridSolver/examples](https://github.com/HAWinther/FML/tree/master/FML/MultigridSolver/examples) ; [MPIParticles/example](https://github.com/HAWinther/FML/tree/master/FML/MPIParticles/example) etc., for examples on how to use the different methods and classes. 

# Known issues

On some of the Intel compilers there is an issue with std::variant (which is used in ParameterMap.cpp). If compilation of this fails with the error (internal error: assertion failed: node\_has\_side\_effects: bad node kind) then one option is simply to comment out the body of info() in ParameterMap.cpp. This means you won't get to print the containts of the map, but everything else works well. Intel also does alot of agressive optimizations by default so using the compiler flag -fp-model=precise is reccomended as the code is mainly tested using clang and gcc.


