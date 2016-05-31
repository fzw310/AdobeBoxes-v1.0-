function [boxesOut uniqueIdx] = BoxRemoveDuplicates(boxesIn)
[dummy uniqueIdx] = unique(boxesIn, 'rows', 'first');
uniqueIdx = sort(uniqueIdx);
boxesOut = boxesIn(uniqueIdx,:);
