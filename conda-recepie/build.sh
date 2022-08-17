mkdir build
mkdir install
cd build
cmake .. \
      -DCMAKE_PREFIX_PATH=$CONDA_PREFIX \
      -DCMAKE_INSTALL_PREFIX=install \
      -DREUSS_BUILD_DETECTOR=OFF \
      -DREUSS_BUILD_PYTHON=ON \
      -DREUSS_TUNE_LOCAL=OFF \
     
NCORES=$(getconf _NPROCESSORS_ONLN)
echo "Building using: ${NCORES} cores"
cmake --build . -- -j${NCORES}
cmake --build . --target install