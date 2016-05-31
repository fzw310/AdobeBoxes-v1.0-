function objPotentialBoxes = getObjBndBoxes( im3u, BING)

BoxesSI = predictBBoxSI(im3u, BING.base, BING.W, BING.NSS, BING.numDetPerSz, BING.T, BING.filter1W, BING.sizeActive);
BoxesSII = predictBBoxSII(BoxesSI, 1, BING.svmModel2);
objPotentialBoxes = [];
for i = 1:length(BoxesSII)
    if isempty(BoxesSII{i}), continue; end
    objPotentialBoxes = cat(1, objPotentialBoxes, BoxesSII{i});
end

objPotentialBoxes = -sortrows(-objPotentialBoxes, 1);

objPotentialBoxes = objPotentialBoxes(:, 1:5);

end

function BoxesSII = predictBBoxSII( BoxesSI , layer, svmModelII)


BoxesSII = cell(1, length(BoxesSI));
for r = 1:length(BoxesSI)
    rBoxes = BoxesSI{r};
    if isempty(rBoxes), continue; end
    rBoxes(:, 1) = rBoxes(:, 1) .* svmModelII(r+layer -1, 1) + svmModelII(r+layer -1, 2);
    BoxesSII{r} = rBoxes;
end

end


