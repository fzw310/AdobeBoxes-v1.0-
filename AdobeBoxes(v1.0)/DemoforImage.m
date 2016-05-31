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
%% Demo for image
clear all;
clc;

%% Images
im = imread('003542.jpg'); % load image
gtBox = [210,226,98,60; 137,225, 87,62; 36, 245, 114, 79; 229, 183, 57, 87; 153, 180, 60, 99; 100, 186, 54, 59; 61, 195, 77, 101];
% im = imread('002638.jpg'); % load image
% gtBox = [217,78,80,68; 212,174,134,201;129,79,92,88];
% im = imread('002793.jpg'); % load image
% gtBox = [1,255,144,39; 139, 250, 160,43; 331,94,139,99];

%%
addpath(genpath('./utility/'));
addpath(genpath('./mex/'));
%% load parameters
fprintf('Loading parameters...\n');
cfg = default_parameters(1);
fprintf('Finished!!\n');
%%
[row,col,~] = size(im);
%% Efficient segmentation
imageToSegment = cvtColor(im, 'RGB2HSV');%Our paper uses this code compiled from openCV.If this code can not work, please use the next one. 
% [~,imageToSegment] = Image2ColourSpace(im, 'Hsv');

[blobIndIm, blobBoxes, neighbours] = mexFelzenSegmentIndex(imageToSegment, cfg.sigma, cfg.k, cfg.minSize);
superPixelBoxes(:, 1) = blobBoxes(:, 2);
superPixelBoxes(:, 2) = blobBoxes(:, 1);
superPixelBoxes(:, 4) = blobBoxes(:, 3) - blobBoxes(:, 1) + 1;
superPixelBoxes(:, 3) = blobBoxes(:, 4) - blobBoxes(:, 2) + 1;

[colourHist,blobSizes] = BlobStructColourHist(blobIndIm, single(imageToSegment)/255);
distance = distanceMex(colourHist, colourHist, 1)';%1-HID£¬2-Euclidean distance

%% Propose the initial windows
%%%%%coarseWindow = [x, y, width, height];
[coarseWindow, BINGscore] =  BINGWindowGen(im, cfg.BING);

[initalWindow, ratio] = proposeInitialWindow(coarseWindow, superPixelBoxes, cfg.eta, col, row); 

%% adobe boxes
[adobeBoxes, adobeCompactness] = AdobeBoxesMex(initalWindow, superPixelBoxes, blobIndIm, colourHist, blobSizes, ratio, distance);  %%AdobeBoxes     
adobeCompactness = adobeCompactness / max(adobeCompactness);

adobeBoxes = [adobeBoxes, adobeCompactness'.* BINGscore];
adobeBoxes = BoxRemoveDuplicates(adobeBoxes);
adobeBoxes = -sortrows(-adobeBoxes, 5);

%% Non maximum suppression £¨NMS£©
adobeBoxes = boxesNmsMex(adobeBoxes, 0.6);

%%for visualization result
[Dr,BestLocation] = evaluatePerImgRecallforImg(adobeBoxes(:,1:4), gtBox, 100);
imshow(im);hold on;
for j = 1:size(BestLocation, 1)
    rectangle('Position',BestLocation(j,2:5),'edgecolor','g','LineWidth',2);
    rectangle('Position',gtBox(j,1:4),'edgecolor','r','LineWidth',2);  % ground truth
end
hold off ;   