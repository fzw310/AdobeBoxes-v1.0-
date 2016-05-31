function [im, gtBoxes] = initalData(testStruct, datasetPath, datasetLabel)
    fileName = testStruct.annotation.filename;
    im = imreadUser(fileName, datasetPath, datasetLabel);% load image
    object = testStruct.annotation.object;
    objBoxes = [];%%obtain GroundTruth boxes
    for j = 1:length(object)
        if str2double(object(j).difficult), continue; end
        box = object(j).bndbox;
        xmin = str2double(box.xmin);
        xmax = str2double(box.xmax);
        ymin = str2double(box.ymin);
        ymax = str2double(box.ymax);
        objBox = [xmin, ymin, xmax - xmin + 1, ymax - ymin + 1]; % [x, y, width, length]
        objBoxes = cat(1, objBoxes, objBox);
    end
    gtBoxes = objBoxes;
end