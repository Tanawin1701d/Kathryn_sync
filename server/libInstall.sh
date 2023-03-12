sudo apt update
sudo apt install build-essential git m4 scons zlib1g zlib1g-dev \
    libprotobuf-dev protobuf-compiler libprotoc-dev libgoogle-perftools-dev \
    python3-dev python-is-python3 libboost-all-dev pkg-config
sudo apt install libmysqlcppconn-dev
sudo apt install mysql-client
sudo apt-get install -y libasio-dev
cmake .. -DCROW_BUILD_EXAMPLES=OFF -DCROW_BUILD_TESTS=OFF -DCROW_FEATURES="compression"

sudo make install