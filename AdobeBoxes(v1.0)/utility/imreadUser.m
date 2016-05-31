function im = imreadUser(fileName, datasetPath, datasetLabel)% load image
    switch datasetLabel       
        case 4, im = imread(strcat(fullfile(datasetPath, fileName), '.jpeg'));%ImageNet
        otherwise
             im = imread(fullfile(datasetPath, fileName)); 
    end 
    [~,~,n] = size(im);
    if n==1, im = repmat(im, 1, 1, 3);end    
end