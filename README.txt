###################################################################
#                                                                 #
# Adobe Boxes: Locating Object Proposals using Object Adobes V1.0 #
#                                                                 #
#                     Accepted by TIP                             #
#                                                                 #
###################################################################

The software was developed under windows with Matlab 2014a and VS2013.
- The code has been test on 64-bit Windows 7 and 64-bit Windows 10 systems respectively.
- 64-bit Matlab is required. 

1. Introduction.

Adobe Boxes are used to efficiently locate the potential objects with
fewer proposals, in terms of searching the object adobes which are 
the salient object ¡°parts¡±easy to be perceived. Because of the visual
difference between the object and its surroundings, an object adobe 
obtained from the local region has a high probability to be a part of
an object, which is capable of depicting the locative information
of the proto-object. 

###################################################################

2. License.

Copyright (C) 2016 Fang zhiwen 

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA. 

###################################################################


3. Installation.

- Run "compile.m" to compile the .cpp files.

4. Getting Started.

- Make sure the .cpp files are compiled successfully.
- Please load the default parameters and update the paths of the test images in "default_parameters.m".
- Please see "DemoforImage.m" and "DemoforDataset.m" to run demos.
- Please set "cfg.mode" in "default_parameters.m" to switch the working models while running on datasets. 

5. Others
- Our software only needs the images on datasets. Other files have been saved as mat-files. 
These images can be downloaded from the dataset homepages or the following links.
  Images for testing on VOC2007: http://pan.baidu.com/s/1jIhf9aA          
                       Homepage: http://pascallin.ecs.soton.ac.uk/challenges/VOC/voc2007/workshop/index.html
                                 # Test images are used for testing.
  Images for testing on VOC2010: http://pan.baidu.com/s/1gfFH8Ev
                       Homepage: http://pascallin.ecs.soton.ac.uk/challenges/VOC/voc2010/workshop/index.html
                                 # Validation images are used for testing.
  Images for testing on VOC2012: http://pan.baidu.com/s/1o7WieeA
                       Homepage: http://pascallin.ecs.soton.ac.uk/challenges/VOC/voc2012/workshop/index.html
                                 # Validation images are used for testing.
  Images for testing on ImageNet: part1 : http://pan.baidu.com/s/1o86FR4I
                                  part2 : http://pan.baidu.com/s/1nuDnqIX
                       Homepage: http://image-net.org/challenges/LSVRC/2014/download-images-5jj5.php
                                 # Validation images on CLS-LOC dataset are used for testing.

- If you use our codes, please cite our papers.








