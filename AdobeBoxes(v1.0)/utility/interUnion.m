function ratio = interUnion(bb, gtbb)


gtbbArea = gtbb(:, 3) .* gtbb(:, 4);

bbArea = bb(:, 3) .* bb(:, 4);
bbArea = repmat(bbArea, 1, size(gtbb, 1))';

intArea = rectint(gtbb, bb);
unionArea = bsxfun(@plus, bbArea, gtbbArea) - intArea;

ratio = bsxfun(@rdivide, intArea, unionArea);
