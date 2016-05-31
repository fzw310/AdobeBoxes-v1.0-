function [initalWindow, ratio] = proposeInitialWindow(coarseWindow, superPixelBoxes, eta, col, row)
    coarseWindowAmp(:, 1) = max(floor(coarseWindow(:, 1) - eta * coarseWindow(:, 3)), 1);
    coarseWindowAmp(:, 2) = max(floor(coarseWindow(:, 2) - eta * coarseWindow(:, 4)), 1);
    coarseWindowAmp(:, 3) = min(ceil(coarseWindow(:, 1) + (1+eta) * coarseWindow(:, 3)), col) - coarseWindowAmp(:, 1) + 1;
    coarseWindowAmp(:, 4) = min(ceil(coarseWindow(:, 2) + (1+eta) * coarseWindow(:, 4)), row) - coarseWindowAmp(:, 2) + 1;

    ratio = interUnionSuperPixelMex(superPixelBoxes, coarseWindowAmp);%计算超像素被框包围的情况,大于0，9的计为1，其他为0
    
    initalWindow = adjustBoxesMex(coarseWindowAmp, superPixelBoxes, ratio, row, col);
end