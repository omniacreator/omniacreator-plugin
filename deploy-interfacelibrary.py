#!/usr/bin/env python

################################################################################
# @file
# Deploy Interface Library Script
#
# @version @n 1.0
# @date @n 5/8/2014
#
# @author @n Kwabena W. Agyeman
# @copyright @n (c) 2014 Kwabena W. Agyeman
# @n All rights reserved - Please see the end of the file for the terms of use
#
# @par Update History:
# @n v1.0 - Original release - 5/8/2014
################################################################################

import argparse, os, sys, shutil, subprocess

INSTALL_FOLDER = "share/qtcreator/libraries/InterfaceLibrary"
SRC_FOLDER = "interfacelibrary"

if __name__ == "__main__":

    __folder__ = os.path.dirname(os.path.abspath(__file__))

    parser = argparse.ArgumentParser(description =
    "Deploy Interface Library Script")

    parser.add_argument("install_folder", nargs = '?',
    default = os.getcwd())

    parser.add_argument("src_folder", nargs = '?',
    default = __folder__)

    args = parser.parse_args()

    install_folder = os.path.abspath(os.path.join(args.install_folder,
    INSTALL_FOLDER))

    src_folder = os.path.abspath(os.path.join(args.src_folder,
    SRC_FOLDER))

    if not os.path.exists(install_folder):
        os.makedirs(install_folder)

    if not os.path.exists(src_folder):
        sys.exit("Src Folder \"%s\" does not exist!" % src_folder)

    # Deploy ##################################################################

    os.chdir(src_folder)

    # Deploy Library #

    subprocess.check_call(["python", "preprocess.py",
    os.path.join(src_folder, "il.h"), # old file name
    os.path.join(install_folder, "InterfaceLibrary.h")]) # new file name

    # Deploy Documentation #

    subprocess.check_call(["doxygen", "Doxyfile"])
    shutil.rmtree(os.path.join(install_folder, "html"), True)
    shutil.move(os.path.join(src_folder, "html"),
                os.path.join(install_folder, "html"))

################################################################################
# @file
# @par MIT License - TERMS OF USE:
# @n Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"), to
# deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
# sell copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# @n
# @n The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
# @n
# @n THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
################################################################################
