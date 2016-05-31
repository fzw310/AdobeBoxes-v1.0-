function filter1f = loadTrainedModel( W , svmModelI )
%LOADTRAINEDMODEL load svm model trained by libLinear
%

filter1f = svmModelI.w(1:end-1);
filter1f = reshape(filter1f, W, W);
% imtool(filter1f, []);

end

