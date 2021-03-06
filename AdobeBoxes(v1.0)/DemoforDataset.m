%
%  Adobe Boxes: Locating Object Proposals using Object Adobes
%
%  Zhiwen Fang*#, Zhiguo Cao*, Yang Xiao*, Lei Zhu@ and Junsong Yuan&, 2015
%
%  * Huazhong University of Science and Technology
%  & Nanyang Technological University 
%  # Hunan University of Humanities, Science and Technology
%  @ Wuhan University of Science and Technology
%
% Demo for dataset
%% main_AdobeBoxes
clear; close all; clc
addpath(genpath('./utility/'));
addpath(genpath('./mex/'));
%%
datasetLabel = 3; % 1 for VOC 2007 % 2 for VOC2010 % 3 for VOC2012 % 4 for ImageNet2014
%%
fprintf('Loading parameters...\n');
cfg = default_parameters(datasetLabel);
fprintf('Finished!!\n');
%% get potential bounding boxes for images
fprintf('Running AdobeBoxes...\n');
[recall, objPotentialBoxes] = getObjBndBoxesForTestFast(cfg);

