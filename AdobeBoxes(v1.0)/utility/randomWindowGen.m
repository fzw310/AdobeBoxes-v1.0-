function randomWindow = randomWindowGen(neighbours, blobBoxes, superPixelBoxes, im, WindowNum)    
    [row,col,~] = size(im);
    for j = 1 : WindowNum
        x1 = 0; x2 = 0;y1 = 0; y2 = 0;
        while(abs(x1 - x2) < 16 || abs(y1 - y2) < 16)
            x1 = ceil((col-1)*rand(1));
            x2 = ceil((col-1)*rand(1));
            y1 = ceil((row-1)*rand(1));
            y2 = ceil((row-1)*rand(1));
        end
        Wr(j, 1) = min(x1, x2);
        Wr(j, 2) = min(y1, y2);
        Wr(j, 3) = max(x1, x2) - min(x1, x2) + 1;
        Wr(j, 4) = max(y1, y2) - min(y1, y2) + 1;
    end
    
    neighbours = tril(neighbours,-1);
    [nb,na] = find(neighbours == 1);
    neighboursBoxes = [];
    for j = 1:length(nb)
        x1 = min(blobBoxes(nb(j), 2), blobBoxes(na(j), 2));
        y1 = min(blobBoxes(nb(j), 1), blobBoxes(na(j), 1));
        x2 = max(blobBoxes(nb(j), 4), blobBoxes(na(j), 4));
        y2 = max(blobBoxes(nb(j), 3), blobBoxes(na(j), 3));   
        neighboursBoxes(j, :) = [x1, y1, x2-x1+1, y2-y1+1];
    end
    
    Ws = [neighboursBoxes; superPixelBoxes];   

    Wi = [1,1,col,row];
    
    randomWindow = [Wi; Wr; Ws];

end