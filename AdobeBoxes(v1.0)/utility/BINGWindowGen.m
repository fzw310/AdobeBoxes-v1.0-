function [window, score] = BINGWindowGen(im, BING)
   %Matlab version
   BINGproposals = getObjBndBoxes(im, BING);
   window = BINGproposals(:, 2:5);
   score = BINGproposals(:, 1);
   score = score - min(score);
end