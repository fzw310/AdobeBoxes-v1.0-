function [ cfg ] = default_parameters(datasetLabel)
%DEFAULT_PARAMETERS_DAT Default parametrization
    %% segmentation
    cfg.sigma = 1;
    cfg.minSize = 128;
    cfg.k = cfg.minSize;
    %% amplification
    cfg.eta = 1/4;
    %% ramdonly sampled windows
    cfg.WindowNum = 2000;
    %% threshold for nms
    cfg.nmsthreshold = 0.6;

    %% parameter setting for BING
    cfg.BING.base = 2;  cfg.BING.W = 8;  cfg.BING.NSS = 2;  cfg.BING.numDetPerSz = 130;
    cfg.BING.T.logBase = log(cfg.BING.base); cfg.BING.T.minT = ceil(log(10) / cfg.BING.T.logBase); 
    cfg.BING.T.maxT = ceil(log(500) / cfg.BING.T.logBase); cfg.BING.T.numT = cfg.BING.T.maxT - cfg.BING.T.minT + 1;    
    
    %% set for datasets
    if nargin == 1
    cfg.mode = 'refineBING';% 'random' -- randomly sampled windows ;   'refineBING' --  refine the proposals of BING    
    cfg.datasetLabel = datasetLabel;
    curFloder = cd('.\mat');
        switch datasetLabel
            case 1%VOC2007
                cfg.datasetPath = 'F:\公共数据库\pascal 2007\VOC2007\JPEGImages';%image folder 
                load annoStruct2007Test; cfg.testStruct = annoStruct(IDX_testObj);                               
                %for BING
                load paraBING2007
                cfg.BING.sizeActive = szActive; 
                cfg.BING.svmModel2 = svmModelII;
                cfg.BING.filter1W = loadTrainedModel(cfg.BING.W, svmModelI);
            case 2%VOC2010
                cfg.datasetPath = 'F:\公共数据库\pascal 2010\VOC2010\JPEGImages';%image folder
                load annoStruct2010Test; cfg.testStruct = annoStruct(IDX_testObj);  
                %for BING
                load paraBING2010
                cfg.BING.sizeActive = szActive; 
                cfg.BING.svmModel2 = svmModelII;
                cfg.BING.filter1W = loadTrainedModel(cfg.BING.W, svmModelI);
            case 3%VOC2012
                cfg.datasetPath = 'F:\公共数据库\pascal 2012\VOC2012\JPEGImages';%image folder
                load annoStruct2012Test; cfg.testStruct = annoStruct(IDX_testObj);
                %for BING
                load paraBING2012
                cfg.BING.sizeActive = szActive; 
                cfg.BING.svmModel2 = svmModelII;
                cfg.BING.filter1W = loadTrainedModel(cfg.BING.W, svmModelI);
            case 4%ImageNet
                cfg.datasetPath = 'F:\公共数据库\ImageNet\ILSVRC2012_img_val';%image folder
                load annoStructImageNetValTest;  cfg.testStruct = annoStruct(IDX_testObj);   
                %for BING
                load paraBINGImageNet
                cfg.BING.sizeActive = szActive; 
                cfg.BING.svmModel2 = svmModelII;
                cfg.BING.filter1W = loadTrainedModel(cfg.BING.W, svmModelI);
        end
    cd(curFloder);
    end
end

