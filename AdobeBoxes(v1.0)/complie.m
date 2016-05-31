curFloder = cd('.\mex');
mexFile = dir('*.cpp');
for i = 1 : length(mexFile)
   mex(mexFile(i).name);
end
cd(curFloder);