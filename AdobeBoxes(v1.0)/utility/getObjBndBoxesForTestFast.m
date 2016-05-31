function [recall, objPotentialBoxes] = getObjBndBoxesForTestFast( cfg )

NUM_test = length(cfg.testStruct);
gtTestBoxes = cell(1, NUM_test); 
objPotentialBoxes = cell(1, NUM_test);

tStart = tic;
parfor i = 1:NUM_test
    
    [im, gtBoxes] = initalData(cfg.testStruct(i), cfg.datasetPath, cfg.datasetLabel); 
    [row,col,~] = size(im);       
    if row>=1024 || col>=1024, continue; end  % Remove 968 images from ILSVRC test set. The big images can be resized for application.
    if size(gtBoxes,1) == 0,continue;end % Remove the images whose GTs are all denoted as 'difficult'
    gtTestBoxes{i} = gtBoxes;     
    % objectness test main program
    if mod(i, 500) == 0, fprintf('current processed %d images\n', i);end
    %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
    superPixelBoxes = [];
    coarseWindow = []; 
    %% Efficient segmentation
    imageToSegment = cvtColor(im, 'RGB2HSV'); % Our paper uses this code compiled from openCV.If this code can not work, please use the next one.
%    [~,imageToSegment] = Image2ColourSpace(im, 'Hsv');
    [blobIndIm, blobBoxes, neighbours] = mexFelzenSegmentIndex(imageToSegment, cfg.sigma, cfg.k, cfg.minSize);
    superPixelBoxes(:, 1) = blobBoxes(:, 2);
    superPixelBoxes(:, 2) = blobBoxes(:, 1);
    superPixelBoxes(:, 4) = blobBoxes(:, 3) - blobBoxes(:, 1) + 1;
    superPixelBoxes(:, 3) = blobBoxes(:, 4) - blobBoxes(:, 2) + 1;
    
    [colourHist,blobSizes] = BlobStructColourHist(blobIndIm, single(imageToSegment)/255);
    distance = distanceMex(colourHist, colourHist, 1)';%histogram intersection distance
    
    %% Propose the initial windows
    %%%%%coarseWindow = [x, y, width, height];
    switch cfg.mode
        case 'random'
            coarseWindow = randomWindowGen(neighbours, blobBoxes, superPixelBoxes, im, cfg.WindowNum);  % randomly sampled windows
        case 'refineBING'
             [coarseWindow, BINGscore] =  BINGWindowGen(im, cfg.BING);% refine the proposals of BING 
    end
    
    [initalWindow, ratio] = proposeInitialWindow(coarseWindow, superPixelBoxes, cfg.eta, col, row); 

    %% Area-weight for final score (Equ. 11)
    AreaScore = log2(initalWindow(:, 3) .* initalWindow(:, 4));
    AreaScore = AreaScore/max(AreaScore); AreaScore = 1 ./ (1 - AreaScore);
   
    %% adobe boxes
    adobeBoxes = [];
    [adobeBoxes, adobeCompactness] = AdobeBoxesMex(initalWindow, superPixelBoxes, blobIndIm, colourHist, blobSizes, ratio, distance);       
    adobeCompactness = adobeCompactness / max(adobeCompactness);%(Equ. 10)
    
    switch cfg.mode
        case 'random'
            adobeBoxes = [adobeBoxes, adobeCompactness'.* AreaScore];%% randomly sampled windows
        case 'refineBING'
            adobeBoxes = [adobeBoxes, adobeCompactness'.* BINGscore];%% refine the proposals of BING 
    end
    adobeBoxes = BoxRemoveDuplicates(adobeBoxes);
    adobeBoxes = -sortrows(-adobeBoxes, 5);
    adobeBoxes = boxesNmsMex(adobeBoxes, cfg.nmsthreshold);    
    objPotentialBoxes{i} = adobeBoxes(:,1:4);
end
tValue = toc(tStart);
noGTImg = 0; for i = 1 : NUM_test; if size(gtTestBoxes, 1) == 0; noGTImg = noGTImg + 1; end; end
fprintf('Average time for predicting an image is %f seconds.\n', tValue / (NUM_test-noGTImg));
%--------------------------------------------------------------------------
% output performace index - recall
[recall,~] = evaluatePerImgRecall(objPotentialBoxes, gtTestBoxes, 10000 , NUM_test);
end

