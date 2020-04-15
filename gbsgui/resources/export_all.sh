#/bin/sh

# export all bitmap from .gam file to header file
# author: Nguyen Chi Thuc
# email:  gthuc.nguyen@gmail.com
# last updated: 2012-05-11 14:34:49

rm images.hpp
rm images_extern.hpp
for f in images/normal/*.gam; do ./gam2h "$f" -a images.hpp -e images_extern.hpp; done
for f in images/transparent/*.gam; do ./gam2h "$f" -a images.hpp -e images_extern.hpp -t; done
for f in images/games/ChineseChess/*.gam; do ./gam2h "$f" -a images.hpp -e images_extern.hpp -t; done
