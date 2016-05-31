function [recall, BestLocation] = evaluatePerImgRecallforImg(objPotentialBoxes, gtTestBoxes, NUM_WIN)
    recallOne = zeros(NUM_WIN, 1);

    gtTestBox = gtTestBoxes;
    objPotentialBox = objPotentialBoxes(1 : min(NUM_WIN, size(objPotentialBoxes, 1)),:);
    
    ratio = interUnion(objPotentialBox, gtTestBox);
    
    % find the first detected index that grater than 0.5
    [~, idx_vec] = max(ratio >= 0.5, [], 2);
    idx_vec(ratio(sub2ind(size(ratio), 1:length(idx_vec), idx_vec')) < 0.5) = [];
    idx_vec = sort(idx_vec);

    [~,MaxIdx] = max(ratio,[],2);
    ratioMx = [];
    for i = 1 : size(ratio, 1)
        ratioMx = [ratioMx; ratio(i, MaxIdx(i, 1))];
    end
    BestLocation = objPotentialBox(MaxIdx,:);
    BestLocation = [MaxIdx, BestLocation, ratioMx];
    for j = length(idx_vec):-1:1
        recallOne(idx_vec(j):end) = recallOne(idx_vec(j):end) + 1;
    end
    
    recall = recallOne / size(gtTestBox, 1);
end