function objPotentialBoxesSI = predictBBoxSI( im3u, base, W, NSS, numDetPerSz, T, filter1W, szActive)

minT = T.minT;
numT = T.numT;

NUM_sz = length(szActive);
[imH, imW, ~] = size(im3u);

objPotentialBoxesSI = cell(1, NUM_sz);

for i = NUM_sz:-1:1
    r = szActive(i);
    height = round(base ^ (floor(r / numT) + minT));
    width = round(base ^ (rem(r, numT) + minT));
    if height > imH * base || width > imW * base, continue; end
    height = min(height, imH);
    width = min(width, imW);

    subIm3u = imresize(im3u, [round(W * imH / height), round(W * imW / width)], 'nearest'); % resize image

    mag1f = computeNormedGradient(subIm3u, 3);
    
    matchCost1f = reshape(dotMex(mag1f, filter1W), size(mag1f, 1) - W + 1, size(mag1f, 2) - W + 1);
   
    ColorboxesRecLayerRate = 1;
    match = nonMaxSupMex(matchCost1f, NSS, numDetPerSz, ColorboxesRecLayerRate, height/W , width/W);
    matchCost.x = match(3, :)';
    matchCost.y = match(2, :)';
    matchCost.socre = match(1,:)';     

    % find true locations and match values
    ratioX = floor(width / W);
    ratioY = floor(height / W);
    box1 = round((matchCost.x - 1) * ratioX);
    box2 = round((matchCost.y - 1) * ratioY);
    box3 = min(box1 + width, imW);
    box4 = min(box2 + height, imH);
    box1 = box1 + 1; box2 = box2 + 1;
    boxW = box3 - box1 + 1;
    boxH = box4 - box2 + 1;
    box = [matchCost.socre, box1, box2, boxW, boxH];
    objPotentialBoxesSI{i} = box;
end

end
