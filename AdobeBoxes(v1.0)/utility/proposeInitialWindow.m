function [initalWindow, ratio] = proposeInitialWindow(coarseWindow, superPixelBoxes, eta, col, row)
    coarseWindowAmp(:, 1) = max(floor(coarseWindow(:, 1) - eta * coarseWindow(:, 3)), 1);
    coarseWindowAmp(:, 2) = max(floor(coarseWindow(:, 2) - eta * coarseWindow(:, 4)), 1);
    coarseWindowAmp(:, 3) = min(ceil(coarseWindow(:, 1) + (1+eta) * coarseWindow(:, 3)), col) - coarseWindowAmp(:, 1) + 1;
    coarseWindowAmp(:, 4) = min(ceil(coarseWindow(:, 2) + (1+eta) * coarseWindow(:, 4)), row) - coarseWindowAmp(:, 2) + 1;

    ratio = interUnionSuperPixelMex(superPixelBoxes, coarseWindowAmp);%���㳬���ر����Χ�����,����0��9�ļ�Ϊ1������Ϊ0
    
    initalWindow = adjustBoxesMex(coarseWindowAmp, superPixelBoxes, ratio, row, col);
end