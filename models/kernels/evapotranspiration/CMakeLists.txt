cmake_minimum_required(VERSION 3.10)
add_library(kernels_et STATIC
        Et_main.cpp
        )
add_library(NGen::kernels_et ALIAS kernels_et)
target_include_directories(kernels_et PUBLIC
        /glade/work/jframe/ngen/models/kernels/evapotranspiration
        )
