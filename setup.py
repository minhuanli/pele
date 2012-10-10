from numpy.distutils.core import setup
from numpy.distutils.core import Extension

setup(name='PyGMIN', 
        version='0.1', 
        description="Pythin implementation of GMIN, OPTIM, and PATHSAMPLE",
        url='http://github.com/js850/minGMIN.git',
        packages=["pygmin", 
            "pygmin.potentials",
            "pygmin.gui",
            "pygmin.mindist",
            "pygmin.optimize",
            "pygmin.printing",
            "pygmin.takestep",
            "pygmin.utils",
            "pygmin.wham"
            ],
      ext_modules= [
            Extension("pygmin.mindist.overlap", 
                      ["pygmin/mindist/overlap.f90"]),
                    
            Extension("pygmin.optimize.mylbfgs_fort", 
                      ["pygmin/optimize/mylbfgs_fort.f90"]),
            Extension("pygmin.optimize.mylbfgs_updatestep", 
                      ["pygmin/optimize/mylbfgs_updatestep.f90"]),
                    
            Extension("pygmin.potentials.fortran.AT", 
                      ["pygmin/potentials/fortran/AT.f"]),
            #Extension("pygmin.potentials.fortran.lj_hess", 
            #          ["pygmin/potentials/fortran/lj_hess.f"]),
            Extension("pygmin.potentials.fortran.ljpshiftfort", 
                      ["pygmin/potentials/fortran/ljpshiftfort.f"]),
            Extension("pygmin.potentials.fortran.lj", 
                      ["pygmin/potentials/fortran/lj.f90"]),
            Extension("pygmin.potentials.fortran.rmdrvt", 
                      ["pygmin/potentials/fortran/rmdrvt.f90"]),
            Extension("pygmin.potentials.fortran.soft_sphere_pot", 
                      ["pygmin/potentials/fortran/soft_sphere_pot.f90"]),
                    
            Extension("pygmin.utils._fortran_utils", 
                      ["pygmin/utils/_fortran_utils.f90"]),
                ]
        )

