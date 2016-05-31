function [recall,avgScore] = evaluatePerImgRecall(objPotentialBoxes, gtTestBoxes, NUM_WIN, NUM_test)

recall = [];
avgScore = [];
noBlockImg = 0;
for i = 1:NUM_test
    recallOne = zeros(NUM_WIN, 1);

    gtTestBox = gtTestBoxes{i};
    if size(gtTestBox,1) == 0
        noBlockImg = noBlockImg + 1;
        continue;
    end
    objPotentialBox = objPotentialBoxes{i};
    
    ratio = interUnion(objPotentialBox, gtTestBox);
    
    % find the first detected index that grater than 0.5
    [~, idx_vec] = max(ratio >= 0.5, [], 2);
    idx_vec(ratio(sub2ind(size(ratio), 1:length(idx_vec), idx_vec')) < 0.5) = [];
    idx_vec = sort(idx_vec);
    abo = sum(max(ratio,[],2))/size(ratio, 1);

    for j = length(idx_vec):-1:1
        recallOne(idx_vec(j):end) = recallOne(idx_vec(j):end) + 1;
    end
    
    recallOne = recallOne / size(gtTestBox, 1);
    recall = cat(2, recall, recallOne);
    avgScore = cat(1, avgScore, abo);
end
recall = sum(recall, 2) / (NUM_test-noBlockImg);
avgScore = sum(avgScore)/(NUM_test-noBlockImg);
idx = [1,10,100,200, 300,400,500,600,700,800,900,1000,2000,3000,5000];
for j = 1:length(idx) 
    fprintf('Recall at %d : %f\n', idx(j), recall(idx(j)) * 100);
end
fprintf('avgScore is = %f\n', avgScore * 100);
end